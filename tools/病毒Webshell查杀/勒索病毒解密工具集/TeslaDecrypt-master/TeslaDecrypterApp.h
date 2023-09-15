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
 *	Filename: TeslaDecrypterApp.h
 *	Defines the CTeslaDecrypterApp class 
 *	Last revision: 05/30/2016
 *
 */

#pragma once
#include "TeslaDecrypter.h"
#include "log.h"

class CTeslaDecrypterApp
{
public:
	CTeslaDecrypterApp(void);
	~CTeslaDecrypterApp(void);

	// Main application entry point
	int Main(int argc, TCHAR * argv[]);

	// Parse the command line
	int ParseCommandLine(int argc, TCHAR * argv[]);

	// Perform a classical search in a buffer
	static LPBYTE SearchUString(LPBYTE buffer, DWORD buffSize, LPTSTR lpString, bool bCaseSensitive);

private:
	// Initialize the global APP Log
	bool InitializeLog();
	// Show this application command line usage
	void ShowUsage();
	// Create console screen buffer and set it to application
	bool SetConsoleBuffers();
	// Create application console and attach to executable
	bool CreateAndAttachConsole();
	// Normal application startup without any command line
	int NoCmdLineMain();
	// Compose destination decrypted file name
	static LPTSTR ComposeDestFileName(LPTSTR orgFile);
	// Search the "key.dat" file in standard locations
	LPTSTR SearchAndImportKeyFile();
	// Search if there is a suspicious TeslaCrypt process
	DWORD SearchForTeslaCryptProcess(LPTSTR lpFileFullPath = NULL, DWORD sizeInChars = 0);
	// Search and kill the TeslaCrypt process
	bool SearchAndKillTeslaProc(bool bAskUser = true, bool bKill = true, bool bDelete = false);
	// Launch the MSIEVE log analysis and exit
	bool ProcessMsieveLog(LPTSTR lpLogFile, LPTSTR lpInfectedFile);
	// Get the decrypter and allocate one if needed
	CTeslaDecrypter * GetDecrypter();

private:
	// %APPDATA% path
	LPTSTR g_strAppData;
	// Local %APPDATA% path
	LPTSTR g_strLocalAppData;

	// The global application Log
	CLog * g_pLog;

	// TRUE if this application is launched from console
	BOOLEAN g_bConsoleAttached;

	// The associated CTeslaDecrypter instance
	CTeslaDecrypter * g_pTeslaDec;

};
