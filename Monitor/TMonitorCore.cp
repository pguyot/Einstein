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

#ifdef JITTARGET_GENERIC
#include "Emulator/JIT/Generic/TJITGenericRetarget.h"
#endif

TMonitorCore::TMonitorCore(TSymbolList* inSymbolList)
:	mMemory(0L),
	mSymbolList(inSymbolList)
#ifdef JITTARGET_GENERIC
	,mRetarget(0L)
#endif
{
}


TMonitorCore::~TMonitorCore()
{
#ifdef JITTARGET_GENERIC
	if (mRetarget)
		delete mRetarget;
#endif
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
	
	if (inCommand[0]=='#') {	// script comment
#ifdef JITTARGET_GENERIC
	} else if (::strncmp(inCommand, "rt ", 3)==0) {
		theResult = ExecuteRetargetCommand(inCommand+3);
#endif
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

#ifdef JITTARGET_GENERIC
// -------------------------------------------------------------------------- //
// ExecuteRetargetCommand( const char* inCommand )
// -------------------------------------------------------------------------- //
Boolean
TMonitorCore::ExecuteRetargetCommand( const char* inCommand )
{
#if TARGET_OS_WIN32
	assert(0); // FIXME later
	return 0;
#else
	if (!mRetarget)
		mRetarget = new TJITGenericRetarget(mMemory, mSymbolList);
	Boolean theResult = true;
	if (::strncmp(inCommand, "open ", 5) == 0) {
		if (mRetarget->OpenFiles(inCommand+5)) {
			PrintLine("Can't open file", MONITOR_LOG_ERROR);
			theResult = false;
		} else {
			PrintLine("Retarget files opened", MONITOR_LOG_INFO);
		}
	} else if (::strcmp(inCommand, "close") == 0) {
		mRetarget->CloseFiles();
		PrintLine("Retarget files closed", MONITOR_LOG_INFO);
	} else if (::strncmp(inCommand, "code ", 5) == 0) {
		unsigned long first, last;
		int n = 0;
		n = sscanf(inCommand+5, "%lx-%lx", &first, &last);
		if (n==0) {
			char buf[512];
			::sprintf(buf, "rt code: Can't read memory range - undefined symbol? %s", inCommand);
			PrintLine(buf, MONITOR_LOG_ERROR);
			theResult = false;
		} else {
			mRetarget->SetRetargetMap((KUInt32)first, (KUInt32)last, 1);
		}
	} else if (::strncmp(inCommand, "cjit ", 5) == 0) {
		unsigned long first = 0, last = 4;
		int arg = 5;
		bool dontLink = 0;
		if (inCommand[arg]=='!') { dontLink = 1; arg++; }
		bool continueAfterFunction = 0;
		if (inCommand[arg]=='^') { continueAfterFunction = 1; arg++; }
		bool isJumpTable = 0;
		if (inCommand[arg]=='#') { isJumpTable = 1; arg++; }
		int n = 0;
		if (n==0) {
			first = mSymbolList->GetSymbol(inCommand+arg);
			if (first!=TSymbolList::kNoSymbol) {
				n = 1;
				last = (KUInt32)mSymbolList->GetNextSymbol((KUInt32)first);
				if (last!=TSymbolList::kNoSymbol) {
					n = 2;
				}
			}
		}
		if (n==0) {
			n = sscanf(inCommand+arg, "%lx-%lx", &first, &last);
		}
		if (n==0) {
			char buf[512];
			::sprintf(buf, "rt cjit: Can't read memory range - undefined symbol? %s", inCommand);
			PrintLine(buf, MONITOR_LOG_ERROR);
			theResult = false;
		} else {
			if (n==1) last = first + 80;
			char name[512], cmt[512];
			int off;
			const char *n = inCommand+arg;
			for (;;n++) { // skip the start and end address and find the function name
				if (*n==0) break;
				if (*n==' ') break;
			}
			if (isJumpTable) {
				for ( ; first<last; first+=4) {
					sprintf(name, "JumpTable_0x%08lX", first);
					mRetarget->TranslateFunction((KUInt32)first, (KUInt32)first+4, name, continueAfterFunction, dontLink);
				}
			} else {
				while (*n==' ') n++;
				if (*n) {
					strcpy(name, n);
				} else if (mSymbolList->GetSymbolExact((KUInt32)first, name, cmt, &off)) {
					// symbol name is in 'name'
				} else {
					sprintf(name, "Func_0x%08lX", first);
				}
				mRetarget->TranslateFunction((KUInt32)first, (KUInt32)last, name, continueAfterFunction, dontLink);
			}
		}
	} else if (::strncmp(inCommand, "cjitr ", 6) == 0) {
		unsigned long first = 0;
		int n = 0;
		if (n==0) {
			first = mSymbolList->GetSymbol(inCommand+6);
			if (first!=TSymbolList::kNoSymbol) {
				n = 1;
			}
		}
		if (n==0) {
			n = sscanf(inCommand+6, "%lx", &first);
		}
		if (n==0) {
			char buf[64];
			snprintf(buf, 64, "Start address not found: %s", inCommand);
			PrintLine(buf, MONITOR_LOG_ERROR);
			theResult = false;
		} else {
			char name[512], cmt[512];
			int off;
			const char *n = inCommand+6;
			for (;;n++) { // skip the start address and find the function name
				if (*n==0) break;
				if (*n==' ') break;
			}
			while (*n==' ') n++;
			if (*n) {
				strcpy(name, n);
			} else if (mSymbolList->GetSymbolExact((KUInt32)first, name, cmt, &off)) {
				// symbol name is in 'name'
			} else {
				sprintf(name, "Func_0x%08X", (unsigned int)first);
			}
			mRetarget->ReturnToEmulator((KUInt32)first, name);
		}
	} else if (::strcmp(inCommand, "monty") == 0) {
		int i, j, si;
		KUInt32 addr = 0;
		FILE *f = fopen("/Users/matt/dev/Einstein/_Data_/monty", "wb");
		for (i=0; i<16; i++) {
			for (j=0; j<16; j++) {
				fprintf(f, "\n\nrt open /Users/matt/dev/Einstein/Newt/Monty/Grp%X/Fn%X\n", i, j);
				for (si=0; si<100; si++) {
					char sym[1024];
					mSymbolList->GetSymbolExact(addr, sym);
					if (addr<0x003AE58C) {
						fprintf(f, "rt cjit %s\n", sym);
						addr = mSymbolList->GetNextSymbol(addr);
					}
				}
				fprintf(f, "rt close\n\n");
			}
		}
		fclose(f);
		f = fopen("/Users/matt/dev/Einstein/_Data_/monty.h", "wb");
		for (i=0; i<16; i++) {
			for (j=0; j<16; j++) {
				fprintf(f, "#include \"Monty/Grp%X/Fn%X.h\"\n", i, j);
			}
		}
		fclose(f);
	} else {
		theResult = false;
	}
	return theResult;
#endif
}
#endif

// ==================================================================== //
// I am not now, nor have I ever been, a member of the demigodic party. //
//                 -- Dennis Ritchie                                    //
// ==================================================================== //
