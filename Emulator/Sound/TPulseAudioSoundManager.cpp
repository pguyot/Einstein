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
#include <algorithm>
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
#include "Emulator/Log/TLog.h"

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
		mPAOperation(nullptr),
		mPAOperationDescr(nullptr),
		mOutputStream(nullptr),
		mPAMainLoop(nullptr),
		mPAMainLoopAPI(nullptr),
		mPAContext(nullptr),
		mOutputBuffer(new TCircleBuffer(
			kNewtonBufferSizeInFrames * 4 * sizeof(KUInt16))),
		mDataMutex(new TMutex()),
		mOutputIsRunning(false),
		mOutputRequestPending(false)
{
	int result = 0;
	int stream_flags = 0;
	const char* errorText = "";

	mPAMainLoop = pa_threaded_mainloop_new();
	if (!mPAMainLoop)
	{
		errorText = "Can't allocate PulseAudio loop";
		goto error;
	}

	mPAMainLoopAPI = pa_threaded_mainloop_get_api(mPAMainLoop);
	if (!mPAMainLoopAPI)
	{
		errorText = "Can't allocate PulseAudio loop API";
		goto error;
	}

	mPAContext = pa_context_new(mPAMainLoopAPI, "Einstein");
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

	mOutputStream = pa_stream_new(mPAContext, "Playback", &outputParameters, &channelMap);
	pa_stream_set_state_callback(mOutputStream, &SPAStreamStateCallback, this);
	pa_stream_set_write_callback(mOutputStream, &SPAStreamWriteCallback, this);
	pa_stream_set_underflow_callback(mOutputStream, &SPAStreamUnderflowCallback, this);

	pa_buffer_attr buffer_attr;

	buffer_attr.maxlength = kNewtonBufferSize * 8;
	buffer_attr.tlength = kNewtonBufferSize * 2;
	buffer_attr.prebuf = kNewtonBufferSize / 2;
	buffer_attr.minreq = (uint32_t) -1;

#ifdef DEBUG_SOUND
	if (GetLog())
	{
		GetLog()->FLogLine("PulseAudio Buffer: maxlength=%d, tlength=%d, prebuf=%d, minreq=%d",
			buffer_attr.maxlength, buffer_attr.tlength, buffer_attr.prebuf, buffer_attr.minreq);
	}

#endif

	stream_flags = (PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_NOT_MONOTONIC | PA_STREAM_AUTO_TIMING_UPDATE);

	result = pa_stream_connect_playback(mOutputStream, NULL, &buffer_attr, (pa_stream_flags_t) stream_flags, NULL,
		NULL);

	if (result != 0)
	{
		if (GetLog())
		{
			GetLog()->FLogLine(
				"PulseAudio stream did not connect with pa_stream_connect_playback (%s)",
				pa_strerror(result));
		}
		pa_threaded_mainloop_unlock(mPAMainLoop);
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
	if (GetLog())
	{
		if (mPAContext)
		{
			GetLog()->FLogLine("TPulseAudioSoundManager: %s: %d\n", errorText, pa_context_errno(mPAContext));
		} else
		{
			GetLog()->FLogLine("TPulseAudioSoundManager: %s.\n", errorText);
		}
	} else
	{
		if (mPAContext)
		{
			KPrintf("TPulseAudioSoundManager: %s: %d\n", errorText, pa_context_errno(mPAContext));
		} else
		{
			KPrintf("TPulseAudioSoundManager: %s.\n", errorText);
		}
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
		if (mPAMainLoop)
		{
			pa_threaded_mainloop_lock(mPAMainLoop);
		}
		pa_stream_set_write_callback(mOutputStream, NULL, NULL);
		pa_stream_set_underflow_callback(mOutputStream, NULL, NULL);
		pa_stream_set_state_callback(mOutputStream, NULL, NULL);
		// disconnect the stream
		pa_stream_disconnect(mOutputStream);
		pa_stream_unref(mOutputStream);
		if (mPAContext)
		{
			pa_context_disconnect(mPAContext);
			pa_context_unref(mPAContext);
		}
		if (mPAMainLoop)
		{
			pa_threaded_mainloop_unlock(mPAMainLoop);
		}
	}
	if (mPAMainLoop)
	{
		pa_threaded_mainloop_stop(mPAMainLoop);
		pa_threaded_mainloop_free(mPAMainLoop);
	}
	if (mDataMutex)
	{
		delete mDataMutex;
	}
	if (mOutputBuffer)
	{
		delete mOutputBuffer;
	}
}

// -------------------------------------------------------------------------- //
//  * ScheduleOutput( KUInt8*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TPulseAudioSoundManager::ScheduleOutput(const KUInt8* inBuffer, KUInt32 inSize)
{
	if (inSize > 0)
	{
#ifdef DEBUG_SOUND
		if (GetLog())
		{
			GetLog()->FLogLine("***** FROM NOS: ScheduleOutput size:%d, frames:%ld",
				inSize, (inSize / sizeof(KSInt16)));
		}
#endif
		mDataMutex->Lock();
		mOutputBuffer->Produce(inBuffer, inSize);
		mOutputRequestPending = false;
		mDataMutex->Unlock();

		pa_threaded_mainloop_lock(mPAMainLoop);
		PAStreamWriteQueuedOutput(mOutputStream, pa_stream_writable_size(mOutputStream), false);
		pa_threaded_mainloop_unlock(mPAMainLoop);
	} else if (mOutputIsRunning)
	{
#ifdef DEBUG_SOUND
		if (GetLog())
		{
			GetLog()->FLogLine("***** FROM NOS: ScheduleOutput no incoming data, STOP Output?");
		}
#endif
		mDataMutex->Lock();
		mOutputRequestPending = false;
		mDataMutex->Unlock();
		StopOutput();
	}
}

// -------------------------------------------------------------------------- //
//  * StartOutput( void )
// -------------------------------------------------------------------------- //
void
TPulseAudioSoundManager::StartOutput(void)
{
#ifdef DEBUG_SOUND
	if (GetLog())
	{
		GetLog()->FLogLine("   _____  StartOutput  _____");
	}
#endif
	mDataMutex->Lock();
	mOutputIsRunning = true;
	mOutputRequestPending = false;
	mDataMutex->Unlock();
	pa_threaded_mainloop_lock(mPAMainLoop);

	if (pa_stream_is_corked(mOutputStream))
	{
		mPAOperationDescr = "UNCORK";
		mPAOperation = pa_stream_cork(mOutputStream, 0, &SPAStreamOpCB, this);

		while (pa_operation_get_state(mPAOperation) == PA_OPERATION_RUNNING)
		{
			pa_threaded_mainloop_wait(mPAMainLoop);
		}
		pa_operation_unref(mPAOperation);
	}
#ifdef DEBUG_SOUND
	if (GetLog())
	{
		GetLog()->FLogLine("           Triggering!");
	}
#endif
	mPAOperationDescr = "TRIGGER";
	mPAOperation = pa_stream_trigger(mOutputStream, &SPAStreamOpCB, this);

	pa_threaded_mainloop_unlock(mPAMainLoop);
	RequestOutputInterrupt();
#ifdef DEBUG_SOUND
	if (GetLog())
	{
		GetLog()->FLogLine("   ^^^^^  StartOutput  ^^^^^");
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
// -------------------------------------------------------------------------- //
void
TPulseAudioSoundManager::StopOutput(void)
{
#ifdef DEBUG_SOUND
	if (GetLog())
	{
		GetLog()->FLogLine("   _____  StopOutput BEGIN _____");
	}
#endif

	pa_threaded_mainloop_lock(mPAMainLoop);

	mDataMutex->Lock();
	mOutputIsRunning = false;
	mOutputRequestPending = false;
	mDataMutex->Unlock();

	mPAOperationDescr = "DRAIN";
	mPAOperation = pa_stream_drain(mOutputStream, &SPAStreamOpCB, this);

	while (pa_operation_get_state(mPAOperation) == PA_OPERATION_RUNNING)
	{
		pa_threaded_mainloop_wait(mPAMainLoop);
	}

	pa_operation_unref(mPAOperation);
	pa_threaded_mainloop_unlock(mPAMainLoop);
#ifdef DEBUG_SOUND
	if (GetLog())
	{
		GetLog()->FLogLine("   ^^^^^  StopOutput END ^^^^^");
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
bool
TPulseAudioSoundManager::OutputIsRunning(void)
{
	bool streamCorked = (bool) pa_stream_is_corked(mOutputStream);
	bool outputIsRunning;
	mDataMutex->Lock();
	outputIsRunning = mOutputIsRunning;
	mDataMutex->Unlock();
#ifdef DEBUG_SOUND
	if (GetLog())
	{
		GetLog()->FLogLine("   *****  OutputIsRunning: (PA Stream Corked? %s) (mOutputIsRunning? %s)",
			streamCorked ? "true" : "false",
			outputIsRunning ? "true" : "false");
		GetLog()->FLogLine("   *****  OutputIsRunning returns %s\n", outputIsRunning ? "true" : "false");
	}
#else
	(void) streamCorked;
#endif

	return outputIsRunning;
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
	const char* sStateStr = "";
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
#ifdef DEBUG_SOUND
	if (GetLog())
	{
		GetLog()->FLogLine("  *** StreamStateCallback: %s", sStateStr);
	}
#else
	(void) sStateStr;
#endif
	if (mainloop)
	{
		pa_threaded_mainloop_signal(mainloop, 0);
	}
}

void
TPulseAudioSoundManager::PAStreamUnderflowCallback(pa_stream* s, pa_threaded_mainloop* mainloop)
{
#ifdef DEBUG_SOUND
	if (GetLog())
	{
		GetLog()->FLogLine("   *** PA Underflow occurred!");
	}
#endif
	RequestOutputInterrupt();
	if (mainloop)
	{
		pa_threaded_mainloop_signal(mainloop, 0);
	}
}

void
TPulseAudioSoundManager::PAStreamWriteCallback(pa_stream* s, unsigned int requested_bytes)
{
	PAStreamWriteQueuedOutput(s, requested_bytes, true);
}

void
TPulseAudioSoundManager::PAStreamWriteQueuedOutput(pa_stream* s, size_t requested_bytes, bool requestMoreOutput)
{
	void* outBuffer = NULL;
	size_t bytesToWrite = requested_bytes;
	KUIntPtr bytesCopied;

	if (bytesToWrite == 0)
	{
		return;
	}
	if (mOutputBuffer == NULL || mDataMutex == NULL)
	{
		return;
	}

	mDataMutex->Lock();
	bytesToWrite = std::min<size_t>(mOutputBuffer->AvailableBytes(), bytesToWrite);
	mDataMutex->Unlock();

	if (bytesToWrite == 0)
	{
		if (requestMoreOutput)
		{
			RequestOutputInterrupt();
		}
		return;
	}

	if (pa_stream_begin_write(s, &outBuffer, &bytesToWrite) < 0)
	{
		if (requestMoreOutput)
		{
			RequestOutputInterrupt();
		}
		return;
	}
	if (outBuffer == NULL || bytesToWrite == 0)
	{
		pa_stream_cancel_write(s);
		if (requestMoreOutput)
		{
			RequestOutputInterrupt();
		}
		return;
	}

	mDataMutex->Lock();
	bytesCopied = mOutputBuffer->Peek(outBuffer, bytesToWrite);
	mDataMutex->Unlock();

	if (bytesCopied == 0)
	{
		pa_stream_cancel_write(s);
		if (requestMoreOutput)
		{
			RequestOutputInterrupt();
		}
		return;
	}

	if (pa_stream_write(s, outBuffer, bytesCopied, NULL, 0LL, PA_SEEK_RELATIVE) < 0)
	{
		if (requestMoreOutput)
		{
			RequestOutputInterrupt();
		}
		return;
	}

	mDataMutex->Lock();
	mOutputBuffer->Discard(bytesCopied);
	KUIntPtr bytesLeft = mOutputBuffer->AvailableBytes();
	mDataMutex->Unlock();

	if (requestMoreOutput && bytesLeft < kNewtonBufferSize)
	{
		RequestOutputInterrupt();
	}
}

void
TPulseAudioSoundManager::RequestOutputInterrupt(void)
{
	bool requestOutput = false;

	if (mDataMutex == NULL)
	{
		return;
	}

	mDataMutex->Lock();
	if (mOutputIsRunning && !mOutputRequestPending)
	{
		mOutputRequestPending = true;
		requestOutput = true;
	}
	mDataMutex->Unlock();

	if (requestOutput)
	{
		RaiseOutputInterrupt();
	}
}

void
TPulseAudioSoundManager::PAStreamOpCB(pa_stream* s, int success, pa_threaded_mainloop* mainloop)
{
#ifdef DEBUG_SOUND
	if (GetLog())
	{
		GetLog()->FLogLine("   %s returned %d", mPAOperationDescr, success);
	}
#endif
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
