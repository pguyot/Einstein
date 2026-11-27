// ==============================
// File:			TCoreAudioSoundManager.cp
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

#include "TCoreAudioSoundManager.h"

// ANSI C & POSIX
#include <algorithm>
#include <stdio.h>
#include <strings.h>

#include <TargetConditionals.h>

// CoreServices
#if TARGET_OS_IOS
#include <CFNetwork/CFNetwork.h>
#else
#include <CoreServices/CoreServices.h>
#endif

#include <dispatch/dispatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// K
#include <K/Defines/UByteSex.h>
#include <K/Misc/TCircleBuffer.h>

// Einstein.
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#define kNewtonBufferSizeInFrames 0x750
#define kNewtonSampleRate 22050
#define kNewtonFormatID kAudioFormatLinearPCM
#define kNewtonFormatFlags (kLinearPCMFormatFlagIsSignedInteger \
	| kLinearPCMFormatFlagIsBigEndian                           \
	| kLinearPCMFormatFlagIsPacked                              \
	| kAudioFormatFlagIsNonInterleaved)
#define kNewtonBytesPerFrame 2
#define kBytesInAPacket kNewtonBytesPerFrame
#define kNewtonNumChannels 1
#define kNewtonBitsPerChannel 16
#define kFramesPerPacket 1

// How long the output unit keeps running (and plays silence) after the last
// sound. Starting the unit takes a few milliseconds of a sound, which is all
// of a tap.
#define kStopDelayInSeconds 1

// Set EINSTEIN_SOUND_TRACE to see the calls of NewtonOS's sound driver and
// the output unit with their times.
static bool sTrace = (getenv("EINSTEIN_SOUND_TRACE") != nullptr);

static void
Trace(const char* inWhat, long inValue = -1, long inValue2 = -1)
{
	if (!sTrace)
		return;
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	fprintf(stderr, "sound %6ld.%03ld %s", (long) (now.tv_sec % 100000), now.tv_nsec / 1000000, inWhat);
	if (inValue >= 0)
		fprintf(stderr, " %ld", inValue);
	if (inValue2 >= 0)
		fprintf(stderr, " %ld", inValue2);
	fprintf(stderr, "\n");
}

// -------------------------------------------------------------------------- //
//  * TCoreAudioSoundManager( void )
// -------------------------------------------------------------------------- //
TCoreAudioSoundManager::TCoreAudioSoundManager(TLog* inLog /* = nullptr */) :
		TBufferedSoundManager(inLog),
		mOutputBuffer(new TCircleBuffer(
			kNewtonBufferSizeInFrames * 4 * sizeof(KUInt16)))
{
	dispatch_queue_t theQueue = dispatch_queue_create("Einstein sound stop", DISPATCH_QUEUE_SERIAL);
	dispatch_source_t theTimer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, theQueue);
	dispatch_set_context(theTimer, this);
	dispatch_source_set_event_handler_f(theTimer, SStopWhenDrained);
	dispatch_source_set_timer(theTimer, DISPATCH_TIME_FOREVER, DISPATCH_TIME_FOREVER, 0);
	dispatch_resume(theTimer);
	mStopQueue = theQueue;
	mStopTimer = theTimer;
	CreateDefaultAU();
}

// -------------------------------------------------------------------------- //
//  * ~TCoreAudioSoundManager( void )
// -------------------------------------------------------------------------- //
TCoreAudioSoundManager::~TCoreAudioSoundManager(void)
{
	// No more render callbacks, and no StopWhenDrained() left to run: the
	// empty call on the queue waits for one that runs now.
	(void) AudioOutputUnitStop(mOutputUnit);
	dispatch_source_cancel((dispatch_source_t) mStopTimer);
	dispatch_sync_f((dispatch_queue_t) mStopQueue, nullptr, [](void*) {});
	dispatch_release((dispatch_source_t) mStopTimer);
	dispatch_release((dispatch_queue_t) mStopQueue);

#if TARGET_OS_IOS
	AudioComponentInstanceDispose(mOutputUnit);
#else
	AudioComponentInstanceDispose(mOutputUnit);
	AudioUnitUninitialize(mOutputUnit);
#endif

	delete mOutputBuffer;
}

// -------------------------------------------------------------------------- //
//  * CreateDefaultAU( void )
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::CreateDefaultAU(void)
{
	OSStatus err = noErr;

	do
	{
		// Open the default output unit
		AudioComponentDescription desc;

		desc.componentType = kAudioUnitType_Output;
#if TARGET_OS_IOS
		desc.componentSubType = kAudioUnitSubType_RemoteIO;
#else
		desc.componentSubType = kAudioUnitSubType_DefaultOutput;
#endif
		desc.componentManufacturer = kAudioUnitManufacturer_Apple;
		desc.componentFlags = 0;
		desc.componentFlagsMask = 0;

		AudioComponent comp = AudioComponentFindNext(NULL, &desc);

		if (comp == NULL)
		{
			if (GetLog())
			{
				GetLog()->LogLine(
					"CreateDefaultAU: FindNextComponent returned null");
			}
			break;
		}

		err = AudioComponentInstanceNew(comp, &mOutputUnit);

		if (err != noErr)
		{
			if (GetLog())
			{
				GetLog()->FLogLine(
					"CreateDefaultAU: OpenAComponent returned an error (%i)",
					err);
			}
			break;
		}

		// Set up a callback function to generate output to the output unit
		AURenderCallbackStruct input;
		input.inputProc = SRenderCallback;
		input.inputProcRefCon = this;

		err = AudioUnitSetProperty(mOutputUnit,
			kAudioUnitProperty_SetRenderCallback,
			kAudioUnitScope_Input,
			0,
			&input,
			sizeof(input));
		if (err != noErr)
		{
			if (GetLog())
			{
				GetLog()->FLogLine(
					"CreateDefaultAU: AudioUnitSetProperty(CB) returned an error (%i)",
					err);
			}
			break;
		}

		// We tell the Output Unit what format we're going to supply data to it
		// this is necessary if you're providing data through an input callback
		// AND you want the DefaultOutputUnit to do any format conversions
		// necessary from your format to the device's format.
		AudioStreamBasicDescription streamFormat;
		streamFormat.mSampleRate = kNewtonSampleRate; //	the sample rate of the audio stream
		streamFormat.mFormatID = kNewtonFormatID; //	the specific encoding type of audio
												  // stream
		streamFormat.mFormatFlags = kNewtonFormatFlags; //	flags specific to each format
		streamFormat.mBytesPerPacket = kBytesInAPacket;
		streamFormat.mFramesPerPacket = kFramesPerPacket;
		streamFormat.mBytesPerFrame = kNewtonBytesPerFrame;
		streamFormat.mChannelsPerFrame = kNewtonNumChannels;
		streamFormat.mBitsPerChannel = kNewtonBitsPerChannel;

		err = AudioUnitSetProperty(mOutputUnit,
			kAudioUnitProperty_StreamFormat,
			kAudioUnitScope_Input,
			0,
			&streamFormat,
			sizeof(AudioStreamBasicDescription));
		if (err != noErr)
		{
			if (GetLog())
			{
				GetLog()->FLogLine(
					"CreateDefaultAU: AudioUnitSetProperty(SF) returned an error (%i)",
					err);
			}
			break;
		}

		// Initialize unit
		err = AudioUnitInitialize(mOutputUnit);
		if (err != noErr)
		{
			if (GetLog())
			{
				GetLog()->FLogLine(
					"CreateDefaultAU: AudioUnitInitialize returned an error (%i)",
					err);
			}
			break;
		}
	} while (false);
}

// -------------------------------------------------------------------------- //
//  * RenderCallback( ... )
// -------------------------------------------------------------------------- //
OSStatus
TCoreAudioSoundManager::RenderCallback(
	AudioUnitRenderActionFlags* ioActionFlags,
	const AudioTimeStamp* inTimeStamp,
	UInt32 inBusNumber,
	UInt32 inNumberFrames,
	AudioBufferList* ioData)
{
	(void) ioActionFlags;
	(void) inTimeStamp;
	(void) inBusNumber;

	// Rate limiter - only request bytes from NewtonOS when bytes are needed
	// Otherewise we exhaust the buffers too quickly and truncate the sounds.
	KUIntPtr bytesInBuffer = 0;
	{
		std::lock_guard<std::mutex> lock(mDataMutex);
		bytesInBuffer = mOutputBuffer->AvailableBytes();
	}

	// Real hardware raises the interrupt when a buffer is played, this one as
	// soon as there is room for the next. So NewtonOS stops the output while
	// samples are still queued (all of a short sound), and must not be asked
	// for more then.
	if (!mStopping && bytesInBuffer < kNewtonBufferSize)
	{
		// Ask for more data.
		Trace("interrupt, queued", (long) bytesInBuffer);
		RaiseOutputInterrupt();
	}

	// Copy data from the circle buffer.
	KUIntPtr wanted = inNumberFrames * sizeof(KSInt16);
	KUIntPtr amount = std::min(bytesInBuffer, wanted);
	KUIntPtr available = 0;
	bool drained = false;
	{
		std::lock_guard<std::mutex> lock(mDataMutex);
		available = mOutputBuffer->Consume(
			ioData->mBuffers[0].mData,
			amount);
		drained = mOutputBuffer->IsEmpty();
	}

	if (available > 0 && drained)
		Trace("played out");

	// Stopped and played out: stop the unit after a while, so the next sound
	// starts at once, and not from its own render callback.
	if (mStopping && drained && !mStopScheduled.exchange(true))
	{
		mStopStartCount = mStartCount.load();
		dispatch_source_set_timer((dispatch_source_t) mStopTimer,
			dispatch_time(DISPATCH_TIME_NOW, kStopDelayInSeconds * NSEC_PER_SEC),
			DISPATCH_TIME_FOREVER, 10 * NSEC_PER_MSEC);
	}

	// Zero the remaining of the buffer: it still holds what it held at the
	// last callback, which would play again.
	if (available < wanted)
	{
		::bzero(&((KUInt8*) ioData->mBuffers[0].mData)[available], wanted - available);
	}

	return noErr;
}

// -------------------------------------------------------------------------- //
//  * ScheduleOutput( const KUInt8*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::ScheduleOutput(const KUInt8* inBuffer, KUInt32 inSize)
{
	if (inSize > 0)
	{
		// Write to the output buffer.
		// Copy data.
		KUIntPtr queued = 0;
		{
			std::lock_guard<std::mutex> lock(mDataMutex);
			mOutputBuffer->Produce(inBuffer, inSize);
			queued = mOutputBuffer->AvailableBytes();
		}
		Trace("ScheduleOutput, now queued", (long) inSize, (long) queued);
		// Ask for more data.
		// RaiseOutputInterrupt();
	}
}

// -------------------------------------------------------------------------- //
//  * StartOutput( void )
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::StartOutput(void)
{
	std::lock_guard<std::mutex> runLock(mRunMutex);
	Trace("StartOutput");
	mStopping = false;
	mStartCount++;

	// Set the volume before starting to render the sound
	OutputVolumeChanged();

	// Start the rendering
	// The DefaultOutputUnit will do any format conversions to the format of the
	// default device
	OSStatus err = AudioOutputUnitStart(mOutputUnit);
	if (err != noErr)
	{
		if (GetLog())
		{
			GetLog()->FLogLine(
				"CreateDefaultAU: AudioOutputUnitStart returned an error (%i)",
				err);
		}
	}
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
//		The samples that are queued still play: NewtonOS stops the output when
//		it has no more data, which with our early interrupts is before the last
//		buffer is played. The render callback stops the unit a while after
//		they are, see StopWhenDrained().
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::StopOutput(void)
{
	std::lock_guard<std::mutex> runLock(mRunMutex);
	KUIntPtr queued = 0;
	{
		std::lock_guard<std::mutex> dataLock(mDataMutex);
		queued = mOutputBuffer->AvailableBytes();
	}
	Trace("StopOutput, queued", (long) queued);
	mStopping = true;
}

// -------------------------------------------------------------------------- //
//  * SStopWhenDrained( void* )
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::SStopWhenDrained(void* inManager)
{
	((TCoreAudioSoundManager*) inManager)->StopWhenDrained();
}

// -------------------------------------------------------------------------- //
//  * StopWhenDrained( void )
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::StopWhenDrained(void)
{
	std::lock_guard<std::mutex> runLock(mRunMutex);
	mStopScheduled = false;
	bool drained = false;
	{
		std::lock_guard<std::mutex> dataLock(mDataMutex);
		drained = mOutputBuffer->IsEmpty();
	}
	// Not if NewtonOS started a sound in the meantime: then the render
	// callback schedules another stop when that one is played.
	if (mStopping && drained && (mStartCount == mStopStartCount))
	{
		Trace("output unit stopped");
		(void) AudioOutputUnitStop(mOutputUnit);
	}
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
bool
TCoreAudioSoundManager::OutputIsRunning(void)
{
	return !mOutputBuffer->IsEmpty();
}

// -------------------------------------------------------------------------- //
//  * OutputVolumeChanged() override
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::OutputVolumeChanged()
{
	// Adjust the sound if there is an AudioUnit
	if (mOutputUnit)
	{
		AudioUnitSetParameter(mOutputUnit,
			kHALOutputParam_Volume,
			kAudioUnitScope_Output,
			0,
			OutputVolumeNormalized(), 0);
	}
}

// ====================================================== //
// The world is coming to an end ... SAVE YOUR BUFFERS!!! //
// ====================================================== //
