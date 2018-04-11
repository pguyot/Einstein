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
#include <pulse/pulseaudio.h>
#include <pulse/error.h>

// Einstein
#include "TBufferedSoundManager.h"

class TMutex;

///
/// Class to handle sound input/output and direct them to PulseAudio.
///
/// \author Victor Rehorst <victor@chuma.org>
/// \version $Revision: 111 $
///
/// \test	aucun test d�fini.
///
class TPulseAudioSoundManager
	:
		public TBufferedSoundManager
{
public:
	///
	/// Constructor from a log.
	///
	TPulseAudioSoundManager( TLog* inLog = nil );

	///
	/// Destructor.
	///
	virtual ~TPulseAudioSoundManager( void );

	///
	/// Schedule output of some buffer.
	///
	virtual void	ScheduleOutput( const KUInt8* inBufferAddr, KUInt32 inSize );

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

    // callback routines (static)
    static void SPAContextStateCallback(pa_context* context, void* userData)
    {
        return ((TPulseAudioSoundManager*) userData)->PAContextStateCallback(context, ((TPulseAudioSoundManager*) userData)->mPAMainLoop);
    }

    static void SPAStreamStateCallback(pa_stream* s, void* userData)
    {
        return ((TPulseAudioSoundManager*) userData)->PAStreamStateCallback(s, ((TPulseAudioSoundManager*) userData)->mPAMainLoop);
    }

    static void SPAStreamUnderflowCallback(pa_stream* s, void* userData)
    {
        return ((TPulseAudioSoundManager*) userData)->PAStreamUnderflowCallback(s, ((TPulseAudioSoundManager*) userData)->mPAMainLoop);
    }

    static void SPAStreamWriteCallback(pa_stream* s, size_t requested_bytes, void* userData)
    {
        return ((TPulseAudioSoundManager*) userData)->PAStreamWriteCallback(s, requested_bytes);
    }

    void PAContextStateCallback(pa_context* context, pa_threaded_mainloop* mainloop);

    void PAStreamStateCallback(pa_stream* s, pa_threaded_mainloop* mainloop);
    void PAStreamUnderflowCallback(pa_stream* s, pa_threaded_mainloop* mainloop);

    void PAStreamWriteCallback(pa_stream* s, unsigned int requested_bytes);

    static void SPAStreamOpCB(pa_stream *stream, int success, void *userData)
    {
        return ((TPulseAudioSoundManager*) userData)->PAStreamOpCB(stream, success, ((TPulseAudioSoundManager*) userData)->mPAMainLoop);
    }

    void PAStreamOpCB(pa_stream* s, int success, pa_threaded_mainloop* mainloop);

    static void PAStreamSuccessCallback(pa_stream *stream, int success, void *userData)
    {
        return pa_threaded_mainloop_signal((pa_threaded_mainloop*) userData, 0);
    }

	/// \name Variables
    pa_operation*           mPAOperation;
    char*                   mPAOperationDescr;
    pa_stream*              mOutputStream;
    pa_threaded_mainloop*   mPAMainLoop;
    pa_mainloop_api*        mPAMainLoopAPI;
    pa_context*             mPAContext;
    TMutex*                 mDataMutex;
	Boolean			        mOutputIsRunning;
};

#endif
// _TPulseAudioSoundManager_H

// ========================================= //
// Bugs come in through open Windows.        //
// ========================================= //
