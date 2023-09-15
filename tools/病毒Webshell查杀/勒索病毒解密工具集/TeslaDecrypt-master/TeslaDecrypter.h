/*
 *  Copyright (C) 2015 Cisco Talos Security Intelligence and Research Group
 *
 *  Authors: Andrea Allievi and Emmanuel Tacheau
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
 *	Filename: TeslaDecrypter.h
 *	Defines the CTeslaDecrypter class
 *	Last revision: 05/30/2016
 *
 */
#pragma once
#include "Log.h"
#include "resource.h"

// Include the new AlphaCrypt header
#include "AlphaCrypt.h"

// The TeslaCrypt supported files extension pattern:
#define TESLA_CRYPT_ALL_EXT_PATTERN L"*.ecc;*.ezz;*.exx;*.vvv;*.micro;"

// Flags helper macros
#define SET_DECRYPTER_FLAG(flag) g_dwFlags = (g_dwFlags | flag)
#define CLEAR_DECRYPTER_FLAG(flag) g_dwFlags = (g_dwFlags & (~flag))
#define IS_DECRYPTER_FLAG_SET(flag) ((g_dwFlags & flag) != 0)

// Tesla Decrypter class
class CTeslaDecrypter
{
public:
	enum DecrypterFlags {
		KEEPORGFILES_FLAG	 =	0x01,		// Set to keep the original encrypted files
		FORCEKEY_FLAG		 =	0x02,		// Set if I would like to force the key importing process
		DELETEGARBAGE_FLAG =	0x04,		// Set if I have to delete all the TeslaCrypt garbage files
		ANALYSENOEXTFILES_FLAG = 0x08		// Set to analyse even the files with no Tesla extension in the directory serach
	};

public:
	// Default constructor
	CTeslaDecrypter(CLog * pLog = NULL);

	// Default destructor
	~CTeslaDecrypter(void);

	// Set if to keep the original files or not
	void KeepOriginalFiles(bool bValue) { 
		(bValue ? SET_DECRYPTER_FLAG(KEEPORGFILES_FLAG) : CLEAR_DECRYPTER_FLAG(KEEPORGFILES_FLAG)); }
	// Set whether or not to delete the TeslaCrypt garbage files
	void DeleteTeslaCryptGarbage(bool bValue) { 
		(bValue ? SET_DECRYPTER_FLAG(DELETEGARBAGE_FLAG) : CLEAR_DECRYPTER_FLAG(DELETEGARBAGE_FLAG)); }
	// Set whether or not to force the key importing
	void ForceKey(bool bValue) { 
		(bValue ? SET_DECRYPTER_FLAG(FORCEKEY_FLAG) : CLEAR_DECRYPTER_FLAG(FORCEKEY_FLAG)); }

	// Read the "key.dat" file and obtain the Master Key
	bool ReadKeyFile(LPTSTR fileName, BOOLEAN * pbMasterKeyStripped = NULL);
	// Manually set the master key
	bool SetMasterKey(BYTE key[32]); 
	// Get master key (if it has been set)
	bool IsMasterKeySet(LPBYTE * lppKey = NULL, DWORD * lpdwKeySize = NULL);
	// Decrypt a TeslaLocker encryped file (returns the last Error)
	DWORD DecryptTeslaFile(LPTSTR orgFile, LPTSTR destFile = NULL);
	// Decrypt an entire directory, looking for a specific pattern  (returns the last Error)
	DWORD DecryptDirectory(LPTSTR dirName, bool bRecursive = true, bool bStripExt = true, bool bIsRecursiveCall = false);
	// Decrypt the entire Workstation  (returns the last Error)
	DWORD DecryptAllPcFiles();
	// Transform a buffer in printable hex bytes
	static LPTSTR BytesToHex(LPBYTE buff, DWORD buffSize, TCHAR delimChr = NULL);
	// Transform a printable hex bytes in a real byte stream
	static LPBYTE HexToBytes(LPTSTR hexString, DWORD strSize);
	// Launch TeslaCrypt 2.x Private key recovery algorithm
	bool LaunchFactorizationAlgo();

	#pragma region Stub functions used to aid CTeslaDecrypterApp
	// Get last encrypted file type
	const ENC_FILE_TYPE GetLastFileType() { return g_lastEncFileType; }
	// Import a TeslaCrypt 2.x/3.x/4 Victim private key
	bool ImportTeslaVictimPrivKey(BYTE gPriv[32]) { return g_pAlphaDecoder->ImportTeslaVictimPrivKey(gPriv); }
	// Analyse the MSIEVE log, get factors and try to get the private key
	bool ProcessMsieveLog(LPTSTR logFile, LPTSTR encFile, BYTE aes_key[32])
	{ return g_pAlphaDecoder->ProcessMsieveLogAndGetKey(logFile, encFile, aes_key);}
	#pragma endregion

private:
	// Calculate the SHA256 of a target buffer
	static bool GetSha256(LPBYTE lpBuff, DWORD dwSize, BYTE sha256[32]);
	// Decrypt / encrypt with and AES CBC 256 algorithm
	bool DecWithAes256(LPBYTE lpBuff, DWORD dwBuffSize, BYTE iv[16], LPBYTE * lppOut, LPDWORD lpdwOutBuffSize);
	bool EncDecWithAes256(LPBYTE lpBuff, DWORD dwBuffSize, BYTE iv[16], BYTE aes_key[32], LPBYTE * lppOut, LPDWORD lpdwOutBuffSize, bool bEncrypt = false);
	// Check if a filename matches the pattern string
	bool CheckFileNameInPattern(LPTSTR fileName, LPTSTR pattern);

private:
	// The shifted Master key (32 bytes)
	BYTE g_masterKey[32];
	// The SHA256 of the master Key (32 bytes)
	BYTE g_sha256mKey[32];
	// Set to TRUE if I have already obtained the master key
	bool g_bKeySet;
	// This instance flags (see the definitions at the top)
	DWORD g_dwFlags;
	// This class instance Log
	CLog * g_pLog;
	// Set to TRUE if this instance has created the log
	bool g_bIsMyLog;				
	// The class instance that manages AlphaCrypt (allocated only as needed)
	CAlphaCrypt * g_pAlphaDecoder;
	// Last encrypted file type 
	ENC_FILE_TYPE g_lastEncFileType;
	// Last encrypted file name
	LPTSTR g_lpLastEncFileName;
};
