// ==============================
// File:			UserInfoDefinitions.h
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

#ifndef _USERINFODEFINITIONS_H
#define _USERINFODEFINITIONS_H

#if TARGET_OS_NEWTON
	#include <Newton.h>
	#include <AEvents.h>
	typedef ULong	KUInt32;
	typedef UChar	KUInt8;
#else
	#include <K/Defines/KDefinitions.h>
#endif

enum EUserInfoSel {
	kUserInfo_FirstName			= 'firs',
	kUserInfo_LastName			= 'last',
	kUserInfo_Company			= 'comp',
	kUserInfo_Addr				= 'addr',
	kUserInfo_Addr2				= 'add2',
	kUserInfo_PostalCode		= 'post',
	kUserInfo_City				= 'city',
	kUserInfo_Region			= 'regn',
	kUserInfo_Country			= 'coun',
	kUserInfo_CountryISOCode	= 'cISO',
	kUserInfo_HomePhone			= 'home',
	kUserInfo_HomeFaxPhone		= 'homf',
	kUserInfo_WorkPhone			= 'work',
	kUserInfo_WorkFaxPhone		= 'worf'
};

#endif
		// _TUSERINFODEFINITIONS_H

// ====================================================================== //
//         THE LESSER-KNOWN PROGRAMMING LANGUAGES #18: FIFTH              //
//                                                                        //
// FIFTH is a precision mathematical language in which the data types     //
// refer to quantity.  The data types range from CC, OUNCE, SHOT, and     //
// JIGGER to FIFTH (hence the name of the language), LITER, MAGNUM and    //
// BLOTTO.  Commands refer to ingredients such as CHABLIS, CHARDONNAY,    //
// CABERNET, GIN, VERMOUTH, VODKA, SCOTCH, and WHATEVERSAROUND.           //
//                                                                        //
// The many versions of the FIFTH language reflect the sophistication and //
// financial status of its users.  Commands in the ELITE dialect include  //
// VSOP and LAFITE, while commands in the GUTTER dialect include HOOTCH   //
// and RIPPLE. The latter is a favorite of frustrated FORTH programmers   //
// who end up using this language.                                        //
// ====================================================================== //

