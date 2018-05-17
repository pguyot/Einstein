// ==============================
// File:			TBasicSerialPortManager.cp
// Project:			Einstein
//
// Copyright 2017 by Matthias Melcher (mm@matthiasm.com).
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
#include "TBasicSerialPortManager.h"
#include "TPathHelper.h"

// POSIX
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#if !TARGET_OS_WIN32
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/stat.h>
#if !TARGET_OS_IOS
#include <CoreServices/CoreServices.h>
#endif
#endif

#include "Emulator/Log/TLog.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TDMAManager.h"
#include "Emulator/TMemory.h"
#include "Emulator/TARMProcessor.h"

// --- include these to be able to patch the ROM before the emulator starts
#include "Emulator/JIT/Generic/TJITGenericROMPatch.h"
#include "Emulator/JIT/Generic/TJITGeneric_Macros.h"


/*
 
 TODO: change named pipe path from "Einstein Emulator" to "Einstein Platform"
		(see: TCococaAppController: + (NSString *)getAppSupportDirectory)

 Software using the Serial Port
 ==============================

 NCU	The origial Newton Connect Utilities. NCU runs under Classic MacOS. The
		commonly used emulatr, BasiliskII, has no serial port emulation. It
		may be possible to patch BasiliskII to support the serial port emulation
		used in this file.
 NCX	Simon Bell's Newton Connection for Mac OS X. As of Feb. 2017, a patched
		version is available from GitHub that connects to Einstein:
		https://github.com/MatthiasWM/newton-connection/releases/tag/mv0.1
 socat  Not a Newton tool by itself, but a Unix way of rerouting file system
		based pipes of all kind. In order to reroute the "named pipe" approach
		below into something that look like a true serial port, use this:
		  sudo socat -d -d -d PTY,raw,mode=666,echo=0 \
            PIPE:$(HOME)/Library/Application\ Support/Einstein\ Emulator/ExtrSerPortSend\!\!PIPE:$(HOME)/Library/Application\ Support/Einstein\ Emulator/ExtrSerPortRecv
 NewtonInspector   Jake Borden's implementation of the NTK Inspector window.
		Jake has tested Einstein Serial emulation with his NewtonInspector and
		as of Feb. 2017 it is working. There seems to be a remaining flaw when
		transfering large data set. Speed is also still an issue that needs
		to be fixed in Einstein.
		https://github.com/jake-b/NewtonInspector
 DyneTK	Currently not compilable, and, according to the author, the 
		wrong approach to modern app development
 tntk	Eckhart Koeppen's approach to develop apps for NewtonOS on OS X. It is a
		combination of the NewtonScript compiler newt/0, and the DCL tools to
		communicate with a device. More investigation needed.
		https://github.com/ekoeppen/tntk/
		http://40hz.org/Pages/Mottek:%202011-01-16 etc.
 NewTen More information needed.
		https://github.com/panicsteve/NewTen/blob/master/Instructions.rtf
 UnixNPI by Victor Rehorst, More information needed.
		http://unixnpi.sourceforge.net


 TSerialChipVoyager : TSerialChip : TProtocol
 ============================================

 +36    0x24: HW Base Address
 +56    0x38: Recieve buffer size
 +100    0x64: bit 7: set if Tx Buffer is full
 +116    0x74: Rx: TSerialDMAEngine*
 +120    0x78: Tx: TSerialDMAEngine*
 +136    0x88: TCircleBuf Rx
 +143    0x8F: b: 02 to handle rx int
 +144    0x90: w:


 Serial port registers starting at 0x0F1C0000
 ============================================

 ? kSerReg_BreakDuplex	= 0x2400,

 0000:  w 40
 0400:  w 06
 0800:  w 00
 0C00:  w 02
 1000:  w 00
 2000:  w 62 (EnableRxDMA writes 62)
 bit 1: write: Clear Rx Error Status?
 2400: rw 25 (EnableTxDMA)
 bit 0: TxDMAEnable
 2800:  w 23 (SetTxDTransceiverEnable writes 23)
 bit 1: write: set outputs? DTR, RTS?
 3000: rw F9 (SetInterruptEnable writes F9) Writes the mask of all enabled interrupts
 3400:  w F9
 3800:  w 19
 3C00:  w 40 (SetIntSourceEnable(0x00000040, 1) writes 40) Setting a bit here enables additional interrupt sources
 bit 7: write: Tx Buffer is empty
 4400: r  00
 bit 7: read:  Tx Buffer is empty
 bit 6: read:  Rx Buffer is full
 bit 5: read:  Rx Byte available?
 bit 4: read:  DCD asserted
 bit 3: read:  CTS asserted
 bit 2: read:  Tx underrun (also in 3000)
 bit 0: read:  Serial abort (also in 3000)
 4800: r  00
 bit 7, 6, 5, 4: read:  Rx Error Status
 5000:  w 00
 5400:  w 02
 5800: r  00
 5C00: r  00
 6000:       tx byte buffer (non-DMA)
 7000:       rx byte buffer (non-DAM)
 8000:  w 00

 // TSerialPortVoyager
 //  +36 : HW Base Address
 // +100, bit 7: set if Tx Buffer is full

 //0x0F1C0000	kExternalSerialBase
 //0x0F1D0000	kInfraredSerialBase
 //0x0F1E0000	kBuiltInSerialBase
 //0x0F1F0000	kModemSerialBase

 // Port base address: 0F1C0000 'extr', 'vshw'=kHMOSerialVoyagerHardware
 // Port base address: 0F1D0000 'infr'
 // Port base address: 0F1E0000 'tblt'
 // Port base address: 0F1F0000 'mdem'
 //  +2000 : bit 1: write: Clear Rx Error Status?
 //  +2400 : bit 0: read/write: enable tx DMA
 //  +2800 : bit 1: write: set outputs? DTR, RTS?
 //  +3000 : read (some status mask?)
 //  +3000 : write: interrupt enable?
 //  +3C00 : bit 7: write: Tx Buffer is empty
 //  +4400 : bit 7: read:  Tx Buffer is empty
 //  +4400 : bit 6: read:  Rx Buffer is full
 //  +4400 : bit 5: read:  Rx Byte available?
 //  +4400 : bit 4: read:  DCD asserted
 //  +4400 : bit 3: read:  CTS asserted
 //  +4400 : bit 2: read:  Tx underrun (also in 3000)
 //  +4400 : bit 0: read:  Serial abort (also in 3000)
 //  +4800 : bit 7, 6, 5, 4: read:  Rx Error Status
 //  +6000 : tx byte buffer
 //  +7000 : rx byte buffer

 compare to Cirrus Logig EP93xx registers:
 quite a bunch of registers there...


 Serial port DMA
 ===============

 Address   Bank, Channel, Register
 Receive:
 F080000 = 1,0,0:  w: rx physical buffer address (InitRxDMA)
 F080400 = 1,0,1:  w: current buffer address? (RxDMAControl)
 F080800 = 1,0,2
 F080C00 = 1,0,3:  w: 00000080 (RxDMAControl), 00000000 (The interrupt that we trigger?)
 F081000 = 1,0,4: rw: 00000403 buffer count? (RxDMAControl) (reading when shutting down?)
 F081400 = 1,0,5:  w: 00000404 buffer size? (RxDMAControl)
 F081800 = 1,0,6:  w: 00000000 (InitRxDMA), 000000FF (RxDMAControl): FF = clear all bits in another register?
 F090000 = 2,0,0:  w: 00000006 (RxDMAControl), 00000000
 F090400 = 2,0,1: r : (RxDMAControl)
 F090800 = 2,0,2:  w: 00000000 (RxDMAControl)
 F090C00 = 2,0,3:  w: 00000006 (InitRxDMA)
 Transmit:
 F082000 = 1,1,0:  w: tx physical buffer address (InitTxDMA)
 F082400 = 1,1,1: rw: physical address of data start
 F082800 = 1,1,2
 F082C00 = 1,1,3:  w: 000000C0 (TxDMAControl) Some Control Register
 F083000 = 1,1,4: rw: number of bytes to write (TxDMAControl)
 F083400 = 1,1,5:  w: 204, buffer size? (TxDMAControl) bytes to end of buffer
 F083800 = 1,1,6:  w: 00000000 (InitTxDMA)
 F091000 = 2,1,0:  w: 00000002 (TxDMAControl)
 F091400 = 2,1,1: r : (TxDMAControl)
 F091800 = 2,1,2:  w: 00000000 (TxDMAControl) (whatever we read from 2,1,1)
 F091C00 = 2,1,3:  w: 00000002 (InitTxDMA)

 compare to Cirrus Logig EP93xx registers:
 CONTROL rw  flags that enable run modi and interrupts
 INTERRUPT rw  write bits to clear interrupt flag, read to see which int was triggered
 PPALLOC rw  declare the periphery that connects to this DMA
 STATUS ro  status of the state machine: running, stalling, stop, etc.
 REMAIN ro  number of bytes remaining in the current DMA transfer
 MAXCNT1 rw  maximum byte count for this buffer
 BASE1 rw  base address for the current and next DMA transfer
 CURRENT1 ro  current position, read BASE when DMA is started

 // ^^ control, base address, current pointer, size, count, word, compare, interrupt mask

 AssignmentRegister:     InitByOption is setting bit 2
 EnableRegister:      w: 00000001, 00000002 (RxDMAControl) // Bits set to 1 will start the DMA transfers.
 StatusRegister:         Bits set to 1 indicate pending dma transfers.
 DisableRegister:     w: 00000001, 00000002 // Bits set to 1 will abort the DMA transfers.
 WordStatusRegister: r : Bits set to 1 indicate words in channel word registers.

// ---

 DMA Channel Assignemnet Register (global)
 - the channel assignment is static in the Newton:
   * Ch0 = ser 0 rx
   * Ch1 = ser 0 tx, etc.

 DMA Enable Register (global)
 - enables individual DMA channels using a mask
 - works as a status register: reading it determins if a channel is enabled
 - when DMA operation completes, the corresponding bit is cleared
 
 DMA disable Register (global)
 - setting a bit will pause DMA operation on that channel and clear the Enable Register bit
 - DMA also automatically disables when:
   * wrapping is disabled and the count registeer goes to 0
   * the size register goes to 0
   * chan 0 receives End Of Frame form SCC Channel 0

 Word Status Register (global)
 - a bit is set if a valid word still exists in the word register of that channel
   read that word to complete and= interrupted DMA operation
 
// - per channel registers
 
 Control Register
 - used to configure the channel (varies per channel)
 
 Base Register
 - physical start address of DMA buffer (or next buffer during chained DMA)
 - word aligned
 
 Pointer Register
 - can point anywhere in the buffer
 - word aligned! Read a word and store it in the Word Register, increment by four!
 - if wrapping is enabled, will be loaded with Base Register on wrap
 
 Count Register
 - number of byte remaining to be transferred, decremented by one after every byte sent
 - after wrapping, will be set to the vaule in Size Register
 
 Size Register
 - can be configure to hold the buffer size. Count is loaded with Size after a wrap
 - or size in byte of DMA packet, decrements by 4 if a word is read into the word buffer until0
 
 Word Register
 - four bytes are received and stored in the word register 
 - when the register is 'full', data is copied to memory
 - if the received data had a non-module 4 size, the word register must be read as it contains the remaining bytes!

 Compare Register
 - generate an interrupt when the size register equals this value.
 
 
 DMA Interface SCC registers are:
 Tx_CMD
 Rx_CMD

// ---
 


 Serial port interrupts
 ======================

 kDMAChannel0IntMask	= 0x00000080,						// Serial port 0 rcv
 kDMAChannel1IntMask	= 0x00000100,						// Serial port 0 tx

 compare to Cirrus Logig EP93xx registers:
 DMA interrupts are the exact same!
 There are two interrupts (23, 24) for bytebang serial IO
	Registers:
 IRQ Status ro
 FIQ Status ro
 Raw Status ro
 Int Select rw
 Int Enable rw
 Int Clear Enable wo
 Software Int wo (write a 1 to trigger an int)
 Software Int clear wo


 Which register holds the values below?
 typedef ULong SerialIntSource;	// matches Voyager interrupt mask reg bits
 #define kSerIntSrcAbort		(0x00000001)	// break or abort
 #define kSerIntSrcHunt			(0x00000002)
 #define kSerIntSrcUnderRun		(0x00000004)
 #define kSerIntSrcCTS			(0x00000008)
 #define kSerIntSrcDCD			(0x00000010)
 #define kSerIntSrcRxSpecial		(0x00000020)
 #define kSerIntSrcRxOnAllChars		(0x00000040)
 #define kSerIntSrcTxBufEmpty		(0x00000080)
 #define kSerIntSrcRxOnFirstChar	(0x00000100)	// may go away
 */


#if TARGET_OS_MAC
/*
 * This is a hack that removes a reference from the 'extr' serial port driver
 * from the REx. This is required to make low level comm emulation possible.
 * Removing this patch is not harmful beyond disabling comm emulation.
 * Keeping the patch should not be harmful either.
 *
 * Find the pattern 'extr\0\0\0\0' in the first 1MB of the REx.
 */
TJITGenericPatchFindAndReplace gEnableSerialPort(0x00800634, 0x00900000,
												 (KUInt32[]){2, 'extr', 0},
												 (KUInt32[]){1, '~xtr'},
												 "Enable 'extr' serial port.\n");
#endif


// -------------------------------------------------------------------------- //
//  * TBasicSerialPortManager()
//		Create a DMA and Interrupt emulation for a serial port.
//		The original Newton ROM does not seem to have the driver system via
//		ROM Extension sufficiently implemented. To circumvent patching a
//		multitude of ROMs, Einstein emulates the hardware at the lowest
//		level.
//
//		The registers for serial hardware access, DMA access, and interrupt
//		documentation has not been accessible as of February 2017. All
//		implementations of this driver are based on observation on the
//		original NewtonOS and are emulated as needed. Emulation is by
//		no means complete, or correct.
//
// TODO: This class should be made more universal and then be derived into
//		three new classes to emulate the external serial port, the modem,
//		and possibly the third internal port.
// TODO: it would be nice to implement access to a host hardware serial
//		port, especially for mobile device. Maybe a remote serial port
//		via TCP/IP would be useful?
// FIXME: the current implementation is for MacOS only. We should add MSWindows,
//		Android, and iOS support, or at least make sure that compilation
//		on those systems does not break.
// -------------------------------------------------------------------------- //
TBasicSerialPortManager::TBasicSerialPortManager(
													 TLog* inLog,
													 ELocationID inLocationID)
:	TSerialPortManager(inLog, inLocationID),

	mTxDMAPhysicalBufferStart(0),
	mTxDMAPhysicalData(0),
	mTxDMADataCountdown(0),
	mTxDMAControl(0),
	mTxDMAEvent(0),

	mRxDMAPhysicalBufferStart(0),
	mRxDMAPhysicalData(0),
	mRxDMADataCountdown(0),
	mRxDMAControl(0),
	mRxDMAEvent(0)
{
}


// -------------------------------------------------------------------------- //
//  * ~TBasicSerialPortManager( void )
//		Don't worry about releasing  resources. All drivers will live
//		throughout the run time of the app.
// -------------------------------------------------------------------------- //
TBasicSerialPortManager::~TBasicSerialPortManager()
{
}

// -------------------------------------------------------------------------- //
//  * run( TInterruptManager*, TDMAManager*, TMemory* )
// -------------------------------------------------------------------------- //
void TBasicSerialPortManager::run(TInterruptManager* inInterruptManager,
							 TDMAManager* inDMAManager,
							 TMemory* inMemory)
{
	mInterruptManager = inInterruptManager;
	mDMAManager = inDMAManager;
	mMemory = inMemory;
	// nothing to do here
}

// -------------------------------------------------------------------------- //
//  * WriteRegister( KUInt32, KUInt8 )
//		This function is called whenever the TMemory class determines a write
//		operation to a memory address that represents a hardware register
//		associted with this serial port.
//
//		Hardware registers control the baud rate, modem signals, but also
//		some DMA activity. All serial ports can be configured to use no DMA
//		at all and instead read and write directly to the serial port via
//		hardware register access alone.
//
//		Einstein does not implement many registers. In particular, data
//		transfer via hardware registers is not implemented at all. "Dock" and
//		"Inspector" use DMA transfer only.
//
// TODO: If we decide to support the host's serial port hardware, we will have
//		to put a sizeable amount of reasearch into understanding the hardware
//		registers. For communication via named pipe or software null modem,
//		emulation can stay at a minimum.
// -------------------------------------------------------------------------- //
void
TBasicSerialPortManager::WriteRegister( KUInt32 inOffset, KUInt8 inValue )
{
//	printf("***** 'extr' serial, writing unknown register 0x%08X = 0x%08X\n", inOffset, inValue);

	if (mLog)
	{
		mLog->FLogLine(
					   "[%c%c%c%c] - Write %.2X to serial register %.4X",
					   (char) ((mLocationID >> 24) & 0xFF),
					   (char) ((mLocationID >> 16) & 0xFF),
					   (char) ((mLocationID >> 8) & 0xFF),
					   (char) ((mLocationID) & 0xFF),
					   (unsigned int) inValue,
					   (unsigned int) inOffset );
	}
	switch (inOffset) {
		case 0x2400:
			TriggerEvent(kSerCmd_TxCtrlChanged);
			break;
	}
}


// -------------------------------------------------------------------------- //
//  * ReadRegister( KUInt32 )
//		This function is called whenever the TMemory class determines a read
//		operation to a memory address that represents a hardware register
//		associted with this serial port.
// -------------------------------------------------------------------------- //
KUInt8
TBasicSerialPortManager::ReadRegister( KUInt32 inOffset )
{
//	printf("***** 'extr' serial, reading unknown register 0x%08X\n", inOffset);

	KUInt8 theResult = 0;

	if (inOffset == 0x4400)
	{
		// TODO: what is this? Please write more documentation
		// Both buffers are empty for now.
		// We also don't want a beacon.
		theResult = 0x80;
	} else if (inOffset == 0x4800) {
		// TODO: what is this? Please write more documentation
		// RxEOF
		//		theResult = 0x80;
		//		TDebugger::BreakInDebugger();
	} else {
		if (mLog)
		{
			mLog->FLogLine(
						   "[%c%c%c%c] - Read unknown serial register %.4X : %.2X",
						   (char) ((mLocationID >> 24) & 0xFF),
						   (char) ((mLocationID >> 16) & 0xFF),
						   (char) ((mLocationID >> 8) & 0xFF),
						   (char) ((mLocationID) & 0xFF),
						   (unsigned int) inOffset,
						   (unsigned int) theResult );
		}
	}
	
	return theResult;
}


// -------------------------------------------------------------------------- //
//  * ReadDMARegister( KUInt32, KUInt32, KUInt32 )
//		TDMAManage has determined that the CPU reads a memory address that
//		refers to a DMA register associted with this serial port.
//		This function dispatches to the DMA emulation.
// -------------------------------------------------------------------------- //
KUInt32
TBasicSerialPortManager::ReadDMARegister( KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister )
{
	if (inChannel==TDMAManager::kSerialPort0Receive) {
		return ReadRxDMARegister(inBank, inRegister);
	} else if (inChannel==TDMAManager::kSerialPort0Transmit) {
		return ReadTxDMARegister(inBank, inRegister);
	} else {
		return TSerialPortManager::ReadDMARegister(inBank, inChannel, inRegister);
	}
}


// -------------------------------------------------------------------------- //
//  * WriteDMARegister( KUInt32, KUInt32, KUInt32, KUInt32 )
//		TDMAManage has determined that the CPU writes a memory address that
//		refers to a DMA register associted with this serial port.
//		This function dispatches to the DMA emulation.
// -------------------------------------------------------------------------- //
void
TBasicSerialPortManager::WriteDMARegister( KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister, KUInt32 inValue )
{
	if (inChannel==TDMAManager::kSerialPort0Receive) {
		return WriteRxDMARegister(inBank, inRegister, inValue);
	} else if (inChannel==TDMAManager::kSerialPort0Transmit) {
		return WriteTxDMARegister(inBank, inRegister, inValue);
	} else {
		return TSerialPortManager::WriteDMARegister(inBank, inChannel, inRegister, inValue);
	}
}


// -------------------------------------------------------------------------- //
//  * ReadRxDMARegister( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TBasicSerialPortManager::ReadRxDMARegister( KUInt32 inBank, KUInt32 inRegister )
{
	KUInt32 result = 0;

	if (inBank==1) {
		switch (inRegister) {
			case 0: result = mRxDMAPhysicalBufferStart; break;
			case 1: result = mRxDMAPhysicalData; break;
			case 3: result = 0; break;
			case 4: result = mRxDMADataCountdown; break;
			case 5: result = mRxDMABufferSize; break;
			case 6: result = 0; break;
			case 2:
			default:
				printf("***** 'extr' serial Rx, reading unknown DMA register %d %d %d\n", inBank, 0, inRegister);
				break;
		}
	} else if (inBank==2) {
		switch (inRegister) {
			case 0: result = mRxDMAControl; break;
			case 1: // TSerialDMAEngine::StartRxDMA reading
				//printf("----- 'extr' serial Rx DMA, reading interrupt reason (?) %d %d %d (0x%08X)\n", inBank, 0, inRegister, mRxDMAEvent);
				result = mRxDMAEvent; break; // FIXME: additional action needed?
			case 2: result = 0; break;
			case 3: result = 0; break;
			default:
				printf("***** 'extr' serial Rx, reading unknown DMA register %d %d %d\n", inBank, 0, inRegister);
				break;
		}
	}

	return result;
}


// -------------------------------------------------------------------------- //
//  * WriteRxDMARegister( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TBasicSerialPortManager::WriteRxDMARegister( KUInt32 inBank, KUInt32 inRegister, KUInt32 inValue )
{
	if (inBank==1) {
		switch (inRegister) {
			case 0: // TSerialDMAEngine::BindToBuffer buffer start
				//printf("----- 'extr' serial Rx DMA, set buffer start %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMAPhysicalBufferStart = inValue; break;
			case 1: // TSerialDMAEngine::StartRxDMA data start
				//printf("----- 'extr' serial Rx DMA, set data start %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMAPhysicalData = inValue; break;
			case 3: // TSerialDMAEngine::StartRxDMA writes 00000080
				//printf("----- 'extr' serial Rx DMA, set ??? %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break; // FIXME: will other values be written here? What do they do?
			case 4: // TSerialDMAEngine::StartRxDMA buffer max count
				//printf("----- 'extr' serial Rx DMA, set data count %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMADataCountdown = inValue; break;
			case 5: // TSerialDMAEngine::StartRxDMA buffer size
				//printf("----- 'extr' serial Rx DMA, set buffer size %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMABufferSize = inValue; break;
			case 6:
				// TSerialDMAEngine::BindToBuffer writing 00000000
				// TSerialDMAEngine::StartRxDMA writing 000000FF
				//printf("----- 'extr' serial Rx DMA, set ??? %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break; // FIXME: additional action needed?
			case 2:
			default:
				printf("***** 'extr' serial Rx, writing unknown DMA register %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break;
		}
	} else if (inBank==2) {
		switch (inRegister) {
			case 0: // TSerialDMAEngine::Init writing 00000000, TSerialDMAEngine::StartRxDMA writing 00000006
				//printf("----- 'extr' serial Rx DMA, set control register %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMAControl = inValue; break;
			case 1:
				//printf("----- 'extr' serial Rx DMA, set interrupt reason %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMAEvent = inValue; break;
			case 2: // TSerialDMAEngine::StartRxDMA writing 00000000
				// in all likelyhodd, this clears the events that were triggered and read from mRxDMAEvent
				//printf("----- 'extr' serial Rx DMA, clear event mask %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMAEvent &= ~inValue; break; // FIXME: additional action needed?
			case 3: // TSerialDMAEngine::ConfigureInterrupts writing 00000006
				//printf("----- 'extr' serial Rx DMA, set interrupt select (?) %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break; // FIXME: additional action needed?
			default:
				printf("***** 'extr' serial Rx, writing unknown DMA register %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break;
		}
	}
}


// -------------------------------------------------------------------------- //
//  * ReadTxDMARegister( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TBasicSerialPortManager::ReadTxDMARegister( KUInt32 inBank, KUInt32 inRegister )
{
	KUInt32 result = 0;

	if (inBank==1) {
		switch (inRegister) {
			case 0: result = mTxDMAPhysicalBufferStart; break;
			case 1: result = mTxDMAPhysicalData; break;
			case 3: result = 0; break;
			case 4: result = mTxDMADataCountdown; break;
			case 5: result = mTxDMABufferSize; break;
			case 6: result = 0; break;
			case 2:
			default:
				printf("***** 'extr' serial Tx, reading unknown DMA register %d %d %d\n", inBank, 1, inRegister);
				break;
		}
	} else if (inBank==2) {
		switch (inRegister) {
			case 0: result = mTxDMAControl; break;
			case 1:
				// TSerialDMAEngine::StartTxDMA reads this register
				// TSerialDMAEngine::DMAInterrupt reads this register as a very first step!
				//printf("----- 'extr' serial Tx DMA, reading interrupt reason (?) %d %d %d (0x%08X)\n", inBank, 1, inRegister, mTxDMAEvent);
				result = mTxDMAEvent; break; // FIXME: additional action needed?
			case 2: result = 0; break;
			case 3: result = 0; break;
			default:
				printf("***** 'extr' serial Tx, reading unknown DMA register %d %d %d\n", inBank, 1, inRegister);
				break;
		}
	}

	return result;
}


// -------------------------------------------------------------------------- //
//  * WriteTxDMARegister( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TBasicSerialPortManager::WriteTxDMARegister( KUInt32 inBank, KUInt32 inRegister, KUInt32 inValue )
{
	if (inBank==1) {
		switch (inRegister) {
			case 0: // TSerialDMAEngine::BindToBuffer writing the start address of the buffer
				//printf("----- 'extr' serial Tx DMA, set buffer start %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMAPhysicalBufferStart = inValue; break;
			case 1: // TSerialDMAEngine::StartTxDMA writes the start address of the transfer
				//printf("----- 'extr' serial Tx DMA, set data start %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMAPhysicalData = inValue; break;
			case 3: // TSerialDMAEngine::StartTxDMA writes 000000C0
				//printf("----- 'extr' serial Tx DMA, set ??? %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				break; // FIXME: will other values be written here? What do they do?
			case 4: // TSerialDMAEngine::StartTxDMA writes the number of bytes to be sent
				//printf("----- 'extr' serial Tx DMA, set data count %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMADataCountdown = inValue; break;
			case 5: // TSerialDMAEngine::StartTxDMA writes the size of the buffer
				//printf("----- 'extr' serial Tx DMA, set buffer size %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMABufferSize = inValue; break;
			case 6: // TSerialDMAEngine::BindToBuffer writing 0
				//printf("----- 'extr' serial Tx DMA, set ??? %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				break; // FIXME: additional action needed?
			case 2:
			default:
				printf("***** 'extr' serial Tx, writing unknown DMA register %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				break;
		}
	} else if (inBank==2) {
		switch (inRegister) {
			case 0:
				// TSerialDMAEngine::Init writes 0 (probably disabeling the entire DMA channel)
				// TSerialDMAEngine::StartTxDMA writes 00000002 (probably to prepare the DMA, write DMA enable 00000002 is next.)
				// TSerialDMAEngine::StopTxDMA writes 0
				//printf("----- 'extr' serial Tx DMA, set control register %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMAControl = inValue; break;
			case 1:
				//printf("----- 'extr' serial Tx DMA, set interrupt reason %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMAEvent = inValue; break;
			case 2:
				// TSerialDMAEngine::StartTxDMA writes whatever it reads in 2.1.1
				// TSerialDMAEngine::DMAInterrupt set this to 0 after reading 2.1.1
				// in all likelyhodd, this clears the events that were triggered and read from mTxDMAEvent
				//printf("----- 'extr' serial Tx DMA, clear event mask %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMAEvent &= ~inValue; break; // FIXME: additional action needed?
			case 3: // TSerialDMAEngine::ConfigureInterrupts writes 00000002
				//printf("----- 'extr' serial Tx DMA, set interrupt select (?) %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				break; // FIXME: additional action needed?
			default:
				printf("***** 'extr' serial Tx, writing unknown DMA register %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				break;
		}
	}
	return;
}


// -------------------------------------------------------------------------- //
// Enable the ROM patches below to get an overview of the calls related to
// serial communication. I left those in for reference and if anyone feels like
// taking a peek at the complexity of a serial port via DMA.
// -------------------------------------------------------------------------- //

#if 0

T_ROM_INJECTION(0x001D6780, "Init__18TSerialChipVoyagerFP11TCardSocketP12TCardHandlerPUc") {
	printf("0x001D6780: TSerialChipVoyager::Init(TCardSocket *, TCardHandler *, unsigned char *)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D678C, "InitByOption__18TSerialChipVoyagerFP7TOption") {
	printf("0x001D678C: TSerialChipVoyager::InitByOption(TOption *)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6A14, "InstallChipHandler__18TSerialChipVoyagerFPvP14SCCChannelInts") {
	printf("0x001D6A14: TSerialChipVoyager::InstallChipHandler(void *, SCCChannelInts *)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6A5C, "RemoveChipHandler__18TSerialChipVoyagerFPv") {
	printf("0x001D6A5C: TSerialChipVoyager::RemoveChipHandler(void *)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6A88, "InitializeForNextHandler__18TSerialChipVoyagerFv") {
	printf("0x001D6A88: TSerialChipVoyager::InitializeForNextHandler(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6AF0, "PutByte__18TSerialChipVoyagerFUc") {
	printf("0x001D6AF0: TSerialChipVoyager::PutByte(unsigned char)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6B3C, "ResetTxBEmpty__18TSerialChipVoyagerFv") {
	printf("0x001D6B3C: TSerialChipVoyager::ResetTxBEmpty(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6B60, "GetByte__18TSerialChipVoyagerFv") {
	printf("0x001D6B60: TSerialChipVoyager::GetByte(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6B70, "TxBufEmpty__18TSerialChipVoyagerFv") {
	printf("0x001D6B70: TSerialChipVoyager::TxBufEmpty(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6B84, "RxBufFull__18TSerialChipVoyagerFv") {
	printf("0x001D6B84: TSerialChipVoyager::RxBufFull(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6BAC, "GetRxErrorStatus__18TSerialChipVoyagerFv") {
	printf("0x001D6BAC: TSerialChipVoyager::GetRxErrorStatus(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6BD8, "GetSerialStatus__18TSerialChipVoyagerFv") {
	printf("0x001D6BD8: TSerialChipVoyager::GetSerialStatus(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6C6C, "ResetSerialStatus__18TSerialChipVoyagerFv") {
	printf("0x001D6C6C: TSerialChipVoyager::ResetSerialStatus(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6C70, "SetSerialOutputs__18TSerialChipVoyagerFUl") {
	printf("0x001D6C70: TSerialChipVoyager::SetSerialOutputs(unsigned long)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6CA0, "ClearSerialOutputs__18TSerialChipVoyagerFUl") {
	printf("0x001D6CA0: TSerialChipVoyager::ClearSerialOutputs(unsigned long)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6CCC, "GetSerialOutputs__18TSerialChipVoyagerFv") {
	printf("0x001D6CCC: TSerialChipVoyager::GetSerialOutputs(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6CFC, "PowerOff__18TSerialChipVoyagerFv") {
	printf("0x001D6CFC: TSerialChipVoyager::PowerOff(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6E58, "PowerOn__18TSerialChipVoyagerFv") {
	printf("0x001D6E58: TSerialChipVoyager::PowerOn(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6F70, "PowerIsOn__18TSerialChipVoyagerFv") {
	printf("0x001D6F70: TSerialChipVoyager::PowerIsOn(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6F78, "SetInterruptEnable__18TSerialChipVoyagerFUc") {
	UInt32 r1 = ioCPU->GetRegister(1);
	printf("0x001D6F78: TSerialChipVoyager::SetInterruptEnable(%d)\n", (unsigned)r1);
	return ioUnit;
}

T_ROM_INJECTION(0x001D6FA8, "Reset__18TSerialChipVoyagerFv") {
	printf("0x001D6FA8: TSerialChipVoyager::Reset(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6FAC, "SetBreak__18TSerialChipVoyagerFUc") {
	printf("0x001D6FAC: TSerialChipVoyager::SetBreak(unsigned char)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D6FD4, "SetSpeed__18TSerialChipVoyagerFUl") {
	UInt32 r1 = ioCPU->GetRegister(1);
	printf("0x001D6FD4: TSerialChipVoyager::SetSpeed(%d)\n", (unsigned)r1);
	return ioUnit;
}

T_ROM_INJECTION(0x001D7148, "SetIOParms__18TSerialChipVoyagerFP17TCMOSerialIOParms") {
	printf("0x001D7148: TSerialChipVoyager::SetIOParms(TCMOSerialIOParms *)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D7204, "Reconfigure__18TSerialChipVoyagerFv") {
	printf("0x001D7204: TSerialChipVoyager::Reconfigure(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D7364, "GetFeatures__18TSerialChipVoyagerFv") {
	printf("0x001D7364: TSerialChipVoyager::GetFeatures(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D73DC, "ProcessOption__18TSerialChipVoyagerFP7TOption") {
	printf("0x001D73DC: TSerialChipVoyager::ProcessOption(TOption *)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D7808, "SetSerialMode__18TSerialChipVoyagerFUl") {
	KUInt32 r1 = ioCPU->GetRegister(1);
	char buf[128] = { 0 };
	switch (r1&3) {
		case 0: strcpy(buf, "kSerModeAsync"); break;
		case 1: strcpy(buf, "kSerModeSync"); break;
		case 2: strcpy(buf, "kSerModeLocalTalk"); break;
		default: strcpy(buf, "unknown"); break;
	}
	if (r1&0x04) {
		strcat(buf, " | kSerModeHalfDuplex");
	}
	if (r1&0x08) {
		strcat(buf, " | kSerModePolled");
	}
	printf("0x001D7808: TSerialChipVoyager::SetSerialMode(0x%08X=\"%s\")\n", (unsigned)r1, buf);
	return ioUnit;
}

T_ROM_INJECTION(0x001D7920, "SysEventNotify__18TSerialChipVoyagerFUl") {
	printf("0x001D7920: TSerialChipVoyager::SysEventNotify(unsigned long)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D793C, "SetTxDTransceiverEnable__18TSerialChipVoyagerFUc") {
	KUInt32 r1 = ioCPU->GetRegister(1);
	printf("0x001D793C: TSerialChipVoyager::SetTxDTransceiverEnable(%d)\n", (unsigned)r1);
	return ioUnit;
}

T_ROM_INJECTION(0x001D799C, "GetByteAndStatus__18TSerialChipVoyagerFPUc") {
	printf("0x001D799C: TSerialChipVoyager::GetByteAndStatus(unsigned char *)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D79DC, "SetIntSourceEnable__18TSerialChipVoyagerFUlUc") {
	KUInt32 r1 = ioCPU->GetRegister(1);
	KUInt32 r2 = ioCPU->GetRegister(2);
	char buf[1024];
	buf[0] = 0;
	if (r1&0x001) strcat(buf, "Abort ");
	if (r1&0x002) strcat(buf, "Hunt ");
	if (r1&0x004) strcat(buf, "UnderRun ");
	if (r1&0x008) strcat(buf, "CTS ");
	if (r1&0x010) strcat(buf, "DCD ");
	if (r1&0x020) strcat(buf, "RxSpecial ");
	if (r1&0x040) strcat(buf, "RxOnAllChars ");
	if (r1&0x080) strcat(buf, "TxBufEmpty ");
	if (r1&0x100) strcat(buf, "RxOnFirstChar ");
	printf("0x001D79DC: TSerialChipVoyager::SetIntSourceEnable(0x%08X=\"%s\", enable=%d)\n", (unsigned)r1, buf, (unsigned)r2);
	return ioUnit;
}

T_ROM_INJECTION(0x001D7A5C, "AllSent__18TSerialChipVoyagerFv") {
	printf("0x001D7A5C: TSerialChipVoyager::AllSent(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D7A78, "WaitForAllSent__18TSerialChipVoyagerFv") {
	printf("0x001D7A78: TSerialChipVoyager::WaitForAllSent(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D7ABC, "ConfigureForOutput__18TSerialChipVoyagerFUc") {
	printf("0x001D7ABC: TSerialChipVoyager::ConfigureForOutput(unsigned char)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D7BCC, "SetSDLCAddress__18TSerialChipVoyagerFUc") {
	printf("0x001D7BCC: TSerialChipVoyager::SetSDLCAddress(unsigned char)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D7C00, "ReEnableReceiver__18TSerialChipVoyagerFUc") {
	printf("0x001D7C00: TSerialChipVoyager::ReEnableReceiver(unsigned char)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D7CC4, "InitTxDMA__18TSerialChipVoyagerFP10TCircleBufPFPv_v") {
	KUInt32 r2 = ioCPU->GetRegister(2);
	printf("0x001D7CC4: TSerialChipVoyager::InitTxDMA(TCircleBuf *, void (*)(void *)=0x%08X)\n", unsigned(r2));
	// For the external serial port and the Docking app, this is:
	// TxDMAInterrupt__13TAsyncSerToolFv:
	// @ 0x0003A7C4: TAsyncSerTool::TxDMAInterrupt(void)
	return ioUnit;
}

T_ROM_INJECTION(0x001D7D70, "InitRxDMA__18TSerialChipVoyagerFP10TCircleBufUlPFPvUl_v") {
	KUInt32 r2 = ioCPU->GetRegister(2);
	KUInt32 r3 = ioCPU->GetRegister(3);
	printf("0x001D7D70: TSerialChipVoyager::InitRxDMA(TCircleBuf *, notifyLevel=0x%08X, void (*)(void *, unsigned long)=0x%08X)\n", unsigned(r2), unsigned(r3));
	// For the external serial port and the Docking app, this is:
	// RxMultiByteInterrupt__13TAsyncSerToolFUl:
	// @ 0x0003ABC8: TAsyncSerTool::RxMultiByteInterrupt(unsigned long)
	return ioUnit;
}

T_ROM_INJECTION(0x001D7DF0, "TxDMAControl__18TSerialChipVoyagerFUc") {
	KUInt32 r1 = ioCPU->GetRegister(1);
	char buf[64] = { 0 };
	switch (r1&15) {
		case 0: strcpy(buf, "kDMANoOp"); break;
		case 1: strcpy(buf, "kDMAStart"); break;
		case 2: strcpy(buf, "kDMAStop"); break;
		case 3: strcpy(buf, "kDMASuspend"); break;
		case 4: strcpy(buf, "kDMASync"); break;
		case 5: strcpy(buf, "kDMAFlush"); break;
		default: strcpy(buf, "unknown"); break;
	}
	if (r1&0x10) {
		strcat(buf, " | kDMANotifyOnNext");
	}
	printf("0x001D7DF0: TSerialChipVoyager::TxDMAControl(0x%02X=\"%s\")\n", (unsigned)r1, buf);

	return ioUnit;
}

T_ROM_INJECTION(0x001D7F28, "RxDMAControl__18TSerialChipVoyagerFUc") {
	KUInt32 r1 = ioCPU->GetRegister(1);
	char buf[64] = { 0 };
	switch (r1&15) {
		case 0: strcpy(buf, "kDMANoOp"); break;
		case 1: strcpy(buf, "kDMAStart"); break;
		case 2: strcpy(buf, "kDMAStop"); break;
		case 3: strcpy(buf, "kDMASuspend"); break;
		case 4: strcpy(buf, "kDMASync"); break;
		case 5: strcpy(buf, "kDMAFlush"); break;
		default: strcpy(buf, "unknown"); break;
	}
	if (r1&0x10) {
		strcat(buf, " | kDMANotifyOnNext");
	}
	printf("0x001D7F28: TSerialChipVoyager::RxDMAControl(0x%02X=\"%s\")\n", (unsigned)r1, buf);
	return ioUnit;
}

T_ROM_INJECTION(0x001D8088, "LinkIsFree__18TSerialChipVoyagerFUc") {
	printf("0x001D8088: TSerialChipVoyager::LinkIsFree(unsigned char)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D80B4, "SendControlPacket__18TSerialChipVoyagerFUcN21") {
	printf("0x001D80B4: TSerialChipVoyager::SendControlPacket(unsigned char, unsigned char, unsigned char)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8180, "WaitForPacket__18TSerialChipVoyagerFUl") {
	printf("0x001D8180: TSerialChipVoyager::WaitForPacket(unsigned long)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D81C8, "RegisterPrimaryInterrupt__18TSerialChipVoyagerFv") {
	printf("0x001D81C8: TSerialChipVoyager::RegisterPrimaryInterrupt(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8238, "ReconfigureIOParms__18TSerialChipVoyagerFv") {
	printf("0x001D8238: TSerialChipVoyager::ReconfigureIOParms(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D82D4, "ReconfigureAll__18TSerialChipVoyagerFv") {
	printf("0x001D82D4: TSerialChipVoyager::ReconfigureAll(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8380, "IRCmdTxInterrupt__18TSerialChipVoyagerFv") {
	printf("0x001D8380: TSerialChipVoyager::IRCmdTxInterrupt(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D83E0, "IRCmdOtherInterrupt__18TSerialChipVoyagerFv") {
	printf("0x001D83E0: TSerialChipVoyager::IRCmdOtherInterrupt(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8420, "ConfigureIRChip__18TSerialChipVoyagerFUc") {
	printf("0x001D8420: TSerialChipVoyager::ConfigureIRChip(unsigned char)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D885C, "Sizeof__18TSerialChipVoyagerSFv") {
	printf("0x001D885C: static TSerialChipVoyager::Sizeof(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8864, "EnableTVModeIR__18TSerialChipVoyagerFP17TCMOSlowIRBitBang") {
	printf("0x001D8864: TSerialChipVoyager::EnableTVModeIR(TCMOSlowIRBitBang *)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8914, "WriteNextByteWithTimeOut__18TSerialChipVoyagerFUc") {
	printf("0x001D8914: TSerialChipVoyager::WriteNextByteWithTimeOut(unsigned char)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8964, "ReadNextByteWithTimeOut__18TSerialChipVoyagerFPUc") {
	printf("0x001D8964: TSerialChipVoyager::ReadNextByteWithTimeOut(unsigned char *)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D89BC, "WaitNextByteWithTimeOut__18TSerialChipVoyagerFv") {
	printf("0x001D89BC: TSerialChipVoyager::WaitNextByteWithTimeOut(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8A00, "EnableRxDMA__18TSerialChipVoyagerFv") {
	printf("0x001D8A00: TSerialChipVoyager::EnableRxDMA(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8A24, "DisableRxDMA__18TSerialChipVoyagerFv") {
	printf("0x001D8A24: TSerialChipVoyager::DisableRxDMA(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8A48, "TxDMAEnable__18TSerialChipVoyagerFi") {
	KUInt32 r1 = ioCPU->GetRegister(1);
	printf("0x001D8A48: TSerialChipVoyager::TxDMAEnable(%d)\n", (unsigned)r1);
	return ioUnit;
}

T_ROM_INJECTION(0x001D8A84, "StartSendDMAPacket__18TSerialChipVoyagerFv") {
	printf("0x001D8A84: TSerialChipVoyager::StartSendDMAPacket(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8BB8, "ReceivePacket__18TSerialChipVoyagerFv") {
	printf("0x001D8BB8: TSerialChipVoyager::ReceivePacket(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8D84, "DCDInterrupt__18TSerialChipVoyagerFv") {
	printf("0x001D8D84: TSerialChipVoyager::DCDInterrupt(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8D90, "New__18TSerialChipVoyagerFv") {
	printf("0x001D8D90: TSerialChipVoyager::New(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8E10, "SetModemDCDIntEnable__18TSerialChipVoyagerFUc") {
	KUInt32 r1 = ioCPU->GetRegister(1);
	printf("0x001D8E10: TSerialChipVoyager::SetModemDCDIntEnable(%d)\n", (unsigned)r1);
	return ioUnit;
}

T_ROM_INJECTION(0x001D8E48, "SerialInterrupt__18TSerialChipVoyagerFv") {
	printf("0x001D8E48: TSerialChipVoyager::SerialInterrupt(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8F8C, "AsyncDMAOutInt__18TSerialChipVoyagerFv") {
	printf("0x001D8F8C: TSerialChipVoyager::AsyncDMAOutInt(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D8FC4, "AsyncDMAInInt__18TSerialChipVoyagerFv") {
	printf("0x001D8FC4: TSerialChipVoyager::AsyncDMAInInt(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D9020, "LocalTalkInterrupt__18TSerialChipVoyagerFv") {
	printf("0x001D9020: TSerialChipVoyager::LocalTalkInterrupt(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D90EC, "LocalTalkDMAOutInt__18TSerialChipVoyagerFv") {
	printf("0x001D90EC: TSerialChipVoyager::LocalTalkDMAOutInt(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D916C, "LTReceiveTimer__18TSerialChipVoyagerFUl") {
	printf("0x001D916C: TSerialChipVoyager::LTReceiveTimer(unsigned long)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D917C, "LocalTalkDMAInInt__18TSerialChipVoyagerFv") {
	printf("0x001D917C: TSerialChipVoyager::LocalTalkDMAInInt(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D91B0, "StopRcvDataPacket__18TSerialChipVoyagerFv") {
	printf("0x001D91B0: TSerialChipVoyager::StopRcvDataPacket(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D920C, "Delete__18TSerialChipVoyagerFv") {
	printf("0x001D920C: TSerialChipVoyager::Delete(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001D9278, "CardRemoved__18TSerialChipVoyagerFv") {
	printf("0x001D9278: TSerialChipVoyager::CardRemoved(void)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x00384A00, "ClassInfo__18TSerialChipVoyagerSFv") {
	printf("0x00384A00: static TSerialChipVoyager::ClassInfo(void)\n");
	return ioUnit;
}



T_ROM_INJECTION(0x001B975C, "PutBytes__8TSerToolFP11CBufferList") {
	printf("0x001B975C: TSerTool::PutBytes(CBufferList *)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001B9778, "PutFramedBytes__8TSerToolFP11CBufferListUc") {
	printf("0x001B9778: TSerTool::PutFramedBytes(CBufferList *, unsigned char)\n");
	return ioUnit;
}

T_ROM_INJECTION(0x001B9794, "StartOutput__8TSerToolFP11CBufferList") {
	KUInt32 r1 = ioCPU->GetRegister(1);
	printf("0x001B9794: TSerTool::StartOutput(CBufferList *0x%08X)\n", (unsigned int)r1);
	return ioUnit;
}




T_ROM_INJECTION(0x001D9304, "StopRxDMA__16TSerialDMAEngineFv") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D9304: (0x%08X) TSerialDMAEngine::StopRxDMA(void)\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D9440, "StartTxDMA__16TSerialDMAEngineFv") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D9440: (0x%08X) TSerialDMAEngine::StartTxDMA(void)\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D94D0, "StartTxDMA__16TSerialDMAEngineFv") {
#if 0
	TEmulator *emu = ioCPU->GetEmulator();
	TDMAManager *dma = emu->GetDMAManager();
	TMemory *mem = ioCPU->GetMemory();
	KUInt32 bufferStart = dma->ReadChannel1Register(1, 0);
	KUInt32 dataStart = dma->ReadChannel1Register(1, 1);
	KUInt32 dataSize = dma->ReadChannel1Register(1, 4);
	KUInt32 bytesToEndOfBuffer = dma->ReadChannel1Register(1, 5);
	printf("Sending %d bytes:\n", (unsigned)dataSize);
	for (int i=0;;++i) {
		KUInt8 data;
		if (dataSize==0) break;
		mem->ReadBP(dataStart, data);
		printf("  0x%08X (%.3d): 0x%02X\n", dataStart, i, data);
		dataSize--;
		dataStart++;
		bytesToEndOfBuffer--;
		if (bytesToEndOfBuffer==0)
			dataStart = bufferStart;
	}
#endif
	return ioUnit;
}

T_ROM_INJECTION(0x001D94D4, "StopTxDMA__16TSerialDMAEngineFUc") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D94D4: (0x%08X) TSerialDMAEngine::StopTxDMA(unsigned char)\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D9550, "DMAInterrupt__16TSerialDMAEngineFv") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D9550: (0x%08X) TSerialDMAEngine::DMAInterrupt(void)\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D9594, "Init__16TSerialDMAEngineFP21TDMAChannelDiscriptorPvUc") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D9594: (0x%08X) TSerialDMAEngine::Init(TDMAChannelDiscriptor *, void *, unsig\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D966C, "BindToBuffer__16TSerialDMAEngineFP10TCircleBufUc") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D966C: (0x%08X) TSerialDMAEngine::BindToBuffer(TCircleBuf *, unsigned char)\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D96F8, "ConfigureInterrupts__16TSerialDMAEngineFUlPFPv_l") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D96F8: (0x%08X) TSerialDMAEngine::ConfigureInterrupts(unsigned long, long (*)(void *))\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D970C, "PauseDMA__16TSerialDMAEngineFUc") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D970C: (0x%08X) TSerialDMAEngine::PauseDMA(unsigned char)\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D972C, "ShareEngine__16TSerialDMAEngineFPv") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D972C: (0x%08X) TSerialDMAEngine::ShareEngine(void *)\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D9734, "StartIn__16TSerialDMAEngineFv") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D9734: (0x%08X) TSerialDMAEngine::StartIn(void)\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D97E0, "StopIn__16TSerialDMAEngineFi") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D97E0: (0x%08X) TSerialDMAEngine::StopIn(int)\n", (unsigned int)r0);
	return ioUnit;
}

T_ROM_INJECTION(0x001D987C, "StartRxDMA__16TSerialDMAEngineFUl") {
	KUInt32 r0 = ioCPU->GetRegister(0);
	printf("0x001D987C: (0x%08X) TSerialDMAEngine::StartRxDMA(unsigned long)\n", (unsigned int)r0);
	return ioUnit;
}

#endif


// ================================================================== //
// You never finish a program, you just stop working on it.           //
//  - Anonymous                                                       //
// ================================================================== //
