// ==============================
// File:			TATACard.h
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

#ifndef _TATACARD_H
#define _TATACARD_H

#include <K/Defines/KDefinitions.h>
#include "TPCMCIACard.h"

///
/// Class for ATA cards.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TATACard
	:
		public TPCMCIACard
{
public:
	///
	/// Constructor from the size.
	///
	TATACard( KUInt32 inSize );

	///
	/// Destructor.
	///
	virtual ~TATACard( void );

	///
	/// Get PCMCIA pins.
	///
	virtual KUInt32		GetVPCPins( void );
	
	///
	/// Set PCMCIA pins.
	///
	virtual void		SetVPCPins( KUInt32 inPins );

	///
	/// Read attribute space.
	///
	virtual KUInt32		ReadAttr( KUInt32 inOffset );

	///
	/// Read attribute space (byte).
	///
	virtual KUInt8		ReadAttrB( KUInt32 inOffset );

	///
	/// Read I/O space.
	///
	virtual KUInt32		ReadIO( KUInt32 inOffset );

	///
	/// Read I/O space (byte).
	///
	virtual KUInt8		ReadIOB( KUInt32 inOffset );

	///
	/// Read memory space.
	///
	virtual KUInt32		ReadMem( KUInt32 inOffset );

	///
	/// Read memory space (byte).
	///
	virtual KUInt8		ReadMemB( KUInt32 inOffset );

	///
	/// Write attribute space.
	///
	virtual void		WriteAttr( KUInt32 inOffset, KUInt32 inValue );

	///
	/// Write attribute space (byte).
	///
	virtual void		WriteAttrB( KUInt32 inOffset, KUInt8 inValue );

	///
	/// Write I/O space.
	///
	virtual void		WriteIO( KUInt32 inOffset, KUInt32 inValue );

	///
	/// Write I/O space (byte).
	///
	virtual void		WriteIOB( KUInt32 inOffset, KUInt8 inValue );

	///
	/// Write memory space.
	///
	virtual void		WriteMem( KUInt32 inOffset, KUInt32 inValue );

	///
	/// Write memory space (byte).
	///
	virtual void		WriteMemB( KUInt32 inOffset, KUInt8 inValue );

private:
	/// \name Variables
};

#endif
		// _TATACARD_H

// ============================================================================== //
//         One of the questions that comes up all the time is: How enthusiastic   //
// is our support for UNIX?                                                       //
//         Unix was written on our machines and for our machines many years ago.  //
// Today, much of UNIX being done is done on our machines. Ten percent of our     //
// VAXs are going for UNIX use.  UNIX is a simple language, easy to understand,   //
// easy to get started with. It's great for students, great for somewhat casual   //
// users, and it's great for interchanging programs between different machines.   //
// And so, because of its popularity in these markets, we support it.  We have    //
// good UNIX on VAX and good UNIX on PDP-11s.                                     //
//         It is our belief, however, that serious professional users will run    //
// out of things they can do with UNIX. They'll want a real system and will end   //
// up doing VMS when they get to be serious about programming.                    //
//         With UNIX, if you're looking for something, you can easily and quickly //
// check that small manual and find out that it's not there.  With VMS, no matter //
// what you look for -- it's literally a five-foot shelf of documentation -- if   //
// you look long enough it's there.  That's the difference -- the beauty of UNIX  //
// is it's simple; and the beauty of VMS is that it's all there.                  //
//                 -- Ken Olsen, president of DEC, DECWORLD Vol. 8 No. 5, 1984    //
// [It's been argued that the beauty of UNIX is the same as the beauty of Ken     //
// Olsen's brain.  Ed.]                                                           //
// ============================================================================== //
