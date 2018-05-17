// ==============================
// File:			TEmulator.h
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

#ifndef _TEMULATOR_H
#define _TEMULATOR_H

#include <K/Defines/KDefinitions.h>

// Einstein
#include "TARMProcessor.h"
#include "TMemory.h"
#include "Log/TLog.h"

class TSerialPortManager;
class TInterruptManager;
class TDMAManager;
class TPlatformManager;
class TROMImage;
class TLog;
class TNetworkManager;
class TSoundManager;
class TScreenManager;
class TMonitor;
class TStream;
class TFileManager;

///
/// Class for the main loop of the emulator.
/// Also handles the interrupt thread.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TEmulator
{
  friend class TMonitor;

public:
	///
	/// Constructor from a ROM image and a RAM size.
	///
	/// Constructor from the ROM Image and the amount of RAM to use
	/// in the emulator.
	///
	/// \param inLog				interface for logging.
	/// \param inROMImage			ROM Image.
	/// \param inFlashPath			path to the flash file.
	/// \param inSoundManager		sound manager.
	/// \param inScreenManager		screen manager.
	/// \param inRAMSize			size of the RAM installed (in bytes)
	/// \param inExtrSerialPortManager	serial port manager
	///
	TEmulator(
			TLog* inLog,
			TROMImage* inROMImage,
			const char* inFlashPath,
			TSoundManager* inSoundManager,
			TScreenManager* inScreenManager,
			TNetworkManager* inNetworkManager,
			KUInt32 inRAMSize = 0x00400000,
			TSerialPortManager* inExtrSerialPortManager = 0L);

	///
	/// Constructor from a rom image buffer.
	/// This is typically used in tests.
	///
	/// \param inLog				interface for logging.
	/// \param inROMImageBuffer		pointer to ROM data.
	/// \param inFlashPath			path to the flash file.
	/// \param inRAMSize			size of the RAM installed (in bytes)
	///
	TEmulator(
			  TLog* inLog,
			  KUInt8* inROMImageBuffer,
			  const char* inFlashPath,
			  KUInt32 inRAMSize = 0x00400000);

	///
	/// Destructor.
	///
	~TEmulator( void );
	
	///
	/// Run the emulator until it is interrupted.
	///
	void	Run( void );

	///
	/// Perform a single step.
	/// This is useful for debugging. Timers are suspended.
	///
	void	Step( void );

	///
	/// Signal an interrupt.
	///
	inline void	SignalInterrupt( void )
		{
			mSignal = false;
			mInterrupted = true;
		}
	
	///
	/// Called after the interrupt was processed.
	///
	inline void	AckInterrupt( void )
		{
			if (mRunning && (!mPaused))
			{
				mSignal = true;
			}
			mInterrupted = false;
		}

	///
	/// Stop the emulator.
	///
	void	Stop( void );

	///
	/// Breakpoint.
	/// Method called by the processor when it encounters a BKPT instruction.
	///
	inline void	Breakpoint( KUInt16 inID )
		{
			mSignal = false;
			mRunning = false;
			mBPHalted = true;
			mBPID = inID;
			// We can suppose the emulator is running now.
			// mInterruptManager.WakeEmulatorThread();
		}
	
	///
	/// NewtonOS Debugger UND instruction.
	/// Method called by the processor when it encounters a SystemBootUND
	/// instruction.
	///
	void		SystemBootUND( KUInt32 inPAddr );
	
	///
	/// NewtonOS Debugger UND instruction.
	/// Method called by the processor when it encounters a DebuggerUND
	/// instruction.
	///
	void		DebuggerUND( KUInt32 inPAddr );
		
	///
	/// NewtonOS Debugger UND instruction.
	/// Method called by the processor when it encounters a TapFileCntlUND
	/// instruction.
	///
	void		TapFileCntlUND( KUInt32 inPAddr );
	
	///
	/// Selector on the monitor.
	///
	/// \param inMonitor	reference to the monitor.
	///
	void		SetMonitor( TMonitor* inMonitor )
		{
			mMonitor = inMonitor;
		}

	///
	/// Selector on the file manager interface.
	///
	/// \param inMonitor	reference to the manager.
	///
	void		SetFileManager( TFileManager* inManager )
		{
			mFileManager = inManager;
		}

	///
	/// Break in monitor, if present (don't do anything otherwise).
	///
	void		BreakInMonitor( void );
	
	///
	/// Determine if we're stopped in a breakpoint.
	///
	/// \return \c true if we are.
	///
	inline Boolean	IsBPHalted( void )
		{
			return mBPHalted;
		}
	
	///
	/// Determine if we're processing an interrupt.
	///
	/// \return \c true if we are.
	///
	inline Boolean	IsInterrupted( void )
		{
			return mInterrupted;
		}

	///
	/// Return the ID of the Breakpoint.
	///
	/// \return the ID.
	///
	inline KUInt16	GetBPID( void )
		{
			return mBPID;
		}
	
	///
	/// Determine if we're paused.
	///
	/// \return \c true if we are.
	///
	inline Boolean	IsPaused( void )
		{
			return mPaused;
		}
	
	///
	/// Determine if we're running.
	///
	/// \return \c true if we are.
	///
	inline Boolean	IsRunning( void )
		{
			return mRunning;
		}

	///
	/// Accessor on the CPU.
	///
	/// \return a pointer on the CPU interface.
	///
	TARMProcessor*  GetProcessor( void )
		{
			return &mProcessor;
		}
	
	///
	/// Accessor on the memory interface.
	///
	/// \return a pointer on the memory interface.
	///
	TMemory*  GetMemory( void )
		{
			return &mMemory;
		}
	
	///
	/// Accessor on the interrupt manager interface.
	///
	TInterruptManager*  GetInterruptManager( void )
		{
			return mInterruptManager;
		}
	
	///
	/// Accessor on the sound manager interface.
	///
	/// \return a pointer to the sound manager.
	///
	TSoundManager*  GetSoundManager( void )
	{
		return mSoundManager;
	}
	
	///
	/// Accessor on the network manager interface.
	///
	/// \return a pointer to the network manager.
	///
	TNetworkManager*  GetNetworkManager( void )
	{
		return mNetworkManager;
	}
	
	///
	/// Accessor on the sound manager interface.
	///
	/// \return a pointer to the screen manager.
	///
	TScreenManager*  GetScreenManager( void )
		{
			return mScreenManager;
		}

	///
	/// Accessor on the file manager interface.
	///
	/// \return a pointer to the file manager.
	///
	TFileManager*  GetFileManager( void )
		{
			return mFileManager;
		}
	
	///
	/// Accessor on the sound manager interface.
	///
	/// \return a pointer to the screen manager.
	///
	TPlatformManager*  GetPlatformManager( void )
		{
			return mPlatformManager;
		}
	
	///
	/// Accessor on the DMA manager interface.
	///
	/// \return a pointer to the DMA manager.
	///
	TDMAManager*  GetDMAManager( void )
		{
			return mDMAManager;
		}
	
	///
	/// Accessor on the external serial port.
	///
	/// \return a pointer to the serial port.
	///
	TSerialPortManager*	GetExternalSerialPort( void )
		{
			return mExternalPort;
		}

	///
	/// Accessor on the infrared serial port.
	///
	/// \return a pointer to the serial port.
	///
	TSerialPortManager*	GetInfraredSerialPort( void )
		{
			return mInfraredPort;
		}

	///
	/// Accessor on the built-in extra serial port.
	///
	/// \return a pointer to the serial port.
	///
	TSerialPortManager*	GetBuiltInExtraSerialPort( void )
		{
			return mBuiltInExtraPort;
		}

	///
	/// Accessor on the modem serial port.
	///
	/// \return a pointer to the serial port.
	///
	TSerialPortManager*	GetModemSerialPort( void )
		{
			return mModemPort;
		}

	///
	/// Accessor to the NewtonID.
	///
	/// \return a pointer to the NewtonID.
	///
	const KUInt32*		GetNewtonID( void )
		{
			return mNewtonID;
		}

	///
	/// Pause system and wait for next interrupt.
	///
	inline void	PauseSystem( void )
		{
			mSignal = false;
			mPaused = true;
		}
	
	///
	/// Quit.
	///
	void		Quit( void );

	///
	/// Save the state to a file.
	///
	/// \return an error code if a problem occurred.
	///
	void		SaveState( const char* inPath );
	
	///
	/// Load the state from a file.
	///
	/// \return an error code if a problem occurred.
	///
	void		LoadState( const char* inPath );

	///
	/// Save or restore the state to or from a file.
	///
	void		TransferState( TStream* inStream );
	
	///
	/// Insert a card.
	///
	void		InsertCard( void );
	
	///
	/// Set a new NewtonID
	///
	void		SetNewtonID(KUInt32 inID0, KUInt32 inID1);
	
private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TEmulator( const TEmulator& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TEmulator& operator = ( const TEmulator& inCopy );

	/// \name Variables
	TMemory				mMemory;			///< Memory.
	TARMProcessor		mProcessor;			///< CPU.
	TInterruptManager*	mInterruptManager;	///< Interrupt manager.
	TDMAManager*		mDMAManager;		///< DMA manager.
	TPlatformManager*	mPlatformManager;	///< Platform manager.
	TSerialPortManager*	mExternalPort;		///< External serial port.
	TSerialPortManager*	mInfraredPort;		///< Infrared serial port.
	TSerialPortManager*	mBuiltInExtraPort;	///< Built-in Extra serial port.
	TSerialPortManager*	mModemPort;			///< Modem serial port.
	TNetworkManager*	mNetworkManager;	///< Network manager.
	TSoundManager*		mSoundManager;		///< Sound manager.
	TScreenManager*		mScreenManager;		///< Screen manager.
	TFileManager*       mFileManager;
	KUInt32				mNewtonID[2];		///< NewtonID (48 bits, 16+32).
	TLog*				mLog;				///< Interface for logging.
	TMonitor*			mMonitor;			///< Monitor (or \c nil).
	bool				mSignal;			///< Signal for JIT (if we're running).
	KUInt32				mInterrupted;		///< We got a (processor) interrupt.
	KUInt32				mRunning;			///< If we're running.
	KUInt32				mPaused;			///< If we're paused (until next interrupt).
	KUInt32				mBPHalted;			///< If we're halted because of a breakpoint.
	KUInt16				mBPID;				///< ID of the breakpoint.
};

#endif
		// _TEMULATOR_H

// ============================================================================== //
// Hacker's Guide To Cooking:                                                     //
// 2 pkg. cream cheese (the mushy white stuff in silver wrappings that doesn't    //
//         really  come from Philadelphia after all; anyway, about 16 oz.)        //
// 1 tsp. vanilla  extract  (which is more alcohol than vanilla and pretty        //
//         strong so this part you *GOTTA* measure)                               //
// 1/4 cup sugar (but honey works fine too)                                       //
// 8 oz. Cool Whip (the fluffy stuff devoid of nutritional value that you         //
//         can squirt all over your friends and lick off...)                      //
// "Blend all together until creamy with no lumps."  This is where you get to     //
//         join(1) all the raw data in a big buffer and then filter it through    //
//         merge(1m) with the -thick option, I mean, it starts out ultra lumpy    //
//         and icky looking and you have to work hard to mix it.  Try an electric //
//         beater if you have a cat(1) that can climb wall(1s) to lick it off     //
//         the ceiling(3m).                                                       //
// "Pour into a graham cracker crust..."  Aha, the BUGS section at last.  You     //
//         just happened  to have a GCC sitting around under /etc/food, right?    //
//         If not, don't panic(8), merely crumble a rand(3m) handful of innocent  //
//         GCs into a suitable tempfile and mix in some melted butter.            //
// "...and  refrigerate for an hour."  Leave the  recipe's  stdout in a fridge    //
//         for 3.6E6 milliseconds while you work on cleaning up stderr, and       //
//         by time out your cheesecake will be ready for stdin.                   //
// ============================================================================== //
