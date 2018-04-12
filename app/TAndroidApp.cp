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
#include <unistd.h>

// Einstein
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Network/TUsermodeNetwork.h"
#include "Emulator/Sound/TAndroidSoundManager.h"
#include "Emulator/Screen/TAndroidScreenManager.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Log/TLog.h"

//#include <SLES/OpenSLES.h>
//#include <SLES/OpenSLES_Android.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#if 0
SLObjectItf mEngineObj;
SLEngineItf mEngine;
SLObjectItf mOutputMixObj;

void audio_test() {
	
	SLresult lRes;
	
	const SLuint32      lEngineMixIIDCount = 1;
	const SLInterfaceID lEngineMixIIDs[]={SL_IID_ENGINE};
	const SLboolean lEngineMixReqs[]={SL_BOOLEAN_TRUE};
	const SLuint32 lOutputMixIIDCount=0;
	const SLInterfaceID lOutputMixIIDs[]={};
	const SLboolean lOutputMixReqs[]={};
	
	lRes = slCreateEngine(&mEngineObj, 0, NULL, lEngineMixIIDCount, lEngineMixIIDs, lEngineMixReqs);
	(*mEngineObj)->Destroy(mEngineObj);
}
#endif

#if 0
namespace packt {
	SoundService::SoundService(android_app* pApplication):
	mApplication(pApplication),
	mEngineObj(NULL), mEngine(NULL),
	mOutputMixObj(NULL)
	{}
	status SoundService::start() {
		Log::info(“Starting SoundService.”);
		SLresult lRes;
		const SLuint32      lEngineMixIIDCount = 1;
		const SLInterfaceID lEngineMixIIDs[]={SL_IID_ENGINE};
		const SLboolean lEngineMixReqs[]={SL_BOOLEAN_TRUE};
		const SLuint32 lOutputMixIIDCount=0;
		const SLInterfaceID lOutputMixIIDs[]={};
		const SLboolean lOutputMixReqs[]={};
		lRes = slCreateEngine(&mEngineObj, 0, NULL,
							  lEngineMixIIDCount, lEngineMixIIDs, lEngineMixReqs);
		if (lRes != SL_RESULT_SUCCESS) goto ERROR;
		lRes=(*mEngineObj)->Realize(mEngineObj,SL_BOOLEAN_FALSE);
		if (lRes != SL_RESULT_SUCCESS) goto ERROR;
		lRes=(*mEngineObj)->GetInterface(mEngineObj,SL_IID_ENGINE, &mEngine);
		if (lRes != SL_RESULT_SUCCESS) goto ERROR;
		lRes=(*mEngine)->CreateOutputMix(mEngine,
										 &mOutputMixObj,lOutputMixIIDCount,lOutputMixIIDs,
										 lOutputMixReqs);
		lRes=(*mOutputMixObj)->Realize(mOutputMixObj,
									   SL_BOOLEAN_FALSE);
		return STATUS_OK;
	ERROR:
		Packt::Log::error(“Error while starting SoundService.”);
		stop();
		return STATUS_KO;
	}
	void SoundService::stop() {
		if (mOutputMixObj != NULL) {
			(*mOutputMixObj)->Destroy(mOutputMixObj);
			mOutputMixObj = NULL;
		}
		if (mEngineObj != NULL) {
			(*mEngineObj)->Destroy(mEngineObj);
			mEngineObj = NULL; mEngine = NULL;
		}
	}
}
#endif




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
	mQuit(false),
	mNewtonID0(0x00004E65),
	mNewtonID1(0x77746F6E)
{
//	audio_test();
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
TAndroidApp::Run(const char *dataPath, int newtonScreenWidth, int newtonScreenHeight, TLog *inLog)
{
	mProgramName = "Einstein";
	mROMImage = NULL;
	mEmulator = NULL;
	mSoundManager = NULL;
	mScreenManager = NULL;
	mPlatformManager = NULL;
	mLog = NULL;
	
	if (inLog) inLog->LogLine("Loading assets...");
	
	if (inLog) inLog->LogLine("  mLog:");
	// The log slows down the emulator and may cause a deadlock when running 
	// the Network card emulation. Only activate if you really need it!
	// CAUTION: the destructor will delete our mLog. That is not good! Avoid!
	//if (inLog) mLog = inLog;
	if (inLog) inLog->FLogLine("    OK: 0x%08x", (intptr_t)mLog);

	char theROMPath[1024];
	snprintf(theROMPath, 1024, "%s/717006.rom", dataPath);
	if (mLog) mLog->FLogLine("  ROM exists at %s?", theROMPath);
	if (access(theROMPath, R_OK)==-1) {
		if (mLog) mLog->FLogLine("Can't read ROM file %s", theROMPath);
		return;
	}
	if (mLog) mLog->FLogLine("    OK");

	char theREXPath[1024];
	snprintf(theREXPath, 1024, "%s/Einstein.rex", dataPath);
	if (mLog) mLog->FLogLine("  ROM exists at %s?", theREXPath);
	if (access(theREXPath, R_OK)==-1) {
		if (mLog) mLog->FLogLine("Can't read REX file %s", theREXPath);
		return;
	}
	if (mLog) mLog->FLogLine("    OK");
	
	char theImagePath[1024];
	snprintf(theImagePath, 1024, "%s/717006.img", dataPath);
	
	char theFlashPath[1024];
	snprintf(theFlashPath, 1024, "%s/flash", dataPath);
	
	if (mLog) mLog->FLogLine("  mROMImage:");
	mROMImage = new TFlatROMImageWithREX(theROMPath, theREXPath, "717006", false, theImagePath);
	if (mLog) mLog->FLogLine("    OK: 0x%08x", (intptr_t)mROMImage);

	if (mLog) mLog->FLogLine("  mSoundManager:");
	mSoundManager = new TAndroidSoundManager(mLog);
	if (mLog) mLog->FLogLine("    OK: 0x%08x", (intptr_t)mSoundManager);

	Boolean isLandscape = false;
	if (mLog) mLog->FLogLine("  mScreenManager");
	mScreenManager = new TAndroidScreenManager(mLog,
											   newtonScreenWidth, newtonScreenHeight,
											   true, // fullscreen
											   isLandscape);
	if (mLog) mLog->FLogLine("    OK: 0x%08x", (intptr_t)mScreenManager);
	
	if (mLog) mLog->FLogLine("  mNetworkManager:");
	mNetworkManager = new TUsermodeNetwork(mLog);
	if (mLog) mLog->FLogLine("    OK: 0x%08x", (intptr_t)mNetworkManager);
	
	if (mLog) mLog->FLogLine("  mEmulator:");
	mEmulator = new TEmulator(
							  mLog, 
							  mROMImage, 
							  theFlashPath,
							  mSoundManager, 
							  mScreenManager, 
							  mNetworkManager, 
							  0x40 << 16);
	if (mLog) mLog->FLogLine("    OK: 0x%08x", (intptr_t)mEmulator);
	mEmulator->SetNewtonID(mNewtonID0, mNewtonID1);

	mPlatformManager = mEmulator->GetPlatformManager();
	mPlatformManager->SetDocDir(dataPath);

	// Create the Overlay text window
	mScreenManager->OverlayClear();
	mScreenManager->OverlayOn();
	mScreenManager->OverlayPrintAt(0, 0, "Booting...", true);
	mScreenManager->OverlayPrintProgress(1, 0);
	mScreenManager->OverlayFlush();

	if (mLog) mLog->FLogLine("Creating helper thread.");
	pthread_t theThread;
	int theErr = ::pthread_create( &theThread, NULL, SThreadEntry, this );
	if (theErr) {
		if (mLog) mLog->FLogLine( "Error with pthread_create (%i)\n", theErr );
		::exit(2);
	}
	if (mLog) mLog->FLogLine("Booting NewtonOS...");
}


// -------------------------------------------------------------------------- //
// Quit the Main tread
// -------------------------------------------------------------------------- //
void 
TAndroidApp::Stop( void )
{
	mEmulator->Stop();
}


// -------------------------------------------------------------------------- //
// Wake up Emulator
// -------------------------------------------------------------------------- //
void
TAndroidApp::PowerOn( void )
{
#if 0
	mPlatformManager->PowerOn();
#else
	if (!mPlatformManager->IsPowerOn())
		mPlatformManager->SendPowerSwitchEvent();
#endif
}


// -------------------------------------------------------------------------- //
// Send Emulator to Sleep
// -------------------------------------------------------------------------- //
void
TAndroidApp::PowerOff( void )
{
#if 0
	mPlatformManager->PowerOff();
#else
	if (mPlatformManager->IsPowerOn())
		mPlatformManager->SendPowerSwitchEvent();
#endif
}


void TAndroidApp::reboot()
{
	TARMProcessor *cpu = mEmulator->GetProcessor();
	cpu->ResetInterrupt();
}

int TAndroidApp::IsPowerOn()
{
	return mPlatformManager->IsPowerOn();
}

void TAndroidApp::ChangeScreenSize(int w, int h)
{
	mScreenManager->ChangeScreenSize(w, h);
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
