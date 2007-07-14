// ==============================
// File:			TFLApp.cp
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
#include "TFLApp.h"

// ANSI C & POSIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>


// Einstein
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/Sound/TNullSoundManager.h"
#include "Emulator/Sound/TWaveSoundManager.h"
#include "Emulator/Screen/TFLScreenManager.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/Log/TFileLog.h"
#include "Emulator/Log/TBufferLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TFLApp( void )
// -------------------------------------------------------------------------- //
TFLApp::TFLApp( void )
	:
		mProgramName( nil ),
		mROMImage( nil ),
		mEmulator( nil ),
		mSoundManager( nil ),
		mScreenManager( nil ),
		mPlatformManager( nil ),
		mLog( nil )
{
}

// -------------------------------------------------------------------------- //
//  * ~TFLApp( void )
// -------------------------------------------------------------------------- //
TFLApp::~TFLApp( void )
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
}

// -------------------------------------------------------------------------- //
// Run( int, char** )
// -------------------------------------------------------------------------- //
void
TFLApp::Run( int argc, char* argv[] )
{
	mProgramName = argv[0];
	
	const char* defaultMachineString = "717006";
	const char* theMachineString = nil;
	const char* theRestoreFile = nil;
	const char* theSoundManagerClass = nil;
	const char* theScreenManagerClass = nil;
	int portraitWidth = TScreenManager::kDefaultPortraitWidth;
	int portraitHeight = TScreenManager::kDefaultPortraitHeight;
	int ramSize = 0x40;
	Boolean fullscreen = false;		// Default is not full screen.
	Boolean useAIFROMFile = false;	// Default is to use flat rom format.
	
	int indexArgs = 1;
	if (argc < 2)
	{
		SyntaxError();
	}
	
	if ((::strcmp(argv[1], "--help") == 0)
				|| (::strcmp(argv[1], "-h") == 0)) {
		Help();
	} else if ((::strcmp(argv[1], "--version") == 0)
				|| (::strcmp(argv[1], "-v") == 0)) {
		Version();
	}
	
	while (indexArgs < argc - 1)
	{
		if (::strcmp(argv[indexArgs], "-a") == 0)
		{
			indexArgs++;
			if ((indexArgs == argc - 1) || (mSoundManager != nil))
			{
				SyntaxError( argv[indexArgs-1] );
			}
			
			theSoundManagerClass = argv[indexArgs];
		} else if (::strncmp(argv[indexArgs], "--audio=", 8) == 0) {
			if (mSoundManager != nil)
			{
				SyntaxError( argv[indexArgs] );
			}
			
			theSoundManagerClass = &argv[indexArgs][8];
		} else if (::strcmp(argv[indexArgs], "-s") == 0) {
			indexArgs++;
			if ((indexArgs == argc - 1) || (mScreenManager != nil))
			{
				SyntaxError( argv[indexArgs-1] );
			}
			
			theScreenManagerClass = argv[indexArgs];
		} else if (::strncmp(argv[indexArgs], "--screen=", 9) == 0) {
			if (mScreenManager != nil)
			{
				SyntaxError( argv[indexArgs] );
			}
			
			theScreenManagerClass = &argv[indexArgs][9];
		} else if (::strcmp(argv[indexArgs], "-l") == 0) {
			indexArgs++;
			if ((indexArgs == argc - 1) || (mLog != nil))
			{
				SyntaxError( argv[indexArgs-1] );
			}
			
			CreateLog( argv[indexArgs] );
		} else if (::strncmp(argv[indexArgs], "--log=", 6) == 0) {
			if (mLog != nil)
			{
				SyntaxError( argv[indexArgs] );
			}
			
			CreateLog( &argv[indexArgs][6] );
		} else if (::strcmp(argv[indexArgs], "-r") == 0) {
			indexArgs++;
			if ((indexArgs == argc - 1) || (theRestoreFile != nil))
			{
				SyntaxError( argv[indexArgs-1] );
			}
			
			theRestoreFile = argv[indexArgs];
		} else if (::strncmp(argv[indexArgs], "--restore=", 10) == 0) {
			if (theRestoreFile != nil)
			{
				SyntaxError( argv[indexArgs] );
			}
			
			theRestoreFile = &argv[indexArgs][10];
		} else if (::strcmp(argv[indexArgs], "-m") == 0) {
			indexArgs++;
			if ((indexArgs == argc - 1) || (theMachineString != nil))
			{
				SyntaxError( argv[indexArgs-1] );
			}
			
			theMachineString = argv[indexArgs];
		} else if (::strncmp(argv[indexArgs], "--machine=", 10) == 0) {
			if (theMachineString != nil)
			{
				SyntaxError( argv[indexArgs] );
			}
			
			theMachineString = &argv[indexArgs][10];
		} else if (::sscanf(argv[indexArgs], "--width=%i", &portraitWidth) == 1) {
			if (portraitWidth < (int) TScreenManager::kDefaultPortraitWidth)
			{
				(void) ::fprintf(
					stderr,
					"Warning, width is smaller than original (%i)\n",
					(int) TScreenManager::kDefaultPortraitWidth);
			}
		} else if (::sscanf(argv[indexArgs], "--height=%i", &portraitHeight) == 1) {
			if (portraitHeight < (int) TScreenManager::kDefaultPortraitHeight)
			{
				(void) ::fprintf(
					stderr,
					"Warning, height is smaller than original (%i)\n",
					(int) TScreenManager::kDefaultPortraitHeight);
			}
		} else if (::sscanf(argv[indexArgs], "--ram=%i", &ramSize) == 1) {
			if ((ramSize < 1) || (ramSize > 0x100))
			{
				(void) ::fprintf(
					stderr,
					"Ram size must be between 1 and 255 (for now, because only "
					"first bank is handled)\nI'll boot with 4 MB (64).\n");
				ramSize = 0x40;
			}
		} else if (::strcmp(argv[indexArgs], "--aif") == 0) {
			useAIFROMFile = true;
		} else if (::strcmp(argv[indexArgs], "--fullscreen") == 0) {
			fullscreen = true;
		} else if ((::strcmp(argv[indexArgs], "--help") == 0)
					|| (::strcmp(argv[indexArgs], "-h") == 0)) {
			Help();
		} else if ((::strcmp(argv[indexArgs], "--version") == 0)
					|| (::strcmp(argv[indexArgs], "-v") == 0)) {
			Version();
		} else {
			SyntaxError( argv[indexArgs] );
		}
		
		indexArgs++;
	}
	
	if (portraitHeight < portraitWidth)
	{
		(void) ::fprintf(
			stderr,
			"Warning, (portrait) height (%i) is smaller than width (%i). Boot screen won't be displayed properly\n",
			portraitHeight,
			portraitWidth );
	}

	(void) ::printf( "Welcome to Einstein console.\n" );
	(void) ::printf( "This is %s.\n", VERSION_STRING );

	Fl_Window *win = new Fl_Window(portraitWidth, portraitHeight+30, "Einstein");
	win->callback(quit_cb, this);

	if (theSoundManagerClass == nil)
	{
		mSoundManager = new TWaveSoundManager( mLog );
	} else {
		CreateSoundManager( theSoundManagerClass );
	}
	if (theScreenManagerClass == nil)
	{
		CreateScreenManager( "FL", portraitWidth, portraitHeight, fullscreen );
	} else {
		CreateScreenManager( theScreenManagerClass, portraitWidth, portraitHeight, fullscreen );
	}
	if (theMachineString == nil)
	{
		theMachineString = defaultMachineString;
	}
	{
		const char* theDataPath = argv[argc - 1];
		char theROMImagePath[512];
		char theREX1Path[512];
		char theFlashPath[512];
		(void) ::sprintf( theREX1Path, "%s/Einstein.rex", theDataPath );
		(void) ::sprintf( theFlashPath, "%s/flash", theDataPath );
		
		if (useAIFROMFile)
		{
			char theREX0Path[512];
			(void) ::sprintf( theREX0Path, "%s/%s.rex", theDataPath, theMachineString );
			(void) ::sprintf( theROMImagePath, "%s/%s.aif", theDataPath, theMachineString );
			mROMImage = new TAIFROMImageWithREXes(
				theROMImagePath, theREX0Path, theREX1Path, theMachineString, false );
		} else {
			(void) ::sprintf( theROMImagePath, "%s/%s", theDataPath, theMachineString );
			mROMImage = new TFlatROMImageWithREX(
				theROMImagePath, theREX1Path, theMachineString, false );
		}
		mEmulator = new TEmulator(
					mLog, mROMImage, theFlashPath,
					mSoundManager, mScreenManager, ramSize << 16 );
		mPlatformManager = mEmulator->GetPlatformManager();
		
		(void) ::printf( "Booting...\n" );

		Fl::lock();
		win->show(1, argv);

#if TARGET_OS_WIN32
		HANDLE theThread = CreateThread(0L, 0, (LPTHREAD_START_ROUTINE)SThreadEntry, this, 0, 0L);
#else
		pthread_t theThread;
		int theErr = ::pthread_create( &theThread, NULL, SThreadEntry, this );
		if (theErr) 	{
			(void) ::fprintf( stderr, "Error with pthread_create (%i)\n", theErr );
			::exit(2);
		}
#endif

		Fl::run();

		// FIXME Tell the emulator that the power was switched off
		// FIXME Then wait for it to quit gracefully
		//mPlatformManager->PowerOff();
		mEmulator->Quit();
		mQuit = true;

		// Wait for the thread to finish.
#if TARGET_OS_WIN32
		WaitForSingleObject(theThread, INFINITE);
#else
		(void) ::pthread_join( theThread, NULL );
#endif

	}
}

// -------------------------------------------------------------------------- //
// ThreadEntry( void )
// -------------------------------------------------------------------------- //
void
TFLApp::ThreadEntry( void )
{
	mEmulator->Run();
	mQuit = true;
}

/* FIXME call at least these:
		mPlatformManager->InstallPackage( theArg );
		mPlatformManager->EvalNewtonScript( theArg );
		mPlatformManager->SendPowerSwitchEvent();
		mPlatformManager->SendBacklightEvent();
		mEmulator->InsertCard();
		mEmulator->SaveState( theArg );
		mQuit = true; // quitting
*/

// -------------------------------------------------------------------------- //
// Help( void )
// -------------------------------------------------------------------------- //
void
TFLApp::Help( void )
{
	(void) ::printf(
				"%s - Einstein Platform\n",
				mProgramName );
	(void) ::printf(
				"%s [options] data_path\n",
				mProgramName );
	(void) ::printf(
				"  -a | --audio=audiodriver        (null, wave)\n" );
	(void) ::printf(
				"  -s | --screen=screen driver     (FL)\n" );
	(void) ::printf(
				"  --width=portrait width          (default is 320)\n" );
	(void) ::printf(
				"  --height=portrait height        (default is 480)\n" );
	(void) ::printf(
				"  -l | --log=log file             (default to no log)\n" );
	(void) ::printf(
				"  -r | --restore=restore file     (default to start from scratch)\n" );
	(void) ::printf(
				"  -m | --machine=machine string   (717006, 737041, 747129)\n" );
	(void) ::printf(
				"  --monitor                       monitor mode\n" );
	(void) ::printf(
				"  --ram=size                      ram size in 64 KB (1-255) (default: 64, i.e. 4 MB)\n" );
//	(void) ::printf(
//				"  --aif                           read aif files\n" );
	::exit(1);
}

// -------------------------------------------------------------------------- //
// Version( void )
// -------------------------------------------------------------------------- //
void
TFLApp::Version( void )
{
	(void) ::printf( "%s\n", VERSION_STRING );
	(void) ::printf( "%s.\n", COPYRIGHT_STRING );
	::exit(0);
}

// -------------------------------------------------------------------------- //
// CreateLog( const char* )
// -------------------------------------------------------------------------- //
void
TFLApp::CreateLog( const char* inFilePath )
{
	if (mLog)
	{
		(void) ::printf( "A log already exists (--monitor & --log are exclusive)\n" );
		::exit(1);
	}
	mLog = new TFileLog( inFilePath );
}

// -------------------------------------------------------------------------- //
// CreateSoundManager( const char* )
// -------------------------------------------------------------------------- //
void
TFLApp::CreateSoundManager( const char* inClass )
{
	if (::strcmp( inClass, "null" ) == 0)
	{
		mSoundManager = new TNullSoundManager( mLog );
	} else if (::strcmp( inClass, "wave" ) == 0) {
		mSoundManager = new TWaveSoundManager( mLog );
	} else {
		(void) ::fprintf( stderr, "Unknown sound manager class %s\n", inClass );
		::exit( 1 );
	}
}

// -------------------------------------------------------------------------- //
// CreateScreenManager( const char*, int, int, Boolean )
// -------------------------------------------------------------------------- //
void
TFLApp::CreateScreenManager(
				const char* inClass,
				int inPortraitWidth,
				int inPortraitHeight,
				Boolean inFullScreen)
{	
	if (::strcmp( inClass, "FL" ) == 0)
	{
		Boolean screenIsLandscape = true;

		KUInt32 theWidth;
		KUInt32 theHeight;
		if (inFullScreen)
		{
			KUInt32 theScreenWidth;
			KUInt32 theScreenHeight;
			TFLScreenManager::GetScreenSize(&theScreenWidth, &theScreenHeight);
			if (theScreenWidth >= theScreenHeight)
			{
				screenIsLandscape = true;
				theWidth = inPortraitHeight;
				theHeight = inPortraitWidth;
			} else {
				screenIsLandscape = false;
				theWidth = inPortraitWidth;
				theHeight = inPortraitHeight;
			}
		} else {
			theWidth = inPortraitWidth;
			theHeight = inPortraitHeight;
		}

		mScreenManager = new TFLScreenManager(
									mLog,
									theWidth,
									theHeight,
									inFullScreen,
									screenIsLandscape);
	} else {
		(void) ::fprintf( stderr, "Unknown screen manager class %s\n", inClass );
		::exit( 1 );
	}
}

// -------------------------------------------------------------------------- //
// SyntaxError( const char* )
// -------------------------------------------------------------------------- //
void TFLApp::SyntaxError( const char* inBadOption )
{
	(void) ::fprintf(
				stderr,
				"%s -- syntax error with option %s\n",
				mProgramName,
				inBadOption );
	(void) ::fprintf(
				stderr,
				"Try %s --help for more help\n",
				mProgramName );
	::exit(1);
}

// -------------------------------------------------------------------------- //
// SyntaxError( void )
// -------------------------------------------------------------------------- //
void TFLApp::SyntaxError( void )
{
	(void) ::fprintf(
				stderr,
				"%s -- syntax error\n",
				mProgramName );
	(void) ::fprintf(
				stderr,
				"syntax is %s [options] path_to_data\n",
				mProgramName );
	(void) ::fprintf(
				stderr,
				"Try %s --help for more help\n",
				mProgramName );
	::exit(1);
}


void TFLApp::quit_cb(Fl_Widget *, void *p) 
{
	TFLApp *my = (TFLApp*)p;
//	my->mPlatformManager->PowerOff();
	my->mPlatformManager->SendPowerSwitchEvent();
}

int main(int argc, char** argv )
{
	TFLApp theApp;
	theApp.Run( argc, argv );
	return 0;
}


// ======================================================================= //
// We build our computer (systems) the way we build our cities: over time, 
// without a plan, on top of ruins.
//   -- Ellen Ullman
// ======================================================================= //
