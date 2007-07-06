// ==============================
// File:			TJITGeneric_Test_template.h
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

// -------------------------------------------------------------------------- //
//  * TestEQ
// -------------------------------------------------------------------------- //
Test_Template(EQ, OFFSET,
{
	// Z set (equal)
	if (ioCPU->mCPSR_Z)
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestNE
// -------------------------------------------------------------------------- //
Test_Template(NE, OFFSET,
{
	// Z clear (not equal)
	if (!ioCPU->mCPSR_Z)
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestCS
// -------------------------------------------------------------------------- //
Test_Template(CS, OFFSET,
{
	// C set (unsigned higher or same)
	if (ioCPU->mCPSR_C)
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestCC
// -------------------------------------------------------------------------- //
Test_Template(CC, OFFSET,
{
	// C clear (unsigned lower)
	if (!ioCPU->mCPSR_C)
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestMI
// -------------------------------------------------------------------------- //
Test_Template(MI, OFFSET,
{
	// N set (negative)
	if (ioCPU->mCPSR_N)
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestPL
// -------------------------------------------------------------------------- //
Test_Template(PL, OFFSET,
{
	// N clear (positive or zero)
	if (!ioCPU->mCPSR_N)
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestVS
// -------------------------------------------------------------------------- //
Test_Template(VS, OFFSET,
{
	// V set (overflow)
	if (ioCPU->mCPSR_V)
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestVC
// -------------------------------------------------------------------------- //
Test_Template(VC, OFFSET,
{
	// V clear (no overflow)
	if (!ioCPU->mCPSR_V)
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestHI
// -------------------------------------------------------------------------- //
Test_Template(HI, OFFSET,
{
	// C set and Z clear (unsigned higher)
	if (((ioCPU->mCPSR_C) && !(ioCPU->mCPSR_Z)))
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestLS
// -------------------------------------------------------------------------- //
Test_Template(LS, OFFSET,
{
	// C clear or Z set (unsigned lower or same)
	if ((!(ioCPU->mCPSR_C) || (ioCPU->mCPSR_Z)))
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestGE
// -------------------------------------------------------------------------- //
Test_Template(GE, OFFSET,
{
	// N set and V set, or N clear and V clear (greater or equal)
	if ((ioCPU->mCPSR_N == ioCPU->mCPSR_V))
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestLT
// -------------------------------------------------------------------------- //
Test_Template(LT, OFFSET,
{
	// N set and V clear, or N clear and V set (less than)
	if ((ioCPU->mCPSR_N != ioCPU->mCPSR_V))
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestGT
// -------------------------------------------------------------------------- //
Test_Template(GT, OFFSET,
{
	// Z clear, and either N set and V set, or N clear and V clear (greater than)
	if (((!ioCPU->mCPSR_Z) && (ioCPU->mCPSR_N == ioCPU->mCPSR_V)))
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// -------------------------------------------------------------------------- //
//  * TestLE
// -------------------------------------------------------------------------- //
Test_Template(LE, OFFSET,
{
	// Z set, or N set and V clear, or N clear and V set (less than or equal)
	if (((ioCPU->mCPSR_Z) || (ioCPU->mCPSR_N != ioCPU->mCPSR_V)))
	{
		EXECUTENEXTUNIT;
	} else {
		CALLUNIT(OFFSET);
	}
})

// ================================================= //
// The world will end in 5 minutes.  Please log out. //
// ================================================= //
