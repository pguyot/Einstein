// ==============================
// File:			TEinsteinFlashDriver.h
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

#ifndef _TEINSTEINFLASHDRIVER_H
#define _TEINSTEINFLASHDRIVER_H

#include "TFlashDriver.h"

///
/// Class for our flash driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
PROTOCOL TEinsteinFlashDriver : public TFlashDriver
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(TEinsteinFlashDriver);

	CAPABILITIES( ("copyright" "Paul Guyot") ("version" "1.0") )

	ULong	Identify(unsigned long, unsigned long, SFlashChipInformation&);
	void	CleanUp(TMemoryAllocator&);
	ULong	Init(TMemoryAllocator&);
	ULong	InitializeDriverData(TFlashRange&, TMemoryAllocator&);
	ULong	CleanUpDriverData(TFlashRange&, TMemoryAllocator&);
	ULong	StartReadingArray(TFlashRange&);
	ULong	DoneReadingArray(TFlashRange&);
	ULong	Write(ULong data, ULong mask, ULong addr, TFlashRange&);
	ULong	StartErase(TFlashRange&, unsigned long);
	ULong	ResetBlockStatus(TFlashRange&, unsigned long);
	ULong	IsEraseComplete(TFlashRange&, unsigned long, long&);
	ULong	LockBlock(TFlashRange&, unsigned long);
	ULong	BeginWrite(TFlashRange&, unsigned long, unsigned long);
	ULong	ReportWriteResult(TFlashRange&, unsigned long);

private:
//	ULong	DoWrite(ULong data, ULong mask, ULong addr, ULong StartOfBlock);
	ULong	DoErase(ULong addr, ULong inSize);
};

#endif
		// _TEINSTEINFLASHDRIVER_H

// ============================================================================== //
// Dear Emily:                                                                    //
//         I'm having a serious disagreement with somebody on the net. I          //
// tried complaints to his sysadmin, organizing mail campaigns, called for        //
// his removal from the net and phoning his employer to get him fired.            //
// Everybody laughed at me.  What can I do?                                       //
//                 -- A Concerned Citizen                                         //
//                                                                                //
// Dear Concerned:                                                                //
//         Go to the daily papers.  Most modern reporters are top-notch computer  //
// experts who will understand the net, and your problems, perfectly.  They       //
// will print careful, reasoned stories without any errors at all, and surely     //
// represent the situation properly to the public.  The public will also all      //
// act wisely, as they are also fully cognizant of the subtle nature of net       //
// society.                                                                       //
//         Papers never sensationalize or distort, so be sure to point out things //
// like racism and sexism wherever they might exist.  Be sure as well that they   //
// understand that all things on the net, particularly insults, are meant         //
// literally.  Link what transpires on the net to the causes of the Holocaust, if //
// possible.  If regular papers won't take the story, go to a tabloid paper --    //
// they are always interested in good stories.                                    //
// ============================================================================== //
