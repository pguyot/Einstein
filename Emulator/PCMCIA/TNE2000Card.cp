// ==============================
// File:			TNE2000Card.cp
// Project:			Einstein
//
// Copyright 2010 by Matthias Melcher (newton@matthiasm.com).
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
#include "TNE2000Card.h"

// Einstein
#include "Emulator/TEmulator.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/PCMCIA/TPCMCIAController.h"

/* 
 
 PCMCIA physical connection for an I/O card:
 
   Power
 GND			Ground
 Vcc			Vcc	+5V
 Vpp1			Vpp1
 Vpp2			Programmeing Voltage2 (EPROM)
 
   Access
 D0-D15			Data Lines, connected to CPU
 A0-A25			Address Lines, connected to CPU (64MB)
 /CD1			Card Detect 1, L if card was inserted all the way
 /CD2			Card Detect 2, L if card was inserted all the way
 /CE1			Card Enable 1
 /CE2			Card Enable 2
 /OE			Output Enable
 /WE			Write Enable : Programm
 /WP,/IOIS16	Write Protect (H = prtected): IOIS16 I/O data bus is 16 bit
 /IORD			I/O Read	 
 /IOWR			I/O Write
 
   Control
 RESET			Reset (H will reset the card, held high during the first ms of insertion)
 READY,/IREQ	Ready : Busy (IREQ), if H, the card can be accessed (set by the card), I/O Mode: trigger interrupt
 /WAIT			CPU must wait for operation to end (L=ok)
 /BVD1:STSCHG	Battery Voltage Detect1 (H=good): Card Status Changed
 /BVD2:SPKR		Battery Voltage Detect2 (H=good): Speaker
 
   Unknown
 /VS1			Refresh
 /VS2,RFU	 
 /INPACK		Input Port Ack. 
 /REG	 
 
   Pins can be reassigned to allow DMA:
 DREQ# can be put on IOS16, INPACK, or SPKR
 DACK
 IORD#
 IOWR#
 TC
 CE1 & CE2
 IREQ
 

 VCP pins to PC pins conversion:
	keep FIFO and Failure flags
	VS2		0x0020 -> 0x0400
	VS1		0x0010 -> 0x0200
	RDY		0x0400 -> 0x0004
	BVD2	0x0002 -> 0x0010
	BVD1	0x0001 -> 0x0008
	WP		0x0200 -> 0x0020
	CD1		0x0004 -> 0x0002
	CD2		0x0008 -> 0x0001
    GPIO Port Door Lock -> 0x0080
 */

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

const KUInt8 TNE2000Card::kCISData[90] = {
// NE2000:
// 
	0x01, 3,	// Tuple #1, code = 0x1 (Common memory descriptor), length = 3
	0x00, 0x00, 0xff,
	// Common memory device information:
	// Device number 1, type No device, WPS = OFF
	// Speed = No speed, Memory block size = 512b, 1 units
	0x15, 51,	// Tuple #2, code = 0x15 (Version 1 info), length = 51
	0x04, 0x01, 0x45, 0x74, 0x68, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x20, 0x41, 0x64, 0x61, 0x70, 0x74,
	0x65, 0x72, 0x20, 0x45, 0x32, 0x30, 0x30, 0x30, 0x00, 0x50, 0x43, 0x4d, 0x43, 0x49, 0x41, 0x20,
	0x45, 0x74, 0x68, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x00, 0x44, 0x00, 0x4e, 0x45, 0x32, 0x30, 0x30,
	0x30, 0x00, 0xff,
	// Version = 4.1, Manuf = [Ethernet Adapter], card vers = [E2000 PCMCIA Ethernet]
	// Addit. info = [D],[NE2000]
	0x1a, 5,	// Tuple #3, code = 0x1a (Configuration map), length = 5
	0x01, 0x20, 0xf8, 0x03, 0x03,
	// Reg len = 2, config register addr = 0x3f8, last config = 0x20
	// Registers: XX------
	0x1b, 17,	// Tuple #4, code = 0x1b (Configuration entry), length = 17
	0xe0, 0x81, 0x1d, 0x3f, 0x55, 0x4d, 0x5d, 0x06, 0x86, 0x46, 0x26, 0xfc, 0x24, 0x65, 0x30, 0xff,
	0xff,
	// Config index = 0x20(default)
	// Interface byte = 0x81 (I/O)  wait signal supported
	// Vcc pwr:
	// Nominal operating supply voltage: 5 x 1V
	// Minimum operating supply voltage: 4.5 x 1V
	// Maximum operating supply voltage: 5.5 x 1V
	// Continuous supply current: 1 x 100mA
	// Max current average over 1 second: 1 x 100mA, ext = 0x46
	// Max current average over 10 ms: 2 x 100mA
	// Wait scale Speed = 1.5 x 10 us
	// Card decodes 5 address lines, full 8/16 Bit I/O
	// IRQ modes: Level
	// IRQs:  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
	0x21, 2,	// Tuple #5, code = 0x21 (Functional ID), length = 2
	0x06, 0x00,
	// Network/LAN adapter
	0xff, 0 // Tuple #6, code = 0xff (Terminator), length = 0
};

// -------------------------------------------------------------------------- //
//  * TNE2000Card( KUInt32 )
// -------------------------------------------------------------------------- //
TNE2000Card::TNE2000Card()
{
}

// -------------------------------------------------------------------------- //
//  * ~TNE2000Card( void )
// -------------------------------------------------------------------------- //
TNE2000Card::~TNE2000Card( void )
{
}

// -------------------------------------------------------------------------- //
//  * GetVPCPins( void )
// -------------------------------------------------------------------------- //
KUInt32
TNE2000Card::GetVPCPins( void )
{
	if (GetLog())
	{
		GetLog()->LogLine( "TNE2000Card::GetVPCPins" );
	}
	
	return TPCMCIAController::k1C00_CardIsPresent | 0x0603; // Present and ready
}

// -------------------------------------------------------------------------- //
//  * SetVPCPins( KUInt32 )
// -------------------------------------------------------------------------- //
void
TNE2000Card::SetVPCPins( KUInt32 inPins )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::SetVPCPins( %.4X )", (unsigned int) inPins );
	}
}

// -------------------------------------------------------------------------- //
//  * ReadAttr( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TNE2000Card::ReadAttr( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::ReadAttr( %.8X )", (unsigned int) inOffset );
		GetEmulator()->BreakInMonitor();
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadAttrB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TNE2000Card::ReadAttrB( KUInt32 inOffset )
{
	KUInt8 theResult = 0;
	KUInt32 theOffset = (inOffset^2)>>1;
	
	if (theOffset<sizeof(kCISData)) {
		theResult = kCISData[theOffset];
	} else if (theOffset==508) {
		theResult = 0x41;  // What is this? Why is the OS reading and writing this?
	} else {
		theResult = 0;
	}
	
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::ReadAttrB( %.8X => %d = '%c' (%d))", 
						   (unsigned int) inOffset,
						   theOffset,
						   (theResult>31&&theResult<128)?theResult:'.',
						   theResult );
		GetEmulator()->BreakInMonitor();
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * ReadIO( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TNE2000Card::ReadIO( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::ReadIO( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadIOB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TNE2000Card::ReadIOB( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::ReadIOB( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadMem( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TNE2000Card::ReadMem( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::ReadMem( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadMemB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TNE2000Card::ReadMemB( KUInt32 inOffset )
{
	// Inserting the Card read these locations before the driver is loaded:
	// 0000 0003 0002 0001 0000 0007
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::ReadMemB( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * WriteAttr( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TNE2000Card::WriteAttr( KUInt32 inOffset, KUInt32 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::WriteAttr( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteAttrB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TNE2000Card::WriteAttrB( KUInt32 inOffset, KUInt8 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::WriteAttrB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteIO( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TNE2000Card::WriteIO( KUInt32 inOffset, KUInt32 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::WriteIO( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteIOB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TNE2000Card::WriteIOB( KUInt32 inOffset, KUInt8 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::WriteIOB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteMem( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TNE2000Card::WriteMem( KUInt32 inOffset, KUInt32 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::WriteMem( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteMemB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TNE2000Card::WriteMemB( KUInt32 inOffset, KUInt8 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TNE2000Card::WriteMemB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// ======================================================================= //
// Real programmers don't draw flowcharts.  Flowcharts are, after all, the //
// illiterate's form of documentation.  Cavemen drew flowcharts; look how  //
// much good it did them.                                                  //
// ======================================================================= //
