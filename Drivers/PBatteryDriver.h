// ==============================
// File:			PBatteryDriver.h
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

#ifndef _PBATTERYDRIVER_H
#define _PBATTERYDRIVER_H

#include <Newton.h>
#include <Protocols.h>

enum {
	kNoBatteryType		= 0,	///< I think
	kNiMhBatteryType	= 3		///< I guess we could find the other values.
};

struct PowerPlantStatus
{
										/// Example, unplugged
												// Status				RawStatus
	ULong	mBatteryType;				/// 00	// 00000003 (nimh)
	ULong	mVoltage1;					/// 04	// 000587C0				0C97D000
	ULong	mBatteryLevel;				/// 08	// 00000064 = 100
	ULong	mBatteryAlert;				/// 0C	// 00000014 = 20
	ULong	fUnknown_10;				/// 10	// 00000000
	ULong	mVoltage6;					/// 14	// 006CF999				00E19000
	ULong	mAdapterPlugged;			/// 18	// 00000000
	ULong	mVoltage7;					/// 1C	// 00003F36				005C0000
	ULong	fUnknown_20;				/// 20	// 00000000
	ULong	fUnknown_24;				/// 24	// FFFFFFFF
	ULong	mUnknownDIOPins33Related;	/// 28	// FFFFFFFF
	ULong	mVoltage4;					/// 2C	// 001A2F28				086E2000
	ULong	mVoltage5;					/// 30	// 001A8D79				07D3B000
};

///
/// Protocol for the battery driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
PROTOCOL PBatteryDriver : public TProtocol
{
public:
	static	PBatteryDriver*		New( char* inImplementationName );	// PROTOCOL ctor
			void				Delete( void );						// PROTOCOL dtor

	ULong	Init();
	ULong	WakeUp();
	ULong	ShutDown();
	ULong	Count();
	ULong	Status(unsigned long, PowerPlantStatus*);
	ULong	RawStatus(unsigned long, PowerPlantStatus*);
	ULong	StartSleepCharge();
	ULong	SetType(unsigned long, unsigned long);
	ULong	ReadADCVoltage(unsigned long);
	ULong	ConvertVoltage(unsigned long, unsigned long);
};

#endif
		// _PBATTERYDRIVER_H

// =========================================================================== //
//         One day a student came to Moon and said, "I understand how to make  //
// a better garbage collector.  We must keep a reference count of the pointers //
// to each cons."                                                              //
//         Moon patiently told the student the following story -- "One day a   //
// student came to Moon and said, "I understand how to make a better garbage   //
// collector..."                                                               //
// =========================================================================== //
