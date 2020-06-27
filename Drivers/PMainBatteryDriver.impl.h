// ==============================
// File:			PMainBatteryDriver.impl.h
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

#ifndef _PMAINBATTERYDRIVER_IMPL_H
#define _PMAINBATTERYDRIVER_IMPL_H

#include "PBatteryDriver.h"

///
/// Class for our battery driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
PROTOCOL PMainBatteryDriver : public PBatteryDriver
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(PMainBatteryDriver);

	CAPABILITIES( ("copyright" "Paul Guyot") ("version" "1.0") )

	PBatteryDriver*	New( void );
	void			Delete( void );

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
		// _PMAINBATTERYDRIVER_IMPL_H

// =============================================================================== //
//         Several students were asked to prove that all odd integers are prime.   //
//         The first student to try to do this was a math student.  "Hmmm...       //
// Well, 1 is prime, 3 is prime, 5 is prime, and by induction, we have that all    //
// the odd integers are prime."                                                    //
//         The second student to try was a man of physics who commented, "I'm not  //
// sure of the validity of your proof, but I think I'll try to prove it by         //
// experiment."  He continues, "Well, 1 is prime, 3 is prime, 5 is prime, 7 is     //
// prime, 9 is...  uh, 9 is... uh, 9 is an experimental error, 11 is prime, 13     //
// is prime...  Well, it seems that you're right."                                 //
//         The third student to try it was the engineering student, who responded, //
// "Well, to be honest, actually, I'm not sure of your answer either.  Let's       //
// see...  1 is prime, 3 is prime, 5 is prime, 7 is prime, 9 is... uh, 9 is...     //
// well, if you approximate, 9 is prime, 11 is prime, 13 is prime...  Well, it     //
// does seem right."                                                               //
//         Not to be outdone, the computer science student comes along and says    //
// "Well, you two sort've got the right idea, but you'll end up taking too long!   //
// I've just whipped up a program to REALLY go and prove it."  He goes over to     //
// his terminal and runs his program.  Reading the output on the screen he says,   //
// "1 is prime, 1 is prime, 1 is prime, 1 is prime..."                             //
// =============================================================================== //
