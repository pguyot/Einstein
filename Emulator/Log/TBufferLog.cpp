// ==============================
// File:			TBufferLog.cp
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

#include "TBufferLog.h"

#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#if TARGET_OS_WIN32
	#include <io.h>
#else
	#include <sys/uio.h>
	#include <unistd.h>
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TBufferLog( void )
// -------------------------------------------------------------------------- //
TBufferLog::TBufferLog( void )
	:
		mTopLineIndex( 0 ),
		mLogFile( NULL ),
		mRefreshSocket( 0 )
{
	// Empty lines.
	int indexLines;
	for (indexLines = 0; indexLines < 32; indexLines++)
	{
		mBuffer[indexLines][0] = '\0';
		mBuffer[indexLines][79] = '\0';
	}
}

// -------------------------------------------------------------------------- //
//  * ~TBufferLog( void )
// -------------------------------------------------------------------------- //
TBufferLog::~TBufferLog( void )
{
	if (mLogFile)
	{
		::fclose( mLogFile );
		mLogFile = NULL;
	}
}

// -------------------------------------------------------------------------- //
//  * DoLogLine( const char* )
// -------------------------------------------------------------------------- //
void
TBufferLog::DoLogLine( const char* inLine )
{
	if (mLogFile)
	{
		(void) ::fprintf( mLogFile, "%s\n", inLine );
	}
	(void) ::strncpy( mBuffer[mTopLineIndex], inLine, 79 );
	mTopLineIndex = (mTopLineIndex + 1) % 32;
	
	if (mRefreshSocket)
	{
		// Write a dummy byte.
		char someByte = 1;
		(void) ::write( mRefreshSocket, &someByte, 1 );
	}
}

// -------------------------------------------------------------------------- //
//  * OpenLog( const char* )
// -------------------------------------------------------------------------- //
void
TBufferLog::OpenLog( const char* inLogPath )
{
	if (mLogFile)
	{
		LogLine( "Log file is already open." );
	} else {
		mLogFile = ::fopen( inLogPath, "a" );
		if (mLogFile == NULL)
		{
			LogLine( "Could not open log file." );
		} else {
			char theLine[512];
			(void) ::snprintf(
				theLine, 512, "Logging to '%s'", inLogPath );
			LogLine(theLine);
		}
	}
}

// -------------------------------------------------------------------------- //
//  * CloseLog( void )
// -------------------------------------------------------------------------- //
void
TBufferLog::CloseLog( void )
{
	if (mLogFile == NULL)
	{
		LogLine( "Log file is not open." );
	} else {
		LogLine( "Log file closed." );
		::fclose( mLogFile );
		mLogFile = NULL;
	}
}

// ======================================================================= //
// "Our attitude with TCP/IP is, `Hey, we'll do it, but don't make a big   //
// system, because we can't fix it if it breaks -- nobody can.'"           //
//                                                                         //
// "TCP/IP is OK if you've got a little informal club, and it doesn't make //
// any difference if it takes a while to fix it."                          //
//                 -- Ken Olson, in Digital News, 1988                     //
// ======================================================================= //
