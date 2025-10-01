
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

#include <K/Defines/KDefinitions.h>

#include <SDL3/SDL.h>

#include <string>
#include <filesystem>
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

	float mScreenScaleX { 1.0f };
	float mScreenScaleY { 1.0f };
	SDL_Window *mSDLWindow { nullptr };
	SDL_Renderer *mSDLRenderer { nullptr };
	SDL_Texture *mSDLTexture { nullptr };
	int mTextureEncoding { 0 };
};

#endif
// _TSDLAPP_H

// ============================================================================ //
// "Give someone a program; you frustrate them for a day; teach them how to     //
// program, and you frustrate them for a lifetime.“ – David Leinweber           //
// ============================================================================ //
