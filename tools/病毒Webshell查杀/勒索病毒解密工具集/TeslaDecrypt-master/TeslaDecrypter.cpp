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
 *	Filename: TeslaDecrypter.cpp
 *	Implements the CTeslaDecrypter class, contains code needed to decrypt
 *	all the TeslaCrypt encrypted files
 *	Last revision: 06/01/2016
 *
 */

#include "StdAfx.h"
#include "TeslaDecrypter.h"
#include <openssl\sha.h>
#include <openssl\aes.h>
//#include <openssl\rand.h>
#include <crtdbg.h>

CTeslaDecrypter::CTeslaDecrypter(CLog * pLog):
	g_pLog(pLog),
	g_bKeySet(false),
	g_bIsMyLog(false),
	g_pAlphaDecoder(NULL),
	g_lpLastEncFileName(NULL)
{
	// Set the initial decrypter flags:
	g_dwFlags = DELETEGARBAGE_FLAG | ANALYSENOEXTFILES_FLAG;
	RtlZeroMemory(g_masterKey, sizeof(g_masterKey));
	RtlZeroMemory(g_sha256mKey, sizeof(g_sha256mKey));           

	if (!pLog) {
		// Initialize an empty log 
		g_pLog = new CLog();
		g_bIsMyLog = true;
	} else {
		g_pLog = pLog;
		g_bIsMyLog = false;
	}
	// ALWAYS instantiate "g_pAlphaDecoder" in this version
	g_pAlphaDecoder = new CAlphaCrypt(g_pLog);
}

CTeslaDecrypter::~CTeslaDecrypter(void)
{
	if (g_bIsMyLog && g_pLog) {
		g_pLog->Close();
		delete g_pLog;
	}
	if (g_pAlphaDecoder) delete g_pAlphaDecoder;
	if (g_lpLastEncFileName) delete g_lpLastEncFileName;
}

// Read the "key.dat" file and obtain the Master Key
bool CTeslaDecrypter::ReadKeyFile(LPTSTR fileName, BOOLEAN * pbMasterKeyStripped) {
	BOOL bRetVal = FALSE;
	HANDLE hFile = NULL;
	DWORD dwLastErr = 0;			// Last Win32 error
	DWORD dwFileSize = 0;			// Key file size
	DWORD dwBytesIo = 0;			// Number of bytes read
	DWORD masterKeyOffset = 0x177;	// Master key Offset in the key.dat file
	DWORD yearOffset = 0x126;		// Year WORD offset in the key.dat file
	//DWORD paymentKeyOffset = 0x64;	// Payment key offset in the "storage.bin" file (we don't care about this)
	DWORD recKeyOffset = 0x00;		// Recovery key offset in the key file
	BYTE keyFileVersion = 0;		// The "key.dat" detected file version 
	BYTE zeroedBuff[32] = {0};		// A zeroed 32-bytes buffer
	LPBYTE lpBuff = NULL;
	BYTE masterKey[32] = {0};
	CHAR recKeyHex[0x82] = {0};	// The recovery key in hex

	hFile = CreateFile(fileName, FILE_GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	dwLastErr = GetLastError();

	if (hFile == INVALID_HANDLE_VALUE) 
		return false;

	dwFileSize = GetFileSize(hFile, NULL);
	if (dwFileSize > 0x2000) {
		return false;
	}

	// Allocate the memory for the file content
	lpBuff = (LPBYTE)new BYTE[dwFileSize];
	RtlZeroMemory(lpBuff, dwFileSize);

	// Read the entire file
	bRetVal = ReadFile(hFile, lpBuff, dwFileSize, &dwBytesIo, NULL);
	CloseHandle(hFile);			// We don't need it anymore

	// Compile the offset based on the "key.dat" file size and content:
	if (!bRetVal || dwFileSize < 0x1A0) {			// 0x1A0 is the aligned (masterKeyOffset + sizeof(SHA256))
		// Wrong file size, exit...
		if (lpBuff) delete lpBuff;
		return false;
	} 
	else if (dwFileSize == 0x280) {
		// First version of TeslaCrypt - the master key should be always located inside this key file
		yearOffset = 0x126;
		masterKeyOffset = 0x177;
		recKeyOffset = 0;
		keyFileVersion = 1;
	}
	else if (dwFileSize == 0x290) {
		// Second version of TeslaCrypt - the master key could be stripped
		yearOffset = 0x126;
		masterKeyOffset = 0x177;
		recKeyOffset = 0x84;
		keyFileVersion = 2;
	} 
	else if (dwFileSize >= 0x2F0) {
		// Third version of TeslaCrypt - All version of AlphaCrypt
		yearOffset = 0x18A;
		masterKeyOffset = 0x1DB;
		recKeyOffset = 0x84;

		// To get if this is TeslaCrypt version 3 or AlphaCrypt EXX variant analyse offset +0x1C0
		// Latest version of AlphaCrypt indeed fill the offset +0x148 and +0x19A with random OS data
		DWORD dwToCheck = *((DWORD*)(lpBuff+ 0x1C0));
		if (dwToCheck != 0) {
			// Latest version of Alphacrypt 
			keyFileVersion = 5;
			masterKeyOffset = 0x1B1;
		} else
			// Third and last version of TeslaCrypt OR first version of AlphaCrypt
			keyFileVersion = 4;
	} 

	// Verify its content
	SYSTEMTIME * pKeyTime = (PSYSTEMTIME)&lpBuff[yearOffset];
	if (pKeyTime->wYear < 2014 || pKeyTime->wYear > 2020) {
		g_pLog->WriteLine(L"ReadKeyFile - Invalid key file format for file \"%s\"", fileName);
		if (lpBuff) delete lpBuff;
		return false;
	}
	
	if (keyFileVersion <= 3) {
		if (memcmp(lpBuff + 0x278, "AaLl86", 6) == 0) {
			g_pLog->WriteLine(L"ReadKeyFile - Detected a key file generated by TALOS.");
			// Import the private key:
			if (memcmp(lpBuff + 0x137, zeroedBuff, 0x20) != 0) {
				g_pAlphaDecoder->ImportTeslaVictimPrivKey(lpBuff + 0x137);
			}
		}
		else
			g_pLog->WriteLine(L"ReadKeyFile - Detected a key file for TeslaCrypt version %i", (LPVOID)keyFileVersion);
	} else
		g_pLog->WriteLine(L"ReadKeyFile - Detected a key file for AlphaCrypt version %i", (LPVOID)(keyFileVersion - 3));


	// Get the master key
	RtlCopyMemory(masterKey, lpBuff + masterKeyOffset, 32);
	// Grab the recovery key (if any)
	if (recKeyOffset) {
		bool bIsHex = false;
		LPBYTE lpRecKeyPtr = lpBuff + recKeyOffset;
		
		bIsHex = CAlphaCrypt::IsBuffAnHexString(lpRecKeyPtr, 0x80);
		
		if (!bIsHex) {
			// Need to convert from binary
			lpRecKeyPtr = (LPBYTE)CAlphaCrypt::bin2hex(lpRecKeyPtr, 0x40);
			RtlCopyMemory(recKeyHex, lpRecKeyPtr, 0x80);
			delete lpRecKeyPtr;
		} else
			RtlCopyMemory(recKeyHex, lpRecKeyPtr, 0x80);
	}

	// Analyse the master key and get if it is empty
	if (memcmp(masterKey, zeroedBuff, sizeof(DWORD)) == 0) {
		g_pLog->WriteLine(L"ReadKeyFile - Warning! The master key in file \"%s\" is stripped down. "
			L"Unable to import master key.", fileName);
		if (pbMasterKeyStripped) *pbMasterKeyStripped = TRUE;
		bRetVal = FALSE;
	} else
		bRetVal = TRUE;

	// Calculate the inverse of the master key (if needed)
	if (keyFileVersion >= 5) {
		g_pLog->WriteLine(L"ReadKeyFile - Detected master key for AlphaCrypt v2, needs to be normalized...");
		bRetVal = g_pAlphaDecoder->GetTheInverse(masterKey, masterKey);
		if (!bRetVal) {
			g_pLog->WriteLine(L"ReadKeyFile - Calculation of the inverse key has failed. Unable to continue.");
			if (lpBuff) delete lpBuff;
			return false;
		}
	}

	// Verify here the master key
	if (keyFileVersion >= 2) {
		bRetVal = g_pAlphaDecoder->VerifyAlphaMasterKey(masterKey, recKeyHex);
		if (!bRetVal) {
			g_pLog->WriteLine(L"ReadKeyFile - Master key in file \"%s\" can't be verified. "
				L"This could cause to unexpected results.", fileName);
			
			if (!IS_DECRYPTER_FLAG_SET(FORCEKEY_FLAG)) {
				if (pbMasterKeyStripped) *pbMasterKeyStripped = TRUE;
				if (lpBuff) delete lpBuff;
				return false;
			} else {
				cl_wprintf(YELLOW, L"Warning! ");
				wprintf(L"The master key has not been verified. This could cause unexpected results!\r\n");
			}
		}
	}

	bRetVal = SetMasterKey(masterKey);
	if (bRetVal && pbMasterKeyStripped) *pbMasterKeyStripped = FALSE;
	
	//Cleanup here
	if (lpBuff) delete lpBuff;

	return (bRetVal != FALSE);
}

// Get master key (if it has been set)
bool CTeslaDecrypter::IsMasterKeySet(LPBYTE * lppKey, DWORD * lpdwKeySize) {
	if (!g_bKeySet) return false;
	if (lppKey) {
		LPBYTE buff = new BYTE[sizeof(g_masterKey)];
		RtlCopyMemory(buff, g_masterKey, sizeof(g_masterKey));
		*lppKey = buff;
		if (lpdwKeySize) *lpdwKeySize = sizeof(g_masterKey);
	}
	return true;
}

// Manually set the master key
bool CTeslaDecrypter::SetMasterKey(BYTE key[32]) {
	BYTE sha256[32] = {0};
	bool bRetVal = false;
	if (memcmp(key, g_masterKey, 32) == 0) return true;

	// Calculate the SHA256 of the key
	bRetVal = GetSha256(key, 32, sha256);			// BANG! Don't use COUNTOF(key) when an array is passed as argument
	if (bRetVal) {
		// Copy the output SHA256 and the key
		RtlCopyMemory(g_sha256mKey, sha256, sizeof(g_sha256mKey));
		RtlCopyMemory(g_masterKey, key, sizeof(g_masterKey));
		LPTSTR hexKey = BytesToHex(g_masterKey, sizeof(g_masterKey), NULL);
		g_pLog->WriteLine(L"SetMasterKey - Successfully imported master key \"%s\".", hexKey);
		delete hexKey;
		g_bKeySet = true;
	} else 
		g_pLog->WriteLine(L"SetMasterKey - Error! Unable to calculate the SHA256 of the master key!");

	return bRetVal;
}


/* Decrypt a TeslaLocker encryped file (returns the last Error)
 * Input parameters:
 *		orgFile - The original file FULL path
 *		destFile - The optional destination file name 
 * Return Value:
 *		The last Win32 error value; ERROR_SUCCESS if the operation was successful;
 *		ERROR_BADKEY if no Master key was available */
DWORD CTeslaDecrypter::DecryptTeslaFile(LPTSTR orgFile, LPTSTR destFile) {
	HANDLE hOrgFile = NULL,					// Handle to the original file ...
		hDestFile = NULL;					// ... and the handle of the target decrypted file
	BOOL bRetVal = FALSE;					// Win32 returned value
	DWORD dwNumBytesIo = 0;					// Number of bytes I/O
	DWORD dwFileSize = 0,					// Encrypted file size
		  dwChunkSize = 0,					// File chunk size in bytes
		  dwOrgFileSize = 0,				// Original file size
		  dwCurFilePos = 0,					// Current file offset 
		  dwLastErr = 0;					// Last Win32 Error
	BYTE fileHdr[0x200] = {0};				// Tesla/AlphaCrypt file header
	DWORD dwHdrSize = 0;					// The encrypted file header size
	DWORD dwIvOffset = NULL,				// AES IV initialization vector offset
		dwIvSize = 0x10;					// AES IV size
	LPBYTE lpChunkBuff = NULL,				// File chunk buffer
		lpDecBuff = NULL;					// The decrypted chunk buffer
	ENC_FILE_TYPE fileType =				// The type of the encrypted file
		ENC_FILE_TYPE_UNKNOWN;
	BYTE aes_key[32] = {0};					// Calculated AES key
	bool bKeyVerified = false;				// TRUE if the AES key has been verified for this file
	LPTSTR lpDestFile = NULL;				// The temporary or definitive destination file

	// Open the original file for read
	hOrgFile = CreateFile(orgFile, FILE_GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	dwLastErr = GetLastError();

	if (hOrgFile == INVALID_HANDLE_VALUE) {
		g_pLog->WriteLine(L"DecryptTeslaFile - Unable to open encrypted file \"%s\" for reading. Last Win32 error: %i.", orgFile, (LPVOID)dwLastErr);
		return dwLastErr;
	}

	// Read the file header
	bRetVal = ReadFile(hOrgFile, (LPVOID)fileHdr, sizeof(fileHdr), &dwHdrSize, NULL);
	dwLastErr = GetLastError();
	dwFileSize = GetFileSize(hOrgFile, NULL);

	// Verify the header
	if (!bRetVal || dwOrgFileSize > dwFileSize) {
		g_pLog->WriteLine(L"DecryptTeslaFile - Encrypted file format for \"%s\" is invalid. Maybe it is already decrypted or it's not a file encrypted by TeslaCrypt. (last Win32 error: %i).", orgFile, (LPVOID)dwLastErr);
		CloseHandle(hOrgFile);
		return ERROR_BAD_FORMAT;
	}

	// Analyse here the file header
	fileType = CAlphaCrypt::GetTeslaFileType(fileHdr, dwHdrSize, dwFileSize);
	g_lastEncFileType = fileType;
	if (fileType >= ENC_FILE_TYPE_TESLACRYPT_2) {
		// Set the last encrypted file
		DWORD dwStrSize = wcslen(orgFile);
		if (g_lpLastEncFileName) delete g_lpLastEncFileName;
		g_lpLastEncFileName = new TCHAR[dwStrSize + 2];
		wcscpy_s(g_lpLastEncFileName, dwStrSize + 2, orgFile);

		if (g_bKeySet) {
			/* Verify that the set AES key correspond to the file
			 * QUICK Explanation: this code allow me to proper manage different kinds of files.
			 * In this way It is not possible to decrypt a file wrongly.
			 * As usual AaLl86 Rulex... Working on this on Sunday :-(		*/
			bKeyVerified = g_pAlphaDecoder->VerifyTeslaCryptAesKey(fileHdr, dwHdrSize, dwFileSize, g_masterKey);
			if (!bKeyVerified) {
				g_pLog->WriteLine(L"DecryptTeslaFile - Detected wrong AES master key for file \"%s\". Resetting...", orgFile);
				RtlZeroMemory(g_masterKey, sizeof(g_masterKey));
				g_bKeySet = false;
			}
		}
	}

	switch (fileType) {
		case ENC_FILE_TYPE_TESLACRYPT_3_OR_4: 
			if (!g_bKeySet) {
				bRetVal = g_pAlphaDecoder->GetTeslaCrypt3AesKey(fileHdr, dwHdrSize, aes_key);
				if (bRetVal) {
					LPTSTR hexBytes = NULL;
					bRetVal = (memcmp(g_masterKey, aes_key, sizeof(g_masterKey)) == 0);
					if (!bRetVal) {
						hexBytes = BytesToHex(aes_key, sizeof(aes_key));
						RtlCopyMemory(g_masterKey, aes_key, sizeof(g_masterKey));			// The same as SetMasterKey(aes_key) but without log
						g_bKeySet = true;
						g_pLog->WriteLine(L"DecryptTeslaFile - Successfully calculated and imported AES key for file \"%s\": %s", orgFile, hexBytes);
					}
					if (hexBytes) delete[] hexBytes;
				} else {
					g_pLog->WriteLine(L"DecryptTeslaFile - Unable to find master AES key for TeslaCrypt version 3 or 4 file \"%s\" file.", orgFile);
					return ERROR_BADKEY;
				}
			}
			dwIvOffset = 0x15C;
			dwOrgFileSize = *((DWORD*)(fileHdr + 0x16C));
			break;

		case ENC_FILE_TYPE_TESLACRYPT_2:
			if (!g_bKeySet) {
				// The following call should return FALSE (unless we are in the future and we know TeslaCrypt 2 Master C2 private key) 
				bRetVal = g_pAlphaDecoder->GetTeslaCrypt2AesKey(fileHdr, dwHdrSize, aes_key);
				if (bRetVal) RtlCopyMemory(g_masterKey, aes_key, sizeof(g_masterKey));
				else 
					return ERROR_BADKEY;
			}
			dwIvOffset = 0x18A;
			dwOrgFileSize = *((DWORD*)(fileHdr + 0x19A));
			break;

		case ENC_FILE_TYPE_ALPHACRYPT:
			// Master AES key should be set, otherwise I have no clue
			dwIvOffset = 0xC8;
			dwOrgFileSize = *((DWORD*)(fileHdr + 0xD8));
			break;

		case ENC_FILE_TYPE_TESLACRYPT_1:
			// Same as above: Master AES key should be set, otherwise I have no clue
			dwIvOffset = 0x00;
			dwOrgFileSize = *((DWORD*)(fileHdr + 0x10));
			break;

		default:
			g_pLog->WriteLine(L"DecryptTeslaFile - Unable to identify the encrypted file \"%s\". Returned type: %i", orgFile, (LPVOID)fileType);
			return false;
	}

	// Get the real file header size:
	dwHdrSize = dwIvOffset + dwIvSize + sizeof(DWORD);
	if (!g_bKeySet && fileType <= ENC_FILE_TYPE_ALPHACRYPT) return ERROR_BADKEY;

	// Open the target decrypted filename for writing
	if (!destFile) {
		TCHAR lpOrgFilePath[MAX_PATH] = {0};
		LPTSTR lpSlashPtr = wcsrchr(orgFile, L'\\');

		wcsncpy_s(lpOrgFilePath, MAX_PATH, orgFile, lpSlashPtr - orgFile);
		lpDestFile = new TCHAR[MAX_PATH];
		GetTempFileName(lpOrgFilePath, L"dec", 0, lpDestFile);
	} else
		lpDestFile = destFile;

	hDestFile = CreateFile(lpDestFile, FILE_GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	dwLastErr = GetLastError();
	if (hDestFile == INVALID_HANDLE_VALUE) {	
		g_pLog->WriteLine(L"DecryptTeslaFile - Unable to create the target destination file \"%s\". Last error: %i", destFile, (LPVOID)dwLastErr);
		CloseHandle(hOrgFile);
		return dwLastErr;
	}

	// Allocate the memory and read the file chunk by chunk (16 MB per chunk)
	dwChunkSize = ((dwFileSize - dwHdrSize) > 0x1000000) ? 0x1000000 : (dwFileSize - dwHdrSize);		
	dwCurFilePos = dwHdrSize;

	// Move file pointer according to Header size
	bRetVal = SetFilePointer(hOrgFile, dwCurFilePos, NULL, FILE_BEGIN);

	// Allocate the right chunk buffer
	lpChunkBuff = (LPBYTE)VirtualAlloc(NULL, dwChunkSize, MEM_COMMIT, PAGE_READWRITE);
	if (!lpChunkBuff) {			// I am too lazy ... :-( ... but check the returned buffer 
		g_pLog->WriteLine(L"DecryptTeslaFile - Unable to read from encrypted file \"%s\". System does not have enough free resources.", orgFile);
		CloseHandle(hOrgFile);
		return ERROR_NO_SYSTEM_RESOURCES;
	}

	while (dwCurFilePos < dwFileSize) {
		// Calculate the exact number of bytes to read
		if (dwChunkSize > (dwFileSize - dwCurFilePos))
			dwChunkSize = dwFileSize - dwCurFilePos;

		// Read a chunk of data
		bRetVal = ReadFile(hOrgFile, lpChunkBuff, dwChunkSize, &dwNumBytesIo, NULL);
		dwLastErr = GetLastError();
		if (!bRetVal) {
			g_pLog->WriteLine(L"DecryptTeslaFile - Error, unable to read from file \"%s\". Returned error: %i.", orgFile, (LPVOID)dwLastErr);
			if (lpChunkBuff) VirtualFree(lpChunkBuff, 0, MEM_RELEASE);
			CloseHandle(hOrgFile); CloseHandle(hDestFile);
			return dwLastErr;
		}

		// Try to perform the decryption now
		if (fileType < ENC_FILE_TYPE_TESLACRYPT_2)
			// Decrypt using the SHA256 of the master key
			bRetVal = DecWithAes256(lpChunkBuff, dwChunkSize, fileHdr + dwIvOffset, &lpDecBuff, &dwNumBytesIo);
		else
			// Decrypt using the standard AES master key
			bRetVal = EncDecWithAes256(lpChunkBuff, dwChunkSize, fileHdr + dwIvOffset, g_masterKey,
				&lpDecBuff, &dwNumBytesIo);

		if (bRetVal) {
			// Write the decrypted chunk
			bRetVal = WriteFile(hDestFile, lpDecBuff, dwChunkSize, &dwNumBytesIo, NULL);
			dwLastErr = GetLastError();
		}
		if (!bRetVal) break;
		if (lpDecBuff) VirtualFree(lpDecBuff, 0, MEM_RELEASE);

		dwCurFilePos += dwChunkSize;
	}

	// Cleanup original buffer
	if (lpChunkBuff) VirtualFree(lpChunkBuff, 0, MEM_RELEASE);
	if (hOrgFile != INVALID_HANDLE_VALUE) CloseHandle(hOrgFile); 
	lpChunkBuff = NULL;

	if (bRetVal && (dwCurFilePos > dwOrgFileSize)) {
		bRetVal = SetFilePointer(hDestFile, dwOrgFileSize, NULL, FILE_BEGIN);
		SetEndOfFile(hDestFile);
	}
	if (hDestFile != INVALID_HANDLE_VALUE) CloseHandle(hDestFile);

	if (!bRetVal || !dwFileSize) {
		g_pLog->WriteLine(L"DecryptTeslaFile - Decryption error! Unable to decrypt file \"%s\".", orgFile);
		return ERROR_DECRYPTION_FAILED;
	}

	if (destFile) {
		// If I don't have to keep the original file delete it
		if (!IS_DECRYPTER_FLAG_SET(KEEPORGFILES_FLAG)) {
			BOOL dRetVal = DeleteFile(orgFile);
			if (dRetVal)
				g_pLog->WriteLine(L"DecryptTeslaFile - Encrypted file (\"%s\") deleted.", orgFile);
		} else {
			g_pLog->WriteLine(L"DecryptTeslaFile - A backup of the original encrypted file "
			L"was stored in \"%s\".", orgFile);
		}
	} else {
		// Here I have generated a temporary destination file.
		DeleteFile(orgFile);
		MoveFile(lpDestFile, orgFile);
	}

	if (bRetVal) {
		g_pLog->WriteLine(L"DecryptTeslaFile - Successfully decrypted file \"%s\".", lpDestFile);
		bRetVal = ERROR_SUCCESS;
	} else {		
		g_pLog->WriteLine(L"DecryptTeslaFile - Unable to write decrypted file \"%s\". Returned error: %i.", lpDestFile, (LPVOID)dwLastErr);
		bRetVal = dwLastErr;
	}
	if (lpDestFile != destFile) delete[] lpDestFile;
	return bRetVal;
}

// Check if a filename matches the pattern string
bool CTeslaDecrypter::CheckFileNameInPattern(LPTSTR fileName, LPTSTR pattern) {
	LPTSTR fPattern = NULL;				// Formatted pattern string
	LPTSTR lpExtPtr = NULL;				// File extension pointer
	LPTSTR lpCurPtr = NULL,				// Current pointer in the pattern string
		lpEndPtr = NULL;				// End pointer in the pattern string
	//DWORD dwFileNameLen = 0;			// File name string length
	DWORD dwPatternStrLen = 0;			// Pattern string length
	bool bExtFound = false;				// True if I have found an extension in the pattern

	if (!fileName) return false;
	if (!pattern) return true;

	dwPatternStrLen = wcslen(pattern);			// Remember the space for the NULL char
	fPattern = new TCHAR[dwPatternStrLen+6];
	RtlZeroMemory(fPattern, (dwPatternStrLen+6) * sizeof(TCHAR));
	wcscpy_s(fPattern, dwPatternStrLen+1, pattern);
	if (pattern[dwPatternStrLen-1] != L';') {
		fPattern[dwPatternStrLen++] = L';';
		fPattern[dwPatternStrLen] = 0;
	}
	
	// Get the file extension
	lpExtPtr = wcsrchr(fileName, L'.');
	if (!lpExtPtr) {
		delete fPattern;
		return false;
	} else
		lpExtPtr++;

	// Search if the file extension is inside the pattern
	lpCurPtr = fPattern;
	while ((lpEndPtr = wcschr(lpCurPtr, L';')) != NULL) {
		LPTSTR lpDotPtr = NULL;			// The dot pointer
		lpEndPtr[0] = 0;
		lpDotPtr = wcschr(lpCurPtr, L'.');
		if (lpDotPtr) lpDotPtr++;
		else lpDotPtr = lpCurPtr;

		// Here theoretically I need to implement even the body pattern match
		// But we don't care in this version of the decryptor
		// Too lazy man! :-)
		if (_wcsicmp(lpDotPtr, lpExtPtr) == 0) 
			bExtFound = true;
		else if (_wcsicmp(lpDotPtr, L"*") == 0 ||
			_wcsicmp(lpDotPtr, L"*.*") == 0)
			// Global search pattern
			bExtFound = true;

		lpEndPtr[0] = L';';
		lpCurPtr = lpEndPtr + 1;

		if (bExtFound) break;
	}
	
	// Cleanup
	delete fPattern;
	return bExtFound;
}

/* Decrypt an entire directory, looking for the TeslaCrypt files
 * Input parameters:
 *		dirName - The original file FULL path
 *		bRecursive - a "bool" value indicating if I need to search inside the sub-dirs
 *		bStripExt -	True if I have to strip the file extension from the target file
 *		bIsRecursiveCall - Internal flag used if the function is called from itself
 * Return Value:
 *		The last Win32 error value; ERROR_SUCCESS if the operation was successful;
 *		ERROR_BADKEY if no Master key was available */
DWORD CTeslaDecrypter::DecryptDirectory(LPTSTR dirName, bool bRecursive, bool bStripExt, bool bIsRecursiveCall) {
	HANDLE hSearch = NULL;					// Handle to the file search
	BOOL bRetVal = FALSE;					// Win32 returned value
	bool bSomeErrors = false,				// True if I have encountered some errors
		bAtLeastOneDecrypted = false,		// True if I have decrypted almost one file
		bFileHasTeslaExt = false;			// True if the intercepted file has the TeslaCrypt extension
	WIN32_FIND_DATA findData = {0};			// Win32 find data
	TCHAR fullSearchPattern[0x200] = {0};	// FULL search pattern
	DWORD dwStrLen = 0;						// String size in TCHARs
	if (!FileExists(dirName)) return false;

	if (!bIsRecursiveCall)
		g_pLog->WriteLine(L"DecryptDirectory - Processing directory \"%s\" (Recursive: %s, Strip file extensions: %s)...", dirName,
			(bRecursive ? L"True": L"False"), (bStripExt ? L"True": L"False"));

	// Create full search path
	wcscpy_s(fullSearchPattern, COUNTOF(fullSearchPattern), dirName);
	dwStrLen = wcslen(dirName);
	if (fullSearchPattern[dwStrLen-1] != '\\')  {
		wcscat_s(fullSearchPattern, COUNTOF(fullSearchPattern), L"\\");
		dwStrLen = wcslen(fullSearchPattern);
	}
	// Compose the FULL search path
	wcscat_s(fullSearchPattern, COUNTOF(fullSearchPattern), L"*.*");

	// Damn heck FindFirstFile API doesn't support multiple pattern
	hSearch = FindFirstFile(fullSearchPattern, &findData);
	bRetVal = (hSearch != INVALID_HANDLE_VALUE);
	// Trim down the original search pattern
	fullSearchPattern[dwStrLen] = 0;

	while (bRetVal) {
		// Compose the full file path
		TCHAR fileFullPath[MAX_PATH] = {0};				// Full original file path
		LPTSTR lpDestFileName = NULL;					// New file full path (if needed)
		wcscpy_s(fileFullPath, COUNTOF(fileFullPath), fullSearchPattern);
		wcscat_s(fileFullPath, COUNTOF(fileFullPath), findData.cFileName);
		
		if (_wcsicmp(findData.cFileName, L"..") == 0 ||
			_wcsicmp(findData.cFileName, L".") == 0) 
		{
			// Wrong filename, go to next file
			bRetVal = FindNextFile(hSearch, &findData);
			continue;
		}

		if (GetFileAttributes(fileFullPath) & FILE_ATTRIBUTE_DIRECTORY) {
			// This is a directory
			if (bRecursive == true) {
				bRetVal = DecryptDirectory(fileFullPath, true, bStripExt, true);
				if (bRetVal) bAtLeastOneDecrypted = true;
			} else {
				// Go next
				bRetVal = FindNextFile(hSearch, &findData);
				continue;
			}
		} 

		// Should I have to cleanup all the TeslaCrypt files?
		bool bIsGarbageFile = 
			(_wcsicmp(findData.cFileName, L"HELP_RESTORE_FILES.txt") == 0);
		bIsGarbageFile |= 
			(_wcsicmp(findData.cFileName, L"HELP_RESTORE_FILES.bmp") == 0);
		bIsGarbageFile |= 
			(_wcsnicmp(findData.cFileName,L"HELP_RESTORE_FILES_", 19) == 0);
		bIsGarbageFile |= 
			(_wcsicmp(findData.cFileName, L"HELP_TO_SAVE_FILES.txt") == 0);
		bIsGarbageFile |=
			(_wcsnicmp(findData.cFileName,L"help_recover_instructions+", 25) == 0);
			
		if (IS_DECRYPTER_FLAG_SET(DELETEGARBAGE_FLAG) && bIsGarbageFile) {
			bRetVal = DeleteFile(fileFullPath);
			if (bRetVal)
				g_pLog->WriteLine(L"DecryptDirectory - TeslaCrypt garbage file \"%s\" deleted.", fileFullPath);
			// Continue the cycle.... uuuhm ... Maybe it's going to continue in the next block???
			bRetVal = FindNextFile(hSearch, &findData);
			continue;
		}

		// Check here if the file name has the TeslaCrypt pattern
		bFileHasTeslaExt = CheckFileNameInPattern(fileFullPath, TESLA_CRYPT_ALL_EXT_PATTERN);

		if (!bFileHasTeslaExt) {
			bool bRecognized = false;
			if (IS_DECRYPTER_FLAG_SET(ANALYSENOEXTFILES_FLAG)) 
				// Check if I can recognize the file type
				bRecognized = (CAlphaCrypt::TryToGetFileType(fileFullPath) >= ENC_FILE_TYPE_TESLACRYPT_1); 
			if (!bRecognized) {
				// This file doesn't match, go next
				bRetVal = FindNextFile(hSearch, &findData);
				continue;
			}
			bFileHasTeslaExt = CheckFileNameInPattern(fileFullPath, L".mp3;");
		}

		// Strip the ".ecc" (".ezz", ".exx" or whatever) part if it exists
		LPTSTR extPtr = wcsrchr(fileFullPath, L'.');
		if ((bFileHasTeslaExt && bStripExt) || IS_DECRYPTER_FLAG_SET(KEEPORGFILES_FLAG)) {
			lpDestFileName = new TCHAR[MAX_PATH];
			if (extPtr)	wcsncpy_s(lpDestFileName, MAX_PATH, fileFullPath, extPtr - fileFullPath);
			else wcscpy_s(lpDestFileName, MAX_PATH, fileFullPath);
			if (!bFileHasTeslaExt) {
				wcscat_s(lpDestFileName, MAX_PATH, L"_decrypted");
				if (extPtr) wcscat_s(lpDestFileName, MAX_PATH, extPtr);
			}
		}

		bRetVal = DecryptTeslaFile(fileFullPath, lpDestFileName);
		if (bRetVal == ERROR_BADKEY && g_lastEncFileType == ENC_FILE_TYPE_TESLACRYPT_2) 
			break;			// Try to launch the factorization
		else if (bRetVal != ERROR_SUCCESS) bSomeErrors = true;
		else bAtLeastOneDecrypted = true;

		// Delete the new file name buffer 
		if (lpDestFileName) {
			delete lpDestFileName;
			lpDestFileName = NULL;
		}

		// Go to next file
		bRetVal = FindNextFile(hSearch, &findData);
	}
	FindClose(hSearch);

	if (bRetVal == ERROR_BADKEY && g_lastEncFileType == ENC_FILE_TYPE_TESLACRYPT_2) return ERROR_BADKEY;
	if (bAtLeastOneDecrypted) return ERROR_SUCCESS;
	if (!bSomeErrors) {
		//g_pLog->WriteLine(L"DecryptDirectory - Nothing to decrypt here (\"%s\").", dirName);
		return ERROR_SUCCESS;
	}
	return bRetVal;
}

// Launch TeslaCrypt 2.x Private key recovery algorithm
bool CTeslaDecrypter::LaunchFactorizationAlgo() {
	BOOL bRetVal = FALSE;					// Win32 APIs returned value
	BYTE aes_key[32] = {0};
	if (!g_lpLastEncFileName || g_lastEncFileType != ENC_FILE_TYPE_TESLACRYPT_2) return false;

	bRetVal = g_pAlphaDecoder->LaunchTeslaCrypt2RecoveryAlgorithm(g_lpLastEncFileName, aes_key);

	if (bRetVal) {
		SetMasterKey(aes_key);			// Are we really sure to do this?
		return true;
	} else
		return false;
}


/* Decrypt the entire Workstation
 * Return Value:
 *		The last Win32 error value; ERROR_SUCCESS if the operation was successful;
 *		ERROR_BADKEY if no Master key was available */ 
DWORD CTeslaDecrypter::DecryptAllPcFiles() {
	bool bAtLeastOneDriveOk = false,
		bSomeErrors = false;
	DWORD dwRetCode = 0;

	DWORD drivesMask = GetLogicalDrives();

	for (int i = 0; i < sizeof(DWORD)*8; i++) {
		if ((drivesMask & (1 << i)) == 0) continue;

		TCHAR drvName[10] = {0};
		wcscpy_s(drvName, COUNTOF(drvName), L"A:\\"); 
		drvName[0] = L'A' + (TCHAR)i;

		UINT drvType = GetDriveType(drvName);
		if (drvType == DRIVE_FIXED || drvType == DRIVE_REMOTE || 
			drvType == DRIVE_REMOVABLE) {
				TCHAR fsName[0x20] = {0};			// File System name (NTFS)
				TCHAR volumeName[0x100] = {0};		// Volume name
				DWORD volSn = 0,					// Volume serial number
					fsFlags = 0,					// File system flags	
					maxPathLen = 0;					// Maximum sizes of the FS paths

				dwRetCode = GetVolumeInformation(drvName, volumeName, COUNTOF(volumeName), &volSn, &maxPathLen,
					&fsFlags, fsName, COUNTOF(fsName));

				if (dwRetCode) {
					// Do the decryption of this volume
					wprintf(L"Working on \"%s\" drive... ", drvName);
					dwRetCode = DecryptDirectory(drvName, true);
					if (dwRetCode == ERROR_SUCCESS) {
						cl_wprintf(GREEN, L"Success!\r\n");
						bAtLeastOneDriveOk = true;
					} else if (g_lastEncFileType == ENC_FILE_TYPE_TESLACRYPT_2 && dwRetCode == ERROR_BADKEY) {
						// Interrupt here the process, and calculate the Victim Private key using the Factorization
						return ERROR_BADKEY;
					} else {
						cl_wprintf(RED, L"Errors!\r\n");
						bSomeErrors = true;
					}
				}
		}
		// Go to next drive
	}
	if (bAtLeastOneDriveOk || !bSomeErrors) dwRetCode = ERROR_SUCCESS;
	else dwRetCode = ERROR_GEN_FAILURE;
	return dwRetCode;
}

#pragma region AES-SHA256 functions
// Decrypt / encrypt with and AES CBC 256 algorithm
bool CTeslaDecrypter::DecWithAes256(LPBYTE lpBuff, DWORD dwBuffSize, BYTE iv[16], LPBYTE * lppOut, LPDWORD lpdwOutBuffSize) {
	BYTE aes_key[32] = {0};				// AES 256 Master key
	// Copy the current master key
	RtlCopyMemory(aes_key, g_sha256mKey, 0x20);
	return EncDecWithAes256(lpBuff, dwBuffSize, iv, aes_key, lppOut, lpdwOutBuffSize, false);
}

bool CTeslaDecrypter::EncDecWithAes256(LPBYTE lpBuff, DWORD dwBuffSize, BYTE iv[16], BYTE aes_key[32], LPBYTE * lppOut, LPDWORD lpdwOutBuffSize, bool bEncrypt) {
	const int key_length = 256;			// AES key size in bit
	LPBYTE lpOutBuff = NULL;			// Output buffer
	DWORD dwOutBuffSize = NULL;			// Output buffer aligned size
	BOOL bRetVal = FALSE;				// Returned value

	// Calculate the right output buffer size
	dwOutBuffSize = (dwBuffSize % AES_BLOCK_SIZE) + dwBuffSize;

	// Allocate the output buffer
	lpOutBuff = (LPBYTE)VirtualAlloc(NULL, dwOutBuffSize, MEM_COMMIT, PAGE_READWRITE);
	if (!lpOutBuff) return false;
	
	// so i can do with this aes-cbc-128 aes-cbc-192 aes-cbc-256
	if (bEncrypt) {
		// Do the encryption
		AES_KEY enc_key = {0};			// Encryption key
		bRetVal = (AES_set_encrypt_key(aes_key, key_length, &enc_key) == 0);
		AES_cbc_encrypt(lpBuff, lpOutBuff, dwOutBuffSize, &enc_key, (LPBYTE)iv, AES_ENCRYPT);
	} else {
		// Do the decryption
		AES_KEY dec_key = {0};				// Decryption key
	    bRetVal = (AES_set_decrypt_key(aes_key, key_length, &dec_key) == 0);
		AES_cbc_encrypt(lpBuff, lpOutBuff, dwOutBuffSize, &dec_key, (LPBYTE)iv, AES_DECRYPT);
	}
	
	if (bRetVal) {
		if (lppOut) *lppOut = lpOutBuff;
		else VirtualFree(lpOutBuff, 0, MEM_RELEASE);
		if (lpdwOutBuffSize) *lpdwOutBuffSize = dwOutBuffSize;
		return true;
	}
	VirtualFree(lpOutBuff, 0, MEM_RELEASE);
	return false;
}


// Calculate the SHA256 of a target buffer
bool CTeslaDecrypter::GetSha256(LPBYTE lpBuff, DWORD dwSize, BYTE sha256[SHA256_DIGEST_LENGTH]) {
    BOOL bRetVal = FALSE;
	unsigned char hash[SHA256_DIGEST_LENGTH];
    
	SHA256_CTX sha256ctx;
    bRetVal = SHA256_Init(&sha256ctx);
    bRetVal = SHA256_Update(&sha256ctx, lpBuff, (size_t)dwSize);
	if (bRetVal) {
		bRetVal = SHA256_Final(hash, &sha256ctx);
		RtlCopyMemory(sha256, hash, SHA256_DIGEST_LENGTH);
	}

	return bRetVal != FALSE;
}

// Transform a buffer in printable hex bytes
LPTSTR CTeslaDecrypter::BytesToHex(LPBYTE buff, DWORD buffSize, TCHAR delimChr) {
	LPTSTR outStr = NULL;							// Output string (allocated from the heap)
	DWORD strSize = buffSize * 2 + 4;				// Output string size in TCHARs
	DWORD curOffset = 0;							// Current output offset
	const LPTSTR hexMap = L"0123456789ABCDEF";		// Hex map

	if (delimChr) 
		strSize += buffSize;
	
	outStr = new TCHAR[strSize];
	RtlZeroMemory(outStr, strSize * sizeof(TCHAR));
	for (unsigned i = 0; i < buffSize; i++) {
		BYTE curByte = buff[i];
		TCHAR firstHex = hexMap[(curByte >> 4)];
		TCHAR secondHex = hexMap[curByte & 0xF];
		outStr[curOffset++] = firstHex;
		outStr[curOffset++] = secondHex;
		if (delimChr)
			outStr[curOffset++] = delimChr;
	}
	if (delimChr) outStr[--curOffset] = 0;

	// IMPORTANT! Remember to delete the output string after its usage
	return outStr;
}

// Transform a printable hex bytes in a real byte stream
LPBYTE CTeslaDecrypter::HexToBytes(LPTSTR hexString, DWORD strSize) {
	LPBYTE hexBuff = NULL;				// Hex buffer
	BYTE curByte = 0;					// Current processing byte
	DWORD dwCounter = 0;				// Counter for the Hex buffer

	if (strSize % 2) strSize++;
	hexBuff = new BYTE[strSize/2];
	if (!hexBuff) return NULL;
	RtlZeroMemory(hexBuff, strSize/2);
	
	for (unsigned i = 0; i < strSize; i++) {
		TCHAR curChr = hexString[i];

		if (!(curChr >= L'0' && curChr <= L'9'))  {
			curChr &= (~0x20);
			if (curChr < L'A' || curChr > L'F') {
				delete hexBuff; return NULL;
			}
		}

		if (curChr >= L'0' && curChr <= L'9') curChr = curChr - L'0';
		else curChr = (curChr - L'A') + 10;

		if ((i % 2) == 0) curByte = ((BYTE)curChr << 4);
		else {
			curByte |= (BYTE)curChr;
			hexBuff[dwCounter++] = curByte;
			curByte = 0;
		}
	}

	return hexBuff;
}

#pragma endregion
