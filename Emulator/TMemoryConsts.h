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
		kLowROMEnd = 0x00800000, ///< Also High ROM base.
		kHighROMEnd = 0x01000000, ///< BTW, High means 8-16 MB here.
		kROMEnd = kHighROMEnd, ///< Full ROM End
		kROMEndMask = 0xFF000000, ///< What's after ROM.
		kFlashBank1 = 0x02000000, ///< Internal store.
		kFlashBank1End = 0x02400000, ///< End of internal store (bank#1)
		// bank #2
		kRAMStart = 0x04000000,
		// bank #3
		kHardwareBase = 0x0F000000,
		kHdWr_PlatformVers = 0x0F000008, ///< Not actually a hardware address, but a transparent way for native apps to read the Platform driver version

		// Memory controller.
		// Register values below are what the MP2x00 ROM writes. The boot code
		// is DiagBootStub (ROM 0x0001955C) and BasicBusControlRegInit
		// (0x00394504), the sleep code SaveCPUStateAndStopSystem (0x00018F48).
		kHdWr_MemCtrlReg = 0x0F001000, ///< R/W Memory controller. Set by the boot code to 0x1A4, 0x524 or 0x184
									   ///< depending on the RAM banks, 0x3104 by BasicBusControlRegInit.
									   ///< Bits 12..15 enable PCMCIA sockets 0..3 (TCardSocket::Init).
		kHdWr_04RAMSize = 0x0F001800, ///< R/W Size of the DRAM bank at 0x04000000 in 64 KB units (0x40 = 4 MB,
									  ///< 0x10 = 1 MB, 0 = none), found by the boot code. We say 0xXYXY00XY.
		kHdWr_08RAMSize = 0x0F001C00, ///< R/W Same for the DRAM bank at 0x08000000. We say 0.
		kHdWr_MemCtrl2000 = 0x0F002000, ///< W  Unknown, set to 0x80 by the boot code.

		// BIO interface (Keynes, TBIOInterface). BIO register n is at
		// kHdWr_BIOCmdBase + (n << 10). Writes wait for status bit 0x1000,
		// read results arrive in kHdWr_BIOReadData (status bit 0x80).
		kHdWr_BIO3000 = 0x0F043000, ///< W  Unknown, set to 0x7400 by the boot code.
		kHdWr_BIO3800 = 0x0F043800, ///< W  Unknown, set to 0x2000, then 0x2400 by the boot code.
		kHdWr_BIOStatus = 0x0F048000, ///< R/W BIO status (ReadBIOStatus). TBIOInterface::Init writes 0.
		kHdWr_BIOReadData = 0x0F048C00, ///< R  Result of a BIO register read (BIOReadCommandData).
		kHdWr_BIOCmdBase = 0x0F050000, ///< R/W BIO register 0.
		kHdWr_BIOCmd11 = 0x0F052C00, ///< R/W BIO register 11, TBIOInterface::Init writes 0x4E.
		kHdWr_BIOCmd12 = 0x0F053000, ///< R/W BIO register 12 (00007000)
		kHdWr_BIOCmd13 = 0x0F053400, ///< R/W BIO register 13 (00008C00)
		kHdWr_BIOCmd14 = 0x0F053800, ///< R/W BIO register 14 (00000000)
		kHdWr_BIOCmd17 = 0x0F054400, ///< W  BIO register 17 (00008400)
		kHdWr_BIOCmd18 = 0x0F054800, ///< W  BIO register 18 (00008400)
		kHdWr_BIOCmd20 = 0x0F055000, ///< W  BIO register 20 (00008400)

		// DMA controller, see TDMAManager.h
		kHdWr_DMAChan1Base = 0x0F080000, ///< DMA Channel Registers (bank#1)
		kHdWr_DMAChan1End = 0x0F08FC00, ///< DMA Channel Registers (bank#1) (end)
		kHdWr_DMAAssgmnt = 0x0F08FC00, ///< DMA Assignment register (R/W)
		kHdWr_DMAChan2Base = 0x0F090000, ///< DMA Channel Registers (bank#2)
		kHdWr_DMAChan2End = 0x0F098000, ///< DMA Channel Registers (bank#2) (end)
		kHdWr_DMAEnableStat = 0x0F098000, ///< DMA Enable/Status register (R/W)
		kHdWr_DMADisable = 0x0F098400, ///< DMA Disable register (W)
		kHdWr_DMAWordStat = 0x0F098800, ///< DMA Word status register (R)

		// Power and clocks ("Milton").
		kHdWr_MiltonPwrReg = 0x0F110000, ///< R/W Milton power register, one bit per subsystem
										 ///< (TVoyagerPlatform::TurnOnMiltonPwrRegBit, power map at ROM 0x0037AC40):
										 ///< 0x0001 subsystem 32, 0x0002 'extr' serial, 0x0004 LCD controller (subsystem 33),
										 ///< 0x0008 stops the CPU until a wake-up interrupt (sleep),
										 ///< 0x0040 DMA (TVoyagerPlatform::PowerOnDMA), 0x0080 BIO interface,
										 ///< 0x0100 'infr' serial, 0x0200 'tblt' serial, 0x0400 'mdem' serial,
										 ///< 0x0800 subsystem 31 (also set before each delay at boot).
										 ///< Boot values 0x800, 0xFC7, 0x881.
		kHdWr_HighSpeedClck = 0x0F110400, ///< High speed clock, written 0x90 at boot (R, v=0x90)
		kHdWr_Milton1400 = 0x0F111400, ///< W  Unknown, ROMBoot writes 0x7E6, waits a moment and writes 0.

		// LCD controller. Only accessed by TMainDisplayDriver in the
		// 'ScreenDrivers' ROM extension (ROM 0x007A5700), which Einstein
		// replaces with its own driver. The panel is 320x480, black and white,
		// the controller generates 16 gray levels from the 4 bit frame buffer.
		// No gray pattern tables are written by the driver.
		// Values are what the driver writes at power on, in this order.
		kHdWr_LCDCtrl0000 = 0x0F140000, ///< W  0x0004FDDF
		kHdWr_LCDCtrl0400 = 0x0F140400, ///< W  0
		kHdWr_LCDCtrl0800 = 0x0F140800, ///< W  1 (written after LCDCtrl1800)
		kHdWr_LCDCtrl0C00 = 0x0F140C00, ///< W  0x3B, probably words per line - 1 (480 pixels * 4 bits / 32 - 1)
		kHdWr_LCDCtrl1000 = 0x0F141000, ///< W  0x800, written first at power on and at power off
		kHdWr_LCDDisplayOn = 0x0F141400, ///< W  12 at power on (before the contrast ramp), 0 at power off
		kHdWr_LCDCtrl1800 = 0x0F141800, ///< W  13
		kHdWr_LCDCtrl1C00 = 0x0F141C00, ///< W  3
		kHdWr_LCDCtrl2000 = 0x0F142000, ///< W  2, written after the frame buffer address
		kHdWr_LCDCtrl2400 = 0x0F142400, ///< W  0x18A
		kHdWr_LCDCtrl2800 = 0x0F142800, ///< W  1
		kHdWr_LCDFrameBuf1 = 0x0F142C00, ///< W  Physical address of the frame buffer
		kHdWr_LCDFrameBuf2 = 0x0F143000, ///< W  Same physical address of the frame buffer

		// System: timers, interrupts, GPIO.
		kHdWr_SysCtrl0400 = 0x0F180400, ///< W  Unknown, 0x8000 at boot, InitCGlobals sets a value from the flash reserved block.
		kHdWr_CalendarReg = 0x0F181000, ///< Calendar register, in seconds.
		kHdWr_AlarmReg = 0x0F181400, ///< Alarm register, in seconds.
		kHdWr_Ticks = 0x0F181800, ///< Used by SafeShortTimerDelay as
								  ///< containing the number of hardware
								  ///< ticks. N2 clock is at 3.6864MHz.
								  ///< Also written (?)
		kHdWr_MatchReg0 = 0x0F182000, ///< First timer match register (FIQTimer)
		kHdWr_MatchReg1 = 0x0F182400, ///< Second timer match register (IRQTimer)
		kHdWr_MatchReg2 = 0x0F182800, ///< Third timer match registe (Timer)
		kHdWr_MatchReg3 = 0x0F182C00, ///< Fourth timer match register (Scheduler)

		// Interrupt controller, see TInterruptManager.h
		kHdWr_IntPresent = 0x0F183000, ///< R  Pending interrupts.
		kHdWr_IntCtrlReg = 0x0F183400, ///< R/W Interrupt mask, EnterFIQAtomic writes 0x0C400000 here.
		kHdWr_IntClear = 0x0F183800, ///< W  Writing 1 clears a pending interrupt.
		kHdWr_FIQMaskReg = 0x0F183C00, ///< R/W Bit set -> FIQ
		kHdWr_IntEDReg1 = 0x0F184000, ///< R/W Set by EnableInterrupt (flag 0x001), cleared by DisableInterrupt.
		kHdWr_IntEDReg2 = 0x0F184400, ///< R/W Set by EnableInterrupt (flag 0x002), cleared by DisableInterrupt.
		kHdWr_IntEDReg3 = 0x0F184800, ///< R/W Wake-up mask, set by EnableInterrupt (flag 0x400). Used as interrupt
									  ///< mask while sleeping, PowerOnSystem uses IntPresent & this to find the wake-up source.
		kHdWr_IntLevelReg = 0x0F184C00, ///< R  Current level of the interrupt inputs, not latched. FIQHandler checks the
										///< reset switch (0x00400000), the boot code the 'mdem' DCD line (0x00200000).
		kHdWr_SleepCtrlReg = 0x0F185000, ///< W  Set to 0 before the CPU is stopped, to 1 after it wakes up and at boot.

		// GPIO interface (TGPIOInterface), one bit per GPIO line:
		//  0 IN power switch, 1 IN AC adapter, 2/3 IN PCMCIA card lock 0/1,
		//  4 OUT +5V, 5 OUT +12V, 6 OUT disable LTC1323 line driver,
		//  7 OUT fast battery charging, 8 IN IR busy, 9 serial ~CP enable.
		kHdWr_GPIOIntRaised = 0x0F18C000, ///< R  Pending GPIO interrupts.
		kHdWr_GPIOIntEnable = 0x0F18C400, ///< R/W GPIO interrupt enable.
		kHdWr_GPIOIntClear = 0x0F18C800, ///< W  Writing 1 clears a pending GPIO interrupt.
		kHdWr_GPIOIntRising = 0x0F18CC00, ///< R/W Interrupt flag 0x01, probably rising edge (00000103)
		kHdWr_GPIOIntFalling = 0x0F18D000, ///< R/W Interrupt flag 0x02, probably falling edge (0000000F)
		kHdWr_GPIOInput = 0x0F18D400, ///< R  GPIO input data (ReadGPIOData).
		kHdWr_GPIOIntWake = 0x0F18D800, ///< R/W Interrupt flag 0x08, GPIO interrupts that wake up the Newton.
		kHdWr_GPIOPullup = 0x0F18DC00, ///< R/W Pullups (00001EF0, FFFF0FF0)
		kHdWr_GPIOPolarity = 0x0F18E000, ///< R/W Polarity
		kHdWr_GPIODirection = 0x0F18E800, ///< R/W Direction. EarlyIOPowerOn or's this with 0x30.
		kHdWr_GPIOOutput = 0x0F18EC00, ///< R/W Output data. EarlyIOPowerOn or's this with 0x10 (+5V) and
									   ///< then with 0x20 (+12V), EarlyIOPowerOff bic's it with 0x30.
		// Serial bank
		kExternalSerialBase = 0x0F1C0000, ///< Voyager 'extr' serial port base
		kInfraredSerialBase = 0x0F1D0000, ///< Voyager 'infr' serial port base
		kBuiltInSerialBase = 0x0F1E0000, ///< Voyager 'tblt' serial port base
		kModemSerialBase = 0x0F1F0000, ///< Voyager 'mdem' serial port base
		kSerialEnd = 0x0F200000, ///< End of voyager serial ports.
		// bank #4
		kHdWr_ExtDataAbt1 = 0x0F240000, ///< First external data abort register (R), read by DataAbortHandler
		kHdWr_ExtDataAbt2 = 0x0F240400, ///< Second external data abort register (W), TCardSocket::EnableSocketAbort writes 0x10
		kHdWr_ExtDataAbt3 = 0x0F240800, ///< Third external data abort register (W), bit 0x10 cleared by
										///< TCardSocket::EnableSocketAbort, set by DisableSocketAbort
		kHdWr_BankCtrlReg = 0x0F241000, ///< Bank control register.
										///< FFFFFFFF -> 0x000
										///< FFFF0000 -> 0x200
										///< 0000FFFF -> 0x300
										///< FF000000 -> 0x400
										///< 0000FF00 -> 0x500
		kHdWr_Bank1800 = 0x0F241800, ///< W  Unknown, 0x3916 at boot, InitCGlobals sets a value from the flash reserved block.
		kHdWr_DRAMCtrlReg = 0x0F242400, ///< R/W DRAM controller: 0x01F9453C or 0x01F94573 while the boot code sizes the
										///< RAM banks (bit 0x10 is cleared before each change). ROMBoot looks at the lower
										///< 24 bits to detect a restart (00000000, 01F9453C, 01F94573)
		kROMSerialChip = 0x0F243000, ///< R/W (ROM Serial chip?) (00000000, 00000001)
		kHdWr_Bank7000 = 0x0F247000, ///< W  Unknown, 1 at boot, saved and restored across sleep.

		// Bus control (BasicBusControlRegInit). All are saved and restored
		// across sleep.
		kHdWr_BusCtrl00 = 0x0F280000, ///< W  0x465A at boot, then 0xC044 (0x6043 on a StrongARM).
									  ///< InitCGlobals can override it from the 'ctim' config entry.
		kHdWr_BusCtrl01 = 0x0F280400, ///< W  0x181A at boot, then 0x2C34, 0x1816 or the 'ctim' config entry.
		kHdWr_BusCtrl02 = 0x0F280800, ///< W  0x2003
		kHdWr_BusCtrl03 = 0x0F280C00, ///< R/W only saved and restored.
		kHdWr_BusCtrl08 = 0x0F282000, ///< R/W only saved and restored.
		kHdWr_BusCtrl0C = 0x0F283000, ///< W  0x257 at boot, then 0x000, then 0x255.
		kHdWr_BusCtrl0D = 0x0F283400, ///< W  0x23 at boot.
		kHdWr_BusCtrl10 = 0x0F284000, ///< W (00000023) Not accessed directly by the MP2x00 ROM.
		kFlashBank2 = 0x10000000, ///< More flash here.
		kFlashBank2End = 0x10400000, ///< End of second bank.
		kHdWr_LCDContrast = 0x20000000, ///< W  LCD contrast (bias voltage): user contrast (-16..16) +
										///< temperature compensation + 102. Written by TMainDisplayDriver at
										///< every blit and ramped up slowly at power on.
		// bank #5
		kPCMCIA0Base = 0x30000000,
		kPCMCIA1Base = 0x40000000,
		kPCMCIA2Base = 0x50000000,
		kPCMCIA3Base = 0x60000000,
		kPCMCIA3End = 0x70000000
	};

	///
	/// Fault Status Register bits.
	///
	enum {
		kFSR_TerminalException = 0x2, ///< 0b0010 # 003932DC
		kFSR_VectorException = 0x0, ///< 0b0000 # 003932DC
		kFSR_Alignment1 = 0x1, ///< 0b0001 # 003932FC
		kFSR_Alignment2 = 0x3, ///< 0b0011 # 003932FC
		kFSR_ExternalTrLvl1 = 0xC, ///< 0b1100 # 0039339C
		kFSR_ExternalTrLvl2 = 0xE, ///< 0b1110 # 0039339C
		kFSR_TranslationSection = 0x5, ///< 0b0101 # 00393314
		kFSR_TranslationPage = 0x7, ///< 0b0111 # 00393314
		kFSR_DomainSection = 0x9, ///< 0b1001 # 00393314
		kFSR_DomainPage = 0xB, ///< 0b1011 # 00393314
		kFSR_PermissionSection = 0xD, ///< 0b1101 # 0039339C
		kFSR_PermissionPage = 0xF, ///< 0b1111 # 0039339C
		kFSR_ExternalLFSection = 0x4, ///< 0b0100 # 0039339C
		kFSR_ExternalLFPage = 0x6, ///< 0b0110 # 0039339C
		kFSR_ExternalNLFSection = 0x8, ///< 0b1000 # 0039339C
		kFSR_ExternalNLFPage = 0xA, ///< 0b1010 # 0039339C
		kFSR_DomainShift = 4,
		kFSR_DomainShiftMin1 = 3
	};

	///
	/// Other MMU-related constants.
	///
	enum {
		kMMUSectionMask = 0xFFF00000,
		kMMUSectionMaskNeg = ~kMMUSectionMask,
		kMMULargePageMask = 0xFFFF0000,
		kMMULargePageMaskNeg = ~kMMULargePageMask,
		kMMUSmallPageMask = 0xFFFFF000,
		kMMUSmallPageMaskNeg = ~kMMUSmallPageMask,
		kMMUTinyPageMask = 0xFFFFFC00,
		kMMUTinyPageMaskNeg = ~kMMUTinyPageMask,
		kMMUSmallestPageMask = kMMUTinyPageMask,
		kMMUSmallestPageMaskNeg = ~kMMUSmallestPageMask,
		kMMUSmallestPageSize = 1024, ///< 1 KB (that's not big).
		kMMUDirtyCacheMask = 1
	};

	///
	/// Other hardware constants.
	///
	enum {
		kHighSpeedClockVal = 0x90 ///< Value of the high speed clock.
	};
};

#endif
// _TMEMORYCONSTS_H

// ====================== //
// Loose bits sink chips. //
// ====================== //
