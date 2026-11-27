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
#include "Emulator/Log/KPrintf.h"
#include "Emulator/TARMProcessor.h"

// POSIX & ANSI C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// C++ std lib
#include <chrono>
#include <ctime>

// K
#include <K/Streams/TStream.h>

// Einstein
#include "Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TInterruptManager( TLog*, TARMProcessor* )
// -------------------------------------------------------------------------- //
TInterruptManager::TInterruptManager(TLog* inLog, TARMProcessor* inProcessor) :
		mLog(inLog),
		mProcessor(inProcessor)
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
	KUInt32 inTimer) :
		mLog(inLog),
		mProcessor(inProcessor),
		mTimer(inTimer)
{
	// Start the thread.
	Init();
}

// -------------------------------------------------------------------------- //
//  * ~TInterruptManager( void )
// -------------------------------------------------------------------------- //
TInterruptManager::~TInterruptManager(void)
{
	// Stop the timer thread.
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mExiting.store(true);
		mRunning.store(false);
		mTimerCondVar.notify_one();
	}

	// Wait for the timer thread to finish completely.
	if (mThread.joinable())
	{
		mThread.join();
	}
}

// -------------------------------------------------------------------------- //
//  * Init( void )
// -------------------------------------------------------------------------- //
void
TInterruptManager::Init(void)
{
	// Clear the match registers.
	mMatchRegisters[0] = 0;
	mMatchRegisters[1] = 0;
	mMatchRegisters[2] = 0;
	mMatchRegisters[3] = 0;

	std::unique_lock<std::mutex> lock(mMutex);

	// The timer isn't running.
	mRunning.store(false);
	mTimerStateAcked = false;

	// Create the thread.
	mThread = std::thread(&TInterruptManager::Run, this);

	// Wait on the condition variable for the thread to be running.
	//	KPrintf("%i-Emulator-Sleep-1\n", (int) time(NULL));
	mEmulatorCondVar.wait(lock, [this] { return mTimerStateAcked; });
	//	KPrintf("%i-Emulator-WakeUp-1\n", (int) time(NULL));
}

// -------------------------------------------------------------------------- //
//  * ResumeTimer( void )
// -------------------------------------------------------------------------- //
void
TInterruptManager::ResumeTimer(void)
{
	// Is the timer indeed suspended?
	if (!mRunning.load())
	{
		std::unique_lock<std::mutex> lock(mMutex);

		// Here the timer is waiting (since we have the mutex).

		// Say the timer is running.
		mTimerStateAcked = false;
		mRunning.store(true);

		// Signal the condition variable to wake the timer thread.
		mTimerCondVar.notify_one();

		// Wait for the thread to have been resumed.
		//		KPrintf("%i-Emulator-Sleep-2\n", (int) time(NULL));
		mEmulatorCondVar.wait(lock, [this] { return mTimerStateAcked; });
		//		KPrintf("%i-Emulator-WakeUp-2\n", (int) time(NULL));
	}
}

// -------------------------------------------------------------------------- //
//  * SuspendTimer( void )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SuspendTimer(void)
{
	// Is the timer indeed running?
	if (mRunning.load())
	{
		std::unique_lock<std::mutex> lock(mMutex);

		// Here the timer is waiting (since we have the mutex).

		// Say the timer is not running.
		mTimerStateAcked = false;
		mRunning.store(false);

		// Signal the condition variable to wake the timer thread.
		mTimerCondVar.notify_one();

		// Wait for the thread to have been suspended.
		//		KPrintf("%i-Emulator-Sleep-3\n", (int) time(NULL));
		mEmulatorCondVar.wait(lock, [this] { return mTimerStateAcked; });
		//		KPrintf("%i-Emulator-WakeUp-3\n", (int) time(NULL));
	}
}

// -------------------------------------------------------------------------- //
//  * WaitUntilInterrupt( bool, bool )
// -------------------------------------------------------------------------- //
void
TInterruptManager::WaitUntilInterrupt(bool inMaskIRQ, bool inMaskFIQ)
{
	if (mLog)
	{
		//		mLog->LogLine( "Wait until interrupt" );
	}

	std::unique_lock<std::mutex> lock(mMutex);

	// Here the timer is waiting (since we have the mutex).

	// Say we're waiting.
	mWaiting.store(true);

	// Note what we are waiting for.
	mMaskIRQ = inMaskIRQ;
	mMaskFIQ = inMaskFIQ;

	// Wake the thread now, but it will actually wait until we go to sleep.
	mTimerCondVar.notify_one();

	// Wait for the thread to signal us.
	//	KPrintf("%i-Emulator-Sleep-4\n", (int) time(NULL));
	mEmulatorCondVar.wait(lock);
	//	KPrintf("%i-Emulator-WakeUp-4\n", (int) time(NULL));

	// We're no longer waiting.
	mWaiting.store(false);
}

// -------------------------------------------------------------------------- //
//  * RaiseInterrupt( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::RaiseInterrupt(KUInt32 inIntMask)
{
	std::lock_guard<std::mutex> lock(mMutex);

	// Here the timer is waiting (since we have the mutex).

	// Raise the interrupt.
	mIntRaised |= inIntMask;

	// Signal the condition variable to wake the timer thread.
	mTimerCondVar.notify_one();
}

// -------------------------------------------------------------------------- //
//  * GetRealTimeClock( void ) const
// -------------------------------------------------------------------------- //
KUInt32
TInterruptManager::GetRealTimeClock(void) const
{
	return (KUInt32) (GetHostUTCTime() - mCalendarDelta);
}

// -------------------------------------------------------------------------- //
//  * SetRealTimeClock( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetRealTimeClock(KUInt32 inValue)
{
	std::lock_guard<std::mutex> lock(mMutex);

	// Here the timer is waiting (since we have the mutex).

	// newton = host - delta
	// delta = host - newton
	//  KPrintf("mCalendarDelta was %i\n", (int) mCalendarDelta);
	mCalendarDelta = (KSInt32) (GetHostUTCTime() - inValue);
	//  KPrintf("mCalendarDelta now is %i\n", (int) mCalendarDelta);

	// Signal the condition variable to wake the timer thread.
	mTimerCondVar.notify_one();
}

// -------------------------------------------------------------------------- //
//  * SetAlarm( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetAlarm(KUInt32 inValue)
{
	std::lock_guard<std::mutex> lock(mMutex);

	// Here the timer is waiting (since we have the mutex).

	mAlarmRegister = inValue;
	ArmAlarm(false);

	// Signal the condition variable to wake the timer thread, which raises
	// the alarm interrupt when the RTC reaches the register.
	mTimerCondVar.notify_one();
}

// -------------------------------------------------------------------------- //
//  * ArmAlarm( bool )
// -------------------------------------------------------------------------- //
void
TInterruptManager::ArmAlarm(bool inEvenIfPassed)
{
	KUInt32 now = GetRealTimeClock();
	mAlarmArmTicks = GetTimeInTicks();
	if (mAlarmRegister > now)
	{
		KUInt32 seconds = mAlarmRegister - now;
		if (seconds > 60)
			seconds = 60;
		mAlarmArmed = true;
		mAlarmCheckTicks = seconds * 3686400;
	} else
	{
		mAlarmArmed = inEvenIfPassed;
		mAlarmCheckTicks = 0;
	}
}

// -------------------------------------------------------------------------- //
//  * SetTimerMatchRegister( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetTimerMatchRegister(KUInt32 inMatchReg, KUInt32 inValue)
{
	std::lock_guard<std::mutex> lock(mMutex);

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
		mTimerCondVar.notify_one();
	}
}

// -------------------------------------------------------------------------- //
//  * SetIntCtrlReg( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetIntCtrlReg(KUInt32 inValue)
{
	std::lock_guard<std::mutex> lock(mMutex);

	// Here the timer is waiting (since we have the mutex).

	if (inValue != mIntCtrlReg)
	{
		// Set the interrupt mask (0x0F183400). The OS rewrites it at the
		// start and end of every atomic section.
		mIntCtrlReg = inValue;

		// Signal the condition variable to wake the timer thread.
		mTimerCondVar.notify_one();
	}
}

// -------------------------------------------------------------------------- //
//  * SetIntEDReg1( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetIntEDReg1(KUInt32 inValue)
{
	std::lock_guard<std::mutex> lock(mMutex);

	// Here the timer is waiting (since we have the mutex).

	if (inValue != mIntEDReg1)
	{
		// Set the ED register.
		mIntEDReg1 = inValue;

		// Signal the condition variable to wake the timer thread.
		mTimerCondVar.notify_one();
	}
}

// -------------------------------------------------------------------------- //
//  * SetIntEDReg2( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetIntEDReg2(KUInt32 inValue)
{
	std::lock_guard<std::mutex> lock(mMutex);

	// Here the timer is waiting (since we have the mutex).

	if (inValue != mIntEDReg2)
	{
		// Set the ED register.
		mIntEDReg2 = inValue;

		// Signal the condition variable to wake the timer thread.
		mTimerCondVar.notify_one();
	}
}

// -------------------------------------------------------------------------- //
//  * SetIntEDReg3( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetIntEDReg3(KUInt32 inValue)
{
	std::lock_guard<std::mutex> lock(mMutex);

	// Here the timer is waiting (since we have the mutex).

	if (inValue != mIntEDReg3)
	{
		// Set the ED register.
		mIntEDReg3 = inValue;

		// Signal the condition variable to wake the timer thread.
		mTimerCondVar.notify_one();
	}
}

// -------------------------------------------------------------------------- //
//  * ClearInterrupts( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::ClearInterrupts(KUInt32 inMask)
{
	std::lock_guard<std::mutex> lock(mMutex);

	// Here the timer is waiting (since we have the mutex).

	// Writing 1 to 0x0F183800 clears the pending interrupt. The OS does this
	// before calling the interrupt handler.
	mIntRaised &= ~inMask;

	// Signal the condition variable to wake the timer thread.
	mTimerCondVar.notify_one();
}

// -------------------------------------------------------------------------- //
//  * RaiseGPIO( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::RaiseGPIO(KUInt32 inValue)
{
	mGPIORaised |= inValue;

	if (mGPIOCtrlReg & mGPIORaised)
	{
		// Raise the GPIO interrupt.
		RaiseInterrupt(kGPIOIntMask);

		std::lock_guard<std::mutex> lock(mMutex);

		// Resume the processor if it was waiting in our
		// WaitUntilInterrupt loop.
		if (mWaiting.load())
		{
			mEmulatorCondVar.notify_one();
		}
	}
}

// -------------------------------------------------------------------------- //
//  * SetGPIOCtrlReg( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::SetGPIOCtrlReg(KUInt32 inValue)
{
	if (inValue != mGPIOCtrlReg)
	{
		// Set the GPIO interrupt enable register (0x0F18C400).
		mGPIOCtrlReg = inValue;

		if (mGPIOCtrlReg & mGPIORaised)
		{
			// Enabling a pending GPIO interrupt raises it.
			RaiseInterrupt(kGPIOIntMask);
		}
	}
}

// -------------------------------------------------------------------------- //
//  * ClearGPIO( KUInt32 )
// -------------------------------------------------------------------------- //
void
TInterruptManager::ClearGPIO(KUInt32 inValue)
{
	// Writing 1 to 0x0F18C800 clears the pending GPIO interrupt.
	// kGPIOIntMask itself is cleared by the OS through 0x0F183800.
	mGPIORaised &= ~inValue;
}

// -------------------------------------------------------------------------- //
//  * Run( void )
// -------------------------------------------------------------------------- //
void
TInterruptManager::Run(void)
{
	// Setup: make sure we're waiting on the condition variable.
	std::unique_lock<std::mutex> lock(mMutex);

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
		mTimerDelta = theDelta; // mTimer = ticks - delta
								// newton = host - delta
								// host = newton + delta

		// The alarm check counts host ticks, which wrap after 19 minutes;
		// the timer may have been stopped longer.
		if (mAlarmArmed)
			ArmAlarm(true);
		KUInt32 newTicks = ticks;
		KUInt32 nextMatch;

		if (mRunning.load())
		{
			// Wake the resuming thread.
			mTimerStateAcked = true;
			mEmulatorCondVar.notify_one();
		}

		// Are we running?
		while (mRunning.load())
		{
			// We are running.
			// How long should we sleep?
			if (FireTimersAndFindNext(ticks, newTicks, &nextMatch))
			{
				// Shift the ticks.
				ticks = newTicks;

				// Pass pending and unmasked interrupts to the CPU, as FIQ if
				// set in the FIQ mask, as IRQ otherwise.
				bool gotAnInterrupt = false;
				if (mIntRaised & mIntCtrlReg & mFIQMask)
				{
					mProcessor->FIQInterrupt();
					if ((!mWaiting.load()) || (!mMaskFIQ))
					{
						gotAnInterrupt = true;
					}
				} else
				{
					mProcessor->ClearFIQInterrupt();
				}

				if (mIntRaised & mIntCtrlReg & ~mFIQMask)
				{
					mProcessor->IRQInterrupt();
					if ((!mWaiting.load()) || (!mMaskIRQ))
					{
						gotAnInterrupt = true;
					}
				} else
				{
					mProcessor->ClearIRQInterrupt();
				}

				if (gotAnInterrupt && mWaiting.load())
				{
					// Resume the processor if it was waiting in our
					// WaitUntilInterrupt loop.
					mEmulatorCondVar.notify_one();
				}

				// Save the timer value.
				mTimer = newTicks - mTimerDelta;

				// Wait.
				TicksWaitOnCondVar(lock, nextMatch - newTicks);
			} else
			{
				// Shift the ticks.
				ticks = newTicks;

				// Pass pending and unmasked interrupts to the CPU, as FIQ if
				// set in the FIQ mask, as IRQ otherwise.
				bool gotAnInterrupt = false;
				if (mIntRaised & mIntCtrlReg & mFIQMask)
				{
					mProcessor->FIQInterrupt();
					if ((!mWaiting.load()) || (!mMaskFIQ))
					{
						gotAnInterrupt = true;
					}
				} else
				{
					mProcessor->ClearFIQInterrupt();
				}

				if (mIntRaised & mIntCtrlReg & ~mFIQMask)
				{
					mProcessor->IRQInterrupt();
					if ((!mWaiting.load()) || (!mMaskIRQ))
					{
						gotAnInterrupt = true;
					}
				} else
				{
					mProcessor->ClearIRQInterrupt();
				}

				if (gotAnInterrupt && mWaiting.load())
				{
					// Resume the processor if it was waiting in our
					// WaitUntilInterrupt loop.
					mEmulatorCondVar.notify_one();
				}

				// Save the timer value.
				mTimer = newTicks - mTimerDelta;

				// No interrupt is planned.
				// Wait forever on the condition variable.
				mTimerCondVar.wait(lock);
				//				KPrintf("%i-Timer-WakeUp-1\n", (int) time(NULL));
			}

			if (mExiting.load())
			{
				// Exit.
				break;
			}

			// Get the new time.
			newTicks = GetTimeInTicks();
		}

		if (mExiting.load())
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
		mTimerStateAcked = true;
		mEmulatorCondVar.notify_one();

		// Then wait forever on the condition variable.
		mTimerCondVar.wait(lock);
		//		KPrintf("%i-Timer-WakeUp-2\n", (int) time(NULL));

		if (mExiting.load())
		{
			// Exit.
			break;
		}
	}

	mExiting.store(false);
}

// -------------------------------------------------------------------------- //
//  * GetTimeInTicks( void )
// -------------------------------------------------------------------------- //
inline KUInt32
TInterruptManager::GetTimeInTicks(void)
{
	static const auto sStartTime = std::chrono::steady_clock::now();
	const auto elapsed = std::chrono::steady_clock::now() - sStartTime;
	const auto secs = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
	const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed - secs);
	// NewtonOS hardware clock runs at 3.6864 MHz (3,686,400 ticks/s).
	return static_cast<KUInt32>(
		static_cast<KUInt64>(secs.count()) * 3686400ULL
		+ (static_cast<KUInt64>(ns.count()) * 36864ULL) / 10000000ULL);
}

// -------------------------------------------------------------------------- //
//  * GetTimer( void ) const
// -------------------------------------------------------------------------- //
KUInt32
TInterruptManager::GetTimer(void) const
{
	// Get the time now and subtract with the correction.
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
TInterruptManager::TicksWaitOnCondVar(
	std::unique_lock<std::mutex>& inLock,
	KUInt32 inTicks)
{
	// Translate from ticks:
	// 3.6864 MHz -> 3686400 per seconds.
	KUInt32 sec = inTicks / 3686400;
	KUInt32 rem = inTicks - (sec * 3686400);
	auto amount = std::chrono::seconds(sec)
		+ std::chrono::nanoseconds(static_cast<KUInt64>(rem) * 271ULL); // (1000000000/3686400)

	//	KPrintf("TicksWaitOnCondVar begin (%f)\n", inTicks/4000000.0f);
	mTimerCondVar.wait_for(inLock, amount);
	//	KPrintf("TicksWaitOnCondVar\n" );
	//	KPrintf("%i-Timer-WakeUp-3\n", (int) time(NULL));
}

// -------------------------------------------------------------------------- //
//  * FireTimersAndFindNext( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
inline bool
TInterruptManager::FireTimersAndFindNext(
	KUInt32 inOldTicks,
	KUInt32 inNewTicks,
	KUInt32* outNextMatch)
{
	bool hasNextTimer = false;
	KUInt32 nextTicksValue = 0;
	KUInt32 intRaised = mIntRaised;
	KUInt32 ticksValue;
	KUInt32 delta = mTimerDelta;

	if (inOldTicks < inNewTicks)
	{
		if (!(intRaised & kTimer0IntMask))
		{
			ticksValue = mMatchRegisters[0] + delta; // host = newton + delta
			if ((inOldTicks < ticksValue)
				&& (ticksValue <= inNewTicks))
			{
				intRaised |= kTimer0IntMask;
			} else
			{
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
			} else
			{
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else
				{
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
			} else
			{
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else
				{
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
			} else
			{
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else
				{
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
	} else if (inOldTicks > inNewTicks)
	{
		// Overflow.
		if (!(intRaised & kTimer0IntMask))
		{
			ticksValue = mMatchRegisters[0] + delta;
			if ((inOldTicks < ticksValue)
				|| (ticksValue <= inNewTicks))
			{
				intRaised |= kTimer0IntMask;
			} else
			{
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
			} else
			{
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else
				{
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
			} else
			{
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else
				{
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
			} else
			{
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else
				{
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
	} else
	{
		// inOldTicks == inNewTicks.
		if (!(intRaised & kTimer0IntMask))
		{
			ticksValue = mMatchRegisters[0] + delta;
			if (ticksValue == inNewTicks)
			{
				intRaised |= kTimer0IntMask;
			} else
			{
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
			} else
			{
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else
				{
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
			} else
			{
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else
				{
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
			} else
			{
				if (hasNextTimer)
				{
					nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
				} else
				{
					nextTicksValue = ticksValue;
					hasNextTimer = true;
				}
			}
		}
	}

	// The RTC alarm: raise the interrupt once when the RTC reaches the alarm
	// register. The RTC is only read when the alarm seems due (or a minute
	// has passed), so most turns cost a subtraction. While NewtonOS runs,
	// its timers wake this thread often enough; only while the emulator
	// waits for an interrupt (idle or asleep) must the alarm wake it, and
	// only then is it a timer, because a timed wait costs more than a plain
	// one.
	if (mAlarmArmed && ((KUInt32) (inNewTicks - mAlarmArmTicks) >= mAlarmCheckTicks))
	{
		if (GetRealTimeClock() >= mAlarmRegister)
		{
			intRaised |= kRTCAlarmIntMask;
			mAlarmArmed = false;
			// A sleeping Newton wakes up for the interrupts in the wake-up
			// mask (0x0F184800), not for those in the interrupt mask. Resume
			// the processor if it waits in PowerOffSystem.
			if (mWaiting.load() && (mIntEDReg3 & kRTCAlarmIntMask))
				mEmulatorCondVar.notify_one();
		} else
		{
			ArmAlarm(true); // not yet: check again later
		}
	}
	if (mAlarmArmed && mWaiting.load())
	{
		ticksValue = mAlarmArmTicks + mAlarmCheckTicks;
		if (hasNextTimer)
		{
			nextTicksValue = GetNextTimer(inNewTicks, ticksValue, nextTicksValue);
		} else
		{
			nextTicksValue = ticksValue;
			hasNextTimer = true;
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
	KUInt32 inTimerB)
{
	if (inTimerA <= inTimerB)
	{
		// A <= B
		if (inNow <= inTimerA)
		{
			// now <= A <= B
			return inTimerA;
		} else if (inNow <= inTimerB)
		{
			// A <= now <= B
			return inTimerB;
		} else
		{
			// A <= B <= now
			return inTimerA;
		}
	} else
	{
		// B <= A
		if (inNow <= inTimerB)
		{
			// now <= B <= A
			return inTimerB;
		} else if (inNow <= inTimerA)
		{
			// B <= now <= A
			return inTimerA;
		} else
		{
			// B <= A <= now
			return inTimerB;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * HostUTCOffset( time_t, bool* )
//		Offset of the host's local time from UTC at the time inTime, in
//		seconds, including daylight saving time. outIsDST tells whether
//		daylight saving time is in effect (may be null).
// -------------------------------------------------------------------------- //
static KSInt32
HostUTCOffset(time_t inTime, bool* outIsDST)
{
	struct tm gm;
	struct tm local;
#ifdef _WIN32
	gmtime_s(&gm, &inTime);
	gm.tm_isdst = -1;
	localtime_s(&local, &inTime);
#else
	gmtime_r(&inTime, &gm);
	gm.tm_isdst = -1;
	localtime_r(&inTime, &local);
#endif
	if (outIsDST)
		*outIsDST = (local.tm_isdst > 0);
	return (KSInt32) difftime(mktime(&local), mktime(&gm));
}

// -------------------------------------------------------------------------- //
//  * GetHostLocalTime( void )
// -------------------------------------------------------------------------- //
KUInt32
TInterruptManager::GetHostLocalTime(void)
{
	time_t t = time(NULL);
	return (KUInt32) (t + HostUTCOffset(t, nullptr) + kEpochInNewtonBase);
}


// -------------------------------------------------------------------------- //
//  * GetHostUTCTime( void )
// -------------------------------------------------------------------------- //
KUInt32
TInterruptManager::GetHostUTCTime(void)
{
	return (KUInt32) (time(NULL) + kEpochInNewtonBase);
}

// -------------------------------------------------------------------------- //
//  * GetHostTimeZone( KSInt32&, KSInt32& )
// -------------------------------------------------------------------------- //
void
TInterruptManager::GetHostTimeZone(KSInt32& outGMTOffset, KSInt32& outDSTOffset)
{
	bool isDST = false;
	KSInt32 theOffset = HostUTCOffset(time(NULL), &isDST);
	outDSTOffset = isDST ? 3600 : 0;
	outGMTOffset = theOffset - outDSTOffset;
}

// -------------------------------------------------------------------------- //
//  * TransferState( TStream* ) const
// -------------------------------------------------------------------------- //
void
TInterruptManager::TransferState(TStream* inStream)
{
	// mRunning, mExiting and mWaiting control the timer thread, not the
	// emulated machine, and are not saved.

	// Other threads (sound, serial) may raise interrupts at any time. Hold the
	// mutex, so the registers are saved or loaded as one consistent set. The
	// guard unlocks it when we leave, even if reading fails with an exception.
	std::lock_guard<std::mutex> lock(mMutex);

	// Interrupt manager specific stuff.
	// A reset clears the interrupt controller. The clock (calendar delta,
	// timer) keeps running.
	inStream->TransferInt32BE(mMaskIRQ, 0);
	inStream->TransferInt32BE(mMaskFIQ, 0);
	inStream->TransferInt32BE(mIntRaised, 0);
	inStream->TransferInt32BE(mIntCtrlReg, 0);
	inStream->TransferInt32BE(mFIQMask, 0);
	inStream->TransferInt32BE(mIntEDReg1, 0);
	inStream->TransferInt32BE(mIntEDReg2, 0);
	inStream->TransferInt32BE(mIntEDReg3, 0);
	inStream->TransferInt32BE(mGPIORaised, 0);
	inStream->TransferInt32BE(mGPIOCtrlReg, 0);
	inStream->TransferInt32BE(mCalendarDelta);

	inStream->TransferInt32BE(mAlarmRegister, 0);
	if (inStream->IsReading() || inStream->IsResetting())
	{
		mAlarmArmed = false;
		if (mAlarmRegister != 0)
			ArmAlarm(true);
	}
	inStream->TransferInt32BE(mTimerDelta);
	inStream->TransferInt32BE(mTimer);
	inStream->TransferInt32ArrayBE(
		mMatchRegisters,
		sizeof(mMatchRegisters) / sizeof(KUInt32), 0);
}

// ======================================== //
// Cobol programmers are down in the dumps. //
// ======================================== //
