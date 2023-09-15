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
 *	Filename: AlphaCrypt.cpp
 *	Contains the code needed to properly deal with the Elliptic 
 *	Curve encryption of the latest AlphaCrypt droppers
 *	Last revision: 05/30/2016
 */

#include "StdAfx.h"
#include "AlphaCrypt.h"
#include "EcFactorizer.h"
#include <strsafe.h>

// OpenSSL EC specific files:
#include "openssl\\ecdh.h"
#include "openssl\\ec_lcl.h"
#include "openssl\\ech_locl.h"
#include "openssl\\aes.h"			// For AES support
#include "openssl\\sha.h"			// For SHA256 support


// Set the default AlphaCrypt Curve ID (SECG curve over a 256 bit prime field)
const int CAlphaCrypt::g_iCurveId = NID_secp256k1;
// TheC&C Server Private Key of TeslaCrypt 3 & 4 
const BYTE CAlphaCrypt::g_lpTesla3_C2PrivKey[] = {0x44, 0x0A, 0x24, 0x1D, 0xD8, 0x0F, 0xCC, 0x56, 0x64, 0xE8, 0x61, 0x98, 0x9D, 0xB7, 0x16, 0xE0, 
	0x8C, 0xE6, 0x27, 0xD8, 0xD4, 0x0C, 0x7E, 0xA3, 0x60, 0xAE, 0x85, 0x5C, 0x72, 0x7A, 0x49, 0xEE};
// The C&C Server Private Key of TeslaCrypt 2 (still unknown)
const BYTE CAlphaCrypt::g_lpTesla2_C2PrivKey[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


// Constructor with the associated log 
CAlphaCrypt::CAlphaCrypt(CLog * pLog):
	g_pLog(NULL),
	g_lpTeslaVictimPrivKey(NULL),
	g_bIsMyLog(false),
	g_lpCurve(NULL)
{
	// Zero out the buffered keys
	_ASSERT(sizeof(g_lpTesla3YPubKey) == 0x41);
	_ASSERT(sizeof(g_lpTesla3AesKey) == 0x20);
	RtlZeroMemory(g_lpTesla3YPubKey, 0x41);
	RtlZeroMemory(g_lpTesla3AesKey, 0x20);

	if (!pLog) {
		// Initialize an empty log 
		g_pLog = new CLog();
		g_bIsMyLog = true;
	} else {
		g_pLog = pLog;
		g_bIsMyLog = false;
	}
}

// Default class destructor
CAlphaCrypt::~CAlphaCrypt(void)
{
	if (g_lpCurve) EC_GROUP_free(g_lpCurve);
	if (g_bIsMyLog && g_pLog) {
		g_pLog->Close();
		delete g_pLog;
	}
}

// Get the Tesla file type from its header
const ENC_FILE_TYPE CAlphaCrypt::GetTeslaFileType(LPBYTE lpFileHdr, DWORD dwHdrSize, DWORD dwFileSize) {
	/* TeslaCrypt 0.x - size 0x14:
	  + 0x00 - AES Initialization vector 
	  + 0x10 - File size DWORD
	 AlphaCrypt 0.x - size 0xC8
	  + 0x00 - Payment key in HEX (0x40 bytes)
	  + 0x40 - NULL Dword (0x4 bytes)
	  + 0x44 - Recovery key in HEX (0x80 bytes)
	  + 0xC4 - NULL Dword (0x4 bytes)
	  + 0xC8 - AES Initialization Vector (0x10 bytes)
	  + 0xD8 - File size DWORD
	 Teslacrypt 2.x (VVV version) - size 0x1C8:
	  + 0x00 - 0xEFBEADDEh (Deadbeef) signature
	  + 0x04 - Real Victim public key (0x41 bytes) - QA
	  + 0x45 - Real recovery key in HEX (0x80 bytes) - RECAB
	  + 0xC7 - Encrypted file AES Public key (0x41 bytes) - Qc
	  + 0x108 - AES encryption Recovery key in HEX (0x80 bytes) - RECCD
	  + 0x188 - NULL alignment WORD (2 bytes)
	  + 0x18A - AES Initialization vector (16 bytes)
	  + 0x19A - Original file size (4 bytes)
	 Teslacrypt 3.x (MP3, MICRO version), 4.0 - size 0x15C:
	  + 0x08 - 8 bytes filled by 0 - 8 bytes filled with and random data - 8 bytes with 0
	  + 0x18 - Real recovery key (0x61 bytes) - Rx
	  + 0x98 - Victim public key generated from the SHA256 of the victim private key (0x41 bytes) - QD
	  + 0xDC - AES encryption Recovery key (0x61 bytes) - Ra
	  + 0x15C - AES Initialization vector (16 bytes)
	  + 0x16C - Original file size (4 bytes) */

	DWORD dwOrgFileSize = 0;			// Original file size grabbed from the header
	BYTE ZeroBuff[8] = {0};
	if (!lpFileHdr || dwHdrSize < 0x10) return ENC_FILE_TYPE_ERROR;

	// TeslaCrypt 0.x
	if (dwHdrSize < 0x14) return ENC_FILE_TYPE_UNKNOWN;
	dwOrgFileSize = *((LPDWORD)(lpFileHdr + 0x10));
	if (dwFileSize >= dwOrgFileSize && dwFileSize < dwOrgFileSize  + 0x40)
		return ENC_FILE_TYPE_TESLACRYPT_1;

	// Alphacrypt 0.x
	if (dwHdrSize < 0xDC) return ENC_FILE_TYPE_UNKNOWN;
	dwOrgFileSize = *((LPDWORD)(lpFileHdr + 0xD8));
	if ((dwFileSize >= dwOrgFileSize && dwFileSize < dwOrgFileSize  + 0x100) &&
		IsBuffAnHexString(lpFileHdr, 0x40))
		return ENC_FILE_TYPE_ALPHACRYPT;

	// TeslaCrypt 3.x and 4.x
	if (dwHdrSize < 0x170) return ENC_FILE_TYPE_UNKNOWN;
	dwOrgFileSize = *((LPDWORD)(lpFileHdr + 0x16C));
	if ((dwFileSize >= dwOrgFileSize && dwFileSize < dwOrgFileSize  + 0x200) &&
		memcmp(ZeroBuff, lpFileHdr, 8) == 0 &&
		memcmp(ZeroBuff, lpFileHdr + 16, 8) == 0)
		return ENC_FILE_TYPE_TESLACRYPT_3_OR_4;

	// TeslaCrypt 2.x
	if (dwHdrSize < 0x1A0) return ENC_FILE_TYPE_UNKNOWN;
	dwOrgFileSize = *((LPDWORD)(lpFileHdr + 0x19A));
	if ((dwFileSize >= dwOrgFileSize && dwFileSize < dwOrgFileSize  + 0x200) &&
		IsBuffAnHexString(lpFileHdr + 0x45, 0x80))
		return ENC_FILE_TYPE_TESLACRYPT_2;
	return ENC_FILE_TYPE_UNKNOWN;
}

// Calculate the SHA256 of a buffer
bool CAlphaCrypt::CalculateSha256(LPBYTE lpBuff, DWORD dwBuffSize, BYTE outSha256[32]) {
	BYTE hash[SHA256_DIGEST_LENGTH];				// DAMN! See CTeslaDecrypter::GetSha256 - I have to verify that a function is not already implemented; I have to verify that a function is not already implemented; I have to verify that a function is not ....
	SHA256_CTX sha256 = {0};
	if (!lpBuff || dwBuffSize < 1) return false;
    
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, lpBuff, dwBuffSize);
    SHA256_Final(hash, &sha256);
	RtlCopyMemory(outSha256, hash, 32);
	return true;
}

// Decrypt a buffer with AES-256 (null IV)
bool CAlphaCrypt::DecryptWithAes256(LPBYTE lpBuff, DWORD dwBuffSize, BYTE aes_key[32], LPBYTE * lppOut) {
	const int key_length = 256;			// AES key size in bit
	LPBYTE lpOutBuff = NULL;			// Output buffer
	DWORD dwOutBuffSize = NULL;			// Output buffer aligned size
	BOOL bRetVal = FALSE;				// Returned value
	BYTE iv[16] = {0};					// The NULL Initialization Vector

	// Calculate the right output buffer size
	dwOutBuffSize = (dwBuffSize % AES_BLOCK_SIZE) + dwBuffSize;

	// Allocate the output buffer
	lpOutBuff = new BYTE[dwOutBuffSize];
	if (!lpOutBuff) return false;
	
	// Do the decryption
	AES_KEY dec_key = {0};				// Decryption key
    bRetVal = (AES_set_decrypt_key(aes_key, key_length, &dec_key) == 0);
	AES_cbc_encrypt(lpBuff, lpOutBuff, dwOutBuffSize, &dec_key, (LPBYTE)iv, AES_DECRYPT);
	
	if (bRetVal) {
		if (lppOut) *lppOut = lpOutBuff;
		else delete lpOutBuff;
		return true;
	}
	delete lpOutBuff;
	return false;
}

// Verify the AES key of the encrypted TeslaCrypt 2.x & 3 file
// This routine needs to be fast
bool CAlphaCrypt::VerifyTeslaCryptAesKey(LPBYTE lpFileHdr, DWORD dwHdrSize, DWORD dwFileSize, BYTE lpAesKey[32]) {
	BN_CTX * bnCtx = NULL;						// The BN context to speed up the process
	LPBYTE lpYPubKeyPtr = NULL;					// The temporary per-session public key (Ypub) ptr
	EC_POINT * lpYPubKey = NULL,				// The per-session AES public key (Ypub)
		* lpTestPubKey = NULL;					// The temporary calculated public key
	BIGNUM * lpBnAmaster = NULL;				// The per-session AES private key (Ypriv OR Amaster)
	int iRetVal = FALSE;						// Returned value from EC APIs
	bool bVerified = false;						// FALSE if the key has not been verified, TRUE otherwise
	ENC_FILE_TYPE fileType =					// The file type
		GetTeslaFileType(lpFileHdr, dwHdrSize, dwFileSize);

	// Create the curve (if needed)
	if (!g_lpCurve) g_lpCurve = EC_GROUP_new_by_curve_name(NID_secp256k1);

	if (fileType == ENC_FILE_TYPE_TESLACRYPT_2) {
		lpYPubKeyPtr = lpFileHdr + 0xC7;
	} else if (fileType == ENC_FILE_TYPE_TESLACRYPT_3_OR_4) {
		// In TeslaCrypt 3.x the Ypub and the AesMaster are totally unrelated (Amaster is not equal to Ypriv)
		lpYPubKeyPtr = lpFileHdr + 0xDC;
		// Use another trick: 
		if (memcmp(g_lpTesla3YPubKey, lpYPubKeyPtr, 0x41) == 0)
			return true;
		else
			return false;
	}

	// Allocate the CONTEXT
	bnCtx = BN_CTX_new();
	// Import the public key 
	lpYPubKey = EC_POINT_new(g_lpCurve);
	lpTestPubKey = EC_POINT_new(g_lpCurve);
	iRetVal = EC_POINT_oct2point(g_lpCurve, lpYPubKey, lpYPubKeyPtr, 0x41, bnCtx);
	// Import the private key
	lpBnAmaster = BN_bin2bn(lpAesKey, 32, NULL);
	// Calculate the test public key
	iRetVal = EC_POINT_mul(g_lpCurve, lpTestPubKey, lpBnAmaster, NULL, NULL, bnCtx);
	// Do the comparison
	if (iRetVal && EC_POINT_cmp(g_lpCurve, lpTestPubKey, lpYPubKey, bnCtx) == 0) 
		bVerified = true;

	// Cleanup and Exit
	if (bnCtx) BN_CTX_free(bnCtx);
	if (lpYPubKey) EC_POINT_free(lpYPubKey);
	if (lpTestPubKey) EC_POINT_free(lpTestPubKey);
	if (lpBnAmaster) BN_free(lpBnAmaster);
	return bVerified;
}

// Get the AES key of an encrypted TeslaCrypt 3/4 file
bool CAlphaCrypt::GetTeslaCrypt3AesKey(LPBYTE lpFileHdr, DWORD dwHdrSize, BYTE lpAesKey[32]) {
	EC_POINT * lpXPubKey = NULL,			// The global public key (Xpub)
		* lpYPubKey = NULL;					// The temporary per-session public key (Ypub)
	BIGNUM * lpC2PrivKey = NULL,			// The C&C private key (available now)
		* lpBnSx = NULL,					// The global shared secret key (Sx)
		* lpBnSa = NULL,					// The per-session shared secret
		* lpBnGPrivSha = NULL;				// The SHA256 of the global victim's private key (Gpri-sha) BigNumber
	BYTE SxBin[32] = {0},					// The global shared secret key (Sx) in binary format
		SaBin[32] = {0};					// The session shared secret key (Sa) in binary format
	BYTE SxSha[32] = {0},					// The SHA256 of global shared secret key (Sx-sha)
		SaSha[32] = {0};					// The SHA256 of the session shared secret key (Sa-sha)
	BYTE GPrivSha[32] = {0};				// The SHA256 of the global victim's private key (Gpri-sha)
	LPBYTE lpGPrivKey = NULL;				// The global victim's private key (Gpriv)
	LPBYTE  lpFinalAesKey = NULL;			// The final per-session AES key
	int iRetVal = FALSE;					// Returned value from EC APIs
	LPBYTE lpAesRecKey = lpFileHdr + 0xDC,	// The AES recovery key (Ra) ptr
		lpGlobalRecKey = lpFileHdr + 0x18;	// The Global recovery key (Rx) ptr
	DWORD dwSizeCheck = 0;					// A size checkpoint
	BYTE lpZeroedBuff[32] = {0};			// Zeroed buffer
	if (dwHdrSize < 0x170 || !lpFileHdr) return false;

	// NB: Holger has done an invaluable job in drawing the summary-picture of the decryption process:
	// http://blog.talosintel.com/2016/03/teslacrypt-301-tales-from-crypto.html - Figure C

	// Performance clue (I am too cunning :-) )
	if (memcmp(g_lpTesla3YPubKey, lpAesRecKey, 0x41) == 0) {
		// BINGO, I have already made all these damn calculation:
		RtlCopyMemory(lpAesKey, g_lpTesla3AesKey, 0x20);
		// Bye bye
		return true;			// ahahahah AaLl86 vs Damn PC   1 - 0
	}

	// Create the curve
	if (!g_lpCurve)
		g_lpCurve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	if (!g_lpCurve) return false;

	// Check and allocate the C&C private key BIGNUM
	if (memcmp(g_lpTesla3_C2PrivKey, lpZeroedBuff, 16) != 0)
		lpC2PrivKey = BN_bin2bn(g_lpTesla3_C2PrivKey, sizeof(g_lpTesla3_C2PrivKey), NULL);
	if (!lpC2PrivKey) return false; 

	/****************************************************************************************************
	 *								GLOBAL PER-WORKSTATION DATA:										*/
	// Import the global public key
	lpXPubKey = EC_POINT_new(g_lpCurve);
	iRetVal = EC_POINT_oct2point(g_lpCurve, lpXPubKey, lpGlobalRecKey, 0x41, NULL); 
	// ... and calculate the global shared secret (Sx) ...
	lpBnSx =  GenerateSharedSecretKey(lpC2PrivKey, lpXPubKey);
	dwSizeCheck = BN_num_bytes(lpBnSx);
	if (!lpBnSx || (dwSizeCheck < 31 || dwSizeCheck > 32)) {
		g_pLog->WriteLine(L"GetTeslaCrypt3AesKey - There were errors in calculating the Global shared secret key (Sx).");
		if (lpXPubKey) EC_POINT_free(lpXPubKey);
		if (lpBnSx)	BN_free(lpBnSx);
		return false;
	} else
		// Transform the Shared secret to Binary
		iRetVal = BN_bn2bin(lpBnSx, SxBin + (32 - dwSizeCheck));

	// Get the SHA256 of the global shared secret
	iRetVal = CalculateSha256(SxBin, sizeof(SxBin), SxSha);
	// Get the Global victim private key
	iRetVal = DecryptWithAes256(lpGlobalRecKey + 0x41, 32, SxSha, &lpGPrivKey);
	if (lpGPrivKey) {
		// Calculate its SHA256
		iRetVal = CalculateSha256(lpGPrivKey, 32, GPrivSha);
		// Convert it to a Big Number
		lpBnGPrivSha = BN_bin2bn(GPrivSha, SHA256_DIGEST_LENGTH, NULL);
	} else
		g_pLog->WriteLine(L"GetTeslaCrypt3AesKey - Unable to calculate the global victim private key (Gpriv).");
	
	// Cleanup
	if (lpC2PrivKey) BN_free(lpC2PrivKey); lpC2PrivKey = NULL;		// Doom 4 rulex!
	if (lpXPubKey) EC_POINT_free(lpXPubKey); lpXPubKey = NULL;		// Under the table there is a big elephant!
	if (lpBnSx)	BN_free(lpBnSx); lpBnSx = NULL;						// We don't need anymore
	if (lpGPrivKey) delete[] lpGPrivKey; 
	else return false;
	/****************************************************************************************************/

	/****************************************************************************************************
	 *										PER-SESSION DATA:											*/
	// Import the per-session public temporary key (Ypub)
	lpYPubKey = EC_POINT_new(g_lpCurve);
	iRetVal = EC_POINT_oct2point(g_lpCurve, lpYPubKey, lpAesRecKey, 0x41, NULL); 
	// Speed optimization: copy the per-session public key
	RtlCopyMemory(g_lpTesla3YPubKey, lpAesRecKey, sizeof(g_lpTesla3YPubKey));
	// ... and calculate the session shared secret (Sa) ...
	lpBnSa =  GenerateSharedSecretKey(lpBnGPrivSha, lpYPubKey);
	dwSizeCheck = BN_num_bytes(lpBnSa);
	if (!lpBnSa ||  (dwSizeCheck < 31 || dwSizeCheck > 32)) {
		g_pLog->WriteLine(L"GetTeslaCrypt3AesKey - There were errors in calculating the per-session shared secret key (Sa).");
		if (lpYPubKey) EC_POINT_free(lpYPubKey);
		if (lpBnSa)	BN_free(lpBnSa);
		return false;
	} else
		// Transform the Shared secret to Binary
		iRetVal = BN_bn2bin(lpBnSa, SaBin + (32 - dwSizeCheck));

	// Get the SHA256 of the session shared secret
	iRetVal = CalculateSha256(SaBin, sizeof(SaBin), SaSha);

	// Finally decrypt the master AES key:
	iRetVal = DecryptWithAes256(lpAesRecKey + 0x41, 32, SaSha, &lpFinalAesKey);

	if (iRetVal) {
		if (lpFinalAesKey)
			RtlCopyMemory(lpAesKey, lpFinalAesKey, 32);
		// Speed optimization: copy the per-session AES key
		RtlCopyMemory(g_lpTesla3AesKey, lpFinalAesKey, sizeof(g_lpTesla3AesKey));
	} else 
		g_pLog->WriteLine(L"GetTeslaCrypt3AesKey - Unable to calculate the per-session victim AES key (Amaster).");

	// Cleanup
	if (lpBnGPrivSha) BN_free(lpBnGPrivSha); lpBnGPrivSha = NULL;	
	if (lpYPubKey) EC_POINT_free(lpYPubKey); lpYPubKey = NULL;		
	if (lpBnSa)	BN_free(lpBnSa); lpBnSa = NULL;	
	if (lpFinalAesKey) delete[] lpFinalAesKey;
	/****************************************************************************************************/

	// Cleanup
	return (iRetVal > 0);
}

// Get the AES key of an encrypted TeslaCrypt 2.x file
bool CAlphaCrypt::GetTeslaCrypt2AesKey(LPBYTE lpFileHdr, DWORD dwHdrSize, BYTE lpAesKey[32]) {
	BIGNUM * lpC2PrivKey = NULL,			// The C&C private key (available now)
		* lpGPrivKey = NULL,				// The global victim's private key (Gpriv)
		* lpBnGPrivSha = NULL,				// The SHA256 of the global victim's private key (Gpri-sha) BigNumber
		* lpGlobalRecKey = NULL,			// The Global recovery key (Rx) Big number
		* lpAesRecKey = NULL,				// The per-session AES recovery key (Ra)
		* lpBnSx = NULL,					// The global shared secret key (Sx)
		* lpBnSa = NULL,					// The per-session shared secret (Sa)
		* lpBnAmaster = NULL;				// The per-session AES key
	EC_POINT * lpXPubKey = NULL,			// The temporary global public key (Xpub)
		* lpYPubKey = NULL;					// The temporary per-session public key (Ypub)
	BYTE GPrivSha[32] = {0};				// The SHA256 of the global victim's private key (Gpri-sha)
	BYTE GPrivBin[32] = {0};				// The global victim's private key (Gpriv) in binary format
	bool bGPrivAcquired = false;			// Set to true if I have already acquired the Victim private key (Gpriv)
	BN_CTX * bnCtx = NULL;					// The context for the BIGNUMBER calculations
	DWORD dwSizeCheck = 0;					// A size checkpoint
	int iRetVal = FALSE;					// Returned value from EC APIs
	BYTE lpZeroedBuff[32] = {0};			// Zeroed buffer

	// Important data pointers:	
	LPBYTE lpXPubKeyPtr = lpFileHdr + 0x04;						// The global public key (Xpub)
	LPBYTE lpYPubKeyPtr = lpFileHdr + 0xC7;						// The temporary per-session public key (Ypub)
	LPSTR lpGlobalRecKeyHex = (LPSTR)(lpFileHdr + 0x45);		// The HEX string of the global recovery key (Rx) ptr
	LPSTR lpAesRecKeyHex = (LPSTR)(lpFileHdr + 0x108);			// The HEX string of the AES recovery key (Ra) ptr
	if (!lpFileHdr || dwHdrSize < 0x1A0) return false;

	// NB: Holger hasn't done a job in drawing the summary-picture of the decryption process :-(
	// I have to do on my own 

	// Performance clue (I am too cunning :-) )
	if (memcmp(g_lpTesla3YPubKey, lpYPubKeyPtr, 0x41) == 0) {
		// BINGO, I have already made all these damn calculation:
		RtlCopyMemory(lpAesKey, g_lpTesla3AesKey, 0x20);
		// Bye bye
		return true;			// ahahahah AaLl86 vs Damn PC   2 - 0
	}

	// Create the curve ...
	if (!g_lpCurve)
		g_lpCurve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	if (!g_lpCurve) return false;
	// ...and the context
	bnCtx = BN_CTX_new();

	/****************************************************************************************************
	 *								GLOBAL PER-WORKSTATION DATA:										*/
	lpXPubKey = EC_POINT_new(g_lpCurve);
	// Import the global victim public key and re-calculate the shared secret (Sx)
	iRetVal = EC_POINT_oct2point(g_lpCurve, lpXPubKey, lpXPubKeyPtr, 0x41, bnCtx);

	if (g_lpTeslaVictimPrivKey != NULL) {
		// Try to calculate the Global public key and compare it (Gpub)
		lpGPrivKey = BN_bin2bn(g_lpTeslaVictimPrivKey, 32, NULL);
		EC_POINT * testPubKey = EC_POINT_new(g_lpCurve);
		iRetVal = EC_POINT_mul(g_lpCurve, testPubKey, lpGPrivKey, NULL, NULL, bnCtx);
		if (iRetVal) {
			// Do the verification
			if (EC_POINT_cmp(g_lpCurve, testPubKey, lpXPubKey, bnCtx) == 0) {
				// BINGO! The victim private key has been successfully verified
				bGPrivAcquired = true;
			}
		}
	
		if (!bGPrivAcquired) {
			// NOOOO, the verification is not correct :-(
			g_pLog->WriteLine(L"GetTeslaCrypt2AesKey - Detected an incorrect Victim private key. Resetting...");
			delete[] g_lpTeslaVictimPrivKey;
			g_lpTeslaVictimPrivKey = NULL;
			BN_free(lpGPrivKey); lpGPrivKey = NULL; 
		}
		EC_POINT_free(testPubKey);
	}

	if (!bGPrivAcquired) {
		// Check and allocate the C&C private key BIGNUM
		if (memcmp(g_lpTesla2_C2PrivKey, lpZeroedBuff, 16) != 0)
			// Import the private C&C private key:
			lpC2PrivKey = BN_bin2bn(g_lpTesla2_C2PrivKey, sizeof(g_lpTesla2_C2PrivKey), NULL);

		if (lpC2PrivKey) {
			lpBnSx = GenerateSharedSecretKey(lpC2PrivKey, lpXPubKey);
			dwSizeCheck = BN_num_bytes(lpBnSx);
		} else dwSizeCheck = 0;
		if (!lpBnSx || (dwSizeCheck < 31 || dwSizeCheck > 32)) {
			g_pLog->WriteLine(L"GetTeslaCrypt2AesKey - There were errors in calculating the Global shared secret key (Sx).");
			if (lpXPubKey) EC_POINT_free(lpXPubKey);
			if (lpBnSx)	BN_free(lpBnSx);
			if (bnCtx) BN_CTX_free(bnCtx);
			if (lpC2PrivKey) BN_free(lpC2PrivKey);
			return false;
		} 

		// Calculate the victim global private key (Gpriv = Rx / Sx)
		lpGPrivKey = BN_new();
		iRetVal = BN_hex2bn(&lpGlobalRecKey, lpGlobalRecKeyHex);
		iRetVal = BN_div(lpGPrivKey, NULL, lpGlobalRecKey, lpBnSx, bnCtx);
	} else
		iRetVal = 1;

	dwSizeCheck = BN_num_bytes(lpGPrivKey);
	if (iRetVal && (dwSizeCheck >= 31 && dwSizeCheck <= 32)) {
		iRetVal = BN_bn2bin(lpGPrivKey, GPrivBin + (32 - dwSizeCheck));
		// Calculate the SHA256 of the global private key (Gpri-sha)
		CalculateSha256(GPrivBin, sizeof(GPrivBin), GPrivSha);
	} else
		g_pLog->WriteLine(L"GetTeslaCrypt2AesKey - Unable to calculate the global victim private key (Gpriv).");

	// Cleanup
	if (lpC2PrivKey) BN_free(lpC2PrivKey); lpC2PrivKey = NULL;
	if (lpXPubKey) EC_POINT_free(lpXPubKey); lpXPubKey = NULL;		
	if (lpBnSx)	BN_free(lpBnSx); lpBnSx = NULL;						
	if (lpGlobalRecKey) BN_free(lpGlobalRecKey); lpGlobalRecKey = NULL;
	if (lpGPrivKey) { delete[] lpGPrivKey; lpGPrivKey = NULL; }
	else {
		if (bnCtx) BN_CTX_free(bnCtx);
		return false;
	}
	/****************************************************************************************************/

	/****************************************************************************************************
	 *										PER-SESSION DATA:											*/
	lpYPubKey = EC_POINT_new(g_lpCurve);
	lpBnGPrivSha = BN_bin2bn(GPrivSha, sizeof(GPrivSha), NULL);
	// Import the per-session public key (Ypub) and Calculate the AES shared secret (Sa)
	iRetVal = EC_POINT_oct2point(g_lpCurve, lpYPubKey, lpYPubKeyPtr, 0x41, bnCtx);
	lpBnSa = GenerateSharedSecretKey(lpBnGPrivSha, lpYPubKey);
	dwSizeCheck = BN_num_bytes(lpBnSa);
	if (!lpBnSa || (dwSizeCheck < 31 || dwSizeCheck > 32)) {
		g_pLog->WriteLine(L"GetTeslaCrypt2AesKey - There were errors in calculating the per-session shared secret key (Sa).");
		if (lpYPubKey) EC_POINT_free(lpYPubKey);
		if (lpBnSa)	BN_free(lpBnSa);
		if (lpBnGPrivSha) BN_free(lpBnGPrivSha);
		if (bnCtx) BN_CTX_free(bnCtx);
		return false;
	} 

	// Now calculate the AES key (Amaster)
	lpBnAmaster = BN_new();
	iRetVal = BN_hex2bn(&lpAesRecKey, lpAesRecKeyHex);
	dwSizeCheck = BN_num_bytes(lpAesRecKey);
	if (iRetVal && (dwSizeCheck >= 62 && dwSizeCheck <= 64))
		iRetVal = BN_div(lpBnAmaster, NULL, lpAesRecKey, lpBnSa, bnCtx);
	
	dwSizeCheck = BN_num_bytes(lpBnAmaster);
	if (iRetVal && (dwSizeCheck >= 31 && dwSizeCheck <= 32)) 
		BN_bn2bin(lpBnAmaster, lpAesKey + (32 - dwSizeCheck));
	else iRetVal = 0;

	// Cleanup
	if (lpYPubKey) EC_POINT_free(lpYPubKey);
	if (lpBnSa)	BN_free(lpBnSa); lpBnSa = NULL;
	if (lpBnGPrivSha) BN_free(lpBnGPrivSha); lpBnGPrivSha = NULL;
	if (lpAesRecKey) BN_free(lpAesRecKey); lpAesRecKey = NULL;
	if (lpBnAmaster) BN_free(lpBnAmaster); lpBnAmaster = NULL;
	if (bnCtx) BN_CTX_free(bnCtx);

	return (iRetVal != 0);
}

// Open a file header and try to determine the file type
const ENC_FILE_TYPE CAlphaCrypt::TryToGetFileType(LPTSTR lpFileFullPath, LPBYTE * lppFileHdr, DWORD * lpdwFileHdrSize) {
	HANDLE hFile = NULL;							// Target file handle
	BYTE * lpFileHdr = NULL ;						// The file header buffer
	DWORD dwHdrSize = 0x200;						// File header buffer size
	DWORD dwBytesIo = 0,							// Number of I/O bytes
		dwFileSize = 0;								// Total size of the target file
	BOOL bRetVal = FALSE;							// Win32 returned value
	ENC_FILE_TYPE fileType;

	hFile = CreateFile(lpFileFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return ENC_FILE_TYPE_ERROR;

	lpFileHdr = new BYTE[dwHdrSize];
	bRetVal = ReadFile(hFile, lpFileHdr, dwHdrSize, &dwBytesIo, NULL);
	dwFileSize = GetFileSize(hFile, NULL);
	CloseHandle(hFile);
	if (bRetVal) fileType = GetTeslaFileType(lpFileHdr, dwHdrSize, dwFileSize);
	else fileType = ENC_FILE_TYPE_ERROR;

	if (lppFileHdr) *lppFileHdr = lpFileHdr;
	else delete[] lpFileHdr;
	if (lpdwFileHdrSize) *lpdwFileHdrSize = dwBytesIo;
	return fileType;
}

// Launch TeslaCrypt 2.x Private key recovery algorithm
bool CAlphaCrypt::LaunchTeslaCrypt2RecoveryAlgorithm(LPTSTR lpTargetFile, BYTE aes_key[32]) {
	TCHAR appPath[MAX_PATH] = {0};					// The application path
	TCHAR pThreadDllPath[MAX_PATH] = {0};			// "pthreadGC2.dll" file full path
	TCHAR msievePath[MAX_PATH] = {0};				// "msieve152.exe" file full path
	TCHAR tmpFileName[MAX_PATH] = {0},
		logFileName[MAX_PATH]= {0};					// Temporary and Log file name
	TCHAR commandLine[MAX_PATH] = {0};				// The command line
	CHAR hexRecKey[0x100] = {0};					// The recovery key in hex
	LPBYTE lpFileHdr = NULL;						// The file header buffer
	BOOL bRetVal = FALSE;							// Win32 returned value
	LPTSTR lpSlashPtr = NULL;						// Slash chr pointer
	TCHAR answer[10] = {0};							// The user's answer
	DWORD dwSize = 0;								// A generic size
	SYSTEMTIME localTime = {0};
	ENC_FILE_TYPE fileType =						// The target file type
		ENC_FILE_TYPE_UNKNOWN;
	
	// Check the file
	fileType = TryToGetFileType(lpTargetFile, &lpFileHdr, &dwSize);
	if (fileType != ENC_FILE_TYPE_TESLACRYPT_2) {
		return false;
		//return ERROR_BAD_FILE_TYPE;
	}
	GetLocalTime(&localTime);

	// Get all paths
	GetModuleFileName(NULL, appPath, MAX_PATH);
	lpSlashPtr = wcsrchr(appPath, L'\\');
	if (lpSlashPtr) lpSlashPtr[1] = 0;
	// ... and compose the dependent files path
	wcscpy_s(pThreadDllPath, MAX_PATH, appPath);
	wcscpy_s(msievePath, MAX_PATH, appPath);
	wcscat_s(pThreadDllPath, MAX_PATH, L"pthreadGC2.dll");
	wcscat_s(msievePath, MAX_PATH, L"msieve152.exe");
	StringCchPrintf(logFileName, MAX_PATH, L"%s%04i%02i%02i%02i%02i_Msieve.log",
		appPath, localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, localTime.wMinute);

	// Get MSIEVE temp log file name
	GetTempPath(MAX_PATH, tmpFileName);
	GetTempFileName(tmpFileName, L"mie", 0, tmpFileName);
	DeleteFile(tmpFileName);

	// Compy the global Recovery key (Sx)
	dwSize = strlen((LPSTR)(lpFileHdr + 0x45));
	_ASSERT(dwSize == 0x80);
	strncpy_s(hexRecKey, COUNTOF(hexRecKey), (LPSTR)(lpFileHdr + 0x45), 0x80);

	// Compose the MSIEVE command line
	StringCchPrintfW(commandLine,  MAX_PATH, L"-v -e 0x%S -l \"%s\"", hexRecKey, tmpFileName);

	system("cls");

	g_pLog->WriteLine(L"LaunchTeslaCrypt2RecoveryAlgorithm - File \"%s\" needs the factorization algorithm to properly detect and validate its private key.", lpTargetFile);
	CVersionInfo appInfo;
	wprintf(L"Talos %s %s\r\n", appInfo.GetProductName(), appInfo.GetFileVersionString());
	wprintf(L"\r\nWarning! To properly decrypt file \"%s\",\r\n"
		L"the user's master private key is needed. A process called \"Factorization\" \r\n"
		L"needs to be executed. The algorithm can take up to 8H to run\r\n"
		L"depending on your system...\r\n\r\n", (wcsrchr(lpTargetFile, L'\\') + 1));

	if (FileExists(msievePath) && FileExists(pThreadDllPath)) {
		wprintf(L"Would you like to launch start the Factorization process? [Y/N] ");
		wscanf_s(L"%4s", answer, COUNTOF(answer));
		rewind(stdin);

		if ((answer[0] | 0x20) == 'y') {
			STARTUPINFO startupInfo = {0};
			PROCESS_INFORMATION procInfo = {0};

			// Launch here MSIEVE
			startupInfo.cb = sizeof(STARTUPINFO);
			startupInfo.lpTitle = L"Msieve Factorization (working...)";
			g_pLog->WriteLine(L"LaunchTeslaCrypt2RecoveryAlgorithm - Launching \"Msieve\" with the following command: %s.", commandLine);
			bRetVal = CreateProcess(msievePath, commandLine, NULL, NULL, FALSE, 
				CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, NULL, appPath, &startupInfo, &procInfo);
				
			if (bRetVal) {
				wprintf(L"\r\nWaiting for the factorization algorithm... ");
				WaitForSingleObject(procInfo.hProcess, INFINITE);
				g_pLog->WriteLine(L"LaunchTeslaCrypt2RecoveryAlgorithm - Msieve has ended!");
				CloseHandle(procInfo.hProcess); CloseHandle(procInfo.hThread);
			}
		} else
			// User has chosen to not launch the Factorization
			bRetVal = FALSE;
	} else
		g_pLog->WriteLine(L"LaunchTeslaCrypt2RecoveryAlgorithm - Msieve not found in the application directory.");

	if (!bRetVal) {
		// No msieve here
		wprintf(L"To properly launch the Factorization and recover the victim's private\r\n"
			L"key you should download and execute MSIEVE from the website:\r\n");
		cl_wprintf(YELLOW, L"   https://sourceforge.net/projects/msieve/\r\n\r\n");
		wprintf(L"Execute it with the following command line arguments:\r\n"
			L"msieve.exe -v -e 0x%S\r\n\r\n", hexRecKey);
		wprintf(L"Finally relaunch this application using the command argument\r\n"
			L"   /process-msieve:<Msieve.log_Path> /file:%s\r\n\r\n", lpTargetFile);
	} else {
		bRetVal = ProcessMsieveLogAndGetKey(tmpFileName, lpTargetFile, aes_key);
		if (!bRetVal) {
			cl_wprintf(RED, L"Error!\r\n");
			wprintf(L"There were errors while executing the factorization algorithm.\r\n"
				L"See the log file for details....\r\n");
		} else 
			cl_wprintf(GREEN, L"Done!\r\n");

		// Try to move the temporary MSIEVE log
		if (MoveFile(tmpFileName, logFileName))
			g_pLog->WriteLine(L"LaunchTeslaCrypt2RecoveryAlgorithm - The Msieve log has been saved in file \"%s\".", logFileName);
	}

	rewind(stdin);
	if (bRetVal) {
		wprintf(L"Press any key to continue...");
		_getwch();
		wprintf(L"\r\n");
	}
	return (bRetVal != FALSE);
}

// Analyse the MSIEVE log, get factors and try to get the private key
bool CAlphaCrypt::ProcessMsieveLogAndGetKey(LPTSTR logFile, LPTSTR encFile, BYTE aes_key[32], bool bGenerateKeyFile) {
	LPSTR logBuff = NULL;				// The log file buffer
	DWORD dwSize = 0,					// Generic file size
		dwBytesIo = 0;					// Number of I/O bytes
	HANDLE hLogFile = NULL;				// Handle to the log file
	BOOL bRetVal = FALSE;				// Returned value
	DWORD dwNumOfFactors = 0;			// Number of found factors
	LPBYTE lpFileHdr = NULL;			// The file header buffer
	CEcFactorizer::KEY_TYPE keyType;	// The found key type
	LPSTR lpCurPtr = NULL,
		lpEndLinePtr = NULL;
	if (!logFile || !encFile) return false;

	// Check the file
	ENC_FILE_TYPE fileType = TryToGetFileType(encFile, &lpFileHdr, &dwSize);
	if (fileType != ENC_FILE_TYPE_TESLACRYPT_2 || dwSize < 0x190) 
		return false;

	hLogFile = CreateFile(logFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hLogFile == INVALID_HANDLE_VALUE) return false;
	dwSize = GetFileSize(hLogFile, NULL);
	logBuff = new CHAR[dwSize + 2];
	bRetVal = ReadFile(hLogFile, logBuff, dwSize, &dwBytesIo, NULL);
	CloseHandle(hLogFile);
	if (!bRetVal) { delete[] logBuff; return false; }

	// Allocate the EC Factorizer instance
	CEcFactorizer factorizer(g_pLog);

	// Now process each line
	lpCurPtr = logBuff;
	while (lpCurPtr) {
		LPSTR lpFactorPtr = NULL,				// Found factor string ptr
			lpEndExecPtr = NULL;				// END of execution string ptr

		lpEndLinePtr = strchr(lpCurPtr, '\n');
		if (lpEndLinePtr && (lpEndLinePtr - lpCurPtr) > 2) {
			if ((lpEndLinePtr-1)[0] == '\r') 
				// Substitute each "carriage return" with a space char
				(lpEndLinePtr - 1)[0] = ' ';
			lpEndLinePtr[0] = 0;
		}

		// Analyse current line
		Trim(lpCurPtr);
		_strlwr_s(lpCurPtr, strlen(lpCurPtr) + 1);

		lpFactorPtr = strstr(lpCurPtr, " factor:");
		lpEndExecPtr = strstr(lpCurPtr, "elapsed time ");
		if (lpFactorPtr) {
			// Found one factor
			lpFactorPtr += strlen(" factor:");
			Trim(lpFactorPtr);
			dwNumOfFactors++;
			bRetVal = factorizer.AddFactor((LPVOID)lpFactorPtr, CEcFactorizer::FACTOR_FORMAT_DEC);
		} else if (lpEndExecPtr) {
			// Found END of execution block
			lpEndExecPtr += strlen("elapsed time ");
			g_pLog->WriteLine(L"ProcessMsieveLogAndGetKey - Found Log's end block statement. Total number of added factors: %i", (LPVOID)dwNumOfFactors);
			break;
		}

		// Go to the next line
		if (!lpEndLinePtr) break;
		lpEndLinePtr[0] = '\n';
		lpCurPtr = lpEndLinePtr + 1;
	}
	
	// Add the available keys
	//  + 0x04 - Real Victim public key (0x41 bytes) - Gpub
	factorizer.SetPubKey(lpFileHdr + 0x04, CEcFactorizer::KEY_TYPE_GLOBAL, CEcFactorizer::FACTOR_FORMAT_BINARY, 0x41);
	//  + 0x45 - Real recovery key in HEX (0x80 bytes) - Sx
	factorizer.SetRecKey(lpFileHdr + 0x45, CEcFactorizer::KEY_TYPE_GLOBAL, CEcFactorizer::FACTOR_FORMAT_HEX, 0x80);
	//  + 0xC7 - Encrypted file AES Public key (0x41 bytes) - Ypub
	factorizer.SetPubKey(lpFileHdr + 0xC7, CEcFactorizer::KEY_TYPE_SESSION, CEcFactorizer::FACTOR_FORMAT_BINARY, 0x41);
	//  + 0x108 - AES encryption Recovery key in HEX (0x80 bytes) - Sa */
	factorizer.SetRecKey(lpFileHdr + 0x108, CEcFactorizer::KEY_TYPE_SESSION, CEcFactorizer::FACTOR_FORMAT_HEX, 0x80);

	// Let's start the factorization
	bRetVal = factorizer.DoFactorization();
	if (bRetVal) {
		LPBYTE lpKey = NULL;
		DWORD dwKeySize = 0;

		bRetVal = factorizer.GetFactorizedKey((LPVOID*)&lpKey, CEcFactorizer::FACTOR_FORMAT_BINARY, &keyType, &dwKeySize);
		if (bRetVal && dwKeySize == 32) {
			if (keyType == CEcFactorizer::KEY_TYPE_GLOBAL) {
				// Good, I have found the global victim key... copy it and grab the master aes key
				if (g_lpTeslaVictimPrivKey) delete[] g_lpTeslaVictimPrivKey;
				g_lpTeslaVictimPrivKey = new BYTE[32];
				RtlCopyMemory(g_lpTeslaVictimPrivKey, lpKey, 32);

				// Grab the AES key
				bRetVal = GetTeslaCrypt2AesKey(lpFileHdr, 0x1A0, aes_key);
				if (bRetVal) {
					LPSTR lpHex = bin2hex(aes_key, 0x20);
					g_pLog->WriteLine(L"ProcessMsieveLogAndGetKey - Successfully calculated the per-session AES key: 0x%S", lpHex);
					if (lpHex) delete lpHex;
				}
			} else {
				// Standard AES key:
				RtlCopyMemory(aes_key, lpKey, dwKeySize);
				bRetVal = TRUE;
			}
			delete[] lpKey;
		}
	}
	if (!bRetVal) return false;
	if (!bGenerateKeyFile) return true;

	// Now generate the new faked "key.dat" file
	HANDLE hKeyFile = NULL;					// Handle to the "key.dat" file
	BYTE keyFileBuff[0x280] = {0};			// A new TeslaCrypt 1 faked key file
	TCHAR appPath[MAX_PATH] = {0},			// This application path
		keyFilePath[MAX_PATH] = {0},		// The "key.dat" file full path
		oldKeyFile[MAX_PATH] = {0};			// The original "key.dat" file new path
	dwBytesIo = 1;

	// Get all the needed paths
	GetModuleFileName(NULL, appPath, MAX_PATH);
	LPTSTR lpSlashPtr = wcsrchr(appPath, L'\\');
	if (lpSlashPtr) lpSlashPtr[1] = 0;
	// ... and compose the dependent files path
	wcscpy_s(keyFilePath, MAX_PATH, appPath);
	wcscpy_s(oldKeyFile, MAX_PATH, appPath);
	wcscat_s(keyFilePath, MAX_PATH, L"key.dat");
	wcscat_s(oldKeyFile, MAX_PATH, L"old_key_01.dat");
	dwSize = wcslen(oldKeyFile);
	while (FileExists(oldKeyFile)) {
		StringCchPrintf(oldKeyFile + dwSize - 6, 8, L"%02i.dat", dwBytesIo);
		dwBytesIo++;
	}

	// Create a faked "key.dat" file
	const LPSTR hdrString = "TESLACRYPT configuration file.\r\n"
		"This file has been generated by TALOS TeslaDecrypter. Please do not delete.";
	RtlCopyMemory(keyFileBuff, hdrString, strlen(hdrString) + 1);		// YES I don't use strcpy_s due to the padding bytes (FEFEFEFEFEF...)
	RtlCopyMemory(keyFileBuff + 0x278, "AaLl86", 7);					// "key.dat" file signature :-) :-)
	RtlCopyMemory(keyFileBuff + 0x177, aes_key, 32);
	* ((LPDWORD)(keyFileBuff + 0x126)) = 2016;
	if (keyType == CEcFactorizer::KEY_TYPE_GLOBAL && g_lpTeslaVictimPrivKey) {
		// Insert even the private key inside the faked key file
		RtlCopyMemory(keyFileBuff + 0x137, g_lpTeslaVictimPrivKey, 32);
	}

	if (bRetVal) {
		// Rename the original "key.dat" file (if exists)
		if (MoveFile(keyFilePath, oldKeyFile))
			g_pLog->WriteLine(L"ProcessMsieveLogAndGetKey - The old \"key.dat\" file has been renamed to \"%s\".", oldKeyFile);
		// And create the new one
		hKeyFile = CreateFile(keyFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hKeyFile != INVALID_HANDLE_VALUE) {
			bRetVal = WriteFile(hKeyFile, keyFileBuff, sizeof(keyFileBuff), &dwBytesIo, NULL);
			CloseHandle(hKeyFile);
			if (bRetVal) 
				g_pLog->WriteLine(L"ProcessMsieveLogAndGetKey - Successfully created a \"key.dat\" file that contains the calculated AES key.");
		}
	}
	return true;
}

// Import a TeslaCrypt 2.x & 3.x Victim private key
bool CAlphaCrypt::ImportTeslaVictimPrivKey(BYTE gPriv[32]) {
	BYTE zeroedBuff[32] = {0};		// A zeroed 32-bytes buffer
	LPBYTE lpVictimPrivKey = NULL;
	if (memcmp(gPriv, zeroedBuff, 32) == 0) return false;

	lpVictimPrivKey  = new BYTE[32];
	RtlCopyMemory(lpVictimPrivKey, gPriv, 32);
	LPSTR lpGPrivHex = bin2hex(gPriv, 32);
	g_pLog->WriteLine(L"ImportTeslaVictimPrivKey - Successfully imported the victim private key (Gpriv - 0x%S) for TeslaCrypt 2.x,3 or 4.", lpGPrivHex);
	if (lpGPrivHex) delete[] lpGPrivHex;
	g_lpTeslaVictimPrivKey = lpVictimPrivKey;
	return true;
}


// Binary to hex conversion
LPSTR CAlphaCrypt::bin2hex(LPBYTE buff, DWORD dwBuffLen) {
	const LPSTR hexMap = "0123456789ABCDEF";
	DWORD strLen = dwBuffLen * 2 + 1;
	LPSTR hexBuff = new CHAR[strLen];
	RtlZeroMemory(hexBuff, strLen);

	for (int i = 0; i < (int)dwBuffLen; i ++) {
		BYTE curByte = buff[i];
		hexBuff[(i*2)] = hexMap[curByte >> 4];
		hexBuff[(i*2)+1] = hexMap[curByte & 0xF];
	}
	return hexBuff;
}

// Get if a buffer represent an Hex string
bool CAlphaCrypt::IsBuffAnHexString(LPBYTE buff, DWORD dwLen, bool bUnicode) {
	bool bIsBinary = false;	
	LPTSTR uStr = (LPTSTR)buff;
	if (!buff || dwLen < 1 || (dwLen % 2) > 0) 
		return false;

	if (!bUnicode) {
		// ASCII check
		for (int i = 0; i < (int)dwLen; i++) {
			// Verify if the key is in hex digits or in standard binary digits
			bool bIsHexDigit = 
				(buff[i] >= '0' && buff[i] <= '9') ||
				(buff[i] >= 'A' && buff[i] <= 'F');
			if (!bIsHexDigit) {
				bIsBinary = true;
				break;
			}
		}	
	} 
	else {
		// Unicode check
		if ((dwLen % 4) > 0) return false;
		for (int i = 0; i < (int)(dwLen / 2); i++) {
			// Verify if the key is in hex digits or in standard binary digits
			bool bIsHexDigit = 
				(uStr[i] >= L'0' && uStr[i] <= L'9') ||
				(uStr[i] >= L'A' && uStr[i] <= L'F');
			if (!bIsHexDigit) {
				bIsBinary = true;
				break;
			}
		}	
	}
	return (!bIsBinary);
}

// Get the AlphaCrypt default PEER public Key
EC_POINT * CAlphaCrypt::GetAlphaCryptPublicKey() {
	EC_KEY * lpPublicCurve = NULL;				// Curve that contains the public key
	EC_POINT * pubKey = NULL;					// Public key generated from the 2 coordinates
	const LPSTR XCoordHex = "46668077A4449322CA896BD64901DE333156B6FEAE75ABE5D4922A039B3CD013";
	const LPSTR YCoordHex = "304AB8B3F15F498094F14058A1D1EBE823BEF512D44210CC50BBD94128D2CD05";
	BIGNUM * pBnX = NULL, * pBnY = NULL;
	int iRet = 0;

	// Allocate the 2 points structures
	pBnX = BN_new(); pBnY = BN_new();

	// Get X and Y Coordinate
	BN_hex2bn(&pBnX, XCoordHex);
	BN_hex2bn(&pBnY, YCoordHex);

	// Create the curve that contains the public key 
	lpPublicCurve = EC_KEY_new_by_curve_name(NID_secp256k1);

	// Create the generator 
	pubKey = EC_POINT_new(lpPublicCurve->group);

	// Generate the Public key and verify it
	EC_POINT_set_affine_coordinates_GFp(lpPublicCurve->group, pubKey, pBnX, pBnY, NULL);
	EC_KEY_set_public_key(lpPublicCurve, pubKey);
	iRet = EC_KEY_check_key(lpPublicCurve);

	// Cleanup
	EC_KEY_free(lpPublicCurve);
	BN_free(pBnX); BN_free(pBnY);

	if (iRet)
		return pubKey;
	else 
		EC_POINT_free(pubKey);
	return NULL;
}

// Generate a Shared secret key from 
BIGNUM * CAlphaCrypt::GenerateSharedSecretKey(BIGNUM * pUsrPrivKey, EC_POINT * lpPeerPubKey) {
	EC_KEY * lpFullCurve = NULL;				// Full elliptic curve
	EC_POINT * pubKey = NULL;					// The peer public key (bad guys one)
	ECDH_DATA * ecdh_data = NULL;				// Elliptic Curve data structure
	BYTE secretKey[0x20] = {0};					// Shared secret key
	BIGNUM * pSecretBn = NULL;					// Secret shared key BIGNUM
	int iRet = 0;

	if (!lpPeerPubKey)
		// Get the default AlphaCrypt peer public key
		pubKey = GetAlphaCryptPublicKey();		// DON'T forget to delete it, damn heck! :-)
	else
		// Don't delete the following one:
		pubKey = lpPeerPubKey;

	if (!pubKey) return NULL;

	// Create the FULL curve that contains public/private key pair
	lpFullCurve = EC_KEY_new_by_curve_name(NID_secp256k1);
	//EC_KEY_set_public_key(lpFullCurve, pStartKey);			// No my own public key (I need to calculate it)
	iRet = SetPrivateKey(lpFullCurve, pUsrPrivKey);
	iRet = EC_KEY_check_key(lpFullCurve);

	// Compute the shared secret key
	ecdh_data = ecdh_check(lpFullCurve);
	if (ecdh_data)
		ecdh_data->meth = ECDH_OpenSSL();
	// Calculate shared secret key: My private Key * Peer public key
	iRet = ECDH_compute_key(secretKey, 0x20, pubKey, lpFullCurve, NULL);

	// Convert the secret key in a BIGNUMBER 
	pSecretBn = BN_bin2bn(secretKey, 0x20, NULL);

	/*////////////////////////////////////////////////////////////////////////
	//							Brief explaination:							//
		Here is what "ECDH_compute_key" does:
		Calculate "da * Qb" (that is equal to "da * db * G"). Where:
			da = my ownPrivate key (the master key)
			Qb = the peer Public key (standard one inserted in AlphaCrypt)
	*////////////////////////////////////////////////////////////////////////

	// Cleanup
	EC_KEY_free(lpFullCurve);
	if (pubKey != lpPeerPubKey)
		EC_POINT_free(pubKey);
	return pSecretBn;
}


// Verify the Recovery key for an AlphaCrypt infection
bool CAlphaCrypt::VerifyRecoveryKey(BIGNUM * pMasterKey, BIGNUM * pRecKey) {
	// Big Number Context structure
	BN_CTX * lpBnCtx = NULL;	
	// Start public key
	EC_POINT * pubMasterKey = NULL;
	BIGNUM * lpBnSecret = NULL,				// The shared secret key
		* pNewRecKey = NULL;				// New Calculated Recovery Key
	// Set if the recovery key is verified
	bool bOkRecKey = false;			
	int iRet = 0;

	// Allocate the new curve
	if (!g_lpCurve)
		g_lpCurve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	// Allocate the context for the Big Numbers operation 
	lpBnCtx = BN_CTX_new();
	pubMasterKey = EC_POINT_new(g_lpCurve);

	// Compute my own initial public key (Qa = da * G)
	iRet = EC_POINT_mul(g_lpCurve, pubMasterKey, pMasterKey, NULL, NULL, lpBnCtx);

	if (iRet) {
		lpBnSecret = GenerateSharedSecretKey(pMasterKey, NULL);

		// Generate the ReCovery Key
		pNewRecKey = BN_new();
		BN_mul(pNewRecKey, lpBnSecret, pMasterKey, lpBnCtx);
		if (BN_cmp(pNewRecKey, pRecKey) == 0)
			bOkRecKey = true;

		// Cleanup
		BN_free(pNewRecKey);
		BN_free(lpBnSecret);
	}

	// Cleanup
	EC_POINT_free(pubMasterKey);
	BN_CTX_free(lpBnCtx);
	return bOkRecKey;
}

// Set the private key to a full-fledged curve
bool CAlphaCrypt::SetPrivateKey(EC_KEY * lpCurve, BIGNUM * lpPrivateKey) {
	BIGNUM * lpBnCopy = NULL;
	if (!lpPrivateKey) return false;
	if (lpCurve->priv_key) {
		// Free the resource of this private key
		BN_free(lpCurve->priv_key);
	}

	// Copy the private key
	lpBnCopy = BN_new();
	if (!BN_copy(lpBnCopy, lpPrivateKey)) {
		BN_free(lpBnCopy);
		return false;
	}
	lpCurve->priv_key = lpBnCopy;
	return true;
}

// Verify AlphaCrypt Master Key
bool CAlphaCrypt::VerifyAlphaMasterKey(BYTE masterKey[32], LPSTR recKey) {
	BIGNUM * bnPrivKey = NULL,			// My private key (master key)
		* bnRecKey = NULL;				// My recovery key 
	int len = 0;						// The length of the recovery key string
	bool bRetVal = false;

	// Get the master key
	bnPrivKey = BN_bin2bn(masterKey, 0x20, NULL);
	// Get the recovery key
	len = BN_hex2bn(&bnRecKey, recKey);

	if (bnPrivKey && bnRecKey &&
		len >= 0x80)
		bRetVal = VerifyRecoveryKey(bnPrivKey, bnRecKey);

	if (bnPrivKey) BN_free(bnPrivKey);
	if (bnRecKey) BN_free(bnRecKey);
	return bRetVal;
}

bool CAlphaCrypt::VerifyAlphaMasterKey(BYTE masterKey[32], BYTE recKey[0x40]) {
	LPSTR recKeyHex = bin2hex(recKey, 0x40);
	bool bRetVal = VerifyAlphaMasterKey(masterKey, recKeyHex);
	if (recKeyHex) delete recKeyHex ;
	return bRetVal;
}

// Calculate the inverse of a key (needed in latest version of AlphaCrypt)
bool CAlphaCrypt::GetTheInverse(BYTE key[32], BYTE inverse[32]) {
	bool bRetVal = true;
	BIGNUM * lpBnKey = NULL;				// The master key
	BYTE buff[0x100] = {0};					// Big buffer to prevent buffer overflow conditions
	int len = 0;
	BN_CTX * lpBnCtx = NULL;				// Big Number Context structure

	// Allocate the BIGNUMBER that host the master key, the curve and the context
	lpBnKey = BN_bin2bn(key, 0x20, NULL);
	if (!g_lpCurve) g_lpCurve = EC_GROUP_new_by_curve_name(NID_secp256k1);
	lpBnCtx = BN_CTX_new();

	if (!lpBnKey || !g_lpCurve || !lpBnCtx) {
		// Not enough available memory space - it's very unlikely that this will happen
		WriteToLog(L"CAlphaCrypt::GetTheInverse - Not enough system resources are available to complete the process.");
		bRetVal = false;
	}

	// Calculate the new Master key
	if (bRetVal && BN_mod_inverse(lpBnKey, lpBnKey, &g_lpCurve->order, lpBnCtx) != NULL) 
		// Save the new key
		len = BN_bn2bin(lpBnKey, buff);

	if (len == 0x20) {
		RtlCopyMemory(inverse, buff, 0x20);
		bRetVal = true;
	}
	else
		bRetVal = false;

	// Free the resource needed by CONTEXT and BIGNUM structure
	if (lpBnKey) BN_free(lpBnKey);
	if (lpBnCtx) BN_CTX_free(lpBnCtx);
	return bRetVal;

}
