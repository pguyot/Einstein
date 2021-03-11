// ==============================
// File:			TATACard.cp
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

#include "TATACard.h"

// Einstein
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TATACard( KUInt32 )
// -------------------------------------------------------------------------- //
TATACard::TATACard( KUInt32 inSize )
{
}

// -------------------------------------------------------------------------- //
//  * ~TATACard( void )
// -------------------------------------------------------------------------- //
TATACard::~TATACard( void )
{
}

// -------------------------------------------------------------------------- //
//  * GetVPCPins( void )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::GetVPCPins( void )
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
TATACard::SetVPCPins( KUInt32 inPins )
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
TATACard::ReadAttr( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "ReadAttr( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadAttrB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TATACard::ReadAttrB( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "ReadAttrB( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadIO( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TATACard::ReadIO( KUInt32 inOffset )
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
TATACard::ReadIOB( KUInt32 inOffset )
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
TATACard::ReadMem( KUInt32 inOffset )
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
TATACard::ReadMemB( KUInt32 inOffset )
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
TATACard::WriteAttr( KUInt32 inOffset, KUInt32 inValue )
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
TATACard::WriteAttrB( KUInt32 inOffset, KUInt8 inValue )
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
TATACard::WriteIO( KUInt32 inOffset, KUInt32 inValue )
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
TATACard::WriteIOB( KUInt32 inOffset, KUInt8 inValue )
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
TATACard::WriteMem( KUInt32 inOffset, KUInt32 inValue )
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
TATACard::WriteMemB( KUInt32 inOffset, KUInt8 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "WriteMemB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// =================================================== //
// There's got to be more to life than compile-and-go. //
// =================================================== //
