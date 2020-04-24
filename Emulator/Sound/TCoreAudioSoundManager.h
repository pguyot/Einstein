// ==============================
// File:			TCoreAudioSoundManager.h
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

#ifndef _TCOREAUDIOSOUNDMANAGER_H
#define _TCOREAUDIOSOUNDMANAGER_H

#include <K/Defines/KDefinitions.h>
#include "TBufferedSoundManager.h"

// CoreAudio
#include <AudioUnit/AudioUnit.h>

class TMutex;
class TCircleBuffer;

///
/// Class for a CoreAudio-based sound manager.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TCoreAudioSoundManager
	:
		public TBufferedSoundManager
{
public:
	///
	/// Constructor from a log.
	///
	/// \param inLog				log interface (can be null)
	///
	TCoreAudioSoundManager( TLog* inLog = nullptr );

	///
	/// Destructor.
	///
	~TCoreAudioSoundManager() override;

	///
	/// Schedule output of some buffer.
	///
	void ScheduleOutput( const KUInt8* inBuffer, KUInt32 inSize ) override;
	
	///
	/// Start output.
	///
	void StartOutput() override;

	///
	/// Stop output.
	///
	void StopOutput() override;

	///
	/// Is output running?
	///
	bool OutputIsRunning() override;

    ///
    /// Method called to signal a change in the output volume.
    ///
    void OutputVolumeChanged() override;

private:
	///
	/// Render callback (static).
	///
	static OSStatus	SRenderCallback(
						void* inRefCon,
						AudioUnitRenderActionFlags* ioActionFlags,
						const AudioTimeStamp* inTimeStamp,
						UInt32 inBusNumber,
						UInt32 inNumberFrames,
						AudioBufferList* ioData )
		{
			return ((TCoreAudioSoundManager*) inRefCon)->RenderCallback(
							ioActionFlags,
							inTimeStamp,
							inBusNumber,
							inNumberFrames,
							ioData );
		}
		
	///
	/// Render callback.
	///
	OSStatus	RenderCallback(
						AudioUnitRenderActionFlags* ioActionFlags, 
						const AudioTimeStamp* inTimeStamp, 
						UInt32 inBusNumber, 
						UInt32 inNumberFrames, 
						AudioBufferList* ioData );

	///
	/// Create the AudioUnit.
	///
	void		CreateDefaultAU( void );

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TCoreAudioSoundManager( const TCoreAudioSoundManager& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TCoreAudioSoundManager& operator = ( const TCoreAudioSoundManager& inCopy );

	/// \name Variables
	TCircleBuffer*		mOutputBuffer;	///< Output buffer.
	TMutex*				mDataMutex;		///< Mutex on shared structures.
	AudioUnit			mOutputUnit;	///< Output unit.
};

#endif
		// _TCOREAUDIOSOUNDMANAGER_H

// ========================================================================== //
//         Something mysterious is formed, born in the silent void.  Waiting  //
// alone and unmoving, it is at once still and yet in constant motion.  It is //
// the source of all programs.  I do not know its name, so I will call it the //
// Tao of Programming.                                                        //
//         If the Tao is great, then the operating system is great.  If the   //
// operating system is great, then the compiler is great.  If the compiler is //
// greater, then the applications is great.  The user is pleased and there is //
// harmony in the world.                                                      //
//         The Tao of Programming flows far away and returns on the wind of   //
// morning.                                                                   //
//                 -- Geoffrey James, "The Tao of Programming"                //
// ========================================================================== //
