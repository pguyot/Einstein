// ==============================
// File:			TJITGeneric_Multiply_template.h
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

#if (Rd == 15) || (Rs == 15) || (Rm == 15)
	#error "Use of R15 for Rd, Rs or Rm is unpredictable"
#endif

// -------------------------------------------------------------------------- //
//  * Multiply
// -------------------------------------------------------------------------- //
Multiply_Template(FLAG_S, Rd, Rs, Rm)
#if IMPLEMENTATION
{
	const KUInt32 theResult = ioCPU->mCurrentRegisters[Rm] * ioCPU->mCurrentRegisters[Rs];
	ioCPU->mCurrentRegisters[Rd] = theResult;
#if FLAG_S
	// We track status flags
	if(theResult & 0x80000000)
		ioCPU->mCPSR_N = 1;
	else
		ioCPU->mCPSR_N = 0;
	
	if(theResult == 0)
		ioCPU->mCPSR_Z = 1;
	else
		ioCPU->mCPSR_Z = 0;
#endif

	CALLNEXTUNIT;
}
#endif
