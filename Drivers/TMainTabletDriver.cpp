// ==============================
// File:			TMainTabletDriver.cp
// Project:			Einstein
//
// Copyright 2003-2013 by Paul Guyot (pguyot@kallisys.net).
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

#include "TMainTabletDriver.impl.h"

#include "K/Misc/RelocHack.h"

PROTOCOL_IMPL_SOURCE_MACRO(TMainTabletDriver)	// Magic stuff, do not touch

typedef long (*InterruptHandlerProcPtr)(void*);

// All these functions are in Public JT.
InterruptObject* RegisterInterrupt(
			ULong inInterruptMask,
			void* inCookie,
			InterruptHandlerProcPtr inHandler,
			ULong inFlags);
void DeregisterInterrupt(InterruptObject*);
extern "C" long EnableInterrupt(InterruptObject*, ULong);
extern "C" long DisableInterrupt(InterruptObject*);
extern "C" long ClearInterrupt(InterruptObject*);
void TBCWakeUpInkerFromInterrupt(ULong);
void TBCInsertTabletSample(ULong, ULong);

#define kTabletInterruptMask	0x10000000

// ------------------------------------------------------------------------ //
// New( void )
// ------------------------------------------------------------------------ //
// Protocol constructor.
TTabletDriver*
TMainTabletDriver::New( void )
{
	// Register the interrupt.
	mInterrupt = RegisterInterrupt(
			kTabletInterruptMask,
			this,
			(InterruptHandlerProcPtr)
#if __GNUC__
// With gcc, assume we are using a modern, relocation-friendly toolchain.
				&SInterruptHandler,
#else
				RelocFuncPtr(&SInterruptHandler),
#endif
			2 );

	(void) EnableInterrupt(mInterrupt, 2);

	return this;
}

// ------------------------------------------------------------------------ //
// Delete( void )
// ------------------------------------------------------------------------ //
// Protocol destructor.
void
TMainTabletDriver::Delete( void )
{
	DeregisterInterrupt(mInterrupt);
}

// ------------------------------------------------------------------------ //
// SInterruptHandler( void* )
// ------------------------------------------------------------------------ //
long
TMainTabletDriver::SInterruptHandler( void* inUserData )
{
	TMainTabletDriver* theDriver = (TMainTabletDriver*) inUserData;
	(void) DisableInterrupt(theDriver->mInterrupt);
	(void) ClearInterrupt(theDriver->mInterrupt);

	ULong theSampleRecord;
	ULong theSampleTime;

	TBCWakeUpInkerFromInterrupt(0);

	while (theDriver->NativeGetSample(&theSampleRecord, &theSampleTime))
	{
		TBCInsertTabletSample(theSampleRecord, theSampleTime);
	}

	return EnableInterrupt(theDriver->mInterrupt, 2);
}

// =============================================================================== //
//         *** STUDENT SUCCESSES ***                                               //
//                                                                                 //
// Many of our students have gone on to achieve great success in all fields of     //
// programming.  One former student developed the concept of the personalized      //
// form letter.  Does the phrase, "Dear Mr.(insert name), You may already be a     //
// winner!," sound familiar?  Another student writes "After only five lessons I    //
// sold a "My Most Unforgettable Program" article to Corrosive Computing magazine. //
// Another of our graduates writes, "I recently completed a database-management    //
// program for my department manager.  My program touched him so deeply that he    //
// was speechless.  He told me later that he had never seen such a program in      //
// his entire career.  Thank you, Famous Programmers' school; only you could       //
// have made this possible."  Send for our introductory brochure which explains    //
// in vague detail the operation of the Famous Programmers' School, and you'll     //
// be eligible to win a possible chance to enter a drawing, the winner of which    //
// can vie for a set of free steak knives.  If you don't do it now, you'll hate    //
// yourself in the morning.                                                        //
// =============================================================================== //
