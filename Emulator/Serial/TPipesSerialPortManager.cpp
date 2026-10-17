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

// Throttle the emulated serial line: the time it takes to transfer one byte,
// in microseconds. 0 transfers as fast as the host can.
// 260 us is 38400 bps (ten bits per byte with start and stop bit), the
// fastest speed that NewtonOS supports.
static constexpr int kTxMicrosecondsPerByte = 90; // Newton to sending pipe
static constexpr int kRxMicrosecondsPerByte = 90; // receiving pipe to Newton

#include "TPipesSerialPortManager.h"
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
#endif

#include "Emulator/TInterruptManager.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TLog.h"

// Largest number of bytes read from or written to a pipe at once.
static constexpr KUInt32 kMaxChunkSize = 1024;

// After a read error, wait this long before reading again.
static constexpr std::chrono::milliseconds kReadErrorDelay { 100 };

// -------------------------------------------------------------------------- //
//  * TPipesSerialPortManager()
// Emulate a serial connection using two named pipes via "mkfifo". The pipes
// are named:
// 	$(HOME)/Library/Application Support/Einstein Emulator/ExtrSerPortSend
// and
//  $(HOME)/Library/Application Support/Einstein Emulator/ExtrSerPortRecv")
// -------------------------------------------------------------------------- //
TPipesSerialPortManager::TPipesSerialPortManager(TLog* inLog,
	TSerialPorts::EPortIndex inPortIx) :
		TBasicSerialPortManager(inLog, inPortIx),
		mTxThrottle(kTxMicrosecondsPerByte),
		mRxThrottle(kRxMicrosecondsPerByte)
{
}

// -------------------------------------------------------------------------- //
//  * ~TPipesSerialPortManager( void )
//		The driver is deleted when the user selects another driver. Stop the
//		worker thread first, it uses this object.
// -------------------------------------------------------------------------- //
TPipesSerialPortManager::~TPipesSerialPortManager()
{
	if (mDMAIsRunning)
	{
		TriggerEvent('q');
		pthread_join(mDMAThread, nullptr);
		mDMAIsRunning = false;
	}
	if (mTxPort != -1)
		close(mTxPort);
	if (mRxPort != -1)
		close(mRxPort);
	if (mPipe[0] != -1)
		close(mPipe[0]);
	if (mPipe[1] != -1)
		close(mPipe[1]);
}

// -------------------------------------------------------------------------- //
//  * run( TInterruptManager*, TDMAManager*, TMemory* )
// -------------------------------------------------------------------------- //
void
TPipesSerialPortManager::run(TInterruptManager* inInterruptManager,
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
// 'q' quits the worker thread, any other command just wakes it up.
// -------------------------------------------------------------------------- //
void
TPipesSerialPortManager::TriggerEvent(KUInt8 cmd)
{
	if (cmd == 'q')
		mQuitRequested = true;
	if (mPipe[1] != -1)
	{
		// If the pipe is full, the worker thread will wake up anyway.
		(void) write(mPipe[1], &cmd, 1);
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
	if (!mTxPortName.empty() && !mRxPortName.empty())
		return;

	std::string path = TPathHelper::GetSerialPipeBasePath();

	// create the file path to the sending and receiving node
	path += "/Einstein Emulator";
	if (access(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == -1)
	{
		if (mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == -1)
		{
			KPrintf("***** Error creating named pipe directory %s - %s (%d).\n", path.c_str(), strerror(errno), errno);
		}
	}

	// create the name for the transmitting pipe
	mTxPortName = path + "/ExtrSerPortSend";

	// create the name for the receiving pipe
	mRxPortName = path + "/ExtrSerPortRecv";
}

// -------------------------------------------------------------------------- //
// * CreateNamedPipes
//		Create the named pipes as nodes in the file system.
// -------------------------------------------------------------------------- //
bool
TPipesSerialPortManager::CreateNamedPipes()
{
	// crete the sending node if it does not exist yet
	if (access(mTxPortName.c_str(), S_IRUSR | S_IWUSR) == -1)
	{
		if (mkfifo(mTxPortName.c_str(), S_IRUSR | S_IWUSR) == -1)
		{
			KPrintf("***** Error creating named pipe %s - %s (%d).\n", mTxPortName.c_str(), strerror(errno), errno);
			return false;
		}
	}

	// crete the receiving node if it does not exist yet
	if (access(mRxPortName.c_str(), S_IRUSR | S_IWUSR) == -1)
	{
		if (mkfifo(mRxPortName.c_str(), S_IRUSR | S_IWUSR) == -1)
		{
			KPrintf("***** Error creating named pipe %s - %s (%d).\n", mRxPortName.c_str(), strerror(errno), errno);
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
	if (!CreateNamedPipes())
	{
		return;
	}

	// open the named pipes
	// They do not block: the worker writes while it holds mDMAMutex.
	mTxPort = open(mTxPortName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (mTxPort == -1)
	{
		KPrintf("***** Error opening named sending pipe %s - %s (%d).\n", mTxPortName.c_str(), strerror(errno), errno);
		return;
	}
	mRxPort = open(mRxPortName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (mRxPort == -1)
	{
		KPrintf("***** Error opening named receiving pipe %s - %s (%d).\n", mRxPortName.c_str(), strerror(errno), errno);
		return;
	}
	tcflush(mTxPort, TCIOFLUSH);
	tcflush(mRxPort, TCIOFLUSH);

	// open the thread communication pipe
	int err = pipe(mPipe);
	if (err == -1)
	{
		KPrintf("***** Error opening pipe - %s (%d).\n", strerror(errno), errno);
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
		KPrintf("***** Error creating pthread - %s (%d).\n", strerror(ptErr), ptErr);
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
//		The loop waits for a command, the pipes, or the throttle, whatever
//		comes first. It never sleeps, and never waits while it holds
//		mDMAMutex.
// -------------------------------------------------------------------------- //
void
TPipesSerialPortManager::HandleDMA()
{
	// thread loops and handles pipe, port, and DMA
	for (;;)
	{
		// Decide what to wait for, and how long.
		long long timeout = -1; // microseconds, -1 waits for an event
		auto waitAtMost = [&timeout](long long us) {
			if (timeout < 0 || us < timeout)
				timeout = us;
		};
		bool watchRead = false;
		bool watchWrite = false;

		// Leave new data in the pipe until the throttle allows it, and while
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

		KUInt32 pending = PendingTxBytes();
		if (mSendBlocked)
			watchWrite = true;
		else if (pending)
			waitAtMost(mTxThrottle.Delay(pending));

		fd_set readFDs;
		fd_set writeFDs;
		FD_ZERO(&readFDs);
		FD_ZERO(&writeFDs);
		FD_SET(mPipe[0], &readFDs);
		if (watchRead)
			FD_SET(mRxPort, &readFDs);
		if (watchWrite)
			FD_SET(mTxPort, &writeFDs);
		int maxFD = mPipe[0];
		if (mRxPort > maxFD)
			maxFD = mRxPort;
		if (mTxPort > maxFD)
			maxFD = mTxPort;

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
				KPrintf("***** Error waiting for named pipes - %s (%d).\n", strerror(errno), errno);
			continue;
		}
		bool canRead = watchRead && FD_ISSET(mRxPort, &readFDs);
		bool canWrite = watchWrite && FD_ISSET(mTxPort, &writeFDs);

		// Commands from other threads. The pipe only wakes us up, the flag
		// tells us to quit. Empty the pipe first, so no new command gets lost.
		// TODO: add a command for power on and use it to open
		//		 files and ports.
		// TODO: add a command for power off and use it to close
		//		 files and ports.
		if (FD_ISSET(mPipe[0], &readFDs))
		{
			KUInt8 commands[64];
			while (read(mPipe[0], commands, sizeof(commands)) > 0)
			{
			}
		}
		if (mQuitRequested)
			return;

		if (canWrite)
			mSendBlocked = false;
		if (canRead)
			HandleDMAReceive();
		HandleDMASend();
	}
}

// -------------------------------------------------------------------------- //
//  * HandleDMASend()
//		Send as many bytes as the throttle allows in one write.
// -------------------------------------------------------------------------- //
void
TPipesSerialPortManager::HandleDMASend()
{
	// Wait until the pipe can take more data.
	if (mSendBlocked)
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
		// KPrintf(":::::>> TX: 0x%02X '%c'\n", buf[i], isprint(buf[i])?buf[i]:'.');
		address++;
		if (--bytesToBufferEnd == 0)
			address = mTxDMAPhysicalBufferStart;
	}

	// The pipe does not block. If it takes only some of the bytes, the rest
	// is sent in the next round. If it takes none, wait until it can. Other
	// errors drop the bytes, as before.
	KUInt32 nSent = n;
	ssize_t nWritten = write(mTxPort, buf, n);
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
		// mDMAManager->WriteChannel2Register(1, 1, 0x00000080); // 0x80 = TxBufEmpty, 0x00000180
		// KPrintf(":::::>> buffer is now empty\n");
		mTxDMAEvent = 0x00000080;
		mInterruptManager->RaiseInterrupt(0x00000100);
	}
}

// -------------------------------------------------------------------------- //
//  * HandleDMAReceive()
//		Copy as many bytes as the throttle allows to the Newton DMA buffer,
//		and leave the rest in the pipe until the next round.
//
//		Note that the original Newton hardware and OS did have timing issues
//		when the server PC communicated faster as expected in the year 1996,
//		which lead to CPU cycle burning software like "slowdown.exe". The
//		throttle (kRxMicrosecondsPerByte) takes care of that.
// -------------------------------------------------------------------------- //
void
TPipesSerialPortManager::HandleDMAReceive()
{
	// no more than receive DMA can take; the rest waits at the host's end
	KUInt32 room = RxDMARoom();
	if (room == 0)
		return;
	KUInt32 nAllowed = mRxThrottle.Allowance((room < kMaxChunkSize) ? room : kMaxChunkSize);
	if (nAllowed == 0)
		return;

	// read bytes that come in through the serial port
	KUInt8 buf[kMaxChunkSize];
	int n = (int) read(mRxPort, buf, nAllowed);
	if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
		return; // nothing to read after all (the pipe does not block)
	if (n <= 0)
	{
		// Don't spin on a broken pipe, and report the error only once.
		if (n == -1 && !mReadErrorReported)
		{
			KPrintf("***** Error reading from serial port %s - %s (%d).\n", mRxPortName.c_str(), strerror(errno), errno);
			mReadErrorReported = true;
		}
		mReadRetryTime = Clock::now() + kReadErrorDelay;
		return;
	}
	mReadErrorReported = false;
	mRxThrottle.Consume((KUInt32) n);

	// Suspend() holds this mutex while the emulator state is saved or loaded
	std::lock_guard<std::mutex> theLock(mDMAMutex);
	// KPrintf("----> Received %d bytes data from NCX\n", n);
	for (int i = 0; i < n; i++)
	{
		KUInt8 data = buf[i];
		mMemory->WriteBP(mRxDMAPhysicalData, data);
		// KPrintf(" rx[%.3d] -> %02X '%c'\n", i, data, isprint(data)?data:'.');
		mRxDMAPhysicalData++;
		mRxDMABytesToBufferEnd--;
		if (mRxDMABytesToBufferEnd == 0)
		{ // or mRxDMADataCountdown?
			//					if (mRxDMADataCountdown==0) { // or mRxDMADataCountdown does not work!
			mRxDMAPhysicalData = mRxDMAPhysicalBufferStart;
		}
		mRxDMADataCountdown--;
		if (mRxDMADataCountdown == 0)
		{
			// buffer overflow?
		}
	}
	// KPrintf("===> Start Rx DMA\n");
	mRxDMAEvent = 0x00000040;
	mInterruptManager->RaiseInterrupt(0x00000080); // 0x00000180
	// KPrintf("===> End Rx DMA\n");
}

// ================================================================== //
// You never finish a program, you just stop working on it.           //
//  - Anonymous                                                       //
//                                                                    //
// So true!                                                           //
//  - Matthias Melcher                                                //
// ================================================================== //
