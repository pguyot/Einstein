// ==============================
// File:			TPlatformManager.h
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

#ifndef _TPLATFORMMANAGER_H
#define _TPLATFORMMANAGER_H

#include <K/Defines/KDefinitions.h>

// Einstein
#include "PlatformEvents.h"
#ifndef IGNORE_TNEWT
#include "TNewt.h"
#endif
#include "Emulator/Host/UserInfoDefinitions.h"

class TInterruptManager;
class TARMProcessor;
class TScreenManager;
class TMemory;
class TLog;
class TMutex;
class TPCMCIACard;


///
/// Class for the native-side of the platform driver.
/// This class handles events.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TPlatformManager
{
public:
	///
	/// Constructor from a log.
	///
	/// \param inLog				log interface (can be null)
	/// \param inScreenManager		screen interface (can be null)
	///
	TPlatformManager( TLog* inLog = nil, TScreenManager* inScreenManager = nil );

	///
	/// Destructor.
	///
	virtual ~TPlatformManager( void );

	///
	/// The version number of the Newt Platform interface
	///
	KUInt32 GetVersion();

	///
	/// Set the reference to the emulator.
	///
	void	SetEmulator( TEmulator *inEmulator )
	{
		mEmulator = inEmulator;
	}

	///
	/// Set the interrupt manager.
	/// This method is called once the interrupt manager is created.
	///
	/// \param inManager	reference to the interrupt manager
	///
	void	SetInterruptManager( TInterruptManager* inManager )
		{
			mInterruptManager = inManager;
		}

	///
	/// Set the memory interface.
	/// This method is called once the memory interface is created.
	///
	/// \param inManager	reference to the memory interface
	///
	void	SetMemory( TMemory* inMemory )
		{
			mMemory = inMemory;
		}

	///
	/// Set the SPU
	///
	void	SetProcessor( TARMProcessor* inCPU )
	{
		mCPU = inCPU;
	}

	///
	/// Power off the system.
	///
	void	PowerOff( void );

	///
	/// Power on the system.
	///
	void	PowerOn( void );

	///
	/// Is the system powered on?
	///
	bool IsPowerOn( void ) { return mPowerOn; }
	
	/**
	 * Keep Einstein from triggering new Platform Interrupts.
	 * 
	 * This method is call from NewtonOS. A recursive lock is set
	 * that will keep Einstein from triggering new interrupts
	 * until UnlockEventQueue() is called.
	 * 
	 * These kind of Platform Interrupts send keyboard events, run
	 * NewtonScript snippets, and install packages.
	 * 
	 * \see UnlockEventQueue()
	 * \see TNativePrimitives::ExecutePlatformDriverNative()
	 * \see TPlatformManager::NewtonScriptCall()
	 */
	void LockEventQueue();

	/**
	 * Allow Einstein to send Platform Interrupts again.
	 * 
	 * This method is called from NewtonOS after a Platform Event was 
	 * handled and NewtonOS is ready for new events. If any events are 
	 * pending, the next event in the list is sent by triggering
	 * a Platform Interrupt.
	 *
	 * \see LockEventQueue()
	 */
	void UnlockEventQueue();

	///
	/// Get some information about the user.
	/// Return the number of bytes written.
	/// If outData is null, don't write anything but return the full size.
	///
	/// \param inSelector	selector for the user info to retrieve.
	/// \param inBufferSize	size of the buffer.
	/// \param outData		on output, user info.
	/// \return the number of bytes written.
	///
	KUInt32	GetUserInfo(
					EUserInfoSel inSelector,
					KUInt32 inBufferSize,
					KUInt32 outAddress ) const;

	///
	/// Get the host time zone.
	///
	/// \return the host time zone.
	///
	int					GetHostTimeZone( void ) const;

	///
	/// Send an aevt event to the Newton.
	/// Data is copied.
	///
	void	SendAEvent( EPort inPortId, KUInt32 inSize, const KUInt8* inData );

	///
	/// Insert or replace a PCCard in a given slot, or remove a PCCard
	/// 
	int	InsertPCCard(KUInt32 inSLot, TPCMCIACard* inCard);

	///
	/// Return the card that is currently in the given PCMCIA slot.
	/// 
	TPCMCIACard* GetPCCard(KUInt32 inSlot);

	///
	/// Send a power switch event.
	///
	void	SendPowerSwitchEvent( void );
	
	///
	/// Send a backlight event.
	///
	void	SendBacklightEvent( void );

	///
	/// Send a keyboard event.
	///
	void	SendKeyEvent( KUInt32 inType, KUInt8 inKeyCode, KUInt32 time = 0 );

	///
	/// Send some NewtonScript command for evaluation.
	///
	void	EvalNewtonScript( const char* inNewtonScriptCode );

    ///
    /// Install a package (the package will be added to a queue, sliced, read by
    /// the newtonscript task).
    ///
    void    InstallPackage(const KUInt8* inPackageData, KUInt32 inPackageSize);

    ///
    /// Install a package (the package will be added to a queue, sliced, read by
    /// the newtonscript task).
    ///
    void    InstallPackage( const char* inPackagePath );

	///
	/// Install all packages that were added to the directory on the host computer
	/// since the last call to this function.
	///
	void InstallNewPackages( const char *hostPackageDirectory=0L );
	
	///
	/// Get next event from the queue (and remove it).
	/// Address is wired (and physical).
	///
	bool	GetNextEvent( KUInt32 outEventPAddr );

	///
	/// Dispose a buffer from the queue.
	/// \return true if the buffer was found and removed.
	///
	bool	DisposeBuffer( KUInt32 inID );

	///
	/// Copy some buffer data (and remove the buffer).
	/// \return true if the write operation succeeded.
	///
	bool	CopyBufferData( KUInt32 inID, KUInt32 outVAddress, KUInt32 inOffset, KUInt32 inAmount );

	///
	/// Display a choice of options on the host platform. This is for devices
	/// that use a full-screen mode and can't display a host menu, but instead use 
	/// a Newton OS call to display a menu.
	///
	void OpenEinsteinMenu();
	
	///
	/// Directory for all kinds of documents, in particular, a list of pakages
	/// to be installed with an iOS/iPhone host.
	///
	void SetDocDir(const char *inDocDir);
	
	///
	/// Send a buffer event.
	/// Data is copied.
	///
	void	SendBufferAEvent(
				EPort inPortId,
				KUInt32 inClass,
				KUInt32 inID,
				KUInt32 inDataClass,
				KUInt32 inSize,
				const KUInt8* inData );

	///
	/// Allow NewtonScript to call methods within Einstein.
	///
#ifndef IGNORE_TNEWT
	NewtRef NewtonScriptCall(TNewt::RefArg rcvr, TNewt::RefArg arg0, TNewt::RefArg arg1);
#endif

private:
	struct SBuffer {
		KUInt32			fID;
		const KUInt8*	fData;
		KUInt32			fSize;
	};
	
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TPlatformManager( const TPlatformManager& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TPlatformManager& operator = ( const TPlatformManager& inCopy );

	///
	/// Add a buffer in the queue.
	/// \return the buffer ID.
	///
	KUInt32	AddBuffer( KUInt32 inSize, const KUInt8* inData );

	///
	/// Raise platform interrupt to trigger platform driver.
	///
	void	RaisePlatformInterrupt( void );

	/// \name Variables
	TLog*				mLog = nullptr;				///< Reference to the log.
	TEmulator*			mEmulator = nullptr;		///< Reference to the emulator
	TScreenManager*		mScreenManager = nullptr;	///< Reference to the screen manager.
	TInterruptManager*	mInterruptManager = nullptr;///< Reference to the interrupt mgr.
	TMemory*			mMemory = nullptr;			///< Reference to the memory interface.
	TARMProcessor*		mCPU = nullptr;				///< Reference to the processor
	SEvent*				mEventQueue = nullptr;		///< Liste des événements.
	KUInt32				mEventQueueCCrsr = 0;	    ///< Consumer queue cursor (Newton)
	KUInt32				mEventQueuePCrsr = 0;	    ///< Producer queue cursor (Host)
	KUInt32				mEventQueueSize = kDEFAULTEVENTQUEUESIZE; ///< Size of the event queue.
	SBuffer*			mBufferQueue = nullptr;		///< List of buffers.
	KUInt32				mBufferCount = 0;		    ///< Number of buffers.
	KUInt32				mBufferQueueSize = kDEFAULTBUFFERQUEUESIZE;	///< Size of the buffer queue.
	KUInt32				mBufferNextID = 0;		    ///< Next ID for buffers.
	bool				mPowerOn = true;			///< If power is on.
	bool				mQueuePreLock = false;		///< Non-recursive lock to keep interrupts from triggering twice 
	KUInt32				mQueueLockCount = 0;	    ///< Lock count for the queue.
	TMutex*				mMutex = nullptr;			///< Mutex of the queue.
	char*				mDocDir = nullptr;			///< Directory on host containing all kinds of documents
};

#endif
		// _TPLATFORMMANAGER_H

// ======================================================================= //
// FORTRAN is not a flower but a weed -- it is hardy, occasionally blooms, //
// and grows in every computer.                                            //
//                 -- A.J. Perlis                                          //
// ======================================================================= //
