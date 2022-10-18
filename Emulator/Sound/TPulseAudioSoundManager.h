// ==============================
// File:			TPulseAudioSoundManager.h
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

#ifndef _TPulseAudioSoundManager_H
#define _TPulseAudioSoundManager_H

#include <K/Defines/KDefinitions.h>

// PulseAudio
#include <pulse/error.h>
#include <pulse/pulseaudio.h>

// Einstein
#include "TBufferedSoundManager.h"

class TMutex;
class TCircleBuffer;

///
/// Class to handle sound input/output and direct them to PulseAudio.
///
/// \author Victor Rehorst <victor@chuma.org>
/// \version $Revision: 111 $
///
/// \test	aucun test défini.
///
class TPulseAudioSoundManager
		: public TBufferedSoundManager
{
public:
	///
	/// Constructor from a log.
	///
	TPulseAudioSoundManager(TLog* inLog = nil);

	///
	/// Destructor.
	///
	~TPulseAudioSoundManager(void) override;

	///
	/// Schedule output of some buffer. Called when NewtonOS has sounds to play.
	///
	void ScheduleOutput(const KUInt8* inBufferAddr, KUInt32 inSize) override;

	///
	/// Start output.
	///
	void StartOutput(void) override;

	///
	/// Is output running?
	///
	Boolean OutputIsRunning(void) override;

	///
	/// Stop output.
	///
	void StopOutput(void) override;

	///
	/// Method called to signal a change in the output volume.
	///
	void OutputVolumeChanged(void) override;

private:
	// callback routines (static).  PulseAudio lets us pass an arbitrary void pointer to callback functions.
  // When callback function pointer is given to PulseAudio, we give a pointer to the class instance (this) as userData.
  // Then in the static functions here, userData is used to call the class function
  // defined in the .cpp file.  That function receives a pointer to the PA mainloop, NOT the original UserData!

  // Context state change callback
	static void
	SPAContextStateCallback(pa_context* context, void* userData)
	{
		return ((TPulseAudioSoundManager*) userData)->PAContextStateCallback(context, ((TPulseAudioSoundManager*) userData)->mPAMainLoop);
	}
  void PAContextStateCallback(pa_context* context, pa_threaded_mainloop* mainloop);

  // Stream state change callback
	static void SPAStreamStateCallback(pa_stream* s, void* userData)
	{
		return ((TPulseAudioSoundManager*) userData)->PAStreamStateCallback(s, ((TPulseAudioSoundManager*) userData)->mPAMainLoop);
	}
  void PAStreamStateCallback(pa_stream* s, pa_threaded_mainloop* mainloop);

  // Stream Underflow event callback.
	static void SPAStreamUnderflowCallback(pa_stream* s, void* userData)
	{
		return ((TPulseAudioSoundManager*) userData)->PAStreamUnderflowCallback(s, ((TPulseAudioSoundManager*) userData)->mPAMainLoop);
	}
  void PAStreamUnderflowCallback(pa_stream* s, pa_threaded_mainloop* mainloop);

  // Stream write callback - called when PulseAudio wants us to write to the stream.
	static void SPAStreamWriteCallback(pa_stream* s, size_t requested_bytes, void* userData)
	{
		return ((TPulseAudioSoundManager*) userData)->PAStreamWriteCallback(s, (unsigned) requested_bytes);
	}
	void PAStreamWriteCallback(pa_stream* s, unsigned int requested_bytes);

  // Generic stream  operation callback function.
	static void SPAStreamOpCB(pa_stream* stream, int success, void* userData)
	{
		return ((TPulseAudioSoundManager*) userData)->PAStreamOpCB(stream, success, ((TPulseAudioSoundManager*) userData)->mPAMainLoop);
	}
	void PAStreamOpCB(pa_stream* s, int success, pa_threaded_mainloop* mainloop);

  // Stream Drain operation callback.  Called when PA has played everything in the buffer.
  static void SPAStreamDrainedCB(pa_stream* stream, int success, void* userData)
  {
    return ((TPulseAudioSoundManager*) userData)->PAStreamOpCB(stream, success, ((TPulseAudioSoundManager*) userData)->mPAMainLoop);
  }
  void PAStreamDrainedCB(pa_stream* s, int success, pa_threaded_mainloop* mainloop);

  // Post-uncork operation callback.  Trigger immediate output.
  static void SPATriggerAfterOpCB(pa_stream* stream, int success, void* userData)
  {
    return ((TPulseAudioSoundManager*) userData)->PATriggerAfterOpCB(stream, success, ((TPulseAudioSoundManager*) userData)->mPAMainLoop);
  }
  void PATriggerAfterOpCB(pa_stream* s, int success, pa_threaded_mainloop* mainloop);

	// static void PAStreamSuccessCallback(pa_stream* stream, int success, void* userData)
	// {
	// 	return pa_threaded_mainloop_signal((pa_threaded_mainloop*) userData, 0);
	// }

	/// \name Variables
	pa_operation* mPAOperation;
	char const* mPAOperationDescr;
  TCircleBuffer* mOutputBuffer;
  TMutex* mDataMutex;
	pa_stream* mOutputStream;
	pa_threaded_mainloop* mPAMainLoop;
	pa_context* mPAContext;
};

#endif
// _TPulseAudioSoundManager_H

// ========================================= //
// Bugs come in through open Windows.        //
// ========================================= //
