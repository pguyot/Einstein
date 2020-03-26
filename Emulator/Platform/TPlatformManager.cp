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

static const KSInt32 kNSErrUndefinedMethod = -48809;
static const KSInt32 kNSErrNotASymbol = -48410;


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


/*
 Ags is a "C" string that indicates the type of the argument list
 - s: 0-terminated "C" string
 ? u: 0-terminated "C" string in utf8 that is passed as a MSB utf16 string
 ? i: 32 bit integer
 - d: 64 bit double precission floating point valu
 ? R: a NewtonsScript Ref
 */
KUInt32 TPlatformManager::CallNewton(VAddr functionVector, const char *args, ...)
{
	KUInt32 regs[16];
	va_list ap;
	KUInt32 sp = mCPU->GetRegister(13);
	KUInt32 pc = functionVector;
	KUInt32 r0 = 0;

	// backing up all registers
	memcpy(regs, mCPU->mCurrentRegisters, sizeof(regs));

	// Convert the list of arguments into ARM function call convention
	va_start (ap, args);
	for (int i=0; ;i++) {
		char c = args[i];
		if (c==0) break;
		if (i>3) {
			fprintf(stderr, "ERROR in TPlatformManager::CallNewton: too many arguments\n");
			break;
		}
		switch (c) {
			case 's': { // C utf-8 string
				const char *s = va_arg(ap, char*);
				if (s) {
					KUInt32 n = (KUInt32)strlen(s)+1, n4 = (n+3) & ~3;
					sp -= n4;
					mMemory->FastWriteString(sp, &n, s);
					mCPU->SetRegister(i, sp);
				} else {
					mCPU->SetRegister(i, 0);
				}
				break; }
			case 'd': { // double precission floating point
				union { double d; KUInt32 i[2]; } u;
				u.d = va_arg(ap, double);
				// FIXME: make sure that this order is the same on LSB and MSB machines (tested ok for Intelorder (LSB))
				mCPU->SetRegister(i+1, u.i[0]);
				mCPU->SetRegister(  i, u.i[1]);
				// FIXME: wrong argument counting!
				i++;
				break; }
			case 'A': { // NewtRefArg
				KUInt32 v = va_arg(ap, KUInt32);
				sp -= 4;
				mMemory->Write(sp, v);
				mCPU->SetRegister(i, sp);
				sp -= 4;
				break; }
			case '0': { // zero indirection, just copy to register
				KUInt32 v = va_arg(ap, KUInt32);
				mCPU->SetRegister(i, v);
				break; }
			case 'i': { // int
				KUInt32 v = va_arg(ap, KUInt32);
				mCPU->SetRegister(i, v);
				break; }
			default:
				fprintf(stderr, "ERROR in TPlatformManager::CallNewton: unspported argument type '%c'\n", c);
				break;
		}
	}
	va_end(ap);

	// Preset the return address for the emulator loop
    mCPU->SetRegister(13, sp);
	mCPU->SetRegister(14, 0x00fffffc); // magic address at the end of the REx
	mCPU->SetRegister(15, pc);

	TJITGeneric *mJit = mMemory->GetJITObject();
	JITUnit* theJITUnit = mJit->GetJITUnitForPC( mCPU, mMemory, pc+4);
	for(;;)
	{
		theJITUnit = theJITUnit->fFuncPtr( theJITUnit, mCPU );
		if (mCPU->mCurrentRegisters[15]==0x00fffffc+4) break;
	}

	r0 = mCPU->GetRegister(0);

	// Restoring all registers
	memcpy(mCPU->mCurrentRegisters, regs, sizeof(regs));
	return r0;
}

// Tested and OK
NewtRef TPlatformManager::MakeString(const char *txt)
{
	return (NewtRef)CallNewton(0x0180099c, "s", txt);
}

// Tested and OK
NewtRef TPlatformManager::MakeSymbol(const char *txt)
{
	return (NewtRef)CallNewton(0x018029cc, "s", txt);
}

// Tested and OK
NewtRef TPlatformManager::MakeReal(double v)
{
	return (NewtRef)CallNewton(0x01800998, "d", v);
}

// TODO: implement argument types and RefVar
NewtRefVar TPlatformManager::AllocateRefHandle(NewtRef r)
{
	return (NewtRefVar)CallNewton(0x01800818, "0", r); // AllocateRefHandle_Fl
}

// TODO: implement argument types and RefVar
void TPlatformManager::DisposeRefHandle(NewtRefVar v) // DisposeRefHandle_FP9RefHandle
{
	CallNewton(0x01800888, "0", v);
}

// TODO: implement argument types and RefVar
NewtRef TPlatformManager::AllocateFrame()
{
	return (NewtRef)CallNewton(0x0180080c, ""); // AllocateFrame_Fv
}

// TODO: implement argument types and RefVar
KUInt32 TPlatformManager::AddSlot(NewtRefArg frame, NewtRefArg slot)
{
	return CallNewton(0x018007fc, "AA", frame, slot); // AddSlot__FRC6RefVarTl
}

NewtRef TPlatformManager::AddArraySlot(NewtRefArg array, NewtRefArg value)
{
	return (NewtRef)CallNewton(0x018007f4, "AA"); // AddArraySlot__FRC6RefVarT1
}

// Works
NewtRef TPlatformManager::AllocateArray(NewtRefArg symbol, KUInt32 nSlots)
{
	return (NewtRef)CallNewton(0x01800804, "Ai", symbol, nSlots); // AllocateArray__FRC6RefVarl
}

NewtRef TPlatformManager::AllocateArray(KUInt32 nSlots)
{
	NewtRefVar sym = AllocateRefHandle(MakeSymbol("array"));
	NewtRef arrRef = AllocateArray(sym, 3);
	DisposeRefHandle(sym);
	return arrRef;
}

// works
NewtRef TPlatformManager::SetArraySlotRef(NewtRef array, KUInt32 index, NewtRef value)
{
	return (NewtRef)CallNewton(0x01800a24, "0i0", array, index, value);
}

// works
NewtRef TPlatformManager::SetArraySlot(NewtRefArg array, KUInt32 index, NewtRefArg value)
{
	return (NewtRef)CallNewton(0x018029e4, "AiA", array, index, value); // SetArraySlot__FRC6RefVarlT1
}

// works
NewtRef TPlatformManager::SetFrameSlot(NewtRefArg frame, NewtRefArg key, NewtRefArg value)
{
	return (NewtRef)CallNewton(0x01800a34, "AAA", frame, key, value);
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

	if (NewtRefIsSymbol(arg0)) {
		char sym[64] = { 0 };
		if (NewtSymbolToCString(arg0, sym, 64)) {
			fprintf(stderr, "Platform call '%s'\n", sym);
			if (strcmp(sym, "getframerate")==0) {
				return NewtMakeInt(50);
			} else if (strcmp(sym, "setframerate")==0) {
				KSInt32 v = NewtRefToInt(arg1);
				printf("setFrameRate: '%s, %d\n", sym, v);
				return kNewtRefTRUE;
			} else if (strcmp(sym, "getserialportdriver")==0) {
				// get current driver for main port
				KUInt32 id = TSerialPortManager::CurrentDriver(
				    TSerialPortManager::kExternalSerialPort)->GetID();
				return NewtMakeInt(id);
			} else if (strcmp(sym, "setserialportdriver")==0) {
				// set driver for main port to driver with index ix
				KSInt32 id = NewtRefToInt(arg1);
				printf("ID: %d\n", id);
				TSerialPortManager::ReplaceDriver(
                    TSerialPortManager::kExternalSerialPort,
				    TSerialPortManager::CreateByID(id, 0L,
					    TSerialPortManager::kExternalSerialPort));
				return kNewtRefNIL;
			} else if (strcmp(sym, "getserialportdrivernames")==0) {
				char const* const *names = TSerialPortManager::DriverName;
				int nNames = 5;
				NewtRef arrayRef = AllocateArray(nNames);
				NewtRefVar array = AllocateRefHandle(arrayRef);
				for (int i=0; i<nNames; i++) {
					SetArraySlotRef(arrayRef, i, MakeString(names[i]) );
				}
				DisposeRefHandle(array);
				return arrayRef;
			} else if (strcmp(sym, "getserialportdriverlist")==0) {
				int n = TSerialPortManager::DriverListSize;
				NewtRef arrayRef = AllocateArray(n);
				NewtRefVar array = AllocateRefHandle(arrayRef);
				for (int i=0; i<n; i++) {
					SetArraySlotRef(arrayRef, i, NewtMakeInt(TSerialPortManager::DriverList[i]) );
				}
				DisposeRefHandle(array);
				return arrayRef;
			} else if (strcmp(sym, "getserialportdriveroptions")==0) {
				// parameter is the fourCC symbol of the serial port
				// returns nil if there are no settings
				// for TCP, this returns { driver:4, server:"127.0.0.1", port:"3679" }
				// 4 is the ID for a TCP client driver
				fprintf(stderr, "TODO: get serial port driver options\n");
//				NewtRefVar port = AllocateRefHandle( MakeString("Welt") );

				NewtRef fRef = AllocateFrame();
				NewtRefVar f = AllocateRefHandle( fRef );

				NewtRefVar sym = AllocateRefHandle(MakeSymbol("driver"));
				NewtRefVar server = AllocateRefHandle( MakeString("Hallo") );
				SetFrameSlot(f, sym, server);
				DisposeRefHandle(server);
				DisposeRefHandle(sym);

				//SetFrameSlot(f, MakeSymbol("server"), server);
				//SetFrameSlot(f, MakeSymbol("port"), port);
//				DisposeRefHandle(port);

				DisposeRefHandle(f);
				return fRef;
			} else if (strcmp(sym, "setserialportdriveroptions")==0) {
				// for TCP, send { serPort:'extr, serverAddr:"127.0.0.1", sereverPort:"3679" }
				fprintf(stderr, "TODO: set serial port driver options\n");
				return kNewtRefNIL;
			} else if (strcmp(sym, "getsymbol")==0) {
				//NewtRef symRef = MakeSymbol("test");
				//NewtRefVar symVar = AllocateRefHandle( symRef );

				NewtRef arrRef = AllocateArray(3);
				// TODO: Symbol caching!

				NewtRefVar arrVar = AllocateRefHandle( arrRef );
				NewtRef txtRef = MakeString("Will this ever work?");
				NewtRefVar txtVar = AllocateRefHandle( txtRef );
//				SetArrySlotRef(arrRef, 1, txtRef);
				SetArraySlot(arrVar, 1, txtVar);
				// FIXME: doubles are counted wrong (see above)
				SetArraySlotRef(arrRef, 2, MakeReal(3.141592654));
				return arrRef;
//				return MakeSymbol("Matt:MM");
//				return MakeString("This is Einstein talking...!");
			} else {
				fprintf(stderr, "Unknown command: %s\n", sym);
				return NewtMakeInt(kNSErrUndefinedMethod);
			}
		}
	}
	return NewtMakeInt(kNSErrNotASymbol);
}



bool TPlatformManager::NewtRefIsInt(NewtRef r)
{
	return ((r&3)==0);
}

KSInt32 TPlatformManager::NewtRefToInt(NewtRef r)
{
	KSInt32 v = (KSInt32)r;
	return v>>2;
}

NewtRef TPlatformManager::NewtMakeInt(KSInt32 v)
{
	return ((KUInt32)v<<2) & 0xFFFFFFFC;
}

bool TPlatformManager::NewtRefIsSymbol(NewtRef r)
{
	if (!NewtRefIsPointer(r)) return false;
	KUInt32 p = NewtRefToPointer(r);
	KUInt32 newtPtrClass = 0;
	mMemory->Read(p+8, newtPtrClass);
	if (newtPtrClass!=kNewtSymbolClass) return false;
	return true;
}

bool TPlatformManager::NewtSymbolToCString(NewtRef r, char *buf, int bufSize)
{
	if (!NewtRefIsPointer(r))
		return false;

	KUInt32 p = NewtRefToPointer(r);

	KUInt32 newtPtrClass = 0;
	mMemory->Read(p+8, newtPtrClass);
	if (newtPtrClass!=kNewtSymbolClass)
		return false;

	KUInt32 newtSize = 0;
	mMemory->Read(p, newtSize);
	int strSize = (newtSize>>8)-16;
	if (strSize>bufSize)
		return false;

	mMemory->FastReadBuffer(p+16, strSize, (KUInt8*)buf);

	for (int i=0; i<strSize; i++)
		buf[i] = tolower(buf[i]&0x7f);

	return true;
}

bool TPlatformManager::NewtRefIsString(NewtRef r)
{
	// TODO: write this
	return false;
}

KUInt32 TPlatformManager::NewtRefStringLength(NewtRef r)
{
	// TODO: write this
	return 0;
}

char *TPlatformManager::NewtRefToStringDup(NewtRef r)
{
	// TODO: write this
	return 0;
}

NewtRef TPlatformManager::NewtRefReplaceString(NewtRef, const char*)
{
	// TODO: write this
	return kNewtRefNIL;
}

bool TPlatformManager::NewtRefIsPointer(NewtRef r)
{
	return ((((KUInt32)r)&3)==1);
}

KUInt32 TPlatformManager::NewtRefToPointer(NewtRef r)
{
	return ((KUInt32)r)&0xFFFFFFFC;
}

NewtRef TPlatformManager::NewtMakePointer(KUInt32 r)
{
	return (NewtRef)((r&0xFFFFFFFC)|1);
}


// ====================================================================== //
// Nurse Donna:    Oh, Groucho, I'm afraid I'm gonna wind up an old maid. //
// Groucho:        Well, bring her in and we'll wind her up together.     //
// Nurse Donna:    Do you believe in computer dating?                     //
// Groucho:        Only if the computers really love each other.          //
// ====================================================================== //
