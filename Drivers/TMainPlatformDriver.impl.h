// ==============================
// File:			TMainPlatformDriver.impl.h
// Project:			Einstein
//
// Copyright 2003-2013 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TMAINPLATFORMDRIVER_IMPL_H
#define _TMAINPLATFORMDRIVER_IMPL_H

#include "TPlatformDriver.h"
#include "Emulator/Host/UserInfoDefinitions.h"
#include "Emulator/Platform/PlatformEvents.h"

#include <AEvents.h>

struct GPIOIntObject;
struct SEvent;
class TUAsyncMessage;
struct SGestaltEinsteinEmulatorInfo;
class TUTask;
class TUPort;
class TNativeCalls;

///
/// Class for our platform driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
PROTOCOL TMainPlatformDriver : public TPlatformDriver
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(TPlatformDriver);

	CAPABILITIES( ("copyright" "Paul Guyot") ("version" "1.0") )

	TPlatformDriver*	New( void );
	void				Delete( void );
	void				Init( void );
	ULong				BacklightTrigger( void );
	NewtonErr			RegisterPowerSwitchInterrupt( void );
	NewtonErr			EnableSysPowerInterrupt( void );
	NewtonErr			InterruptHandler( void );
	ULong				TimerInterruptHandler( void );
	ULong				ResetZAPStoreCheck( void );
	ULong				PowerOnSubsystem( ULong inArg1 );
	ULong				PowerOffSubsystem( ULong inArg1 );
	ULong				PowerOffAllSubsystems( void );
	ULong				PauseSystem( void );
	ULong				PowerOffSystem( void );
	ULong				PowerOnSystem( void );
	ULong				TranslatePowerEvent( ULong inArg1 );
	NewtonErr			GetPCMCIAPowerSpec( ULong inArg1, ULong* outArg2 );
	ULong				PowerOnDeviceCheck( unsigned char inArg1 );
	ULong				SetSubsystemPower( ULong, ULong );
	ULong				GetSubsystemPower( ULong, ULong* );

	///
	/// Stop in Einstein's monitor.
	/// Effectless if the monitor isn't running.
	///
	void				BreakInMonitor( void );

	void				LockEventQueue( void );
	void				UnlockEventQueue( void );

	///
	/// Log some string.
	///
	void				Log( const char* inString );

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
	ULong				GetUserInfo(
								ULong inSelector,
								ULong inBufferSize,
								void* outData );

	///
	/// Get the host time zone.
	///
	/// \return the host time zone.
	///
	int					GetHostTimeZone( void );

	///
	/// Calibrate the tablet.
	///
	void				CalibrateTablet( void );

	///
	/// Quit Einstein.
	///
	void				QuitEinstein( void );

	///
	/// Accessor to the platform driver.
	///
	static	TMainPlatformDriver*
						GetPlatformDriver( void );

	///
	/// Accessor to the native calls driver.
	///
	TNativeCalls*		GetNativeCalls( void );

	///
	/// Dipose a buffer.
	///
	/// \param inBufferID	ID of the buffer to dispose.
	/// \return \c true if the buffer was found and disposed.
	///
	Boolean					DisposeBuffer( ULong inBufferID );

	///
	/// Copy data from a buffer.
	///
	/// \param inBufferID	ID of the buffer to retrieve.
	/// \return \c true if the buffer was found and data was copied.
	///
	Boolean					CopyBufferData(
									ULong inBufferID,
									void* outBuffer,
									ULong inOffset,
									ULong inAmount );

private:
	///
	/// Interrupt handler.
	///
	static long				SInterruptHandler( void* inUserData );

	///
	/// From a port name, return a port id.
	///
	ULong					GetPortIdFromName( ULong inPortName );

	///
	/// Native function to get the next event.
	///
	/// \param outEvent			event obtained from the native interface.
	/// \return \c true if there was an event returned.
	///
	Boolean					GetNextEvent( SEvent* outEvent );

	///
	/// Native function to fill the gestalt record.
	///
	/// \param outInfo			info filled by the emulator.
	///
	void					FillGestaltEmulatorInfo( SGestaltEinsteinEmulatorInfo* outInfo );

	///
	/// Task proc.
	///
	static void				STaskProc(
								void* inObject,
								ULong inObjectSize,
								TObjectId inTaskID );

	///
	/// Task proc.
	///
	void					TaskProc( void );

	/// \name variables
	GPIOIntObject*			mInterrupt;		///< Result of RegisterInterrupt
	SEvent*					mEvent;			///< Event (allocated in a wired
											///< buffer).
	TUAsyncMessage*			mAsyncMessage;	///< Async message.
	TUPort*					mPort;			///< Port (to receive replies).
	TUTask*					mTask;			///< Task (to receive replies).
	ULong					mPowerPortId;	///< Power port Id (0 if unknown yet)
	ULong					mNewtPortId;	///< Newt port Id (0 if unknown yet)
	TNativeCalls*			mNativeCalls;	///< Interface for native calls.
};

#endif
		// _TMAINPLATFORMDRIVER_IMPL_H

// =============================================================== //
// Per buck you get more computing action with the small computer. //
//                 -- R.W. Hamming                                 //
// =============================================================== //
