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
#include "Emulator/Sound/TNullSoundManager.h"
#include "Emulator/Screen/TAndroidScreenManager.h"
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
mLog( nil )
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
}


// -------------------------------------------------------------------------- //
// Run( int, char** )
// -------------------------------------------------------------------------- //
void
TAndroidApp::Run(const char *dataPath)
{
	LOGE("TAndroidApp::Run() not yet implemented");
	
	/*
	mNetworkManager = NULL;
	mSoundManager = NULL;
	mScreenManager = NULL;
	mROMImage = NULL;
	mEmulator = NULL;
	mPlatformManager = NULL;
	mLog = NULL;
	
	// Create a log if possible
	//#ifdef _DEBUG
	mLog = new TStdOutLog(); 
	//#endif
	
	NSString* docdir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex: 0];
	
	// Create the ROM.
	
	NSString* einsteinRExPath = nil;
	NSBundle* thisBundle = [NSBundle mainBundle];
	
	if ( !(einsteinRExPath = [thisBundle pathForResource:@"Einstein" ofType:@"rex"]) )
	{
		//[self abortWithMessage: @"Couldn't load Einstein REX"];
		mLog->LogLine("Couldn't load Einstein REX");
		return 0;
	}
	
	NSString* theROMPath = [docdir stringByAppendingPathComponent:@"717006.rom"];
	NSString* theDebugROMPath = [docdir stringByAppendingPathComponent:@"717006.aif"];
	NSString* theDebugHighROMPath = [docdir stringByAppendingPathComponent:@"717006.rex"];
	NSString* theImagePath = [docdir stringByAppendingPathComponent:@"717006.img"];
    
	NSFileManager* theFileManager = [NSFileManager defaultManager];
    
    if ([theFileManager fileExistsAtPath:theROMPath]) {
        mROMImage = new TFlatROMImageWithREX(
											 [theROMPath fileSystemRepresentation],
											 [einsteinRExPath fileSystemRepresentation],
											 "717006", false,
											 [theImagePath fileSystemRepresentation]);
    } else if ([theFileManager fileExistsAtPath:theDebugROMPath]
			   &&[theFileManager fileExistsAtPath:theDebugHighROMPath]) {
        mROMImage = new TAIFROMImageWithREXes(
											  [theDebugROMPath fileSystemRepresentation],
											  [theDebugHighROMPath fileSystemRepresentation],
											  [einsteinRExPath fileSystemRepresentation],
											  "717006" );
    } else {
        fprintf(stderr, "ROM file required here:\n %s\nor here:\n %s\n %s\n\n",
                [theROMPath fileSystemRepresentation],
                [theDebugROMPath fileSystemRepresentation],
                [theDebugHighROMPath fileSystemRepresentation]);
		//[self abortWithMessage: @"ROM file not found"];
        [self explainMissingROM];
        mROMImage = 0L;
        return 0;
    }
	
	// Create the network manager.
	
	mNetworkManager = new TNullNetwork(mLog);
	
	// Create the sound manager.
	
	mSoundManager = new TCoreAudioSoundManager(mLog);
	
    // iPad is 1024x768. This size, and some appropriate scaling factors, should be selectable from
    // the 'Settings' panel.
	
    static int widthLUT[]  = { 320, 640, 384,  786 };
    static int heightLUT[] = { 480, 960, 512, 1024 };
    
    NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
	int index = [(NSNumber*)[prefs objectForKey:@"screen_resolution"] intValue];
	int newtonScreenWidth = widthLUT[index];
	int newtonScreenHeight = heightLUT[index];
	
	iEinsteinView* einsteinView = (iEinsteinView*)[self view];
	Boolean isLandscape = (newtonScreenWidth > newtonScreenHeight);
	
	mScreenManager = new TIOSScreenManager(
										   einsteinView,
										   self,
										   mLog,
										   newtonScreenWidth, newtonScreenHeight,
										   true,
										   isLandscape);
	
	[einsteinView setScreenManager:mScreenManager];
	
	// Create the emulator.
	
	NSString* theFlashPath = [docdir stringByAppendingPathComponent:@"flash"];
	printf("Flash file is %s\n", [theFlashPath fileSystemRepresentation]);
	
	mEmulator = new TEmulator(
							  mLog, mROMImage, [theFlashPath fileSystemRepresentation],
							  mSoundManager, mScreenManager, mNetworkManager, 0x40 << 16);
	
	mPlatformManager = mEmulator->GetPlatformManager();
    mPlatformManager->SetDocDir([docdir fileSystemRepresentation]);
    
	[einsteinView setEmulator:mEmulator];
	
	((TIOSScreenManager*)mScreenManager)->SetPlatformManager(mPlatformManager);
    
    return 1;
	*/
#if 0
	mProgramName = "AndriodEinstein";	
	const char* defaultMachineString = "717006";
	int theMachineID = flSettings->wMachineChoice->value();
	const Fl_Menu_Item *theMachineMenu = flSettings->wMachineChoice->menu()+theMachineID;
	const char* theMachineString = strdup((char*)theMachineMenu->user_data());
	const char* theRestoreFile = nil;
	const char* theSoundManagerClass = nil;
	const char* theScreenManagerClass = nil;
	const char *theROMImagePath = strdup(flSettings->ROMPath);
	const char *theFlashPath = strdup(flSettings->FlashPath);
	int portraitWidth = flSettings->screenWidth;
	int portraitHeight = flSettings->screenHeight;
	int ramSize = flSettings->RAMSize;
	Boolean fullscreen = (bool)flSettings->fullScreen;
	Boolean hidemouse = (bool)flSettings->hideMouse;
	Boolean useMonitor = (bool)flSettings->useMonitor;
	int useAIFROMFile = 0;	// 0 uses flat rom, 1 uses .aif/.rex naming, 2 uses Cirrus naming scheme
	
	int xx, yy, ww, hh;
	if (fullscreen) {
		Fl::screen_xywh(xx, yy, ww, hh, 0);
		portraitWidth = ww;
		portraitHeight = hh;
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
	
	Fl_Einstein_Window *win;
	if (fullscreen) {
		win = new Fl_Einstein_Window(xx, yy, portraitWidth, portraitHeight, this);
		win->border(0);
	} else {
		win = new Fl_Einstein_Window(portraitWidth, portraitHeight, this, "Einstein");
	}
	win->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
	win->callback(quit_cb, this);
	
	if (theSoundManagerClass == nil)
	{
		mSoundManager = new TWaveSoundManager( mLog );
	} else {
		CreateSoundManager( theSoundManagerClass );
	}
	if (theScreenManagerClass == nil)
	{
		//CreateScreenManager( "FL", portraitWidth, portraitHeight, fullscreen );
		CreateScreenManager( "FL", portraitWidth, portraitHeight, 0 );
	} else {
		CreateScreenManager( theScreenManagerClass, portraitWidth, portraitHeight, fullscreen );
	}
	if (theMachineString == nil)
	{
		theMachineString = defaultMachineString;
	}
	{
		// will we use an AIF image?
		{ 
			const char *ext = fl_filename_ext(theROMImagePath);
			if ( ext && stricmp(ext, ".aif")==0 )
			{
				useAIFROMFile = 1;
			}
			const char *name = fl_filename_name(theROMImagePath);
			if (	name 
				&& strncmp(name, "Senior Cirrus", 13)==0 
				&& strstr(name, "image"))
			{
				useAIFROMFile = 2;
			}
		}
		char theREX1Path[FL_PATH_MAX];
		strcpy(theREX1Path, theROMImagePath);
		char *rexName = (char*)fl_filename_name(theREX1Path);
		if (rexName) {
			strcpy(rexName, "Einstein.rex");
		}
		
		switch (useAIFROMFile) {
			case 0:
				mROMImage = new TFlatROMImageWithREX(
													 theROMImagePath, theREX1Path, theMachineString, useMonitor);
				break;
			case 1:
			{
				char theREX0Path[FL_PATH_MAX];
				strcpy(theREX0Path, theROMImagePath);
				fl_filename_setext(theREX0Path, FL_PATH_MAX, ".rex");
				mROMImage = new TAIFROMImageWithREXes(
													  theROMImagePath, theREX0Path, theREX1Path, theMachineString, useMonitor );
			}
				break;
			case 2:
			{
				char theREX0Path[FL_PATH_MAX];
				strcpy(theREX0Path, theROMImagePath);
				char *image = strstr(theREX0Path, "image");
				strcpy(image, "high");
				mROMImage = new TAIFROMImageWithREXes(
													  theROMImagePath, theREX0Path, theREX1Path, theMachineString, useMonitor );
			}
				break;
		}
		
		mEmulator = new TEmulator(
								  mLog, mROMImage, theFlashPath,
								  mSoundManager, mScreenManager, ramSize << 16 );
		mPlatformManager = mEmulator->GetPlatformManager();

		ILOG("Booting NewtonOS ROM...");
		
		Fl::lock();
		win->show(1, argv);
		if (hidemouse) {
			win->hideMouse();
		}
		
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
		
		mPipeServer.open();
		
		Fl::run();
		
		mPipeServer.close();
		
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
#endif
}


#if 0

// -------------------------------------------------------------------------- //
// ThreadEntry( void )
// -------------------------------------------------------------------------- //
void
TAndroidApp::ThreadEntry( void )
{
	if (mMonitor) {
		mMonitor->Run();
	} else {
		mEmulator->Run();
	}
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
TAndroidApp::Help( void )
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
TAndroidApp::Version( void )
{
	(void) ::printf( "%s\n", VERSION_STRING );
	(void) ::printf( "%s.\n", COPYRIGHT_STRING );
	::exit(0);
}

// -------------------------------------------------------------------------- //
// CreateLog( const char* )
// -------------------------------------------------------------------------- //
void
TAndroidApp::CreateLog( const char* inFilePath )
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
TAndroidApp::CreateSoundManager( const char* inClass )
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
TAndroidApp::CreateScreenManager(
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
		
#if 0
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
#else
		theWidth = inPortraitWidth;
		theHeight = inPortraitHeight;
#endif
		
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
void TAndroidApp::SyntaxError( const char* inBadOption )
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
void TAndroidApp::SyntaxError( void )
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


void TAndroidApp::quit_cb(Fl_Widget *, void *p) 
{
	TAndroidApp *my = (TAndroidApp*)p;
	//	my->mPlatformManager->PowerOff();
	my->mPlatformManager->SendPowerSwitchEvent();
}

void TAndroidApp::do_callback(Fl_Callback *cb, void *user)
{
	cb(flSettings->RMB, user);
}

void TAndroidApp::menuPower()
{
	mPlatformManager->SendPowerSwitchEvent();
}

void TAndroidApp::menuBacklight()
{
	mPlatformManager->SendBacklightEvent();
}

void TAndroidApp::menuInstallPackage()
{
	static char *filename = 0L;
	const char *newname = fl_file_chooser("Install Package...", "Package (*.pkg)", filename);
	if (newname) {
		if (!filename)
			filename = (char*)calloc(FL_PATH_MAX, 1);
		strncpy(filename, newname, FL_PATH_MAX);
		filename[FL_PATH_MAX] = 0;
		mPlatformManager->InstallPackage(filename);
	}
}

void TAndroidApp::menuAbout()
{
	static Fl_Window *flAbout = 0L;
	if (!flAbout)
		flAbout = createAboutDialog();
	flAbout->show();
}

void TAndroidApp::menuShowSettings() 
{
	flSettings->show();
}

void TAndroidApp::menuDownloadROM()
{
	static Fl_Window *downloadDialog = 0L;
	if (!downloadDialog) {
		downloadDialog = createROMDownloadDialog();
		wDownloadIP3->value("192");
		wDownloadIP2->value("168");
		wDownloadIP1->value("0");
		wDownloadIP0->value("24");
		wDownloadPort->value("10080");
		char path[FL_PATH_MAX]; path[0] = 0;
		fl_filename_absolute(path, ".");
		strcat(path, "myROM");
		wDownloadPath->copy_label(path);
	}
	downloadDialog->show();
}

int main(int argc, char** argv )
{
	TAndroidApp theApp;
	theApp.Run( argc, argv );
	return 0;
}

#endif

// ======================================================================= //
// We build our computer (systems) the way we build our cities: over time, 
// without a plan, on top of ruins.
//   -- Ellen Ullman
// ======================================================================= //
