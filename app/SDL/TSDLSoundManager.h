// ==============================
// File:			TSDLSoundManager.h
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

#ifndef _TSDLSOUNDMANAGER_H
#define _TSDLSOUNDMANAGER_H

#include <K/Defines/KDefinitions.h>
#include "Emulator/Sound/TBufferedSoundManager.h"

#include <SDL3/SDL_audio.h>

#include <atomic>

///
/// Sound output through an SDL3 audio stream.
///
/// NewtonOS hands over buffers of 16-bit big-endian mono samples at
/// 22050 Hz. They are converted to the host byte order and queued on an
/// SDL audio stream; SDL converts them to whatever the device needs.
/// When less than one Newton buffer is left in the queue, the output
/// interrupt asks NewtonOS for more, one request at a time (like the
/// PulseAudio manager). An empty buffer from NewtonOS ends the output.
///
class TSDLSoundManager : public TBufferedSoundManager
{
public:
	TSDLSoundManager(TLog* inLog = nullptr);
	~TSDLSoundManager() override;

	void ScheduleOutput(const KUInt8* inBuffer, KUInt32 inSize) override;
	void StartOutput() override;
	void StopOutput() override;
	Boolean OutputIsRunning() override;
	void OutputVolumeChanged() override;

private:
	static void SDLCALL AudioCallback(void* inUserData, SDL_AudioStream* inStream,
		int inAdditionalAmount, int inTotalAmount);
	void FeedDevice(SDL_AudioStream* inStream);
	void RequestOutputInterrupt();

	SDL_AudioStream* mStream { nullptr };
	KSInt16* mHostBuffer { nullptr };
	std::atomic<bool> mOutputIsRunning { false };
	std::atomic<bool> mDeviceRunning { false };
	/// One request for more data at a time, cleared when data arrives.
	std::atomic<bool> mOutputRequestPending { false };
};

#endif
// _TSDLSOUNDMANAGER_H
