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
#include <strings.h>
#include <stdio.h>

// CoreServices
#if TARGET_IOS
#import <CFNetwork/CFNetwork.h>
#else
#import <CoreServices/CoreServices.h>
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

#define kNewtonBufferSizeInFrames		0x750
#define kNewtonSampleRate				22050
#define kNewtonFormatID					kAudioFormatLinearPCM
#define kNewtonFormatFlags				(kLinearPCMFormatFlagIsSignedInteger \
										| kLinearPCMFormatFlagIsBigEndian \
										| kLinearPCMFormatFlagIsPacked \
										| kAudioFormatFlagIsNonInterleaved)
#define kNewtonBytesPerFrame			2
#define kBytesInAPacket					kNewtonBytesPerFrame
#define kNewtonNumChannels				1
#define kNewtonBitsPerChannel			16
#define kFramesPerPacket				1

// -------------------------------------------------------------------------- //
//  * TCoreAudioSoundManager( void )
// -------------------------------------------------------------------------- //
TCoreAudioSoundManager::TCoreAudioSoundManager( TLog* inLog /* = nil */ )
	:
		TBufferedSoundManager( inLog ),
        mOutputBuffer( new TCircleBuffer(
								kNewtonBufferSizeInFrames *
								4 * sizeof(KUInt16) ) ),
		mDataMutex( new TMutex() ),
        mOutputUnit(0L)
{
	mDataMutex = new TMutex();
	CreateDefaultAU();
}

// -------------------------------------------------------------------------- //
//  * ~TCoreAudioSoundManager( void )
// -------------------------------------------------------------------------- //
TCoreAudioSoundManager::~TCoreAudioSoundManager( void )
{
#if TARGET_IOS
	AudioComponentInstanceDispose( mOutputUnit );
#else
	AudioComponentInstanceDispose( mOutputUnit );
	AudioUnitUninitialize( mOutputUnit );
#endif

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
//  * CreateDefaultAU( void )
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::CreateDefaultAU( void )
{
	OSStatus err = noErr;
	
	do {
		// Open the default output unit
		AudioComponentDescription desc;

		desc.componentType = kAudioUnitType_Output;
#if TARGET_IOS
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
					"CreateDefaultAU: FindNextComponent returned null" );
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
					err );
			}
			break;
		}

		// Set up a callback function to generate output to the output unit
		AURenderCallbackStruct input;
		input.inputProc = SRenderCallback;
		input.inputProcRefCon = this;
	
		err = AudioUnitSetProperty( mOutputUnit, 
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
					err );
			}
			break;
		}

		// We tell the Output Unit what format we're going to supply data to it
		// this is necessary if you're providing data through an input callback
		// AND you want the DefaultOutputUnit to do any format conversions
		// necessary from your format to the device's format.
		AudioStreamBasicDescription streamFormat;
			streamFormat.mSampleRate = kNewtonSampleRate;	//	the sample rate of the audio stream
			streamFormat.mFormatID = kNewtonFormatID;		//	the specific encoding type of audio
															// stream
			streamFormat.mFormatFlags = kNewtonFormatFlags;	//	flags specific to each format
			streamFormat.mBytesPerPacket = kBytesInAPacket;	
			streamFormat.mFramesPerPacket = kFramesPerPacket;	
			streamFormat.mBytesPerFrame = kNewtonBytesPerFrame;		
			streamFormat.mChannelsPerFrame = kNewtonNumChannels;	
			streamFormat.mBitsPerChannel = kNewtonBitsPerChannel;	

		err = AudioUnitSetProperty( mOutputUnit,
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
					err );
			}
			break;
		}

		// Initialize unit
		err = AudioUnitInitialize( mOutputUnit );
		if (err != noErr)
		{
			if (GetLog())
			{
				GetLog()->FLogLine(
					"CreateDefaultAU: AudioUnitInitialize returned an error (%i)",
					err );
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
		AudioBufferList* ioData )
{
	
	// Rate limiter - only request bytes from NewtonOS when bytes are needed
	// Otherewise we exhaust the buffers too quickly and truncate the sounds.
	mDataMutex->Lock();
	KUIntPtr bytesInBuffer = mOutputBuffer->AvailableBytes();
	mDataMutex->Unlock();
	
	if (bytesInBuffer < kNewtonBufferSize) {
		// Ask for more data.
		RaiseOutputInterrupt();
	}

	// Copy data from the circle buffer.
	mDataMutex->Lock();
	KUIntPtr amount = MIN(bytesInBuffer, inNumberFrames * sizeof( KSInt16 ));
	// TODO: find possible error in the ringBuffer implementation?
	
	KUIntPtr available =
		mOutputBuffer->Consume(
			ioData->mBuffers[0].mData,
			amount );
	mDataMutex->Unlock();
	
	KUIntPtr delta = amount - available;
	if (delta > 0)
	{
		// Zero the remaining of the buffer.
		::bzero( &((KUInt8*) ioData->mBuffers[0].mData)[available], delta );
	}
		
	return noErr;
}

// -------------------------------------------------------------------------- //
//  * ScheduleOutput( const KUInt8*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::ScheduleOutput( const KUInt8* inBuffer, KUInt32 inSize )
{
	if (inSize > 0)
    {
        // Write to the output buffer.
        // Copy data.
        mDataMutex->Lock();
        mOutputBuffer->Produce( inBuffer, inSize );
        mDataMutex->Unlock();
		// Ask for more data.
		//RaiseOutputInterrupt();
	}
}

// -------------------------------------------------------------------------- //
//  * StartOutput( void )
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::StartOutput( void )
{
    // Set the volume before starting to render the sound
    OutputVolumeChanged();

	// Start the rendering
	// The DefaultOutputUnit will do any format conversions to the format of the
	// default device
	OSStatus err = AudioOutputUnitStart( mOutputUnit );
	if (err != noErr)
	{
		if (GetLog())
		{
			GetLog()->FLogLine(
				"CreateDefaultAU: AudioOutputUnitStart returned an error (%i)",
				err );
		}
	}
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
// -------------------------------------------------------------------------- //
void
TCoreAudioSoundManager::StopOutput( void )
{
	// Stop the rendering
	OSStatus err = AudioOutputUnitStop( mOutputUnit );
	if (err != noErr)
	{
		if (GetLog())
		{
			GetLog()->FLogLine(
				"CreateDefaultAU: AudioOutputUnitStart returned an error (%i)",
				err );
		}
	}
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
Boolean
TCoreAudioSoundManager::OutputIsRunning( void )
{
	return !mOutputBuffer->IsEmpty();
}


// -------------------------------------------------------------------------- //
//  * OutputVolumeChanged() override
// -------------------------------------------------------------------------- //
void TCoreAudioSoundManager::OutputVolumeChanged()
{
    // Adjust the sound if there is an AudioUnit
    if (mOutputUnit) {
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
