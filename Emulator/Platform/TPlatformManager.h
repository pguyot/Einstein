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
#if TARGET_OS_WIN32
	#include "PlatformEventsLF.h"
	#include "Emulator/Host/UserInfoDefinitionsLF.h"
#else
	#include "PlatformEvents.h"
	#include "Emulator/Host/UserInfoDefinitions.h"
#endif

class TInterruptManager;
class TScreenManager;
class TMemory;
class TLog;
class TMutex;

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
	/// Power off the system.
	///
	void	PowerOff( void );

	///
	/// Power on the system.
	///
	void	PowerOn( void );

	///
	/// Lock the event queue. New events won't be sent to the Newton.
	///
	void	LockEventQueue( void );

	///
	/// Unlock the event queue.
	/// Send first pending event.
	///
	void	UnlockEventQueue( void );

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
	void	InstallPackage( const char* inPackagePath );

	///
	/// Get next event from the queue (and remove it).
	/// Address is wired (and physical).
	///
	Boolean	GetNextEvent( KUInt32 outEventPAddr );

	///
	/// Dispose a buffer from the queue.
	/// \return true if the buffer was found and removed.
	///
	Boolean	DisposeBuffer( KUInt32 inID );

	///
	/// Copy some buffer data (and remove the buffer).
	/// \return true if the write operation succeeded.
	///
	Boolean	CopyBufferData( KUInt32 inID, KUInt32 outVAddress, KUInt32 inOffset, KUInt32 inAmount );

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
	TLog*				mLog;				///< Reference to the log.
	TScreenManager*		mScreenManager;		///< Reference to the screen manager.
	TInterruptManager*	mInterruptManager;	///< Reference to the interrupt mgr.
	TMemory*			mMemory;			///< Reference to the memory interface.
	SEvent*				mEventQueue;		///< Liste des événements.
	KUInt32				mEventQueueCCrsr;	///< Consumer queue cursor (Newton)
	KUInt32				mEventQueuePCrsr;	///< Producer queue cursor (Host)
	KUInt32				mEventQueueSize;	///< Size of the event queue.
	SBuffer*			mBufferQueue;		///< List of buffers.
	KUInt32				mBufferCount;		///< Number of buffers.
	KUInt32				mBufferQueueSize;	///< Size of the buffer queue.
	KUInt32				mBufferNextID;		///< Next ID for buffers.
	Boolean				mPowerOn;			///< If power is on.
	KUInt32				mQueueLockCount;	///< Lock count for the queue.
	TMutex*				mMutex;				///< Mutex of the queue.
};

#endif
		// _TPLATFORMMANAGER_H

// ======================================================================= //
// FORTRAN is not a flower but a weed -- it is hardy, occasionally blooms, //
// and grows in every computer.                                            //
//                 -- A.J. Perlis                                          //
// ======================================================================= //
