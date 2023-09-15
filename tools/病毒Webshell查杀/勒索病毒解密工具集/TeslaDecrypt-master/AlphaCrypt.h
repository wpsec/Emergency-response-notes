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
 *	Filename: AlphaCrypt.h
 *	Class that deals with Elliptic Curve encryption of the latest 
 *	AlphaCrypt droppers definition
 *	Last revision: 05/30/2016
 */

#pragma once
#include "Log.h"

// OpenSSL include files
#include "openssl\\bn.h"
#include "openssl\\ec.h"
#include "openssl\\obj_mac.h"

// The encrypted file type 
enum ENC_FILE_TYPE {
	ENC_FILE_TYPE_ERROR = -1,
	ENC_FILE_TYPE_UNKNOWN = 0,
	ENC_FILE_TYPE_TESLACRYPT_1,
	ENC_FILE_TYPE_ALPHACRYPT,
	ENC_FILE_TYPE_TESLACRYPT_2,
	ENC_FILE_TYPE_TESLACRYPT_3_OR_4,
};

class CAlphaCrypt
{
public:
	// Constructor with the associated log 
	CAlphaCrypt(CLog * pLog = NULL);
	// Default destructor
	~CAlphaCrypt();

	// Verify AlphaCrypt Master Key
	bool VerifyAlphaMasterKey(BYTE masterKey[32], BYTE recKey[0x40]);
	bool VerifyAlphaMasterKey(BYTE masterKey[32], LPSTR recKey);
	// Calculate the inverse of a key (needed in latest version of AlphaCrypt)
	bool GetTheInverse(BYTE key[32], BYTE inverse[32]);
	// Binary to hex conversion
	static LPSTR bin2hex(LPBYTE buff, DWORD dwBuffLen);
	// Get if a buffer represent an Hex string
	static bool IsBuffAnHexString(LPBYTE buff, DWORD dwLen, bool bUnicode = false);
	// Get the Tesla file type from its header
	static const ENC_FILE_TYPE GetTeslaFileType(LPBYTE lpFileHdr, DWORD dwHdrSize, DWORD dwFileSize);
	// Open a file header and try to determine the file type
	static const ENC_FILE_TYPE TryToGetFileType(LPTSTR lpFileFullPath, LPBYTE * lppFileHdr = NULL, DWORD * lpdwFileHdrSize = NULL);
	// Get the AES key of an encrypted TeslaCrypt 2.x file
	bool GetTeslaCrypt2AesKey(LPBYTE lpFileHdr, DWORD dwHdrSize, BYTE lpAesKey[32]);
	// Get the AES key of an encrypted TeslaCrypt 3 file
	bool GetTeslaCrypt3AesKey(LPBYTE lpFileHdr, DWORD dwHdrSize, BYTE lpAesKey[32]);
	// Calculate the SHA256 of a buffer
	bool CalculateSha256(LPBYTE lpBuff, DWORD dwBuffSize, BYTE outSha256[32]);
	// Decrypt a buffer with AES-256 (null IV)
	bool DecryptWithAes256(LPBYTE lpBuff, DWORD dwBuffSize, BYTE aes_key[32], LPBYTE * lppOut);
	// Launch TeslaCrypt 2.x Private key recovery algorithm
	bool LaunchTeslaCrypt2RecoveryAlgorithm(LPTSTR lpTargetFile, BYTE aes_key[32]);
	// Analyse the MSIEVE log, get factors and try to get the private key
	bool ProcessMsieveLogAndGetKey(LPTSTR logFile, LPTSTR encFile, BYTE aes_key[32], bool bGenerateKeyFile = true);
	// Verify the AES key of the encrypted TeslaCrypt 2.x & 3 file
	bool VerifyTeslaCryptAesKey(LPBYTE lpFileHdr, DWORD dwHdrSize, DWORD dwFileSize, BYTE lpAesKey[32]);
	// Import a TeslaCrypt 2.x/3.x/4 Victim private key
	bool ImportTeslaVictimPrivKey(BYTE gPriv[32]);

private:
	// Get the AlphaCrypt default PEER public Key
	static EC_POINT * GetAlphaCryptPublicKey();
	// Set the private key to a full-fledged curve
	static bool SetPrivateKey(EC_KEY * lpCurve, BIGNUM * lpPrivateKey);
	// Generate a Shared secret key from 
	static BIGNUM * GenerateSharedSecretKey(BIGNUM * pMasterKey, EC_POINT * lpPeerPubKey = NULL);
	// Verify the Recovery key for an AlphaCrypt infection
	bool VerifyRecoveryKey(BIGNUM * pMasterKey, BIGNUM * pRecKey);

private:
	// The used curve ID
	static const int g_iCurveId;
	// The TeslaCrypt 2 C&C Server Private Key
	static const BYTE g_lpTesla2_C2PrivKey[];
	// The TeslaCrypt 3 & 4 C&C Server Private Key
	static const BYTE g_lpTesla3_C2PrivKey[];

	// This class instance log
	CLog * g_pLog;
	// Is this log allocated by me?
	bool g_bIsMyLog;
	// TeslaCrypt 3 & 4 data
	// Last TeslaCrypt 3 & 4 per-session public key
	BYTE g_lpTesla3YPubKey[0x41];
	// Last TeslaCrypt 3 & 4 per-session AES key derived from "g_lpTesla3YpubKey"
	BYTE g_lpTesla3AesKey[0x20];
	// TeslaCrypt 2.x victim private key
	BYTE * g_lpTeslaVictimPrivKey;
	// The secp256k1 curve used to speed up a lot of functions
	EC_GROUP * g_lpCurve;
};
