// ==============================
// File:			TPortAudioSoundManager.cp
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
// Copyright 2018 by Victor Rehorst (victor@chuma.org).
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

// only need this if we use the hexdump routine for debugging
#ifdef DEBUG_SOUND
#include <ctype.h>
void hexdump(void *mem, unsigned int len);
#endif

// K
#include <K/Defines/UByteSex.h>
#include <K/Misc/TCircleBuffer.h>
#include <K/Threads/TMutex.h>

// Einstein.
#include "Emulator/Log/TLog.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#define kHostChannelCount				1
#define kHostSampleRate                 (22050.0)
#define	kHostResamplingMultiplier		1
#define kHostMultiplier					(kHostResamplingMultiplier * kHostChannelCount)
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

#ifdef DEBUG_SOUND

    printf("TPortAudioSoundManager: chans %d, sample rate %.2f, resampling_multi %d, host_multi %d\n", kHostChannelCount, kHostSampleRate, kHostResamplingMultiplier, kHostMultiplier);
    printf("                        newton_buf_size_bytes %d, multiplex_buf_size %d, output_buf_size_initial %ld\n", kNewtonBufferSize, kMultiplexedBufferSizeInBytes,  (kNewtonBufferSizeInFrames * 4 * sizeof(KUInt16)));

#endif
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


        // int numDevices = Pa_GetDeviceCount();
        // printf("PortAudio reports %d devices\n", numDevices);
		outputParameters.device           = Pa_GetDefaultOutputDevice();/* Default output device. */
#ifdef DEBUG_SOUND
        if (outputParameters.device == paNoDevice)
        {
            printf("No default PortAudio output device!\n");
        }
#endif
		outputParameters.channelCount     = kHostChannelCount;
		outputParameters.sampleFormat     = paInt16;					/* 16bits ints. */
		outputParameters.suggestedLatency =
			Pa_GetDeviceInfo(outputParameters.device)->defaultHighOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;

#ifdef DEBUG_SOUND
        printf("PortAudio device latency is %.3f\n", outputParameters.suggestedLatency);
#endif

        theErr = Pa_IsFormatSupported(NULL, &outputParameters, kHostSampleRate);

        if (theErr != paFormatIsSupported)
        {
            printf("PortAudio output parameters not supported!\n");
        }

		theErr = Pa_OpenStream(
			&mOutputStream,
			NULL,				// no input
			&outputParameters,
			kHostSampleRate,	// sample rate
            paFramesPerBufferUnspecified,
            // kNewtonBufferSizeInFrames*4,
            //kNewtonBufferSizeInFrames/4,
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
		if ( theErr != paNoError )
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
#ifdef DEBUG_SOUND
            printf("***** FROM NOS: ScheduleOutput size:%ld, frames:%ld, outputBuffer size:%ld\n", inSize, (inSize / sizeof(KSInt16)), inSize * kHostMultiplier );
            // printf("Start of inBuffer:\n");
            // hexdump((void *) inBuffer, 32);
#endif
#if (kHostMultiplier > 1) || TARGET_RT_LITTLE_ENDIAN
            // printf("ScheduleOutput Changing byte order\n");
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
#ifdef DEBUG_SOUND
            // printf("Start of mMultiplexedBuffer\n");
            // hexdump((void *) mMultiplexedBuffer, 32);
#endif
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
		// RaiseOutputInterrupt();
	}

}

// -------------------------------------------------------------------------- //
//  * StartOutput( void )
// -------------------------------------------------------------------------- //
void
TPortAudioSoundManager::StartOutput( void )
{
#ifdef DEBUG_SOUND
	printf( "***** FROM NOS -> StartOutput BEGIN ..." );
#endif
	if (!mOutputIsRunning)
	{
#ifdef DEBUG_SOUND
        printf(" Pa_StartStream...   ");
#endif
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
#ifdef DEBUG_SOUND
    else
    {
        printf("\n");
    }
	printf( "\n***** FROM NOS -> ... END StartOutput\n" );
#endif
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
// -------------------------------------------------------------------------- //
void
TPortAudioSoundManager::StopOutput( void )
{
#ifdef DEBUG_SOUND
    // printf( "StopOutput..." );
    printf( "***** FROM NOS -> StopOutput BEGIN ..." );
#endif
	if (mOutputIsRunning)
	{
        printf( " Pa_StopStream...   " );
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
        mOutputIsRunning = false;

	}
#ifdef DEBUG_SOUND
    else
    {
        printf("\n");
    }
    printf( "\n***** FROM NOS -> ... END StopOutput\n" );
#endif
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
Boolean
TPortAudioSoundManager::OutputIsRunning( void )
{
#ifdef DEBUG_SOUND
	printf( "***** FROM NOS -> OutputIsRunning: !(outputBuffer empty): %i, mOutputIsRunning: %s\n", !mOutputBuffer->IsEmpty(), mOutputIsRunning ? "true" : "false" );
#endif
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
	// Ask for more data / prime OutputBuffers
	// RaiseOutputInterrupt();

	// Copy data.
	// The size in bytes.
	// KUInt32 amount = frameCount * sizeof( KSInt16 );

	int result = paContinue;
	mDataMutex->Lock();
    KUInt32 bytesInBuffer = mOutputBuffer->AvailableBytes();
    mDataMutex->Unlock();
    KUIntPtr amount = MIN(bytesInBuffer, frameCount * sizeof( KSInt16 ));

	if (mOutputIsRunning)
	{
		result = paContinue;
	}
    else
    {
		result = paComplete;
	}
#ifdef DEBUG_SOUND
    printf("PACALLBACK %ld available, wants %ld frames, consuming %ld\n", bytesInBuffer, frameCount, amount);
#endif
    mDataMutex->Lock();
	KUInt32 available = mOutputBuffer->Consume( outputBuffer, amount );
	mDataMutex->Unlock();
	KUInt32 delta = amount - available;
	if (delta > 0)
	{
		// Zero the remaining of the buffer.
        printf("ZEROING DELTA %ld bytes\n", delta);
		::bzero( &((KUInt8*) outputBuffer)[available], delta );
	}

    if (bytesInBuffer < kNewtonBufferSize) {
		// Ask for more data.
        printf("PACALLBACK RAISEOUTPUTINTERRUPT\n");
		RaiseOutputInterrupt();
	}

	return result;
}

#ifdef DEBUG_SOUND

#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 8
#endif

void hexdump(void *mem, unsigned int len)
{
        unsigned int i, j;

        for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
        {
                /* print offset */
                if(i % HEXDUMP_COLS == 0)
                {
                        printf("0x%06x: ", i);
                }

                /* print hex data */
                if(i < len)
                {
                        printf("%02x ", 0xFF & ((char*)mem)[i]);
                }
                else /* end of block, just aligning for ASCII dump */
                {
                        printf("   ");
                }

                /* print ASCII dump */
                if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
                {
                        for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
                        {
                                if(j >= len) /* end of block, not really printing */
                                {
                                        putchar(' ');
                                }
                                else if(isprint(((char*)mem)[j])) /* printable char */
                                {
                                        putchar(0xFF & ((char*)mem)[j]);
                                }
                                else /* other char */
                                {
                                        putchar('.');
                                }
                        }
                        putchar('\n');
                }
        }
}
#endif
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
