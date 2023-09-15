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
 *	Filename: stdafx.h
 *	Precompiled header file
 *	Last revision: 05/30/2016
 *
 */
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Escludere gli elementi utilizzati di rado dalle intestazioni di Windows
					// Exclude some items from the Windows header. See MSDN for more details.
// File di intestazione di Windows:
#include <windows.h>			// Windows header files

// File di intestazione Runtime C
// C Runtime File Headers
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>

// VC2008 Intrinsic functions
#include <intrin.h> 
typedef unsigned __int64 QWORD;

// Application Title and version
#define APPTITLE L"Talos TeslaCrypt Decryptor"
#define LOGTITLE L"Talos TeslaCrypt Decryptor 1.0"

#define CHR_UPR(x) (x & ~0x20)
#define CHR_LWR(x) (x | 0x20)

#ifdef _DEBUG
#define DbgBreak() __debugbreak()
#else
#define DbgBreak() __noop
#endif

// Get number elements of a STACK array
#define COUNTOF(x) sizeof(x) / sizeof(x[0])

// Delete white space from start and end of a string
LPTSTR Trim(LPTSTR string, TCHAR leftCharToTrim = ' ', TCHAR rightCharToTrim = ' ');
LPSTR Trim(LPSTR string, CHAR leftCharToTrim = ' ', CHAR rightCharToTrim = ' ');

// Get if a file Exist
bool FileExists(LPTSTR fileName);

// Get Last Win32 Error description
LPTSTR GetWin32ErrorMessage(DWORD errNum);

// Read a line of input from a console
DWORD ReadLine(LPTSTR buff, int buffCch);

enum ConsoleColor { DARKBLUE = 1, DARKGREEN, DARKTEAL, DARKRED, DARKPINK, DARKYELLOW, 
	GRAY, DARKGRAY, BLUE, GREEN, TEAL, RED, PINK, YELLOW, WHITE };

// Set console text Color
void SetConsoleColor(ConsoleColor c);
// Color WPrintf 
void cl_wprintf(ConsoleColor c, LPTSTR string, LPVOID arg1 = NULL, LPVOID arg2 = NULL, LPVOID arg3 = NULL, LPVOID arg4 = NULL);
