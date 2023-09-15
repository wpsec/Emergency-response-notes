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
 *	Filename: EcFactorizer.h
 *	This class defines the Elliptic Curve Factorization 
 *	Last revision: 05/30/2016
 */
#pragma once
#include "Log.h"

// OpenSSL include files
#include "openssl\\bn.h"
#include "openssl\\ec.h"
#include "openssl\\obj_mac.h"

class CEcFactorizer
{
private:
	struct EC_FACTOR {
		BIGNUM * lpBn;							// The actual factor
		int idx;								// Index inside the list
		EC_FACTOR * next;						// Next item
	};
public:
	// The factor format enumeration
	enum FACTOR_FORMAT {
		FACTOR_FORMAT_BINARY = 0,				// A binary buffer
		FACTOR_FORMAT_DEC,						// A decimal string
		FACTOR_FORMAT_HEX						// An hex string
	};

	// The key type
	enum KEY_TYPE {
		KEY_TYPE_NONE = 0,						// No key
		KEY_TYPE_GLOBAL,						// Global key (Xpub or Sx)
		KEY_TYPE_SESSION,						// Per session key (Ypub or Sy)
	};

public:
	CEcFactorizer(CLog * pLog = NULL, int iCurveType = NID_secp256k1);
	~CEcFactorizer(void);

	// Add a factor to the list
	bool AddFactor(LPVOID factor, FACTOR_FORMAT format, DWORD dwSize = 0);

	// Set the public key (Xpub or Ypub)
	bool SetPubKey(LPVOID lpBuff, KEY_TYPE keyType, FACTOR_FORMAT format, DWORD dwSize = 0);
	// Set the recovery key (Sa o Sx)
	bool SetRecKey(LPVOID lpBuff, KEY_TYPE keyType, FACTOR_FORMAT format, DWORD dwSize = 0);

	// Start the factorization
	bool DoFactorization();

	// Get the factorized key (if success)
	bool GetFactorizedKey(LPVOID * lppBuff, FACTOR_FORMAT format, KEY_TYPE * lpKeyType = NULL, DWORD * lpDwSize = NULL);

private:
	// Free and delete the factor list
	void DeleteFactorList();

	// Get a factor from its index
	BIGNUM * GetFactorAt(int idx);

private:
	// The factorization curve
	EC_GROUP * g_lpCurve;
	// The Factor list
	EC_FACTOR * g_lpFactorList;
	// The target global public key 
	EC_POINT * g_lpTargetGlobalPubKey;
	// The target global public key 
	EC_POINT * g_lpTargetAesPubKey;
	// The global recovery key (Sx)
	BIGNUM * g_lpSx;
	// The per-session AES recovery key (Sa)
	BIGNUM * g_lpSa;
	// The factorized resulted private key
	BIGNUM * g_lpFactorizedKey;
	// The factorized key type (0 - None, 1 - Global private key (Gpriv), 2 - Per session AES key (Amaster))
	KEY_TYPE g_FactorizedKeyType;
	// This class instance Log
	CLog * g_pLog;
	// Set to TRUE if this instance has created the log
	bool g_bIsMyLog;				
};
