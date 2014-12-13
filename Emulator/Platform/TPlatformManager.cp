// ==============================
// File:			TPlatformManager.cp
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

#include <K/Defines/KDefinitions.h>
#include "TPlatformManager.h"

// ANSI C & POSIX
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

// K
#include <K/Unicode/UUTF16CStr.h>
#include <K/Threads/TMutex.h>

// Einstein
#include "TInterruptManager.h"
#include "TMemory.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/Host/THostInfo.h"
#include "Emulator/Screen/TScreenManager.h"
#include "Emulator/PCMCIA/TPCMCIAController.h"
#include "Emulator/PCMCIA/TNE2000Card.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TPlatformManager( TLog*, TScreenManager* )
// -------------------------------------------------------------------------- //
TPlatformManager::TPlatformManager(
			TLog* inLog,
			TScreenManager* inScreenManager)
	:
		mLog( inLog ),
		mScreenManager(inScreenManager),
		mInterruptManager( nil ),
		mMemory( nil ),
		mEventQueue( nil ),
		mEventQueueCCrsr( 0 ),
		mEventQueuePCrsr( 0 ),
		mEventQueueSize( kDEFAULTEVENTQUEUESIZE ),
		mBufferQueue( nil ),
		mBufferCount( 0 ),
		mBufferQueueSize( kDEFAULTBUFFERQUEUESIZE ),
		mBufferNextID( 0 ),
		mPowerOn( true ),
		mQueueLockCount( 0 ),
		mMutex( nil ),
		mDocDir( nil )
{
	mEventQueue = (SEvent*) ::malloc( sizeof(SEvent) * mEventQueueSize );
	mBufferQueue = (SBuffer*) ::malloc( sizeof(SBuffer) * mBufferQueueSize );
	
	mMutex = new TMutex();
}

// -------------------------------------------------------------------------- //
//  * ~TPlatformManager( void )
// -------------------------------------------------------------------------- //
TPlatformManager::~TPlatformManager( void )
{
	if (mDocDir)
	{
		::free(mDocDir);
	}
	
	if (mEventQueue)
	{
		::free(mEventQueue);
	}
	if (mBufferQueue)
	{
		KUInt32 indexQueue;
		for (indexQueue = 0; indexQueue < mBufferCount; indexQueue++)
		{
			::free((void*) mBufferQueue[indexQueue].fData);
		}
		::free(mBufferQueue);
	}
	
	if (mMutex)
	{
		delete mMutex;
	}
}

// -------------------------------------------------------------------------- //
//  * GetNextEvent( KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TPlatformManager::GetNextEvent( KUInt32 outEventPAddr )
{
	Boolean theResult = false;

	mMutex->Lock();

	KUInt32 newCCrsr = mEventQueueCCrsr;
	if (newCCrsr != mEventQueuePCrsr)
	{
		theResult = true;
		SEvent* theEvent = &mEventQueue[newCCrsr++];
		mEventQueueCCrsr = newCCrsr;
		
		// Copy event to Newton memory.
		KUInt32 theSize = 4;
		switch (theEvent->fType)
		{
			case kAEvent:
				theSize += 8 + theEvent->fData.aevent.fSize;
				break;
		}
		
		if (theSize & 0x03)
		{
			theSize = (theSize / 4) + 1;
		} else {
			theSize = theSize / 4;
		}
		
		KUInt32* theEventAsLongs = (KUInt32*) theEvent;
		while (theSize-- > 0)
		{
			(void) mMemory->WriteP(
					outEventPAddr,
					*theEventAsLongs++ );
			outEventPAddr += 4;
		}
	} else {
		// Reached the end.
		// Reset the cursors.
		mEventQueuePCrsr = 0;
		mEventQueueCCrsr = 0;
		
		// Redimension the queue.
		if (mEventQueueSize != kDEFAULTEVENTQUEUESIZE)
		{
			mEventQueueSize = kDEFAULTEVENTQUEUESIZE;
			mEventQueue = 
				(SEvent*) ::realloc(
							mEventQueue,
							sizeof(SEvent) * mEventQueueSize );
		}
	}
	
	mMutex->Unlock();

	return theResult;
}

// -------------------------------------------------------------------------- //
//  * DisposeBuffer( KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TPlatformManager::DisposeBuffer( KUInt32 inID )
{
	Boolean theResult = false;

	mMutex->Lock();

	KUInt32 bufferIndex;
	for (bufferIndex = 0; bufferIndex < mBufferCount; bufferIndex++)
	{
		SBuffer* theBuffer = &mBufferQueue[bufferIndex];
		if (theBuffer->fID == inID)
		{
			// Free the buffer up.
			::free((void*) theBuffer->fData);
			
			// Remove the buffer from the queue.
			::memmove(
				&mBufferQueue[bufferIndex],
				(const void*) &mBufferQueue[bufferIndex+1],
				(mBufferCount - bufferIndex) * sizeof(SBuffer));
			mBufferCount--;
			
			// We're done.
			theResult = true;
			break;
		}
	}

	mMutex->Unlock();

	return theResult;
}

// -------------------------------------------------------------------------- //
//  * CopyBufferData( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
Boolean
TPlatformManager::CopyBufferData(
						KUInt32 inID,
						KUInt32 outVAddress,
						KUInt32 inOffset,
						KUInt32 inAmount)
{
	Boolean theResult = false;

	mMutex->Lock();

	KUInt32 bufferIndex;
	for (bufferIndex = 0; bufferIndex < mBufferCount; bufferIndex++)
	{
		SBuffer* theBuffer = &mBufferQueue[bufferIndex];
		if (theBuffer->fID == inID)
		{
			// Check the bounds.
			if (theBuffer->fSize < inOffset + inAmount)
			{
				break;
			}
			// Copy the data.
			if (mMemory->FastWriteBuffer(
					outVAddress,
					inAmount,
					&theBuffer->fData[inOffset]))
			{
				break;
			}
			
			// We're done.
			theResult = true;
			break;
		}
	}

	mMutex->Unlock();

	return theResult;
}

// -------------------------------------------------------------------------- //
//  * AddBuffer( KUInt32, const KUInt8* )
// -------------------------------------------------------------------------- //
KUInt32
TPlatformManager::AddBuffer( KUInt32 inSize, const KUInt8* inData )
{
	mMutex->Lock();

	// Get a new ID.
	KUInt32 theID = mBufferNextID++;

	// Copy the buffer.
	KUInt8* theData = (KUInt8*) ::malloc(inSize);
	::memcpy(theData, inData, inSize);
	
	// Increase the count.
	mBufferCount++;
	if (mBufferCount == mBufferQueueSize)
	{
		// Increase the queue size.
		mBufferQueueSize += kBUFFERQUEUESIZEINCREMENT;
		mBufferQueue = (SBuffer*) ::realloc(
										mBufferQueue,
										mBufferQueueSize * sizeof(SBuffer));
	}

	// Save the record.
	SBuffer* theNewRecord = &mBufferQueue[mBufferCount - 1];
	theNewRecord->fID = theID;
	theNewRecord->fData = theData;
	theNewRecord->fSize = inSize;

	mMutex->Unlock();

	return theID;
}

// -------------------------------------------------------------------------- //
//  * SendAEvent( EPort, KUInt32, const KUInt8* )
// -------------------------------------------------------------------------- //
void
TPlatformManager::SendAEvent( EPort inPortId, KUInt32 inSize, const KUInt8* inData )
{
	mMutex->Lock();

	KUInt32 newPCrsr = mEventQueuePCrsr;
	SEvent* theEvent = &mEventQueue[newPCrsr++];
	theEvent->fType = kAEvent;
	theEvent->fData.aevent.fPort = inPortId;
	theEvent->fData.aevent.fSize = inSize;
	(void) ::memcpy( theEvent->fData.aevent.fData, inData, inSize );
	if (newPCrsr == mEventQueueSize)
	{
		// Queue overflow.
		// Increase it.
		mEventQueueSize += kEVENTQUEUESIZEINCREMENT;
		mEventQueue =
			(SEvent*) ::realloc(
							mEventQueue,
							sizeof(SEvent) * mEventQueueSize );
	}
	mEventQueuePCrsr = newPCrsr;
	
	mMutex->Unlock();

	// Tell the driver.
	if (mQueueLockCount == 0)
	{
		RaisePlatformInterrupt();
	}
}

// -------------------------------------------------------------------------- //
//  * SendNetworkCardEvent( void )
// -------------------------------------------------------------------------- //
void
TPlatformManager::SendNetworkCardEvent( void )
{
	static TNE2000Card *theCard = NULL;

	// FIXME: Change check mark in Menu.
	if (mMemory) {
		TPCMCIAController *theController = mMemory->GetPCMCIAController(0);
		if (theController) {
			if (theCard==0L) {
				theCard = new TNE2000Card();
				theController->InsertCard(theCard);
			} else {
				theController->RemoveCard();
				theCard = NULL;
			}
		}
	}
}


// -------------------------------------------------------------------------- //
//  * SendPowerSwitchEvent( void )
// -------------------------------------------------------------------------- //
void
TPlatformManager::SendPowerSwitchEvent( void )
{
	if (mPowerOn)
	{
		KUInt32 thePowerSwitchEvent[3];
		thePowerSwitchEvent[0] = 'newt';
		thePowerSwitchEvent[1] = 'pg&e';
		thePowerSwitchEvent[2] = 'powr';
	
		SendAEvent( kPowerPort, sizeof( thePowerSwitchEvent ), (KUInt8*) thePowerSwitchEvent );
	} else {
		// Just wake the system with an interrupt.
		RaisePlatformInterrupt();
	}
}

// -------------------------------------------------------------------------- //
//  * SendBacklightEvent( void )
// -------------------------------------------------------------------------- //
void
TPlatformManager::SendBacklightEvent( void )
{
	KUInt32 theBacklightEvent[3];
	theBacklightEvent[0] = 'newt';
	theBacklightEvent[1] = 'pg&e';
	theBacklightEvent[2] = 'bklt';

	SendAEvent( kPowerPort, sizeof( theBacklightEvent ), (KUInt8*) theBacklightEvent );
}

// -------------------------------------------------------------------------- //
//  * SendKeyEvent( KUInt32, KUInt8, KUInt32 )
// -------------------------------------------------------------------------- //
void
TPlatformManager::SendKeyEvent( KUInt32 inType, KUInt8 inKeyCode, KUInt32 /* when */ )
{
	KUInt32 theKeyEvent[11];
	theKeyEvent[0] = 'newt';
	theKeyEvent[1] = 'idle';
	theKeyEvent[2] = 'keyb';
	theKeyEvent[3] = inType;
	theKeyEvent[4] = 0x00000001;	// ?
	theKeyEvent[5] = inKeyCode;
	theKeyEvent[6] = 0x00000001;	// ?
	theKeyEvent[7] = 0x00000000;	// ?
	theKeyEvent[8] = 0x00000000;	// ?
	theKeyEvent[9] = 0x00000000;	// ?
	theKeyEvent[10] = 0x00000000;	// ?

	SendAEvent( kNewtPort, sizeof( theKeyEvent ), (KUInt8*) theKeyEvent );
}

// -------------------------------------------------------------------------- //
//  * RaisePlatformInterrupt( void )
// -------------------------------------------------------------------------- //
void
TPlatformManager::RaisePlatformInterrupt( void )
{
	// Use the power switch interrupt.
	mInterruptManager->RaiseGPIO(0x00000001);
}

// -------------------------------------------------------------------------- //
//  * PowerOff( void )
// -------------------------------------------------------------------------- //
void
TPlatformManager::PowerOff( void )
{
	mPowerOn = false;
	if (mScreenManager)
	{
		mScreenManager->PowerOff();
	}
}

// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TPlatformManager::PowerOn( void )
{
	mPowerOn = true;
	if (mScreenManager)
	{
		mScreenManager->PowerOn();
	}
}

// -------------------------------------------------------------------------- //
//  * LockEventQueue( void )
// -------------------------------------------------------------------------- //
void
TPlatformManager::LockEventQueue( void )
{
	mQueueLockCount++;
}

// -------------------------------------------------------------------------- //
//  * UnlockEventQueue( void )
// -------------------------------------------------------------------------- //
void
TPlatformManager::UnlockEventQueue( void )
{
	if (mQueueLockCount != 0) {
		mQueueLockCount--;
		if (mQueueLockCount == 0) {
			if (mEventQueuePCrsr != mEventQueueCCrsr)
			{
				RaisePlatformInterrupt();
			}
		}
	}
}

// -------------------------------------------------------------------------- //
//  * GetUserInfo( EUserInfoSel, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TPlatformManager::GetUserInfo(
						EUserInfoSel inSelector,
						KUInt32 inBufferSize,
						KUInt32 outAddress ) const
{
	const KUInt16* theString =
		THostInfo::GetHostInfo()->GetUserInfo(inSelector);
	size_t theSize = (UUTF16CStr::StrLen( theString ) + 1) * sizeof(KUInt16);
	
	// Write the string.
	if (outAddress != 0)
	{
		if (inBufferSize < theSize)
		{
			theSize = inBufferSize;
		}
		mMemory->FastWriteBuffer(
			outAddress,
			(KUInt32) theSize,
			(const KUInt8*) theString);
	}
	
	return (KUInt32) theSize;
}

// -------------------------------------------------------------------------- //
//  * GetHostTimeZone( void )
// -------------------------------------------------------------------------- //
int
TPlatformManager::GetHostTimeZone( void ) const
{
	return THostInfo::GetHostInfo()->GetHostTimeZone();
}

// -------------------------------------------------------------------------- //
//  * SendBufferAEvent( EPort, KUInt32, KUInt32, KUInt32, KUInt32, const KUInt8* )
// -------------------------------------------------------------------------- //
void
TPlatformManager::SendBufferAEvent(
							EPort inPortId,
							KUInt32 inClass,
							KUInt32 inID,
							KUInt32 inDataClass,
							KUInt32 inSize,
							const KUInt8* inData )
{
	SEinsteinBufferEvent theEvent;
	theEvent.fEventClass = inClass;
	theEvent.fEventID = inID;
	theEvent.fDataClass = inDataClass;
	theEvent.fSize = inSize;
	theEvent.fBufferID = AddBuffer(inSize, inData);
	
	SendAEvent(
		inPortId,
		sizeof(theEvent),
		(const KUInt8*) &theEvent );
}

// -------------------------------------------------------------------------- //
//  * EvalNewtonScript( const char* )
// -------------------------------------------------------------------------- //
void
TPlatformManager::EvalNewtonScript( const char* inNewtonScriptCode )
{
	SendBufferAEvent(
		kNewtPort,
		kEinsteinNSEventClass,
		kEventRuntimeWithSData,
		kNewtonScriptEvalData,
		(KUInt32) ::strlen( inNewtonScriptCode ),
		(const KUInt8*) inNewtonScriptCode );
}

// -------------------------------------------------------------------------- //
//  * InstallPackage( const char* )
// -------------------------------------------------------------------------- //
void
TPlatformManager::InstallPackage( const char* inPackagePath )
{
	mLog->FLogLine("Installing package '%s'", inPackagePath);
	// Open the package.
	FILE* thePackageFile = ::fopen( inPackagePath, "rb" );
	if (thePackageFile != NULL)
	{
		// Read it entirely.
		(void) ::fseek( thePackageFile, 0, SEEK_END );
		long theSize = ::ftell( thePackageFile );
		::rewind( thePackageFile );
		
		KUInt8* theBuffer = (KUInt8*) ::malloc( theSize );
		if (((long) ::fread( theBuffer, sizeof(KUInt8), theSize, thePackageFile)) != theSize)
		{
			mLog->FLogLine( "Problem while reading '%s'", inPackagePath );
		} else {
			SendBufferAEvent(
				kNewtPort,
				kEinsteinNSEventClass,
				kEventRuntimeWithSData,
				kPackageInstallData,
				(KUInt32) theSize,
				theBuffer );
		}
		
		(void) ::fclose( thePackageFile );
		::free( theBuffer );
	} else {
		mLog->FLogLine( "Cannot open package '%s'", inPackagePath );
	}
}


// -------------------------------------------------------------------------- //
//  * InstallNewPackages( const char* )
// --------------------------------------------------------------------------
void
TPlatformManager::InstallNewPackages( const char* inPackageDir )
{
	// -- find the directory
	if ( !inPackageDir ) 
		inPackageDir = mDocDir;
		
	if ( !inPackageDir ) {
		mLog->FLogLine("TPlatformManager::InstallNewPackages: No package directory specified, skipping.");
		return;
	}
	
	DIR *dir = opendir(inPackageDir);
		
	if (!dir) {
		mLog->FLogLine("TPlatformManager::InstallNewPackages: Can't open package directory: '%s'", inPackageDir);
		return;
	}
	
	// -- find the last installation date
	char buf[2048];
	sprintf(buf, "%s/.lastInstall", inPackageDir);
	struct stat lastInstall;
	int statErr = ::stat(buf, &lastInstall);
	mLog->FLogLine("TPlatformManager: checking for new packages");
	
	// -- run the directory and install every file that is newer than lastInstall
	struct dirent *de;
	while ((de = readdir(dir))) {
		if (de->d_type==DT_REG) {
			const char *dot = strrchr(de->d_name, '.');
			if (dot && strcasecmp(dot, ".pkg")==0 && strncmp(de->d_name, "._", 2)!=0) {
				mLog->FLogLine("TPlatformManager: Checking '%s'", de->d_name);
				struct stat pkgStat;
				sprintf(buf, "%s/%s", inPackageDir, de->d_name);
				if (::stat(buf, &pkgStat)<0) continue;
				if (statErr>=0 
					&& pkgStat.st_mtime<lastInstall.st_mtime
					&& pkgStat.st_ctime<lastInstall.st_mtime) continue;
				mLog->FLogLine("TPlatformManager: Installing '%s'", de->d_name);
				InstallPackage(buf);
			}
		}
	}
	closedir(dir);
	
	// -- update the modification data
	mLog->FLogLine("TPlatformManager: updating last package installation date");
	sprintf(buf, "%s/.lastInstall", inPackageDir);
	FILE *f = fopen(buf, "wb");
	if (f) fclose(f);
}


// -------------------------------------------------------------------------- //
//  * OpenEinsteinMenu()
// -------------------------------------------------------------------------- //
void 
TPlatformManager::OpenEinsteinMenu()
{
	//mScreenManager->OpenEinsteinMenu();
}


// -------------------------------------------------------------------------- //
//  * SetDocDir()
// -------------------------------------------------------------------------- //
void 
TPlatformManager::SetDocDir(const char *inDocDir)
{
	if (mDocDir) ::free(mDocDir);
	mDocDir = strdup(inDocDir);
}


// ====================================================================== //
// Nurse Donna:    Oh, Groucho, I'm afraid I'm gonna wind up an old maid. //
// Groucho:        Well, bring her in and we'll wind her up together.     //
// Nurse Donna:    Do you believe in computer dating?                     //
// Groucho:        Only if the computers really love each other.          //
// ====================================================================== //
