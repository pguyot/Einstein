// ==============================
// File:			TInterruptManager.cp
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

#include "TInterruptManager.h"
#include "Emulator/TARMProcessor.h"

// POSIX & ANSI C
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#if TARGET_OS_WIN32
	#include <time.h>
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
#endif

// Mach
// #include <mach/mach.h>
// #include <mach/clock.h>
// #include <mach/mach_error.h>

// K
#include <K/Threads/TThread.h>
#include <K/Threads/TMutex.h>
#include <K/Streams/TStream.h>

// Einstein
#include "Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TInterruptManager( TLog*, TARMProcessor* )
// -------------------------------------------------------------------------- //
TInterruptManager::TInterruptManager( TLog* inLog, TARMProcessor* inProcessor )
	:
		mLog( inLog ),
		mProcessor( inProcessor ),
		mExiting( false ),
		mWaiting( false ),
		mMaskIRQ( false ),
		mMaskFIQ( false ),
		mIntRaised( 0 ),
		mIntCtrlReg( 0 ),
		mFIQMask( 0 ),
		mIntEDReg1( 0 ),
		mIntEDReg2( 0 ),
		mIntEDReg3( 0 ),
		mGPIORaised( 0 ),
		mGPIOCtrlReg( 0 ),
		mCalendarDelta( GetSyncedCalendarDelta() ),
		mAlarmRegister( 0 ),
		mTimerDelta( 0 ),
		mTimer( 0 )
{
	// Start the thread.
	Init();
}

// -------------------------------------------------------------------------- //
//  * TInterruptManager( TLog*, TARMProcessor*, KUInt32 )
// -------------------------------------------------------------------------- //
TInterruptManager::TInterruptManager(
						TLog* inLog,
						TARMProcessor* inProcessor,
						KUInt32 inTimer )
	:
		mLog( inLog ),
		mProcessor( inProcessor ),
		mExiting( false ),
		mWaiting( false ),
		mMaskIRQ( false ),
		mMaskFIQ( false ),
		mIntRaised( 0 ),
		mIntCtrlReg( 0 ),
		mFIQMask( 0 ),
		mIntEDReg1( 0 ),
		mIntEDReg2( 0 ),
		mIntEDReg3( 0 ),
		mGPIORaised( 0 ),
		mGPIOCtrlReg( 0 ),
		mCalendarDelta( GetSyncedCalendarDelta() ),
		mAlarmRegister( 0 ),
		mTimerDelta( 0 ),
		mTimer( inTimer ),
		mTimerCondVar( nil ),
		mEmulatorCondVar( nil ),
		mMutex( nil ),
		mThread( nil )
{
	// Start the thread.
	Init();
}

// -------------------------------------------------------------------------- //
//  * ~TInterruptManager( void )
// -------------------------------------------------------------------------- //
TInterruptManager::~TInterruptManager( void )
{
	// Stop the timer thread.
	mMutex->Lock();
	
	mExiting = true;
	mTimerCondVar->Signal();

	mMutex->Unlock();
	
	// Wait for the thread to finish.
	while (mExiting) {};

	if (mThread)
	{
		delete mThread;
	}
	if (mTimerCondVar)
	{
		delete mTimerCondVar;
	}
	if (mEmulatorCondVar)
	{
		delete mEmulatorCondVar;
	}
	if (mMutex)
	{
		delete mMutex;
	}
}

// -------------------------------------------------------------------------- //
//  * Init( void )
// -------------------------------------------------------------------------- //
void
TInterruptManager::Init( void )
{
	// Clear the match registers.
	mMatchRegisters[0] = 0;
	mMatchRegisters[1] = 0;
	mMatchRegisters[2] = 0;
	mMatchRegisters[3] = 0;

	mTimerCondVar = new TCondVar();
	mEmulatorCondVar = new TCondVar();
	mMutex = new TMutex();

	mMutex->Lock();
	
	// The timer isn't running.
	mRunning = false;

	// Create the thread
	mThread = new TThread(this);
	
	// Wait on the condition variable for the thread to be running.
//	fprintf(stderr, "%i-Emulator-Sleep-1\n", (int) time(NULL));
	mEmulatorCondVar->Wait(mMutex);
//	fprintf(stderr, "%i-Emulator-WakeUp-1\n", (int) time(NULL));
	
	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * ResumeTimer( void )
// -------------------------------------------------------------------------- //
void
TInterruptManager::ResumeTimer( void )
{
	// Is the timer indeed suspended?
	if (!mRunning)
	{	
		mMutex->Lock();
		
		// Here the timer is waiting (since we have the mutex).
		
		// Say the timer is running.
		mRunning = true;
		
		// Signal the condition variable to wake the timer thread.
		mTimerCondVar->Signal();

		// Wait for the thread to have been resumed.
//		fprintf(stderr, "%i-Emulator-Sleep-2\n", (int) time(NULL));
		mEmulatorCondVar->Wait(mMutex);
//		fprintf(stderr, "%i-Emulator-WakeUp-2\n", (int) time(NULL));

		// Release the mutex, so the timer thread will get it back.
		mMutex->Unlock();
	}
}

// -------------------------------------------------------------------------- //
//  * SuspendTimer( void )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SuspendTimer( void )
{
	// Is the timer indeed running?
	if (mRunning)
	{	
		mMutex->Lock();
		
		// Here the timer is waiting (since we have the mutex).

		// Say the timer is not running.
		mRunning = false;
		
		// Signal the condition variable to wake the timer thread.
		mTimerCondVar->Signal();

		// Wait for the thread to have been suspended.
//		fprintf(stderr, "%i-Emulator-Sleep-3\n", (int) time(NULL));
		mEmulatorCondVar->Wait(mMutex);
//		fprintf(stderr, "%i-Emulator-WakeUp-3\n", (int) time(NULL));

		// Release the mutex.
		mMutex->Unlock();		
	}
}

// -------------------------------------------------------------------------- //
//  * WaitUntilInterrupt( Boolean, Boolean )
// -------------------------------------------------------------------------- //
void
TInterruptManager::WaitUntilInterrupt( Boolean inMaskIRQ, Boolean inMaskFIQ )
{
	if (mLog)
	{
//		mLog->LogLine( "Wait until interrupt" );
	}

	mMutex->Lock();
	
	// Here the timer is waiting (since we have the mutex).

	// Say we're waiting.
	mWaiting = true;
	
	// Note what we are waiting for.
	mMaskIRQ = inMaskIRQ;
	mMaskFIQ = inMaskFIQ;

	// Wake the thread now, but it will actually wait until we go to sleep.
	mTimerCondVar->Signal();
	
	// Wait for the thread to signal us.
//	fprintf(stderr, "%i-Emulator-Sleep-4\n", (int) time(NULL));
	mEmulatorCondVar->Wait(mMutex);
//	fprintf(stderr, "%i-Emulator-WakeUp-4\n", (int) time(NULL));
	
	// We're no longer waiting.
	mWaiting = false;
	
	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * RaiseInterrupt( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::RaiseInterrupt( KUInt32 inIntMask )
{
	mMutex->Lock();
	
	// Here the timer is waiting (since we have the mutex).

	// Raise the interrupt.
	mIntRaised |= inIntMask;

	// Signal the condition variable to wake the timer thread.
	mTimerCondVar->Signal();

	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * GetRealTimeClock( void ) const
// -------------------------------------------------------------------------- //
KUInt32
TInterruptManager::GetRealTimeClock( void ) const
{
	time_t now = time(NULL);

	return (KUInt32) (now - mCalendarDelta);
}

// -------------------------------------------------------------------------- //
//  * SetRealTimeClock( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetRealTimeClock( KUInt32 inValue )
{
	mMutex->Lock();
	
	// Here the timer is waiting (since we have the mutex).

	// newton = host - delta
	// delta = host - newton
//  fprintf(stderr, "mCalendarDelta was %i\n", (int) mCalendarDelta);
	mCalendarDelta = (KSInt32) (time(NULL) - inValue);
//  fprintf(stderr, "mCalendarDelta now is %i\n", (int) mCalendarDelta);
	
	// Signal the condition variable to wake the timer thread.
	mTimerCondVar->Signal();

	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * SetAlarm( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetAlarm( KUInt32 inValue )
{
	mMutex->Lock();
	
	// Here the timer is waiting (since we have the mutex).

	mAlarmRegister = inValue;
	
	if (mIntCtrlReg & kRTCAlarmIntMask)
	{
		// Signal the condition variable to wake the timer thread.
		mTimerCondVar->Signal();
	}

	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * SetTimerMatchRegister( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetTimerMatchRegister( KUInt32 inMatchReg, KUInt32 inValue )
{
	mMutex->Lock();
	
	// Here the timer is waiting (since we have the mutex).

//	if (mLog)
//	{
//		mLog->FLogLine(
//			"Set match register %i to 0x%.8X",
//			(int) inMatchReg,
//			(unsigned int) inValue );
//	}

	// Set the match register.
	mMatchRegisters[inMatchReg] = inValue;

	if (mIntCtrlReg & (kTimer0IntMask << inMatchReg))
	{
		// Signal the condition variable to wake the timer thread.
		mTimerCondVar->Signal();
	}

	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * SetIntCtrlReg( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetIntCtrlReg( KUInt32 inValue )
{
	mMutex->Lock();
	
	// Here the timer is waiting (since we have the mutex).

	if (inValue != mIntCtrlReg)
	{
		// Set the control register.
		mIntCtrlReg = inValue;
	
		// Signal the condition variable to wake the timer thread.
		mTimerCondVar->Signal();
	}

	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * SetIntEDReg1( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetIntEDReg1( KUInt32 inValue )
{
	mMutex->Lock();
	
	// Here the timer is waiting (since we have the mutex).

	if (inValue != mIntEDReg1)
	{
		// Set the ED register.
		mIntEDReg1 = inValue;
	
		// Signal the condition variable to wake the timer thread.
		mTimerCondVar->Signal();
	}

	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * SetIntEDReg2( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetIntEDReg2( KUInt32 inValue )
{
	mMutex->Lock();
	
	// Here the timer is waiting (since we have the mutex).

	if (inValue != mIntEDReg2)
	{
		// Set the ED register.
		mIntEDReg2 = inValue;
	
		// Signal the condition variable to wake the timer thread.
		mTimerCondVar->Signal();
	}

	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * SetIntEDReg3( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetIntEDReg3( KUInt32 inValue )
{
	mMutex->Lock();
	
	// Here the timer is waiting (since we have the mutex).

	if (inValue != mIntEDReg3)
	{
		// Set the ED register.
		mIntEDReg3 = inValue;
	
		// Signal the condition variable to wake the timer thread.
		mTimerCondVar->Signal();
	}

	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * ClearInterrupts( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::ClearInterrupts( KUInt32 inMask )
{
	mMutex->Lock();
	
	// Here the timer is waiting (since we have the mutex).

	// Clear the interrupts.
	mIntRaised &= ~inMask;
	
	// Signal the condition variable to wake the timer thread.
	mTimerCondVar->Signal();

	// Release the mutex, so the timer thread will get it back.
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * RaiseGPIO( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::RaiseGPIO( KUInt32 inValue )
{
	mGPIORaised |= inValue;
	
	if (mGPIOCtrlReg & mGPIORaised)
	{
		// Raise the GPIO interrupt.
		RaiseInterrupt(kGPIOIntMask);
		
		mMutex->Lock();

		// Resume the processor if it was waiting in our
		// WaitUntilInterrupt loop.
		if (mWaiting)
		{
			mEmulatorCondVar->Signal();
		}

		mMutex->Unlock();
	}
}

// -------------------------------------------------------------------------- //
//  * SetGPIOCtrlReg( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetGPIOCtrlReg( KUInt32 inValue )
{
	if (inValue != mGPIOCtrlReg)
	{
		// Set the control register.
		mGPIOCtrlReg = inValue;
		
		if (mGPIOCtrlReg & mGPIORaised)
		{
			// Raise the GPIO interrupt.
			RaiseInterrupt(kGPIOIntMask);
		}
	}
}

// -------------------------------------------------------------------------- //
//  * ClearGPIO( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::ClearGPIO( KUInt32 inValue )
{
	mGPIORaised &= ~inValue;
}

// -------------------------------------------------------------------------- //
//  * Run( void )
// -------------------------------------------------------------------------- //
void
TInterruptManager::Run( void )
{
	// Setup: make sure we're waiting on the condition variable.	
	mMutex->Lock();
	
	// The init thread is now waiting for sure, since we have the mutex.
	
	// We have the mutex back.
	
	while (true)
	{
		// Get the time now.
		KUInt32 ticks = GetTimeInTicks();

		// We resume where the timer was.
		KUInt32 theDelta = ticks - mTimer;
//		if (mLog)
//		{
//			mLog->FLogLine(
//				"Delta = %.8X, Shift = %.8X, ticks = %.8X, timer = %.8X",
//				(unsigned int) theDelta,
//				(unsigned int) (theDelta - mTimerDelta),
//				(unsigned int) ticks,
//				(unsigned int) mTimer );
//		}
		mTimerDelta = theDelta;					// mTimer = ticks - delta
												// newton = host - delta
												// host = newton + delta
		KUInt32 newTicks = ticks;
		KUInt32 nextMatch;

		if (mRunning)
		{
			// Wake the resuming thread.
			mEmulatorCondVar->Signal();
		}

		// Are we running?
		while (mRunning)
		{
			// We are running.
			// How long should we sleep?
			if (FireTimersAndFindNext( ticks, newTicks, &nextMatch ))
			{
				// Shift the ticks.
				ticks = newTicks;

				// Raise the interrupts.
				Boolean gotAnInterrupt = false;
				if (mIntRaised & mIntCtrlReg & mFIQMask)
				{
					mProcessor->FIQInterrupt();
					if ((!mWaiting) || (!mMaskFIQ))
					{
						gotAnInterrupt = true;
					}
				} else {
					mProcessor->ClearFIQInterrupt();
				}
				
				if (mIntRaised & mIntCtrlReg & ~mFIQMask)
				{
					mProcessor->IRQInterrupt();
					if ((!mWaiting) || (!mMaskIRQ))
					{
						gotAnInterrupt = true;
					}
				} else {
					mProcessor->ClearIRQInterrupt();
				}
				
				if (gotAnInterrupt && mWaiting)
				{
					// Resume the processor if it was waiting in our
					// WaitUntilInterrupt loop.
					mEmulatorCondVar->Signal();
				}

				// Save the timer value.
				mTimer = newTicks - mTimerDelta;

				// Wait.
				TicksWaitOnCondVar( nextMatch - newTicks );
			} else {
				// Shift the ticks.
				ticks = newTicks;

				// Raise the interrupts.
				Boolean gotAnInterrupt = false;
				if (mIntRaised & mIntCtrlReg & mFIQMask)
				{
					mProcessor->FIQInterrupt();
					if ((!mWaiting) || (!mMaskFIQ))
					{
						gotAnInterrupt = true;
					}
				} else {
					mProcessor->ClearFIQInterrupt();
				}
				
				if (mIntRaised & mIntCtrlReg & ~mFIQMask)
				{
					mProcessor->IRQInterrupt();
					if ((!mWaiting) || (!mMaskIRQ))
					{
						gotAnInterrupt = true;
					}
				} else {
					mProcessor->ClearIRQInterrupt();
				}
				
				if (gotAnInterrupt && mWaiting)
				{
					// Resume the processor if it was waiting in our
					// WaitUntilInterrupt loop.
					mEmulatorCondVar->Signal();
				}

				// Save the timer value.
				mTimer = newTicks - mTimerDelta;

				// No interrupt is planned.
				// Wait forever on the condition variable.
				mTimerCondVar->Wait(mMutex);
//				fprintf(stderr, "%i-Timer-WakeUp-1\n", (int) time(NULL));
			}

			if (mExiting)
			{
				// Exit.
				break;
			}

			// Get the new time.
			newTicks = GetTimeInTicks();
		}
		
		if (mExiting)
		{
			// Exit.
			break;
		}

		// We're no longer running.
		// Save the timer value.
		mTimer = newTicks - mTimerDelta;
		
//		if (mLog)
//		{
//			mLog->FLogLine(
//				"Delta = %.8X, newTicks = %.8X, timer = %.8X",
//				(unsigned int) theDelta,
//				(unsigned int) newTicks,
//				(unsigned int) mTimer );
//		}
		
		// Wake the suspending thread.
		mEmulatorCondVar->Signal();

		// Then wait forever on the condition variable.
		mTimerCondVar->Wait(mMutex);
//		fprintf(stderr, "%i-Timer-WakeUp-2\n", (int) time(NULL));

		if (mExiting)
		{
			// Exit.
			break;
		}
	}

	mMutex->Unlock();
	
	mExiting = false;
}

// -------------------------------------------------------------------------- //
//  * GetTimeInTicks( void )
// -------------------------------------------------------------------------- //
inline KUInt32
TInterruptManager::GetTimeInTicks( void )
{
#if TARGET_OS_WIN32
	// FIXME optimize this to avoid using floating point variables
	static BOOL checked = FALSE, found = FALSE;
	static double mult = 0.0;
	if (!checked) {
		checked = TRUE;
		LARGE_INTEGER f;
		found = QueryPerformanceFrequency(&f);
		if (found) {
			if (f.QuadPart==0)
				found = FALSE;
			else
				mult = 4000000.0/((double)f.QuadPart);
		}
	}
	if (found) {
		LARGE_INTEGER f;
		QueryPerformanceCounter(&f);
		return (KUInt32)(((double)f.QuadPart)*mult);
	} else {
		struct _timeb ft;
		_ftime(&ft);
		KUInt32 theResult = ft.time * 4000000;
		theResult += (KUInt32)(ft.millitm * 4000);
		return theResult;
	}
#else
	// Get the time now.
	struct timeval now;
	(void) gettimeofday( &now, NULL );
	
//	kern_return_t ret;
//	clock_serv_t aClock;
//	mach_timespec_t now;

//	ret = host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &aClock);
//	if (ret != KERN_SUCCESS) {
//		fprintf(stderr, "host_get_clock_service() failed: %s\n", mach_error_string(ret));
//		abort();
//	}
//	ret = clock_get_time(aClock, &now);
//	if (ret != KERN_SUCCESS) {
//		fprintf(stderr, "clock_get_time() failed: %s\n", mach_error_string(ret));
//		abort();
//	}

	// Translate to ticks:
#if 1
	// slower and accurate
	// 3.6864 MHz -> 3686400 per seconds.
	KUInt32 theResult = (KUInt32) now.tv_sec * 3686400;
	//theResult += (KUInt32) (now.tv_usec * 3.6864);
	theResult += (KUInt32)((((KUInt64)now.tv_usec) * 36864) / 10000);
#else
	// faster and inaccurate
	// We simply multiply with 4000000 to avoid the expensive float to int
	// conversion.
	KUInt32 theResult = now.tv_sec * 4000000;
	theResult += (KUInt32) (now.tv_usec * 4);
#endif
	return theResult;
#endif
}

// -------------------------------------------------------------------------- //
//  * GetTimer( void ) const
// -------------------------------------------------------------------------- //
KUInt32
TInterruptManager::GetTimer( void ) const
{
	// Get the time now and substract with the correction.
	KUInt32 theResult = GetTimeInTicks() - mTimerDelta;
//	if (mLog)
//	{
//		mLog->FLogLine(
//			"GetTimer: %.8X (delta = %.8X)",
//			(unsigned int) theResult,
//			(unsigned int) mTimerDelta );
//	}

	return theResult;
}

// -------------------------------------------------------------------------- //
//  * TimedWaitOnCondVar( KUInt32 )
// -------------------------------------------------------------------------- //
inline void
TInterruptManager::TicksWaitOnCondVar( KUInt32 inTicks )
{
	// Translate from ticks:
#if 1
	// slower and accurate
	// 3.6864 MHz -> 3686400 per seconds.
	KUInt32 sec = inTicks / 3686400;
	KUInt32 rem = inTicks - (sec*3686400);
	struct timespec amount;
	amount.tv_nsec = rem * 271; // (1000000000/3686400)
	amount.tv_sec  = sec;
#else
	// faster and inaccurate
	// We simply divide with 4000000 to avoid the expensive float to int
	// conversion.
	struct timespec amount;
	amount.tv_nsec = (inTicks * 250) % 1000000000;
	amount.tv_sec = inTicks / 4000000;
#endif

//	fprintf(stderr, "TicksWaitOnCondVar begin (%f)\n", inTicks/4000000.0f);
	mTimerCondVar->TimedWaitRelative( mMutex, &amount );
//	fprintf(stderr, "TicksWaitOnCondVar\n" );
//	fprintf(stderr, "%i-Timer-WakeUp-3\n", (int) time(NULL));
}

// -------------------------------------------------------------------------- //
//  * FireTimersAndFindNext( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
inline Boolean
TInterruptManager::FireTimersAndFindNext(
							KUInt32 inOldTicks,
							KUInt32 inNewTicks,
							KUInt32* outNextMatch )
{
	Boolean hasNextTimer = false;
	KUInt32 nextTicksValue = 0;
	KUInt32 intRaised = mIntRaised;
	KUInt32 ticksValue;
	KUInt32 delta = mTimerDelta;

	if (inOldTicks < inNewTicks)
	{
		if (!(intRaised & kTimer0IntMask))
		{
			ticksValue = mMatchRegisters[0] + delta;	// host = newton + delta
			if ((inOldTicks < ticksValue)
				&& (ticksValue <= inNewTicks))
			{
				intRaised |= kTimer0IntMask;
			} else {
				nextTicksValue = ticksValue;
				hasNextTimer = true;
			}
		}
		if (!(intRaised & kTimer1IntMask))
		{
			ticksValue = mMatchRegisters[1] + delta;
			if ((inOldTicks < ticksValue)
				&& (ticksValue <= inNewTicks))
			{
				intRaised |= kTimer1IntMask;
			} else {
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else {
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
		if (!(intRaised & kTimer2IntMask))
		{
			ticksValue = mMatchRegisters[2] + delta;
			if ((inOldTicks < ticksValue)
				&& (ticksValue <= inNewTicks))
			{
				intRaised |= kTimer2IntMask;
			} else {
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else {
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
		if (!(intRaised & kTimer3IntMask))
		{
			ticksValue = mMatchRegisters[3] + delta;
			if ((inOldTicks < ticksValue)
				&& (ticksValue <= inNewTicks))
			{
				intRaised |= kTimer3IntMask;
			} else {
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else {
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
	} else if (inOldTicks > inNewTicks) {
		// Overflow.
		if (!(intRaised & kTimer0IntMask))
		{
			ticksValue = mMatchRegisters[0] + delta;
			if ((inOldTicks < ticksValue)
				|| (ticksValue <= inNewTicks))
			{
				intRaised |= kTimer0IntMask;
			} else {
				nextTicksValue = ticksValue;
				hasNextTimer = true;
			}
		}
		if (!(intRaised & kTimer1IntMask))
		{
			ticksValue = mMatchRegisters[1] + delta;
			if ((inOldTicks < ticksValue)
				|| (ticksValue <= inNewTicks))
			{
				intRaised |= kTimer1IntMask;
			} else {
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else {
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
		if (!(intRaised & kTimer2IntMask))
		{
			ticksValue = mMatchRegisters[2] + delta;
			if ((inOldTicks < ticksValue)
				|| (ticksValue <= inNewTicks))
			{
				intRaised |= kTimer2IntMask;
			} else {
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else {
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
		if (!(intRaised & kTimer3IntMask))
		{
			ticksValue = mMatchRegisters[3] + delta;
			if ((inOldTicks < ticksValue)
				|| (ticksValue <= inNewTicks))
			{
				intRaised |= kTimer3IntMask;
			} else {
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else {
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
	} else {
		// inOldTicks == inNewTicks.
		if (!(intRaised & kTimer0IntMask))
		{
			ticksValue = mMatchRegisters[0] + delta;
			if (ticksValue == inNewTicks)
			{
				intRaised |= kTimer0IntMask;
			} else {
				nextTicksValue = ticksValue;
				hasNextTimer = true;
			}
		}
		if (!(intRaised & kTimer1IntMask))
		{
			ticksValue = mMatchRegisters[1] + delta;
			if (ticksValue == inNewTicks)
			{
				intRaised |= kTimer1IntMask;
			} else {
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else {
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
		if (!(intRaised & kTimer2IntMask))
		{
			ticksValue = mMatchRegisters[2] + delta;
			if (ticksValue == inNewTicks)
			{
				intRaised |= kTimer2IntMask;
			} else {
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else {
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
		if (!(intRaised & kTimer3IntMask))
		{
			ticksValue = mMatchRegisters[3] + delta;
			if (ticksValue == inNewTicks)
			{
				intRaised |= kTimer3IntMask;
			} else {
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else {
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
	}

	*outNextMatch = nextTicksValue;
	mIntRaised = intRaised;
	
	return hasNextTimer;
}

// -------------------------------------------------------------------------- //
//  * GetNextTimer( void )
// -------------------------------------------------------------------------- //
inline KUInt32
TInterruptManager::GetNextTimer(
						KUInt32 inNow,
						KUInt32 inTimerA,
						KUInt32 inTimerB )
{
	if (inTimerA <= inTimerB)
	{
		// A <= B
		if (inNow <= inTimerA)
		{
			// now <= A <= B
			return inTimerA;
		} else if (inNow <= inTimerB) {
			// A <= now <= B
			return inTimerB;
		} else {
			// A <= B <= now
			return inTimerA;
		}
	} else {
		// B <= A
		if (inNow <= inTimerB)
		{
			// now <= B <= A
			return inTimerB;
		} else if (inNow <= inTimerA) {
			// B <= now <= A
			return inTimerA;
		} else {
			// B <= A <= now
			return inTimerB;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * GetSyncedCalendarDelta( void )
// -------------------------------------------------------------------------- //
KUInt32
TInterruptManager::GetSyncedCalendarDelta( void )
{
	// Currently, we just suppose the epoch is January, 1st, 1970.
	return (KUInt32) (-kEpochInNewtonBase);
}


// -------------------------------------------------------------------------- //
//  * TransferState( TStream* ) const
// -------------------------------------------------------------------------- //
void
TInterruptManager::TransferState( TStream* inStream )
{
	KUInt32 t;
	
	// Interrupt manager specific stuff.
	t = mRunning; inStream->TransferInt32BE( t ); mRunning = t;
	t = mExiting; inStream->TransferInt32BE( t ); mExiting = t;

	t = mWaiting; inStream->TransferInt32BE( t ); mWaiting  = t;

	inStream->TransferInt32BE( mMaskIRQ );
	inStream->TransferInt32BE( mMaskFIQ );
	inStream->TransferInt32BE( mIntRaised );
	inStream->TransferInt32BE( mIntCtrlReg );
	inStream->TransferInt32BE( mFIQMask );
	inStream->TransferInt32BE( mIntEDReg1 );
	inStream->TransferInt32BE( mIntEDReg2 );
	inStream->TransferInt32BE( mIntEDReg3 );
	inStream->TransferInt32BE( mGPIORaised );
	inStream->TransferInt32BE( mGPIOCtrlReg );
	inStream->TransferInt32BE( mCalendarDelta );
	
	inStream->TransferInt32BE( mAlarmRegister );
	inStream->TransferInt32BE( mTimerDelta );
	inStream->TransferInt32BE( mTimer );
	inStream->TransferInt32ArrayBE(
				mMatchRegisters,
				sizeof(mMatchRegisters) / sizeof(KUInt32) );
}


// ======================================== //
// Cobol programmers are down in the dumps. //
// ======================================== //
