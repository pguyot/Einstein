// ==============================
// Fichier:			UScreenTests.cp
// Projet:			Einstein
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			13/2/2005
// Tabulation:		4 espaces
// 
// Copyright:		© 2005 by Paul Guyot.
// 					Tous droits réservés pour tous pays.
// ===========
// $Id: UScreenTests.cp 150 2006-01-10 05:55:43Z paul $
// ===========

#include <K/Defines/KDefinitions.h>
#include "UScreenTests.h"

// ANSI C & POSIX
#include <stdlib.h>
#include <stdio.h>

#if TARGET_OS_WIN32
	#include <assert.h>
#else
	#include <unistd.h>
#endif

// Einstein.
#include "Emulator/TMemory.h"
#include "Emulator/TMemoryConsts.h"
#ifndef NO_X11
#include "Emulator/Screen/TX11ScreenManager.h"
#else
#include "Emulator/Screen/TFBScreenManager.h"
#define TX11ScreenManager TFBScreenManager
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
#if TARGET_OS_WIN32
	#define kTempFlashPath "c:/EinsteinTests.flash"
#else
	#define kTempFlashPath "/tmp/EinsteinTests.flash"
#endif

// -------------------------------------------------------------------------- //
//  * TestX11( void )
// -------------------------------------------------------------------------- //
void
UScreenTests::TestX11( void )
{
#if NO_X11
	assert(0); // FIXME later
#else
	// Create some memory.
	TMemory theMem( (TLog*) NULL, (KUInt8*) NULL, kTempFlashPath );
	
	// Create the screen manager.
	TX11ScreenManager theScreenManager;
	
	theScreenManager.SetMemory( &theMem );
	
	// Open the screen.
	theScreenManager.PowerOnScreen();
	
	KUInt32 theWidth = theScreenManager.GetScreenWidth();
	KUInt32 theHeight = theScreenManager.GetScreenHeight();
	KUInt32 rowBytes = theWidth * 8 / TScreenManager::kBitsPerPixel;
	KUInt32 rowWords = rowBytes / 4;
	KUInt32 pixmapAddr = TMemoryConsts::kRAMStart;
	KUInt32 baseAddy = pixmapAddr + 0x00000100;
	
	// Create a pixmap in RAM.
	(void) theMem.WriteP( pixmapAddr + 0x00, baseAddy );			// base addy
	(void) theMem.WriteP( pixmapAddr + 0x04, rowBytes << 16 );
	(void) theMem.WriteP( pixmapAddr + 0x08, 0x00000000 );			// topleft

	// Fill it with white pixels.
	KUInt32 indexLines;
	KUInt32 cursorWords = baseAddy;
	for (indexLines = 0; indexLines < theHeight; indexLines++)
	{
		KUInt32 indexWords;
		for (indexWords = 0; indexWords < rowWords; indexWords++)
		{
			(void) theMem.WriteP( cursorWords, 0x00000000 );
			cursorWords += 4;
		}
	}
	
	// Blit.
	TScreenManager::SRect theRect;
	theRect.fTop = 0;
	theRect.fBottom = theHeight;
	theRect.fLeft = 0;
	theRect.fRight = theWidth;

	theScreenManager.Blit( pixmapAddr, &theRect, &theRect, 0 /* srcCopy */ );

	KUInt32 indexRow;
	for (indexRow = 0; indexRow < theWidth; indexRow++)
	{
		// Fill the screen with a diagonal line.
		for (indexLines = 0; indexLines < theHeight; indexLines++)
		{
			int xCoord = (indexLines + indexRow) % theWidth;
			DrawPoint( &theMem, baseAddy, rowBytes, xCoord, indexLines, indexRow % 16 );

//			theRect.fTop = indexLines;
//			theRect.fLeft = xCoord;
//			theRect.fBottom = indexLines + 1;
//			theRect.fRight = xCoord + 1;
		}	

		// Blit again.
		theScreenManager.Blit( pixmapAddr, &theRect, &theRect, 1 /* !srcCopy */ );
	}

	// Fill it with black pixels.
	cursorWords = baseAddy;
	for (indexLines = 0; indexLines < theHeight; indexLines++)
	{
		KUInt32 indexWords;
		for (indexWords = 0; indexWords < rowWords; indexWords++)
		{
			(void) theMem.WriteP( cursorWords, 0xFFFFFFFF );
			cursorWords += 4;
		}
	}
	
	// Blit again.
	theScreenManager.Blit( pixmapAddr, &theRect, &theRect, 0 /* srcCopy */ );

	for (indexRow = 0; indexRow < theWidth; indexRow++)
	{
		// Fill the screen with a diagonal line.
		for (indexLines = 0; indexLines < theHeight; indexLines++)
		{
			int xCoord = (indexLines + indexRow) % theWidth;
			DrawPoint( &theMem, baseAddy, rowBytes, xCoord, indexLines, indexRow % 16 );

//			theRect.fTop = indexLines;
//			theRect.fLeft = xCoord;
//			theRect.fBottom = indexLines + 1;
//			theRect.fRight = xCoord + 1;
		}	

		// Blit again.
		theScreenManager.Blit( pixmapAddr, &theRect, &theRect, 0 /* srcCopy */ );
	}
	
	// Close the screen.
	theScreenManager.PowerOffScreen();

	(void) ::unlink( kTempFlashPath );
#endif
}

// -------------------------------------------------------------------------- //
//  * DrawPoint( TMemory*, KUInt32, KUInt32, int, int, int )
// -------------------------------------------------------------------------- //
void
UScreenTests::DrawPoint(
					TMemory* inMemoryIntf,
					KUInt32 inBaseAddy,
					KUInt32 inRowBytes,
					int inXcoord,
					int inYcoord,
					int inColor )
{
	KUInt32 addr = inBaseAddy + (inRowBytes * inYcoord);
	
	if (TScreenManager::kBitsPerPixel == 4)
	{
		KUInt32 shift = (7 - (inXcoord & 0x7)) * 4;
		addr += (inXcoord / 2);
		addr &= ~0x00000003;
		Boolean fault;
		KUInt32 theWord = inMemoryIntf->ReadP( addr, fault );
		KUInt32 mask = 0xF << shift;
		theWord = (theWord & ~ mask) | (inColor << shift);
		(void) inMemoryIntf->WriteP( addr, theWord );
	} else {
		::abort();
	}
}

// =========================================================================== //
//         THE LESSER-KNOWN PROGRAMMING LANGUAGES #8: LAIDBACK                 //
//                                                                             //
// This language was developed at the Marin County Center for T'ai Chi,        //
// Mellowness and Computer Programming (now defunct), as an alternative to     //
// the more intense atmosphere in nearby Silicon Valley.                       //
//                                                                             //
// The center was ideal for programmers who liked to soak in hot tubs while    //
// they worked.  Unfortunately few programmers could survive there because the //
// center outlawed Pizza and Coca-Cola in favor of Tofu and Perrier.           //
//                                                                             //
// Many mourn the demise of LAIDBACK because of its reputation as a gentle and //
// non-threatening language since all error messages are in lower case.  For   //
// example, LAIDBACK responded to syntax errors with the message:              //
//                                                                             //
//         "i hate to bother you, but i just can't relate to that.  can        //
//         you find the time to try it again?"                                 //
// =========================================================================== //
