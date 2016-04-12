// ==============================
// File:			TMemoryConsts.h
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

#ifndef _TMEMORYCONSTS_H
#define _TMEMORYCONSTS_H

#include <K/Defines/KDefinitions.h>

///
/// Constants for the structure of the memory.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 150 $
///
/// \test	aucun test défini.
///
class TMemoryConsts
{
public:
	///
	/// Various constants about Memory space (physical).
	///
	enum {
		// bank #1
		kLowROMEnd			= 0x00800000,   ///< Also High ROM base.
		kHighROMEnd			= 0x01000000,   ///< BTW, High means 8-16 MB here.
		kROMEnd				= kHighROMEnd,  ///< Full ROM End
		kROMEndMask			= 0xFF000000,	///< What's after ROM.
		kFlashBank1			= 0x02000000,   ///< Internal store.
		kFlashBank1End		= 0x02400000,   ///< End of internal store (bank#1)
		// bank #2
		kRAMStart			= 0x04000000,
		// bank #3
		kHardwareBase		= 0x0F000000,
		kHdWr_P0F001000		= 0x0F001000,   ///< R/W
		kHdWr_04RAMSize		= 0x0F001800,   ///< We say 0xXYXY00XY
											///< (also written with 0x40)
		kHdWr_08RAMSize		= 0x0F001C00,   ///< We say 0 (also written with 0x00 & 0x40)
		kHdWr_P0F002000		= 0x0F002000,   ///< W (00000080)
		kHdWr_P0F043000		= 0x0F043000,   ///< W (00007400)
		kHdWr_P0F043800		= 0x0F043800,   ///< W (00002000)
		kHdWr_P0F048000		= 0x0F048000,   ///< R/W (00000000)
		kHdWr_P0F052C00		= 0x0F052C00,   ///< R/W (0000004E)
		kHdWr_P0F053000		= 0x0F053000,   ///< R/W (00007000)
		kHdWr_P0F053400		= 0x0F053400,   ///< R/W (00008C00)
		kHdWr_P0F053800		= 0x0F053800,   ///< R/W (00000000)
		kHdWr_P0F054400		= 0x0F054400,   ///< W (00008400)
		kHdWr_P0F054800		= 0x0F054800,   ///< W (00008400)
		kHdWr_P0F055000		= 0x0F055000,   ///< W (00008400)
		kHdWr_DMAChan1Base	= 0x0F080000,   ///< DMA Channel Registers (bank#1)
		kHdWr_DMAChan1End	= 0x0F08FC00,   ///< DMA Channel Registers (bank#1) (end)
		kHdWr_DMAAssgmnt	= 0x0F08FC00,   ///< DMA Assignment register (R/W)
		kHdWr_DMAChan2Base	= 0x0F090000,   ///< DMA Channel Registers (bank#2)
		kHdWr_DMAChan2End	= 0x0F098000,   ///< DMA Channel Registers (bank#2) (end)
		kHdWr_DMAEnableStat = 0x0F098000,   ///< DMA Enable/Status register (R/W)
		kHdWr_DMADisable	= 0x0F098400,   ///< DMA Disable register (W)
		kHdWr_DMAWordStat	= 0x0F098800,   ///< DMA Word status register (R)
		kHdWr_P0F110000		= 0x0F110000,   ///< External interrupt mask.
		kHdWr_HighSpeedClck	= 0x0F110400,	///< High speed clock (R, v=0x90)
		kHdWr_P0F111400		= 0x0F111400,   ///< W (00000000, 000007E6)
		kHdWr_P0F180400		= 0x0F180400,   ///< W (00008000)
		kHdWr_CalendarReg   = 0x0F181000,   ///< Calendar register, in seconds.
		kHdWr_AlarmReg		= 0x0F181400,   ///< Alarm register, in seconds.
		kHdWr_Ticks			= 0x0F181800,   ///< Used by SafeShortTimerDelay as
											///< containing the number of hardware
											///< ticks. N2 clock is at 3.6864MHz.
											///< Also written (?)
		kHdWr_MatchReg0		= 0x0F182000,   ///< First timer match register (FIQTimer)
		kHdWr_MatchReg1		= 0x0F182400,   ///< Second timer match register (IRQTimer)
		kHdWr_MatchReg2		= 0x0F182800,   ///< Third timer match registe (Timer)
		kHdWr_MatchReg3		= 0x0F182C00,   ///< Fourth timer match register (Scheduler)
		kHdWr_IntPresent	= 0x0F183000,   ///< R (by DispatchFIQInterrupt)
		kHdWr_IntCtrlReg	= 0x0F183400,   ///< EnterFIQAtomic writes 0x0C400000
											///< here.
		kHdWr_IntClear		= 0x0F183800,   ///< Writing here clears an interrupt.
		kHdWr_FIQMaskReg	= 0x0F183C00,   ///< Bit set -> FIQ
		kHdWr_IntEDReg1		= 0x0F184000,   ///< Enable/Disable
		kHdWr_IntEDReg2		= 0x0F184400,   ///< Enable/Disable
		kHdWr_IntEDReg3		= 0x0F184800,   ///< Enable/Disable (Enable)
		kHdWr_P0F184C00		= 0x0F184C00,   ///< R
		kHdWr_P0F185000		= 0x0F185000,   ///< W (00000001)
		kHdWr_GPIO_RReg		= 0x0F18C000,	///< GPIO interface (1) (r) (raised reg?)
		kHdWr_GPIO_EReg		= 0x0F18C400,   ///< GPIO interface (2) (r/w) (enable reg?)
		kHdWr_GPIO_CReg		= 0x0F18C800,   ///< GPIO interface (3) (w) (clear reg?)
		kHdWr_P0F18CC00		= 0x0F18CC00,   ///< W (00000103)
		kHdWr_P0F18D000		= 0x0F18D000,   ///< W (0000000F)
		kHdWr_P0F18D400		= 0x0F18D400,   ///< R GPIO PCMCIA card related (PCMCIA door lock switch?)
		kHdWr_P0F18D800		= 0x0F18D800,   ///< W (00000000)
		kHdWr_P0F18DC00		= 0x0F18DC00,   ///< W (00001EF0, FFFF0FF0)
//		kHdWr_P0F18DC00		= 0x0F18DC00,   ///< W (00000000) (?)
		kHdWr_IOPower1		= 0x0F18E800,   ///< EarlyIOPowerOn or's this with 0x30
		kHdWr_IOPower2		= 0x0F18EC00,   ///< EarlyIOPowerOn or's this with 0x10
											///< and then with 0x20.
											///< EarlyIOPowerOff bic's it with 0x30
		// Serial bank
		kExternalSerialBase	= 0x0F1C0000,	///< Voyager 'extr' serial port base
		kInfraredSerialBase	= 0x0F1D0000,	///< Voyager 'infr' serial port base
		kBuiltInSerialBase	= 0x0F1E0000,	///< Voyager 'tblt' serial port base
		kModemSerialBase	= 0x0F1F0000,	///< Voyager 'mdem' serial port base
		kSerialEnd			= 0x0F200000,	///< End of voyager serial ports.
		// bank #4
		kHdWr_ExtDataAbt1	= 0x0F240000,   ///< First external data abort register (R)
		kHdWr_ExtDataAbt2	= 0x0F240400,   ///< Second external data abort register (W)
		kHdWr_ExtDataAbt3	= 0x0F240800,   ///< Third external data abort register (W)
		kHdWr_BankCtrlReg   = 0x0F241000,   ///< Bank control register.
											///< FFFFFFFF -> 0x000
											///< FFFF0000 -> 0x200
											///< 0000FFFF -> 0x300
											///< FF000000 -> 0x400
											///< 0000FF00 -> 0x500
		kHdWr_P0F241800		= 0x0F241800,   ///< W (00003916)
		kHdWr_P0F242400		= 0x0F242400,   ///< R/W (00000000, 01F9453C, 01F94573)
		kROMSerialChip		= 0x0F243000,   ///< R/W (ROM Serial chip?) (00000000, 00000001)
		kHdWr_P0F247000		= 0x0F247000,   ///< W (00000001)
		kHdWr_P0F280000		= 0x0F280000,   ///< W (0000465A, 0000C044)
		kHdWr_P0F280400		= 0x0F280400,   ///< W (0000181A, 00002C34)
		kHdWr_P0F280800		= 0x0F280800,   ///< W (00002003)
		kHdWr_P0F283000		= 0x0F283000,   ///< W (00000000, 00000255, 00000257)
		kHdWr_P0F284000		= 0x0F284000,   ///< W (00000023)
		kFlashBank2			= 0x10000000,   ///< More flash here.
		kFlashBank2End		= 0x10400000,   ///< End of second bank.
		// bank #5
		kPCMCIA0Base		= 0x30000000,
		kPCMCIA1Base		= 0x40000000,
		kPCMCIA2Base		= 0x50000000,
		kPCMCIA3Base		= 0x60000000,
		kPCMCIA3End			= 0x70000000
	};
	
	///
	/// Fault Status Register bits.
	///
	enum {
		kFSR_TerminalException  = 0x2,  ///< 0b0010 # 003932DC
		kFSR_VectorException	= 0x0,  ///< 0b0000 # 003932DC
		kFSR_Alignment1			= 0x1,  ///< 0b0001 # 003932FC
		kFSR_Alignment2			= 0x3,  ///< 0b0011 # 003932FC
		kFSR_ExternalTrLvl1		= 0xC,  ///< 0b1100 # 0039339C
		kFSR_ExternalTrLvl2		= 0xE,  ///< 0b1110 # 0039339C
		kFSR_TranslationSection = 0x5,  ///< 0b0101 # 00393314
		kFSR_TranslationPage	= 0x7,  ///< 0b0111 # 00393314
		kFSR_DomainSection		= 0x9,  ///< 0b1001 # 00393314
		kFSR_DomainPage			= 0xB,  ///< 0b1011 # 00393314
		kFSR_PermissionSection  = 0xD,  ///< 0b1101 # 0039339C
		kFSR_PermissionPage		= 0xF,  ///< 0b1111 # 0039339C
		kFSR_ExternalLFSection  = 0x4,  ///< 0b0100 # 0039339C
		kFSR_ExternalLFPage		= 0x6,  ///< 0b0110 # 0039339C
		kFSR_ExternalNLFSection = 0x8,  ///< 0b1000 # 0039339C
		kFSR_ExternalNLFPage	= 0xA,  ///< 0b1010 # 0039339C
		kFSR_DomainShift		= 4,
		kFSR_DomainShiftMin1	= 3
	};
	
	///
	/// Other MMU-related constants.
	///
	enum {
		kMMUSectionMask			= 0xFFF00000,
		kMMUSectionMaskNeg		= ~kMMUSectionMask,
		kMMULargePageMask		= 0xFFFF0000,
		kMMULargePageMaskNeg	= ~kMMULargePageMask,
		kMMUSmallPageMask		= 0xFFFFF000,
		kMMUSmallPageMaskNeg	= ~kMMUSmallPageMask,
		kMMUTinyPageMask		= 0xFFFFFC00,
		kMMUTinyPageMaskNeg		= ~kMMUTinyPageMask,
		kMMUSmallestPageMask	= kMMUTinyPageMask,
		kMMUSmallestPageMaskNeg = ~kMMUSmallestPageMask,
		kMMUSmallestPageSize	= 1024,	///< 1 KB (that's not big).
		kMMUDirtyCacheMask		= 1
	};
	
	///
	/// Other hardware constants.
	///
	enum {
		kHighSpeedClockVal		= 0x90	///< Value of the high speed clock.
	};
};

#endif
		// _TMEMORYCONSTS_H

// ====================== //
// Loose bits sink chips. //
// ====================== //
