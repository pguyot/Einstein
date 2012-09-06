// ==============================
// File:			TSymbolList.cp
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
#include "TSymbolList.h"

#include <stdlib.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TSymbolList( const char* )
// -------------------------------------------------------------------------- //
TSymbolList::TSymbolList( const char* inPath )
	:
		mSymbolOffsets( nil ),
		mSymbolCount( 0 ),
		mFile( nil )
{
	mFile = ::fopen( inPath, "r" );
	if (mFile == NULL)
	{
		(void) ::fprintf( stderr, "Cannot open symbol files '%s'\n", inPath );
	} else {
		LoadSymbols();
	}
}

// -------------------------------------------------------------------------- //
//  * ~TSymbolList( void )
// -------------------------------------------------------------------------- //
TSymbolList::~TSymbolList( void )
{
	if (mFile)
	{
		::fclose( mFile );
	}
	if (mSymbolOffsets)
	{
		::free( mSymbolOffsets );
	}
}

// -------------------------------------------------------------------------- //
//  * LoadSymbols( void )
// -------------------------------------------------------------------------- //
void
TSymbolList::LoadSymbols( void )
{
	// I need to load the symbol table.
	KUInt32 theSymbolMax = 0x20000;	// 128 k entries
	mSymbolCount = 0;
	mSymbolOffsets = (SSymbolStruct*) ::malloc( sizeof(SSymbolStruct) * theSymbolMax );
	
	static const char *pattern = "%x";
	fgetc(mFile);
	if (fgetc(mFile)=='x') pattern = "0x%x";
	fseek(mFile, 0, SEEK_SET);
  
	while ( ::fscanf( mFile, pattern, (int*) &mSymbolOffsets[mSymbolCount].fSymbolValue ) )
	{
		int theChar = fgetc( mFile );
		if (theChar == '\t' || theChar == ' ')
		{
			
			(void) ::fgetpos( mFile, &mSymbolOffsets[mSymbolCount].fFileCursor );
			mSymbolCount++;
				
			// Let's look for the next line
			do
			{
				theChar = fgetc( mFile );
			} while ((theChar != EOF) && (theChar != '\n') && (theChar != '\r'));
			
			if (theChar == EOF)
			{
				break;
			}
			
			if (mSymbolCount == theSymbolMax)
			{
				theSymbolMax += 0x4000;	// + 16k
				mSymbolOffsets = (SSymbolStruct *)
									::realloc(
										mSymbolOffsets,
										sizeof(SSymbolStruct) * theSymbolMax );
			}
		} else if (theChar == EOF) {
			break;
		} else {
			(void) ::fprintf( stderr,
				"Symbol list failed at line %i, last value read is %.8X, read %.2X\n", 
				(int) mSymbolCount, (unsigned int) mSymbolOffsets[mSymbolCount].fSymbolValue, theChar );
			mSymbolCount = 0;
			break;
		}
	}
	
	(void) ::fprintf( stderr, "Read %i symbols\n", (int) mSymbolCount );
}

// -------------------------------------------------------------------------- //
//  * GetSymbol( KUInt32, char*, char*, int* )
// -------------------------------------------------------------------------- //
void
TSymbolList::GetSymbol(
				KUInt32 inValue,
				char* outSymbol,
				char* outComment,
				int* outOffset )
{
	if (mSymbolCount == 0)
	{
		::sprintf(outSymbol, "%08X", (unsigned int)inValue);
		::sprintf(outComment, "(no symbol data)");
		*outOffset = 0;
		return;
	}
	
	// Let's look for the symbol.
	// Strings should have a size of 510 bytes plus the terminator (full ANSI C strings).
	KUInt32 indexSymbols;
	
	unsigned long int theNextSymbolValue = mSymbolOffsets[0].fSymbolValue;
	for (indexSymbols = 0; indexSymbols < mSymbolCount; indexSymbols++)
	{
		unsigned long int theSymbolValue = theNextSymbolValue;
		if (indexSymbols < mSymbolCount - 1)
		{
			theNextSymbolValue = mSymbolOffsets[indexSymbols+1].fSymbolValue;
		} else {
			theNextSymbolValue = (unsigned long int) -1;
		}
		if ((inValue >= theSymbolValue) && (inValue < theNextSymbolValue))
		{
			int cursor = 0;
			int theChar;
			
			// Let's read the symbol in the file.
			(void) ::fsetpos( mFile, &mSymbolOffsets[indexSymbols].fFileCursor );
			// Now I fill the symbol
			do
			{
				theChar = ::fgetc( mFile );
				if ((theChar != EOF) && (theChar != '\t') && (cursor < 510) && (theChar != '\n') && (theChar != '\r'))
				{
					outSymbol[cursor] = theChar;
				} else {
					outSymbol[cursor] = '\0';
					break;
				}
				cursor++;
			} while (1);
			
			if (theChar != '\t')
			{
				outComment[0] = '\0';
			} else {
				cursor = 0;
				// Now I fill the comment
				do
				{
					theChar = ::fgetc( mFile );
					if ((theChar != EOF) && (theChar != '\n') && (theChar != '\r') && (cursor < 510))
					{
						outComment[cursor] = theChar;
					} else {
						outComment[cursor] = '\0';
						break;
					}
					cursor++;
				} while (1);
			}
			
			*outOffset = inValue - theSymbolValue;
			break;
		}
	}
}

// ======================================================================= //
// A computer lets you make more mistakes faster than any other invention, //
// with the possible exceptions of handguns and Tequilla.                  //
//         -- Mitch Ratcliffe                                              //
// ======================================================================= //
