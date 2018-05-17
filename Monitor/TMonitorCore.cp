// ==============================
// File:			TMonitor.cp
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

#include <K/Defines/KDefinitions.h>
#include <K/Streams/TFileStream.h>
#include "TMonitorCore.h"

// ANSI C & POSIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#if TARGET_OS_WIN32
	#include <assert.h>
#else
	#include <strings.h>
	#include <sys/socket.h>
	#include <sys/uio.h>
	#include <unistd.h>
#endif


// Einstein
#include "Monitor/TSymbolList.h"
#include "Emulator/JIT/JIT.h"


TMonitorCore::TMonitorCore(TSymbolList* inSymbolList)
:	mMemory(0L),
	mSymbolList(inSymbolList)
{
}


TMonitorCore::~TMonitorCore()
{
}


void TMonitorCore::PrintLine(const char* inLine, int type)
{
	puts(inLine);
}

// -------------------------------------------------------------------------- //
// ExecuteScript( const char* inCommand )
// -------------------------------------------------------------------------- //
Boolean
TMonitorCore::ExecuteScript( const char* inScriptFile )
{
	bool theResult = true;
	FILE *f = fopen(inScriptFile, "rb");
	if (f) {
		for (;;) {
			char buf[2048];
			if (feof(f)) break;
			if (fgets(buf, 2047, f)) {
				int n = (int)strlen(buf);
				if (n) {
					if (buf[n-1]=='\n') n--;
					if (buf[n-1]=='\r') n--;
					buf[n] = 0;
					char *s = buf;
					while (*s=='\t' || *s==' ') s++;
					if (s[0]!='#' && s[0]!=0)
						theResult = ExecuteCommand(s);
				}
			}
			if (theResult==false) break;
		}
		fclose(f);
	} else {
		theResult = false;
		PrintLine("Can't open script file", MONITOR_LOG_ERROR);
	}
	return theResult;
}

// -------------------------------------------------------------------------- //
// ExecuteCommand( const char* inCommand )
// -------------------------------------------------------------------------- //
Boolean
TMonitorCore::ExecuteCommand( const char* inCommand )
{
#if TARGET_OS_WIN32
	assert(0); // FIXME later
	return 0;
#else
	Boolean theResult = true;
	
	if (inCommand[0]=='#') {
		// script comment
	} else if (::strcmp(inCommand, "cd") == 0) {
		const char *home = ::getenv("HOME");
		::chdir(home);
	} else if (::strncmp(inCommand, "cd ", 3) == 0) {
		::chdir(inCommand+3);
	} else if (::strcmp(inCommand, "cwd") == 0) {
		char buf[2048];
		getcwd(buf, 2048);
		PrintLine(buf, MONITOR_LOG_INFO);
	} else if (inCommand[0]=='\'') {
		PrintLine(inCommand+1, MONITOR_LOG_INFO);
	} else {
		PrintLine("Unknown command. Type '?' for help.", MONITOR_LOG_ERROR);
		theResult = false;
	}
	return theResult;
#endif
}


// ==================================================================== //
// I am not now, nor have I ever been, a member of the demigodic party. //
//                 -- Dennis Ritchie                                    //
// ==================================================================== //
