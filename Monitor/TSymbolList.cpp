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

#include "TSymbolList.h"

#include "UDisasm.h"

#include <stdlib.h>
#include <string.h>

#if RASPBERRY_PI || TARGET_OS_LINUX
#include <string.h>
#endif

TSymbolList *TSymbolList::List = 0;

// -------------------------------------------------------------------------- //
// Local functions
// -------------------------------------------------------------------------- //

int symbolAddressCompare( const void * a, const void * b )
{
	int r = 0;

	KUInt32 x = *(KUInt32 *)a;
	KUInt32 y = *(KUInt32 *)b;
	
	if ( x > y )
	{
		r = 1;
	}
	else if ( x < y )
	{
		r = -1;
	}

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
		mSymbolCapacity( 0 ),
		mFile( nil )
{
	// Load symbols and keep them in memory to have fast access when searching.
	
	mFile = ::fopen( inPath, "r" );
	
	if (mFile == NULL)
	{
		(void) ::fprintf( stderr, "Cannot open symbol files '%s'\n", inPath );
	}
	else
	{
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
	if ( mSymbolOffsets )
	{
		for ( int i = 0; i < mSymbolCount; i++ )
		{
			struct SSymbolStruct* s = mSymbolOffsets + i;
			
			if ( s->fName )
				::free(s->fName);
			
			if ( s->fComment )
				::free(s->fComment);
		}
		
		::free( mSymbolOffsets );
	}
}


// -------------------------------------------------------------------------- //
//  * AddSymbol(KUInt32 inAddress, char* inSymbol, char* inComment)
// -------------------------------------------------------------------------- //
void
TSymbolList::AddSymbol(KUInt32 inAddress, const char* inSymbol, const char* inComment)
{
	mSymbolOffsets[mSymbolCount].fName = strdup(inSymbol);
	mSymbolOffsets[mSymbolCount].fAddress = inAddress;
	
	if ( inComment )
	{
		mSymbolOffsets[mSymbolCount].fComment = strdup(inComment);
	}
	
	mSymbolCount++;
	
	if ( mSymbolCount == mSymbolCapacity )
	{
		mSymbolCapacity += 0x4000;	// + 16k
		
		mSymbolOffsets = (SSymbolStruct *)::realloc(
				  mSymbolOffsets,
				  sizeof(SSymbolStruct) * mSymbolCapacity );
	}
}


// -------------------------------------------------------------------------- //
//  * LoadSymbols( void )
// -------------------------------------------------------------------------- //
void
TSymbolList::LoadSymbols( void )
{
	mSymbolCapacity = 0x20000;	// 128 k entries
	mSymbolCount = 0;
	mSymbolOffsets = (SSymbolStruct*) ::malloc( sizeof(SSymbolStruct) * mSymbolCapacity );
	
	static const char *pattern = "%x";
	fgetc(mFile);
	
	if ( fgetc(mFile) == 'x' )
		pattern = "0x%x";
		
	fseek(mFile, 0, SEEK_SET);
  
	while ( ::fscanf( mFile, pattern, (int*) &mSymbolOffsets[mSymbolCount].fAddress ) )
	{
		char prevSym[512];
		prevSym[0] = '\0';
		
		int theChar = fgetc( mFile );
		if ( theChar == '\t' || theChar == ' ' )
		{
			char sym[512], cmt[512];
			ReadSymbolData( mFile, sym, cmt);

			if ( this->GetSymbolByAddress(mSymbolOffsets[mSymbolCount].fAddress, prevSym) )
			{
#if 0
				KPrintf("Warning: redefining symbol at %08X (was: %s, now: %s)\n", mSymbolOffsets[mSymbolCount].fAddress, prevSym, sym);
#endif
			}

			if ( sym[0] )
			{
				mSymbolOffsets[mSymbolCount].fName = strdup(sym);
			}
			else
			{
				mSymbolOffsets[mSymbolCount].fName = NULL;
			}
			
			if ( cmt[0] )
			{
				mSymbolOffsets[mSymbolCount].fComment = strdup(cmt);
			}
			else
			{
				mSymbolOffsets[mSymbolCount].fComment = NULL;
			}

			//printf("Symbol: %08X %s %s\n", mSymbolOffsets[mSymbolCount].fSymbolValue, mSymbolOffsets[mSymbolCount].fSymbol ? mSymbolOffsets[mSymbolCount].fSymbol : "", mSymbolOffsets[mSymbolCount].fComment ? mSymbolOffsets[mSymbolCount].fComment : "");

			mSymbolCount++;
			
			// Let's look for the next line
			do
			{
				theChar = fgetc( mFile );
			} while ( (theChar != EOF) && (theChar != '\n') && (theChar != '\r') );
			
			if ( theChar == EOF )
			{
				break;
			}
			
			if ( mSymbolCount == mSymbolCapacity )
			{
				mSymbolCapacity += 0x4000;	// + 16k
				mSymbolOffsets = (SSymbolStruct *)
									::realloc(
										mSymbolOffsets,
										sizeof(SSymbolStruct) * mSymbolCapacity );
			}
		}
		else if ( theChar == EOF )
		{
			break;
		}
		else
		{
			(void) ::fprintf( stderr,
				"Symbol list failed at line %i, last value read is %.8X, read %.2X\n", 
				(int) mSymbolCount, (unsigned int) mSymbolOffsets[mSymbolCount].fAddress, theChar );
			mSymbolCount = 0;
			break;
		}
	}
/*
	// FIXME: this is only true for the debugging ROM
	AddSymbol( 0x00018450, "Unnamed_00018450");
	AddSymbol( 0x00018B78, "Unnamed_00018B78");
	AddSymbol( 0x00019988, "Unnamed_00019988");
	AddSymbol( 0x00019C64, "Unnamed_00019C64");
	AddSymbol( 0x00019CA4, "Unnamed_00019CA4");
	AddSymbol( 0x00019E9C, "Unnamed_00019E9C");
	AddSymbol( 0x00019FDC, "Unnamed_00019FDC");
	AddSymbol( 0x0001A02C, "Unnamed_0001A02C");
	AddSymbol( 0x0001E934, "Unnamed_0001E934");
	AddSymbol( 0x00029C88, "Unnamed_00029C88");
	AddSymbol( 0x0002C3F8, "Unnamed_0002C3F8");
	AddSymbol( 0x0002C41C, "Unnamed_0002C41C");
	AddSymbol( 0x0002C440, "Unnamed_0002C440");
	AddSymbol( 0x0003E45C, "Unnamed_0003E45C");
	AddSymbol( 0x000565A4, "Unnamed_000565A4");
	AddSymbol( 0x000565B4, "Unnamed_000565B4");
	AddSymbol( 0x000565F8, "Unnamed_000565F8");
	AddSymbol( 0x00069A04, "Unnamed_00069A04");
	AddSymbol( 0x000790B0, "Unnamed_000790B0");
	AddSymbol( 0x0008F62C, "Unnamed_0008F62C");
	AddSymbol( 0x000A0598, "Unnamed_000A0598");
	AddSymbol( 0x000B4324, "Unnamed_000B4324");
	AddSymbol( 0x000B43A4, "Unnamed_000B43A4");
	AddSymbol( 0x000BD6A0, "Unnamed_000BD6A0");
	AddSymbol( 0x000BDCAC, "Unnamed_000BDCAC");
	AddSymbol( 0x001442A0, "Unnamed_001442A0");
	AddSymbol( 0x00144470, "Unnamed_00144470");
	AddSymbol( 0x0014E19C, "Unnamed_0014E19C");
	AddSymbol( 0x00155F28, "Unnamed_00155F28");
	AddSymbol( 0x001621EC, "Unnamed_001621EC");
	AddSymbol( 0x00162894, "Unnamed_00162894");
	AddSymbol( 0x0016F954, "Unnamed_0016F954");
	AddSymbol( 0x0016F9B0, "Unnamed_0016F9B0");
	AddSymbol( 0x0016FBA8, "Unnamed_0016FBA8");
	AddSymbol( 0x0016FCC8, "Unnamed_0016FCC8");
	AddSymbol( 0x0016FD7C, "Unnamed_0016FD7C");
	AddSymbol( 0x00170030, "Unnamed_00170030");
	AddSymbol( 0x00170064, "Unnamed_00170064");
	AddSymbol( 0x00170E90, "Unnamed_00170E90");
	AddSymbol( 0x001B7B80, "Unnamed_001B7B80");
	AddSymbol( 0x001B7FC8, "Unnamed_001B7FC8");
	AddSymbol( 0x001F5D10, "Unnamed_001F5D10");
	AddSymbol( 0x0020E3F8, "Unnamed_0020E3F8");
	AddSymbol( 0x00243240, "Unnamed_00243240");
	AddSymbol( 0x00243354, "Unnamed_00243354");
	AddSymbol( 0x00243840, "Unnamed_00243840");
	AddSymbol( 0x0024D800, "Unnamed_0024D800");
	AddSymbol( 0x00285508, "Unnamed_00285508");
	AddSymbol( 0x00285554, "Unnamed_00285554");
	AddSymbol( 0x0028564C, "Unnamed_0028564C");
	AddSymbol( 0x002858B8, "Unnamed_002858B8");
	AddSymbol( 0x00285960, "Unnamed_00285960");
	AddSymbol( 0x00285BC8, "Unnamed_00285BC8");
	AddSymbol( 0x00286CE8, "Unnamed_00286CE8");
	AddSymbol( 0x00286D54, "Unnamed_00286D54");
	AddSymbol( 0x00286E24, "Unnamed_00286E24");
	AddSymbol( 0x00286F20, "Unnamed_00286F20");
	AddSymbol( 0x00286FB4, "Unnamed_00286FB4");
	AddSymbol( 0x002878C8, "Unnamed_002878C8");
	AddSymbol( 0x00287A44, "Unnamed_00287A44");
	AddSymbol( 0x00287AC0, "Unnamed_00287AC0");
	AddSymbol( 0x00287B14, "Unnamed_00287B14");
	AddSymbol( 0x00287C18, "Unnamed_00287C18");
	AddSymbol( 0x00287F34, "Unnamed_00287F34");
	AddSymbol( 0x00289A0C, "Unnamed_00289A0C");
	AddSymbol( 0x0028A8FC, "Unnamed_0028A8FC");
	AddSymbol( 0x0028AA5C, "Unnamed_0028AA5C");
	AddSymbol( 0x0028AD84, "Unnamed_0028AD84");
	AddSymbol( 0x0028B1F4, "Unnamed_0028B1F4");
	AddSymbol( 0x0028B44C, "Unnamed_0028B44C");
	AddSymbol( 0x0028B514, "Unnamed_0028B514");
	AddSymbol( 0x0028BCA8, "Unnamed_0028BCA8");
	AddSymbol( 0x0028C158, "Unnamed_0028C158");
	AddSymbol( 0x0028C704, "Unnamed_0028C704");
	AddSymbol( 0x0028C9A4, "Unnamed_0028C9A4");
	AddSymbol( 0x0028D260, "Unnamed_0028D260");
	AddSymbol( 0x0028D3FC, "Unnamed_0028D3FC");
	AddSymbol( 0x0028D4B0, "Unnamed_0028D4B0");
	AddSymbol( 0x0028D4D4, "Unnamed_0028D4D4");
	AddSymbol( 0x0028D50C, "Unnamed_0028D50C");
	AddSymbol( 0x0028D558, "Unnamed_0028D558");
	AddSymbol( 0x0028D71C, "Unnamed_0028D71C");
	AddSymbol( 0x0028DB48, "Unnamed_0028DB48");
	AddSymbol( 0x0028E09C, "Unnamed_0028E09C");
	AddSymbol( 0x0028E140, "Unnamed_0028E140");
	AddSymbol( 0x0028E224, "Unnamed_0028E224");
	AddSymbol( 0x0028E2CC, "Unnamed_0028E2CC");
	AddSymbol( 0x0028E394, "Unnamed_0028E394");
	AddSymbol( 0x0028E588, "Unnamed_0028E588");
	AddSymbol( 0x0028E7A0, "Unnamed_0028E7A0");
	AddSymbol( 0x0028EA4C, "Unnamed_0028EA4C");
	AddSymbol( 0x0028EAD0, "Unnamed_0028EAD0");
	AddSymbol( 0x0028F17C, "Unnamed_0028F17C");
	AddSymbol( 0x0028FFE8, "Unnamed_0028FFE8");
	AddSymbol( 0x00290668, "Unnamed_00290668");
	AddSymbol( 0x0029082C, "Unnamed_0029082C");
	AddSymbol( 0x002909F0, "Unnamed_002909F0");
	AddSymbol( 0x00290C68, "Unnamed_00290C68");
	AddSymbol( 0x00290DE8, "Unnamed_00290DE8");
	AddSymbol( 0x00290E2C, "Unnamed_00290E2C");
	AddSymbol( 0x00291060, "Unnamed_00291060");
	AddSymbol( 0x002914B4, "Unnamed_002914B4");
	AddSymbol( 0x0029188C, "Unnamed_0029188C");
	AddSymbol( 0x00291E08, "Unnamed_00291E08");
	AddSymbol( 0x00292014, "Unnamed_00292014");
	AddSymbol( 0x00292184, "Unnamed_00292184");
	AddSymbol( 0x002924B4, "Unnamed_002924B4");
	AddSymbol( 0x002926A0, "Unnamed_002926A0");
	AddSymbol( 0x00292AE8, "Unnamed_00292AE8");
	AddSymbol( 0x00292C60, "Unnamed_00292C60");
	AddSymbol( 0x00292DF0, "Unnamed_00292DF0");
	AddSymbol( 0x00292EB4, "Unnamed_00292EB4");
	AddSymbol( 0x002930E4, "Unnamed_002930E4");
	AddSymbol( 0x002931FC, "Unnamed_002931FC");
	AddSymbol( 0x00293324, "Unnamed_00293324");
	AddSymbol( 0x00293458, "Unnamed_00293458");
	AddSymbol( 0x00293710, "Unnamed_00293710");
	AddSymbol( 0x002939D4, "Unnamed_002939D4");
	AddSymbol( 0x00293AE8, "Unnamed_00293AE8");
	AddSymbol( 0x00293C10, "Unnamed_00293C10");
	AddSymbol( 0x00293D2C, "Unnamed_00293D2C");
	AddSymbol( 0x00293E1C, "Unnamed_00293E1C");
	AddSymbol( 0x00293F7C, "Unnamed_00293F7C");
	AddSymbol( 0x00294134, "Unnamed_00294134");
	AddSymbol( 0x00294450, "Unnamed_00294450");
	AddSymbol( 0x002944B0, "Unnamed_002944B0");
	AddSymbol( 0x002946D8, "Unnamed_002946D8");
	AddSymbol( 0x002949E0, "Unnamed_002949E0");
	AddSymbol( 0x00294B84, "Unnamed_00294B84");
	AddSymbol( 0x00294D78, "Unnamed_00294D78");
	AddSymbol( 0x00294F24, "Unnamed_00294F24");
	AddSymbol( 0x00294F7C, "Unnamed_00294F7C");
	AddSymbol( 0x00294FD0, "Unnamed_00294FD0");
	AddSymbol( 0x002950D4, "Unnamed_002950D4");
	AddSymbol( 0x002953F8, "Unnamed_002953F8");
	AddSymbol( 0x002958CC, "Unnamed_002958CC");
	AddSymbol( 0x00295AE8, "Unnamed_00295AE8");
	AddSymbol( 0x00295B38, "Unnamed_00295B38");
	AddSymbol( 0x00295BC4, "Unnamed_00295BC4");
	AddSymbol( 0x00295D60, "Unnamed_00295D60");
	AddSymbol( 0x002960D8, "Unnamed_002960D8");
	AddSymbol( 0x0029634C, "Unnamed_0029634C");
	AddSymbol( 0x00296470, "Unnamed_00296470");
	AddSymbol( 0x0029698C, "Unnamed_0029698C");
	AddSymbol( 0x00296BEC, "Unnamed_00296BEC");
	AddSymbol( 0x00297438, "Unnamed_00297438");
	AddSymbol( 0x002976E4, "Unnamed_002976E4");
	AddSymbol( 0x00297860, "Unnamed_00297860");
	AddSymbol( 0x00298528, "Unnamed_00298528");
	AddSymbol( 0x00298608, "Unnamed_00298608");
	AddSymbol( 0x0029867C, "Unnamed_0029867C");
	AddSymbol( 0x00298814, "Unnamed_00298814");
	AddSymbol( 0x00298B04, "Unnamed_00298B04");
	AddSymbol( 0x00298D2C, "Unnamed_00298D2C");
	AddSymbol( 0x00299E40, "Unnamed_00299E40");
	AddSymbol( 0x0029A150, "Unnamed_0029A150");
	AddSymbol( 0x0029A8A8, "Unnamed_0029A8A8");
	AddSymbol( 0x0029B9FC, "Unnamed_0029B9FC");
	AddSymbol( 0x0029BB44, "Unnamed_0029BB44");
	AddSymbol( 0x0029CCD4, "Unnamed_0029CCD4");
	AddSymbol( 0x0029CE20, "Unnamed_0029CE20");
	AddSymbol( 0x0029D428, "Unnamed_0029D428");
	AddSymbol( 0x0029D808, "Unnamed_0029D808");
	AddSymbol( 0x0029D900, "Unnamed_0029D900");
	AddSymbol( 0x0029DBD8, "Unnamed_0029DBD8");
	AddSymbol( 0x0029DD6C, "Unnamed_0029DD6C");
	AddSymbol( 0x0029E048, "Unnamed_0029E048");
	AddSymbol( 0x0029E30C, "Unnamed_0029E30C");
	AddSymbol( 0x0029E530, "Unnamed_0029E530");
	AddSymbol( 0x0029E6BC, "Unnamed_0029E6BC");
	AddSymbol( 0x0029E888, "Unnamed_0029E888");
	AddSymbol( 0x0029EAAC, "Unnamed_0029EAAC");
	AddSymbol( 0x0029EEB4, "Unnamed_0029EEB4");
	AddSymbol( 0x0029FBCC, "Unnamed_0029FBCC");
	AddSymbol( 0x0029FFC8, "Unnamed_0029FFC8");
	AddSymbol( 0x002A01DC, "Unnamed_002A01DC");
	AddSymbol( 0x002A0740, "Unnamed_002A0740");
	AddSymbol( 0x002A09B0, "Unnamed_002A09B0");
	AddSymbol( 0x002A0B38, "Unnamed_002A0B38");
	AddSymbol( 0x002A0D14, "Unnamed_002A0D14");
	AddSymbol( 0x002A0D74, "Unnamed_002A0D74");
	AddSymbol( 0x002A1320, "Unnamed_002A1320");
	AddSymbol( 0x002A19EC, "Unnamed_002A19EC");
	AddSymbol( 0x002A1A98, "Unnamed_002A1A98");
	AddSymbol( 0x002A1E98, "Unnamed_002A1E98");
	AddSymbol( 0x002A2078, "Unnamed_002A2078");
	AddSymbol( 0x002A2758, "Unnamed_002A2758");
	AddSymbol( 0x002A2954, "Unnamed_002A2954");
	AddSymbol( 0x002A3608, "Unnamed_002A3608");
	AddSymbol( 0x002A3838, "Unnamed_002A3838");
	AddSymbol( 0x002A3898, "Unnamed_002A3898");
	AddSymbol( 0x002A3AD8, "Unnamed_002A3AD8");
	AddSymbol( 0x002A3B8C, "Unnamed_002A3B8C");
	AddSymbol( 0x002A3DB4, "Unnamed_002A3DB4");
	AddSymbol( 0x002A40A0, "Unnamed_002A40A0");
	AddSymbol( 0x002A4608, "Unnamed_002A4608");
	AddSymbol( 0x002A46FC, "Unnamed_002A46FC");
	AddSymbol( 0x002A480C, "Unnamed_002A480C");
	AddSymbol( 0x002A4978, "Unnamed_002A4978");
	AddSymbol( 0x002A4A34, "Unnamed_002A4A34");
	AddSymbol( 0x002A4C04, "Unnamed_002A4C04");
	AddSymbol( 0x002A5414, "Unnamed_002A5414");
	AddSymbol( 0x002A5574, "Unnamed_002A5574");
	AddSymbol( 0x002A55BC, "Unnamed_002A55BC");
	AddSymbol( 0x002A7078, "Unnamed_002A7078");
	AddSymbol( 0x002A70EC, "Unnamed_002A70EC");
	AddSymbol( 0x002A713C, "Unnamed_002A713C");
	AddSymbol( 0x002A7168, "Unnamed_002A7168");
	AddSymbol( 0x002A7EAC, "Unnamed_002A7EAC");
	AddSymbol( 0x002A83F8, "Unnamed_002A83F8");
	AddSymbol( 0x002D0DD0, "Unnamed_002D0DD0");
	AddSymbol( 0x002D0FD8, "Unnamed_002D0FD8");
	AddSymbol( 0x002D435C, "Unnamed_002D435C");
	AddSymbol( 0x002D48CC, "Unnamed_002D48CC");
	AddSymbol( 0x002D89B0, "Unnamed_002D89B0");
	AddSymbol( 0x002D8A1C, "Unnamed_002D8A1C");
	AddSymbol( 0x002D8AB4, "Unnamed_002D8AB4");
	AddSymbol( 0x002D8AFC, "Unnamed_002D8AFC");
	AddSymbol( 0x002D94DC, "Unnamed_002D94DC");
	AddSymbol( 0x002FEC04, "Unnamed_002FEC04");
	AddSymbol( 0x002FF190, "Unnamed_002FF190");
	AddSymbol( 0x0030660C, "Unnamed_0030660C");
	AddSymbol( 0x00306660, "Unnamed_00306660");
	AddSymbol( 0x00309850, "Unnamed_00309850");
	AddSymbol( 0x00309DA0, "Unnamed_00309DA0");
	AddSymbol( 0x0030A054, "Unnamed_0030A054");
	AddSymbol( 0x0030A0E4, "Unnamed_0030A0E4");
	AddSymbol( 0x00310FFC, "Unnamed_00310FFC");
	AddSymbol( 0x00319030, "Unnamed_00319030");
	AddSymbol( 0x00319074, "Unnamed_00319074");
	AddSymbol( 0x00336AA4, "Unnamed_00336AA4");
	AddSymbol( 0x0033D7F8, "Unnamed_0033D7F8");
	AddSymbol( 0x0033E770, "Unnamed_0033E770");
	AddSymbol( 0x0033E808, "Unnamed_0033E808");
	AddSymbol( 0x0033E950, "Unnamed_0033E950");
	AddSymbol( 0x003444A4, "Unnamed_003444A4");
	AddSymbol( 0x003445CC, "Unnamed_003445CC");
	AddSymbol( 0x003448C4, "Unnamed_003448C4");
	AddSymbol( 0x00344950, "Unnamed_00344950");
	AddSymbol( 0x00344AF4, "Unnamed_00344AF4");
	AddSymbol( 0x00344BD0, "Unnamed_00344BD0");
	AddSymbol( 0x00345220, "Unnamed_00345220");
	AddSymbol( 0x003457C4, "Unnamed_003457C4");
	AddSymbol( 0x00345854, "Unnamed_00345854");
	AddSymbol( 0x00345C10, "Unnamed_00345C10");
	AddSymbol( 0x00345CD4, "Unnamed_00345CD4");
	AddSymbol( 0x00346410, "Unnamed_00346410");
	AddSymbol( 0x003469BC, "Unnamed_003469BC");
	AddSymbol( 0x00346A70, "Unnamed_00346A70");
	AddSymbol( 0x00346AE4, "Unnamed_00346AE4");
	AddSymbol( 0x00346B98, "Unnamed_00346B98");
	AddSymbol( 0x00346BC0, "Unnamed_00346BC0");
	AddSymbol( 0x00346CA4, "Unnamed_00346CA4");
	AddSymbol( 0x00346D90, "Unnamed_00346D90");
	AddSymbol( 0x0034EFBC, "Unnamed_0034EFBC");
	AddSymbol( 0x0034EFCC, "Unnamed_0034EFCC");
	AddSymbol( 0x0034F054, "Unnamed_0034F054");
	AddSymbol( 0x0034F9D0, "Unnamed_0034F9D0");
	AddSymbol( 0x0034FAA8, "Unnamed_0034FAA8");
	AddSymbol( 0x0034FAE8, "Unnamed_0034FAE8");
	AddSymbol( 0x0034FEB4, "Unnamed_0034FEB4");
	AddSymbol( 0x00350560, "Unnamed_00350560");
	AddSymbol( 0x00352FD8, "Unnamed_00352FD8");
	AddSymbol( 0x00353184, "Unnamed_00353184");
	AddSymbol( 0x0035FE60, "Unnamed_0035FE60");
	AddSymbol( 0x00382300, "Unnamed_00382300");
	AddSymbol( 0x00382984, "Unnamed_00382984");
	AddSymbol( 0x003829F8, "Unnamed_003829F8");
	AddSymbol( 0x00383430, "Unnamed_00383430");
	AddSymbol( 0x00383764, "Unnamed_00383764");
	AddSymbol( 0x00383938, "Unnamed_00383938");
	AddSymbol( 0x00383AA8, "Unnamed_00383AA8");
	AddSymbol( 0x00383B90, "Unnamed_00383B90");
	AddSymbol( 0x00383C58, "Unnamed_00383C58");
	AddSymbol( 0x00384090, "Unnamed_00384090");
	AddSymbol( 0x00384AF8, "Unnamed_00384AF8");
	AddSymbol( 0x00384E18, "Unnamed_00384E18");
	AddSymbol( 0x00385048, "Unnamed_00385048");
	AddSymbol( 0x00385414, "Unnamed_00385414");
	AddSymbol( 0x00385568, "Unnamed_00385568");
	AddSymbol( 0x0038579C, "Unnamed_0038579C");
	AddSymbol( 0x00385808, "Unnamed_00385808");
	AddSymbol( 0x00385AFC, "Unnamed_00385AFC");
	AddSymbol( 0x00385B74, "Unnamed_00385B74");
	AddSymbol( 0x00385D80, "Unnamed_00385D80");
	AddSymbol( 0x00385F04, "Unnamed_00385F04");
	AddSymbol( 0x00385F4C, "Unnamed_00385F4C");
	AddSymbol( 0x00386208, "Unnamed_00386208");
	AddSymbol( 0x00386344, "Unnamed_00386344");
	AddSymbol( 0x00386464, "Unnamed_00386464");
	AddSymbol( 0x003869F8, "Unnamed_003869F8");
	AddSymbol( 0x00386C48, "Unnamed_00386C48");
	AddSymbol( 0x00386C90, "Unnamed_00386C90");
	AddSymbol( 0x003872C8, "Unnamed_003872C8");
	AddSymbol( 0x00387344, "Unnamed_00387344");
	AddSymbol( 0x003873EC, "Unnamed_003873EC");
	AddSymbol( 0x00387AB4, "Unnamed_00387AB4");
	AddSymbol( 0x00387DE0, "Unnamed_00387DE0");
	AddSymbol( 0x00388BB8, "Unnamed_00388BB8");
	AddSymbol( 0x00388D94, "Unnamed_00388D94");
	AddSymbol( 0x00389088, "Unnamed_00389088");
	AddSymbol( 0x00389340, "Unnamed_00389340");
	AddSymbol( 0x00389548, "Unnamed_00389548");
	AddSymbol( 0x003895C8, "Unnamed_003895C8");
	AddSymbol( 0x0038965C, "Unnamed_0038965C");
	AddSymbol( 0x003896D4, "Unnamed_003896D4");
	AddSymbol( 0x00389C3C, "Unnamed_00389C3C");
	AddSymbol( 0x00389D8C, "Unnamed_00389D8C");
	AddSymbol( 0x00389E1C, "Unnamed_00389E1C");
	AddSymbol( 0x00389F00, "Unnamed_00389F00");
	AddSymbol( 0x00389F6C, "Unnamed_00389F6C");
	AddSymbol( 0x0038A978, "Unnamed_0038A978");
	AddSymbol( 0x0038A9B8, "Unnamed_0038A9B8");
	AddSymbol( 0x0038A9F8, "Unnamed_0038A9F8");
	AddSymbol( 0x0038AA38, "Unnamed_0038AA38");
	AddSymbol( 0x0038AA4C, "Unnamed_0038AA4C");
	AddSymbol( 0x0038AA58, "Unnamed_0038AA58");
	AddSymbol( 0x0038AA68, "Unnamed_0038AA68");
	AddSymbol( 0x0038AA78, "Unnamed_0038AA78");
	AddSymbol( 0x0038AA88, "Unnamed_0038AA88");
	AddSymbol( 0x0038AA9C, "Unnamed_0038AA9C");
	AddSymbol( 0x0038AAAC, "Unnamed_0038AAAC");
	AddSymbol( 0x0038AAFC, "Unnamed_0038AAFC");
	AddSymbol( 0x0038AB0C, "Unnamed_0038AB0C");
	AddSymbol( 0x0038AB14, "Unnamed_0038AB14");
	AddSymbol( 0x0038AB1C, "Unnamed_0038AB1C");
	AddSymbol( 0x0038AB28, "Unnamed_0038AB28");
	AddSymbol( 0x0038AB44, "Unnamed_0038AB44");
	AddSymbol( 0x0038AB60, "Unnamed_0038AB60");
	AddSymbol( 0x0038AB64, "Unnamed_0038AB64");
	AddSymbol( 0x0038AB68, "Unnamed_0038AB68");
	AddSymbol( 0x0038AB6C, "Unnamed_0038AB6C");
	AddSymbol( 0x0038AB7C, "Unnamed_0038AB7C");
	AddSymbol( 0x0038AB8C, "Unnamed_0038AB8C");
	AddSymbol( 0x0038AB9C, "Unnamed_0038AB9C");
	AddSymbol( 0x0038ABB8, "Unnamed_0038ABB8");
	AddSymbol( 0x0038ABD8, "Unnamed_0038ABD8");
	AddSymbol( 0x0038C878, "Unnamed_0038C878");
	AddSymbol( 0x0038C87C, "Unnamed_0038C87C");
	AddSymbol( 0x0038F04C, "Unnamed_0038F04C");
	AddSymbol( 0x0038F198, "Unnamed_0038F198");
	AddSymbol( 0x0038F2FC, "Unnamed_0038F2FC");
	AddSymbol( 0x0038F51C, "Unnamed_0038F51C");
	AddSymbol( 0x0038FA24, "Unnamed_0038FA24");
	AddSymbol( 0x0038FA68, "Unnamed_0038FA68");
	AddSymbol( 0x0038FF8C, "Unnamed_0038FF8C");
	AddSymbol( 0x0038FFD8, "Unnamed_0038FFD8");
	AddSymbol( 0x0038FFE4, "Unnamed_0038FFE4");
	AddSymbol( 0x00390028, "Unnamed_00390028");
	AddSymbol( 0x00390074, "Unnamed_00390074");
	AddSymbol( 0x003901C0, "Unnamed_003901C0");
	AddSymbol( 0x00390374, "Unnamed_00390374");
	AddSymbol( 0x003906C0, "Unnamed_003906C0");
	AddSymbol( 0x0039070C, "Unnamed_0039070C");
	AddSymbol( 0x003907B8, "Unnamed_003907B8");
	AddSymbol( 0x00390D74, "Unnamed_00390D74");
	AddSymbol( 0x00390D98, "Unnamed_00390D98");
	AddSymbol( 0x00390ED0, "Unnamed_00390ED0");
	AddSymbol( 0x00391CAC, "Unnamed_00391CAC");
	AddSymbol( 0x00392630, "Unnamed_00392630");
	AddSymbol( 0x00392704, "Unnamed_00392704");
	AddSymbol( 0x0039291C, "Unnamed_0039291C");
	AddSymbol( 0x00392924, "Unnamed_00392924");
	AddSymbol( 0x0039292C, "Unnamed_0039292C");
	AddSymbol( 0x00392934, "Unnamed_00392934");
	AddSymbol( 0x0039293C, "Unnamed_0039293C");
	AddSymbol( 0x00392944, "Unnamed_00392944");
	AddSymbol( 0x0039294C, "Unnamed_0039294C");
	AddSymbol( 0x00393710, "Unnamed_00393710");
*/
	qsort(mSymbolOffsets, mSymbolCount, sizeof(struct SSymbolStruct), QSortCallback);
	
	(void) ::fprintf( stderr, "Read %i symbols\n", (int) mSymbolCount );
}


// -------------------------------------------------------------------------- //
//  * QSortCallback(const void*, const void*)
// -------------------------------------------------------------------------- //
int TSymbolList::QSortCallback(const void *a, const void *b)
{
	struct SSymbolStruct* sa = (struct SSymbolStruct*)a;
	struct SSymbolStruct* sb = (struct SSymbolStruct*)b;
	
	if ( sa->fAddress < sb->fAddress )
		return -1;
	
	if ( sa->fAddress > sb->fAddress )
		return 1;
	
	return 0;
}


// -------------------------------------------------------------------------- //
//  * CopySymbolStrings( SSymbolStruct symbol, char* outSymbol, char* outComment )
// -------------------------------------------------------------------------- //
void
TSymbolList::CopySymbolStrings(
							SSymbolStruct *symbol,
							char* outSymbol,
							char* outComment)
{
	if ( outSymbol )
	{
		if ( symbol->fName )
		{
			strcpy(outSymbol, symbol->fName);
		}
		else
		{
			*outSymbol = '\0';
		}
	}
	
	if ( outComment )
	{
		if ( symbol->fComment )
		{
			strcpy(outComment, symbol->fComment);
		}
		else
		{
			*outComment = '\0';
		}
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
		
		if ( (theChar != EOF) && (theChar != '\t') && (cursor < 510) && (theChar != '\n') && (theChar != '\r') )
		{
			outSymbol[cursor] = (char)theChar;
		}
		else
		{
			outSymbol[cursor] = '\0';
			break;
		}
		
		cursor++;
	} while (1);

	if ( theChar != '\t' )
	{
		outComment[0] = '\0';
	}
	else
	{
		cursor = 0;

		// Now I fill the comment

		do
		{
			theChar = ::fgetc( inFile );

			if ( (theChar != EOF) && (theChar != '\n') && (theChar != '\r') && (cursor < 510) )
			{
				outComment[cursor] = (char)theChar;
			}
			else
			{
				outComment[cursor] = '\0';
				break;
			}
			
			cursor++;
		} while (1);
	}
	
	if ( theChar != EOF )
		::ungetc(theChar, inFile);
}

// -------------------------------------------------------------------------- //
//  * GetNearestSymbolByAddress( KUInt32, char*, char*, int* )
// -------------------------------------------------------------------------- //
void
TSymbolList::GetNearestSymbolByAddress(
				KUInt32 inAddress,
				char* outSymbol,
				char* outComment,
				int* outOffset )
{
	if ( outSymbol )
		*outSymbol = '\0';
	
	if ( outComment )
		*outComment = '\0';
	
	if ( outOffset )
		*outOffset = 0;
	
	if ( mSymbolCount == 0 )
	{
		::sprintf(outComment, "(no symbol data)");
		return;
	}
		
	// Let's look for the symbol.
	// Strings should have a size of 510 bytes plus the terminator (full ANSI C strings).
	
	unsigned long int theNextSymbolAddress = mSymbolOffsets[0].fAddress;
	
	for ( KUInt32 indexSymbols = 0; indexSymbols < mSymbolCount; indexSymbols++ )
	{
		unsigned long int theSymbolAddress = theNextSymbolAddress;
		
		if ( indexSymbols < mSymbolCount - 1 )
		{
			theNextSymbolAddress = mSymbolOffsets[indexSymbols + 1].fAddress;
		}
		else
		{
			theNextSymbolAddress = kNoSymbol;
		}
		
		if ( (inAddress >= theSymbolAddress) && (inAddress < theNextSymbolAddress) )
		{
			CopySymbolStrings(&mSymbolOffsets[indexSymbols], outSymbol, outComment);
			
			if ( outOffset )
			{
				*outOffset = (int)(inAddress - theSymbolAddress);
			}
			break;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * GetSymbolByAddress( KUInt32, char*, char*, int* )
// -------------------------------------------------------------------------- //
bool
TSymbolList::GetSymbolByAddress(
				KUInt32 inAddress,
				char* outSymbol,
				char* outComment,
				int* outOffset )
{
	bool found = false;
	
	SSymbolStruct *symbol = (SSymbolStruct *)bsearch(&inAddress, mSymbolOffsets,
								mSymbolCount, sizeof(*mSymbolOffsets),
								symbolAddressCompare);

	if ( symbol != NULL )
	{
		CopySymbolStrings(symbol, outSymbol, outComment);
		
		if ( outOffset )
			*outOffset = inAddress - symbol->fAddress;
		
		found = true;
	}
	else
	{
		if ( outSymbol )
			::sprintf(outSymbol, "%08X", (unsigned int)inAddress);
	
		if ( outComment )
			::sprintf(outComment, "(no symbol data)");
		
		if ( outOffset )
			*outOffset = 0;
	}
	
	return found;
}


// -------------------------------------------------------------------------- //
//  * GetSymbolByName( const char* inName )
// -------------------------------------------------------------------------- //
KUInt32
TSymbolList::GetSymbolByName( const char* inName )
{
	KUInt32 outSymbolValue = kNoSymbol;
	
	if ( inName != NULL )
	{
		for ( int i = 0; i < mSymbolCount; i++ )
		{
			struct SSymbolStruct* s = mSymbolOffsets + i;
			
			if ( s->fName && ::strcmp(inName, s->fName) == 0 )
			{
				outSymbolValue = s->fAddress;
				break;
			}
		}
	}
	
	return outSymbolValue;
}


// -------------------------------------------------------------------------- //
//  * GetNextSymbol( KUInt32 inAddress )
// -------------------------------------------------------------------------- //
KUInt32
TSymbolList::GetNextSymbol( KUInt32 inAddress )
{
	KUInt32 outAddress = kNoSymbol;
	
	for ( int i = 0; i < mSymbolCount; i++ )
	{
		struct SSymbolStruct* s = mSymbolOffsets + i;
		
		if ( s->fAddress > inAddress )
		{
			outAddress = s->fAddress;
			break;
		}
	}
	
	return outAddress;
}


// ======================================================================= //
// A computer lets you make more mistakes faster than any other invention, //
// with the possible exceptions of handguns and Tequilla.                  //
//         -- Mitch Ratcliffe                                              //
// ======================================================================= //
