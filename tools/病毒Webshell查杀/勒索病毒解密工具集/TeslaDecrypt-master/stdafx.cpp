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
 *	Filename: stdafx.h
 *	Standard useful functions implementations
 *	Last revision: 04/17/2015
 *	Developer: Andrea Allievi - aallievi@cisco.com
 *
 */

#include "stdafx.h"

#pragma region Useful generic functions
// Delete white space from start and end of a string
// NOTE: This function doesn't allocate any buffer
LPTSTR Trim(LPTSTR string, TCHAR leftCharToTrim, TCHAR rightCharToTrim) {
	if (!string) return NULL;
	int cch = wcslen(string);			// Chars counter
	int startSpaces = 0;				// Start spaces counter
	int endSpaces = 0;					// End Spaces counter

	// Count start spaces
	for (int i = 0; i < cch; i++) 
		if (string[i] != leftCharToTrim) break;
		else startSpaces++;
	
	// Cont end spaces
	for (int i = cch-1; i > 0; i--) 
		if (string[i] != rightCharToTrim) break;
		else endSpaces++;

	if (leftCharToTrim != rightCharToTrim) {
		if (startSpaces < endSpaces) endSpaces = startSpaces;
		else startSpaces = endSpaces;
	}

	if (!startSpaces && !endSpaces) return string;
	RtlCopyMemory(string, string + startSpaces, (cch - startSpaces - endSpaces) * sizeof(TCHAR));
	string[cch - startSpaces - endSpaces]= 0;
	return string;
}
LPSTR Trim(LPSTR string, CHAR leftCharToTrim, CHAR rightCharToTrim) {
	if (!string) return NULL;
	int cch = strlen(string);			// Chars counter
	int startSpaces = 0;				// Start spaces counter
	int endSpaces = 0;					// End Spaces counter

	// Count start spaces
	for (int i = 0; i < cch; i++) 
		if (string[i] != leftCharToTrim) break;
		else startSpaces++;
	
	// Cont end spaces
	for (int i = cch-1; i > 0; i--) 
		if (string[i] != rightCharToTrim) break;
		else endSpaces++;

	if (leftCharToTrim != rightCharToTrim) {
		if (startSpaces < endSpaces) endSpaces = startSpaces;
		else startSpaces = endSpaces;
	}

	if (!startSpaces && !endSpaces) return string;
	RtlCopyMemory(string, string + startSpaces, (cch - startSpaces - endSpaces) * sizeof(CHAR));
	string[cch - startSpaces - endSpaces]= 0;
	return string;
}

// Get if a file Exist
bool FileExists(LPTSTR fileName) {
	WIN32_FIND_DATA wfData = {0};
	DWORD strLen = 0, strMaxSize = 0;
	HANDLE h = NULL;
	LPTSTR colonPtr = NULL;
	LPTSTR searchPath = NULL;

	if (!fileName) return false;

	// Copy the original filename string and operates on it
	strMaxSize = wcslen(fileName) + 8;
	searchPath = new TCHAR[strMaxSize];
	wcscpy_s(searchPath, strMaxSize, fileName);

	colonPtr = wcsrchr(searchPath, L':');
	if (colonPtr && (colonPtr - searchPath) > 3) colonPtr[0] = L':';
	else colonPtr = NULL;

	// Remake of this function of 16/04/2014
	h = FindFirstFile(searchPath, &wfData);
	if (colonPtr) colonPtr[0] = ':';
	if (h != INVALID_HANDLE_VALUE) {
		delete searchPath;
		FindClose(h);
		return true;
	}

	// Check if this path is a root directory
	strLen = wcslen(searchPath);
	if (searchPath[strLen-1] != L'\\') {
		searchPath[strLen++] = L'\\'; searchPath[strLen] = 0; 
	}
	wcscat_s(searchPath, strMaxSize, L"*.*");
	h = FindFirstFile(searchPath, &wfData);
	delete searchPath;
	if (h != INVALID_HANDLE_VALUE) {
		FindClose(h); 
		return true; 
	}
	return false;
}
#pragma endregion

#pragma region Generic Environment Console functions
// Get Last Win32 Error description
LPTSTR GetWin32ErrorMessage(DWORD errNum) {
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf = NULL;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |  FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errNum, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );
	return (LPTSTR)lpMsgBuf;
}

// Read a line of input from a console
DWORD ReadLine(LPTSTR buff, int buffCch) {
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
	CONSOLE_READCONSOLE_CONTROL cControl = {0};
	DWORD dwCharRead = 0;
	cControl.nLength = sizeof(CONSOLE_READCONSOLE_CONTROL);
	cControl.dwCtrlWakeupMask = (ULONG)L'\n';
	ReadConsole(hConsole, buff, buffCch, &dwCharRead, &cControl);
	return dwCharRead;
}

void SetConsoleColor(ConsoleColor c){
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, (WORD)c);
}

// Color WPrintf 
void cl_wprintf(ConsoleColor c, LPTSTR string, LPVOID arg1, LPVOID arg2, LPVOID arg3, LPVOID arg4) {
	SetConsoleColor(c);
	wprintf(string, arg1, arg2, arg3, arg4);
	SetConsoleColor(GRAY);
}
#pragma endregion