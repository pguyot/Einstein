// ==============================
// File:			TAndroidNativeSoundManager.cp
// Project:			Einstein
//
// Copyright 2003-2013 by Paul Guyot (pguyot@kallisys.net) and Matthias Melcher.
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
#include "TAndroidNativeSoundManager.h"
#include "Log/TLog.h"

#if 0

// ANSI C & POSIX
#include <strings.h>
#include <stdio.h>

// K
#include <K/Defines/UByteSex.h>
#include <K/Misc/TCircleBuffer.h>
#include <K/Threads/TMutex.h>

// Einstein.
#include "Emulator/Log/TLog.h"


// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#define kNewtonBufferSizeInFrames		0x750


// -------------------------------------------------------------------------- //
// Static Member Variables
// -------------------------------------------------------------------------- //
TCircleBuffer* TAndroidNativeSoundManager::mOutputBuffer = 0L;
TMutex* TAndroidNativeSoundManager::mDataMutex = 0L;
int TAndroidNativeSoundManager::mGlobalVolume = 50; // TODO: store between system starts
int TAndroidNativeSoundManager::mGlobalVolumeChanged = 1;


// -------------------------------------------------------------------------- //
//  * TAndroidSoundManager( void )
// -------------------------------------------------------------------------- //
TAndroidNativeSoundManager::TAndroidNativeSoundManager( TLog* inLog /* = nil */ )
:
TBufferedSoundManager( inLog )
{
  if (!mOutputBuffer) {
    mOutputBuffer =
      new TCircleBuffer(kNewtonBufferSizeInFrames * 4 * sizeof(KUInt16) );
  }
  if (!mDataMutex) {
    mDataMutex = new TMutex();
  }
}

// -------------------------------------------------------------------------- //
//  * ~TAndroidSoundManager( void )
// -------------------------------------------------------------------------- //
TAndroidNativeSoundManager::~TAndroidNativeSoundManager( void )
{
}

// -------------------------------------------------------------------------- //
//  * ScheduleOutput( const KUInt8*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TAndroidNativeSoundManager::ScheduleOutput( const KUInt8* inBuffer, KUInt32 inSize )
{
  if (inSize > 0)
  {
    if (OutputVolume() != kOutputVolume_Zero)
    {
      // Write to the output buffer.
      // Copy data.
      mDataMutex->Lock();
      mOutputBuffer->Produce( inBuffer, inSize );
      mDataMutex->Unlock();
    }
    
    // Ask for more data.
    RaiseOutputInterrupt();
  }
}

// -------------------------------------------------------------------------- //
//  * StartOutput( void )
// -------------------------------------------------------------------------- //
void
TAndroidNativeSoundManager::StartOutput( void )
{
  // Start the rendering
  // The DefaultOutputUnit will do any format conversions to the format of the
  // default device
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
// -------------------------------------------------------------------------- //
void
TAndroidNativeSoundManager::StopOutput( void )
{
  // Start the rendering
  // The DefaultOutputUnit will do any format conversions to the format of the
  // default device
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
Boolean
TAndroidNativeSoundManager::OutputIsRunning( void )
{
  return !mOutputBuffer->IsEmpty();
}


bool TAndroidNativeSoundManager::soundOutputDataAvailable()
{
  return !mOutputBuffer->IsEmpty();
}


int TAndroidNativeSoundManager::soundOutputBytesAvailable()
{
  return mOutputBuffer->AvailableBytes();
}

int TAndroidNativeSoundManager::soundOutputBytesCopy(signed short *dst, int max)
{
  int n = soundOutputBytesAvailable();
  if (n>max) n = max;
  mOutputBuffer->Consume((void*)dst, n);
	int i;
	for (i=0; i<n/2; i++) {
		dst[i] = dst[i]<<8;
	}
  return n;
}

void
TAndroidNativeSoundManager::OutputVolumeChanged()
{
	KUInt32 v = OutputVolume();
	int vNew = 0;
	if (v==kOutputVolume_Zero || v<kOutputVolume_Min) {
		vNew = 0;
	} else if (v==kOutputVolume_Max) {
		vNew = 100;
	} else {
		vNew = (v-kOutputVolume_Min) / 19458;
	}
	
	if (mGlobalVolume != vNew)
		mGlobalVolumeChanged = 1;
	mGlobalVolume = vNew;
}

int
TAndroidNativeSoundManager::soundVolumeChanged()
{
  int tmp = mGlobalVolumeChanged;
  mGlobalVolumeChanged = 0;
  return tmp;
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
