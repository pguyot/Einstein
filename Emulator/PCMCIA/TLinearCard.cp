// ==============================
// File:			TLinearCard.cp
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
#include "TLinearCard.h"

// Einstein
#include "Emulator/TEmulator.h"
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

const KUInt8 TLinearCard::kCISData[0x61] = {
	0x01, 0x03, 0x54, 0x16, 0xFF, 0x1E, 0x06, 0x02,
	0x11, 0x01, 0x01, 0x03, 0x01, 0x20, 0x04, 0x89,
	0x00, 0x23, 0x85, 0x21, 0x02, 0x01, 0x00, 0x12,
	0x04, 0x00, 0x00, 0x02, 0x00, 0x15, 0x40, 0x05,
	0x00, 0x69, 0x6E, 0x74, 0x65, 0x6c, 0x00, 0x56,
	0x41, 0x4c, 0x55, 0x45, 0x20, 0x53, 0x45, 0x52,
	0x49, 0x45, 0x53, 0x20, 0x31, 0x30, 0x30, 0x20,
	0x00, 0x30, 0x36, 0x20, 0x00, 0x43, 0x4F, 0x50,
	0x59, 0x52, 0x49, 0x47, 0x48, 0x54, 0x20, 0x49,
	0x4E, 0x54, 0x45, 0x4c, 0x20, 0x43, 0x4F, 0x52,
	0x50, 0x4F, 0x52, 0x41, 0x54, 0x49, 0x4F, 0x4E,
	0x20, 0x31, 0x39, 0x39, 0x35, 0x00, 0xFF, 0xFF,
	0x00 };


// -------------------------------------------------------------------------- //
//  * TLinearCard( KUInt32 )
// -------------------------------------------------------------------------- //
TLinearCard::TLinearCard( KUInt32 inSize )
	:
		mSize( inSize )
{
}

// -------------------------------------------------------------------------- //
//  * ~TLinearCard( void )
// -------------------------------------------------------------------------- //
TLinearCard::~TLinearCard( void )
{
}

// -------------------------------------------------------------------------- //
//  * GetVPCPins( void )
// -------------------------------------------------------------------------- //
KUInt32
TLinearCard::GetVPCPins( void )
{
	if (GetLog())
	{
		GetLog()->LogLine( "GetVPCPins" );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * SetVPCPins( KUInt32 )
// -------------------------------------------------------------------------- //
void
TLinearCard::SetVPCPins( KUInt32 inPins )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "SetVPCPins( %.4X )", (unsigned int) inPins );
	}
}

// -------------------------------------------------------------------------- //
//  * ReadAttr( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TLinearCard::ReadAttr( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "ReadAttr( %.8X )", (unsigned int) inOffset );
		GetEmulator()->BreakInMonitor();
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadAttrB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TLinearCard::ReadAttrB( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "ReadAttrB( %.8X )", (unsigned int) inOffset );
		GetEmulator()->BreakInMonitor();
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadIO( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TLinearCard::ReadIO( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "ReadIO( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadIOB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TLinearCard::ReadIOB( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "ReadIOB( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadMem( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TLinearCard::ReadMem( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "ReadMem( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadMemB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TLinearCard::ReadMemB( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "ReadMemB( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * WriteAttr( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteAttr( KUInt32 inOffset, KUInt32 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "WriteAttr( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteAttrB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteAttrB( KUInt32 inOffset, KUInt8 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "WriteAttrB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteIO( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteIO( KUInt32 inOffset, KUInt32 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "WriteIO( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteIOB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteIOB( KUInt32 inOffset, KUInt8 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "WriteIOB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteMem( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteMem( KUInt32 inOffset, KUInt32 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "WriteMem( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteMemB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteMemB( KUInt32 inOffset, KUInt8 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "WriteMemB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// ======================================================================= //
// Real programmers don't draw flowcharts.  Flowcharts are, after all, the //
// illiterate's form of documentation.  Cavemen drew flowcharts; look how  //
// much good it did them.                                                  //
// ======================================================================= //
