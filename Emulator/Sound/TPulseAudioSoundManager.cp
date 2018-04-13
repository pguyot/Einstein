// ==============================
// File:			TPulseAudioSoundManager.cp
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
#include "TPulseAudioSoundManager.h"

// ANSI C * POSIX
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>
#include <string.h>

// K
#include <K/Threads/TMutex.h>

// Einstein.
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#define kNewtonSampleRate           22050
#define kNewtonBitsPerChannel       16
#define kNewtonNumChannels          1
#define kPulseAudioSampleFormat     PA_SAMPLE_S16BE

// -------------------------------------------------------------------------- //
//  * TPulseAudioSoundManager( TLog* )
// -------------------------------------------------------------------------- //
TPulseAudioSoundManager::TPulseAudioSoundManager( TLog* inLog /* = nil */ )
	:
		TBufferedSoundManager( inLog ),
        mDataMutex( new TMutex() ),
		mOutputIsRunning( false )
{
    int result;

    mPAMainLoop = pa_threaded_mainloop_new();
    mPAMainLoopAPI = pa_threaded_mainloop_get_api(mPAMainLoop);
    mPAContext = pa_context_new(mPAMainLoopAPI, "Einstein");
    assert(mPAContext);
    pa_context_set_state_callback(mPAContext, &SPAContextStateCallback, this);

    pa_threaded_mainloop_lock(mPAMainLoop);

    assert(pa_threaded_mainloop_start(mPAMainLoop) == 0);
    // start the context and wait for it to be ready
    assert(pa_context_connect(mPAContext, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL) == 0);

    for(;;) {
        pa_context_state_t context_state = pa_context_get_state(mPAContext);
        if (context_state == PA_CONTEXT_READY) break;
        pa_threaded_mainloop_wait(mPAMainLoop);
    }

    pa_sample_spec outputParameters;

    outputParameters.rate = kNewtonSampleRate;
    outputParameters.channels = kNewtonNumChannels;
    outputParameters.format = PA_SAMPLE_S16BE;

    pa_channel_map channelMap;
    pa_channel_map_init_mono(&channelMap);

    mOutputStream = pa_stream_new(mPAContext, "Playback", &outputParameters, &channelMap);
    pa_stream_set_state_callback(mOutputStream, &SPAStreamStateCallback, this);
    // NO WRITE CALLBACK - we do writes to PulseAudio immediately upon receiving data from the Newton
    // pa_stream_set_write_callback(mOutputStream, &SPAStreamWriteCallback, this);
    pa_stream_set_underflow_callback(mOutputStream, &SPAStreamUnderflowCallback, this);

    pa_buffer_attr buffer_attr;

    buffer_attr.maxlength = kNewtonBufferSize * 8;
    buffer_attr.tlength = kNewtonBufferSize * 2;
    buffer_attr.prebuf = kNewtonBufferSize / 2;
    buffer_attr.minreq = (uint32_t) -1;

#ifdef DEBUG_SOUND
    printf("PulseAudio Buffer: maxlength=%d, tlength=%d, prebuf=%d, minreq=%d\n",
        buffer_attr.maxlength, buffer_attr.tlength, buffer_attr.prebuf, buffer_attr.minreq);
#endif

    int stream_flags =  (PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_NOT_MONOTONIC | PA_STREAM_AUTO_TIMING_UPDATE);

    result = pa_stream_connect_playback(mOutputStream, NULL, &buffer_attr, (pa_stream_flags_t) stream_flags, NULL, NULL);

    if (result != 0)
    {
        if (GetLog())
		{
			GetLog()->FLogLine(
				"PulseAudio stream did not connect with pa_stream_connect_playback (%s)",
				pa_strerror( result ) );
		}
		return;
    }

    for(;;)
    {
        pa_stream_state_t stream_state = pa_stream_get_state(mOutputStream);
        assert(PA_STREAM_IS_GOOD(stream_state));
        if (stream_state == PA_STREAM_READY) break;
        pa_threaded_mainloop_wait(mPAMainLoop);
    }

    pa_threaded_mainloop_unlock(mPAMainLoop);
    // ready for processing!
}

// -------------------------------------------------------------------------- //
//  * ~TPulseAudioSoundManager( void )
// -------------------------------------------------------------------------- //
TPulseAudioSoundManager::~TPulseAudioSoundManager( void )
{
    if (mOutputStream)
    {
        // disconnect the stream
        pa_stream_disconnect(mOutputStream);
        pa_context_disconnect(mPAContext);
        // pa_threaded_mainloop_stop
        pa_threaded_mainloop_stop(mPAMainLoop);
        pa_threaded_mainloop_free(mPAMainLoop);
    }
    if (mDataMutex)
    {
        delete mDataMutex;
    }
}

// -------------------------------------------------------------------------- //
//  * ScheduleOutput( KUInt8*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TPulseAudioSoundManager::ScheduleOutput( const KUInt8* inBuffer, KUInt32 inSize )
{
    int error;
    if (inSize > 0)
    {
        if (OutputVolume() != kOutputVolume_Zero)
        {
            size_t inputSize = inSize;
            size_t roomInPAStream = pa_stream_writable_size(mOutputStream);
            KUInt8* outBuffer = NULL;
#ifdef DEBUG_SOUND
printf("***** FROM NOS: ScheduleOutput size:%d, frames:%ld, PA out buffer size:%d\n", inputSize, (inSize / sizeof(KSInt16)), roomInPAStream );
#endif
            if (roomInPAStream >= inputSize)
            {
#ifdef DEBUG_SOUND
                printf("***** FROM NOS: ScheduleOutput writing %d to PulseAudio (lots of space)\n", inputSize);
#endif
                pa_stream_begin_write(mOutputStream, (void**) &outBuffer, &inputSize);
                ::memcpy(outBuffer, inBuffer, inputSize);
                pa_stream_write(mOutputStream, outBuffer, inputSize, NULL, 0LL, PA_SEEK_RELATIVE);
            }
            else
            {
#ifdef DEBUG_SOUND
                printf("***** FROM NOS: ScheduleOutput writing %d to PulseAudio (LACK of space)\nRAISEOUTPUTINTERRUPT SCHEDULEOUTPUT\n", roomInPAStream);
#endif
                pa_stream_begin_write(mOutputStream, (void**) &outBuffer, &roomInPAStream);
                ::memcpy(outBuffer, inBuffer, roomInPAStream);
                pa_stream_write(mOutputStream, outBuffer, inputSize, NULL, 0LL, PA_SEEK_RELATIVE);
                RaiseOutputInterrupt();
            }

            if (inSize < kNewtonBufferSize)
            {
#ifdef DEBUG_SOUND
                printf("RAISEOUTPUTINTERRUPT SCHEDULEOUTPUT\n");
#endif
                RaiseOutputInterrupt();
            }
        }
    }
    else if (mOutputIsRunning)
    {
#ifdef DEBUG_SOUND
        printf("***** FROM NOS: ScheduleOutput no incoming data, STOP Output?\n");
#endif
        StopOutput();
    }
}

// void TPulseAudioSoundManager::PAStreamWriteCallback(pa_stream* s, size_t requestedBytes)
// {
//     int bytesRemaining = requestedBytes;
// printf("PAStreamWriteCallback YASSSSSSSS\n");
//     mDataMutex->Lock();
//     KUInt32 bytesAvailable = mOutputBuffer->AvailableBytes();
//     mDataMutex->Unlock();
//     size_t bytesToWrite = MIN(bytesAvailable, bytesRemaining);
//     if (bytesToWrite > 0)
//     {
//         printf("PAStreamWriteCallback writin' %d bytes YASSSSSSSS\n", bytesToWrite);
//         KUInt8 *outBuffer = NULL;
//         size_t i;
//
//         pa_stream_begin_write(mOutputStream, (void**) &outBuffer, &bytesToWrite);
//
//         mDataMutex->Lock();
//         mOutputBuffer->Produce(outBuffer, bytesToWrite);
//         mDataMutex->Unlock();
//
//         pa_stream_write(mOutputStream, outBuffer, bytesToWrite, NULL, 0LL, PA_SEEK_RELATIVE);
//     }
// }

// -------------------------------------------------------------------------- //
//  * StartOutput( void )
// -------------------------------------------------------------------------- //
void
TPulseAudioSoundManager::StartOutput( void )
{
    pa_operation *o;
#ifdef DEBUG_SOUND
    printf("   _____  StartOutput  _____\n");
#endif
    mOutputIsRunning = true;
    pa_threaded_mainloop_lock(mPAMainLoop);

    if (pa_stream_is_corked(mOutputStream))
    {
        printf("           Uncorking!\n");
        mPAOperationDescr = "UNCORK";
        mPAOperation = pa_stream_cork(mOutputStream, 0, &SPAStreamOpCB, this);

        while (pa_operation_get_state(mPAOperation) == PA_OPERATION_RUNNING)
        {
            pa_threaded_mainloop_wait(mPAMainLoop);
        }
        pa_operation_unref(mPAOperation);
    }
#ifdef DEBUG_SOUND
    printf("           Triggering!\n");
#endif
    mPAOperationDescr = "TRIGGER";
    mPAOperation = pa_stream_trigger(mOutputStream, &SPAStreamOpCB, this);

    pa_threaded_mainloop_unlock(mPAMainLoop);
#ifdef DEBUG_SOUND
    printf("   ^^^^^  StartOutput  ^^^^^\n");
#endif;
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
// -------------------------------------------------------------------------- //
void
TPulseAudioSoundManager::StopOutput( void )
{
    pa_operation *o;
#ifdef DEBUG_SOUND
    printf("   _____  StopOutput BEGIN _____\n");
#endif

    pa_threaded_mainloop_lock(mPAMainLoop);

    mPAOperationDescr = "DRAIN";
    mPAOperation = pa_stream_drain(mOutputStream, &SPAStreamOpCB, this);

    while (pa_operation_get_state(mPAOperation) == PA_OPERATION_RUNNING)
    {
        pa_threaded_mainloop_wait(mPAMainLoop);
    }

    pa_operation_unref(mPAOperation);
    mOutputIsRunning = false;
    pa_threaded_mainloop_unlock(mPAMainLoop);
#ifdef DEBUG_SOUND
    printf("   ^^^^^  StopOutput END ^^^^^\n");
#endif
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
Boolean
TPulseAudioSoundManager::OutputIsRunning( void )
{
    Boolean streamCorked = (Boolean)pa_stream_is_corked(mOutputStream);
#ifdef DEBUG_SOUND
    printf( "   *****  OutputIsRunning: (PA Stream Corked? %s) (mOutputIsRunning? %s)\n",
        streamCorked ? "true" : "false",
        mOutputIsRunning ? "true" : "false" );
    printf("   *****  OutputIsRunning returns %s\n", mOutputIsRunning ? "true" : "false");
#endif

    return mOutputIsRunning;
}

void TPulseAudioSoundManager::PAContextStateCallback(pa_context* context, pa_threaded_mainloop* mainloop)
{
    pa_threaded_mainloop_signal(mainloop, 0);
}

void TPulseAudioSoundManager::PAStreamStateCallback(pa_stream* s, pa_threaded_mainloop* mainloop)
{
    pa_stream_state_t sState = pa_stream_get_state(s);
    const char* sStateStr = "";
    switch(sState)
    {
        case PA_STREAM_UNCONNECTED:
            sStateStr = "unconnected";
            break;
        case PA_STREAM_CREATING:
            sStateStr = "creating";
            break;
        case PA_STREAM_READY:
            sStateStr = "ready";
            break;
        case PA_STREAM_TERMINATED:
            sStateStr = "terminated";
            break;
        case PA_STREAM_FAILED:
            sStateStr = "failed";
            break;
    }
#ifdef DEBUG_SOUND
    printf("  *** StreamStateCallback: %s\n", sStateStr);
#endif
    if (mainloop)
    {
        pa_threaded_mainloop_signal(mainloop, 0);
    }
}

void TPulseAudioSoundManager::PAStreamUnderflowCallback(pa_stream* s, pa_threaded_mainloop* mainloop)
{
#ifdef DEBUG_SOUND
    printf("   *** PA Underflow occurred!\n");
#endif;
    RaiseOutputInterrupt();
    if (mainloop)
    {
        pa_threaded_mainloop_signal(mainloop, 0);
    }
}

void TPulseAudioSoundManager::PAStreamOpCB(pa_stream* s, int success, pa_threaded_mainloop* mainloop)
{
#ifdef DEBUG_SOUND
    printf("   %s returned %d\n", mPAOperationDescr, success);
#endif
    if (mainloop)
    {
        pa_threaded_mainloop_signal(mainloop, 0);
    }
}

// ============================================================================= //
// <dark> "Let's form the Linux Standard Linux Standardization Association       //
//         Board. The purpose of this board will be to standardize Linux         //
//         Standardization Organizations."                                       //
// ============================================================================= //
