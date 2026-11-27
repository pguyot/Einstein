// ==============================
// File:			TLog.cp
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

#include "TLog.h"

// POSIX & ANSI
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TLog( void )
// -------------------------------------------------------------------------- //
TLog::TLog(void) = default;

// -------------------------------------------------------------------------- //
//  * ~TLog( void )
// -------------------------------------------------------------------------- //
TLog::~TLog(void) = default;

// -------------------------------------------------------------------------- //
//  * LogLine( const char* )
// -------------------------------------------------------------------------- //
void
TLog::LogLine(const char* inLine)
{
	if (mEnabled)
	{
		LockMutex();

		DoLogLine(inLine);

		UnlockMutex();
	}
}

// -------------------------------------------------------------------------- //
//  * FLogLine( const char*, ... )
// -------------------------------------------------------------------------- //
void
TLog::FLogLine(const char* inFormat, ...)
{
	if (mEnabled)
	{
		char bufferLine[512];
		va_list argList;

		va_start(argList, inFormat);
		(void) ::vsnprintf(bufferLine, sizeof(bufferLine), inFormat, argList);
		va_end(argList);

		LogLine(bufferLine);
	}
}

#if defined(_DEBUG) && defined(_MSC_VER)

#include "Emulator/Log/KPrintf.h"

#include <cstdarg>
#include <cstdlib>
#include <mutex>
#include <windows.h>

void
KPrintf(const char* format, ...)
{
	static char* textbuffer = nullptr;
	static int textbuffersize = 0;
	static std::mutex mutex;

	std::lock_guard<std::mutex> lock(mutex);

	if (textbuffersize == 0)
	{
		textbuffersize = 1023;
		textbuffer = static_cast<char*>(std::malloc(textbuffersize + 1U));
	}

	va_list args;
	va_start(args, format);
	int ret = std::vsnprintf(textbuffer, textbuffersize + 1U, format, args);
	va_end(args);
	if (ret > textbuffersize)
	{
		textbuffersize = ret + 1023;
		std::free(textbuffer);
		textbuffer = static_cast<char*>(std::malloc(textbuffersize + 1U));
		va_start(args, format);
		(void) std::vsnprintf(textbuffer, textbuffersize + 1U, format, args);
		va_end(args);
	}
	OutputDebugStringA(textbuffer);
}

#endif

// ============================================================== //
// Unix will self-destruct in five seconds... 4... 3... 2... 1... //
// ============================================================== //
