// ==============================
// File:			TAndroidApp.cp
// Project:			Einstein
//
// Copyright 2011 by Matthias Melcher (einstein@matthiasm.com).
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
#include "TAndroidApp.h"

// ANSI C & POSIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// Einstein
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Sound/TNullSoundManager.h"
#include "Emulator/Screen/TAndroidScreenManager.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //


class TAndroidLog : public TLog
{
public:
	TAndroidLog() { }
protected:
	virtual void DoLogLine(const char* inLine) {
		LOGW("%s", inLine);
	}
};

// -------------------------------------------------------------------------- //
//  * TAndroidApp( void )
// -------------------------------------------------------------------------- //
TAndroidApp::TAndroidApp( void )
:
mProgramName( nil ),
mROMImage( nil ),
mEmulator( nil ),
mSoundManager( nil ),
mScreenManager( nil ),
mPlatformManager( nil ),
mLog( nil ),
mNetworkManager( nil ),
mQuit(false)
{
}

// -------------------------------------------------------------------------- //
//  * ~TAndroidApp( void )
// -------------------------------------------------------------------------- //
TAndroidApp::~TAndroidApp( void )
{
	if (mEmulator)
	{
		delete mEmulator;
	}
	if (mScreenManager)
	{
		delete mScreenManager;
	}
	if (mSoundManager)
	{
		delete mSoundManager;
	}
	if (mLog)
	{
		delete mLog;
	}
	if (mROMImage)
	{
		delete mROMImage;
	}
	if (mNetworkManager)
	{
		delete mNetworkManager;
	}
}


// -------------------------------------------------------------------------- //
// Run( int, char** )
// -------------------------------------------------------------------------- //
void
TAndroidApp::Run(const char *dataPath)
{
	mProgramName = "Einstein";
	mROMImage = NULL;
	mEmulator = NULL;
	mSoundManager = NULL;
	mScreenManager = NULL;
	mPlatformManager = NULL;
	mLog = NULL;
	
	mLog = new TAndroidLog();
	LOGW("mLog: 0x%08x", (int)mLog);

	char theROMPath[1024];
	snprintf(theROMPath, 1024, "%s/717006.rom", dataPath);
	if (access(theROMPath, R_OK)==-1) {
		LOGE("Can't read ROM file %s", theROMPath);
		return;
	}

	char theREXPath[1024];
	snprintf(theREXPath, 1024, "%s/Einstein.rex", dataPath);
	if (access(theREXPath, R_OK)==-1) {
		LOGE("Can't read REX file %s", theREXPath);
		return;
	}
	
	char theImagePath[1024];
	snprintf(theImagePath, 1024, "%s/717006.img", dataPath);
	
	char theFlashPath[1024];
	snprintf(theFlashPath, 1024, "%s/flash", dataPath);
	
	mROMImage = new TFlatROMImageWithREX(theROMPath, theREXPath, "717006", false, theImagePath);
	LOGW("mROMImage: 0x%08x", (int)mROMImage);

	mSoundManager = new TNullSoundManager(mLog);
	LOGW("mSoundManager: 0x%08x", (int)mSoundManager);

	int newtonScreenWidth = 320;
	int newtonScreenHeight = 480;
	Boolean isLandscape = false;
	
	mScreenManager = new TAndroidScreenManager(mLog,
											   newtonScreenWidth, newtonScreenHeight,
											   true,
											   isLandscape);
	LOGW("mScreenManager: 0x%08x", (int)mScreenManager);
	
	mNetworkManager = new TNullNetwork(mLog);
	LOGW("mNetworkManager: 0x%08x", (int)mNetworkManager);
	
	mEmulator = new TEmulator(
							  mLog, 
							  mROMImage, 
							  theFlashPath,
							  mSoundManager, 
							  mScreenManager, 
							  mNetworkManager, 
							  0x40 << 16);
	LOGW("mEmulator: 0x%08x", (int)mEmulator);

	mPlatformManager = mEmulator->GetPlatformManager();

	pthread_t theThread;
	int theErr = ::pthread_create( &theThread, NULL, SThreadEntry, this );
	if (theErr) 	{
		(void) LOGE( "Error with pthread_create (%i)\n", theErr );
		::exit(2);
	}
	LOGE("Booting NewtonOS...");
}


// -------------------------------------------------------------------------- //
// ThreadEntry( void )
// -------------------------------------------------------------------------- //
void
TAndroidApp::ThreadEntry( void )
{
	mEmulator->Run();
	mQuit = true;
}

int TAndroidApp::updateScreen(unsigned short *buffer)
{
	int ret = 0;
	if (mScreenManager) {
		TAndroidScreenManager *tasm = (TAndroidScreenManager*)mScreenManager;
		ret = tasm->update(buffer);
	}
	return ret;
}

int TAndroidApp::screenIsDirty()
{
	int ret = 0;
	if (mScreenManager) {
		TAndroidScreenManager *tasm = (TAndroidScreenManager*)mScreenManager;
		ret = tasm->isDirty();
	}
	return ret;
}

// ======================================================================= //
// We build our computer (systems) the way we build our cities: over time, 
// without a plan, on top of ruins.
//   -- Ellen Ullman
// ======================================================================= //
