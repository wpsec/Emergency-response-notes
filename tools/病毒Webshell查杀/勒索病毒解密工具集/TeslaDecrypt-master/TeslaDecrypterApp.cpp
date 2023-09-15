/*
 *  Copyright (C) 2016 Cisco Talos Security Intelligence and Research Group
 *
 *  Authors: Andrea Allievi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 * 
 *	Filename: TeslaDecrypterApp.cpp
 *	Implements the CTeslaDecrypterApp class code
 *	This class contains the main application code, Anti-TeslaCrypt routines,
 *  and Log initialization
 *	Last revision: 05/30/2016
 *
 */

#include "StdAfx.h"
#include "TeslaDecrypterApp.h"
#include "AlphaCrypt.h"
#include <Shlobj.h>
#include <TlHelp32.h>

CTeslaDecrypterApp::CTeslaDecrypterApp(void):
	g_strAppData(NULL),
	g_strLocalAppData(NULL),
	g_pLog(NULL),
	g_bConsoleAttached(FALSE),
	g_pTeslaDec(NULL)
{
	LPTSTR appDataStr = NULL;					// %APPDATA% full path
	LPTSTR localAppDataStr = NULL;				// %LOCALAPPDATA% full path 
	DWORD iRetVal = FALSE;
	InitializeLog();

	// Allocate the buffers
	appDataStr = new TCHAR[MAX_PATH];
	localAppDataStr = new TCHAR[MAX_PATH];

	// Retrieve the APPDATA and LOCALAPPDATA path
	iRetVal = SHGetSpecialFolderPath(NULL, appDataStr, CSIDL_APPDATA, 0);
	iRetVal = SHGetSpecialFolderPath(NULL, localAppDataStr, CSIDL_LOCAL_APPDATA, FALSE);

	if (g_strAppData) delete g_strAppData;
	g_strAppData = appDataStr;
	if (g_strLocalAppData) delete g_strLocalAppData;
	g_strLocalAppData = localAppDataStr;

}

CTeslaDecrypterApp::~CTeslaDecrypterApp(void)
{
	if (g_pLog) { g_pLog->Close(); delete g_pLog;}
	if (g_pTeslaDec) { delete g_pTeslaDec; g_pTeslaDec = NULL;	}
	if (g_strAppData) { delete g_strAppData; g_strAppData = NULL; }
	if (g_strLocalAppData) { delete g_strLocalAppData; g_strLocalAppData = NULL; }
}

// Initialize the global APP Log
bool CTeslaDecrypterApp::InitializeLog() {
	TCHAR logFile[MAX_PATH] = {0};			// My log file name
	if (g_pLog) return false;

	// Get my path and initialize log
	GetModuleFileName(GetModuleHandle(NULL), logFile, COUNTOF(logFile));
	LPTSTR dotPtr = wcsrchr(logFile, L'.');
	if (dotPtr) wcscpy_s(dotPtr, 5, L".log");

	g_pLog = new CLog(logFile);
	return true;
}

// Get the decrypter and allocate one if needed
CTeslaDecrypter * CTeslaDecrypterApp::GetDecrypter() {
	if (!g_pTeslaDec) 
		g_pTeslaDec = new CTeslaDecrypter(g_pLog);
	return g_pTeslaDec;
}

#pragma region Command line parsing and Standard entry points
// Show this application command line usage
void CTeslaDecrypterApp::ShowUsage() {
	LPTSTR cmdLineStr = L"Command line usage:\r\n"
		L"TeslaDecrypter [/key:<hex_key>] [/file:<encrypted_file>] [/dir:<encrypted_directory>] [/KeepOriginal]\r\n"
		L"\r\n"
		L"Where:\r\n"
		L"/help			- 	Show this help message\r\n"
		L"/aeskey		- 	Manually specify the AES master key for decryption (32 bytes)\r\n"
		L"/privkey		- 	Manually specify the EC victim's private key for decryption (32 bytes)\r\n"
		L"/keyfile		-	Specify the key file used to recover the master key\r\n"
		L"/forcekey 		-	Force the key to be imported even if it has not been verified\r\n"
		L"/process-msieve 	- 	Process an MSIEVE log file and recover the AES encryption key\r\n"
		L"/file 		- 	Decrypt an encrypted file\r\n"
		L"/dir 			-	Decrypt all files in the target directory and its subdirectories\r\n"
		L"/scanEntirePc		-	Decrypt all files on computer\r\n"
		L"/KeepOriginal		-	Keep original file(s) through the decryption process\r\n"
		L"/deleteTeslaCrypt	- 	Automatically kill and delete the TeslaCrypt dropper\r\n";

	if (g_bConsoleAttached) 
		wprintf(cmdLineStr);
	else
		MessageBox(NULL, cmdLineStr, APPTITLE, MB_ICONINFORMATION);
}

// Parse the command line
int CTeslaDecrypterApp::ParseCommandLine(int argc, TCHAR * argv[]) {
	LPTSTR strOrgFile = NULL;						// Original requested file
	LPTSTR strOrgDir = NULL;						// Original requested directoty
	LPTSTR strKeyFile = NULL;						// Specific key file to use
	LPTSTR strAesKey = NULL;						// The specific per-session AES key
	LPTSTR strPrivKey = NULL;						// The specific global victim private key (Gpriv)
	LPTSTR strMsieveLog = NULL;						// The MSIEVE log to process
	DWORD dwStrLen = 0;								// String size in TCHARs
	BYTE aesKey[32] = {0};							// Specific session AES key 
	BYTE privKey[32] = {0};							// Specific global victim private key (Gpriv)
	bool bScanEntirePc = false;						// TRUE if I have to scan the entire Pc
	bool bDeleteDropper = false;					// TRUE if I have to automatically search and delete TeslaCrypt dropper
	BOOLEAN bMasterKeyStripped = FALSE;
	BOOL bRetVal = 0;

	// Get the main decrypter class instance
	CTeslaDecrypter * pDec = GetDecrypter();

	for (int i = 1; i < argc; i++) {
		LPTSTR arg = argv[i];
		LPTSTR param = NULL;

		param = wcschr(arg, L':');
		if (param) { param[0] = 0; param++; }

		// Check the arg starting chr
		if (arg[0] == L'/' || arg[0] == L'-')
			arg++;
		else
			return false;

		if (_wcsicmp(arg, L"help") == 0) {
			ShowUsage();
			return true;
		}

		else if (_wcsicmp(arg, L"keeporiginal") == 0) {
			pDec->KeepOriginalFiles(true);				// Another non-sense comment: The pen is on the table!!!
			// else
				// Use default strategy (found in SearchAndImportKeyFile);
		}		

		else if (_wcsicmp(arg, L"scanentirepc") == 0) 
			bScanEntirePc = true;

		else if (_wcsicmp(arg, L"deleteteslacrypt") == 0) 
			bDeleteDropper = true;

		else if (_wcsicmp(arg, L"forcekey") == 0) 
			pDec->ForceKey(true);
		

		else if (_wcsicmp(arg, L"aeskey") == 0) {
			if (strAesKey || strKeyFile) return FALSE;
			if (!param) return FALSE;
			strAesKey = Trim(param);
			dwStrLen = wcslen(strAesKey);
			if (dwStrLen != 64) {
				cl_wprintf(RED, L"Error! ");
				wprintf(L"The AES key should be 32-bytes long.");
				return TRUE;
			}
			LPBYTE lpAesKey = CTeslaDecrypter::HexToBytes(strAesKey, dwStrLen);
			if (lpAesKey) {
				RtlCopyMemory(aesKey, lpAesKey, COUNTOF(aesKey));
				delete lpAesKey;			// DO NOT forget to do this
			}
			else {
				cl_wprintf(RED, L"Error! ");
				wprintf(L"Parsing error! Bad HEX key specified. \r\n");
				return TRUE;
			}
		}

		else if (_wcsicmp(arg, L"privkey") == 0) {
			if (strPrivKey || strAesKey || strKeyFile) return FALSE;
			if (!param) return FALSE;
			strPrivKey = Trim(param);
			dwStrLen = wcslen(strPrivKey);
			if (dwStrLen != 64) {
				cl_wprintf(RED, L"Error! ");
				wprintf(L"The EC private key should be 32-bytes long.");
				return TRUE;
			}
			LPBYTE lpPrivKey = CTeslaDecrypter::HexToBytes(strPrivKey, dwStrLen);
			if (lpPrivKey) {
				RtlCopyMemory(privKey, lpPrivKey, COUNTOF(privKey));
				delete lpPrivKey;
			}
			else {
				cl_wprintf(RED, L"Error! ");
				wprintf(L"Parsing error! Bad HEX key specified. \r\n");
				return TRUE;
			}
		}

		else if (_wcsicmp(arg, L"keyfile") == 0) {
			if (strKeyFile || strAesKey) return FALSE;
			if (!param) return FALSE;
			strKeyFile = Trim(param, L'\"', L'\"');
			dwStrLen = wcslen(strKeyFile);
			if (!FileExists(strKeyFile)) {
				cl_wprintf(RED, L"Error! ");
				wprintf(L"Key file \"%s\" does not exist.", strKeyFile);
				return TRUE;
			}
		}

		else if (_wcsicmp(arg, L"file") == 0)  {
			if (strOrgFile || strOrgDir) return FALSE;
			if (!param) return FALSE;
			strOrgFile = Trim(param, L'\"', L'\"');
			if (!FileExists(strOrgFile)) {
				cl_wprintf(RED, L"Error! ");
				wprintf(L"File \"%s\" does not exist.\r\n", strOrgFile);
				return TRUE;
			}
		}

		else if (_wcsicmp(arg, L"dir") == 0) {
			if (strOrgDir || strOrgFile) return FALSE;
			if (!param) return FALSE;
			strOrgDir = Trim(param, L'\"', L'\"');
			if (!FileExists(strOrgDir)) {
				cl_wprintf(RED, L"Error! ");
				wprintf(L"Directory \"%s\" does not exist.\r\n", strOrgDir);
				return TRUE;
			}
		}

		else if (_wcsicmp(arg, L"process-msieve") == 0) {
			strMsieveLog = Trim(param, L'\"', L'\"');
			if (!FileExists(strMsieveLog)) {
				cl_wprintf(RED, L"Error! ");
				wprintf(L"Msieve log file \"%s\" does not exist.\r\n", strMsieveLog);
				return TRUE;
			}
		}
		else 
			// Unrecognized param
			return FALSE;
	}	

	// First thing to do is to process The MSIEVE log (if any)
	if (strMsieveLog) {
		if (!strOrgFile) {
			cl_wprintf(RED, L"Error!\r\n");
			wprintf(L"To properly analyze a log generated by MSIEVE you should specify\r\n"
				L"an encrypted TeslaCrypt file using the \"/file\" argument.\r\n");
			return TRUE;
		}
		ProcessMsieveLog(strMsieveLog, strOrgFile);
		return TRUE;
	}

	// Import the key file or AES key in the decrypter
	if (strPrivKey) pDec->ImportTeslaVictimPrivKey(privKey);
	if (strKeyFile) bRetVal = pDec->ReadKeyFile(strKeyFile, &bMasterKeyStripped);
	else if (strAesKey) bRetVal = pDec->SetMasterKey(aesKey);
	else {
		// Use the standard research algorithm
		LPTSTR impKey = SearchAndImportKeyFile();
		if (impKey) {delete impKey; bRetVal = TRUE; }
	}

	if (bDeleteDropper) 
		// Automatically scan, kill and delete TeslaCrypt dropper
		SearchAndKillTeslaProc(false, true, true);


	/************************************************************************************************
	 *								DECRYPTING arguments parsing:									*/
	if (bScanEntirePc) {
		// Decrypt all PC files
		pDec->DeleteTeslaCryptGarbage(true);
		bRetVal = GetDecrypter()->DecryptAllPcFiles();
	}
	else if (strOrgDir) {
		wprintf(L"Decrypting directory \"%s\"... ", strOrgDir);
		bRetVal = GetDecrypter()->DecryptDirectory(strOrgDir);
	}
	else if (strOrgFile) {
		LPTSTR targetFile = NULL;
		wprintf(L"Decrypting file \"%s\"... ", wcsrchr(strOrgFile, '\\') + 1);
		targetFile = ComposeDestFileName(strOrgFile);
		bRetVal = pDec->DecryptTeslaFile(strOrgFile, targetFile);
		if (targetFile) delete targetFile;			// Don't forget to do this
	}
	else {
		// Use standard No Command line Main
		bRetVal = NoCmdLineMain();
		// iRetVal shouldn't be 0 in this case
		return bRetVal;	
	}

	if (bRetVal == ERROR_SUCCESS) {
		cl_wprintf(GREEN, L"Success!\r\n");
		if (strOrgDir)
			wprintf(L"Encrypted files in the target directory have been decrypted.\r\n"
			L"See the log file for details.\r\n");
	} else {
		cl_wprintf(RED, L"Error!\r\n");
		if (strOrgDir && bRetVal != ERROR_BADKEY)
			wprintf(L"Errors while decrypting files.\r\n"
			L"See log file for details.\r\n");
	}

	// Try to launch the factorization if needed:
	if (bRetVal == ERROR_BADKEY && pDec->GetLastFileType() == ENC_FILE_TYPE_TESLACRYPT_2) {
		// Ask and launch the Factorization?
		Sleep(1400);
		bRetVal = (GetDecrypter()->LaunchFactorizationAlgo() == true);
		if (bRetVal) {
			// Restart the process
			wprintf(L"Restarting....\r\n");
			Sleep(800);
			system("cls");
			CVersionInfo appInfo;
			wprintf(L"Talos %s %s\r\n", appInfo.GetProductName(), appInfo.GetFileVersionString());
			return ParseCommandLine(argc, argv);
		}
	}
	/*************************************************************************************************/

	return (bRetVal == ERROR_SUCCESS ? 1 : -1);
}

// Normal application startup without any command line
int CTeslaDecrypterApp::NoCmdLineMain() {
	BOOL bRetVal = FALSE;
	DWORD dwStrLen = 0;
	LPTSTR keyDatPath = NULL;					// "key.dat" file standard location
	TCHAR answer[8] = {0};						// User answer
	TCHAR dirOrFileToDecrypt[MAX_PATH] = {0};	// The file or directory to decrypt
	DWORD dwActionId = 0;						// Action ID: 0 - Not set, 1 - Decrypt all PC files, 2 - Decrypt a directory or a file
	bool bDoBackup = false;						// TRUE if the user would like to do a backup
	DWORD dwFileAttr = 0;						// File attributes 

	if (!GetDecrypter()->IsMasterKeySet()) {
		// Search key.dat file
		keyDatPath = SearchAndImportKeyFile();
		if (keyDatPath) {
			delete keyDatPath;			// Don't forget to do this
			keyDatPath = NULL;
		}
	}

	// Search the TeslaCrypt process (if any)
	bRetVal = SearchAndKillTeslaProc(true);

	wprintf(L"Would you like to attempt to decrypt all files encrypted by Tesla/AlphaCrypt \r\non this computer, including mounted network shares? [Y/N] ");
	wscanf_s(L"%4s", answer, COUNTOF(answer));
	if (CHR_UPR(answer[0]) == 'Y') dwActionId = 1;

	if (!dwActionId) {
		rewind(stdin);
		wprintf(L"Enter directory or filename that you would like to decrypt:\r\n");
		wscanf_s(L"%[^\n]", dirOrFileToDecrypt, COUNTOF(dirOrFileToDecrypt));
		dwStrLen = wcslen(dirOrFileToDecrypt);
		if (dwStrLen > 1 &&	dirOrFileToDecrypt[dwStrLen-1] == '\\') 
			dirOrFileToDecrypt[--dwStrLen] = 0;

		if (dwStrLen < 1 || !FileExists(dirOrFileToDecrypt)) {
			cl_wprintf(RED, L"Error! ");
			wprintf(L"File or directory specified does not exist!\r\n");
			return -1;	
		}
		// Get specified file/dir attributes
		dwFileAttr = GetFileAttributes(dirOrFileToDecrypt);
		dwActionId = 2;
	}

	// Ask the user for the backup
	wprintf(L"Would you like to make a backup of the encrypted file(s)? [Y/N] ");
	rewind(stdin);
	wscanf_s(L"%4s", answer, COUNTOF(answer));
	if (CHR_UPR(answer[0]) == 'Y') bDoBackup = true;

	// Set the backup mode
	GetDecrypter()->KeepOriginalFiles(bDoBackup);

	while (TRUE) {
		switch (dwActionId) {
			case 1:
				// Decrypt all PC files
				GetDecrypter()->DeleteTeslaCryptGarbage(true);
				bRetVal = GetDecrypter()->DecryptAllPcFiles();
				break;

			case 2:
				// Decrypt a file or a directory
				if (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY) {
					// The location is a directory
					LPTSTR dirOnlyName = wcsrchr(dirOrFileToDecrypt, '\\');
					if (dirOnlyName) dirOnlyName++;
					else dirOnlyName = dirOrFileToDecrypt;
					wprintf(L"\r\nDecrypting \"%s\" directory... ", dirOnlyName);
					bRetVal = GetDecrypter()->DecryptDirectory(dirOrFileToDecrypt);
					if (bRetVal == ERROR_SUCCESS) {
						cl_wprintf(GREEN, L"Success!\r\n");
						wprintf(L"Encrypted files in the target directory have been decrypted.\r\n"
							L"See the log file for details.\r\n");
					} else {
						cl_wprintf(RED, L"Error!\r\n");
						if (bRetVal != ERROR_BADKEY)
							wprintf(L"Errors while decrypting the files.\r\n"
								L"See log file for details.\r\n");
					}
				} else {
					// The path specify a file
					// Compose the target path
					LPTSTR targetFile = ComposeDestFileName(dirOrFileToDecrypt);
					wprintf(L"\r\nDecrypting \"%s\" file... ", wcsrchr(dirOrFileToDecrypt, '\\') + 1);
					bRetVal = GetDecrypter()->DecryptTeslaFile(dirOrFileToDecrypt, targetFile);
					delete targetFile;			// Don't forget to do this
					if (bRetVal == ERROR_SUCCESS)
						cl_wprintf(GREEN, L"Success!\r\n");
					else
						cl_wprintf(RED, L"Error!\r\n");
				}
				break;

			default:
				// PASS the code shouldn't reach this point
				bRetVal = false;
				break;
		}

		// Try to launch the factorization if needed:
		if (bRetVal == ERROR_BADKEY && GetDecrypter()->GetLastFileType() == ENC_FILE_TYPE_TESLACRYPT_2) {
			// Ask and launch the Factorization?
			Sleep(1400);
			bRetVal = (GetDecrypter()->LaunchFactorizationAlgo() == true);
			if (bRetVal) {
				// Restart the process
				wprintf(L"Restarting....\r\n");
				Sleep(800);
				system("cls");
				CVersionInfo appInfo;
				wprintf(L"Talos %s %s\r\n", appInfo.GetProductName(), appInfo.GetFileVersionString());
				continue;
			}
		}
		break;
	}
	return (bRetVal ? 1 : -1);
}

// Main application entry point
int CTeslaDecrypterApp::Main(int argc, TCHAR * argv[]) {
	int iRetVal = 0;
	
	// For now use the console App
	CreateAndAttachConsole();
	CVersionInfo appInfo;
	wprintf(L"Talos %s %s\r\n", appInfo.GetProductName(), appInfo.GetFileVersionString());
	wprintf(L"http://blogs.cisco.com/security/talos/teslacrypt\r\n\r\n");
	wprintf(L"Authors: Andrea Allievi and Emmanuel Tacheau\r\n");
	wprintf(L"Copyright (C) 2016 Cisco Talos Security Intelligence and Research Group\r\n");
	wprintf(L"\r\n");

	if (argc > 1) {
		iRetVal = ParseCommandLine(argc, argv);
		if (!iRetVal) {
			cl_wprintf(RED, L"Error: ");
			wprintf(L"Invalid command line!\r\n");
			ShowUsage();
			iRetVal = -1;
		}
	} else 
		iRetVal = NoCmdLineMain();
	
	wprintf(L"\r\nPress any key to exit...");
	_getwch();

	return (iRetVal);
}
#pragma endregion

#pragma region Anti-TeslaCrypt dropper functions (maybe need to be inserted in a separate class?)
// Search if there is a suspicious TeslaCrypt process
DWORD CTeslaDecrypterApp::SearchForTeslaCryptProcess(LPTSTR lpFileFullPath, DWORD sizeInChars) {
	HANDLE hProcSnap = NULL;			// Current system processes snapshot handle
	BOOL bProcRetVal = FALSE,			// Process32xxx Returned value
		bRetVal = FALSE;				// Standard Win32 returned value
	DWORD nBytesIo = 0;					// Number of I/O bytes
	DWORD dwFoundProcId = 0;			// Found TeslaCrypt process ID
	PROCESSENTRY32 curProc = {0};		// Current process entry
	LPBYTE lpMemBuff = NULL;

	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcSnap == INVALID_HANDLE_VALUE ) return 0;
	if (!g_strAppData) return 0;

	curProc.dwSize = sizeof(PROCESSENTRY32);
	bProcRetVal = Process32First(hProcSnap, &curProc);

	while (bProcRetVal) {
		DWORD dwCurProcId = curProc.th32ProcessID;
		HANDLE hModuleSnap = NULL;
		MODULEENTRY32 modEntry = {sizeof(MODULEENTRY32)};

		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwCurProcId);
		
		// Go to next process
		bProcRetVal = Process32Next(hProcSnap, &curProc);

		if (hModuleSnap == INVALID_HANDLE_VALUE)
			continue;
		
		if (!Module32First( hModuleSnap, &modEntry )) {
			CloseHandle( hModuleSnap );
			continue;
		}
		
		CloseHandle( hModuleSnap );
		
		DWORD dwAppDataLen = wcslen(g_strAppData);
		DWORD dwLocalAppDataLen = wcslen(g_strLocalAppData);
		if ((_wcsnicmp(modEntry.szExePath, g_strAppData, dwAppDataLen) == 0 &&
			wcschr(&modEntry.szExePath[dwAppDataLen+1], L'\\') == NULL) ||
			(_wcsnicmp(modEntry.szExePath, g_strLocalAppData, dwLocalAppDataLen) == 0 &&
			wcschr(&modEntry.szExePath[dwAppDataLen+1], L'\\') == NULL)){

			// Open target process
			HANDLE hProc = OpenProcess(PROCESS_VM_READ | SYNCHRONIZE, FALSE, dwCurProcId);

			if (hProc) {
				lpMemBuff = (LPBYTE)VirtualAlloc(0, modEntry.modBaseSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
				// I don't care if the allocation goes wrong... I am too lazy
				// if (!lpFileBuff) .... blablabla boring stuff ...
				if (lpMemBuff)
					bRetVal = ReadProcessMemory(hProc, modEntry.modBaseAddr, lpMemBuff, modEntry.modBaseSize, &nBytesIo);
				else
					bRetVal = FALSE;

				// Close the process handle
				CloseHandle(hProc);
			}

			if (bRetVal && lpMemBuff) {
				LPSTR lpCryptStrPtr = (LPSTR)SearchUString(lpMemBuff, nBytesIo, L"CryptoLocker", true);
				if (!lpCryptStrPtr) 
					lpCryptStrPtr = (LPSTR)SearchUString(lpMemBuff, nBytesIo, L"HELP_TO_DECRYPT_YOUR_FILES", false);
				if (!lpCryptStrPtr) 
					lpCryptStrPtr = (LPSTR)SearchUString(lpMemBuff, nBytesIo, L"HELP_RESTORE_FILES", false);

				if (lpCryptStrPtr) {
					// Process found
					dwFoundProcId = dwCurProcId;

					g_pLog->WriteLine(L"Searching for TeslaCrypt process - Found TeslaCrypt process (ID: %i - Full Path: \"%s\")",
							(LPVOID)dwCurProcId, modEntry.szExePath);

					if (lpFileFullPath) {
						// Copy the process full path in the target buffer 
						DWORD maxSize = (sizeInChars > wcslen(modEntry.szExePath) ? 
							sizeInChars : wcslen(modEntry.szExePath));
						wcscpy_s(lpFileFullPath, maxSize, modEntry.szExePath);
					}
					break;
				}		
			}
		}
	}

	// Close the process snapshot
	CloseHandle(hProcSnap);
	if (lpMemBuff) VirtualFree(lpMemBuff, 0, MEM_RELEASE);

	if (!dwFoundProcId)
		g_pLog->WriteLine(L"SearchForTeslaCryptProcess - No active TeslaCrypt process found on this system!");

	return dwFoundProcId;
}

// Perform a classical search in a buffer
LPBYTE CTeslaDecrypterApp::SearchUString(LPBYTE buffer, DWORD buffSize, LPTSTR lpwStr, bool bCaseSensitive) {
	DWORD dwStrSize = wcslen(lpwStr);
	for (unsigned i = 0; i <= (buffSize - (dwStrSize * sizeof(TCHAR))); i++) {
		DWORD dwRet = 0;
		if (bCaseSensitive) 
			dwRet = wcsncmp((LPTSTR)(buffer + i), lpwStr, dwStrSize);
		else 
			dwRet = _wcsnicmp((LPTSTR)(buffer + i), lpwStr, dwStrSize);
		if (dwRet == 0) return (&buffer[i]);
	}
	return NULL;
}

// Search and kill the TeslaCrypt process
bool CTeslaDecrypterApp::SearchAndKillTeslaProc(bool bAskUser, bool bKill, bool bDelete) {
	TCHAR answer[6] = {0};				// User answer if I need to aske
	DWORD dwLastErr = 0;				// Last Win32 error
	BOOL bRetVal = FALSE;				// Returned value
	TCHAR teslaProcPath[MAX_PATH] = {0};			// The TeslaCrypt process full path

	// Get if there is a suspicious TeslaCrypt process running
	DWORD dwTeslaProcId = 0;					// Tesla process ID (if any)
	dwTeslaProcId = SearchForTeslaCryptProcess(teslaProcPath, MAX_PATH);

	if (dwTeslaProcId) {
		cl_wprintf(YELLOW, L"Warning! ");
		wprintf(L"Found TeslaCrypt process running on this system...\r\n");

		if (bAskUser) {
			wprintf(L"This process needs to be terminated before running the decryption.\r\n");
			wprintf(L"Would you like to terminate process #%i? [Y/N] ", dwTeslaProcId);
			
			rewind(stdin);
			wscanf_s(L"%4s", answer, COUNTOF(answer));
		} else 
			answer[0] = (bKill == true ? 'Y' : 'N');

		if (CHR_UPR(answer[0]) == 'Y') {
			HANDLE hProc =
				OpenProcess(PROCESS_TERMINATE, FALSE, dwTeslaProcId);
			if (hProc) {
				bRetVal = TerminateProcess(hProc, 0);
				if (bRetVal) Sleep(800);
				CloseHandle(hProc);
			}
			dwLastErr = GetLastError();
			if (bRetVal)
				wprintf(L"Process #%i Terminated.\r\n", dwTeslaProcId);
		}

		if (!bRetVal) {			// if target process was successfully killed
			cl_wprintf(RED, L"Error! ");
			wprintf(L"Unable to terminate TeslaCrypt process!\r\n"
				L"Bad things can happens....\r\n");
		} else {
			// Delete the found dropper (if needed)
			if (bAskUser) {
				wprintf(L"Would you like to delete the TeslaCrypt dropper? [Y/N] ");
				rewind(stdin);
				wscanf_s(L"%4s", answer, COUNTOF(answer));
			} else 
				answer[0] = (bDelete == true ? 'Y' : 'N');
			
			if (CHR_UPR(answer[0]) == 'Y') {
				// Delete the identified dropper file
				bRetVal = DeleteFile(teslaProcPath);
				if (bRetVal) {
					wprintf(L"TeslaCrypt dropper successfully deleted!\r\n");
					g_pLog->WriteLine(L"SearchAndKillTeslaProc - Successfully deleted \"%s\" TeslaCrypt dropper.",
						teslaProcPath);
				} else {
					wprintf(L"Unable to delete TeslaCrypt dropper.\r\n");
					g_pLog->WriteLine(L"SearchAndKillTeslaProc - Unable to delete \"%s\" file. Returned error: %i.",
						teslaProcPath, (LPVOID)GetLastError());
				}
			}
		} // END if target process was successfully killed
	}
	return (dwTeslaProcId != 0);
}
#pragma endregion

#pragma region "key.dat" functions and generic support functions
// Search the "key.dat" file in standard locations
LPTSTR CTeslaDecrypterApp::SearchAndImportKeyFile() {
	bool bMasterKeyObtained = false;			// TRUE if I have already parsed the master key
	LPTSTR keyDatPath = NULL;					// Path of the "key.dat" file
	DWORD dwStrLen = 0;							// String size in TCHARs
	BOOL bRetVal = FALSE;						// Returned value
	BOOLEAN bMasterKeyStripped = FALSE;			// TRUE if the master key is not inside the "key.dat" file
	LPTSTR keyFileName = L"key.dat";				// TeslaCrypt key file name

	// Allocate key.dat full path string
	keyDatPath = new TCHAR[MAX_PATH];
	RtlZeroMemory(keyDatPath, MAX_PATH * sizeof(TCHAR));

	// Compose the "key.dat" full path
	// Try to use the key.dat file located inside my path
	bRetVal = GetModuleFileName(NULL, keyDatPath, MAX_PATH);
	LPTSTR lpSlashPtr = wcsrchr(keyDatPath, L'\\');
	if (lpSlashPtr) lpSlashPtr[1] = 0;
	wcscat_s(keyDatPath, MAX_PATH, keyFileName);

	if (!FileExists(keyDatPath)) {
		// Try to search the "storage.bin" file full path
		keyFileName = L"storage.bin";
		keyDatPath[dwStrLen] = 0;
		wcscat_s(keyDatPath, MAX_PATH, keyFileName);
	}

	if (FileExists(keyDatPath)) {
		// Use this file as "key.dat"
		bRetVal = GetDecrypter()->ReadKeyFile(keyDatPath, &bMasterKeyStripped);
		if (bRetVal) { 
			bMasterKeyObtained = true;
			// Default keep files value
			GetDecrypter()->KeepOriginalFiles(true);
			wprintf(L"Master key obtained from \"%s\" file in current directory.\r\n", keyFileName);
		}
	} else {
		// Search the "key.dat" file inside its standard location
		keyFileName = L"key.dat";
		wcscpy_s(keyDatPath, MAX_PATH, g_strAppData);
		dwStrLen = wcslen(keyDatPath);
		if (keyDatPath[dwStrLen-1] != '\\') wcscat_s(keyDatPath, MAX_PATH, L"\\");
		wcscat_s(keyDatPath, MAX_PATH, keyFileName);

		if (!FileExists(keyDatPath)) {
			// Try to search the "storage.bin" inside its standard location
			keyFileName = L"storage.bin";
			wcscpy_s(keyDatPath, MAX_PATH, g_strLocalAppData);
			dwStrLen = wcslen(keyDatPath);
			if (keyDatPath[dwStrLen-1] != '\\') wcscat_s(keyDatPath, MAX_PATH, L"\\");
			wcscat_s(keyDatPath, MAX_PATH, keyFileName);
		}

		if (FileExists(keyDatPath))
			bRetVal = GetDecrypter()->ReadKeyFile(keyDatPath, &bMasterKeyStripped);
		else {
			bRetVal = false;
			wprintf(L"Warning! No TeslaCrypt/AlphaCrypt key file found in its original location.\r\n");
		}
		if (bRetVal) {
			bMasterKeyObtained = true;
			// Default don't keep files value
			GetDecrypter()->KeepOriginalFiles(false);
			wprintf(L"Master key obtained from \"%s\".\r\n", keyDatPath);
		}
	}

	if (bMasterKeyObtained)
		return keyDatPath;

	if (bMasterKeyStripped) {
		cl_wprintf(YELLOW, L"Warning! ");
		wprintf(L"The file \"%s\" doesn't include the master key.\r\n"
			L"It may have already been deleted by TeslaCrypt/AlphaCrypt.\r\n", keyFileName);
	}
	delete keyDatPath;
	return NULL;
}

// Compose destination decrypted file name
LPTSTR CTeslaDecrypterApp::ComposeDestFileName(LPTSTR strOrgFile) {
	DWORD dwStrLen = 0;
	LPTSTR targetFile = NULL;
	LPTSTR extPtr = TESLA_CRYPT_ALL_EXT_PATTERN;				// The extension ptr in TESLA_CRYPT_ALL_EXT_PATTERN
	bool bFileHasTeslaExt = false;
	if (!strOrgFile) return NULL;

	dwStrLen = wcslen(strOrgFile) + 20;
	targetFile = new TCHAR[dwStrLen];

	wcscpy_s(targetFile, dwStrLen, strOrgFile);
	LPTSTR dotPtr = wcsrchr(targetFile, L'.');

	while (dotPtr) {
		LPTSTR semicolonPtr = NULL;
		extPtr = wcschr(extPtr + 1, L'.');
		if (!extPtr) break;
		semicolonPtr = wcschr(extPtr, L';');
		if (!semicolonPtr) semicolonPtr = extPtr + wcslen(semicolonPtr);
		bFileHasTeslaExt = (_wcsnicmp(dotPtr, extPtr, (DWORD)(semicolonPtr - extPtr)) == 0);
		if (bFileHasTeslaExt) break;
	};

	if (bFileHasTeslaExt)
	{
		memset(dotPtr, 0, 4 * sizeof(TCHAR));
		dotPtr = wcsrchr(targetFile, L'.');
	} else {
		if (dotPtr) dotPtr[0] = 0;
		wcscat_s(targetFile, dwStrLen, L"_decrypted");
		if (dotPtr) {
			dotPtr = wcsrchr(strOrgFile, L'.');
			wcscat_s(targetFile, dwStrLen, dotPtr);
		}
	}

	return targetFile;
}
#pragma endregion

#pragma region Console Support Functions
// Create console screen buffer and set it to application
bool CTeslaDecrypterApp::SetConsoleBuffers() {
	FILE * fOut = NULL, *fIn = NULL, *fErr = NULL;
	freopen_s(&fOut, "CON", "w", stdout ) ;
	freopen_s(&fIn, "CON", "r", stdin ) ;
	freopen_s(&fErr, "CON", "w", stderr ) ;
	std::cin.clear();
	std::cout.clear();
	std::cerr.clear();
	std::ios::sync_with_stdio();

	rewind(stdout);
	rewind(stdin);
	return (fOut != NULL);
}

// Create application console and attach to executable
bool CTeslaDecrypterApp::CreateAndAttachConsole() {
	BOOL bConsoleOk = FALSE;
	bConsoleOk = AllocConsole();
	SetConsoleTitle(APPTITLE);
	if (bConsoleOk) {
		bConsoleOk = SetConsoleBuffers();
		g_bConsoleAttached = true;
	}
	return (bConsoleOk != FALSE);
}
#pragma endregion

#pragma region MSIEVE support functions
// Launch the MSIEVE log analysis and exit
bool CTeslaDecrypterApp::ProcessMsieveLog(LPTSTR lpLogFile, LPTSTR lpInfectedFile) {
	LPTSTR fileOnlyName = NULL;					// The file only name
	BYTE aes_key[32] = {0};						// The found AES key
	LPTSTR aes_key_hex = NULL;					// The AES key in HEX
	bool bRetVal = false;

	g_pLog->WriteLine(L"CTeslaDecrypterApp - Command arguments specified to parse a Msieve log file (\"%s\") using "
		L"\"%s\" encrypted file.", lpLogFile, lpInfectedFile);
	fileOnlyName = wcsrchr(lpLogFile, L'\\');
	if (fileOnlyName) fileOnlyName++;
	wprintf(L"Processing \"%s\" Msieve log file... ", fileOnlyName);

	bRetVal = GetDecrypter()->ProcessMsieveLog(lpLogFile, lpInfectedFile, aes_key);
	if (bRetVal) {
		cl_wprintf(GREEN, L"Success!\r\n");
		aes_key_hex = GetDecrypter()->BytesToHex(aes_key, 32);
		wprintf(L"\r\nThe Msieve log has been successfully processed.\r\n"
			L"The victim private key and the per-session AES decryption key have been\r\n"
			L"reconstructed. A file named \"key.dat\" has been created into the \r\n"
			L"application path.\r\n\r\n");
		if (aes_key_hex) {
			wprintf(L"Found AES key: 0x%s\r\n", aes_key_hex);
			delete[] aes_key_hex;
		}
		wprintf(L"You can now re-launch this application and decrypt all of your files\r\n"
			L"using the new key(s).\r\n");
	} else {
		cl_wprintf(RED, L"Error!\r\n");
		wprintf(L"Errors while parsing the log file.\r\n"
			L"See log file for details.\r\n");
	}

	return bRetVal;
}
#pragma endregion
