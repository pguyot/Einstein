
// ==============================
// File:			TSDLApp.h
// Project:			Einstein
//
// Copyright 2025 by Matthias Melcher.
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

#ifndef _TSDLAPP_H
#define _TSDLAPP_H

#include "app/Version.h"
#include "Emulator/Screen/TScreenManager.h"

#include <K/Defines/KDefinitions.h>

#include <SDL3/SDL.h>

#include <string>
#include <filesystem>
#include <atomic>
#include <vector>


class TROMImage;
class TEmulator;
class TSoundManager;
class TScreenManager;
class TSDLScreenManager;
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

extern class TSDLApp* gApp;

typedef std::vector<std::string> StringList;
typedef std::vector<const char*> StringArray;

enum class BootState {
	Launch,
	LoadROM,
	ROMNotFound,
	PickROM,
	ROMPicked,
	Run,
	Running,
	PackageDialog,
	PickPackage,
	InstallPackage,
	Exit
};

/**
 This is the central class that manages application launch and the user interface.
 */
class TSDLApp
{
public:
	// --- Constructor and destructor

	// Constructor.
	TSDLApp();

	// Destructor.
	~TSDLApp();

	// no copy constructor
	TSDLApp(const TSDLApp& inCopy) = delete;

	// no assignment constructor
	TSDLApp& operator=(const TSDLApp& inCopy) = delete;

	// --- Startup and run the emulator.

	// Set up SDL and launch the boot state machine.
	SDL_AppResult Launch(int argc, char* argv[]);

	// Allocate the remaining part of the emulator an run the emulation in a new thread.
	SDL_AppResult Run();

	// Register Einstein specific events with SDL.
	void InitSDLEvents();

	// Initialize the SDL window and graphics resources and open the window.
	SDL_AppResult InitSDLGraphics();

	// Map window coordinates to the emulated screen for the current window size.
	void UpdateInputScale();

	// Create (or recreate) the texture for the toolbar and the Newton screen.
	SDL_AppResult CreateScreenTexture();

	// Update the boot state and send an event, so the state machine advances.
	void ChangeBootState(BootState newState);

	// Handle any event that SDL may throw at us.
	SDL_AppResult HandleSDLEvent(SDL_Event *event);

	// Handle a change in the boot state machine.
	SDL_AppResult HandleBootStateChange();

	// Open a file chooser to pick a ROM file from the file system.
	SDL_AppResult PickROMFile();

	// Copy the selected file to an app private space if one was selected.
	void ROMFilePicked(const char * const *filelist, int filter);

	// SDL calls and wants us to render a new frame.
	SDL_AppResult IterateSDL();

	// Redraw the toolbar into the texture.
	bool RedrawToolbar();

	// Redraw everything, and wake the Newton if it is asleep.
	void RedrawAll();
	void WakeNewton();
	void SleepNewton(Uint32 inWaitMS);

	// Fast start: continue where Einstein was when Android closed it.
	// LoadFastStartState() runs before the emulator thread starts;
	// SaveFastStartState() when the app goes to the background, once the
	// Newton is asleep; ResumeFromBackground() when it comes back.
	bool LoadFastStartState();
	void SaveFastStartState();
	void ResumeFromBackground();
	std::filesystem::path FastStartFilePath() const { return mPublicDataPath / "FastStart.state"; }
	// How long going to the background waits for NewtonOS to fall asleep
	// (the FLTK app waits up to 10 s when quitting).
	static constexpr Uint32 kSleepTimeoutMS = 5000;

	// Draw recent log lines and the emulator state over the screen.
	void DrawLogOverlay();

	// --- User Actions

	// user wants to install a package
	void UserActionInstallPackage();

	// The toolbar menu: power, backlight, network card, restart, log.
	// (Install package has its own toolbar button.)
	void ShowMenu();
	void HandleMenuChoice(int inItem);
	SDL_AppResult PickPackageFile();
	void PackageFilePicked(const char * const *filelist, int filter);

	// --- Getter and setter

	TPlatformManager*
	GetPlatformManager()
	{
		return mPlatformManager;
	}

	TSDLScreenManager*
	GetScreenManager()
	{
		return mScreenManager;
	}

	void
	RefreshScreen()
	{
		mRefreshScreen = true;
	}

private:

	// Reminder to implement PCMCIA Card support
	// void MountPCCardsKeptInSlot();

	// Emulator thread entry point.
	void EmulatorThreadEntry(void);

	// Einstein Variables

	//int mEmuScreenW { 320 };
	//int mEmuScreenH { 480 };

	TROMImage* mROMImage = nullptr;
	TEmulator* mEmulator = nullptr;
	TSoundManager* mSoundManager = nullptr;
	TSDLScreenManager* mScreenManager = nullptr;
	TPlatformManager* mPlatformManager = nullptr;
	TNetworkManager* mNetworkManager = nullptr;
	TLog* mLog = nullptr;

	// SDL variables

	BootState mBootState { BootState::Launch };

	std::filesystem::path mPrivateDataPath;
	std::filesystem::path mPublicDataPath;
	std::filesystem::path mROMPath;
	std::filesystem::path mFlashPath;

	Uint32 mSDLMinEvent { 0 };
	Uint32 mSDLBootStateEvent { 0 };
	Uint32 mSDLMaxEvent { 0 };

	int mScreenWidth  { TScreenManager::kDefaultPortraitWidth };
	int mScreenHeight { TScreenManager::kDefaultPortraitHeight };
	int mToolbarHeight { 24 };

	float mScreenScaleX { 1.0f };
	float mScreenScaleY { 1.0f };
	float mPixelScaleX { 1.0f };
	float mPixelScaleY { 1.0f };
	SDL_Window *mSDLWindow { nullptr };
	SDL_Renderer *mSDLRenderer { nullptr };
	SDL_Texture *mSDLTexture { nullptr };
	int mTextureEncoding { 0 };
	Uint32 mToolbarColor { 0 };
	bool mAdjustToScreenDPI { true };
	bool mAdjustToAspectRatio { true };
	bool mRefreshToolbar { true };
	bool mRefreshScreen { true };
	int mPenDownN { 0 };
	int mPenMoveN { -1 };
	int mPenDown { 0 }; // 0: up, 1: on screen, 2: on buttons
	// Diagnostics: the log is hidden by default; a tap on the left part of
	// the toolbar shows or hides it.
	bool mShowLog { false };
	// SDL event type for a menu choice made in a native Android dialog.
	Uint32 mMenuEventType { 0 };
	// The NE2000 card is inserted (the platform manager only toggles it).
	bool mNetworkCardInserted { false };
	std::atomic<int> mEmulatorState { 0 }; // 0 not started, 1 running, 2 ended
	// We pressed the power switch to put the Newton to sleep while in the
	// background; don't press it again until we are back.
	std::atomic<bool> mSleepRequested { false };
	// After returning to the foreground, keep checking for a few seconds:
	// a sleep that was still on its way may only finish after we are back.
	Uint64 mWakeUntil { 0 };
	// SDL delivers the background and foreground events at once, on the
	// thread that posts them (the Java UI thread on Android), while the main
	// thread may be drawing. The foreground handler only sets this flag;
	// IterateSDL() wakes the Newton and redraws on the main thread.
	std::atomic<bool> mForegroundPending { false };
	// Runs the emulator thread and does the state saves (no window).
	TMonitor* mMonitor { nullptr };
	TBufferLog* mMonitorLog { nullptr };
	TSymbolList* mSymbolList { nullptr };
	// The emulator was halted in the background after a fast start save.
	std::atomic<bool> mHaltedInBackground { false };
	// Wake the Newton at this time after a fast start (0: no).
	Uint64 mFastStartWakeAt { 0 };
};

#endif
// _TSDLAPP_H

// ============================================================================ //
// "Give someone a program; you frustrate them for a day; teach them how to     //
// program, and you frustrate them for a lifetime.“ – David Leinweber           //
// ============================================================================ //
