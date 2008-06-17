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
#include "TFLSettings.h"

// ANSI C & POSIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <FL/x.H>
#include <FL/fl_draw.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>


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

#include "Monitor/TMonitor.h"
#include "Monitor/TSymbolList.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#include "Emulator/JIT/TJITPerformance.h"
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// Local Classes
// -------------------------------------------------------------------------- //

class Fl_Einstein_Window : public Fl_Window 
{
public:
	Fl_Einstein_Window(int ww, int hh, TFLApp *App, const char *ll=0)
		:	Fl_Window(ww, hh, ll), 
			app(App)
	{
	}
	Fl_Einstein_Window(int xx, int yy, int ww, int hh, TFLApp *App, const char *ll=0)
		:	Fl_Window(xx, yy, ww, hh, ll), 
			app(App)
	{
	}
	int handle(int event) 
	{
		if ( event==FL_PUSH && (
				(Fl::event_button()==3) || 
				(Fl::event_state()&(FL_SHIFT|FL_CTRL|FL_ALT|FL_META))==FL_CTRL) 
			)
		{
			const Fl_Menu_Item *choice = TFLSettings::menu_RMB->popup(Fl::event_x(), Fl::event_y());
			if (choice && choice->callback()) {
				app->do_callback(choice->callback());
			}
			return 1;
		}
		switch (event) {
			case FL_ENTER:
				if (hideMouse_) 
					fl_cursor(FL_CURSOR_NONE);
				break;
			case FL_LEAVE:
				fl_cursor(FL_CURSOR_DEFAULT);
				break;
		}
		return Fl_Window::handle(event);
	}
	void hideMouse() {
		fl_cursor(FL_CURSOR_NONE);
		hideMouse_ = 1;
	}
private:
	TFLApp	*app;
	int		hideMouse_;
};

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
		mLog( nil ),
		mMonitor( nil ),
		mSymbolList( nil ),
		flSettings(0L),
    mPipeServer(this)
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
	if (mMonitor)
	{
		delete mMonitor;
	}
	if (mSymbolList)
	{
		delete mSymbolList;
	}
}

// -------------------------------------------------------------------------- //
// Run( int, char** )
// -------------------------------------------------------------------------- //
void
TFLApp::Run( int argc, char* argv[] )
{
	mProgramName = argv[0];

	Fl::scheme("gtk+");
	Fl::args(1, argv);
	Fl::get_system_colors();

	flSettings = new TFLSettings(425, 392, "Einstein Platform Settings");
	flSettings->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
	flSettings->setApp(this, mProgramName);
	flSettings->loadPreferences();
	flSettings->revertDialog();
	Fl::focus(flSettings->wStart);

	if (!flSettings->dontShow) {
		flSettings->show(1, argv);
		while (flSettings->visible())
			Fl::wait();
	}
	flSettings->runningMode();
	Fl::focus(flSettings->wSave);

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
		
		if (useMonitor)
		{
			char theSymbolListPath[512];
			(void) ::snprintf( theSymbolListPath, 512, "%s/%s.symbols",
								theROMImagePath, theMachineString );
			mSymbolList = new TSymbolList( theSymbolListPath );
			mMonitor = new TMonitor( (TBufferLog*) mLog, mEmulator, mSymbolList );
		} else {
			(void) ::printf( "Booting...\n" );
		}

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
}

// -------------------------------------------------------------------------- //
// ThreadEntry( void )
// -------------------------------------------------------------------------- //
void
TFLApp::ThreadEntry( void )
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

void TFLApp::do_callback(Fl_Callback *cb, void *user)
{
	cb(flSettings->RMB, user);
}

void TFLApp::menuPower()
{
	mPlatformManager->SendPowerSwitchEvent();
}

void TFLApp::menuBacklight()
{
	mPlatformManager->SendBacklightEvent();
}

void TFLApp::menuInstallPackage()
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

void TFLApp::menuAbout()
{
	static Fl_Window *flAbout = 0L;
	if (!flAbout)
		flAbout = createAboutDialog();
	flAbout->show();
}

void TFLApp::menuShowSettings() 
{
	flSettings->show();
}

void TFLApp::menuDownloadROM()
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
	TFLApp theApp;
	theApp.Run( argc, argv );

#if defined(_MSC_VER) && defined(_DEBUG)
	FILE *log = fopen("einsteinPerfLog.txt", "wb");
	branchDestCount.print(log, TJITPerfHitCounter::kStyleMostHit+TJITPerfHitCounter::kStyleHex, 100);
	branchLinkDestCount.print(log, TJITPerfHitCounter::kStyleMostHit+TJITPerfHitCounter::kStyleHex, 100);
	fclose(log);
#endif

	return 0;
}

VOID WINAPI CompletedWriteRoutine(DWORD, DWORD, LPOVERLAPPED); 
VOID WINAPI CompletedReadRoutine(DWORD, DWORD, LPOVERLAPPED); 

TFLApp::TFLAppPipeServer::TFLAppPipeServer(TFLApp *app)
: app_(app),
  hPipeInst(INVALID_HANDLE_VALUE),
  hPipe(INVALID_HANDLE_VALUE)
{
  memset(&over_, 0, sizeof(over_));
}

TFLApp::TFLAppPipeServer::~TFLAppPipeServer()
{
  close();
}

int TFLApp::TFLAppPipeServer::open()
{
  CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_, this, 0, 0);
  return 0;
}

void TFLApp::TFLAppPipeServer::thread_(void *ps) {
  //over_.hEvent = CreateEvent(0L, TRUE, FALSE, 0L);
  //Fl::add_handler(
  TFLAppPipeServer *This = (TFLAppPipeServer*)ps;
  LPTSTR name = TEXT("\\\\.\\pipe\\einstein"); 
  This->hPipe = CreateNamedPipe(
    name,
    PIPE_ACCESS_DUPLEX /*| FILE_FLAG_OVERLAPPED*/,
    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
    PIPE_UNLIMITED_INSTANCES,
    4096, 4096, 5000, 0);
  if (This->hPipe==INVALID_HANDLE_VALUE) {
    printf("ERROR %d\n", GetLastError());
    return;
  }
  for (;;) {
    int ret = ConnectNamedPipe(This->hPipe, 0L);
    char buf[4096]; buf[0] = 0;
    DWORD n;
    ret = ReadFile(This->hPipe, buf, 4096, &n, 0L);
    printf("%d %d %s %d\n", ret, n, buf, GetLastError());
    if (strncmp(buf, "quit", 4)==0) {
      WriteFile(This->hPipe, "QUIT", 5, &n, 0);
      DisconnectNamedPipe(This->hPipe);
      ExitThread(0);
    } else if (strncmp(buf, "dons", 4)==0) {
      This->app_->getPlatformManager()->EvalNewtonScript(buf+4);
      WriteFile(This->hPipe, "OK", 3, &n, 0);
    } else if (strncmp(buf, "inst", 4)==0) {
      This->app_->getPlatformManager()->InstallPackage(buf+4);
    } else {
      WriteFile(This->hPipe, "ERR", 4, &n, 0);
    }
//    Fl::awake(awake_, ps);
    DisconnectNamedPipe(This->hPipe);
  }
  return;
}

void TFLApp::TFLAppPipeServer::awake_(void *ps) {
  /*
  TFLAppPipeServer *This = (TFLAppPipeServer*)ps;
  TFLApp *app = This->app_;
  app->

    EvalNewtonScript( const char* inNewtonScriptCode )
    InstallPackage( const char* inPackagePath )
    */
}

void TFLApp::TFLAppPipeServer::close()
{
  if (hPipe!=INVALID_HANDLE_VALUE) {
    char inbuf[4096];
    DWORD n;
    LPTSTR name = TEXT("\\\\.\\pipe\\einstein"); 
    CallNamedPipe(
      name,
      "quit", 5,
      inbuf, 4096,
      &n, 5000);
    puts(inbuf);
    //CloseHandle(hPipe); // FIXME: unsafe!
  }
}



// ======================================================================= //
// We build our computer (systems) the way we build our cities: over time, 
// without a plan, on top of ruins.
//   -- Ellen Ullman
// ======================================================================= //
