// ==============================
// File:			TJIT.cp
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

#include <K/Defines/KDefinitions.h>
#include "TJIT.h"

// Einstein.
#include "JIT.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TJIT( TMemory*, KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
template<>
TJIT<JITPageClass>::TJIT(
		TMemory* inMemoryIntf,
		TMMU* inMMUIntf )
	:
		mCache( inMemoryIntf, inMMUIntf )
{
}

// -------------------------------------------------------------------------- //
//  * ~TJIT( void )
// -------------------------------------------------------------------------- //
template<>
TJIT<JITPageClass>::~TJIT( void )
{
}

// ======================================================================== //
// Making files is easy under the UNIX operating system.  Therefore, users  //
// tend to create numerous files using large amounts of file space.  It has //
// been said that the only standard thing about all UNIX systems is the     //
// message-of-the-day telling users to clean up their files.                //
//                 -- System V.2 administrator's guide                      //
// ======================================================================== //
