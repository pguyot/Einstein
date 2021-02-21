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

#include "TPlatformManager.h"

// ANSI C & POSIX
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#if TARGET_OS_WIN32
#else
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <assert.h>
#include <thread>
#include <chrono>

// K
#include <K/Unicode/UUTF16CStr.h>
#include <K/Threads/TMutex.h>

// FLTK
#include <FL/fl_utf8.h>

// Einstein
#include "Emulator/TInterruptManager.h"
#include "Emulator/TMemory.h"
#include "Emulator/TARMProcessor.h"
#include "Emulator/TEmulator.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/Host/THostInfo.h"
#include "Emulator/Screen/TScreenManager.h"
#include "Emulator/PCMCIA/TPCMCIAController.h"
#include "Emulator/PCMCIA/TNE2000Card.h"
#include "Emulator/PCMCIA/TLinearCard.h"

#include "Emulator/Serial/TSerialPortManager.h"

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
		mScreenManager(inScreenManager)
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
//  * GetVersion()
// -------------------------------------------------------------------------- //
KUInt32 TPlatformManager::GetVersion()
{
	return 5;
}


// -------------------------------------------------------------------------- //
//  * GetNextEvent( KUInt32 )
// -------------------------------------------------------------------------- //
bool
TPlatformManager::GetNextEvent( KUInt32 outEventPAddr )
{
	bool theResult = false;

	mMutex->Lock();

	// If there are still events in the fifo, copy the next pending event
	// from Einstein space into NewtonOS space.
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

		theSize = ((theSize+3) / 4);
		
		KUInt32* theEventAsLongs = (KUInt32*) theEvent;
		while (theSize-- > 0)
		{
			(void) mMemory->WriteP(
					outEventPAddr,
					*theEventAsLongs++ );
			outEventPAddr += 4;
		}
		// FIXME: can the event be deleted after it was copied? Is it deleted elsewhere?
	} else {
		// FIXME: this code is never reached
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
bool
TPlatformManager::DisposeBuffer( KUInt32 inID )
{
	bool theResult = false;

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
bool
TPlatformManager::CopyBufferData(
						KUInt32 inID,
						KUInt32 outVAddress,
						KUInt32 inOffset,
						KUInt32 inAmount)
{
	bool theResult = false;

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

	// Append the Event to the Einstein Platform queue
	KUInt32 newPCrsr = mEventQueuePCrsr;
	SEvent* theEvent = &mEventQueue[newPCrsr++];
	theEvent->fType = kAEvent;
	theEvent->fData.aevent.fPort = inPortId;
	theEvent->fData.aevent.fSize = inSize;
	(void) ::memcpy( theEvent->fData.aevent.fData, inData, inSize );

	// Expand the queue if there are no more entries available
	if (newPCrsr == mEventQueueSize) {
		mEventQueueSize += kEVENTQUEUESIZEINCREMENT;
		mEventQueue = (SEvent*)::realloc(
							mEventQueue,
							sizeof(SEvent)*mEventQueueSize 
		);
	}
	mEventQueuePCrsr = newPCrsr;
	
	// Trigger the Platform Interrupt to let NewtonOS know that a new event is pending.
	//
	// NewtonOS can only handle one interrupt at a time, so a locking echanism ensures
	// that only one event is handled at a time.
	//
	// mQueuePreLock protects NewtonOS from repeated interrupts as soon as the current 
	// interrupt is triggered until the last recursion of mQueueLockCount is unlocked.
	//
	// mQueueLockCount is a recursive lock that protects the NewtonOS interrupt routine 
	// in ./Drivers/TMainPlatformDriver::InterruptHandler.cpp: TMainPlatformDriver::InterruptHandler()

	if ( (mQueuePreLock==false) && (mQueueLockCount==0) ) {
		mQueuePreLock = true;
		RaisePlatformInterrupt();
	}

	mMutex->Unlock();
}

/**
 * Insert or replace a PCCard in a given slot, or remove a PCCard
 */
int TPlatformManager::InsertPCCard(KUInt32 inSLot, TPCMCIACard* inCard)
{
	if (!mMemory) return -1;

	TPCMCIAController* controller = mMemory->GetPCMCIAController(inSLot);
	if (!controller) return -1;

	bool doPause = (controller->CurrentCard()!=nullptr) && (inCard!=nullptr);

	if (controller->CurrentCard()) {
		controller->RemoveCard();
	}

	if (doPause)
		std::this_thread::sleep_for(std::chrono::seconds(3));

	if (inCard) {
		controller->InsertCard(inCard);
	}

	return 0;
}


// -------------------------------------------------------------------------- //
//  * GetPCCard( KUInt32 )
// -------------------------------------------------------------------------- //
TPCMCIACard* 
TPlatformManager::GetPCCard(KUInt32 inSlot)
{
	if (!mMemory) return nullptr;
	if (inSlot < 0 || inSlot>1) return nullptr;

	TPCMCIAController* controller = mMemory->GetPCMCIAController(inSlot);
	if (!controller) return nullptr;

	return controller->CurrentCard();
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
	mMutex->Lock();
	mQueueLockCount++;
	mMutex->Unlock();
}

// -------------------------------------------------------------------------- //
//  * UnlockEventQueue( void )
// -------------------------------------------------------------------------- //
void
TPlatformManager::UnlockEventQueue( void )
{
	mMutex->Lock();

	// mQueueLockCount should never be less than or equal to zero. If it is, we called 
	// Unlock() more often than Lock(), which may triggere severe bugs
	assert(mQueueLockCount > 0);

	mQueueLockCount--;

	// as soon as the lock is completely unlocked, we can trigger more pending interrupts
	if (mQueueLockCount == 0) 
	{
		// allow Einstein to trigger new interrupts
		mQueuePreLock = false;

		// check if there are more platform events pending
		if (mEventQueuePCrsr != mEventQueueCCrsr) 
		{
			// no further interrupts may be triggered
			mQueuePreLock = true;

			// trigger another interrupt that will take care of handling remaining events
			RaisePlatformInterrupt();
		}
	}

	mMutex->Unlock();
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
TPlatformManager::EvalNewtonScript(const char* inNewtonScriptCode)
{
	unsigned srcLen = strlen(inNewtonScriptCode);
	unsigned dstLen = fl_utf8toa(inNewtonScriptCode, srcLen, nullptr, 0);

	if (srcLen == dstLen) {
		SendBufferAEvent(
			kNewtPort,
			kEinsteinNSEventClass,
			kEventRuntimeWithSData,
			kNewtonScriptEvalData,
			(KUInt32) ::strlen(inNewtonScriptCode),
			(const KUInt8*)inNewtonScriptCode);
	} else {
		char* dstScript = (char*)::malloc(dstLen + 1);
		fl_utf8toa(inNewtonScriptCode, srcLen, dstScript, dstLen+1);
		SendBufferAEvent(
			kNewtPort,
			kEinsteinNSEventClass,
			kEventRuntimeWithSData,
			kNewtonScriptEvalData,
			(KUInt32)dstLen,
			(const KUInt8*)dstScript);
		free(dstScript);
	}
}

// -------------------------------------------------------------------------- //
//  * InstallPackage(const KUInt8* inPackageData, KUInt32 inPackageSIze)
// -------------------------------------------------------------------------- //
void TPlatformManager::InstallPackage(const KUInt8* inPackageData, KUInt32 inPackageSize)
{
    // TODO: we should probably do some basic check if this is really package data
    // TODO: we should return meaningful error codes, so the app can inform the user
    SendBufferAEvent(kNewtPort,
                     kEinsteinNSEventClass,
                     kEventRuntimeWithSData,
                     kPackageInstallData,
                     inPackageSize,
                     inPackageData );
}

// -------------------------------------------------------------------------- //
//  * InstallPackage( const char* )
// -------------------------------------------------------------------------- //
void
TPlatformManager::InstallPackage( const char* inPackagePath )
{
	if ( mLog )
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
			if ( mLog )
				mLog->FLogLine( "Problem while reading '%s'", inPackagePath );
		} else {
            InstallPackage(theBuffer, (KUInt32)theSize);
		}
		
		(void) ::fclose( thePackageFile );
		::free( theBuffer );
	} else {
		if ( mLog )
			mLog->FLogLine( "Cannot open package '%s'", inPackagePath );
	}
}


// -------------------------------------------------------------------------- //
//  * InstallNewPackages( const char* )
// --------------------------------------------------------------------------
void
TPlatformManager::InstallNewPackages( const char* inPackageDir )
{
#if !TARGET_OS_WIN32
	// -- find the directory
	if ( !inPackageDir ) 
		inPackageDir = mDocDir;
		
	if ( !inPackageDir ) {
		if ( mLog )
			mLog->FLogLine("TPlatformManager::InstallNewPackages: No package directory specified, skipping.");
		return;
	}
	
	DIR *dir = opendir(inPackageDir);
		
	if (!dir) {
		if ( mLog )
			mLog->FLogLine("TPlatformManager::InstallNewPackages: Can't open package directory: '%s'", inPackageDir);
		return;
	}
	
	// -- find the last installation date
	char buf[2048];
	sprintf(buf, "%s/.lastInstall", inPackageDir);
	struct stat lastInstall;
	int statErr = ::stat(buf, &lastInstall);
	if ( mLog )
		mLog->FLogLine("TPlatformManager: checking for new packages");
	
	// -- run the directory and install every file that is newer than lastInstall
	struct dirent *de;
	while ((de = readdir(dir))) {
		if (de->d_type==DT_REG) {
			const char *dot = strrchr(de->d_name, '.');
			if (dot && strcasecmp(dot, ".pkg")==0 && strncmp(de->d_name, "._", 2)!=0) {
				if ( mLog )
					mLog->FLogLine("TPlatformManager: Checking '%s'", de->d_name);
				struct stat pkgStat;
				sprintf(buf, "%s/%s", inPackageDir, de->d_name);
				if (::stat(buf, &pkgStat)<0) continue;
				if (statErr>=0 
					&& pkgStat.st_mtime<lastInstall.st_mtime
					&& pkgStat.st_ctime<lastInstall.st_mtime) continue;
				if ( mLog )
					mLog->FLogLine("TPlatformManager: Installing '%s'", de->d_name);
				InstallPackage(buf);
			}
		}
	}
	closedir(dir);
	
	// -- update the modification data
	if ( mLog )
		mLog->FLogLine("TPlatformManager: updating last package installation date");
	sprintf(buf, "%s/.lastInstall", inPackageDir);
	FILE *f = fopen(buf, "wb");
	if (f) fclose(f);
#endif
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


/**
 NewtonOS can call this method to get access into the Einstein system.
 The NTK project must link with NTKGlue.ntkc which provides access to Einstein
 by calling <tt>call Einstein.Platform with ('command, args);</tt> .

 \a arg0 is a Newt symbol that indicates the function call,
 \a arg1 depends on the function call and can be just a value, or an Array
 or a Frame of values. This call can return complex values by using
 Newton ROM functions at known locations (as set by Apple) to create
 Binary Objects like Text or Reals, and if multiple values are need,
 by building Arrays and Frames.

 \param inRcvr the receiver of this call, so basically the calling function
 \param inArg0 a symbol that declares the Einstein command that we want to call
 \param inArg1 the argument fo the call as a NewtonRef
 \return the result of the call as a NewtonRef

 \see TNewt
 */
NewtRef
TPlatformManager::NewtonScriptCall(TNewt::RefArg inRcvr, TNewt::RefArg inArg0, TNewt::RefArg inArg1)
{
	using namespace TNewt;

	NewtRef arg0 = inArg0.Ref();
	if (TNewt::RefIsSymbol(arg0)) {
		char sym[64] = { 0 };
		if (TNewt::SymbolToLowerCaseCString(arg0, sym, 64)) {
			auto it = CallMap.find(sym);
			if (it!=CallMap.end()) {
				PlatformCall call = it->second;
				return call(inRcvr, inArg1);
			} else {
				TNewt::SymbolToCString(arg0, sym, 64);
				fprintf(stderr, "WARNING: TPlatformManager::NewtonScriptCall: Unknown command: %s\n", sym);
				return TNewt::MakeInt(kNSErrUndefinedMethod);
			}
		}
	} else {
		fprintf(stderr, "WARNING: TPlatformManager::NewtonScriptCall: First argument must be a symbol: 0x%08x\n", arg0);
	}
	return TNewt::MakeInt(kNSErrNotASymbol);
}


// ====================================================================== //
// Nurse Donna:    Oh, Groucho, I'm afraid I'm gonna wind up an old maid. //
// Groucho:        Well, bring her in and we'll wind her up together.     //
// Nurse Donna:    Do you believe in computer dating?                     //
// Groucho:        Only if the computers really love each other.          //
// ====================================================================== //
