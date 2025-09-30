// ==============================
// File:			TSDLApp.cp
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

// On Android, we may want to use an Action Bar to get icons and menus for
// Backlight, Power, Package Install, etc.

#define SDL_MAIN_USE_CALLBACKS

#include <K/Defines/KDefinitions.h>

#include "TSDLApp.h"

// Einstein
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TSDLLog.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Network/TUsermodeNetwork.h"
#include "Emulator/PCMCIA/TLinearCard.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/Screen/TSDLScreenManager.h"
#include "Emulator/Serial/TSerialPortManager.h"
#include "Emulator/Serial/TSerialPorts.h"
#include "Emulator/Serial/TTcpClientSerialPortManager.h"
#include "Emulator/Sound/TNullSoundManager.h"

#if TARGET_OS_ANDROID
#include "app/SDL/TSDLAndroid.h"
#endif

// SDL3
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_system.h>

// ANSI C & POSIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// C++17
#include <filesystem>
#include <thread>

TSDLApp* gApp = nullptr;


// ---- SDL Forwarding Calls ---------------------------------------------------

/**
 \brief Called by SDL at every frame, forward to TSDLApp class.
 */
SDL_AppResult SDL_AppIterate(void *appstate)
{
	(void)appstate;
	return gApp->IterateSDL();
}


/**
 \brief Called by SDL for every event, forward to TSDLApp class.
 */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	(void)appstate;
	return gApp->HandleSDLEvent(event);
}


/**
 \brief Called by SDL to launch the app, forward to TSDLApp class.
 */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
	gApp = new TSDLApp();
	*appstate = gApp;
	return gApp->Launch(argc, argv);
}


/**
 \brief Called by SDL when user closes app, forward to TSDLApp class.
 */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	(void)appstate;
	(void)result;

//	SDL_DestroyRenderer(mSDLRenderer);
//	SDL_DestroyWindow(mSDLWindow);
	SDL_Quit();
}


// ==== TSDLApp Class Implementation ===========================================

// --- Constructor and destructor

/**
 \brief Constructor for the app.
 */
TSDLApp::TSDLApp(void) = default;


/**
 \brief Clean up time.
 */
TSDLApp::~TSDLApp(void)
{
	// There is nothing really here yet.
	// This only happens when the app quits, so we don;t really care.
	// But it would be polite to clean up nicely.
}


// --- Startup and run the emulator.

/**
 \brief Set up SDL and launch the boot state machine.
 \param argc ignored
 \param argv ignored
 \return SDL_APP_FAILURE if something went wrong.
 \return SDL_APP_CONTINUE if everything was allocated nicely.
 */
SDL_AppResult
TSDLApp::Launch(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

	SDL_AppResult result = SDL_APP_CONTINUE;

	mLog = new TSDLLog();
	SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	// Init Directories
#if TARGET_OS_ANDROID
	mPrivateDataPath = SDL_GetAndroidInternalStoragePath();
	mPublicDataPath = SDL_GetAndroidExternalStoragePath();
	mROMPath = mPublicDataPath / "ROM.bin"; // FIXME: public or private?
	// ROM in private
	// Setting in private
	// Flash in public?
#else
	mPrivateDataPath = SDL_GetPrefPath("org.messagepad", "einstein");
	mPublicDataPath = SDL_GetUserFolder(SDL_FOLDER_DOCUMENTS);
	mPublicDataPath /= "Einstein";
	mROMPath = "/Users/matt/dev/Einstein/_Data_/717006";
#endif

	// Load Settings
	// TODO: write me

	// Initialize SDL user events
	InitSDLEvents();

	// Initialize window, renderer, and texture
	result = InitSDLGraphics();
	if (result != SDL_APP_CONTINUE) return result;

	// Change BootState to LoadROM (sends a message to SDL)
	ChangeBootState(BootState::LoadROM);

	// Let the message handler take it from here
	return result;
}


/**
 \brief Register Einstein specific events with SDL.
 */
void TSDLApp::InitSDLEvents()
{
	constexpr int numUserEvents = 1;
	mSDLMinEvent = SDL_RegisterEvents(numUserEvents);
	mSDLBootStateEvent = mSDLMinEvent;
	mSDLMaxEvent = mSDLMinEvent + numUserEvents - 1;
}


/**
 \brief Initialize the SDL window and graphics resources and open the window.
 \return SDL_APP_FAILURE if something went wrong.
 \return SDL_APP_CONTINUE if everything went well.
 */
SDL_AppResult TSDLApp::InitSDLGraphics()
{
	SDL_SetAppMetadata("Einstein emulator for Newton MP2x00", "1.0", "org.messagepad.einstein");

	if (!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS)) {
		mLog->FLogLine("SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer(nullptr,
									 TScreenManager::kDefaultPortraitWidth,
									 TScreenManager::kDefaultPortraitHeight,
									 SDL_WINDOW_HIGH_PIXEL_DENSITY
                                     // | SDL_WINDOW_FULLSCREEN // On Android, fills really everything
                                     // | SDL_WINDOW_BORDERLESS // Android still has a title bar
                                     // | SDL_WINDOW_MAXIMIZED // Still a title bar
                                     ,
									 &mSDLWindow, &mSDLRenderer)) {
		mLog->FLogLine("SDL_CreateWindowAndRenderer() failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	int win_w = TScreenManager::kDefaultPortraitWidth;
	int win_h = TScreenManager::kDefaultPortraitHeight;
	SDL_GetWindowSize(mSDLWindow, &win_w, &win_h);
	mScreenScaleX = (float)TScreenManager::kDefaultPortraitWidth / win_w;
	mScreenScaleY = (float)TScreenManager::kDefaultPortraitHeight / win_h;
    // FIXME: on the Medium Phone emulator, the screen decoration overlaps
    // the Einstein screen, hiding the top and bottom contents

#if 0
	int w, h;
	SDL_GetWindowSize(mSDLWindow, &w, &h);
	mLog->FLogLine("SDL_GetWindowSize: %d %d", w, h);
	SDL_GetWindowSizeInPixels(mSDLWindow, &w, &h);
	mLog->FLogLine("SDL_GetWindowSizeInPixels: %d %d", w, h);
	SDL_GetRenderOutputSize(mSDLRenderer, &w, &h);
	mLog->FLogLine("SDL_GetRenderOutputSize: %d %d", w, h);
	SDL_GetCurrentRenderOutputSize(mSDLRenderer, &w, &h);
	mLog->FLogLine("SDL_GetCurrentRenderOutputSize: %d %d", w, h);
	float f = SDL_GetWindowDisplayScale(mSDLWindow);
	mLog->FLogLine("SDL_GetWindowDisplayScale: %g", f);
	SDL_Rect r;
	SDL_GetWindowSafeArea(mSDLWindow, &r);
	mLog->FLogLine("SDL_GetWindowSafeArea: %d %d %d %d", r.x, r.y, r.w, r.h);
	//SDL_GetDisplayUsableBounds(SDL_DisplayID displayID, SDL_Rect *rect);

/* macOS Retina
 320x480
 640x960
 640x960
 640x960
 2
 */
/* Onyx
 1404x1722 for all
 2.1875
 */
	// https://wiki.libsdl.org/SDL3/SDL_GetWindowSizeInPixels
	// https://wiki.libsdl.org/SDL3/SDL_GetWindowSize
	// https://wiki.libsdl.org/SDL3/SDL_GetRenderOutputSize
	// https://wiki.libsdl.org/SDL3/SDL_GetCurrentRenderOutputSize
	// https://wiki.libsdl.org/SDL3/SDL_GetWindowDisplayScale
#endif

	mSDLTexture = SDL_CreateTexture(mSDLRenderer,
								SDL_PIXELFORMAT_RGBX8888,
								SDL_TEXTUREACCESS_STATIC,
								TScreenManager::kDefaultPortraitWidth,
								TScreenManager::kDefaultPortraitHeight);
	if (!mSDLTexture) {
		mLog->FLogLine("SDL_CreateTexture() failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	return SDL_APP_CONTINUE;
}


/**
 \brief Update the boot state and send an event, so the state machine advances.
 \param newState set this state, so we continue with that task after returning.
 */
void TSDLApp::ChangeBootState(BootState newState)
{
	mBootState = newState;
	SDL_Event event { mSDLBootStateEvent };
	SDL_PushEvent(&event);
}


/**
 \brief Handle any event that SDL may throw at us.
 Dispatch incoming event, or, if the code is short, handle them directly here.
 \param event the SDL event
 \return SDL_APP_FAILURE if something went wrong. This may quit Einstein.
 \return SDL_APP_SUCCESS request to quit Einstein.
 \return SDL_APP_CONTINUE if everything went well.
 */
SDL_AppResult TSDLApp::HandleSDLEvent(SDL_Event *event)
{
	auto event_type = event->type;

	if (event_type == mSDLBootStateEvent)
		return HandleBootStateChange();

	switch (event_type) {
		case SDL_EVENT_QUIT:  /* triggers on last window close and other things. End the program. */
			return SDL_APP_SUCCESS;

		case SDL_EVENT_KEY_DOWN:  /* quit if user hits ESC key */
			if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
				return SDL_APP_SUCCESS;
			}
			break;

		case SDL_EVENT_MOUSE_MOTION:  /* keep track of the latest mouse position */
			if (gApp && gApp->GetScreenManager() && mSDLTexture) {
				auto scrn = gApp->GetScreenManager();
				SDL_MouseMotionEvent *e = (SDL_MouseMotionEvent*)event;
				// FIXME: find the x and y scale and offset between window and texture
				if (e->state & SDL_BUTTON_LMASK)
					scrn->PenDown(e->x * mScreenScaleX, e->y * mScreenScaleY);
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (gApp && gApp->GetScreenManager() && mSDLTexture) {
				auto scrn = gApp->GetScreenManager();
				SDL_MouseButtonEvent *e = (SDL_MouseButtonEvent*)event;
				scrn->PenDown(e->x * mScreenScaleX, e->y * mScreenScaleY);
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			if (gApp && gApp->GetScreenManager() && mSDLTexture) {
				auto scrn = gApp->GetScreenManager();
				//SDL_MouseButtonEvent *e = (SDL_MouseButtonEvent*)event;
				scrn->PenUp();
			}
			break;
	}
	return SDL_APP_CONTINUE;
}

/**
 \brief Handle a change in the boot state machine.
 \return SDL_APP_FAILURE if something went wrong. This may quit Einstein.
 \return SDL_APP_SUCCESS to quit Einstein.
 \return SDL_APP_CONTINUE if everything went well.
 */
SDL_AppResult TSDLApp::HandleBootStateChange()
{
	switch (mBootState) {
		case BootState::LoadROM:
			mROMImage = new TFlatROMImageWithREX(mROMPath.c_str(), nullptr);
			if (mROMImage->GetErrorCode() == TAIFROMImageWithREXes::kNoError) {
				ChangeBootState(BootState::Run);
			} else {
				delete mROMImage;
				mROMImage = nullptr;
				ChangeBootState(BootState::ROMNotFound);
			}
			break;
		case BootState::ROMNotFound: // TODO: fix dialog box text
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
									 "Emulator ROM Image",
									 "Loading the Newton ROM image failed\n\n"
									 "Please choose a ROM image file next.",
									 mSDLWindow);
			ChangeBootState(BootState::PickROM);
			break;
		case BootState::PickROM:
			PickROMFile();
			break;
		case BootState::Run:
			Run();	// TODO: evaluate return value
			break;
		case BootState::Running:
			// Nothing to do.
			break;
		case BootState::Exit:
			return SDL_APP_SUCCESS;	// friendly exit
		default:
			mLog->FLogLine("HandleBootStateChange, state %d not handled\n", (int)mBootState);
			break;
	}
	return SDL_APP_CONTINUE;
}


/**
 \brief Open a file chooser to pick a ROM file from the file system.
 This call is asynchronous and will call the callback when the user closes
 the file chooser. Tested on Android and macOS
 \return SDL_APP_CONTINUE. The callback will update the state machine.
 */
SDL_AppResult TSDLApp::PickROMFile()
{
	auto callback = [](void*, const char * const *filelist, int filter) -> void
	{ gApp->ROMFilePicked(filelist, filter); };
	SDL_ShowOpenFileDialog(callback, nullptr, mSDLWindow, nullptr, 0, nullptr, false);
	return SDL_APP_CONTINUE;
}


/**
 \brief Copy the selected file to an app private space if one was selected.
 \param filelist pointer to an array of absolute paths. May be NULL if the
		user canceled the operation
 \param filter not used.
 \return void, but updates the state machine.
 */
void TSDLApp::ROMFilePicked(const char * const *filelist, int filter)
{
	(void)filter;
	if ((filelist == nullptr) || (filelist[0] == nullptr)) {
		ChangeBootState(BootState::ROMNotFound); // or Exit
	} else {
		if (SDL_CopyFile(filelist[0], mROMPath.c_str())) {
			ChangeBootState(BootState::LoadROM);
		} else {
			mLog->FLogLine("Can't copy ROM file '%s' to '%s': %s", filelist[0], mROMPath.c_str(), SDL_GetError());
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
									 "Can't copy file",
									 "(details here)",
									 mSDLWindow);
			ChangeBootState(BootState::PickROM);
		}
	}
}


/**
 \brief Allocate the remaining part of the emulator an run the emulation in a new thread.
 \todo error handling
 \return SDL_APP_CONTINUE if the emulator was started
 \return SDL_APP_ERROR if something went wrong
 */
SDL_AppResult TSDLApp::Run()
{
	mFlashPath = mPublicDataPath / "einstein.flash";
	SDL_CreateDirectory(mPublicDataPath.c_str());

	mScreenManager = new TSDLScreenManager(mLog);

	mSoundManager = new TNullSoundManager(mLog);

	mNetworkManager = new TNullNetworkManager(mLog);

	mEmulator = new TEmulator(mLog,
							  mROMImage,			// TODO
							  mFlashPath.c_str(),	// TODO
							  mSoundManager,		// TODO
							  mScreenManager,		// TODO
							  mNetworkManager,		// TODO
							  0x00400000,			// RAM Size
							  nullptr);				// TODO: printer driver

	mPlatformManager = mEmulator->GetPlatformManager();

	mEmulator->SerialPorts.Initialize(TSerialPorts::kNullDriver,
									  TSerialPorts::kNullDriver,
									  TSerialPorts::kNullDriver,
									  TSerialPorts::kNullDriver);

	//	MountPCCardsKeptInSlot();

	// launch the actual emulation in the background
	auto emulatorThread = new std::thread(&TSDLApp::EmulatorThreadEntry, this);
	(void)emulatorThread;

	ChangeBootState(BootState::Running);
	return SDL_APP_CONTINUE;
}


/**
 \brief SDL calls and wants us to render a new frame.
 This code is just proof of concept and needs to be optimized a lot. Most of it
 should probably move to TSDLScreenManager.
 */
SDL_AppResult TSDLApp::IterateSDL()
{
	// FIXME:: optimize this!

	bool changed = false;

	if (gApp && gApp->GetScreenManager() && mSDLTexture) {
		const int ww = 320;
		const int hh = 480;
		static uint32_t buf[ww*hh];
		static uint32_t rgb_lut[16] = {
			0x00000000, 0x11111100, 0x22222200, 0x33333300,
			0x44444400, 0x55555500, 0x66666600, 0x77777700,
			0x88888800, 0x99999900, 0xaaaaaa00, 0xbbbbbb00,
			0xcccccc00, 0xdddddd00, 0xeeeeee00, 0xffffff00 };
		auto scrn = gApp->GetScreenManager();
		uint32_t *d = buf;
		for (int y=0; y<hh; y++) {
			uint8_t *s = scrn->GetScreenBuffer() + y*(TScreenManager::kDefaultPortraitWidth/2);
			for (int x=0; x<ww/2; x++) {
				uint8_t pp = *s++;
				uint8_t lt = (pp >> 4);
				*d++ = rgb_lut[lt];
				uint8_t rt = (pp & 0x0f);
				*d++ = rgb_lut[rt];
			}
		}
		SDL_Rect dirty = { 0, 0, ww, hh };
		changed = SDL_UpdateTexture(mSDLTexture,
									&dirty,
									buf,
									ww*4); //TScreenManager::kDefaultPortraitWidth*4);
	}

	if (changed) { // Sure? We may have to do that every time.
		// Not needed once we fully rendered the texture.
		SDL_SetRenderDrawColor(mSDLRenderer, 0xaa, 0xaa, 0xaa, 255);
		SDL_RenderClear(mSDLRenderer);

		// Copy the texture to the screen.
		//SDL_SetDefaultTextureScaleMode(mSDLRenderer, SDL_SCALEMODE_NEAREST);
		SDL_SetTextureScaleMode(mSDLTexture, SDL_SCALEMODE_NEAREST);
		//SDL_SetTextureScaleMode(mSDLTexture, SDL_SCALEMODE_LINEAR);
		SDL_RenderTexture(mSDLRenderer, mSDLTexture, NULL, NULL);

		// We can still do some overly rendering here if we like
		SDL_SetRenderTarget(mSDLRenderer, NULL);

		// Present everything to the user.
		SDL_RenderPresent(mSDLRenderer);
	}

	return SDL_APP_CONTINUE;
}


/**
 \brief Emulator thread entry point.
 */
void
TSDLApp::EmulatorThreadEntry()
{
	mEmulator->Run();
	ChangeBootState(BootState::Exit);
}


// =================== //
// "Knock, knock."     //
// "Who's there?"      //
//                     //
//                     //
//                     //
//                     //
//                     //
//                     //
//                     //
//                     //
//                     //
//                     //
//                     //
//                     //
// "Java"              //
// =================== //
