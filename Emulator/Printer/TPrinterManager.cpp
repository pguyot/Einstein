// ==============================
// File:      TPrinterManager.h
// Project:      Einstein
//
// Copyright 2022 by Matthias Melcher (einstein@messagepad.org).
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

#include "TPrinterManager.h"

#include <stdlib.h>

#ifdef TARGET_OS_WIN32
#include <string.h>
#else
#include <strings.h>
#endif

// Einstein.

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TPrinterManager(TLog*)
// -------------------------------------------------------------------------- //
TPrinterManager::TPrinterManager(TLog* inLog) :
		mLog(inLog)
{
	(void) mLog;
}

// -------------------------------------------------------------------------- //
//  * ~TPrinterManager()
// -------------------------------------------------------------------------- //
TPrinterManager::~TPrinterManager()
{
}

void
TPrinterManager::Delete(KUInt32 inDrvr)
{
	(void) inDrvr;
}

KUInt32
TPrinterManager::Open(KUInt32 inDrvr)
{
	(void) inDrvr;
	return noErr;
}

KUInt32
TPrinterManager::Close(KUInt32 inDrvr)
{
	(void) inDrvr;
	return noErr;
}

KUInt32
TPrinterManager::OpenPage(KUInt32 inDrvr)
{
	(void) inDrvr;
	return noErr;
}

KUInt32
TPrinterManager::ClosePage(KUInt32 inDrvr)
{
	(void) inDrvr;
	return noErr;
}

KUInt32
TPrinterManager::ImageBand(KUInt32 inDrvr, KUInt32 inBand, KUInt32 inRect)
{
	(void) inDrvr;
	(void) inBand;
	(void) inRect;
	return noErr;
}

void
TPrinterManager::CancelJob(KUInt32 inDrvr, KUInt32 inAsyncCancel)
{
	(void) inDrvr;
	(void) inAsyncCancel;
}

KUInt32
TPrinterManager::IsProblemResolved(KUInt32 inDrvr)
{
	(void) inDrvr;
	return 1;
}

void
TPrinterManager::GetPageInfo(KUInt32 inDrvr, KUInt32 inInfo)
{
	(void) inDrvr;
	(void) inInfo;
}

void
TPrinterManager::GetBandPrefs(KUInt32 inDrvr, KUInt32 inPrefs)
{
	(void) inDrvr;
	(void) inPrefs;
}

KUInt32
TPrinterManager::FaxEndPage(KUInt32 inDrvr, KUInt32 inPageCount)
{
	(void) inDrvr;
	(void) inPageCount;
	return 0;
}

// ============================================================================= //
// As in Protestant Europe, by contrast, where sects divided endlessly into      //
// smaller competing sects and no church dominated any other, all is different   //
// in the fragmented world of IBM.  That realm is now a chaos of conflicting     //
// norms and standards that not even IBM can hope to control.  You can buy a     //
// computer that works like an IBM machine but contains nothing made or sold by  //
// IBM itself.  Renegades from IBM constantly set up rival firms and establish   //
// standards of their own.  When IBM recently abandoned some of its original     //
// standards and decreed new ones, many of its rivals declared a puritan         //
// allegiance to IBM's original faith, and denounced the company as a divisive   //
// innovator.  Still, the IBM world is united by its distrust of icons and       //
// imagery.  IBM's screens are designed for language, not pictures.  Graven      //
// images may be tolerated by the luxurious cults, but the true IBM faith relies //
// on the austerity of the word.                                                 //
//                 -- Edward Mendelson, "The New Republic", February 22, 1988    //
// ============================================================================= //
