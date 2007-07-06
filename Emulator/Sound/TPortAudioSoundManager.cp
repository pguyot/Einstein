// ==============================
// File:			TPortAudioSoundManager.cp
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

#include <K/Defines/KDefinitions.h>
#include "TPortAudioSoundManager.h"

// ANSI C & POSIX
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

// K
#include <K/Defines/UByteSex.h>
#include <K/Misc/TCircleBuffer.h>
#include <K/Threads/TMutex.h>

// Einstein.
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#define kHostChannelCount				2	/* stereo, mono doesn't work on Mac */
#define	kHostSampleRate					(44100.0)
#define	kHostResamplingMultiplier		2
#define kHostMultiplier					(kHostResamplingMultiplier * kHostChannelCount)
										/* stereo + 44KHz instead of 22 */
// twice the Newton's buffer should be enough.
#define kMultiplexedBufferSizeInBytes	(kNewtonBufferSizeInFrames * kHostMultiplier * sizeof(KSInt16))

// -------------------------------------------------------------------------- //
//  * TPortAudioSoundManager( TLog* )
// -------------------------------------------------------------------------- //
TPortAudioSoundManager::TPortAudioSoundManager( TLog* inLog /* = nil */ )
	:
		TBufferedSoundManager( inLog ),
		mOutputStream( nil ),
		mOutputBuffer( new TCircleBuffer(
								kNewtonBufferSizeInFrames *
								4 * sizeof(KUInt16) ) ),
		mDataMutex( new TMutex() ),
		mMultiplexedBuffer( nil ),
		mOutputIsRunning( false )
{
	do {
		PaStreamParameters outputParameters;
		PaError theErr = Pa_Initialize();
		if( theErr != paNoError )
		{
			if (GetLog())
			{
				GetLog()->FLogLine(
					"Error with PortAudio::Pa_Initialize (%s)",
					Pa_GetErrorText( theErr ) );
			}
			break;
		}

		outputParameters.device           = Pa_GetDefaultOutputDevice();/* Default output device. */
		outputParameters.channelCount     = kHostChannelCount;
		outputParameters.sampleFormat     = paInt16;					/* 16bits ints. */
		outputParameters.suggestedLatency =
			Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;

		theErr = Pa_OpenStream(
			&mOutputStream,
			NULL,				// no input
			&outputParameters,
			kHostSampleRate,	// sample rate
			kNewtonBufferSizeInFrames * kHostResamplingMultiplier,
								/* frames per buffer (newton buffer size) */
			paClipOff | paPrimeOutputBuffersUsingStreamCallback,
			SPACallBack,		// callback method.
			this );				// callback data

		if( theErr != paNoError )
		{
			if (GetLog())
			{
				GetLog()->FLogLine(
					"Error with PortAudio::Pa_OpenStream (%s)",
					Pa_GetErrorText( theErr ) );
			}
			break;
		}
		
#if (kHostMultiplier > 1) || TARGET_RT_LITTLE_ENDIAN
		mMultiplexedBuffer = (KSInt16*) ::malloc( kMultiplexedBufferSizeInBytes );
#endif
    } while (false);
}

// -------------------------------------------------------------------------- //
//  * ~TPortAudioSoundManager( void )
// -------------------------------------------------------------------------- //
TPortAudioSoundManager::~TPortAudioSoundManager( void )
{
	if (mOutputStream)
	{
		PaError theErr = Pa_CloseStream(mOutputStream);
		if( theErr != paNoError )
		{
			if (GetLog())
			{
				GetLog()->FLogLine(
					"Error with PortAudio::Pa_CloseStream (%s)",
					Pa_GetErrorText( theErr ) );
			}
		}

		theErr = Pa_Terminate();
		if( theErr != paNoError )
		{
			if (GetLog())
			{
				GetLog()->FLogLine(
					"Error with PortAudio::Pa_Terminate (%s)",
					Pa_GetErrorText( theErr ) );
			}
		}
	}
	
	if (mDataMutex)
	{
		delete mDataMutex;
	}
	if (mOutputBuffer)
	{
		delete mOutputBuffer;
	}
	if (mMultiplexedBuffer)
	{
		::free( mMultiplexedBuffer );
	}
}

// -------------------------------------------------------------------------- //
//  * ScheduleOutput( const KUInt8*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TPortAudioSoundManager::ScheduleOutput( const KUInt8* inBuffer, KUInt32 inSize )
{
	if (inSize > 0)
	{
		if (OutputVolume() != kOutputVolume_Zero)
		{
			// Write to the output buffer.
			// Resample buffer.
#if (kHostMultiplier > 1) || TARGET_RT_LITTLE_ENDIAN
			KUInt32 framesCount = inSize / sizeof( KSInt16 );
			KSInt16* multiplexedCursor = mMultiplexedBuffer;
			KSInt16* bufferCursor = (KSInt16*) inBuffer;
			
			while (--framesCount)
			{
				KSInt16 theFrame = *bufferCursor++;
				*multiplexedCursor++ = UByteSex_FromBigEndian((KUInt16) theFrame);
#if kHostMultiplier > 1
				*multiplexedCursor++ = UByteSex_FromBigEndian((KUInt16) theFrame);
#if kHostMultiplier > 2
				*multiplexedCursor++ = UByteSex_FromBigEndian((KUInt16) theFrame);
#if kHostMultiplier > 3
				*multiplexedCursor++ = UByteSex_FromBigEndian((KUInt16) theFrame);
#endif
#endif
#endif
			}
	
			// Copy data.
			mDataMutex->Lock();
			mOutputBuffer->Produce(
				mMultiplexedBuffer,
				inSize * kHostMultiplier );
			mDataMutex->Unlock();
#else
			// Copy data.
			mDataMutex->Lock();
			mOutputBuffer->Produce( inBuffer, inSize );
			mDataMutex->Unlock();
#endif
		}

		// Ask for more data.
		RaiseOutputInterrupt();
	}
}

// -------------------------------------------------------------------------- //
//  * StartOutput( void )
// -------------------------------------------------------------------------- //
void
TPortAudioSoundManager::StartOutput( void )
{
	printf( "StartOutput...\n" );
	if (!mOutputIsRunning)
	{
		printf( "	Pa_StartStream\n" );
		PaError theErr = Pa_StartStream(mOutputStream);
		if (theErr != paNoError)
		{
			printf( "	error: %s\n", Pa_GetErrorText( theErr ) );
			if (GetLog())
			{
				GetLog()->FLogLine(
					"Error with PortAudio::Pa_StartStream (%s)",
					Pa_GetErrorText( theErr ) );
			}
		}
		
		mOutputIsRunning = true;
	}
	printf( "...StartOutput\n" );
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
// -------------------------------------------------------------------------- //
void
TPortAudioSoundManager::StopOutput( void )
{
	if (mOutputIsRunning)
	{
		mOutputIsRunning = false;

		PaError theErr = Pa_StopStream(mOutputStream);
		if (theErr != paNoError)
		{
			if (GetLog())
			{
				GetLog()->FLogLine(
					"Error with PortAudio::Pa_StopStream (%s)",
					Pa_GetErrorText( theErr ) );
			}
		}
	}
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
Boolean
TPortAudioSoundManager::OutputIsRunning( void )
{
	printf( "OutputIsRunning: %i\n", !mOutputBuffer->IsEmpty() );
	return !mOutputBuffer->IsEmpty();
}

// -------------------------------------------------------------------------- //
//  * PACallBack( void*, void*, unsigned long, PaTimestamp )
// -------------------------------------------------------------------------- //
int
TPortAudioSoundManager::PACallBack(
				const void* /* inputBuffer */,
				void* outputBuffer,
				unsigned long frameCount,
				const PaStreamCallbackTimeInfo* /* timeInfo */,
				PaStreamCallbackFlags /* statusFlags */ )
{
	// Ask for more data.
	RaiseOutputInterrupt();

	// Copy data.
	// The size in bytes.
	KUInt32 amount = frameCount * sizeof( KSInt16 );
	int result;
	mDataMutex->Lock();
	if (mOutputIsRunning)
	{
		result = paContinue;
	} else {
		result = paComplete;
	}
	KUInt32 available = mOutputBuffer->Consume( outputBuffer, amount );
	mDataMutex->Unlock();
	KUInt32 delta = amount - available;
	if (delta > 0)
	{
		// Zero the remaining of the buffer.
		::bzero( &((KUInt8*) outputBuffer)[available], delta );
	}
	
	return result;
}

// ============================================================================= //
// As in Protestant Europe, by contrast, where sects divided endlessly into      //
// smaller competing sects and no church dominated any other, all is different   //
// in the fragmented world of IBM.  That realm is now a chaos of conflicting     //
// norms and standards that not even IBM can hope to control.  You can buy a     //
// computer that works like an IBM machine but contains nothing made or sold by  //
// IBM itself.  Renegades from IBM constantly set up rival firms and establish   //
// standards of their own.  When IBM recently abandoned some of its original     //
// standards and decreed new ones, many of its rivals declared a puritan         //
// allegiance to IBM's original faith, and denounced the company as a divisive   //
// innovator.  Still, the IBM world is united by its distrust of icons and       //
// imagery.  IBM's screens are designed for language, not pictures.  Graven      //
// images may be tolerated by the luxurious cults, but the true IBM faith relies //
// on the austerity of the word.                                                 //
//                 -- Edward Mendelson, "The New Republic", February 22, 1988    //
// ============================================================================= //
