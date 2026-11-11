// ==============================
// File:			TSDLSoundManager.cpp
// Project:			Einstein
//
// Copyright 2026 by the Einstein team.
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

#include "TSDLSoundManager.h"

#include <SDL3/SDL.h>

#include <cstdlib>

#include "Emulator/Log/TLog.h"

namespace {
	constexpr int kNewtonSampleRate = 22050;
	// Keep about three Newton buffers (250 ms) queued: the emulator on a
	// phone is not always quick enough to refill a single buffer in time,
	// and long sounds stuttered.
	constexpr int kQueueLowWater = 3;
	// Silence queued before a sound that starts from an empty queue (80 ms),
	// so a slow refill during a screen animation does not leave a gap.
	constexpr int kPrerollFrames = kNewtonSampleRate * 80 / 1000;
}

TSDLSoundManager::TSDLSoundManager(TLog* inLog) :
		TBufferedSoundManager(inLog)
{
	mHostBuffer = (KSInt16*) ::malloc(kNewtonBufferSize);

	SDL_AudioSpec spec;
	spec.format = SDL_AUDIO_S16;	// host byte order
	spec.channels = 1;
	spec.freq = kNewtonSampleRate;

	// The device starts paused; StartOutput() resumes it.
	mStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		&spec, AudioCallback, this);
	if (!mStream)
	{
		if (GetLog())
		{
			GetLog()->FLogLine("SDL_OpenAudioDeviceStream failed: %s", SDL_GetError());
		}
		return;
	}
	OutputVolumeChanged();
	if (GetLog())
	{
		GetLog()->FLogLine("Sound: SDL audio device '%s' opened",
			SDL_GetAudioDeviceName(SDL_GetAudioStreamDevice(mStream)));
	}
}

TSDLSoundManager::~TSDLSoundManager()
{
	if (mStream)
	{
		// Closes the device too.
		SDL_DestroyAudioStream(mStream);
	}
	::free(mHostBuffer);
}

void
TSDLSoundManager::ScheduleOutput(const KUInt8* inBuffer, KUInt32 inSize)
{
	if (inSize == 0)
	{
		// NewtonOS has no more data: the sound is over.
		mOutputRequestPending = false;
		if (mOutputIsRunning)
		{
			StopOutput();
		}
		return;
	}
	mOutputRequestPending = false;
	static int sLogCount = 0;
	if (GetLog() && sLogCount < 3)
	{
		sLogCount++;
		GetLog()->FLogLine("Sound: %u bytes from NewtonOS%s", (unsigned) inSize,
			OutputVolume() == kOutputVolume_Zero ? " (volume is zero, dropped)" : "");
	}
	if (!mStream || OutputVolume() == kOutputVolume_Zero)
	{
		return;
	}
	if (inSize > kNewtonBufferSize)
	{
		inSize = kNewtonBufferSize;
	}
	// NewtonOS samples are big-endian.
	KUInt32 frames = inSize / sizeof(KSInt16);
	for (KUInt32 i = 0; i < frames; i++)
	{
		KUInt16 be = (KUInt16) ((inBuffer[2 * i] << 8) | inBuffer[2 * i + 1]);
		mHostBuffer[i] = (KSInt16) be;
	}
	SDL_PutAudioStreamData(mStream, mHostBuffer, (int) (frames * sizeof(KSInt16)));
	// Still short? Ask for the next buffer now instead of waiting for the
	// device callback, which can be tens of milliseconds away.
	if (SDL_GetAudioStreamQueued(mStream) < (int) (kQueueLowWater * kNewtonBufferSize))
	{
		RequestOutputInterrupt();
	}
}

void
TSDLSoundManager::StartOutput()
{
	static int sLogCount = 0;
	if (GetLog() && sLogCount < 5)
	{
		sLogCount++;
		GetLog()->FLogLine("Sound: start, volume %.2f, queued %d bytes", OutputVolumeNormalized(),
			mStream ? SDL_GetAudioStreamQueued(mStream) : -1);
	}
	mOutputRequestPending = false;
	mOutputIsRunning = true;
	if (mStream && SDL_GetAudioStreamQueued(mStream) == 0)
	{
		static const KSInt16 sSilence[kPrerollFrames] = { };
		SDL_PutAudioStreamData(mStream, sSilence, (int) sizeof(sSilence));
	}
	if (mStream && !mDeviceRunning)
	{
		if (SDL_ResumeAudioStreamDevice(mStream))
		{
			mDeviceRunning = true;
		} else if (GetLog())
		{
			GetLog()->FLogLine("SDL_ResumeAudioStreamDevice failed: %s", SDL_GetError());
		}
	}
	// Ask for the first buffer right away.
	RequestOutputInterrupt();
}

void
TSDLSoundManager::StopOutput()
{
	// The queued samples play out. The device keeps running (playing
	// silence) until the next start, so the end of a sound is not cut off.
	mOutputIsRunning = false;
	mOutputRequestPending = false;
}

Boolean
TSDLSoundManager::OutputIsRunning()
{
	return mOutputIsRunning;
}

void
TSDLSoundManager::OutputVolumeChanged()
{
	if (mStream)
	{
		SDL_SetAudioStreamGain(mStream, OutputVolumeNormalized());
	}
}

void
TSDLSoundManager::RequestOutputInterrupt()
{
	bool expected = false;
	if (mOutputIsRunning && mOutputRequestPending.compare_exchange_strong(expected, true))
	{
		RaiseOutputInterrupt();
	}
}

void SDLCALL
TSDLSoundManager::AudioCallback(void* inUserData, SDL_AudioStream* inStream,
	int /* inAdditionalAmount */, int /* inTotalAmount */)
{
	static_cast<TSDLSoundManager*>(inUserData)->FeedDevice(inStream);
}

void
TSDLSoundManager::FeedDevice(SDL_AudioStream* inStream)
{
	// Called on SDL's audio thread whenever the device wants data.
	if (SDL_GetAudioStreamQueued(inStream) < (int) (kQueueLowWater * kNewtonBufferSize))
	{
		RequestOutputInterrupt();
	}
}
