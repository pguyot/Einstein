// ==============================
// File:			TPipesSerialPortManager.cp
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
#include "TPipesSerialPortManager.h"
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
#endif

#include "Emulator/Log/TLog.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TMemory.h"



// -------------------------------------------------------------------------- //
//  * TPipesSerialPortManager()
// Emulate a serial connection using two named pipes via "mkfifo". The pipes
// are named:
// 	$(HOME)/Library/Application Support/Einstein Emulator/ExtrSerPortSend
// and
//  $(HOME)/Library/Application Support/Einstein Emulator/ExtrSerPortRecv")
// -------------------------------------------------------------------------- //
TPipesSerialPortManager::TPipesSerialPortManager(
													 TLog* inLog,
													 ELocationID inLocationID)
:	TBasicSerialPortManager(inLog, inLocationID),

	mPipe{-1,-1},
	mTxPort(-1),
	mRxPort(-1),
	mDMAIsRunning(false),
	mDMAThread(0L),
	mTxPortName(0L),
	mRxPortName(0L)
{
}


// -------------------------------------------------------------------------- //
//  * ~TPipesSerialPortManager( void )
//		Don't worry about releasing  resources. All drivers will live
//		throughout the run time of the app.
// -------------------------------------------------------------------------- //
TPipesSerialPortManager::~TPipesSerialPortManager()
{
	if (mTxPortName)
		free(mTxPortName);
	if (mRxPortName)
		free(mRxPortName);
}

// -------------------------------------------------------------------------- //
//  * run( TInterruptManager*, TDMAManager*, TMemory* )
// -------------------------------------------------------------------------- //
void TPipesSerialPortManager::run(TInterruptManager* inInterruptManager,
								  TDMAManager* inDMAManager,
								  TMemory* inMemory)
{
	mInterruptManager = inInterruptManager;
	mDMAManager = inDMAManager;
	mMemory = inMemory;

	RunDMA();
}

// -------------------------------------------------------------------------- //
// DMA or interrupts trigger a command that must be handled by a derived class.
// -------------------------------------------------------------------------- //
void TPipesSerialPortManager::TriggerEvent(KUInt8 cmd)
{
	if (mPipe[1]!=-1) {
		write(mPipe[1], &cmd, 1);
	}
}


// -------------------------------------------------------------------------- //
// * FindPipeNames()
//		Find names that will be available for external apps for this user.
// TODO: currently we care about the 'extr' port, but we may want to provide
//		pipes for a virtual modem
// -------------------------------------------------------------------------- //
void
TPipesSerialPortManager::FindPipeNames()
{
	if (mTxPortName && mRxPort)
		return;

	char path[PATH_MAX];

	std::string basePath = TPathHelper::GetSerialPipeBasePath();
	::strncpy(path, basePath.c_str(), PATH_MAX);

	char *end = path + strlen(path);

	// crete the file path to the sending and receiving node
	strcpy(end, "/Einstein Emulator");
	if (access(path, S_IRUSR|S_IWUSR|S_IXUSR)==-1) {
		if (mkdir(path, S_IRUSR|S_IWUSR|S_IXUSR)==-1) {
			printf("***** Error creating named pipe directory %s - %s (%d).\n", path, strerror(errno), errno);
		}
	}

	// create the name for the transmitting pipe
	// FIXME: "Einstein Platform"
	strcpy(end, "/Einstein Emulator/ExtrSerPortSend");
	if (!mTxPortName)
		mTxPortName = strdup(path);

	// create the name for the receiving pipe
	strcpy(end, "/Einstein Emulator/ExtrSerPortRecv");
	if (!mRxPortName)
		mRxPortName = strdup(path);
}


// -------------------------------------------------------------------------- //
// * CreateNamedPipes
//		Create the named pipes as nodes in the file system.
// -------------------------------------------------------------------------- //
bool
TPipesSerialPortManager::CreateNamedPipes()
{
	// crete the sending node if it does not exist yet
	if (access(mTxPortName, S_IRUSR|S_IWUSR)==-1) {
		if (mkfifo(mTxPortName, S_IRUSR|S_IWUSR)==-1) {
			printf("***** Error creating named pipe %s - %s (%d).\n", mTxPortName, strerror(errno), errno);
			return false;
		}
	}

	// crete the receiving node if it does not exist yet
	if (access(mRxPortName, S_IRUSR|S_IWUSR)==-1) {
		if (mkfifo(mRxPortName, S_IRUSR|S_IWUSR)==-1) {
			printf("***** Error creating named pipe %s - %s (%d).\n", mRxPortName, strerror(errno), errno);
			return false;
		}
	}

	return true;
}


// -------------------------------------------------------------------------- //
//  * RunDMA()
//		RunDMA initializes the ports that are needed to communicate with the
//		outside world and between threads. It then launches a thread that
//		handles all DMA access independently of anything else going on
//		inside Einstein - just as a hardware DMA would.
//
//		Not to be confused with RunDMC which is an American Hip Hop band from
//		the eighties.
// -------------------------------------------------------------------------- //
void
TPipesSerialPortManager::RunDMA()
{
	// create named pipe nodes
	FindPipeNames();
	if (!CreateNamedPipes()) {
		return;
	}

	// open the named pipes
	mTxPort = open(mTxPortName, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (mTxPort==-1) {
		printf("***** Error opening named sending pipe %s - %s (%d).\n", mTxPortName, strerror(errno), errno);
		return;
	}
	mRxPort = open(mRxPortName, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (mRxPort==-1) {
		printf("***** Error opening named receiving pipe %s - %s (%d).\n", mRxPortName, strerror(errno), errno);
		return;
	}
	tcflush(mTxPort, TCIOFLUSH);
	tcflush(mRxPort, TCIOFLUSH);

	// open the thread communication pipe
	int err = pipe(mPipe);
	if (err==-1) {
		printf("***** Error opening pipe - %s (%d).\n", strerror(errno), errno);
		return;
	}

	// create the actual thread and let it run forever
	int ptErr = ::pthread_create( &mDMAThread, NULL, &SHandleDMA, this );
	if (ptErr==-1) {
		printf("***** Error creating pthread - %s (%d).\n", strerror(errno), errno);
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
TPipesSerialPortManager::HandleDMA()
{
	static int maxFD = -1;

	if (mTxPort>maxFD) maxFD = mTxPort;
	if (mRxPort>maxFD) maxFD = mRxPort;
	if (mPipe[0]>maxFD) maxFD = mPipe[0];

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
		FD_SET(mRxPort, &readSet);
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
				//printf(":::::>> TX: 0x%02X '%c'\n", data, isprint(data)?data:'.');
				write(mTxPort, &data, 1);
				mTxDMAPhysicalData++;
				mTxDMABufferSize--;
				if (mTxDMABufferSize==0) {
					mTxDMAPhysicalData = mTxDMAPhysicalBufferStart;
				}
				mTxDMADataCountdown--;
				if (mTxDMADataCountdown==0) {
					// trigger a "send buffer empty" interrupt
					//mDMAManager->WriteChannel2Register(1, 1, 0x00000080); // 0x80 = TxBufEmpty, 0x00000180
					//printf(":::::>> buffer is now empty\n");
					mTxDMAEvent = 0x00000080;
					mInterruptManager->RaiseInterrupt(0x00000100);
				}
			}
		}

		// handle receiving DMA

		// FIXME: This routine is currently a mess. it needs to be rewritten
		// and retested. Note though that the original Newton hardware and OS
		// did have timing issues whan the server PC communicated faster as
		// expected in the year 1996, which lead to CPU cycle burning software
		// like "slowdown.exe".

		if (FD_ISSET(mRxPort, &readSet)) {
			// read bytes that come in through the serial port
			KUInt8 buf[1026];
			int n = -1;
			usleep(100000); // 1/10th of a second
			for (int j=2; j>0; j--) {
				// FIXME: reading 1024 bytes will overflow the buffer
				// FIXME: reading less bytes must make sure that the next select() call triggers on data left in the buffer
				n = (int)read(mRxPort, buf, 1024);
				if (n<=0)
					usleep(100000); // 1/10th of a second
				else
					break;
			}
			if (n==-1) {
				printf("***** Error reading from serial port %s - %s (%d).\n", mRxPortName, strerror(errno), errno);
			} else if (n==0) {
				// printf("***** No data yet\n");
			} else {
				//printf("----> Received %d bytes data from NCX\n", n);
				for (KUInt32 i=0; i<n; i++) {
					KUInt8 data = buf[i];
					mMemory->WriteBP(mRxDMAPhysicalData, data);
					//printf(" rx[%.3d] -> %02X '%c'\n", i, data, isprint(data)?data:'.');
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
				//printf("===> Start Rx DMA\n");
				mRxDMAEvent = 0x00000040;
				mInterruptManager->RaiseInterrupt(0x00000080); // 0x00000180
				//printf("===> End Rx DMA\n");
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
						// TODO: add a command that is sent whenever a relevant
						//		 DMA register is written in order to launch the
						//		 DMA management loop again.
						// TODO: add a command for power on and use it to open
						//		 files and ports.
						// TODO: add a command for power off and use it to close
						//		 files and ports.
						//printf(":::::>> pipe commend '%c'\n", cmd);
					}
				}
			}
		}

	}
}



// ================================================================== //
// You never finish a program, you just stop working on it.           //
//  - Anonymous                                                       //
//                                                                    //
// So true!                                                           //
//  - Matthias Melcher                                                //
// ================================================================== //
