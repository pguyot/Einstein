// ==============================
// File:			TInterruptManager.h
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

#ifndef _TINTERRUPTMANAGER_H
#define _TINTERRUPTMANAGER_H

#include <K/Defines/KDefinitions.h>

// ANSI C & POSIX
#include <stdio.h>

// K
#include <K/Threads/TCondVar.h>

class TARMProcessor;
class TLog;
class TThread;
class TMutex;
class TStream;

///
/// Class for the interrupt manager.
/// The interrupt manager is a thread that waits most of the time for the next
/// interrupt to happen. It is also woken up when the timer variables change
/// (either some timers are enabled/disabled or the values changed).
/// The interrupt manager also handles interrupts raised by other bits of the
/// emulator.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TInterruptManager
{
public:
	///
	/// Constants for the interrupts.
	///
	enum {
		kRTCAlarmIntMask	= 0x00000004,
		kTimer0IntMask		= 0x00000008,
		kTimer1IntMask		= 0x00000010,
		kTimer2IntMask		= 0x00000020,
		kTimer3IntMask		= 0x00000040,
		kDMAChannel0IntMask	= 0x00000080,						// Serial port 0 rcv
		kDMAChannel1IntMask	= 0x00000100,						// Serial port 0 tx
		kDMAChannel2IntMask	= 0x00000200,						// IR rcv/tx
		kDMAChannel3IntMask	= 0x00000400,   ///< Sound input	// Audio transmit
		kDMAChannel4IntMask	= 0x00000800,						// Audio receive
		kDMAChannel5IntMask	= 0x00001000,   ///< Sound output	// Tablet rcv
		kDMAChannel6IntMask	= 0x00002000,						// Modem (3) rcv
		kDMAChannel7IntMask	= 0x00004000,						// Modem (3) tx
		kKeynesIntMask		= 0x00008000,						// BIO Interface (FIQ)
		kPCMCIA0IntMask		= 0x00010000,						// 0x401
		kGPIOIntMask		= 0x01000000,						// GPIO (0x401)
		kPCMCIA1IntMask		= 0x02000000,						// 0x401
		kTabletIntMask		= 0x10000000,						// Tablet
		kPlatformIntMask	= 0x08000000,						// Platform events.
		kPowerOffMask		= 0x0C400000		// What is enabled when
												// power is off.
		// IC3 power off mask: 0x00408000
		// 00400000 -> reset switch (FIQ)
		// 00008000 -> BIOInterface (IRQ)
	};
	
	enum {
#ifdef WIN32
		kEpochInNewtonBase	= 2082844800 + 24*60*60*150	+ 60*60 
											///< Number of seconds between
											///< 1/1/1970 and 1/1/1904.
											///< The MSWindows time is 150 days off.
#else
		kEpochInNewtonBase	= 2082844800	///< Number of seconds between
											///< 1/1/1970 and 1/1/1904.
#endif
	};

	///
	/// Constructor from the processor.
	/// Initializes the timer to 0.
	/// The timer is suspended (resume it with ResumeTimer).
	///
	/// \param inLog				interface for logging.
	/// \param inProcessor	reference to the processor.
	///
	TInterruptManager( TLog* inLog, TARMProcessor* inProcessor );

	///
	/// Constructor from the processor and a given timer.
	/// The timer is suspended (resume it with ResumeTimer).
	///
	/// \param inLog				interface for logging.
	/// \param inProcessor	reference to the processor.
	/// \param inTimer		initial value of the timer.
	///
	TInterruptManager(
				TLog* inLog,
				TARMProcessor* inProcessor,
				KUInt32 inTimer );

	///
	/// Destructor.
	///
	~TInterruptManager( void );

	///
	/// Get the current value of the timer.
	///
	KUInt32	GetTimer( void ) const;

	///
	/// Accessor on the frozen timer (for the debugger)
	///
	/// \return the value of the timer.
	///
	KUInt32	GetFrozenTimer( void ) const
		{
			return mTimer;
		}

	///
	/// Suspend the timer (i.e. the emulator time).
	///
	void	SuspendTimer( void );

	///
	/// Resume the timer (i.e. the emulator time).
	///
	void	ResumeTimer( void );

	///
	/// Wait until next interrupt.
	/// This method allows the emulator thread to pause until an interrupt
	/// occurs, something that happens frequently with NewtonOS to save the
	/// battery.
	///
	/// \param inMaskIRQ	don't wake up if the interrupt is an IRQ.
	/// \param inMaskFIQ	don't wake up if the interrupt is a FIQ.
	///
	void	WaitUntilInterrupt( bool inMaskIRQ, bool inMaskFIQ );

	///
	/// Wake the emulator, if it's waiting in the loop.
	///
	void	WakeEmulatorThread( void )
		{
			mEmulatorCondVar->Signal();
		}

	///
	/// Accessor on the match registers.
	/// The value is what the OS stored.
	///
	/// \param inMatchReg	index of the match register to get.
	/// \return the value of the match register.
	///
	KUInt32	GetTimerMatchRegister( KUInt32 inMatchReg ) const
		{
			return mMatchRegisters[inMatchReg];
		}

	///
	/// Selector on the match registers.
	///
	/// \param inMatchReg	index of the match register to change.
	/// \param inValue		value to set.
	///
	void	SetTimerMatchRegister( KUInt32 inMatchReg, KUInt32 inValue );

	///
	/// Accessor on the alarm register.
	///
	/// \return the value of the alarm register.
	///
	KUInt32	GetAlarm( void ) const
		{
			return mAlarmRegister;
		}

	///
	/// Selector on the alarm register.
	///
	/// \param inValue		new value for the alarm register.
	///
	void	SetAlarm( KUInt32 inValue );

	///
	/// Accessor on the real time clock (calendar register).
	///
	/// \return the value of the calendar register.
	///
	KUInt32	GetRealTimeClock( void ) const;

	///
	/// Selector on the real time clock.
	///
	/// \param inValue		new value for the calendar register.
	///
	void	SetRealTimeClock( KUInt32 inValue );

	///
	/// Raise an interrupt.
	///
	/// \param inIntMask	mask of the interrupt to raise.
	///
	void	RaiseInterrupt( KUInt32 inIntMask );

	///
	/// Accessor on the interrupts that were raised.
	///
	/// \return a mask of the interrupts that were raised.
	///
	KUInt32	GetIntRaised( void ) const
		{
			return mIntRaised;
		}

	///
	/// Accessor on the interrupt control register.
	///
	/// \return the interrupt control register.
	///
	KUInt32	GetIntCtrlReg( void ) const
		{
			return mIntCtrlReg;
		}

	///
	/// Selector on the interrupt control register.
	///
	/// \param inValue	new value for the interrupt control register.
	///
	void	SetIntCtrlReg( KUInt32 inValue );

	///
	/// Accessor on the interrupt control register at 0x0F184000.
	///
	/// \return the interrupt control register at 0x0F184000.
	///
	KUInt32	GetIntEDReg1( void ) const
		{
			return mIntEDReg1;
		}

	///
	/// Selector on the interrupt control register at 0x0F184000.
	///
	/// \param inValue	new value for the interrupt control register.
	///
	void	SetIntEDReg1( KUInt32 inValue );

	///
	/// Accessor on the interrupt control register at 0x0F184400.
	///
	/// \return the interrupt control register at 0x0F184400.
	///
	KUInt32	GetIntEDReg2( void ) const
		{
			return mIntEDReg2;
		}

	///
	/// Selector on the interrupt control register at 0x0F184400.
	///
	/// \param inValue	new value for the interrupt control register.
	///
	void	SetIntEDReg2( KUInt32 inValue );

	///
	/// Accessor on the interrupt control register at 0x0F184800.
	///
	/// \return the interrupt control register at 0x0F184800.
	///
	KUInt32	GetIntEDReg3( void ) const
		{
			return mIntEDReg3;
		}

	///
	/// Selector on the interrupt control register at 0x0F184800.
	///
	/// \param inValue	new value for the interrupt control register.
	///
	void	SetIntEDReg3( KUInt32 inValue );

	///
	/// Accessor on the FIQ mask.
	///
	/// \return a mask with the interrupts that should be FIQ.
	///
	KUInt32	GetFIQMask( void ) const
		{
			return mFIQMask;
		}

	///
	/// Selector on the FIQ mask.
	///
	/// \param inMask	new mask with the interrupts that should be FIQ.
	///
	void	SetFIQMask( KUInt32 inValue )
		{
			mFIQMask = inValue;
		}

	///
	/// Clear interrupts.
	///
	/// \param inMask		mask of the interrupts to clear.
	///
	void	ClearInterrupts( KUInt32 inMask );

	///
	/// Raise a GPIO interrupt.
	///
	/// \param inIntMask	mask of the interrupt to raise.
	///
	void	RaiseGPIO( KUInt32 inIntMask );

	///
	/// Accessor on the interrupts that were raised.
	///
	/// \return a mask of the interrupts that were raised.
	///
	KUInt32	GetGPIORaised( void ) const
		{
			return mGPIORaised;
		}

	///
	/// Accessor on the GPIO control register.
	///
	/// \return the GPIO control register.
	///
	KUInt32	GetGPIOCtrlReg( void ) const
		{
			return mGPIOCtrlReg;
		}

	///
	/// Selector on the GPIO control register.
	///
	/// \param inValue	new value for the GPIO control register.
	///
	void	SetGPIOCtrlReg( KUInt32 inValue );

	///
	/// Clear a GPIO interrupt.
	///
	/// \param inIntMask	mask of the interrupt to clear.
	///
	void	ClearGPIO( KUInt32 inIntMask );

	///
	/// Save or restore the state to or from a stream.
	///
	void	TransferState( TStream* inStream );
		
	///
	/// Thread loop entry point.
	///
	void	Run( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TInterruptManager( const TInterruptManager& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TInterruptManager& operator = ( const TInterruptManager& inCopy );

	///
	/// Performs initialization (create thread & condition variable).
	///
	void	Init( void );

	///
	/// Fire and find next interrupts.
	///
	/// \return \c true if there is any next interrupt.
	///
	bool	FireTimersAndFindNext(
					KUInt32 inTicks,
					KUInt32 inNewTicks,
					KUInt32* outNextMatch );

	///
	/// Determine the next timer considering the current time.
	///
	/// \param inNow				current time.
	/// \param inTimerA				first timer.
	/// \param inTimerB				second timer.
	/// \return the first timer, A or B, coming just after now.
	///
	static inline   KUInt32 GetNextTimer(
								KUInt32 inNow,
								KUInt32 inTimerA,
								KUInt32 inTimerB );

	/// \name Platform threading primitives

	///
	/// Wait for some ticks on the condition variable.
	///
	/// \param inTicks	number of ticks to sleep.
	///
	void	TicksWaitOnCondVar( KUInt32 inTicks );

	///
	/// Accessor on the time in ticks.
	///
	/// \return the time (any base) in ticks.
	///
	static KUInt32	GetTimeInTicks( void );

	///
	/// Get the a calendar delta such that the current RTC is the current
	/// time in Newton's base.
	///
	/// \return a suitable calendar delta.
	///
	static KUInt32	GetSyncedCalendarDelta( void );

	/// \name Variables
	TLog*			mLog;				///< Interface for logging.
	TARMProcessor*	mProcessor;			///< Reference to the processor.
	volatile KUInt32	mRunning;		///< Whether the timer is running.
	volatile KUInt32	mExiting;		///< Whether the object is being
										///< destroyed.
	volatile KUInt32	mWaiting;		///< Whether some thread is waiting
										///< in WaitUntilInterrupt.
	KUInt32			mMaskIRQ;			///< Whether the processor masks IRQ.
	KUInt32			mMaskFIQ;			///< Whether the processor masks FIQ.
	KUInt32			mIntRaised;			///< Interrupts that were raised.
	KUInt32			mIntCtrlReg;		///< Interrupts that are enabled.
	KUInt32			mFIQMask;			///< Mask for FIQ
	KUInt32			mIntEDReg1;			///< Int. register at 0x0F184000
	KUInt32			mIntEDReg2;			///< Int. register at 0x0F184400
	KUInt32			mIntEDReg3;			///< Int. register at 0x0F184800
	KUInt32			mGPIORaised;		///< GPIO ints that were raised.
	KUInt32			mGPIOCtrlReg;		///< GPIO control register (0x0F18C800)
	KSInt32			mCalendarDelta;		///< Delta with the RTC (seconds),
										///< newton = host - delta.
	KUInt32			mAlarmRegister;		///< Alarm match register (seconds).
	KUInt32			mTimerDelta;		///< Delta with the timer (ticks)
										///< newton = host - delta.
	KUInt32			mTimer;				///< Saved value of the timer (host based).
	KUInt32			mMatchRegisters[4]; ///< Timer match registers (newton).
	TCondVar*		mTimerCondVar;		///< Condition variable (timer thread).
	TCondVar*		mEmulatorCondVar;	///< Condition variable (emulator).
	TMutex*			mMutex;				///< Mutex of the thread.
	TThread*		mThread;			///< The actual thread.
};

#endif
		// _TINTERRUPTMANAGER_H

// ============================================================================= //
// Welcome to UNIX!  Enjoy your session!  Have a great time!  Note the           //
// use of exclamation points!  They are a very effective method for              //
// demonstrating excitement, and can also spice up an otherwise plain-looking    //
// sentence!  However, there are drawbacks!  Too much unnecessary exclaiming     //
// can lead to a reduction in the effect that an exclamation point has on        //
// the reader!  For example, the sentence                                        //
//                                                                               //
//         Jane went to the store to buy bread                                   //
//                                                                               //
// should only be ended with an exclamation point if there is something          //
// sensational about her going to the store, for example, if Jane is a           //
// cocker spaniel or if Jane is on a diet that doesn't allow bread or if         //
// Jane doesn't exist for some reason!  See how easy it is?!  Proper control     //
// of exclamation points can add new meaning to your life!  Call now to receive  //
// my free pamphlet, "The Wonder and Mystery of the Exclamation Point!"!         //
// Enclose fifteen(!) dollars for postage and handling!  Operators are           //
// standing by!  (Which is pretty amazing, because they're all cocker spaniels!) //
// ============================================================================= //
