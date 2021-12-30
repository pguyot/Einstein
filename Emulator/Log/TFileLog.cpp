// ==============================
// File:			TFileLog.cp
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

#include "TFileLog.h"

#include <errno.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TFileLog( const char* )
// -------------------------------------------------------------------------- //
TFileLog::TFileLog( const char* inFilePath )
{
	mFile = ::fopen( inFilePath, "a" );;
	if (mFile == NULL)
	{
		(void) ::fprintf(
					stderr,
					"Couldn't open file %s for writing (%i)\n",
					inFilePath,
					errno );
	}
}

// -------------------------------------------------------------------------- //
//  * ~TFileLog( void )
// -------------------------------------------------------------------------- //
TFileLog::~TFileLog( void )
{
	if (mFile)
	{
		(void) ::fclose( mFile );
	}
}

// -------------------------------------------------------------------------- //
//  * DoLogLine( const char* )
// -------------------------------------------------------------------------- //
void
TFileLog::DoLogLine( const char* inLine )
{
	(void) ::fprintf( mFile, "%s\n", inLine );
	(void) ::fflush( mFile );
}

// ========================================================================= //
// The net is like a vast sea of lutefisk with tiny dinosaur brains embedded //
// in it here and there. Any given spoonful will likely have an IQ of 1, but //
// occasional spoonfuls may have an IQ more than six times that!             //
//         -- James 'Kibo' Parry                                             //
// ========================================================================= //
