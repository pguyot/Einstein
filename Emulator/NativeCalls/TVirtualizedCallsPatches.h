// ==============================
// File:			TVirtualizedCallsPatches.h
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

#ifndef _TVIRTUALIZEDCALLSPATCHES_H
#define _TVIRTUALIZEDCALLSPATCHES_H

#include <K/Defines/KDefinitions.h>

// C++
#include <string>

///
/// Class for virtualized calls constants.
///
/// \author Paul Guyot <pguyot@kallisys.net>
///
class TVirtualizedCallsPatches
{
public:
	enum {
		k__rt_sdiv,
		k__rt_udiv,
		kmemmove,
		ksymcmp__FPcT1,
	};

	static void DoPatchROM(KUInt32* romPtr, const std::string& inMachineName);

};

#endif
		// _TVIRTUALIZEDCALLSPATCHES_H

// ========================================================================== //
// No proper program contains an indication which as an operator-applied      //
// occurrence identifies an operator-defining occurrence which as an          //
// indication-applied occurrence identifies an indication-defining occurrence //
// different from the one identified by the given indication as an            //
// indication-applied occurrence.                                             //
//                 -- ALGOL 68 Report                                         //
// ========================================================================== //
