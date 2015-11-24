// ==============================
// File:			TNativeiOSCalls.h
// Project:			Einstein
//
// Copyright 2015 by Jake Bordens
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

#ifndef _TNATIVEIOSCALLS_H
#define _TNATIVEIOSCALLS_H

#include <K/Defines/KDefinitions.h>

#include "NativeCallsDefines.h"

class TMemory;

///
/// Class for native calls, the first step towards virtualization.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 150 $
///
/// \test	aucun test défini.
///
class TNativeiOSCalls
{
public:	
	///
	/// Constructor from an interface to memory.
	///
	TNativeiOSCalls( TMemory* inMemoryIntf );

	///
	/// Destructor.
	///
	~TNativeiOSCalls( void );
  
#if TARGET_IOS
	KUInt32 iOSActivityWithText(KUInt32 textPtrAddr,
											  KUInt32 textLen);
	/// \name Variables
	TMemory*		mMemoryIntf;			///< Interface to memory.

#endif // TARGET_IOS
};

#endif // _TNATIVEIOSCALLS_H

// ===================================================== //
// Old programmers never die, they just become managers. //
// ===================================================== //
