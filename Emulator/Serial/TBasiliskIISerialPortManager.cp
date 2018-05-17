// ==============================
// File:			TBasiliskIISerialPortManager.cp
// Project:			Einstein
//
// Copyright 2018 by Matthias Melcher (mm@matthiasm.com).
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
#include "TBasiliskIISerialPortManager.h"
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


const char *TBasiliskIISerialPortManager::kBasiliskPipe = "/tmp/pty.BasiliskII";


// -------------------------------------------------------------------------- //
//  * TBasiliskIISerialPortManager()
//
// Emulate a serial port via a pseudo terminal (pty) in MacOS
//
// This driver works around a bug in BasiliskII, where Basilisk will hang
// forever when closing a serial port.
//
// This driver provides a PTY via /tmp/pty.BasiliskII . It will try to
// determine when an app inside Basilisk closes the serial port. The Einstein
// driver will then remove the PTY, which will in turn break the endless
// loop in Basilisk.
//
// This workaround functions well for NTK and NCU, running inside BasiliskII.
//
// \author Matthias Melcher
//
// -------------------------------------------------------------------------- //
TBasiliskIISerialPortManager::TBasiliskIISerialPortManager(
													 TLog* inLog,
													 ELocationID inLocationID)
:	TBasicSerialPortManager(inLog, inLocationID),

	mPipe{-1,-1},
	mDMAIsRunning(false),
	mDMAThread(0L),
	pBasiliskSlaveName(nullptr),
	pBasiliskMaster(-1),
	pBasiliskSlave(-1),
	pComState(kStateInit)
{
}


// -------------------------------------------------------------------------- //
//  * ~TBasiliskIISerialPortManager( void )
//		Don't worry about releasing  resources. All drivers will live
//		throughout the run time of the app.
// -------------------------------------------------------------------------- //
TBasiliskIISerialPortManager::~TBasiliskIISerialPortManager()
{
	if (mDMAIsRunning)
		TriggerEvent('Q');
}


// -------------------------------------------------------------------------- //
//  * run( TInterruptManager*, TDMAManager*, TMemory* )
// -------------------------------------------------------------------------- //
void TBasiliskIISerialPortManager::run(TInterruptManager* inInterruptManager,
								TDMAManager* inDMAManager,
								TMemory* inMemory)
{
	mInterruptManager = inInterruptManager;
	mDMAManager = inDMAManager;
	mMemory = inMemory;

	RunDMA();
}


// -------------------------------------------------------------------------- //
// DMA or interrupts were triggered
// -------------------------------------------------------------------------- //
void TBasiliskIISerialPortManager::TriggerEvent(KUInt8 cmd)
{
	if (mPipe[1]!=-1) {
		write(mPipe[1], &cmd, 1);
	}
}


// -------------------------------------------------------------------------- //
// * OpenPTY
// -------------------------------------------------------------------------- //
bool
TBasiliskIISerialPortManager::OpenPTY()
{
	int ret = -1;

	// TODO: if we ever decide to suppirt more than the internal serial port, we
	// will have to generate diffwerent names for different ports.
	ret = unlink(kBasiliskPipe);
	if (ret==-1 && errno!=2) {
		perror("TBasiliskIISerialPortManager: Can't unlink Basilisk Pipe");
//		return false;
	}

	pBasiliskMaster = posix_openpt(O_RDWR | O_NOCTTY);
	if (pBasiliskMaster==-1) {
		perror("TBasiliskIISerialPortManager: Can't open Basilisk Master");
		return false;
	}

	ret = grantpt(pBasiliskMaster);
	if (ret==-1) {
		perror("TBasiliskIISerialPortManager: Can't grant Basilisk Master pseudo terminal");
		return false;
	}

	ret = unlockpt(pBasiliskMaster);
	if (ret==-1) {
		perror("TBasiliskIISerialPortManager: Can't unlock Basilisk Master pseudo terminal");
		return false;
	}

	const char *slaveName = ptsname(pBasiliskMaster);
	if (slaveName==nullptr) {
		perror("TBasiliskIISerialPortManager: Can't get name of Basilisk Slave pseudo terminal");
		return false;
	}
	pBasiliskSlaveName = strdup(slaveName);

	pBasiliskSlave = open(slaveName, O_RDWR | O_NOCTTY);
	if (pBasiliskSlave==-1) {
		perror("TBasiliskIISerialPortManager: Can't open Basilisk Slave pseudo terminal");
		return false;
	}

	ret = symlink(slaveName, kBasiliskPipe);
	if (ret==-1) {
		perror("TBasiliskIISerialPortManager: Can't link Basilisk pipe");
		return false;
	}

	//	tcgetattr(fd, &struct termios)
	struct termios tios;

	ret = tcgetattr(pBasiliskMaster, &tios);
	if (ret==-1) { perror("TBasiliskIISerialPortManager: tcgetattr"); return false; }
	cfmakeraw(&tios);
	tios.c_cflag |= (HUPCL | CLOCAL);
	ret = tcsetattr(pBasiliskMaster, TCSANOW, &tios);
	if (ret==-1) { perror("TBasiliskIISerialPortManager: tcsetattr"); return false; }

	ret = tcgetattr(pBasiliskSlave, &tios);
	if (ret==-1) { perror("TBasiliskIISerialPortManager: tcgetattr"); return false; }
	cfmakeraw(&tios);
	tios.c_cflag |= (HUPCL | CLOCAL);
	ret = tcsetattr(pBasiliskSlave, TCSANOW, &tios);
	if (ret==-1) { perror("TBasiliskIISerialPortManager: tcsetattr"); return false; }

	int pkt = 1;
	ret = ioctl(pBasiliskMaster, TIOCPKT, &pkt);
	if (ret==-1) { perror("TBasiliskIISerialPortManager: TIOCPKT"); return false; }

	return true;
}


// -------------------------------------------------------------------------- //
// * ClosePTY
// -------------------------------------------------------------------------- //
void
TBasiliskIISerialPortManager::ClosePTY()
{
	unlink(kBasiliskPipe);

	if (pBasiliskMaster!=-1) {
		close(pBasiliskMaster);
		pBasiliskMaster = -1;
	}

	if (pBasiliskSlave!=-1) {
		close(pBasiliskSlave);
		pBasiliskSlave = -1;
	}

	if (pBasiliskSlaveName!=nullptr) {
		free(pBasiliskSlaveName);
		pBasiliskSlaveName = nullptr;
	}
}


// -------------------------------------------------------------------------- //
//  * RunDMA()
//		RunDMA initializes the ports that are needed to communicate with the
//		outside world and between threads. It then launches a thread that
//		handles all DMA access independently of anything else going on
//		inside Einstein - just as a hardware DMA would.
//
//		"When you feel you fail sometimes it hurts
//		For a meaning in life is why you search
//		Take the boys on the train, drive to school on the church
//		It's like that, and that's the way it is"
// -------------------------------------------------------------------------- //
void
TBasiliskIISerialPortManager::RunDMA()
{
	// create PTY and named pipe
	bool hasPty = OpenPTY();
	if (!hasPty) {
		printf("***** TBasiliskIISerialPortManager::RunDMA: Error creating pseudo terminal %s.\n", kBasiliskPipe);
		return;
	}

	// open the thread communication pipe
	int err = pipe(mPipe);
	if (err==-1) {
		printf("***** TBasiliskIISerialPortManager::RunDMA: Error opening pipe - %s (%d).\n", strerror(errno), errno);
		return;
	}

	// create the actual thread and let it run forever
	int ptErr = ::pthread_create( &mDMAThread, NULL, &SHandleDMA, this );
	if (ptErr==-1) {
		printf("***** TBasiliskIISerialPortManager::RunDMA: Error creating pthread - %s (%d).\n", strerror(errno), errno);
		return;
	}
	pthread_detach( mDMAThread );
}


// -------------------------------------------------------------------------- //
//  * HandleDMA()
//		This endless loop watches DMA registers as they are changed by the
//		OS, and read and writes data via the outside communication ports.
//		It can also trigger interrupts when buffers empty, fill, or overflow.
// -------------------------------------------------------------------------- //
void
TBasiliskIISerialPortManager::HandleDMA()
{
	bool shutdownThread = false;
	pComState = kStateInit;
	static int maxFD = -1;

	if (pBasiliskMaster>maxFD) maxFD = pBasiliskMaster;
	if (pBasiliskSlave>maxFD) maxFD = pBasiliskSlave;
	if (mPipe[0]>maxFD) maxFD = mPipe[0];

	// thread loops and handles pipe, port, and DMA
	fd_set readSet;
	struct timeval timeout;

	while (!shutdownThread) {
		bool needTimer = false;

		if (mTxDMAControl&0x00000002) { // DMA is enabled
			if (mTxDMADataCountdown) {
				needTimer = true;
			}
		}

		// wait for the next event
		FD_ZERO(&readSet);
		FD_SET(mPipe[0], &readSet);
		FD_SET(pBasiliskMaster, &readSet);
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
				write(pBasiliskMaster, &data, 1);
				mTxDMAPhysicalData++;
				mTxDMABufferSize--;
				if (mTxDMABufferSize==0) {
					mTxDMAPhysicalData = mTxDMAPhysicalBufferStart;
				}
				mTxDMADataCountdown--;
				if (mTxDMADataCountdown==0) {
					// trigger a "send buffer empty" interrupt
					mTxDMAEvent = 0x00000080;
					mInterruptManager->RaiseInterrupt(0x00000100);
				}
			}
		}

		// handle receiving DMA

		if (FD_ISSET(pBasiliskMaster, &readSet)) {
			// read bytes that come in through the serial port
			KUInt8 buf[1026];
			int n = -1;
			usleep(100000); // 1/10th of a second
			n = (int)read(pBasiliskMaster, buf, 1024);
			if (n<=0) {
				usleep(100000); // 1/10th of a second
			} else if (n==1) {
				const uint8_t FLUSHRW = TIOCPKT_FLUSHREAD|TIOCPKT_FLUSHWRITE;
				uint8_t c = buf[0];
				switch (pComState) {
					case kStateInit:
						// Initial state. Wait for Basilisk to open and setup its COM port
						if ((c&TIOCPKT_NOSTOP) == TIOCPKT_NOSTOP) pComState = kStateOpen;
						break;
					case kStateOpen:
						// The port is open. Wait for Basilisk to flush the COM port after initialising
						if ((c&FLUSHRW) == FLUSHRW) pComState = kStateFlushRW;
						break;
					case kStateFlushRW:
						// wait for a second flushing of the COM port. This indicates to us that Basilisk
						// is very likely trying to close the port, but may be deadloking. Help Basilisk
						// out by closing and reopening the pipe.
						if ((c&FLUSHRW) == FLUSHRW) {
							ClosePTY();
							usleep(500000); // half a second
							OpenPTY();
							pComState = kStateInit;
						}
						break;
				}
				// handle control command in buf[0]
//				printf("BasiliskII sent control code (%d):", pComState);
//				if (c&TIOCPKT_FLUSHREAD) puts("     - TIOCPKT_FLUSHREAD");
//				if (c&TIOCPKT_FLUSHWRITE) puts("     - TIOCPKT_FLUSHWRITE");
//				if (c&TIOCPKT_STOP) puts("     - TIOCPKT_STOP");
//				if (c&TIOCPKT_START) puts("     - TIOCPKT_START");
//				if (c&TIOCPKT_DOSTOP) puts("     - TIOCPKT_DOSTOP");
//				if (c&TIOCPKT_NOSTOP) puts("     - TIOCPKT_NOSTOP");
			} else {
				// handle incomming data (ignore buf[0]!)
				for (KUInt32 i=1; i<n; i++) {
					KUInt8 data = buf[i];
					mMemory->WriteBP(mRxDMAPhysicalData, data);
					mRxDMAPhysicalData++;
					mRxDMABufferSize--;
					if (mRxDMABufferSize==0) { // or mRxDMADataCountdown?
						mRxDMAPhysicalData = mRxDMAPhysicalBufferStart;
					}
					mRxDMADataCountdown--;
					if (mRxDMADataCountdown==0) {
						// buffer overflow?
					}
				}
				mRxDMAEvent = 0x00000040;
				mInterruptManager->RaiseInterrupt(0x00000080); // 0x00000180
			}
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
					int n = (int)read(mPipe[0], &cmd, 1);
					if (n==-1) {
						printf("***** Error reading pipe - %s (%d).\n", strerror(errno), errno);
					} else if (n) {
						if (cmd=='Q') {
							shutdownThread = true;
						}
					}
				}
			}
		}

	}
	ClosePTY();
	mDMAIsRunning = false;
}


// ================================================================== //
//  You should've gone to school, you could've learned a trade        //
//  But you laid in bed where the bums have laid                      //
//  Now all the time you're crying that you're underpaid              //
//  It's like that (what?) and that's the way it is                   //
//  Huh!                                                              //
//   - Run D.M.C.                                                     //
// ================================================================== //
