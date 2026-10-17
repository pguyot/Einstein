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

// Throttle the emulated serial line: the time it takes to transfer one byte,
// in microseconds. 0 transfers as fast as the host can.
// 260 us is 38400 bps (ten bits per byte with start and stop bit), the
// fastest speed that NewtonOS supports.
static constexpr int kTxMicrosecondsPerByte = 90; // Newton to BasiliskII
static constexpr int kRxMicrosecondsPerByte = 90; // BasiliskII to Newton

#include "TBasiliskIISerialPortManager.h"
#include "app/TPathHelper.h"

// POSIX
#include <signal.h>
#include <string.h>
#include <sys/types.h>

#if !TARGET_OS_WIN32
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#if defined(__APPLE__) && !TARGET_OS_IOS
#include <CoreServices/CoreServices.h>
#endif
#endif

#include "Emulator/TARMProcessor.h"
#include "Emulator/TDMAManager.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TLog.h"

const char* TBasiliskIISerialPortManager::kBasiliskPipe = "/tmp/pty.BasiliskII";

// Largest number of bytes read from or written to the PTY at once.
static constexpr KUInt32 kMaxChunkSize = 1024;

// After closing the PTY to get BasiliskII out of its endless loop, wait this
// long before opening it again.
static constexpr std::chrono::milliseconds kReopenDelay { 500 };

// If the PTY can't be opened again, try again after this time.
static constexpr std::chrono::seconds kOpenRetryDelay { 5 };

// After a read error, wait this long before reading again.
static constexpr std::chrono::milliseconds kReadErrorDelay { 100 };

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
	TSerialPorts::EPortIndex inPortIx) :
		TBasicSerialPortManager(inLog, inPortIx),
		mTxThrottle(kTxMicrosecondsPerByte),
		mRxThrottle(kRxMicrosecondsPerByte)
{
}

// -------------------------------------------------------------------------- //
//  * ~TBasiliskIISerialPortManager( void )
//		The driver is deleted when the user selects another driver.
// -------------------------------------------------------------------------- //
TBasiliskIISerialPortManager::~TBasiliskIISerialPortManager()
{
	// always shut down the thread properly, so we clean up the filesystem after ourselves.
	if (mDMAIsRunning)
	{
		void* threadStatus;
		TriggerEvent(kSerCmd_StopThread);
		pthread_join(mDMAThread, &threadStatus);
		mDMAIsRunning = false;
	}
	// the thread closes the PTY, but it may not have started
	ClosePTY();

	if (mPipe[0] != -1)
		close(mPipe[0]);
	if (mPipe[1] != -1)
		close(mPipe[1]);
}

// -------------------------------------------------------------------------- //
//  * run( TInterruptManager*, TDMAManager*, TMemory* )
// -------------------------------------------------------------------------- //
void
TBasiliskIISerialPortManager::run(TInterruptManager* inInterruptManager,
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
// kSerCmd_StopThread quits the worker thread, any other command just wakes
// it up.
// -------------------------------------------------------------------------- //
void
TBasiliskIISerialPortManager::TriggerEvent(KUInt8 cmd)
{
	if (cmd == kSerCmd_StopThread)
		mQuitRequested = true;
	if (mPipe[1] != -1)
	{
		// If the pipe is full, the worker thread will wake up anyway.
		(void) write(mPipe[1], &cmd, 1);
	}
}

// -------------------------------------------------------------------------- //
// * OpenPTY
// -------------------------------------------------------------------------- //
bool
TBasiliskIISerialPortManager::OpenPTY()
{
	int ret = -1;

	// TODO: if we ever decide to support more than the internal serial port, we
	// will have to generate diffwerent names for different ports.
	ret = unlink(kBasiliskPipe);
	if (ret == -1 && errno != 2)
	{
		perror("TBasiliskIISerialPortManager: Can't unlink Basilisk Pipe");
		//		return false;
	}

	pBasiliskLocalFD = posix_openpt(O_RDWR | O_NOCTTY);
	if (pBasiliskLocalFD == -1)
	{
		perror("TBasiliskIISerialPortManager: Can't open Basilisk Local");
		return false;
	}
	// The worker writes while it holds mDMAMutex, so a write must never wait
	// for BasiliskII to read.
	(void) fcntl(pBasiliskLocalFD, F_SETFL, fcntl(pBasiliskLocalFD, F_GETFL) | O_NONBLOCK);

	ret = grantpt(pBasiliskLocalFD);
	if (ret == -1)
	{
		perror("TBasiliskIISerialPortManager: Can't grant Basilisk Local pseudo terminal");
		return false;
	}

	ret = unlockpt(pBasiliskLocalFD);
	if (ret == -1)
	{
		perror("TBasiliskIISerialPortManager: Can't unlock Basilisk Local pseudo terminal");
		return false;
	}

	const char* remoteName = ptsname(pBasiliskLocalFD);
	if (remoteName == nullptr)
	{
		perror("TBasiliskIISerialPortManager: Can't get name of Basilisk Remote pseudo terminal");
		return false;
	}
	pBasiliskRemoteName = strdup(remoteName);

	pBasiliskRemoteFD = open(remoteName, O_RDWR | O_NOCTTY);
	if (pBasiliskRemoteFD == -1)
	{
		perror("TBasiliskIISerialPortManager: Can't open Basilisk Remote pseudo terminal");
		return false;
	}

	ret = symlink(remoteName, kBasiliskPipe);
	if (ret == -1)
	{
		perror("TBasiliskIISerialPortManager: Can't link Basilisk pipe");
		return false;
	}

	//	tcgetattr(fd, &struct termios)
	struct termios tios { };

	ret = tcgetattr(pBasiliskLocalFD, &tios);
	if (ret == -1)
	{
		perror("TBasiliskIISerialPortManager: tcgetattr");
		return false;
	}
	cfmakeraw(&tios);
	tios.c_cflag |= (HUPCL | CLOCAL);
	ret = tcsetattr(pBasiliskLocalFD, TCSANOW, &tios);
	if (ret == -1)
	{
		perror("TBasiliskIISerialPortManager: tcsetattr");
		return false;
	}

	ret = tcgetattr(pBasiliskRemoteFD, &tios);
	if (ret == -1)
	{
		perror("TBasiliskIISerialPortManager: tcgetattr");
		return false;
	}
	cfmakeraw(&tios);
	tios.c_cflag |= (HUPCL | CLOCAL);
	ret = tcsetattr(pBasiliskRemoteFD, TCSANOW, &tios);
	if (ret == -1)
	{
		perror("TBasiliskIISerialPortManager: tcsetattr");
		return false;
	}

	int pkt = 1;
	ret = ioctl(pBasiliskLocalFD, TIOCPKT, &pkt);
	if (ret == -1)
	{
		perror("TBasiliskIISerialPortManager: TIOCPKT");
		return false;
	}

	return true;
}

// -------------------------------------------------------------------------- //
// * ClosePTY
// -------------------------------------------------------------------------- //
void
TBasiliskIISerialPortManager::ClosePTY()
{
	unlink(kBasiliskPipe);

	if (pBasiliskLocalFD != -1)
	{
		close(pBasiliskLocalFD);
		pBasiliskLocalFD = -1;
	}

	if (pBasiliskRemoteFD != -1)
	{
		close(pBasiliskRemoteFD);
		pBasiliskRemoteFD = -1;
	}

	if (pBasiliskRemoteName)
	{
		::free(pBasiliskRemoteName);
		pBasiliskRemoteName = nullptr;
	}

	mSendBlocked = false;
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
	if (!hasPty)
	{
		KPrintf("***** TBasiliskIISerialPortManager::RunDMA: Error creating pseudo terminal %s.\n", kBasiliskPipe);
		ClosePTY();
		return;
	}

	// open the thread communication pipe
	int err = pipe(mPipe);
	if (err == -1)
	{
		KPrintf("***** TBasiliskIISerialPortManager::RunDMA: Error opening pipe - %s (%d).\n", strerror(errno), errno);
		return;
	}
	// Neither end blocks: the emulator thread must never wait for the worker
	// thread, and the worker thread empties the pipe without waiting.
	(void) fcntl(mPipe[0], F_SETFL, fcntl(mPipe[0], F_GETFL) | O_NONBLOCK);
	(void) fcntl(mPipe[1], F_SETFL, fcntl(mPipe[1], F_GETFL) | O_NONBLOCK);

	// create the actual thread and let it run until the driver is deleted
	int ptErr = ::pthread_create(&mDMAThread, nullptr, &SHandleDMA, this);
	if (ptErr != 0)
	{
		KPrintf("***** TBasiliskIISerialPortManager::RunDMA: Error creating pthread - %s (%d).\n", strerror(ptErr), ptErr);
		return;
	}
	mDMAIsRunning = true;
}

// -------------------------------------------------------------------------- //
//  * HandleDMA()
//		This endless loop watches DMA registers as they are changed by the
//		OS, and read and writes data via the outside communication ports.
//		It can also trigger interrupts when buffers empty, fill, or overflow.
//
//		The loop waits for a command, the PTY, or the throttle, whatever comes
//		first. It never sleeps, and never waits while it holds mDMAMutex.
// -------------------------------------------------------------------------- //
int
TBasiliskIISerialPortManager::HandleDMA()
{
	pComState = kStateInit;

	// thread loops and handles pipe, port, and DMA
	for (;;)
	{
		// Decide what to wait for, and how long.
		long long timeout = -1; // microseconds, -1 waits for an event
		auto waitAtMost = [&timeout](long long us) {
			if (timeout < 0 || us < timeout)
				timeout = us;
		};
		int ptyFD = pBasiliskLocalFD;
		bool watchRead = false;
		bool watchWrite = false;
		if (ptyFD == -1)
		{
			// The PTY was closed to help BasiliskII. Open it again later.
			waitAtMost(MicrosecondsUntil(mReopenTime));
		} else
		{
			// Leave new data in the PTY until the throttle allows it, and while
			// receive DMA has no room (NewtonOS wakes us when it has).
			if (RxDMARoom() > 0)
			{
				long long rxDelay = mRxThrottle.Delay(1);
				long long retryDelay = MicrosecondsUntil(mReadRetryTime);
				if (retryDelay > rxDelay)
					rxDelay = retryDelay;
				if (rxDelay == 0)
					watchRead = true;
				else
					waitAtMost(rxDelay);
			}
		}
		KUInt32 pending = PendingTxBytes();
		if (ptyFD != -1 && mSendBlocked)
			watchWrite = true;
		else if (pending)
			waitAtMost(mTxThrottle.Delay(pending));

		fd_set readFDs;
		fd_set writeFDs;
		FD_ZERO(&readFDs);
		FD_ZERO(&writeFDs);
		FD_SET(mPipe[0], &readFDs);
		int maxFD = mPipe[0];
		if (watchRead)
			FD_SET(ptyFD, &readFDs);
		if (watchWrite)
			FD_SET(ptyFD, &writeFDs);
		if ((watchRead || watchWrite) && ptyFD > maxFD)
			maxFD = ptyFD;

		struct timeval tv { };
		if (timeout >= 0)
		{
			tv.tv_sec = (time_t) (timeout / 1000000);
			tv.tv_usec = (suseconds_t) (timeout % 1000000);
		}
		int ret = select(maxFD + 1, &readFDs, &writeFDs, nullptr, (timeout >= 0) ? &tv : nullptr);
		if (ret == -1)
		{
			if (errno != EINTR)
				KPrintf("***** TBasiliskIISerialPortManager::HandleDMA: select failed - %s (%d).\n", strerror(errno), errno);
			continue;
		}
		bool canRead = watchRead && FD_ISSET(ptyFD, &readFDs);
		bool canWrite = watchWrite && FD_ISSET(ptyFD, &writeFDs);

		// Commands from other threads. The pipe only wakes us up, the flag
		// tells us to quit. Empty the pipe first, so no new command gets lost.
		if (FD_ISSET(mPipe[0], &readFDs))
		{
			KUInt8 commands[64];
			while (read(mPipe[0], commands, sizeof(commands)) > 0)
			{
			}
		}
		if (mQuitRequested)
			break;

		// Open the PTY again after it was closed.
		if (pBasiliskLocalFD == -1 && MicrosecondsUntil(mReopenTime) == 0)
		{
			if (OpenPTY())
			{
				pComState = kStateInit;
			} else
			{
				ClosePTY();
				mReopenTime = Clock::now() + kOpenRetryDelay;
			}
		}

		if (canWrite)
			mSendBlocked = false;
		if (canRead)
			HandleDMAReceive();
		HandleDMASend();
	}
	ClosePTY();
	return 0;
}

// -------------------------------------------------------------------------- //
//  * HandleDMASend()
//		Send as many bytes as the throttle allows in one write. While the PTY
//		is closed, the bytes are dropped at the same rate.
// -------------------------------------------------------------------------- //
void
TBasiliskIISerialPortManager::HandleDMASend()
{
	// Wait until the PTY can take more data.
	if (pBasiliskLocalFD != -1 && mSendBlocked)
		return;

	// Suspend() holds this mutex while the emulator state is saved or loaded
	std::lock_guard<std::mutex> theLock(mDMAMutex);

	if (!(mTxDMAIntEnable & 0x00000002) || !mTxDMADataCountdown)
		return; // DMA is disabled or has nothing to send
	KUInt32 n = mTxThrottle.Allowance((mTxDMADataCountdown < kMaxChunkSize) ? mTxDMADataCountdown : kMaxChunkSize);
	if (n == 0)
		return;

	// Collect the bytes from the circular DMA buffer.
	KUInt8 buf[kMaxChunkSize];
	KUInt32 address = mTxDMAPhysicalData;
	KUInt32 bytesToBufferEnd = mTxDMABytesToBufferEnd;
	for (KUInt32 i = 0; i < n; i++)
	{
		mMemory->ReadBP(address, buf[i]);
		address++;
		if (--bytesToBufferEnd == 0)
			address = mTxDMAPhysicalBufferStart;
	}

	KUInt32 nSent = n; // without a PTY, the bytes are dropped
	if (pBasiliskLocalFD != -1)
	{
		// The PTY does not block. If it takes only some of the bytes, the
		// rest is sent in the next round. If it takes none, wait until it
		// can. Other errors drop the bytes, as before.
		ssize_t nWritten = write(pBasiliskLocalFD, buf, n);
		if (nWritten >= 0)
		{
			nSent = (KUInt32) nWritten;
		} else if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			mSendBlocked = true; // select() tells us when it can take more
			return;
		} else if (errno == EINTR)
		{
			return;
		}
	}
	mTxThrottle.Consume(nSent);

	for (KUInt32 i = 0; i < nSent; i++)
	{
		mTxDMAPhysicalData++;
		if (--mTxDMABytesToBufferEnd == 0)
			mTxDMAPhysicalData = mTxDMAPhysicalBufferStart;
	}
	mTxDMADataCountdown -= nSent;
	if (mTxDMADataCountdown == 0)
	{
		// trigger a "send buffer empty" interrupt
		mTxDMAEvent = 0x00000080;
		mInterruptManager->RaiseInterrupt(0x00000100);
	}
}

// -------------------------------------------------------------------------- //
//  * HandleDMAReceive()
//		The PTY is in packet mode (TIOCPKT): every read returns either a
//		single control code, or a 0 followed by data. Copy as many data
//		bytes as the throttle allows to the Newton DMA buffer, and leave the
//		rest in the PTY until the next round.
// -------------------------------------------------------------------------- //
void
TBasiliskIISerialPortManager::HandleDMAReceive()
{
	// no more than receive DMA can take; the rest waits at the host's end
	KUInt32 room = RxDMARoom();
	if (room == 0)
		return;
	KUInt32 nAllowed = mRxThrottle.Allowance((room < kMaxChunkSize) ? room : kMaxChunkSize);
	if (nAllowed == 0)
		return;

	// read bytes that come in through the serial port, plus the packet header
	KUInt8 buf[kMaxChunkSize + 1];
	int n = (int) read(pBasiliskLocalFD, buf, nAllowed + 1);
	if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		return; // nothing to read after all (the PTY does not block)
	if (n <= 0)
	{
		// don't spin on a broken PTY
		mReadRetryTime = Clock::now() + kReadErrorDelay;
		return;
	}

	if (n == 1)
	{
		const uint8_t FLUSHRW = TIOCPKT_FLUSHREAD | TIOCPKT_FLUSHWRITE;
		uint8_t c = buf[0];
		switch (pComState)
		{
			case kStateInit:
				// Initial state. Wait for Basilisk to open and setup its COM port
				if ((c & TIOCPKT_NOSTOP) == TIOCPKT_NOSTOP)
					pComState = kStateOpen;
				break;
			case kStateOpen:
				// The port is open. Wait for Basilisk to flush the COM port after initialising
				if ((c & FLUSHRW) == FLUSHRW)
					pComState = kStateFlushRW;
				break;
			case kStateFlushRW:
				// wait for a second flushing of the COM port. This indicates to us that Basilisk
				// is very likely trying to close the port, but may be deadloking. Help Basilisk
				// out by closing and reopening the pipe. HandleDMA() opens it again.
				if ((c & FLUSHRW) == FLUSHRW)
				{
					ClosePTY();
					mReopenTime = Clock::now() + kReopenDelay;
				}
				break;
		}
		// handle control command in buf[0]
		//				KPrintf("BasiliskII sent control code (%d):", pComState);
		//				if (c&TIOCPKT_FLUSHREAD) puts("     - TIOCPKT_FLUSHREAD");
		//				if (c&TIOCPKT_FLUSHWRITE) puts("     - TIOCPKT_FLUSHWRITE");
		//				if (c&TIOCPKT_STOP) puts("     - TIOCPKT_STOP");
		//				if (c&TIOCPKT_START) puts("     - TIOCPKT_START");
		//				if (c&TIOCPKT_DOSTOP) puts("     - TIOCPKT_DOSTOP");
		//				if (c&TIOCPKT_NOSTOP) puts("     - TIOCPKT_NOSTOP");
		return;
	}

	mRxThrottle.Consume((KUInt32) (n - 1));

	// Suspend() holds this mutex while the emulator state is saved or loaded
	std::lock_guard<std::mutex> theLock(mDMAMutex);
	// handle incomming data (ignore buf[0]!)
	for (int i = 1; i < n; i++)
	{
		KUInt8 data = buf[i];
		mMemory->WriteBP(mRxDMAPhysicalData, data);
		mRxDMAPhysicalData++;
		mRxDMABytesToBufferEnd--;
		if (mRxDMABytesToBufferEnd == 0)
		{ // or mRxDMADataCountdown?
			mRxDMAPhysicalData = mRxDMAPhysicalBufferStart;
		}
		mRxDMADataCountdown--;
		if (mRxDMADataCountdown == 0)
		{
			// buffer overflow?
		}
	}
	mRxDMAEvent = 0x00000040;
	mInterruptManager->RaiseInterrupt(0x00000080); // 0x00000180
}

// ================================================================== //
//  You should've gone to school, you could've learned a trade        //
//  But you laid in bed where the bums have laid                      //
//  Now all the time you're crying that you're underpaid              //
//  It's like that (what?) and that's the way it is                   //
//  Huh!                                                              //
//   - Run D.M.C.                                                     //
// ================================================================== //
