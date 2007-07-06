// ==============================
// File:			NativeCallsDefines.h
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

#ifndef _NATIVECALLSDEFINES_H
#define _NATIVECALLSDEFINES_H

#if TARGET_OS_NEWTON
	#include <Newton.h>
	typedef ULong	KUInt32;
	typedef UChar	KUInt8;
#else
	#include <K/Defines/KDefinitions.h>
#endif

enum EFFI_Type {
	k_void			= 0,
	k_uint8			= 1,
	k_sint8			= 2,
	k_uint16		= 3,
	k_sint16		= 4,
	k_uint32		= 5,
	k_sint32		= 6,
	k_uint64		= 7,
	k_sint64		= 8,
	k_float			= 9,
	k_double		= 10,
	k_longdouble	= 11,
	k_string		= 12,
	k_binary		= 13,
	k_pointer		= 14,
	k_iostring		= 15,
	k_iobinary		= 16
};

enum {
	kNativeCalls_MaxArgs = 16,
	kNativeCalls_SymbolMaxLen = 256
};

#endif
		// _NATIVECALLSDEFINES_H

// ============================================================================= //
// It is a very humbling experience to make a multimillion-dollar mistake, but   //
// it is also very memorable.  I vividly recall the night we decided how to      //
// organize the actual writing of external specifications for OS/360.  The       //
// manager of architecture, the manager of control program implementation, and   //
// I were threshing out the plan, schedule, and division of responsibilities.    //
//         The architecture manager had 10 good men.  He asserted that they      //
// could write the specifications and do it right.  It would take ten months,    //
// three more than the schedule allowed.                                         //
//         The control program manager had 150 men.  He asserted that they       //
// could prepare the specifications, with the architecture team coordinating;    //
// it would be well-done and practical, and he could do it on schedule.          //
// Futhermore, if the architecture team did it, his 150 men would sit twiddling  //
// their thumbs for ten months.                                                  //
//         To this the architecture manager responded that if I gave the control //
// program team the responsibility, the result would not in fact be on time,     //
// but would also be three months late, and of much lower quality.  I did, and   //
// it was.  He was right on both counts.  Moreover, the lack of conceptual       //
// integrity made the system far more costly to build and change, and I would    //
// estimate that it added a year to debugging time.                              //
//                 -- Frederick Brooks Jr., "The Mythical Man Month"             //
// ============================================================================= //
