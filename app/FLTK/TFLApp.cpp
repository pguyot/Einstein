// ==============================
// File:			TFLApp.cp
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

// ----- ASAPs
// TODO: compile for Android
// TODO: FIX 8 and 16 MB internal Flash support

// ----- User Interface improvements
// TODO: option to launch Monitor stopped at boot point
//       Note: the Monitor logging, locking and event mechanism is pretty broken
//       Log messages should be displayed and scroll as they come in
//       Quitting the emulator while the Monitor is visible will lock the app
//       Monitor does not switch to the Log-only screen when running
//       Monitor does not show the assembler commands as we step through them
// TODO: make FKey Bar for eMate emulation with volume slider (screenshot)
// TODO: make main menu and toolbar optional
// TODO: is the drop-down menu still on par?
// TODO: allow skeuomorphism (use photo of MP as an outline in the UI)

// ----- Minor Improvemnts in Usability
// TODO: menu and action to reboot Newton (in different configurations)
//       Reboot__FlUlUc: @ 0x000D9884: Reboot(long, unsigned long, unsigned char)
//       PowerOffAndReboot__Fl: @ 0x000E6BBC: PowerOffAndReboot(long)
//       ZapInternalStoreCheck__Fv: @ 0x00113CBC: ZapInternalStoreCheck(void)
//       FReboot: @ 0x00146AF8 0xE1A0C00D - ....
//       WarmBoot: 0x0038D1E0
// TODO: install essentials
// TODO: drag'n'drop of multiple files and archives
// Done: drag'n'drop from network locations (https:, etc.)
// TODO: drag'n'drop for the Unna Archive
// TODO: option to load and save complete images including ROM, RAM, Flash, and PCMCIA memory snapshots
// TODO: automated Internet access (install and setup)
// TODO: add preferences to point to a UNNA archive image, so we can browse that and install quickly
// TODO: Toolkit: add global functions to handle images, sounds and other external binary data
// TODO: Toolkit: improve decompiler to generate external binary data and reference it
// TODO: Toolkit: reverse bytecode to source code
// Done: Set NewtonOS to Host time (in minutes since Jan 1 1904). Set Locale?
//       Calculate minutes in the current Locale.

// ----- Major new Features
// TODO: NTK Monitor
// TODO: Patch ROMs for Y10k bug (Y26k bug coming up quickly!)
// TODO: Printer support
// Done: Printing screen shots
// TODO: Wake-up/launch on appointment in the future

// ----- Improvemnets to the inner workings
// TODO: Full Android support as an address book and calender app
// TODO: Fix locks and race conditions
// Done: cleanup all compile warnings on all platforms
// TODO: Linux: App Icon, Flatpak?

// ----- Documentations
// TODO: about panel must have all authors and references to the linked libraries (FLTK, ...)
// TODO: release notes
// TODO: help pages for use of Einstein, Monitor, etc.
// TODO: help for getting started with NewtonOS, links

// ----- Flash Memeory Settings
// TODO: even if internal Flash is set to 4MB, it reports 8MB in sum. Also, we can't allocate more than 4MB in settings without crash. Original Einstein works.

// ----- Ethernet Emulation
// TODO: NPDS not working: incomming TCP/IP connections don't work

/*

Documentation:

User's Manual: Title, Introduction and Purpose, Setup guide, Use Guide for all features, Appendices (Resources, Licenses, Contact Information)
 - it's not a nove, users want to get started quickly
 - what is the purpose of this manual
 - don't leave out even 'obvious' steps when explaining something blow by blow
 - who's the audience? What do they want to achieve? User may not have previous knowledge.
 - collect all information that you want to write down
 - structure the colection, create categories by what users may be trying to find
 - write the content, focus on the goal, help the user solve his issue
 - use numbered lists
 - add structure aith the Table of Contents
 - add graphics, screenshots, and even video (explaining stuff in words isboring)

 Title
  - app name, copyright, authors, linked software and their licenses
 Introduction
  - One short paragraph about the MessagePad/eMate300
  - What is an emulator
  - What does Einstein emulate (and what is lacking)
  - Supported Platforms
 First launch
  - What is a ROM and where to get it
  - What is Flash, and how do I set it up
  - How much system RAM to choose
  - Launching an quitting the meulator
 Essential Apps
  - the Y10k patch
  - built-in NewtTest
 How to use the main screen
  - resizing and fullscreen vs. changing resolution
  - copy and paste
  - drag and drop
  - installing apps from disk and internet
  - menus
 How to sync over the 'serial' line
  - (built-in Einstein Prefs)
  - the Dock and its counterparts on various platforms (does this warrant its own chapter?)
 Networking setup
  - install apps with direct links
  - what to enter in the dialogs
  - browser, eMail, Network Dock
 PCMCIA Card Setup
  - how to create new cards
  - how to use card images
  - how to save and backup cards
  - how to dump physical crads int a card image
 Configuring multiple MessagePads and eMates
  - detailed settings for a new machine
  - detailed settings for a hardware
  - how to launch a different machine setup
 Toolkit overview
  - loading and launching sample apps
 Monitor overview
  - breaking into machine code
  - single stepping
  - looking at ram and registers
  - continue execution
 Appendices
  - more Resources
  - Licenses
  - Contact Information

Developer's Documentation: Basic Ideas, Basic Features, Detailed Class Reference (Doxygen)

*/

/*
 Alternative menus:
 - Show Device Bezels
 - Stay on top
 ----
 - Physical size  CMD-1
 - Point accurate CMD-2
 - Pixel Accurate CMD-3
 - Fit Screen     CMD-4
 */

/*
 Einstein threads:

 - Mainthread: GUI
	The main thread at startup runs all code that creates windows, destroys windows,
	or handles user interaction. In FLTK, callbacks are always running in the main
	thread. If other threads need to call FLTK, the you should do it by guarding
	the call with `Fl::lock(); ... ; Fl::unlock();` and possibly `myWidget->redraw();`

- Emulator/Monitor thread:
	The Emulator runs inside its own thread. If the Monitor is enabled, the Emulator
	runs within the Monitor thread.

 - Interrupt Manager:
	The interrupt manager runs within its own thread.

 - UsermodeNetwork thread:
	Parts of the usermode network driver run within a thread.

 - TCP Serial Port:
	All TCP communications runs within a thread

 */

#include <K/Defines/KDefinitions.h>

#include "TFLApp.h"
#include "TFLAppUI.h"
#include "TFLAppWindow.h"
#include "TFLSettingsUI.h"
#if USE_TOOLKIT
#include "Toolkit/TToolkit.h"
#endif

// ANSI C & POSIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// C++17
#include <filesystem>
#include <thread>

// FLTK user interface
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Paged_Device.H>
#include <FL/Fl_Printer.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/x.H>
#if TARGET_OS_MAC
#include <FL/Fl_Sys_Menu_Bar.H>
#endif

// Einstein
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TBufferLog.h"
#include "Emulator/Log/TFileLog.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Network/TUsermodeNetwork.h"
#include "Emulator/PCMCIA/TLinearCard.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/Screen/TFLScreenManager.h"
#include "Emulator/Serial/TSerialPortManager.h"
#include "Emulator/Serial/TSerialPorts.h"
#include "Emulator/Serial/TTcpClientSerialPortManager.h"
#include "Emulator/Sound/TNullSoundManager.h"

// Http Client GET
#undef min
#include "HTTPRequest.hpp"

// Additional managers for every supported platform
#if TARGET_OS_WIN32
#include "Emulator/Sound/TWaveSoundManager.h"
#elif TARGET_OS_LINUX
#include "Emulator/Sound/TPulseAudioSoundManager.h"
#elif TARGET_OS_MAC
#include "Emulator/Sound/TCoreAudioSoundManager.h"
#else
#error Selected target OS not implemented, or no target OS selected
#endif

// Monitor system for debugging ARM code
#include "Monitor/TFLMonitor.h"
#include "Monitor/TSymbolList.h"

enum class FileChooser {
	SaveFile,
	OpenFile,
	ChooseDirectory
};
static const char* tfl_file_chooser(const char* message, const char* pat, const char* fname, FileChooser type);

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// Local Classes
// -------------------------------------------------------------------------- //

TFLApp* gApp = nullptr;

// MARK: - Public: -
// --- Constructor and destructor

/**
 Constructor for the app.
 */
TFLApp::TFLApp(void) = default;

/**
 Clean up time.
 */
TFLApp::~TFLApp(void)
{
	delete mEmulator;
	delete mScreenManager;
	delete mSoundManager;
	delete mNetworkManager;
	delete mLog;
	delete mMonitorLog;
	delete mROMImage;
	delete mMonitor;
	delete mSymbolList;
	delete mFLSettings;
}

// MARK: -
// --- Startup and run the emulator.

/**
 Run EInstein.

 \todo Must urgently refactor this so it becomes readable again.
 */
void
TFLApp::Run(int argc, char* argv[])
{
	mProgramName = argv[0];

	InitFLTK(argc, argv);

	InitSettings();

	int ramSize = mFLSettings->RAMSize;
	Boolean hidemouse = (Boolean) mFLSettings->hideMouse;

	KPrintf("Welcome to Einstein console.\n");
	KPrintf("This is %s.\n", VERSION_STRING);

	static char theROMImagePath[FL_PATH_MAX];

	for (Boolean firstAttempt = true;; firstAttempt = false)
	{
		if (!firstAttempt || !mFLSettings->dontShow)
			mFLSettings->ShowSettingsPanelModal();
		strncpy(theROMImagePath, mFLSettings->ROMPath, FL_PATH_MAX);
		mROMImage = TROMImage::LoadROMAndREX(theROMImagePath, 1, mFLSettings->mUseBuiltinRex);
		if (!mROMImage)
		{
			fl_alert("Can't load ROM image.\nFile format not supported.");
			continue;
		}
		if (mROMImage->GetErrorCode() == TROMImage::kNoError)
			break;
		switch (mROMImage->GetErrorCode())
		{
			case TROMImage::kErrorLoadingROMFile:
				fl_alert("Can't load ROM file\n%s\n%s", theROMImagePath, strerror(errno));
				break;
			case TROMImage::kErrorLoadingNewtonREXFile:
				fl_alert("Can't load Newton REX file\n%s\n%s", theROMImagePath, strerror(errno));
				break;
			case TROMImage::kErrorLoadingEinsteinREXFile:
				fl_alert("Can't load Einstein REX file\n%s\n%s", theROMImagePath, strerror(errno));
				break;
			case TROMImage::kErrorWrongSize:
				fl_alert("Can't load ROM file\n%s\nUnexpected file size.", theROMImagePath);
				break;
		}
		delete mROMImage;
		// go back to showing the settings panel
	}

	const char* theFlashPath = strdup(mFLSettings->FlashPath);
	//    std::filesystem::path flashPath( theFlashPath );
	//    mPresentEssentialsInstaller = !std::filesystem::exists(flashPath);

	InitScreen();

	InitSound();

	InitNetwork();

	// MP2000: 1MB Dynamic RAM, 4MB Flash RAM
	// MP2100: 4MB Dynamic RAM, 4MB Flash RAM
	// eMate:  1MB Dynamic RAM, 2MB Flash RAM
	mEmulator = new TEmulator(mLog, mROMImage, theFlashPath,
		mSoundManager, mScreenManager, mNetworkManager, ramSize << 16);
	mPlatformManager = mEmulator->GetPlatformManager();

	// yes, this is valid C++ code; it tells the emulator to call us so we can tell FLTK to
	// call us again later from the main thread which then closes all windows, terminating
	// the main application loop which then terminates the thread that called us to begin with.
	// Or as Mony says: "Would That It Were So Simple"
	// mEmulator->CallOnQuit([](){Fl::awake([](void*){gApp->UserActionQuit();});});

	// This is called after NewtonOS booted or was restored from sleep.
	// It may be called in a few other instances as well.
	mEmulator->OnPowerRestored(
		[]() {
			Fl::awake([](void*) { gApp->DeferredOnPowerRestored(); });
			//            if (gApp->mPresentEssentialsInstaller)
			//                Fl::awake([](void*){gApp->UserActionInstallEssentials();});
		});

	InitSerialPorts(); // do this after creating the emulator

	InitMonitor(theROMImagePath);
	if (mMonitor)
		mMonitor->RunOnStartup(true);

	MountPCCardsKeptInSlot();

	Fl::lock();
	wAppWindow->show(1, argv);
	StoreAppWindowSize();
	if (hidemouse)
	{
		wAppWindow->HideMousePointer();
	}

	if (mFLSettings->mLaunchMonitorAtBoot)
	{
		mMonitor->Show();
		// if (mFLSettings->mBreatAtROMBoot) {
		//     mMonitor->Stop();
		// }
	}
	// launch the actual emulation in the background
	auto emulatorThread = new std::thread(&TFLApp::EmulatorThreadEntry, this);

	// run the user interface until all windows are close
	Fl::run();
	Fl::unlock();

	// if the emulator does not know yet, tell it to wrap things up and quit
	mEmulator->Quit();
	mPlatformManager->InsertPCCard(0, nullptr); // Flush memory card contents
	mPlatformManager->InsertPCCard(1, nullptr);

	// also, let the Monitor know that we are leaving
	if (mMonitor)
		mMonitor->Stop();

	// This is a good time to save preferences that might have changed while running
	if (wAppWindow->fullscreen_active())
	{
		mFLSettings->mAppWindowPosX = mWindowedX;
		mFLSettings->mAppWindowPosY = mWindowedY;
	} else
	{
		mFLSettings->mAppWindowPosX = wAppWindow->x();
		mFLSettings->mAppWindowPosY = wAppWindow->y();
	}
	mFLSettings->savePreferences();

	// wait for the emulator to finish before we leave the house, too and lock the doors
	emulatorThread->join();
}

// MARK: -
// --- User Actions

/**
 User wants to quit the emulator and leave the app.
 */
void
TFLApp::UserActionQuit()
{
#if USE_TOOLKIT
	// Close the Toolkit window, so it can save its coordinates in the prefrences
	if (mToolkit)
		mToolkit->Hide();
#endif

	// tell the emulator to shut everything down
	if (mEmulator)
	{
		mEmulator->Quit();
	}

	// closing all windows will end Fl::run();
	Fl_Window* w;
	while ((w = Fl::first_window()))
	{
		w->hide();
	}

	// afte Fl::run() if finished, TFLApp waits for the emulator
	// process to finish as well.
}

/**
 User wants us to toggle the power switch.
 \fixme This is currently the same as Menu Quit
 */
void
TFLApp::UserActionTogglePower()
{
	mPlatformManager->SendPowerSwitchEvent();
}

/**
 User wants us to toggle the backlight.
 */
void
TFLApp::UserActionToggleBacklight()
{
	mPlatformManager->SendBacklightEvent();
}

#if TARGET_OS_WIN32
static int
strcasecmp(const char* a, const char* b)
{
	return stricmp(a, b);
}
#endif

/**
 Install a package from a file.

 \param filenames This is a list of filenames, separated by \n.

 \todo Support packages that are compressed in the common old compression formats .hqx, .sit, .zip, .sit.hqx, .sae(?)
 \todo grab apckages from the net using https://github.com/elnormous/HTTPRequest
 */
void
TFLApp::InstallPackagesFromURI(const char* filenames)
{
	// bail early if the filenames are emoty
	if (!filenames || !*filenames)
		return;

	// TODO: do we have to handle backslashes on MSWindows?

	// Check the filename extension! Do that in the event handler too.
	char* fName = strdup(filenames);

	// grab the start of the first filename
	char* fn = fName;
	for (;;)
	{
		// find the start of the next filename
		char* nl = strchr(fn, '\n');
		if (nl)
			*nl = 0;

		// remove all the %nn encoding and insert the corresponding characters
		fl_decode_uri(fn);

		// On macOS, dropped files are just absolute BSD paths, starting with a '/'
		// On MSWindows, dropped files are absolute file paths, starting with 'C:\' or anothor drive name
		// URLs start with http://, for example "http://www.unna.org/unna/games/Pyramid/Pyramid.pkg"
		if (strncmp(fn, "http://", 7) == 0 || strncmp(fn, "https://", 8) == 0)
		{
			if (strcasecmp(fl_filename_ext(fn), ".pkg") == 0)
			{
				try
				{
					http::Request request(fn);
					const http::Response response = request.send("GET");
					if (response.status == 404)
					{
						fl_message("Can't install\n%s\nFile not found.", fn);
					} else if (response.status >= 300)
					{
						fl_message("Can't install\n%s\nFile can't be dowloaded.", fn);
					} else
					{
						const KUInt8* package = reinterpret_cast<const KUInt8*>(response.body.data());
						KUInt32 packageSize = static_cast<KUInt32>(response.body.size());
						if (memcmp(package, "package", 7) != 0)
						{
							fl_message("Can't install\n%s\nThis is not a Newton package.", fn);
						} else
						{
							mPlatformManager->InstallPackage(package, packageSize);
						}
					}
				} catch (const std::exception& e)
				{
					fl_message("Can't install\n%s\n%s", fn, e.what());
				}
			} else
			{
				fl_message("Can't install\n%s\nNetwork download supports only .pkg files.", fn);
			}
		} else
		{
			// install the package
			mPlatformManager->InstallPackage(fn);
		}

		// if there is another filename, loop around
		if (nl)
			fn = nl + 1;
		else
			break;
	}
	free(fName);
}

/**
 User asks Einstein to install a package.

 We open a file chooser dialog and then take the package selected and push it to the emulator.

 \todo Use the system native file chooser!
 */
void
TFLApp::UserActionInstallPackage()
{
	static char* filename = 0L;

	const char* newname = ChooseExistingFile("Install Package...", "Package\t*.pkg", nullptr);
	// "Compressed Package\t*.{sit,sae,hqx,zip,sit.hqx,hqx.sit}");
	// for (int i=0; i<fnfc.count(); i++) {
	//    newname = fnfc.filename(i);
	if (newname && *newname)
	{
		if (!filename)
			filename = (char*) calloc(FL_PATH_MAX, 1);
		strncpy(filename, newname, FL_PATH_MAX);
		filename[FL_PATH_MAX] = 0;
		mPlatformManager->InstallPackage(filename);
	}
	//}
}

/**
 User wants to reset or reboot the machine.
 */
void
TFLApp::UserActionReset(int inType)
{
	switch (inType)
	{
		case 0:
			mEmulator->GetProcessor()->Reset();
			break;
		case 1:
			mPlatformManager->EvalNewtonScript("Reboot();");
			break;
		case 2:
			mEmulator->ZAPMemory(true);
			mEmulator->GetProcessor()->Reset();
			break;
	}
}

/**
 User wants to see the About window.

 \todo The About WIndow is not very beautilf. We should add credits and clickable links
 to give the user complete information on teh project. We should also provide version
 information for the REx and maybe other interfaces.
 */
void
TFLApp::UserActionShowAboutPanel()
{
	mFLSettings->ShowAboutDialog();
}

/**
 User wants to see the setting window.
 */
void
TFLApp::UserActionShowSettingsPanel()
{
	mFLSettings->ShowSettingsPanel();
}

/**
 User wants the screen to be the original scale, so resize it back to 1:1; mapping.

 Depending on the preferences, we would leave fullscreen mode.
 */
void
TFLApp::UserActionOriginalScreenSize()
{
	if (wAppWindow->fullscreen_active())
	{
		UserActionToggleFullscreen();
	}
	if (!wAppWindow->fullscreen_active())
	{
		wAppWindow->resizable(mNewtonScreen);
		int y = 0, w = 0, h = 0;
		if (wMenubar->visible())
			y += wMenubar->h();
		if (wToolbar->visible())
			y += wToolbar->h();
		w = (int) (mScreenManager->GetScreenWidth() * mFLSettings->screenScale / 100.0);
		h = y + (int) (mScreenManager->GetScreenHeight() * mFLSettings->screenScale / 100.0);
		wAppWindow->size(w, h);
		if (!mFLSettings->mAllowScreenResize)
			wAppWindow->resizable(nullptr);
		StoreAppWindowSize();
	}
}

/**
 User wants the app into or out of fullscreen mode.

 Remove the decoration from the main window and set it into fullscreen mode.
 Switch the menubar and the toolbar off.
 Scale the pixel output to the Newton screen.
 Rotating the screen should actually rotate it upside down and left-sdie right in fullscreen mode.
 */
void
TFLApp::UserActionToggleFullscreen()
{
	if (wAppWindow->fullscreen_active())
	{
		wAppWindow->resizable(mNewtonScreen);
		wAppWindow->fullscreen_off(mWindowedX, mWindowedY, mWindowedWidth, mWindowedHeight);
		if (!mFLSettings->mAllowScreenResize)
			wAppWindow->resizable(nullptr);
	} else
	{
		StoreAppWindowSize();
		wAppWindow->resizable(mNewtonScreen);
		wAppWindow->fullscreen();
		if (!mFLSettings->mAllowScreenResize)
			wAppWindow->resizable(nullptr);
	}
}

/**
 Called by the user interface if the user chooses to reice the ROM via TCP.

 This is a lot of effort for something that can be done better using 'netcat' on Unixes.
 I am sure another tool exists on MSWindows. I am also sure that nobody ever uses
 this because I never got any feedback that it is not working.

 \todo we should remove ROM download support.
 */
void
TFLApp::UserActionFetchROM()
{
	// not yet implemented
#if 0
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
#endif
}

void
TFLApp::UserActionPopupMenu()
{
	mFLSettings->HandlePopupMenu();
}

void
TFLApp::UserActionToggleMonitor()
{
	if (mMonitor)
		mMonitor->Show();
}

void
TFLApp::UserActionShowToolkit()
{
#if USE_TOOLKIT
	if (!mToolkit)
		mToolkit = new TToolkit(this);
	mToolkit->Show();
#endif
}

int
TFLApp::UserActionPCMCIAImageFromSnapshot(const char* dst, const char* data, const char* cis, const char* name)
{
	// TODO: make sure that the extension is actually .pcmcia if the file does not exist yet.
	int err = TLinearCard::ComposeImageFile(dst, data, cis, name);
	if (err)
	{
		const char* msg = "An unspecified error occured.";
		switch (err)
		{
			case TLinearCard::kErrCantCreateOutFile:
				msg = "Can't create Image file.";
				break;
			case TLinearCard::kErrCantOpenDataFile:
				msg = "Can't open Data file.";
				break;
			case TLinearCard::kErrCantOpenCISFile:
				msg = "Can't open CIS file.";
				break;
			case TLinearCard::kErrCorruptDataFile:
				msg = "Data file corrupt.";
				break;
			case TLinearCard::kErrDataFileTooBig:
				msg = "Data file too large.";
				break;
			case TLinearCard::kErrOutOfMemory:
				msg = "Out of memory.";
				break;
			case TLinearCard::kErrIncompleteImageFile:
				msg = "Incomplete Image file.";
				break;
			case TLinearCard::kErrCorruptCISFile:
				msg = "CIS file is corrupt.";
				break;
		}
		fl_alert("Can't create Card Image.\n%s", msg);
		return -1;
	}
	return 0;
}

// User wants to insert or remove PCMCIA card into ot from controller 0.
int
TFLApp::UserActionPCCard(int inSlot, long inIndex)
{
	int ret = -1;

	if (inIndex == -1)
	{
		ret = mPlatformManager->InsertPCCard(inSlot, nullptr);
	} else
	{
		TPCMCIACard* card = GetSettings()->mCardList[inIndex]->GetCard();
		if (card && !card->IsInserted())
			ret = mPlatformManager->InsertPCCard(inSlot, card);
	}

	mFLSettings->updateMenus();

	return ret;
}

// User wants to keep this card in the PCCard slot during reboots
int
TFLApp::UserActionKeepPCCardInSlot(int inSlot, int inIndex)
{
	mFLSettings->KeepPCCardInSlot(inSlot, inIndex);
	return 0;
}

// void TFLApp::UserActionAddPCMCIAImage(const char* inImageFilename, const char* inName)
//{
//     // TODO: create a TLinearCard, add it to the Card List, and update the Preferences and the Settings dialog.
// }

void
TFLApp::UserActionPrintScreen()
{
	Fl_Widget* target = mNewtonScreen;
	Fl_Paged_Device* p;
	int err;
	char* err_message = NULL;
	p = new Fl_Printer();
	err = p->begin_job(1, NULL, NULL, &err_message);
	if (!err)
	{
		int w, h;
		p->begin_page();
		// p->scale(72.0/100.0);
		p->printable_rect(&w, &h);
		p->origin((w - target->w()) / 2, (h - target->h()) / 2);
		p->print_widget(target);
		fl_color(FL_BLACK);
		fl_font(FL_HELVETICA, 14);
		fl_draw(VERSION_STRING, 0, target->h(), target->w(), 24,
			FL_ALIGN_INSIDE | FL_ALIGN_CENTER);
		p->end_page();
		p->end_job();
	} else if (err > 1 && err_message)
	{
		fl_alert("%s", err_message);
		delete[] err_message;
	}
	delete p;
}

#define BP fl_begin_polygon()
#define EP fl_end_polygon()
#define BCP fl_begin_complex_polygon()
#define ECP fl_end_complex_polygon()
#define BL fl_begin_line()
#define EL fl_end_line()
#define BC fl_begin_loop()
#define EC fl_end_loop()
#define vv(x, y) fl_vertex(x, y)

#define VF 0.9
#define VL 0.65
#define VB 0.0

#define AF 1.0
#define AL 0.6
#define AB 0.15
#define AA 0.15

static void
extSymbol(Fl_Color c)
{
	fl_color(c);
	BCP;
	// outline square, starting right side
	vv(VF, -VB);
	vv(VF, VF);
	vv(-VF, VF);
	vv(-VF, -VF);
	vv(VB, -VF);
	// inline square
	vv(VB, -VL);
	vv(-VL, -VL);
	vv(-VL, VL);
	vv(VL, VL);
	vv(VL, -VB);
	ECP;
	BCP;
	vv(-AA, -AA);
	vv(0.4, -AL - AA);
	vv(AB, -AF);
	vv(AF, -AF);
	vv(AF, -AB);
	vv(AL + AA, -0.4);
	vv(AA, AA);
	ECP;
#if 0
    fl_color(fl_darker(c));
    BC;
    // outline square, starting right side
    vv(VF, -VB); vv(VF, VF); vv(-VF, VF); vv(-VF, -VF); vv(VB, -VF);
    // inline square
    vv(VB, -VL); vv(-VL, -VL); vv(-VL, VL); vv(VL, VL); vv(VL, -VB);
    EC;
    BC;
    vv(-AA, -AA); vv(0.4, -AL-AA); vv(AB, -AF); vv(AF, -AF); vv(AF, -AB); vv(AL+AA, -0.4); vv(AA, AA);
    EC;
#endif
}

/**
 Create and open a dialog that will give quick access to Newton packages on the Net.

 The dialog presents a hierarchy of Titles, containing Groups, which in turn can
 contain Links to external text and pdf documents, Text blocks, Scripts, and
 Installer links to packages.

 addInstallerLink requires a title and points to multiple external documents.
 The tooltip will display the links after expansion. Users expect only
 one single link though.

 addInstallerText points to a C string. Text is wrapped, and the field is
 sized automatically.

 addInstallerScript can contains multiple scripts that are run sequentially.
 The tooltip will reveal the script texts. One script must not be longer
 than 255 characters. Script lines start with 'S'. Lines starting with 'W'
 will create a Warning dialog for the user.

 addInstallerPackage can contain multiple package links that are installe
 sequentially. Links are expanded using the following rules. Lines starting with
 'W' will create a Warning dialog for the user.

 Links starting with a 'U' will have the Unna link prepended. 'M' will add
 the messegapad.or link as defined in the Settings. Staring text with a 'W'
 will create a Warning dialog to the user, so they can cancel the operation.
 Links starting with a ':' will remain unchanged.
 Scripts must start with the letter 'S'.

 \note We should make the titles, groups, and maybe even comments foldable.
 \note Eventually if would be nice to be able to link to packages inside .zip
	and .sit.hqx archives.
 \note Tooltips for scripts can get very big.
 */
void
TFLApp::UserActionInstallEssentials()
{
	if (!wInstallerWindow)
	{
		fl_add_symbol("ext", extSymbol, 1);
		wInstallerWindow = makeInstaller();
		// --- Essentials
		addInstallerTitle("Essentials");
		// Y2K10
		addInstallerGroup("NewtonOS Y2K10 Fix");
		addInstallerText("NewtonOS has a bug in handling years past 18:48:31 on January 5, 2010. "
						 "Einstein contains a fix for US MP2x00 MessagePads, but "
						 "for eMates and German MP2x00 MessagePads, "
						 "the patch below will fix all date issues until 2026.\n\n"
						 "Please install this patch before installing anything else, "
						 "as this will wipe your Newton's memory.");
		addInstallerLink("explained by Eckhart Köppen", new StringList { ":https://40hz.org/Pages/newton/hacking/newton-year-2010-problem/" });
		addInstallerLink("Readme file for the patch", new StringList { "MDownloads/Einstein/Essentials/y2k10/README.txt" });
		addInstallerText("Please select the patch that matches the ROM image of your machine:");
		addInstallerPackage("German MP2x00 patch", new StringList { "WInstalling this patch may irreversibly erase all data\n"
																	"on your MessagePad.\n\n"
																	"Please proceed only if this a new device, or if your\n"
																	"data is securely backed up!",
													   "MDownloads/Einstein/Essentials/y2k10/Patch_D.pkg" });
		addInstallerPackage("eMate 300 patch", new StringList { "WInstalling this patch may irreversibly erase all data\n"
																"on your eMate 300.\n\n"
																"Please proceed only if this a new device, or if your\n"
																"data is securely backed up!",
												   "MDownloads/Einstein/Essentials/y2k10/Patch_eMate.pkg" });
		// --- Networking
		addInstallerTitle("Networking");
		// NIE
		addInstallerGroup("NIE: Newton Internet Enabler");
		addInstallerText("The Newton Internet Enabler (NIE) allows you to "
						 "access the Internet with your Newton. "
						 "NIE package was released by Apple in 1997.");
		addInstallerPackage("Apple NIE packages", new StringList { "WThis will install four packages on your Newton\nwhich may take a little while.", "Uunna/apple/software/Internet/NIE2/ENETSUP/enetsup.pkg", "Uunna/apple/software/Internet/NIE2/REGPKGS/inetenbl.pkg", "Uunna/apple/software/Internet/NIE2/ENETSUP/newtdev.pkg", "Uunna/apple/software/Internet/NIE2/REGPKGS/inetstup.pkg" });
		addInstallerPackage("Einstein Network Card driver", new StringList { "MDownloads/Einstein/Essentials/NIE/NE2K.pkg" });
		addInstallerScript("Open Internet Setup", new StringList { "SGetRoot().|InternetSetup:NIE|:Open();" });
		// NewtonScript: Reboot(), Sleep(), PowerOff()
		// PlaySoundSync()
		// ROM_alarmWakeup
		//      ROM_click
		//      ROM_crumple
		//      ROM_drawerClose
		//      ROM_drawerOpen
		//      ROM_flip
		//      ROM_funBeep
		//      ROM_hiliteSound
		//      ROM_plinkBeep
		//      ROM_simpleBeep
		//      ROM_wakeupBeep
		//      ROM_plunk
		//      ROM_poof
		// Courier
		addInstallerGroup("Courier Browser 0.5");
		addInstallerText("Courier is a small internet browser. The source code is available on UNNA.");
		addInstallerPackage("Courier packages", new StringList { "WThis will install three packages on your Newton\nwhich may take a little while.", "Uunna/internet/web-browsers/Courier0.5/Courier0.5.pkg", "Uunna/internet/web-browsers/Courier0.5/NHttpLib-3.1.pkg", "Uunna/internet/web-browsers/Courier0.5/NTox-1.6.1.pkg" });
		addInstallerScript("Open Courier Browser", new StringList { "SGetRoot().|Courier:40Hz|:Open();" });
		// -- Developers
		addInstallerTitle("Developer Apps");
		addInstallerGroup("ViewFrame 1.3b");
		addInstallerPackage("ViewFrame packages", new StringList { "WThis will install eight packages on your Newton\nwhich may take a little while.", "Uunna/development/tools/ViewFrame1.3b/PROGKEYB.PKG", "Uunna/development/tools/ViewFrame1.3b/VFEDITOR.PKG", "Uunna/development/tools/ViewFrame1.3b/VFFUNCTI.PKG", "Uunna/development/tools/ViewFrame1.3b/VFGENERA.PKG", "Uunna/development/tools/ViewFrame1.3b/VFINTERC.PKG", "Uunna/development/tools/ViewFrame1.3b/VIEWFRAM.PKG", "Uunna/development/tools/ViewFrame1.3b/ONLYFOR2/VFDANTE.PKG", "Uunna/development/tools/ViewFrame1.3b/ONLYFOR2/VFKEYS.PKG" });
	}
	wInstallerWindow->show();
}

// MARK: -
// ---  Events from within the meulator

/**
 This is called by the screen manager when the state of the backlight changed.
 */
void
TFLApp::PowerChangedEvent(Boolean inState)
{
	// we have a hidden button in the FLuid file that does nothing but keep
	// track of the "on" image.
	static Fl_Image* onImage = nullptr;
	static Fl_Image* offImage = nullptr;
	Fl::lock();
	if (!onImage)
	{
		onImage = wPowerOnTool->image();
		offImage = wPowerTool->image();
	}
	if (inState)
	{
		wPowerTool->image(onImage);
	} else
	{
		wPowerTool->image(offImage);
	}
	wPowerTool->redraw();
	Fl::awake();
	Fl::unlock();
}

/**
 This is called by the screen manager when the state of the backlight changed.
 */
void
TFLApp::BacklightChangedEvent(Boolean inState)
{
	// we have a hidden button in the FLuid file that does nothing but keep
	// track of the "on" image.
	static Fl_Image* onImage = nullptr;
	static Fl_Image* offImage = nullptr;
	Fl::lock();
	if (!onImage)
	{
		onImage = wBacklightOnTool->image();
		offImage = wBacklightTool->image();
	}
	if (inState)
	{
		wBacklightTool->image(onImage);
	} else
	{
		wBacklightTool->image(offImage);
	}
	wBacklightTool->redraw();
	Fl::awake();
	Fl::unlock();
}

void
TFLApp::ResizeFromNewton(int w, int h)
{
	if (mNewtonScreen->w() == w && mNewtonScreen->h() == h)
		return;
	Fl::lock();
	wAppWindow->resizable(mNewtonScreen);
	int dw = w - mNewtonScreen->w();
	int dh = h - mNewtonScreen->h();
	wAppWindow->size(wAppWindow->w() + dw, wAppWindow->h() + dh);
	if (!mFLSettings->mAllowScreenResize)
		wAppWindow->resizable(nullptr);
	Fl::unlock();
}

// MARK: - Private: -

void
TFLApp::InitFLTK(int argc, char** argv)
{
	Fl::scheme("gtk+");
	Fl::args(argc, argv);
	Fl::get_system_colors();
	Fl::use_high_res_GL(1);
	Fl::visual(FL_RGB);
	Fl_Tooltip::size(12);
}

void
TFLApp::InitSettings()
{
	mFLSettings = new TFLSettingsUI();
#if TARGET_OS_WIN32
	mFLSettings->mSettingsPanel->icon((char*) LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#endif
	mFLSettings->setAppPath(mProgramName);
	mFLSettings->loadPreferences();
	mFLSettings->revertDialog();
}

void
TFLApp::InitSound()
{
#if TARGET_OS_WIN32
	mSoundManager = new TWaveSoundManager(mLog);
#elif TARGET_OS_LINUX
	mSoundManager = new TPulseAudioSoundManager(mLog);
#elif TARGET_OS_MAC
	mSoundManager = new TCoreAudioSoundManager(mLog);
#else
#error Selected target OS support not implemented, or no target OS selected
#endif
}

void
TFLApp::InitNetwork()
{
#if TARGET_OS_MAC || TARGET_OS_LINUX || TARGET_OS_WIN32
	mNetworkManager = new TUsermodeNetwork(mLog);
#else
#warn Please configure a network driver
	mNetworkManager = new TNullNetworkManager(mLog);
#endif
}

/**
 Initialize the application window and the screen driver.
 */
void
TFLApp::InitScreen()
{
	int portraitWidth = mFLSettings->screenWidth;
	int windowWidth = (int) (portraitWidth * (mFLSettings->screenScale / 100.0));
	int portraitHeight = mFLSettings->screenHeight;
	int windowHeight = (int) (portraitHeight * (mFLSettings->screenScale / 100.0));

	Fl_Group::current(nullptr);
	wAppWindow = CreateApplicationWindow(mFLSettings->mAppWindowPosX,
		mFLSettings->mAppWindowPosY);

	// -- calculate the height of the screen
	int emulatorScreenY = 0;
	// will there be a menu bar?
	if (mFLSettings->mShowMenubar)
	{
		wMenubar->position(0, 0);
		emulatorScreenY += wMenubar->h();
		wMenubar->show();
	} else
	{
#if TARGET_OS_MAC
		wMenuItemEssentials->flags &= ~FL_MENU_DIVIDER;
		wMenuItemAbout->hide();
		mFLSettings->wSysMenubar = new Fl_Sys_Menu_Bar(0, 0, 1, 1);
		mFLSettings->wSysMenubar->window_menu_style(Fl_Sys_Menu_Bar::no_window_menu);
		mFLSettings->wSysMenubar->menu(wMenubar->menu());
		mFLSettings->wSysMenubar->about(wMenuItemAbout->callback(), wMenuItemAbout->user_data());
#endif
		wMenubar->hide();
	}
	// will there be a tool bar
	if (mFLSettings->mShowToolbar)
	{
		wToolbar->position(0, emulatorScreenY);
		emulatorScreenY += wToolbar->h();
		wToolbar->show();
	} else
	{
		wToolbar->hide();
	}

	wAppWindow->size(windowWidth, windowHeight + emulatorScreenY);
	if (windowWidth > portraitWidth)
		wAppWindow->size_range(portraitWidth, portraitHeight + emulatorScreenY);
	wAppWindow->resizable(nullptr);
#if TARGET_OS_WIN32
	wAppWindow->icon((char*) LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#endif
	wAppWindow->callback(quit_cb, this);
	if (mFLSettings->mAllowFullscreen)
		wMenuItemFullscreen->activate();
	else
		wMenuItemFullscreen->deactivate();

	wAppWindow->begin();
	TFLScreenManager* flScreenManager = new TFLScreenManager(this, mLog, portraitWidth, portraitHeight, false, false);
	mNewtonScreen = flScreenManager->GetWidget();
	mScreenManager = flScreenManager;
	mNewtonScreen->resize(0, emulatorScreenY, windowWidth, windowHeight);
	wAppWindow->end();

	if (mFLSettings->mAllowScreenResize)
		wAppWindow->resizable(mNewtonScreen);
	else
		wAppWindow->resizable(nullptr);
	StoreAppWindowSize();
}

void
TFLApp::InitSerialPorts()
{
	// TODO: add preferences for the current driver, port and server address
	// Basic initialization of all serial ports

	mEmulator->SerialPorts.Initialize(TSerialPorts::kTcpClientDriver,
		TSerialPorts::kNullDriver,
		TSerialPorts::kNullDriver,
		TSerialPorts::kNullDriver);

	// Add default host driver types for the standard four serial port locations.
	// These settings will be used in case no dedicated other settings are used during
	// driver instantiation.
	mEmulator->SerialPorts.SetHostPortSettings('extr',
		{ TSerialPorts::kPtyDriver, std::string("/tmp/einstein-extr.pty") });
	mEmulator->SerialPorts.SetHostPortSettings('mdem',
		{ TSerialPorts::kPtyDriver, std::string("/tmp/einstein-mdem.pty") });
	mEmulator->SerialPorts.SetHostPortSettings('infr',
		{ TSerialPorts::kPtyDriver, std::string("/tmp/einstein-infr.pty") });
	mEmulator->SerialPorts.SetHostPortSettings('tblt',
		{ TSerialPorts::kPtyDriver, std::string("/tmp/einstein-tblt.pty") });
#if 0
    // TODO: save the serial port setting in a safe place
    TSerialPortManager *extr = mEmulator->SerialPorts.GetDriverFor(TSerialPorts::kExtr);
    if (extr && extr->GetID()==TSerialPorts::kTcpClientDriver)
    {
        TTcpClientSerialPortManager *tcp = (TTcpClientSerialPortManager*)extr;
        tcp->SetServerAddress([[defaults stringForKey: kExtrTCPServerAddress] UTF8String]);
        tcp->SetServerPort((int)[defaults integerForKey: kExtrTCPServerPort]);
    }
    mEmulator->SerialPorts.PortChangedCallback(
                                               // THIS IS A LAMBDA FUNCTION. This function is called when an application
                                               // on the emulated Newton changes the serial port settings
                                               [self](int serPort)->void
                                               {
        if (serPort==TSerialPorts::kExtr) {
            TSerialPortManager *extr = mEmulator->SerialPorts.GetDriverFor(TSerialPorts::kExtr);
            if (extr && extr->GetID()==TSerialPorts::kTcpClientDriver) {
                TTcpClientSerialPortManager *tcp = (TTcpClientSerialPortManager*)extr;

                char *tcpServer = tcp->GetServerAddressDup();
                NSString *nsTcpServer = [NSString stringWithUTF8String:tcpServer];
                [[mUserDefaultsController defaults] setValue:nsTcpServer forKey:kExtrTCPServerAddress];
                ::free(tcpServer);

                int tcpPort = tcp->GetServerPort();
                [[mUserDefaultsController defaults] setInteger:tcpPort forKey:kExtrTCPServerPort];
            }
        }
    }
                                               );
#endif
}

void
TFLApp::MountPCCardsKeptInSlot()
{
	int c0 = mFLSettings->GetCardKeptInSlot(0);
	if (c0 != -1)
		UserActionPCCard(0, c0);
	int c1 = mFLSettings->GetCardKeptInSlot(1);
	if (c1 != -1)
		UserActionPCCard(1, c1);
}

void
TFLApp::InitMonitor(const char* theROMImagePath)
{
	mMonitorLog = new TBufferLog();
#if !NDEBUG
#if TARGET_OS_WIN32
	mMonitorLog->OpenLog("C:/user/micro/Einstein_log.txt");
#endif
#if TARGET_OS_MAC
	mMonitorLog->OpenLog("/tmp/Einstein_log.txt");
#endif
#if TARGET_OS_LINUX
	mMonitorLog->OpenLog("/tmp/Einstein_log.txt");
#endif
#endif

	char theSymbolListPath[FL_PATH_MAX];
	strncpy(theSymbolListPath, theROMImagePath, FL_PATH_MAX);
	fl_filename_setext(theSymbolListPath, FL_PATH_MAX, ".symbols");
	mSymbolList = new TSymbolList(theSymbolListPath);
	mMonitor = new TFLMonitor(mMonitorLog, mEmulator, mSymbolList, theROMImagePath);
	KPrintf("Booting... (Monitor enabled)\n");
}

/**
 Launch the emulator or monitor thread.
 */
void
TFLApp::EmulatorThreadEntry()
{
	if (mMonitor)
	{
		// mMonitor->RunOnStartup(mFLSettings->mBreatAtROMBoot == 0);
		mMonitor->Run();
	} else
	{
		mEmulator->Run();
	}
	// wake up the FLTK mainloop and have it call GUI Quit.
	Fl::awake([](void*) { gApp->UserActionQuit(); });
}

/**
 Create a file for logging all important events at runtime.
 */
void
TFLApp::CreateLog(const char* inFilePath)
{
	if (mLog)
	{
		KPrintf("A log already exists (--monitor & --log are exclusive)\n");
		::exit(1);
	}
	mLog = new TFileLog(inFilePath);
}

/**
 Create the appropriate screen manager for this platform.

 In FLTK world, that would always be the FLTK Screen driver.

 \todo do we have to do any fullscreen management here? RaspberryPI? Linux tablets? Pen PCs?
 */
void
TFLApp::CreateScreenManager(
	const char* inClass,
	int inPortraitWidth,
	int inPortraitHeight,
	Boolean inFullScreen)
{
	if (::strcmp(inClass, "FL") == 0)
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

		mScreenManager = new TFLScreenManager(this,
			mLog,
			theWidth,
			theHeight,
			inFullScreen,
			screenIsLandscape);
	} else
	{
		(void) ::fprintf(stderr, "Unknown screen manager class %s\n", inClass);
		::exit(1);
	}
}

/**
 User wants us to quit.

 This may be a menu item or the Cllose button on the window decoration.
 */
void
TFLApp::quit_cb(Fl_Widget*, void*)
{
	gApp->UserActionQuit();
}

/**
 Store the current size of the app window in mWindowed... .
 */
void
TFLApp::StoreAppWindowSize()
{
	mWindowedX = wAppWindow->x();
	mWindowedY = wAppWindow->y();
	mWindowedWidth = wAppWindow->w();
	mWindowedHeight = wAppWindow->h();
}

/**
 Run Stuff after NewtonOS booted or wakes up from sleep.
 */
void
TFLApp::DeferredOnPowerRestored()
{
}

#include "Emulator/JIT/Generic/TJITGenericROMPatch.h"
#include "Emulator/JIT/Generic/TJITGeneric_Macros.h"

/*
 This outputs an error code which should be translated into clear text and written to the Toolkit and log.
 GetRoot():Notify(6, GetRoot():TranslateToOverview(-8005), GetRoot():TranslateToMessage(-8005));
 */
T_ROM_INJECTION(0x00146938, kROMPatchVoid, kROMPatchVoid, kROMPatchVoid, "Log Exception")
{
	KSInt32 ex = (KSInt32) ioCPU->GetRegister(0);
	KPrintf("Exception %d\n", ex);
	if (gToolkit)
		gToolkit->printErrNo(ex);
	return ioUnit;
}

/**
 * Copy NewtonOS clipboard data to the system clipboard.
 *
 * If the user copies data to the clipboard, Einstein will extract all text it can find
 * and post it to the host system clipboard, so it can be pasted into other apps.
 */
// NS: Get/SetClipboard, FGet/SetClipboard, AddClipboard__9TRootViewFRC6RefVarT1, GetClipboard__9TRootViewFv, class TClipboard
// gRootView (0x0C101934) 000A56DC 000A7A34
// TView::FindDropView(TDragInfo const &, TPoint const &) is just a 'return' instruction. Drived functions in ListView and EditView are implemented.
// TView::Drop(RefVar const &, RefVar const &, TPoint *)
// There is a newtonscript function "ViewDropScript"

// "Paste" : GetView('viewfrontmost)...

// GetClipboard__9TRootViewFv:         @ 0x001B58BC: TRootView::GetClipboard(void)
//  is called, whenever we want to get the clipboard data
// If Cmd-P is pressed, called from DoEditCommand__5TViewFl (arg=2 or 3) (009EA00), "FClipboardCommand(arg)"
T_ROM_INJECTION(0x001B37FC, 0x001B5CD4, 0x001A1660, kROMPatchVoid, "AddClipboard__9TRootViewFRC6RefVarT1")
{
	//    KPrintf("AddClipboard__9TRootViewFRC6RefVarT1\n");
	// TRootView::AddClipboard(RefVar const &, RefVar const &): 0x01ABEF3C
	// r0 is a pointer to TRootView
	// r1 is the clipboard data
	// r2 ...
	TNewt::RefArg a = TNewt::RefVar::FromPtr(ioCPU->GetRegister(1));

	NewtRef data = TNewt::GetFrameSlot(a, TNewt::MakeSymbol("data"));
	//    TNewt::PrintRef(data, 8);
	if (!TNewt::RefIsArray(data))
		return ioUnit; // expected an array
	int nData = (int) TNewt::RefArrayGetNumSlots(data);

	std::string clipboardText = "";
	Boolean firstText = true;
	for (int i = 0; i < nData; i++)
	{
		NewtRef dataSet = TNewt::RefArrayGetSlot(data, i);
		//        TNewt::PrintRef(dataSet, 8);
		if (!TNewt::RefIsArray(dataSet))
			continue;
		int nDataSet = (int) TNewt::RefArrayGetNumSlots(dataSet);
		for (int j = 0; j < nDataSet; j++)
		{
			NewtRef textRec = TNewt::RefArrayGetSlot(dataSet, j);
			//            TNewt::PrintRef(textRec, 8);
			if (!TNewt::RefIsFrame(textRec))
				continue;
			NewtRef textRef = TNewt::GetFrameSlot(textRec, TNewt::MakeSymbol("text"));
			//            TNewt::PrintRef(textRef, 8);
			if (TNewt::RefIsString(textRef))
			{
				int textLen = TNewt::RefStringLength(textRef);
				char* text = (char*) malloc(textLen + 1);
				TNewt::RefToString(textRef, text, textLen);
				if (firstText)
					firstText = false;
				else
					clipboardText.append("\n");
				clipboardText.append(text);
				free(text);
				break;
			}
		}
	}
	const char* cstring = clipboardText.c_str();
	Fl::copy(cstring, strlen(cstring), 1);
	return ioUnit;
}

static void
clip_callback(int source, void* data)
{
	(void) data;
	if (source == 1)
	{
		KPrintf("Clipboard: \"%s\"\n", (char*) data);
	}
}

static void
draw_ramp(int x, int y, int w, int h, Fl_Color c)
{
	int h1 = (h < 100) ? h : 100;
	for (int i = y; i < y + h1; i++)
	{
		fl_color(fl_color_average(FL_BACKGROUND_COLOR, c, ((i - y) / 100.0)));
		fl_xyline(x, i, x + w);
	}
	if (h >= 100)
	{
		fl_rectf(x, y + 100, w, h - 100, FL_BACKGROUND_COLOR);
	}
}

static void
tabs_box(int x, int y, int w, int h, Fl_Color c)
{
	const int barHgt = 1;
	fl_rectf(x, y, w, barHgt, fl_color_average(FL_FOREGROUND_COLOR, c, 0.5));
	fl_rectf(x, y + barHgt, w, h - barHgt, c);
}

static const char*
tfl_file_chooser(const char* message, const char* pat, const char* fname, FileChooser type)
{
#if UPDATED_TARGET_OS_LINUX
	char pattern[FL_PATH_MAX];
	pattern[0] = 0;
	if (pat)
	{
		const char* s = pat;
		char* d = pattern;
		Boolean brackets = false;
		while (*s)
		{
			char c = *s++;
			if (c == '\t')
			{
				*d++ = ' ';
				*d++ = '(';
				brackets = true;
			} else if (c == '\n' && brackets)
			{
				*d++ = ')';
				*d++ = '\t';
			} else
			{
				*d++ = c;
			}
		}
		*d = 0;
	}
	return fl_file_chooser(message, pattern[0] ? pattern : nullptr, fname);
#else
	static char tfl_file_chooser_filename[FL_PATH_MAX];
	char name[FL_PATH_MAX];
	name[0] = 0;
	char fdir[FL_PATH_MAX];
	fdir[0] = 0;

	if (fname && *fname)
	{
		const char* n = fl_filename_name(fname);
		if (n)
		{
			int len = n - fname;
			strcpy(name, n);
			strncpy(fdir, fname, len);
			fdir[len] = 0;
		} else
		{
			strcpy(name, fname);
		}
	}

	Fl_Native_File_Chooser fnfc;
	fnfc.title(message);
	switch (type)
	{
		case FileChooser::SaveFile:
			fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
			fnfc.options(Fl_Native_File_Chooser::NEW_FOLDER | Fl_Native_File_Chooser::USE_FILTER_EXT);
			break;
		case FileChooser::OpenFile:
			fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
			fnfc.options(Fl_Native_File_Chooser::USE_FILTER_EXT);
			break;
		case FileChooser::ChooseDirectory:
			fnfc.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
			fnfc.options(Fl_Native_File_Chooser::USE_FILTER_EXT);
			break;
	}
	fnfc.filter(pat);
	fnfc.directory(fdir);
	fnfc.preset_file(name);
	switch (fnfc.show())
	{
		case -1:
			return nullptr; // Error text is in fnfc.errmsg()
		case 1:
			return nullptr; // user canceled
	}
	if (fnfc.filename())
	{
		strcpy(tfl_file_chooser_filename, fnfc.filename());
		return tfl_file_chooser_filename;
	} else
	{
		return nullptr;
	}
#endif
}

const char*
TFLApp::ChooseExistingFile(const char* message, const char* pat, const char* fname)
{
	return tfl_file_chooser(message, pat, fname, FileChooser::OpenFile);
}

const char*
TFLApp::ChooseExistingDirectory(const char* message, const char* pat, const char* fname)
{
	return tfl_file_chooser(message, pat, fname, FileChooser::ChooseDirectory);
}

const char*
TFLApp::ChooseNewFile(const char* message, const char* pat, const char* fname)
{
	return tfl_file_chooser(message, pat, fname, FileChooser::SaveFile);
}

/**
 This is the first function that is called on all platforms.

 We use some static initialisation throughout the code that will be called before this function
 is ever reached. Also, different platforms have different entry points (MS Windows for example calls
 WinMain() first). FLTK makes sure that main() is called soon after.
 */
int
main(int argc, char** argv)
{
	if (Fl::abi_check(FL_ABI_VERSION) == 0)
	{
		fl_alert("Warning: FLTK ABI versions don't match:\n%d", FL_ABI_VERSION);
	}
	Fl::add_clipboard_notify(clip_callback);
	Fl::set_boxtype(FL_FREE_BOXTYPE, draw_ramp, 0, 0, 0, 0);
	Fl::set_boxtype((Fl_Boxtype) (FL_FREE_BOXTYPE + 1), draw_ramp, 0, 0, 0, 0);
	Fl::set_boxtype((Fl_Boxtype) (FL_FREE_BOXTYPE + 2), tabs_box, 0, 2, 0, 0);
	Fl::set_boxtype((Fl_Boxtype) (FL_FREE_BOXTYPE + 3), tabs_box, 0, 2, 0, 0);

	gApp = new TFLApp();
	gApp->Run(argc, argv);
	return 0;
}

// ======================================================================= //
// We build our computer (systems) the way we build our cities: over time,
// without a plan, on top of ruins.
//   -- Ellen Ullman
// ======================================================================= //
