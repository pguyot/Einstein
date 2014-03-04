// ==============================
// File:			TAndroidSoundManager.h
// Project:			Einstein
//
// Copyright 2003-2013 by Paul Guyot (pguyot@kallisys.net) and Matthias Melcher.
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

#ifndef _TANDROIDSOUNDMANAGER_H
#define _TANDROIDSOUNDMANAGER_H

#include <K/Defines/KDefinitions.h>
#include "TBufferedSoundManager.h"

class TMutex;
class TCircleBuffer;

///
/// Class to handle sound input/output and redirect them to Java/Android8.
///
/// \author Matthias Melcher
/// \version $Revision: 111 $
///
/// Derived from TNullSoundManager by Paul Guyot.
///
class TAndroidSoundManager
:
public TBufferedSoundManager
{
public:
	///
	/// Constructor from a log.
	///
	/// \param inLog				log interface (can be null)
	///
	TAndroidSoundManager( TLog* inLog = nil );
	
	///
	/// Destructor.
	///
	virtual ~TAndroidSoundManager( void );
	
	///
	/// Schedule output of some buffer.
	///
	virtual void	ScheduleOutput( const KUInt8* inBuffer, KUInt32 inSize );
	
	///
	/// Start output.
	///
	virtual void	StartOutput( void );
	
	///
	/// Stop output.
	///
	virtual void	StopOutput( void );
	
	///
	/// User changed the sound volume
	///
	virtual void	OutputVolumeChanged( void );

	///
	/// Is output running?
	///
	virtual Boolean	OutputIsRunning( void );
	
	static bool soundOutputDataAvailable();
	static int soundOutputBytesAvailable();
	static int soundOutputBytesCopy(signed short *, int);
	static int getGlobalVolume() { return mGlobalVolume; }
	static int soundVolumeChanged();
	
private:
	
	/// \name Variables
	static TCircleBuffer*		mOutputBuffer;	///< Output buffer.
	static TMutex*				mDataMutex;		///< Mutex on shared structures.
	static int					mGlobalVolume;
	static int					mGlobalVolumeChanged;
};

#endif
// _TANDROIDSOUNDMANAGER_H

// ========================================= //
// TRANSACTION CANCELLED - FARECARD RETURNED //
// ========================================= //
