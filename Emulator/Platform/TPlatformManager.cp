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
#include <dirent.h>
#include <sys/stat.h>

// K
#include <K/Unicode/UUTF16CStr.h>
#include <K/Threads/TMutex.h>

// Einstein
#include "TInterruptManager.h"
#include "TMemory.h"
#include "TARMProcessor.h"
#include "TEmulator.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/Host/THostInfo.h"
#include "Emulator/Screen/TScreenManager.h"
#include "Emulator/PCMCIA/TPCMCIAController.h"
#include "Emulator/PCMCIA/TNE2000Card.h"

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
//  * GetVersion()
// -------------------------------------------------------------------------- //
KUInt32 TPlatformManager::GetVersion()
{
	return 5;
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


// -------------------------------------------------------------------------- //
//  * SetDocDir(NewtRef rcvr, NewtRef arg0, NewtRef arg1)
// -------------------------------------------------------------------------- //
NewtRef
TPlatformManager::NewtonScriptCall(NewtRef rcvr, NewtRef arg0, NewtRef arg1)
{
	// The implemenatation and calls into Newton OS should be in their own class!

	// arg0 is a Newt symbol that indicates the function call
	// arg1 depends on the function call and can be just a value, or an Array
	// or a Frame of values. This call can return complex values by using
	// Newton ROM functions at known locations (as set by Apple) to create
	// Binary Objects like Text or Reals, an if multiple values are need,
	// by building Arrays and Frames.

	// More official addresses from the universal object file:
	//  - MakeString(const unsigned short *) @ 0x018009a0
	//  - AddArraySlot(const RefVar &, const RefVar &) @ 0x018007f4
	//  - AllocateArray(const RefVar &, long) & 01800804
	// TODO: get the RefVar class emulated in a nice way

	if (TNewt::RefIsSymbol(arg0)) {
		char sym[64] = { 0 };
		if (TNewt::SymbolToCString(arg0, sym, 64)) {
			fprintf(stderr, "Platform call '%s'\n", sym);
			if (strcmp(sym, "getframerate")==0) {
				return TNewt::MakeInt(50);
			} else if (strcmp(sym, "setframerate")==0) {
				KSInt32 v = TNewt::RefToInt(arg1);
				printf("setFrameRate: '%s, %d\n", sym, v);
				return kNewtRefTRUE;

			// serial port driver support

				// ('print, Ref)
				// ('SetSerialPortDriver, driverId)
				// names := call Einstein.Platform with ('GetSerialPortDriverNames, nil); 			ok
				// idx := call Einstein.Platform with ('GetSerialPortDriver, nil);					ok
				// names := call EInstein.Platform with ('GetSerialPortDriverNames, nil);			ok
				// drivers := call Einstein.Platform with ('GetSerialPortDriverList, nil);
				// opt := call Einstein.Platform with ('GetSerialPortDriverOptions, 'extr);
				// call Einstein.Platform with ('SetSerialPortDriverOptions,
				//							 { id:'extr, server:wServer.text, port:wPort.text }


			} else if (strcmp(sym, "getserialportdrivernames")==0) { // do me
//				// return an array of names, one for every driver available
				int nNames = (int)TSerialPorts::DriverNames.size();
				TNewt::RefVar array( TNewt::AllocateArray(nNames) );
//				NewtRef arrayRef = TNewt::AllocateArray(nNames);
//				NewtRefVar array = TNewt::AllocateRefHandle(arrayRef);
				for (int i=0; i<nNames; i++) {
					TNewt::RefVar str( TNewt::MakeString(TSerialPorts::DriverNames[i]) );
//					NewtRef strRef = TNewt::MakeString(TSerialPorts::DriverNames[i]);
//					NewtRefVar str = TNewt::AllocateRefHandle(strRef);
					//TNewt::SetArraySlotRef(arrayRef, i,  strRef);
					TNewt::SetArraySlot(array, i,  str);
//					NewtRef ref = str.Ref();
				}
//				TNewt::DisposeRefHandle(array);
				return array.Ref();
//				return arrayRef;
			} else if (strcmp(sym, "getserialportdriverlist")==0) { // do me
				int nNames = 5; // FIXME:
				NewtRef arrayRef = TNewt::AllocateArray(nNames);
				NewtRefVar array = TNewt::AllocateRefHandle(arrayRef);
				for (int i=0; i<nNames; i++) {
					NewtRef strRef = TNewt::MakeInt(TSerialPorts::ValidDrivers[i]);
					NewtRefVar str = TNewt::AllocateRefHandle(strRef);
					//TNewt::SetArraySlotRef(arrayRef, i,  strRef);
					TNewt::SetArraySlot(array, i,  str);
					TNewt::DisposeRefHandle(str);
				}
				TNewt::DisposeRefHandle(array);
				return arrayRef;
			} else if (strcmp(sym, "setserialportdriver")==0) { // do me
				// send the index of the driver to query 0...3, external, infrared, internal, modem
				// example: { port:0, driver:4, config:{ server:127.0.0.1, port:3679 } }
				// example: { port:0, driver:4, config:'default }
				// if values are missing, keep the current config
				// receive negative error code, or 0 for success
				return kNewtRefNIL;
			} else if (strcmp(sym, "getserialportdriver")==0) { // do me
				// send the index of the driver to query 0...3, external, infrared, internal, modem
				// recive nil for unavailable port
				// receive a recored with the current driver, containing a record with its parameters, and an array with available drivers for this port
				// example: { driver:4, config:{ server:127.0.0.1, port:3679 }, valid:[0, 3, 4] }
//				int n = TSerialPortManager::DriverListSize;
//				NewtRef arrayRef = AllocateArray(n);
//				NewtRefVar array = AllocateRefHandle(arrayRef);
//				for (int i=0; i<n; i++) {
//					SetArraySlotRef(arrayRef, i, NewtMakeInt(TSerialPortManager::DriverList[i]) );
//				}
//				DisposeRefHandle(array);
//				return arrayRef;
				return TNewt::MakeInt(3);
			// remove the remaining options!
//			} else if (strcmp(sym, "getserialportdriver")==0) {
//				// get current driver for main port
//				KUInt32 id = TSerialPortManager::CurrentDriver(
//				    TSerialPortManager::kExternalSerialPort)->GetID();
//				return NewtMakeInt(id);
//				return kNewtRefNIL;
//			} else if (strcmp(sym, "setserialportdriver")==0) {
//				// set driver for main port to driver with index ix
//				KSInt32 id = NewtRefToInt(arg1);
//				printf("ID: %d\n", id);
//				TSerialPortManager::ReplaceDriver(
//                    TSerialPortManager::kExternalSerialPort,
//				    TSerialPortManager::CreateByID(id, 0L,
//					    TSerialPortManager::kExternalSerialPort));
//				return kNewtRefNIL;
			} else if (strcmp(sym, "getserialportdriveroptions")==0) { // do me
//				// parameter is the fourCC symbol of the serial port
//				// returns nil if there are no settings
//				// for TCP, this returns { driver:4, server:"127.0.0.1", port:"3679" }
//				// 4 is the ID for a TCP client driver
//				fprintf(stderr, "TODO: get serial port driver options\n");
////				NewtRefVar port = AllocateRefHandle( MakeString("Welt") );
//
//				NewtRef fRef = AllocateFrame();
//				NewtRefVar f = AllocateRefHandle( fRef );
//
//				NewtRefVar sym = AllocateRefHandle(MakeSymbol("driver"));
//				NewtRefVar server = AllocateRefHandle( MakeString("Hallo") );
//				SetFrameSlot(f, sym, server);
//				DisposeRefHandle(server);
//				DisposeRefHandle(sym);
//
//				//SetFrameSlot(f, MakeSymbol("server"), server);
//				//SetFrameSlot(f, MakeSymbol("port"), port);
////				DisposeRefHandle(port);
//
//				DisposeRefHandle(f);
//				return fRef;
				return TNewt::AllocateArray(2);
				return kNewtRefNIL;
			} else if (strcmp(sym, "setserialportdriveroptions")==0) { // do me
//				// for TCP, send { serPort:'extr, serverAddr:"127.0.0.1", sereverPort:"3679" }
//				fprintf(stderr, "TODO: set serial port driver options\n");
				return kNewtRefNIL;
			} else if (strcmp(sym, "getsymbol")==0) {
				//NewtRef symRef = MakeSymbol("test");
				//NewtRefVar symVar = AllocateRefHandle( symRef );

				NewtRef arrRef = TNewt::AllocateArray(3);

				NewtRefVar arrVar = TNewt::AllocateRefHandle( arrRef );
				NewtRef txtRef = TNewt::MakeString("Will this ever work?");
				NewtRefVar txtVar = TNewt::AllocateRefHandle( txtRef );
//				SetArrySlotRef(arrRef, 1, txtRef);
				TNewt::SetArraySlot(arrVar, 1, txtVar);
				// FIXME: doubles are counted wrong (see above)
				TNewt::SetArraySlotRef(arrRef, 2, TNewt::MakeReal(3.141592654));
				return arrRef;
//				return MakeSymbol("Matt:MM");
//				return MakeString("This is Einstein talking...!");
			} else {
				fprintf(stderr, "Unknown command: %s\n", sym);
				return TNewt::MakeInt(kNSErrUndefinedMethod);
			}
		}
	}
	return TNewt::MakeInt(kNSErrNotASymbol);
}


// ====================================================================== //
// Nurse Donna:    Oh, Groucho, I'm afraid I'm gonna wind up an old maid. //
// Groucho:        Well, bring her in and we'll wind her up together.     //
// Nurse Donna:    Do you believe in computer dating?                     //
// Groucho:        Only if the computers really love each other.          //
// ====================================================================== //
