/*
 *  Copyright (C) 2016 Cisco Talos Security Intelligence and Research Group
 *
 *  Author: Andrea Allievi
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
 *	Filename: EcFactorizer.cpp
 *	Implements the Elliptic Curve Factorization code
 *	Last revision: 05/30/2016
 *
 */
#include "StdAfx.h"
#include "EcFactorizer.h"

// Default class constructor
CEcFactorizer::CEcFactorizer(CLog * pLog, int iCurveType):
	g_lpFactorList(NULL),
	g_lpTargetGlobalPubKey(NULL),
	g_lpTargetAesPubKey(NULL),
	g_lpSx(NULL),
	g_lpSa(NULL),
	g_lpFactorizedKey(NULL),
	g_FactorizedKeyType(KEY_TYPE_NONE)
{
	
	if (!pLog) {
		// Initialize an empty log 
		g_pLog = new CLog();
		g_bIsMyLog = true;
	} else {
		g_pLog = pLog;
		g_bIsMyLog = false;
	}
	g_lpCurve = EC_GROUP_new_by_curve_name(iCurveType);
}

// Class destructor
CEcFactorizer::~CEcFactorizer(void)
{
	DeleteFactorList();

	// Cleanup old factorization
	if (g_lpFactorizedKey) {
		BN_free(g_lpFactorizedKey);
		g_lpFactorizedKey = NULL;
		g_FactorizedKeyType = KEY_TYPE_NONE;
	}
	if (g_lpCurve) EC_GROUP_free(g_lpCurve);
	if (g_lpTargetGlobalPubKey) EC_POINT_free(g_lpTargetGlobalPubKey);
	if (g_lpTargetAesPubKey) EC_POINT_free(g_lpTargetAesPubKey);
	if (g_lpSx) BN_free(g_lpSx);
	if (g_lpSa) BN_free(g_lpSa);
}

// Start the factorization
bool CEcFactorizer::DoFactorization() {
	EC_FACTOR * lpFactorItm = 
		g_lpFactorList;
	DWORD dwNumOfPrimes = 0;				// Number of prime numbers
	BIGNUM * lpProd = NULL;					// The product between each primes
	BN_CTX * bn_ctx = NULL;					// The EC context
	BIGNUM * bn_cofactor = NULL,			// The cofactor used to verify the recovery key
		* bn_remainder = NULL,				// A generic remainder of the division
		* bn_div = NULL;					// A test dividend
	BIGNUM * bn_ecdh = NULL;				// The global or per-session AES recovery key used for the factorization (PTR)
	BOOL bFound = FALSE;					// Set to TRUE if I have found the private key
	int iRetVal = 0;

	// Check the available data
	if ((g_lpSx == NULL && g_lpSa == NULL) || 
		(g_lpTargetGlobalPubKey == NULL && g_lpTargetAesPubKey == NULL))
		return false;

	// Allocate what needed:
	BN_dec2bn(&lpProd, "1");
	bn_ctx = BN_CTX_new();

	g_pLog->WriteLine(L"DoFactorization - Starting private key search algorithm...");
	// First do the product between each primes
	while (lpFactorItm) {
		BN_mul(lpProd, lpProd, lpFactorItm->lpBn, bn_ctx);
		lpFactorItm = lpFactorItm->next;
		dwNumOfPrimes++;
	}

	if (dwNumOfPrimes < 2) {
		g_pLog->WriteLine(L"DoFactorization - Error! Too few cofactors to properly launch the private key search algorithm.");
		if (lpProd) BN_free(lpProd);
		if (bn_ctx) BN_CTX_free(bn_ctx);
		return false;
	}

	// Checkpoint 1 - test the prod
	LPSTR lpProdHex = BN_bn2hex(lpProd);
	g_pLog->WriteLine(L"DoFactorization - The product of all the %i prime numbers is 0x%S",
		(LPVOID)dwNumOfPrimes, lpProdHex);
	delete lpProdHex;

	bn_cofactor = BN_new(); bn_remainder = BN_new();
	if (g_lpSx) iRetVal = BN_div(bn_cofactor, bn_remainder, g_lpSx, lpProd, bn_ctx);
	if (iRetVal && BN_num_bytes(bn_remainder) < 1) {
		bn_ecdh = g_lpSx;
		g_pLog->WriteLine(L"DoFactorization - Successfully verified the cofactors for the Global recovery key (Sx).");
	} else {
		// Try with the per-session shared recovery key
		BN_clear(bn_cofactor); BN_clear(bn_remainder);
		if (g_lpSa) iRetVal = BN_div(bn_cofactor, bn_remainder, g_lpSa, lpProd, bn_ctx);
		if (iRetVal && BN_num_bytes(bn_remainder) < 1) {
			bn_ecdh = g_lpSa;
			g_pLog->WriteLine(L"DoFactorization - Successfully verified the cofactors for the per-session AES recovery key (Sa).");
		}
	} 

	// Exit if the verification has not been passed
	if (!iRetVal || BN_num_bytes(bn_remainder)) {
		g_pLog->WriteLine(L"DoFactorization - Error! Superfluous factors or incorrect factorization detected!");
		BN_free(bn_cofactor); BN_free(bn_remainder); BN_free(lpProd);
		BN_CTX_free(bn_ctx);
		return false;
	}

	// Cleanup old factorization
	if (g_lpFactorizedKey) {
		BN_free(g_lpFactorizedKey);
		g_lpFactorizedKey = NULL;
		g_FactorizedKeyType = KEY_TYPE_NONE;
	}

	// Do the actual factorization
	// http://etutorials.org/Programming/secure+programming/Chapter+7.+Public+Key+Cryptography/7.4+Manipulating+Big+Numbers/
	BIGNUM * lpExp = BN_new();
	int iStep = 0;
	while (iStep < 2) {
		// Do this kind of stuff twice: 1 time starting with 1 - 1 time starting with cofactor
		for (int i = 1; i < (1 << dwNumOfPrimes); i++) {
			if (iStep == 0) BN_one(lpExp);			// x = 1
			else BN_copy(lpExp, bn_cofactor);		// x = cofactor

			for (int j = 0; j < (int)dwNumOfPrimes; j++) {
				if (i & (1 << j)) 
					BN_mul(lpExp, lpExp, GetFactorAt(j), bn_ctx);
			}
			
			BN_clear(bn_remainder); bn_div = BN_new();
			iRetVal = BN_div(bn_div, bn_remainder, bn_ecdh, lpExp, bn_ctx);
			if (BN_num_bits(lpExp) <= 256 && BN_num_bits(bn_div) <= 256) {
				// Calculate a new public key form the exponent (exp * curve_generator)
				EC_POINT * testPubKey = EC_POINT_new(g_lpCurve);
				iRetVal = EC_POINT_mul(g_lpCurve, testPubKey, lpExp, NULL, NULL, bn_ctx);

				// Transform the test Public key in OCT
				BYTE testKeyBuff[0x80] = {0};
				EC_POINT_point2oct(g_lpCurve, testPubKey, POINT_CONVERSION_UNCOMPRESSED, testKeyBuff, sizeof(testKeyBuff), bn_ctx);

				// Check now the public key
				if (iRetVal) {
					LPSTR lpHexKey = BN_bn2hex(lpExp);
					iRetVal = 600;			// Ahahahah what a hell? 600? Why?
					if (g_lpTargetGlobalPubKey)
						iRetVal = EC_POINT_cmp(g_lpCurve, testPubKey, g_lpTargetGlobalPubKey, bn_ctx);
					if (iRetVal == 0) {
						// BINGO! We have found the global private key
						g_FactorizedKeyType = KEY_TYPE_GLOBAL;
						g_lpFactorizedKey = lpExp;
						g_pLog->WriteLine(L"DoFactorization - Successfully found the global victim private key (Gpriv). Key: 0x%S", lpHexKey);
						bFound = TRUE;
						break;
					}
					if (g_lpTargetAesPubKey)
						iRetVal = EC_POINT_cmp(g_lpCurve, testPubKey, g_lpTargetAesPubKey, bn_ctx);
					if (iRetVal == 0) {
						// BINGO! We have found the per-session AES private key (Amaster)
						g_FactorizedKeyType = KEY_TYPE_SESSION;
						g_lpFactorizedKey = lpExp;
						g_pLog->WriteLine(L"DoFactorization - Successfully found the per-session AES private key (Amaster). Key: 0x%S", lpHexKey);
						bFound = TRUE;
						break;
					}
					if (lpHexKey) delete lpHexKey;
				}
				EC_POINT_free(testPubKey);
			}
		}
		if (bFound) break;
		iStep++;
	};
	
	// Cleanup
	if (bn_ctx) BN_CTX_free(bn_ctx);
	if (lpProd) BN_free(lpProd);
	if (bn_cofactor) BN_free(bn_cofactor);
	if (bn_remainder) BN_free(bn_remainder);
	if (bn_div) BN_free(bn_div);
	if (bFound)	return true;

	if (lpExp) BN_free(lpExp);
	return false;
}


// Add a factor to the list
bool CEcFactorizer::AddFactor(LPVOID factor, FACTOR_FORMAT format, DWORD dwSize) {
	EC_FACTOR * lpFactorItm = NULL,
		* lpLastFactor = NULL;
	BIGNUM * lpBn = NULL;
	int iRetVal = 0, idx = 0;
	if (!factor) return false;

	switch (format) {
		case FACTOR_FORMAT_BINARY:
			if (dwSize < 1) return false;
			lpBn = BN_bin2bn((LPBYTE)factor, dwSize, NULL);
			iRetVal = (lpBn != NULL);
			break;
		case FACTOR_FORMAT_DEC:
			iRetVal = BN_dec2bn(&lpBn, (LPSTR)factor);
			break;
		case FACTOR_FORMAT_HEX:
			iRetVal = BN_hex2bn(&lpBn, (LPSTR)factor);
			break;
	}
	if (!iRetVal) return false;

	// Log this factor
	LPSTR lpHexStr = BN_bn2hex(lpBn);
	g_pLog->WriteLine(L"AddFactor - Successfully addedd 0x%S prime number to the factor-list", (LPVOID)lpHexStr);
	delete[] lpHexStr;

	// Allocate an EC_FACTOR from the HEAP
	lpFactorItm = new EC_FACTOR();
	memset(lpFactorItm, 0, sizeof(EC_FACTOR));
	lpFactorItm->lpBn = lpBn;

	// Go to the end of the list
	lpLastFactor = g_lpFactorList;
	while (lpLastFactor) { 
		idx ++;
		if (!lpLastFactor->next) break;
		lpLastFactor = lpLastFactor->next;
	}

	if (lpLastFactor) lpLastFactor->next = lpFactorItm;
	else g_lpFactorList = lpFactorItm;
	lpFactorItm->idx = idx;
	return true;
}

// Get a factor from its index
BIGNUM * CEcFactorizer::GetFactorAt(int idx) {
	int iCurIdx = 0;
	EC_FACTOR * lpCurFactor = g_lpFactorList;

	while (lpCurFactor) {
		if (idx == iCurIdx) {		
			// Check this:
			_ASSERT(lpCurFactor->idx == iCurIdx);
			// Found it
			return lpCurFactor->lpBn;
		}
		iCurIdx++;
		lpCurFactor = lpCurFactor->next;
	}

	// I haven't found anything
	return NULL;
}

// Set the public key (Xpub or Ypub)
bool CEcFactorizer::SetPubKey(LPVOID lpBuff, KEY_TYPE keyType, FACTOR_FORMAT format, DWORD dwSize) {
	EC_POINT * pubKeyPt = NULL;
	int iRetVal = 0;
	if (!lpBuff) return false;

	pubKeyPt = EC_POINT_new(g_lpCurve);
	if (!pubKeyPt) return false;
	switch (format) {
		case FACTOR_FORMAT_BINARY:
			if (dwSize < 1) return false;
			iRetVal = EC_POINT_oct2point(g_lpCurve, pubKeyPt, (LPBYTE)lpBuff, dwSize, NULL);
			break;
		case FACTOR_FORMAT_HEX:
			iRetVal = (EC_POINT_hex2point(g_lpCurve, (LPSTR)lpBuff, pubKeyPt, NULL) != NULL);
			break;
		default:
			iRetVal = 0;			// No Import version here
			break;
	}
	if (!iRetVal) { EC_POINT_free(pubKeyPt); return false; }

	// Free the previous key
	if (keyType == KEY_TYPE_GLOBAL && g_lpTargetGlobalPubKey) {
		EC_POINT_free(g_lpTargetGlobalPubKey);
		g_lpTargetGlobalPubKey = NULL;
	} else if (keyType == KEY_TYPE_SESSION && g_lpTargetAesPubKey) {
		EC_POINT_free(g_lpTargetAesPubKey);
		g_lpTargetAesPubKey = NULL;
	}

	if (keyType == KEY_TYPE_GLOBAL) g_lpTargetGlobalPubKey = pubKeyPt; 
	else if (keyType == KEY_TYPE_SESSION) g_lpTargetAesPubKey = pubKeyPt;
	else { EC_POINT_free(pubKeyPt); return false; }
	return true; 
}

// Set the recovery key (Sa o Sx)
bool CEcFactorizer::SetRecKey(LPVOID lpBuff, KEY_TYPE keyType, FACTOR_FORMAT format, DWORD dwSize) {
	BIGNUM * lpBn = NULL;
	int iRetVal = 0;
	if (!lpBuff) return false;

	switch (format) {
		case FACTOR_FORMAT_BINARY:
			if (dwSize < 1) return false;
			lpBn = BN_bin2bn((LPBYTE)lpBuff, dwSize, NULL);
			iRetVal = (lpBn != NULL);
			break;
		case FACTOR_FORMAT_DEC:
			iRetVal = BN_dec2bn(&lpBn, (LPSTR)lpBuff);
			break;
		case FACTOR_FORMAT_HEX:
			iRetVal = BN_hex2bn(&lpBn, (LPSTR)lpBuff);
			break;
	}
	if (!iRetVal) return false;

	// Free old recovery key
	if (keyType == KEY_TYPE_GLOBAL && g_lpSx) {
		BN_free(g_lpSx);
		g_lpSx = NULL;
	} else if (keyType == KEY_TYPE_SESSION && g_lpSa) {
		BN_free(g_lpSa);
		g_lpSa = NULL;
	}

	if (keyType == KEY_TYPE_GLOBAL) g_lpSx = lpBn;
	else if (keyType == KEY_TYPE_SESSION) g_lpSa = lpBn;
	else { if (lpBn) BN_free(lpBn); return false;}
	return true;
}

// Free and delete the factor list
void CEcFactorizer::DeleteFactorList() {
	EC_FACTOR * lpItm = NULL,
		* lpNextItm = NULL;

	lpItm = g_lpFactorList;
	if (!lpItm) return;

	while (lpItm) {
		lpNextItm = lpItm->next;
		delete lpItm;
		lpItm = lpNextItm;
	}
	g_lpFactorList = NULL;
}

// Get the factorized key (if success)
bool CEcFactorizer::GetFactorizedKey(LPVOID * lppBuff, FACTOR_FORMAT format, KEY_TYPE * lpKeyType, DWORD * lpDwSize) {
	DWORD dwSize = 0;						// The buffer size
	LPBYTE lpBuff = NULL;					// The binary buffer
	int iRetVal = 0;
	if (!lppBuff) return false;
	if (!g_lpFactorizedKey || g_FactorizedKeyType == KEY_TYPE_NONE) return false;

	switch (format) {
		case FACTOR_FORMAT_BINARY:
			dwSize = BN_num_bytes(g_lpFactorizedKey);
			lpBuff = new BYTE[dwSize + 1];
			RtlZeroMemory(lpBuff, dwSize);
			iRetVal = BN_bn2bin(g_lpFactorizedKey, lpBuff);
			break;
		case FACTOR_FORMAT_DEC:
			lpBuff = (LPBYTE)BN_bn2dec(g_lpFactorizedKey);
			dwSize = strlen((LPSTR)lpBuff) + 1;
			break;
		case FACTOR_FORMAT_HEX:
			lpBuff = (LPBYTE)BN_bn2hex(g_lpFactorizedKey);
			dwSize = strlen((LPSTR)lpBuff) + 1;
			break;
	}
	*lppBuff = (LPVOID)lpBuff;
	if (lpDwSize) *lpDwSize = dwSize;
	if (lpKeyType) *lpKeyType = g_FactorizedKeyType;
	return true;
}
