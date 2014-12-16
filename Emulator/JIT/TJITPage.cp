// ==============================
// File:			TJITPage.cp
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
#include "TJITPage.h"

// Einstein
#include "TMemory.h"
#include "JIT.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TJITPage( void )
// -------------------------------------------------------------------------- //
template<>
TJITPage<JITClass, JITPageClass>::TJITPage( void )
	:
		mPointer( NULL ),
		mVAddr( 0 ),
		mPAddr( 0 )
{
}

// -------------------------------------------------------------------------- //
//  * ~TJITPage( void )
// -------------------------------------------------------------------------- //
template<>
TJITPage<JITClass, JITPageClass>::~TJITPage( void )
{
}

// -------------------------------------------------------------------------- //
//  * Init( KUInt32*, KUInt32 )
// -------------------------------------------------------------------------- //
template<>
void
TJITPage<JITClass, JITPageClass>::Init(
			TMemory* inMemoryIntf,
			KUInt32 inVAddr,
			KUInt32 inPAddr )
{
	mPointer = inMemoryIntf->GetDirectPointerToPage( inPAddr );
	mVAddr = inVAddr;
	mPAddr = inPAddr;
}

// =========================== //
// Memory fault -- brain fried //
// =========================== //
