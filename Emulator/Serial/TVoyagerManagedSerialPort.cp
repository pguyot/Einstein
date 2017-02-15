// ==============================
// File:			TVoyagerManagedSerialPort.cp
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
#include "TVoyagerManagedSerialPort.h"

// POSIX
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#if !TARGET_OS_WIN32
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif

#include "../Log/TLog.h"
#include "../TInterruptManager.h"
#include "../TDMAManager.h"

#include "TMemory.h"

#define NEWSTYLE

/*
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
 F081800 = 1,0,6:  w: 00000000 (InitRxDMA), 000000FF (RxDMAControl)
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

void crc16(unsigned short &crc, unsigned char d)
{
	static unsigned short crctab[256] = {
		0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
		0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
		0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
		0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
		0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
		0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
		0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
		0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
		0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
		0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
		0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
		0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
		0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
		0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
		0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
		0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
		0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
		0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
		0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
		0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
		0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
		0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
		0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
		0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
		0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
		0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
		0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
		0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
		0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
		0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
		0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
		0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
	};

	crc = ((crc>>8)&0x00ff)^crctab[(crc&0xff)^d];
}



static const KUInt8 esc_ = 0x33;
static KUInt32 txCnt_;
static unsigned char buf[1024];
static KUInt32 nBuf;

void send_block(unsigned char *data, int size)
{
	txCnt_ = 0;

	static unsigned char hdr[] = { 0x16, 0x10, 0x02 };
	static unsigned char ftr[] = { 0x10, 0x03 };

	unsigned char *d = buf;

	int i;
	*d++ = hdr[0];
	*d++ = hdr[1];
	*d++ = hdr[2];
	unsigned short crc = 0;
	for (i=0; i<size; i++) {
		unsigned char c = data[i];
		if (i==2 && data[1]==4)
			c = ++txCnt_;
		crc16(crc, c);
		*d++ = c;
		if (c==0x10)
			*d++ = c;
		if (c==esc_) {
			*d++ = 1;
			crc16(crc, 1);
	  printf("--- CONTROL BLOCK *NOT* changing esc from 0x%02x to 0x%02x\n", esc_, esc_+51);
			//esc_ += 51;
		}
	}
	*d++ = ftr[0];
	*d++ = ftr[1];
	crc16(crc, ftr[1]);
	*d++ = crc;
	*d++ = crc>>8;
	//	write(buf, d-buf);
	nBuf = d-buf;
}



// -------------------------------------------------------------------------- //
//  * TVoyagerManagedSerialPort()
// -------------------------------------------------------------------------- //
TVoyagerManagedSerialPort::TVoyagerManagedSerialPort(
													 TLog* inLog,
													 ELocationID inLocationID,
													 TInterruptManager* inInterruptManager,
													 TDMAManager* inDMAManager,
													 TMemory* inMemory)
:	TVoyagerSerialPort(inLog, inLocationID, inInterruptManager, inDMAManager, inMemory),

	mTxDMAPhysicalBufferStart(0),
	mTxDMAPhysicalData(0),
	mTxDMADataCountdown(0),
	mTxDMAControl(0),
	mTxDMAEvent(0),

	mRxDMAPhysicalBufferStart(0),
	mRxDMAPhysicalData(0),
	mRxDMADataCountdown(0),
	mRxDMAControl(0),
	mRxDMAEvent(0),

	mPipe{-1,-1},
	mSerialPort(-1),
	mDMAIsRunning(false),
	mDMAThread(0L)
{
	// TODO: don't run this unless we get a PowerOn for the serial port
	RunDMA();
}


// -------------------------------------------------------------------------- //
//  * ~TVoyagerManagedSerialPort( void )
// -------------------------------------------------------------------------- //
TVoyagerManagedSerialPort::~TVoyagerManagedSerialPort()
{
}


// -------------------------------------------------------------------------- //
//  * WriteRegister( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TVoyagerManagedSerialPort::WriteRegister( KUInt32 inOffset, KUInt8 inValue )
{
	printf("***** 'extr' serial, writing unknown register 0x%08X = 0x%08X\n", inOffset, inValue);

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
#ifdef NEWSTYLE
			if (mPipe[1]!=-1) {
				KUInt8 cmd = kSerCmd_TxCtrlChanged;
				write(mPipe[1], &cmd, 1);
			}
#else
			if (inValue & 0x01)
				//if (mDMAManager->GetEnabledChannels() & 0x0001)
			{
				printf("===> Start Tx DMA\n");
				KUInt32 dmaStart = mDMAManager->ReadChannel1Register(1, 1);
				KUInt32 dmaSize = mDMAManager->ReadChannel1Register(1, 4);
				for (KUInt32 i=0; i<dmaSize; i++) {
					KUInt8 data = 0;
					mMemory->ReadBP(dmaStart+i, data);
					printf(" tx[%.3d] -> %02X '%c'\n", i, data, isprint(data)?data:'.');
					buf[i] = data;
				}

#if 1
				KUInt32 bufferStart = mDMAManager->ReadChannel1Register(1, 0);
				KUInt32 dataStart = mDMAManager->ReadChannel1Register(1, 1);
				KUInt32 dataSize = mDMAManager->ReadChannel1Register(1, 4);
				KUInt32 bytesToEndOfBuffer = mDMAManager->ReadChannel1Register(1, 5);
				for (int i=0;;++i) {
					KUInt8 data;
					if (dataSize==0) break;
					mMemory->ReadBP(dataStart, data);
					buf[i] = data;
					dataSize--;
					dataStart++;
					bytesToEndOfBuffer--;
					if (bytesToEndOfBuffer==0)
						dataStart = bufferStart;
				}
				mDMAManager->WriteChannel1Register(1, 1, bufferStart);
				mDMAManager->WriteChannel1Register(1, 4, dataSize);
				mDMAManager->WriteChannel2Register(1, 1, 0x00000080);
				mInterruptManager->RaiseInterrupt(0x00000100); // 0x80 = TxBufEmpty, 0x00000180

				// NCX default port: 3679

				static int fd = -1;

				if (fd==-1) {
					fd = open("/dev/ttys016", O_RDWR | O_NOCTTY | O_NONBLOCK);
				}
				if (fd==-1) {
					printf("Error opening serial port %s - %s (%d).\n", "/dev/ttyp8", strerror(errno), errno);
				} else {
					printf("\n\n\n\n---------------> writing %d bytes\n\n\n\n", dmaSize);
					int err = write(fd, buf, dmaSize);
					if (err==-1) {
						printf("Error writing to serial port %s - %s (%d).\n", "/dev/ttyp8", strerror(errno), errno);
					}
					//close(fd);
				}
#endif



				printf("===> End Tx DMA\n");
#if 0
#if 1
				int n = -1;
				for (int j=2; j>0; j--) {
					n = read(fd, buf, 1024);
					if (n<=0)
						sleep(1);
					else
						break;
				}
				if (n==-1) {
					printf("Error reading from serial port %s - %s (%d).\n", "/dev/ttyp8", strerror(errno), errno);
				} else if (n==0) {
					printf("No data yet\n");
				} else {
					printf("----> Received %d bytes data from NCX\n", n);
					KUInt32 dmaRxStart = mDMAManager->ReadChannel1Register(0, 0);
					KUInt32 dmaRxSize = n; //sizeof(LR_ack);
					for (KUInt32 i=0; i<dmaRxSize; i++) {
						KUInt8 data = buf[i];
						mMemory->WriteBP(dmaRxStart+i, data);
						printf(" rx[%.3d] -> %02X '%c'\n", i, data, isprint(data)?data:'.');
					}
					printf("===> Start Rx DMA\n");
					KUInt32 dmaRxPos = mDMAManager->ReadChannel1Register(0, 1);
					mDMAManager->WriteChannel1Register(0, 1, dmaRxPos + dmaRxSize);
					KUInt32 dmaRxBuf = mDMAManager->ReadChannel1Register(0, 4);
					mDMAManager->WriteChannel1Register(0, 4, dmaRxBuf - dmaRxSize);
					mDMAManager->WriteChannel2Register(0, 1, 0x00000040); // ???
					mInterruptManager->RaiseInterrupt(0x00000080); // 0x00000180
					printf("===> End Rx DMA\n");
				}
#else
				static unsigned char LR_ack[] = {
					0x1d, 0x01, 0x02, 0x01, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x01, 0x02, 0x03, 0x01,
					0x08, 0x04, 0x02, 0x40, 0x00, 0x08, 0x01, 0x03, 0x0E, 0x04, 0x02, 0x04, 0x00, 0xFA,
				};

				send_block(LR_ack, sizeof(LR_ack));

				KUInt32 dmaRxStart = mDMAManager->ReadChannel1Register(0, 0);
				KUInt32 dmaRxSize = nBuf; //sizeof(LR_ack);
				for (KUInt32 i=0; i<dmaRxSize; i++) {
					mMemory->WriteBP(dmaRxStart+i, buf[i]);
				}
				printf("===> Start Rx DMA\n");
				KUInt32 dmaRxPos = mDMAManager->ReadChannel1Register(0, 1);
				mDMAManager->WriteChannel1Register(0, 1, dmaRxPos + dmaRxSize);
				KUInt32 dmaRxBuf = mDMAManager->ReadChannel1Register(0, 4);
				mDMAManager->WriteChannel1Register(0, 4, dmaRxBuf - dmaRxSize);
				mDMAManager->WriteChannel2Register(0, 1, 0x00000040);
				//mInterruptManager->RaiseInterrupt(0x00000100); // 0x00000180
				printf("===> End Rx DMA\n");
#endif
#endif
			}
#endif
			break;
	}
}

// -------------------------------------------------------------------------- //
//  * ReadRegister( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TVoyagerManagedSerialPort::ReadRegister( KUInt32 inOffset )
{
	printf("***** 'extr' serial, reading unknown register 0x%08X\n", inOffset);
	/*
	 TSerialDMAEngine Rx = 0x0CCACB38
	 TSerialDMAEngine Tx = 0x0CCACAE8
	 */

	KUInt8 theResult = 0;
	/*	if (mLog)
	 {
		mLog->FLogLine(
	 "[%c%c%c%c] - Read serial register %.4X : %.2X",
	 (char) ((mLocationID >> 24) & 0xFF),
	 (char) ((mLocationID >> 16) & 0xFF),
	 (char) ((mLocationID >> 8) & 0xFF),
	 (char) ((mLocationID) & 0xFF),
	 (unsigned int) inOffset,
	 (unsigned int) theResult );
	 }
	 */
	if (inOffset == 0x4400)
	{
		// Both buffers are empty for now.
		// We also don't want a beacon.
		theResult = 0x80;
	} else if (inOffset == 0x4800) {
		// RxEOF
		//		theResult = 0x80;
		//		TDebugger::BreakInDebugger();
	} else {
		//		TDebugger::BreakInDebugger();
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
// -------------------------------------------------------------------------- //
KUInt32
TVoyagerManagedSerialPort::ReadDMARegister( KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister )
{
	if (inChannel==TDMAManager::kSerialPort0Receive) {
		return ReadRxDMARegister(inBank, inRegister);
	} else if (inChannel==TDMAManager::kSerialPort0Transmit) {
		return ReadTxDMARegister(inBank, inRegister);
	} else {
		return TVoyagerSerialPort::ReadDMARegister(inBank, inChannel, inRegister);
	}
}


// -------------------------------------------------------------------------- //
//  * WriteDMARegister( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TVoyagerManagedSerialPort::WriteDMARegister( KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister, KUInt32 inValue )
{
	if (inChannel==TDMAManager::kSerialPort0Receive) {
		return WriteRxDMARegister(inBank, inRegister, inValue);
	} else if (inChannel==TDMAManager::kSerialPort0Transmit) {
		return WriteTxDMARegister(inBank, inRegister, inValue);
	} else {
		return TVoyagerSerialPort::WriteDMARegister(inBank, inChannel, inRegister, inValue);
	}
}



// -------------------------------------------------------------------------- //
//  * ReadDMARegister( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TVoyagerManagedSerialPort::ReadRxDMARegister( KUInt32 inBank, KUInt32 inRegister )
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
				//printf("***** 'extr' serial Rx, reading unknown DMA register %d %d %d\n", inBank, 0, inRegister);
				break;
		}
	} else if (inBank==2) {
		switch (inRegister) {
			case 0: result = mRxDMAControl; break;
			case 1: // TSerialDMAEngine::StartRxDMA reading
				printf("----- 'extr' serial Rx DMA, reading interrupt reason (?) %d %d %d (0x%08X)\n", inBank, 0, inRegister, mRxDMAEvent);
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
//  * WriteDMARegister( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TVoyagerManagedSerialPort::WriteRxDMARegister( KUInt32 inBank, KUInt32 inRegister, KUInt32 inValue )
{
	if (inBank==1) {
		switch (inRegister) {
			case 0: // TSerialDMAEngine::BindToBuffer buffer start
				printf("----- 'extr' serial Rx DMA, set buffer start %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMAPhysicalBufferStart = inValue; break;
			case 1: // TSerialDMAEngine::StartRxDMA data start
				printf("----- 'extr' serial Rx DMA, set data start %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMAPhysicalData = inValue; break;
			case 3: // TSerialDMAEngine::StartRxDMA writes 00000080
				printf("----- 'extr' serial Rx DMA, set ??? %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break; // FIXME: will other values be written here? What do they do?
			case 4: // TSerialDMAEngine::StartRxDMA buffer max count
				printf("----- 'extr' serial Rx DMA, set data count %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMADataCountdown = inValue; break;
			case 5: // TSerialDMAEngine::StartRxDMA buffer size
				printf("----- 'extr' serial Rx DMA, set buffer size %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMABufferSize = inValue; break;
			case 6:
				// TSerialDMAEngine::BindToBuffer writing 00000000
				// TSerialDMAEngine::StartRxDMA writing 000000FF
				printf("----- 'extr' serial Rx DMA, set ??? %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break; // FIXME: additional action needed?
			case 2:
			default:
				printf("***** 'extr' serial Rx, writing unknown DMA register %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break;
		}
	} else if (inBank==2) {
		switch (inRegister) {
			case 0: // TSerialDMAEngine::Init writing 00000000, TSerialDMAEngine::StartRxDMA writing 00000006
				printf("----- 'extr' serial Rx DMA, set control register %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMAControl = inValue; break;
			case 1:
				printf("----- 'extr' serial Rx DMA, set interrupt reason %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				mRxDMAEvent = inValue; break;
			case 2: // TSerialDMAEngine::StartRxDMA writing 00000000
				printf("----- 'extr' serial Rx DMA, set ??? %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break; // FIXME: additional action needed?
			case 3: // TSerialDMAEngine::ConfigureInterrupts writing 00000006
				printf("----- 'extr' serial Rx DMA, set interrupt select (?) %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break; // FIXME: additional action needed?
			default:
				printf("***** 'extr' serial Rx, writing unknown DMA register %d %d %d = 0x%08X\n", inBank, 0, inRegister, inValue);
				break;
		}
	}
}


// -------------------------------------------------------------------------- //
//  * ReadDMARegister( KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TVoyagerManagedSerialPort::ReadTxDMARegister( KUInt32 inBank, KUInt32 inRegister )
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
				printf("----- 'extr' serial Tx DMA, reading interrupt reason (?) %d %d %d (0x%08X)\n", inBank, 1, inRegister, mTxDMAEvent);
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
//  * WriteDMARegister( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TVoyagerManagedSerialPort::WriteTxDMARegister( KUInt32 inBank, KUInt32 inRegister, KUInt32 inValue )
{
	if (inBank==1) {
		switch (inRegister) {
			case 0: // TSerialDMAEngine::BindToBuffer writing the start address of the buffer
				printf("----- 'extr' serial Tx DMA, set buffer start %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMAPhysicalBufferStart = inValue; break;
			case 1: // TSerialDMAEngine::StartTxDMA writes the start address of the transfer
				printf("----- 'extr' serial Tx DMA, set data start %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMAPhysicalData = inValue; break;
			case 3: // TSerialDMAEngine::StartTxDMA writes 000000C0
				printf("----- 'extr' serial Tx DMA, set ??? %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				break; // FIXME: will other values be written here? What do they do?
			case 4: // TSerialDMAEngine::StartTxDMA writes the number of bytes to be sent
				printf("----- 'extr' serial Tx DMA, set data count %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMADataCountdown = inValue; break;
			case 5: // TSerialDMAEngine::StartTxDMA writes the size of the buffer
				printf("----- 'extr' serial Tx DMA, set buffer size %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMABufferSize = inValue; break;
			case 6: // TSerialDMAEngine::BindToBuffer writing 0
				printf("----- 'extr' serial Tx DMA, set ??? %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
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
				printf("----- 'extr' serial Tx DMA, set control register %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMAControl = inValue; break;
			case 1:
				printf("----- 'extr' serial Tx DMA, set interrupt reason %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				mTxDMAEvent = inValue; break;
			case 2:
				// TSerialDMAEngine::StartTxDMA writes whatever it reads in 2.1.1
				// TSerialDMAEngine::DMAInterrupt set this to 0 after reading 2.1.1
				printf("----- 'extr' serial Tx DMA, set ??? %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				break; // FIXME: additional action needed?
			case 3: // TSerialDMAEngine::ConfigureInterrupts writes 00000002
				printf("----- 'extr' serial Tx DMA, set interrupt select (?) %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				break; // FIXME: additional action needed?
			default:
				printf("***** 'extr' serial Tx, writing unknown DMA register %d %d %d = 0x%08X\n", inBank, 1, inRegister, inValue);
				break;
		}
	}
	return;
}


// -------------------------------------------------------------------------- //
//  * RunDMA()
// -------------------------------------------------------------------------- //
void
TVoyagerManagedSerialPort::RunDMA()
{
	static const char *portName = "/dev/ttys016";

	// open the serial port
	mSerialPort = open(portName, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (mSerialPort==-1) {
		printf("Error opening serial port %s - %s (%d).\n", portName, strerror(errno), errno);
		return;
	}
	tcflush(mSerialPort, TCIOFLUSH);

	// open the communication pipe
	int err = pipe(mPipe);
	if (err==-1) {
		printf("Error opening pipe - %s (%d).\n", strerror(errno), errno);
		return;
	}

	// create the actual thread and let it run forever
	int ptErr = ::pthread_create( &mDMAThread, NULL, &SHandleDMA, this );
	if (ptErr==-1) {
		printf("Error creating pthread - %s (%d).\n", strerror(errno), errno);
		return;
	}
	pthread_detach( mDMAThread );
}

// -------------------------------------------------------------------------- //
//  * HandleDMA()
// -------------------------------------------------------------------------- //
void
TVoyagerManagedSerialPort::HandleDMA()
{
	static int maxFD = -1;

	if (mSerialPort>maxFD) maxFD = mSerialPort;
	if (mPipe[0]>maxFD) maxFD = mPipe[0];

	printf(":::::>> fork is working\n");

	// thread loops and handles pipe, port, and DMA
	fd_set readSet;
	struct timeval timeout;
	for (;;) {
		bool needTimer = false;

		if (mTxDMAControl&0x00000002) { // DMA is enabled
			if (mTxDMADataCountdown) {
				needTimer = true;
			}
		}

		// wait for the next event
		FD_ZERO(&readSet);
		FD_SET(mPipe[0], &readSet);
		FD_SET(mSerialPort, &readSet);
		if (needTimer) {
			timeout.tv_sec = 0;
			timeout.tv_usec = 260; // one byte at 38400bps serial port speed
		}
		/*int s =*/ select(maxFD+1, &readSet, 0L, 0L, needTimer ? &timeout : 0L);

		// handle transmitting DMA
		if (mTxDMAControl&0x00000002) { // DMA is enabled
			if (mTxDMADataCountdown) {
				// write a byte
				KUInt8 data = 0;
				mMemory->ReadBP(mTxDMAPhysicalData, data);
				printf(":::::>> TX: 0x%02X '%c'\n", data, isprint(data)?data:'.');
				write(mSerialPort, &data, 1);
				mTxDMAPhysicalData++;
				mTxDMABufferSize--;
				if (mTxDMABufferSize==0) {
					mTxDMAPhysicalData = mTxDMAPhysicalBufferStart;
				}
				mTxDMADataCountdown--;
				if (mTxDMADataCountdown==0) {
					// trigger a "send buffer empty" interrupt
					//mDMAManager->WriteChannel2Register(1, 1, 0x00000080); // 0x80 = TxBufEmpty, 0x00000180
					printf(":::::>> buffer is now empty\n");
					mTxDMAEvent = 0x00000080;
					mInterruptManager->RaiseInterrupt(0x00000100);
				}
			}
		}

		// handle receiving DMA
		if (FD_ISSET(mSerialPort, &readSet)) {
			// read bytes that come in through the serial port
#if 1
			KUInt8 buf[1026];
			int n = -1;
			usleep(100000); // 1/10th of a second
			for (int j=2; j>0; j--) {
				n = read(mSerialPort, buf, 1024);
//				n = read(mSerialPort, buf, 1);
				if (n<=0)
					usleep(100000); // 1/10th of a second
				else
					break;
			}
			if (n==-1) {
				printf("Error reading from serial port %s - %s (%d).\n", "/dev/ttyp8", strerror(errno), errno);
			} else if (n==0) {
				printf("No data yet\n");
			} else {
				printf("----> Received %d bytes data from NCX\n", n);
#if 1
				for (KUInt32 i=0; i<n; i++) {
					KUInt8 data = buf[i];
					mMemory->WriteBP(mRxDMAPhysicalData, data);
					printf(" rx[%.3d] -> %02X '%c'\n", i, data, isprint(data)?data:'.');
					mRxDMAPhysicalData++;
					mRxDMABufferSize--;
					if (mRxDMABufferSize==0) { // or mRxDMADataCountdown?
//					if (mRxDMADataCountdown==0) { // or mRxDMADataCountdown does not work!
						mRxDMAPhysicalData = mRxDMAPhysicalBufferStart;
					}
					mRxDMADataCountdown--;
					if (mRxDMADataCountdown==0) {
						// buffer overflow?
					}
				}
				printf("===> Start Rx DMA\n");
#else
				KUInt32 dmaRxStart = mRxDMAPhysicalData;
				KUInt32 dmaRxSize = n;
				for (KUInt32 i=0; i<dmaRxSize; i++) {
					KUInt8 data = buf[i];
					mMemory->WriteBP(dmaRxStart+i, data);
					printf(" rx[%.3d] -> %02X '%c'\n", i, data, isprint(data)?data:'.');
				}
				printf("===> Start Rx DMA\n");
				KUInt32 dmaRxPos = mRxDMAPhysicalData;
				mRxDMAPhysicalData = dmaRxPos + dmaRxSize;
				KUInt32 dmaRxBuf = mRxDMADataCountdown;
				mRxDMADataCountdown = dmaRxBuf - dmaRxSize;
#endif
				mRxDMAEvent = 0x00000040;
				mInterruptManager->RaiseInterrupt(0x00000080); // 0x00000180
				printf("===> End Rx DMA\n");
			}

#else
			KUInt8 data;
			int nAvail;
			int err = ioctl(mSerialPort, FIONREAD, &nAvail);
			if (err==-1) {
			} else {
				for (int i=0; i<nAvail; i++) {
					int n = read(mSerialPort, &data, 1);
					if (n==-1) {
						printf("Error reading serial port - %s (%d).\n", strerror(errno), errno);
					} else if (n) {
						printf(":::::>> RX: 0x%02X '%c'\n", data, isprint(data)?data:'.');
						mMemory->WriteBP(mRxDMAPhysicalData, data);
						mRxDMAPhysicalData++;
						mRxDMADataCountdown--;
						if (mRxDMADataCountdown==0) {
							// TODO: generate an overflow error
						}

					}
				}
				if (nAvail) {
					printf(":::::>> Raising receive interrupt\n");
					mRxDMAEvent = 0x00000040;
					mInterruptManager->RaiseInterrupt(0x00000080); // 0x00000180
				}
			}
#endif
		}

		// handle commands from the pipe
		if (FD_ISSET(mPipe[0], &readSet)) {
			// read and interprete command from the pipe
			// - control register changed (start or stop DMA?)
			KUInt8 cmd;
			int nAvail;
			int err = ioctl(mPipe[0], FIONREAD, &nAvail);
			if (err==-1) {
			} else {
				for (int i=0; i<nAvail; i++) {
					int n = read(mPipe[0], &cmd, 1);
					if (n==-1) {
						printf("Error reading pipe - %s (%d).\n", strerror(errno), errno);
					} else if (n) {
						printf(":::::>> pipe commend '%c'\n", cmd);
					}
				}
			}
		}

	}
}


/*
 F080000 = 1,0,0:  w: rx physical buffer address (InitRxDMA)
 F080400 = 1,0,1:  w: current buffer address? (RxDMAControl)
 F080800 = 1,0,2
 F080C00 = 1,0,3:  w: 00000080 (RxDMAControl), 00000000 (The interrupt that we trigger?)
 F081000 = 1,0,4: rw: 00000403 buffer count? (RxDMAControl) (reading when shutting down?)
 F081400 = 1,0,5:  w: 00000404 buffer size? (RxDMAControl)
 F081800 = 1,0,6:  w: 00000000 (InitRxDMA), 000000FF (RxDMAControl)
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
*/









// ============================== References ===================================
#if 0

// POSIX
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#if !TARGET_OS_WIN32
#include <unistd.h>
#include <sys/time.h>
#endif

#include "../Log/TLog.h"
#include "../TInterruptManager.h"
#include "../TDMAManager.h"
#include "TMemory.h"

/*
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
 F081800 = 1,0,6:  w: 00000000 (InitRxDMA), 000000FF (RxDMAControl)
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

void crc16(unsigned short &crc, unsigned char d)
{
	static unsigned short crctab[256] = {
		0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
		0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
		0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
		0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
		0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
		0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
		0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
		0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
		0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
		0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
		0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
		0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
		0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
		0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
		0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
		0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
		0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
		0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
		0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
		0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
		0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
		0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
		0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
		0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
		0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
		0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
		0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
		0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
		0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
		0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
		0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
		0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
	};

	crc = ((crc>>8)&0x00ff)^crctab[(crc&0xff)^d];
}



static const KUInt8 esc_ = 0x33;
static KUInt32 txCnt_;
static unsigned char buf[1024];
static KUInt32 nBuf;

void send_block(unsigned char *data, int size)
{
	txCnt_ = 0;

	static unsigned char hdr[] = { 0x16, 0x10, 0x02 };
	static unsigned char ftr[] = { 0x10, 0x03 };

	unsigned char *d = buf;

	int i;
	*d++ = hdr[0];
	*d++ = hdr[1];
	*d++ = hdr[2];
	unsigned short crc = 0;
	for (i=0; i<size; i++) {
		unsigned char c = data[i];
		if (i==2 && data[1]==4)
			c = ++txCnt_;
		crc16(crc, c);
		*d++ = c;
		if (c==0x10)
			*d++ = c;
		if (c==esc_) {
			*d++ = 1;
			crc16(crc, 1);
	  printf("--- CONTROL BLOCK *NOT* changing esc from 0x%02x to 0x%02x\n", esc_, esc_+51);
			//esc_ += 51;
		}
	}
	*d++ = ftr[0];
	*d++ = ftr[1];
	crc16(crc, ftr[1]);
	*d++ = crc;
	*d++ = crc>>8;
	//	write(buf, d-buf);
	nBuf = d-buf;
}





// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TVoyagerSerialPort( TLog*, ELocationID )
// -------------------------------------------------------------------------- //
TVoyagerSerialPort::TVoyagerSerialPort(
									   TLog* inLog,
									   ELocationID inLocationID,
									   TInterruptManager* inInterruptManager,
									   TDMAManager* inDMAManager,
									   TMemory* inMemory)
:
mLog( inLog ),
mLocationID( inLocationID ),
mInterruptManager( inInterruptManager ),
mDMAManager( inDMAManager ),
mMemory(inMemory)
{
}

// -------------------------------------------------------------------------- //
//  * ~TVoyagerSerialPort( void )
// -------------------------------------------------------------------------- //
TVoyagerSerialPort::~TVoyagerSerialPort( void )
{
}

// -------------------------------------------------------------------------- //
//  * WriteRegister( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TVoyagerSerialPort::WriteRegister( KUInt32 inOffset, KUInt8 inValue )
{
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
			if (inValue & 0x01)
				//if (mDMAManager->GetEnabledChannels() & 0x0001)
			{
				printf("===> Start Tx DMA\n");
				KUInt32 dmaStart = mDMAManager->ReadChannel1Register(1, 1);
				KUInt32 dmaSize = mDMAManager->ReadChannel1Register(1, 4);
				for (KUInt32 i=0; i<dmaSize; i++) {
					KUInt8 data = 0;
					mMemory->ReadBP(dmaStart+i, data);
					printf(" tx[%.3d] -> %02X '%c'\n", i, data, isprint(data)?data:'.');
					buf[i] = data;
				}

#if 1
				KUInt32 bufferStart = mDMAManager->ReadChannel1Register(1, 0);
				KUInt32 dataStart = mDMAManager->ReadChannel1Register(1, 1);
				KUInt32 dataSize = mDMAManager->ReadChannel1Register(1, 4);
				KUInt32 bytesToEndOfBuffer = mDMAManager->ReadChannel1Register(1, 5);
				for (int i=0;;++i) {
					KUInt8 data;
					if (dataSize==0) break;
					mMemory->ReadBP(dataStart, data);
					buf[i] = data;
					dataSize--;
					dataStart++;
					bytesToEndOfBuffer--;
					if (bytesToEndOfBuffer==0)
						dataStart = bufferStart;
				}
				mDMAManager->WriteChannel1Register(1, 1, bufferStart);
				mDMAManager->WriteChannel1Register(1, 4, dataSize);
				mDMAManager->WriteChannel2Register(1, 1, 0x00000080);
				mInterruptManager->RaiseInterrupt(0x00000100); // 0x80 = TxBufEmpty, 0x00000180

				// NCX default port: 3679

				static int fd = -1;

				if (fd==-1) {
					fd = open("/dev/ttys008", O_RDWR | O_NOCTTY | O_NONBLOCK);
				}
				if (fd==-1) {
					printf("Error opening serial port %s - %s (%d).\n", "/dev/ttyp8", strerror(errno), errno);
				} else {
					printf("\n\n\n\n---------------> writing %d bytes\n\n\n\n", dmaSize);
					int err = write(fd, buf, dmaSize);
					if (err==-1) {
						printf("Error writing to serial port %s - %s (%d).\n", "/dev/ttyp8", strerror(errno), errno);
					}
					//close(fd);
				}
#endif



				printf("===> End Tx DMA\n");

#if 1
				int n = -1;
				for (int j=2; j>0; j--) {
					n = read(fd, buf, 1024);
					if (n<=0)
						sleep(1);
					else
						break;
				}
				if (n==-1) {
					printf("Error reading from serial port %s - %s (%d).\n", "/dev/ttyp8", strerror(errno), errno);
				} else if (n==0) {
					printf("No data yet\n");
				} else {
					printf("----> Received %d bytes data from NCX\n", n);
					if (buf[0]==0x10) {
						memmove(buf+1, buf, n);
						buf[0] = 0x16;
						n++;
					}
					KUInt32 dmaRxStart = mDMAManager->ReadChannel1Register(0, 0);
					KUInt32 dmaRxSize = n; //sizeof(LR_ack);
					for (KUInt32 i=0; i<dmaRxSize; i++) {
						KUInt8 data = buf[i];
						mMemory->WriteBP(dmaRxStart+i, data);
						printf(" rx[%.3d] -> %02X '%c'\n", i, data, isprint(data)?data:'.');
					}
					printf("===> Start Rx DMA\n");
					KUInt32 dmaRxPos = mDMAManager->ReadChannel1Register(0, 1);
					mDMAManager->WriteChannel1Register(0, 1, dmaRxPos + dmaRxSize);
					KUInt32 dmaRxBuf = mDMAManager->ReadChannel1Register(0, 4);
					mDMAManager->WriteChannel1Register(0, 4, dmaRxBuf - dmaRxSize);
					mDMAManager->WriteChannel2Register(0, 1, 0x00000040); // ???
					mInterruptManager->RaiseInterrupt(0x00000080); // 0x00000180
					printf("===> End Rx DMA\n");
				}
#else
				static unsigned char LR_ack[] = {
					0x1d, 0x01, 0x02, 0x01, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x01, 0x02, 0x03, 0x01,
					0x08, 0x04, 0x02, 0x40, 0x00, 0x08, 0x01, 0x03, 0x0E, 0x04, 0x02, 0x04, 0x00, 0xFA,
				};

				send_block(LR_ack, sizeof(LR_ack));

				KUInt32 dmaRxStart = mDMAManager->ReadChannel1Register(0, 0);
				KUInt32 dmaRxSize = nBuf; //sizeof(LR_ack);
				for (KUInt32 i=0; i<dmaRxSize; i++) {
					mMemory->WriteBP(dmaRxStart+i, buf[i]);
				}
				printf("===> Start Rx DMA\n");
				KUInt32 dmaRxPos = mDMAManager->ReadChannel1Register(0, 1);
				mDMAManager->WriteChannel1Register(0, 1, dmaRxPos + dmaRxSize);
				KUInt32 dmaRxBuf = mDMAManager->ReadChannel1Register(0, 4);
				mDMAManager->WriteChannel1Register(0, 4, dmaRxBuf - dmaRxSize);
				mDMAManager->WriteChannel2Register(0, 1, 0x00000040);
				//mInterruptManager->RaiseInterrupt(0x00000100); // 0x00000180
				printf("===> End Rx DMA\n");
#endif

			}
			break;
	}
}

// -------------------------------------------------------------------------- //
//  * ReadRegister( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TVoyagerSerialPort::ReadRegister( KUInt32 inOffset )
{
	KUInt8 theResult = 0;
	/*	if (mLog)
	 {
		mLog->FLogLine(
	 "[%c%c%c%c] - Read serial register %.4X : %.2X",
	 (char) ((mLocationID >> 24) & 0xFF),
	 (char) ((mLocationID >> 16) & 0xFF),
	 (char) ((mLocationID >> 8) & 0xFF),
	 (char) ((mLocationID) & 0xFF),
	 (unsigned int) inOffset,
	 (unsigned int) theResult );
	 }
	 */
	if (inOffset == 0x4400)
	{
		// Both buffers are empty for now.
		// We also don't want a beacon.
		theResult = 0x80;
	} else if (inOffset == 0x4800) {
		// RxEOF
		//		theResult = 0x80;
		//		TDebugger::BreakInDebugger();
	} else {
		//		TDebugger::BreakInDebugger();
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


/*
 TSerialDMAEngine Rx = 0x0CCACB38
 TSerialDMAEngine Tx = 0x0CCACAE8
 */

#endif

// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
