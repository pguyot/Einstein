// ==============================
// File:			TPortAudioSoundManager.h
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

#ifndef _TPORTAUDIOSOUNDMANAGER_H
#define _TPORTAUDIOSOUNDMANAGER_H

#include <K/Defines/KDefinitions.h>

// PortAudio
#include <portaudio.h>
// #include <pa_linux_alsa.h>

// Einstein
#include "TBufferedSoundManager.h"

// #define DEBUG_SOUND	0

class TCircleBuffer;
class TMutex;

///
/// Class to handle sound input/output with portaudio.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test d�fini.
///
class TPortAudioSoundManager
	:
		public TBufferedSoundManager
{
public:
	///
	/// Constructor from a log.
	///
	/// \param inLog				log interface (can be null)
	///
	TPortAudioSoundManager( TLog* inLog = nil );

	///
	/// Destructor.
	///
	virtual ~TPortAudioSoundManager( void );

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
	/// Is output running?
	///
	virtual Boolean	OutputIsRunning( void );

private:
	///
	/// Callback routine (static).
	///
	static int SPACallBack(
				const void* inputBuffer,
				void* outputBuffer,
				unsigned long frameCount,
				const PaStreamCallbackTimeInfo* timeInfo,
				PaStreamCallbackFlags statusFlags,
				void* userData )
		{
			return ((TPortAudioSoundManager*) userData)->PACallBack(
											inputBuffer,
											outputBuffer,
											frameCount,
											timeInfo,
											statusFlags );
		};

	///
	/// Callback routine.
	///
	int PACallBack(
				const void* inputBuffer,
				void* outputBuffer,
				unsigned long frameCount,
				const PaStreamCallbackTimeInfo* timeInfo,
				PaStreamCallbackFlags statusFlags );

	/// \name Variables
	PaStream*			mOutputStream;		///< Output stream.
	TCircleBuffer*		mOutputBuffer;		///< Output buffer.
	TMutex*				mDataMutex;			///< Mutex on shared structures.
	KSInt16*			mMultiplexedBuffer;	///< Buffer to resample.
	Boolean				mOutputIsRunning;	///< If output is running.
};

#endif
		// _TPORTAUDIOSOUNDMANAGER_H

// ========================================= //
// TRANSACTION CANCELLED - FARECARD RETURNED //
// ========================================= //
