// ==============================
// File:			TFLApp.h
// Project:			Einstein
//
// Copyright 2003-2022 by Paul Guyot and Matthias Melcher.
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

#include "app/Version.h"

#include <K/Defines/KDefinitions.h>

#include <FL/Enumerations.H>
#if (FL_API_VERSION < 10400)
#error Einstein for FLTK requires FLTK 1.4.0 or higher to compile.
#endif
#include <FL/Fl_Widget.H>
#include <FL/x.H>

#include <string>
#include <vector>

typedef std::vector<std::string> StringList;

class TROMImage;
class TEmulator;
class TSoundManager;
class TScreenManager;
class TLog;
class TBufferLog;
class TPlatformManager;
class TNetworkManager;
class TPrinterManager;
class TMonitor;
class TSymbolList;
#if USE_TOOLKIT
class TToolkit;
#endif

class Fl_Widget;
class TFLApp;
class TFLSettingsUI;

extern TFLApp* gApp;

typedef std::vector<const char*> StringArray;

/**
 This is the central class that manages application launch and the user interface.
 */
class TFLApp
{
public:
	// --- Constructor and destructor

	// Constructor.
	TFLApp();

	// Destructor.
	~TFLApp();

	// no copy constructor
	TFLApp(const TFLApp& inCopy) = delete;

	// no assignment constructor
	TFLApp& operator=(const TFLApp& inCopy) = delete;

	// --- Startup and run the emulator.

	// Launch the app.
	void Run(int argc, char* argv[]);

	// --- User Actions

	// user wants to quit the emulator
	void UserActionQuit();

	// user pull power switch
	void UserActionTogglePower();

	// user toggles backlight
	void UserActionToggleBacklight();

	// install a package
	void InstallPackagesFromURI(const char* filenames);

	// user wants to install a package
	void UserActionInstallPackage();

	// user wants to reset or reboot the emulator
	void UserActionReset(int inType);

	// user wants to see the About window
	void UserActionShowAboutPanel();

	// user wants to see the Setting window
	void UserActionShowSettingsPanel();

	// user wants screen to be its original size
	void UserActionOriginalScreenSize();

	// user wants Einstein to take over the entire screen
	void UserActionToggleFullscreen();

	// user wants to download a ROM file from a physical device
	void UserActionFetchROM();

	// react to a right-click on the main screen
	void UserActionPopupMenu();

	// Show or raise or hide the Monitor window
	void UserActionToggleMonitor();

	// Show or raise or hide the Toolkit window
	void UserActionShowToolkit();

	// User wants to create a PCMCIA image from snapshot files and add it to the PCMCIA card list.
	int UserActionPCMCIAImageFromSnapshot(const char* dst, const char* data, const char* cis, const char* name);

	// User wants to insert or remove PCMCIA card into ot from controller 0.
	int UserActionPCCard(int inSlot, long inIndex);

	// User wants to keep this card in the PCCard slot during reboots
	int UserActionKeepPCCardInSlot(int inSlot, int inIndex);

	// User wants to print a screenshot
	void UserActionPrintScreen();

	// User wants to install essential software from UNNA and friends.
	void UserActionInstallEssentials();

	// ---  Events from within the emulator

	// this is called by the screen manager when the state of the backlight changed
	void PowerChangedEvent(Boolean);

	// this is called by the screen manager when the state of the backlight changed
	void BacklightChangedEvent(Boolean);

	// Newton OS needs a new screen size (usually by rotating the screen)
	void ResizeFromNewton(int w, int h);

	// --- Getter and setter

	TPlatformManager*
	GetPlatformManager()
	{
		return mPlatformManager;
	}

	TFLSettingsUI*
	GetSettings()
	{
		return mFLSettings;
	}

	const char* ChooseExistingFile(const char* message, const char* pat, const char* fname);
	const char* ChooseExistingDirectory(const char* message, const char* pat, const char* fname);
	const char* ChooseNewFile(const char* message, const char* pat, const char* fname);

private:
	void InitSettings();

	void InitFLTK(int argc, char** argv);

	void InitScreen();

	void InitSound();

	void InitNetwork();

	void InitSerialPorts();

	void InitMonitor(const char* theROMImagePath);

	void MountPCCardsKeptInSlot();

	// create the driver for our screen output
	void CreateScreenManager(
		const char* inClass,
		int inPortraitWidth,
		int inPortraitHeight,
		Boolean inFullScreen);

	// create a log file
	void CreateLog(const char* inPath);

	// run the emulation in a new thread
	void EmulatorThreadEntry(void);

	// called when the user wants to quit Einstein (close button on window decoration)
	void static quit_cb(Fl_Widget* w, void* p);

	// store the current size of the app window in mWindowed...
	void StoreAppWindowSize();

	void DeferredOnPowerRestored();

	// Variables
	const char* mProgramName = nullptr;
	TROMImage* mROMImage = nullptr;
	TEmulator* mEmulator = nullptr;
	TSoundManager* mSoundManager = nullptr;
	TScreenManager* mScreenManager = nullptr;
	TPlatformManager* mPlatformManager = nullptr;
	TNetworkManager* mNetworkManager = nullptr;
	TLog* mLog = nullptr;
	TBufferLog* mMonitorLog = nullptr;
	TMonitor* mMonitor = nullptr;
	TSymbolList* mSymbolList = nullptr;
	TFLSettingsUI* mFLSettings = nullptr;
#if USE_TOOLKIT
	TToolkit* mToolkit = nullptr;
#endif
	Fl_Widget* mNewtonScreen = nullptr;
	int mWindowedX = 150;
	int mWindowedY = 150;
	int mWindowedWidth = 320;
	int mWindowedHeight = 480;
	//    bool                mPresentEssentialsInstaller = false;
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
