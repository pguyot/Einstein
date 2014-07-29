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

#include "UDisasm.h"

#include <stdlib.h>

// -------------------------------------------------------------------------- //
// Local functions
// -------------------------------------------------------------------------- //

int symbolValueCompare( const void * a, const void * b )
{
	int r;
	KUInt32 x = *(KUInt32 *) a, y = *(KUInt32 *) b;
	if (x > y) r = 1; else if (x < y) r = -1; else r = 0;
	return r;
}

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
	// Load symbols and keep them in memory to have fast access when searching.
	mFile = ::fopen( inPath, "r" );
	if (mFile == NULL)
	{
		(void) ::fprintf( stderr, "Cannot open symbol files '%s'\n", inPath );
	} else {
		LoadSymbols();
		UDisasm::setSymbolList(this);
		::fclose( mFile );
	}
}

// -------------------------------------------------------------------------- //
//  * ~TSymbolList( void )
// -------------------------------------------------------------------------- //
TSymbolList::~TSymbolList( void )
{
	if (mSymbolOffsets)
	{
		int i;
		for (i=0; i<mSymbolCount; i++) {
			struct SSymbolStruct* s = mSymbolOffsets+i;
			if (s->fSymbol)
				::free(s->fSymbol);
			if (s->fComment)
				::free(s->fComment);
		}
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
			char sym[512], cmt[512];
			ReadSymbolData( mFile, sym, cmt);
			if (sym[0]) {
				mSymbolOffsets[mSymbolCount].fSymbol = strdup(sym);
			} else {
				mSymbolOffsets[mSymbolCount].fSymbol = 0L;
			}
			if (cmt[0]) {
				mSymbolOffsets[mSymbolCount].fComment = strdup(cmt);
			} else {
				mSymbolOffsets[mSymbolCount].fComment = 0L;
			}
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
//  * ReadSymbolData( SSymbolStruct symbol, char* outSymbol, char* outComment )
// -------------------------------------------------------------------------- //
void
TSymbolList::ReadSymbolData(
							SSymbolStruct *symbol,
							char* outSymbol,
							char* outComment)
{
	if (symbol->fSymbol) {
		strcpy(outSymbol, symbol->fSymbol);
	} else {
		outSymbol = 0L;
	}
	if (symbol->fComment) {
		strcpy(outComment, symbol->fComment);
	} else {
		outComment = 0L;
	}
}

// -------------------------------------------------------------------------- //
//  * ReadSymbolData( SSymbolStruct symbol, char* outSymbol, char* outComment )
// -------------------------------------------------------------------------- //
void
TSymbolList::ReadSymbolData(
	FILE *inFile,
	char* outSymbol,
	char* outComment)
{
	int cursor = 0;
	int theChar;

	do
	{
		theChar = ::fgetc( inFile );
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
			theChar = ::fgetc( inFile );
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
	if (theChar != EOF)
		::ungetc(theChar, inFile);
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
	if (this == NULL || mSymbolCount == 0)
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
			ReadSymbolData(&mSymbolOffsets[indexSymbols], outSymbol, outComment);
			*outOffset = inValue - theSymbolValue;
			break;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * GetSymbol( KUInt32, char*, char*, int* )
// -------------------------------------------------------------------------- //
bool
TSymbolList::GetSymbolExact(
				KUInt32 inValue,
				char* outSymbol,
				char* outComment,
				int* outOffset )
{
	if (this == NULL) return false;

	bool r;
	SSymbolStruct *symbol = (SSymbolStruct *) bsearch(&inValue, mSymbolOffsets,
		mSymbolCount, sizeof(*mSymbolOffsets), symbolValueCompare);

	if (symbol != NULL)
	{
		ReadSymbolData(symbol, outSymbol, outComment);
		*outOffset = inValue - symbol->fSymbolValue;
		r = true;
	} else {
		::sprintf(outSymbol, "%08X", (unsigned int)inValue);
		::sprintf(outComment, "(no symbol data)");
		*outOffset = 0;
		r = false;
	}
	return r;
}


// -------------------------------------------------------------------------- //
//  * GetSymbol( const char* inName )
// -------------------------------------------------------------------------- //
KUInt32
TSymbolList::GetSymbol( const char* inName )
{
	if (this == NULL || mSymbolCount == 0)
		return kNoSymbol;
	int i;
	for (i=0; i<mSymbolCount; i++) {
		struct SSymbolStruct* s = mSymbolOffsets+i;
		if (s->fSymbol && ::strcmp(inName, s->fSymbol)==0)
			return s->fSymbolValue;
	}
	return kNoSymbol;
}


// -------------------------------------------------------------------------- //
//  * GetNextSymbol( KUInt32 inValue )
// -------------------------------------------------------------------------- //
KUInt32
TSymbolList::GetNextSymbol( KUInt32 inValue )
{
	if (this == NULL || mSymbolCount == 0)
		return kNoSymbol;
	int i;
	for (i=0; i<mSymbolCount; i++) {
		struct SSymbolStruct* s = mSymbolOffsets+i;
		if (s->fSymbolValue > inValue)
			return s->fSymbolValue;
	}
	return kNoSymbol;
}


// ======================================================================= //
// A computer lets you make more mistakes faster than any other invention, //
// with the possible exceptions of handguns and Tequilla.                  //
//         -- Mitch Ratcliffe                                              //
// ======================================================================= //
