// ==============================
// File:			TFLApp.h
// Project:			Einstein
//
// Copyright 2003-2020 by Paul Guyot and Matthias Melcher.
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

#ifndef _TFLAPP_H
#define _TFLAPP_H

#include <K/Defines/KDefinitions.h>

#include <FL/x.H>
#include <FL/Fl_Widget.H>

#include "Version.h"

class TROMImage;
class TEmulator;
class TSoundManager;
class TScreenManager;
class TLog;
class TPlatformManager;
class TNetworkManager;
class TMonitor;
class TSymbolList;

class Fl_Widget;
class TFLApp;
class TFLSettings;


extern TFLApp *gApp;


/**
 This is the central class that manages application launch and the user interface.
 */
class TFLApp
{
public:
    // Constructor.
	TFLApp();

    // Destructor.
	~TFLApp();

    // Launch the app.
	void Run( int argc, char* argv[] );

    // user wants to quit the emulator
    void UserActionQuit();

    // user pull power switch
	void UserActionTogglePower();

    // this is called by the screen manager when the state of the backlight changed
    void PowerChangedEvent(bool);

    // user toggles backlight
    void UserActionToggleBacklight();

    // this is called by the screen manager when the state of the backlight changed
    void BacklightChangedEvent(bool);

    // user toggles network card
    void UserActionToggleNetworkCard();

    // install a package
    void InstallPackagesFromURI(const char *filenames);

    // user wants to install a package
    void UserActionInstallPackage();

    // user wants to see the About window
	void UserActionShowAboutPanel();

    // user wants to see the Setting window
	void UserActionShowSettingsPanel();

    // user wants to download a ROM file from a physical device
	void UserActionFetchROM();

    // get the interface to the running emulations
    TPlatformManager *getPlatformManager() { return mPlatformManager; }

    // no copy constructor
    TFLApp( const TFLApp& inCopy ) = delete;

    // no assignment constructor
    TFLApp& operator = ( const TFLApp& inCopy ) = delete;

    // react to a right-click on the main screen
    void UserActionPopupMenu();

private:

    // create the driver for our screen output
	void CreateScreenManager(
				const char* inClass,
				int inPortraitWidth,
				int inPortraitHeight,
				bool inFullScreen);

    // create a log file
	void CreateLog( const char* inPath );

    // run the emulation in a new thread
	void EmulatorThreadEntry( void );

    // called when the user wants to quit Einstein (close button on window decoration)
	void static quit_cb(Fl_Widget *w, void *p);

	// Variables
    const char*			mProgramName = nullptr;
    TROMImage*			mROMImage = nullptr;
    TEmulator*			mEmulator = nullptr;
    TSoundManager*		mSoundManager = nullptr;
    TScreenManager*		mScreenManager = nullptr;
    TPlatformManager*	mPlatformManager = nullptr;
    TNetworkManager*	mNetworkManager = nullptr;
    TLog*				mLog = nullptr;
    TMonitor*			mMonitor = nullptr;
    TSymbolList*		mSymbolList = nullptr;
    TFLSettings*        mFLSettings = nullptr;
};

#endif
		// _TFLAPP_H

// ============================================================================ //
// Mac Airways:                                                                 //
// The cashiers, flight attendants and pilots all look the same, feel the same  //
// and act the same. When asked questions about the flight, they reply that you //
// don't want to know, don't need to know and would you please return to your   //
// seat and watch the movie.                                                    //
// ============================================================================ //
