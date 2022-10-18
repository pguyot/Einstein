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
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// K
#include <K/Misc/TCircleBuffer.h>
#include <K/Threads/TMutex.h>

// Einstein.
#include "Emulator/Log/TFileLog.h"

#define DEBUG_SOUND

// Debug logging
#ifdef DEBUG_SOUND
#define LOG_LINE(STR, ...) \
	if (GetLog())          \
	GetLog()->FLogLine("TPulseAudioSM:%3d: " STR, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_LINE(__VA_ARGS__, 0)
#define LOG_ERROR(...) LOG_LINE("ERROR! " __VA_ARGS__, 0)
#define LOG_ENTER(...) LOG_LINE("--> ENTER " __VA_ARGS__, 0)
#define LOG_WITHIN(...) LOG_LINE("          " __VA_ARGS__, 0)
#define LOG_LEAVE(...) LOG_LINE("<-- LEAVE " __VA_ARGS__, 0)
#else
#define LOG_LINE(...)
#define LOG_DEBUG(...)
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#define kNewtonSampleRate 22050
#define kNewtonBitsPerChannel 16
#define kNewtonNumChannels 1
#define kPulseAudioSampleFormat PA_SAMPLE_S16BE

// -------------------------------------------------------------------------- //
//  * TPulseAudioSoundManager( TLog* )
// -------------------------------------------------------------------------- //
TPulseAudioSoundManager::TPulseAudioSoundManager(TLog* inLog /* = nil */) :
		TBufferedSoundManager(inLog),
		mOutputBuffer(new TCircleBuffer(
			kNewtonBufferSizeInFrames * 4 * sizeof(KUInt16))),
		mDataMutex(new TMutex())
{
	int result = 0;
	int stream_flags = 0;
	const char* errorText = "";
	LOG_DEBUG("INIT PULSEAUDIO");

	mPAMainLoop = pa_threaded_mainloop_new();
	if (!mPAMainLoop)
	{
		errorText = "Can't allocate PulseAudio loop";
		goto error;
	}

	mPAContext = pa_context_new(pa_threaded_mainloop_get_api(mPAMainLoop), "Einstein");
	if (!mPAContext)
	{
		errorText = "Can't allocate PulseAudio context";
		goto error;
	}
	pa_context_set_state_callback(mPAContext, &SPAContextStateCallback, this);

	pa_threaded_mainloop_lock(mPAMainLoop);

	result = pa_threaded_mainloop_start(mPAMainLoop);
	if (result < 0)
	{
		errorText = "Can't start the PulseAudio main loop";
		goto error;
	}

	// start the context and wait for it to be ready
	result = pa_context_connect(mPAContext, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);
	if (result < 0)
	{
		errorText = "Can't connect to the PulseAudio server";
		goto error;
	}

	for (;;)
	{
		pa_context_state_t context_state = pa_context_get_state(mPAContext);
		if (context_state == PA_CONTEXT_READY)
			break;
		if (context_state == PA_CONTEXT_FAILED)
		{
			errorText = "Can't connect to the PulseAudio server, connection attempts failed";
			goto error;
		}
		if (context_state == PA_CONTEXT_TERMINATED)
		{
			errorText = "Can't connect to the PulseAudio server, connection was terminated";
			goto error;
		}
		pa_threaded_mainloop_wait(mPAMainLoop);
	}

	pa_sample_spec outputParameters;

	outputParameters.rate = kNewtonSampleRate;
	outputParameters.channels = kNewtonNumChannels;
	outputParameters.format = PA_SAMPLE_S16BE;

	pa_channel_map channelMap;
	pa_channel_map_init_mono(&channelMap);

	mOutputStream = pa_stream_new(mPAContext, "NewtonOS Sounds", &outputParameters, &channelMap);
	pa_stream_set_state_callback(mOutputStream, &SPAStreamStateCallback, this);
	pa_stream_set_write_callback(mOutputStream, &SPAStreamWriteCallback, this);
	pa_stream_set_underflow_callback(mOutputStream, &SPAStreamUnderflowCallback, this);

	pa_buffer_attr buffer_attr;

	buffer_attr.maxlength = kNewtonBufferSize * 8;
	buffer_attr.tlength = kNewtonBufferSize * 2;
	// buffer_attr.prebuf = kNewtonBufferSize / 2;

	// manual stream control - need to cork and uncork
	// This sort of works but then stops working.
	buffer_attr.prebuf = 0;

	// with this set and the driver starting CORKed, nothing ever plays.
	// buffer_attr.prebuf = kNewtonBufferSize / 4;
	// buffer_attr.minreq = (uint32_t) -1;
	buffer_attr.minreq = kNewtonBufferSize / 4;

	LOG_DEBUG("PA Buffer params: maxlength=%d, tlength=%d, prebuf=%d, minreq=%d",
		buffer_attr.maxlength, buffer_attr.tlength, buffer_attr.prebuf, buffer_attr.minreq);

	stream_flags = (PA_STREAM_START_CORKED | PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_NOT_MONOTONIC | PA_STREAM_AUTO_TIMING_UPDATE);

	result = pa_stream_connect_playback(mOutputStream, NULL, &buffer_attr, (pa_stream_flags_t) stream_flags, NULL,
		NULL);

	if (result != 0)
	{
		LOG_ERROR(
			"PulseAudio stream did not connect with pa_stream_connect_playback (%s)",
			pa_strerror(result));
		return;
	}

	for (;;)
	{
		pa_stream_state_t stream_state = pa_stream_get_state(mOutputStream);
		assert(PA_STREAM_IS_GOOD(stream_state));
		if (stream_state == PA_STREAM_READY)
			break;
		pa_threaded_mainloop_wait(mPAMainLoop);
	}

	pa_threaded_mainloop_unlock(mPAMainLoop);
	// ready for processing!
	OutputVolumeChanged();
	return;

error:
	if (mPAContext)
	{
		LOG_ERROR("TPulseAudioSoundManager: %s: %d\n", errorText, pa_context_errno(mPAContext));
	} else
	{
		LOG_ERROR("TPulseAudioSoundManager: %s.\n", errorText);
	}

	if (mPAMainLoop)
	{
		pa_threaded_mainloop_unlock(mPAMainLoop);
		pa_threaded_mainloop_stop(mPAMainLoop);
	}
	if (mPAContext)
	{
		pa_context_disconnect(mPAContext);
		pa_context_unref(mPAContext);
	}
	if (mPAMainLoop)
	{
		pa_threaded_mainloop_free(mPAMainLoop);
	}
	mPAContext = nullptr;
	mPAMainLoop = nullptr;
}

// -------------------------------------------------------------------------- //
//  * ~TPulseAudioSoundManager( void )
// -------------------------------------------------------------------------- //
TPulseAudioSoundManager::~TPulseAudioSoundManager(void)
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
TPulseAudioSoundManager::ScheduleOutput(const KUInt8* inBuffer, KUInt32 inSize)
{
	LOG_ENTER("ScheduleOutput");

	// just copy to the circle buffer.
	LOG_WITHIN("ScheduleOutput FROM NOS: %d bytes scheduled", inSize);
	if (inSize > 0)
	{
		mDataMutex->Lock();
		mOutputBuffer->Produce(inBuffer, inSize);
		mDataMutex->Unlock();
		// CoreAudio does not raise output interrupt here.  But we do.
		// StartOutput();
	} else
	{
		// StopOutput();
	}
	RaiseOutputInterrupt();
	LOG_LEAVE("ScheduleOutput");

	return;
}

// -------------------------------------------------------------------------- //
//  * StartOutput( void )
// -------------------------------------------------------------------------- //
void
TPulseAudioSoundManager::StartOutput(void)
{
	LOG_ENTER("StartOutput");
	pa_threaded_mainloop_lock(mPAMainLoop);

	if (pa_stream_is_corked(mOutputStream))
	{
		LOG_WITHIN("StartOutput uncorking!");
		mPAOperationDescr = "UNCORK";
		mPAOperation = pa_stream_cork(mOutputStream, 0, &SPATriggerAfterOpCB, this);

		mDataMutex->Lock();
		unsigned int bytesInBuffer = mOutputBuffer->AvailableBytes();
		mDataMutex->Unlock();

		if (bytesInBuffer >= kNewtonBufferSize / 8)
		{
			LOG_WITHIN("StartOutput Triggering!");
		}
	}

	pa_threaded_mainloop_unlock(mPAMainLoop);

	LOG_LEAVE("StartOutput");
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
// -------------------------------------------------------------------------- //
void
TPulseAudioSoundManager::StopOutput(void)
{
	LOG_ENTER("StopOutput - Drain");

	pa_threaded_mainloop_lock(mPAMainLoop);

	mPAOperationDescr = "DRAIN";
	mPAOperation = pa_stream_drain(mOutputStream, &SPAStreamOpCB, this);
	// using StreamDrainedCB to cork afterwards does not work.
	pa_threaded_mainloop_unlock(mPAMainLoop);
	LOG_LEAVE("StopOutput");
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
Boolean
TPulseAudioSoundManager::OutputIsRunning(void)
{
	LOG_ENTER("OutputIsRunning");
	pa_threaded_mainloop_lock(mPAMainLoop);
	Boolean streamCorked = (Boolean) pa_stream_is_corked(mOutputStream);
	pa_threaded_mainloop_unlock(mPAMainLoop);

	LOG_LEAVE("OutputIsRunning returning %s (PA Stream corked? %s)",
		streamCorked ? "false" : "true",
		streamCorked ? "true" : "false");

	return !streamCorked;
}

void
TPulseAudioSoundManager::PAStreamWriteCallback(pa_stream* s, unsigned int requestedSize)
{
	LOG_ENTER("Stream write callback");

	mDataMutex->Lock();
	unsigned int bytesInBuffer = mOutputBuffer->AvailableBytes();
	mDataMutex->Unlock();

	KUInt8* outBuffer;

	if (bytesInBuffer)
	{
		LOG_WITHIN("WriteCB: PA Requested %d, we have %d available", requestedSize, bytesInBuffer);
		size_t paBufferSize = bytesInBuffer;
		pa_stream_begin_write(s, (void**) &outBuffer, &paBufferSize);
		mDataMutex->Lock();
		KUIntPtr consumedBytes = mOutputBuffer->Consume(outBuffer, paBufferSize);
		mDataMutex->Unlock();
		LOG_WITHIN("WriteCB: Consumed %d bytes from ring buffer", consumedBytes);
		paBufferSize = consumedBytes;
		LOG_WITHIN("WriteCB: Writing %d to PA", consumedBytes);

		pa_stream_write(s, outBuffer, paBufferSize, NULL, 0LL, PA_SEEK_RELATIVE);
	} else
	{
		LOG_WITHIN("There was no data to write");
		pa_stream_cancel_write(s);
		// do we drain and cork here?  I don't think so.
		// mPAOperationDescr = "DRAIN";
		// mPAOperation = pa_stream_drain(s, &SPAStreamDrainedCB, this);
	}
	LOG_LEAVE("Stream write callback");
}

void
TPulseAudioSoundManager::PAContextStateCallback(pa_context* context, pa_threaded_mainloop* mainloop)
{
	pa_threaded_mainloop_signal(mainloop, 0);
}

void
TPulseAudioSoundManager::PAStreamStateCallback(pa_stream* s, pa_threaded_mainloop* mainloop)
{
	pa_stream_state_t sState = pa_stream_get_state(s);
	const char* sStateStr = "unknown";
	switch (sState)
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

	LOG_DEBUG("StreamStateCallback: %s", sStateStr);

	if (mainloop)
	{
		pa_threaded_mainloop_signal(mainloop, 0);
	}
}

void
TPulseAudioSoundManager::PAStreamUnderflowCallback(pa_stream* s, pa_threaded_mainloop* mainloop)
{
	LOG_DEBUG("   *** PA Underflow occurred, corking output.");

	mPAOperationDescr = "CORK";
	mPAOperation = pa_stream_cork(s, 1, &SPAStreamOpCB, this);

	// mPAOperationDescr = "DRAIN";
	// mPAOperation = pa_stream_drain(s, &SPAStreamDrainedCB, this);

	// RaiseOutputInterrupt();
	if (mainloop)
	{
		pa_threaded_mainloop_signal(mainloop, 0);
	}
}

// PA is telling us the stream is drained - so CORK it.  Don't need to wait for completion.
void
TPulseAudioSoundManager::PAStreamDrainedCB(pa_stream* s, int success, pa_threaded_mainloop* mainloop)
{
	LOG_ENTER("StreamDrainedCB, cork it!");
	SPASafeUnrefOp(mPAOperation);
	pa_operation* thisOp = pa_stream_cork(s, 1, NULL, NULL);
	SPASafeUnrefOp(thisOp);

	if (mainloop)
	{
		pa_threaded_mainloop_signal(mainloop, 0);
	}
	LOG_LEAVE("StreamDrainedCB");
}

void
TPulseAudioSoundManager::PATriggerAfterOpCB(pa_stream* s, int success, pa_threaded_mainloop* mainloop)
{
	LOG_ENTER("TriggerAfterOpCB: Stream uncorked, Trigger it!");
	SPASafeUnrefOp(mPAOperation);
	mPAOperation = pa_stream_trigger(s, &SPAStreamOpCB, this);

	if (mainloop)
	{
		pa_threaded_mainloop_signal(mainloop, 0);
	}
	LOG_LEAVE("TriggerAfterOpCB");
}

void
TPulseAudioSoundManager::PAStreamOpCB(pa_stream* s, int success, pa_threaded_mainloop* mainloop)
{
	SPASafeUnrefOp(mPAOperation);
	LOG_DEBUG("StreamOpCB: %s returned %d", mPAOperationDescr, success);
	mPAOperationDescr = "";

	if (mainloop)
	{
		pa_threaded_mainloop_signal(mainloop, 0);
	}
}

void
TPulseAudioSoundManager::OutputVolumeChanged()
{
	if (!mOutputStream)
		return;
	pa_threaded_mainloop_lock(mPAMainLoop);
	LOG_DEBUG("OutputVolume Changed %.2f", OutputVolumeNormalized());
	pa_cvolume cvolume;
	pa_cvolume_set(&cvolume, 1, pa_sw_volume_from_linear(OutputVolumeNormalized()));
	pa_context_set_sink_input_volume(mPAContext, pa_stream_get_index(mOutputStream), &cvolume, NULL, NULL);
	pa_threaded_mainloop_unlock(mPAMainLoop);
}

// ============================================================================= //
// <dark> "Let's form the Linux Standard Linux Standardization Association       //
//         Board. The purpose of this board will be to standardize Linux         //
//         Standardization Organizations."                                       //
// ============================================================================= //
