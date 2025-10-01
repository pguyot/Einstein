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

#include "Emulator/TEmulator.h"
#include "Emulator/Log/TSDLLog.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/Screen/TSDLScreenManager.h"
#include "Emulator/Serial/TSerialPorts.h"
#include "Emulator/Sound/TNullSoundManager.h"

#if TARGET_OS_ANDROID
#include "app/SDL/TSDLAndroid.h"
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_system.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

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

//	mScreenWidth = 480;
//	mScreenHeight = 640;

	SDL_AppResult result = SDL_APP_CONTINUE;

	mLog = new TSDLLog();
	SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	// Init Directories
#if TARGET_OS_ANDROID
	mPrivateDataPath = SDL_GetAndroidInternalStoragePath();
	mPublicDataPath = SDL_GetAndroidExternalStoragePath();
	mROMPath = mPublicDataPath / "ROM.bin";
#else
	mPrivateDataPath = SDL_GetPrefPath("org.messagepad", "einstein");
	mPublicDataPath = SDL_GetUserFolder(SDL_FOLDER_DOCUMENTS);
	mPublicDataPath /= "Einstein";
	mROMPath = mPrivateDataPath / "ROM.bin";
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

 On desktop machines, we will get a window with the requested size. The pixel
 resolution however may be different. We also may want to scale the screen,
 so we need to request the resolution times the scale.

 If the scale is odd and the pixel resolution is high, the Texture should be
 rendered with linear interpolation on.

 If we are on a mobile device, we will get a window that is full screen, and
 usually a different size than the requested one. So after opening the window,
 we musta adjust the emulation size so that the Newton screen fits best.

 \return SDL_APP_FAILURE if something went wrong.
 \return SDL_APP_CONTINUE if everything went well.
 */
SDL_AppResult TSDLApp::InitSDLGraphics()
{
	SDL_SetAppMetadata("Einstein emulator for Newton MP2x00", "1.0", "org.messagepad.einstein");
	SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");

	if (!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS)) {
		mLog->FLogLine("SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer(nullptr,
									 mScreenWidth,
									 mScreenHeight + mToolbarHeight,
									 SDL_WINDOW_HIGH_PIXEL_DENSITY
                                     // | SDL_WINDOW_FULLSCREEN // On Android, fills really everything
                                     // | SDL_WINDOW_BORDERLESS // Android still has a title bar
                                     // | SDL_WINDOW_MAXIMIZED // Still a title bar
                                     ,
									 &mSDLWindow, &mSDLRenderer)) {
		mLog->FLogLine("SDL_CreateWindowAndRenderer() failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	int win_w = mScreenWidth;
	int win_h = mScreenHeight;
	SDL_GetWindowSize(mSDLWindow, &win_w, &win_h);
	mScreenScaleX = (float)mScreenWidth / win_w;
	mScreenScaleY = (float)(mScreenHeight + mToolbarHeight) / win_h;

	int pix_w = win_w;
	int pix_h = win_h;
	SDL_GetWindowSizeInPixels(mSDLWindow, &pix_w, &pix_h);
	mPixelScaleX = (float)pix_w / win_w;
	mPixelScaleY = (float)pix_h / win_h;

//	int w, h;
//	SDL_GetWindowSize(mSDLWindow, &w, &h);
//	mLog->FLogLine("SDL_GetWindowSize: %d %d", w, h);
//	SDL_GetWindowSizeInPixels(mSDLWindow, &w, &h);
//	mLog->FLogLine("SDL_GetWindowSizeInPixels: %d %d", w, h);
//	SDL_GetRenderOutputSize(mSDLRenderer, &w, &h);
//	mLog->FLogLine("SDL_GetRenderOutputSize: %d %d", w, h);
//	SDL_GetCurrentRenderOutputSize(mSDLRenderer, &w, &h);
//	mLog->FLogLine("SDL_GetCurrentRenderOutputSize: %d %d", w, h);
//	float f = SDL_GetWindowDisplayScale(mSDLWindow);
//	mLog->FLogLine("SDL_GetWindowDisplayScale: %g", f);
//	SDL_Rect r;
//	SDL_GetWindowSafeArea(mSDLWindow, &r);
//	mLog->FLogLine("SDL_GetWindowSafeArea: %d %d %d %d", r.x, r.y, r.w, r.h);
//
//    int num_disp;
//    SDL_DisplayID *disp = SDL_GetDisplays(&num_disp);
//    if (disp) {
//        if (num_disp > 0) {
//            SDL_GetDisplayUsableBounds(disp[0], &r);
//            mLog->FLogLine("SDL_GetDisplayUsableBounds: %d %d %d %d", r.x, r.y, r.w, r.h);
//            SDL_GetDisplayBounds(disp[0], &r);
//            mLog->FLogLine("SDL_GetDisplayBounds: %d %d %d %d", r.x, r.y, r.w, r.h);
//        }
//        SDL_free(disp);
//    }

#if USE_SDL_LOCK_TEXTURE
	constexpr SDL_TextureAccess texture_access = SDL_TEXTUREACCESS_STREAMING;
#else
	constexpr SDL_TextureAccess texture_access = SDL_TEXTUREACCESS_STATIC;
#endif

	// Find the fastest render format for the texture (take the first one in the list that matches)
SDL_PixelFormat pf = SDL_PIXELFORMAT_ARGB8888;
SDL_PropertiesID prop_id = SDL_GetRendererProperties(mSDLRenderer);
if (prop_id) {
	const SDL_PixelFormat *fmts = (SDL_PixelFormat*)SDL_GetPointerProperty(prop_id, SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER, nullptr);
	if (fmts) {
		for (int i=0;;++i) {
			SDL_PixelFormat pf = fmts[i];
			if (pf == SDL_PIXELFORMAT_UNKNOWN) break;
			if (pf == SDL_PIXELFORMAT_ARGB8888) { mTextureEncoding = 0; break; }
			if (pf == SDL_PIXELFORMAT_XRGB8888) { mTextureEncoding = 1; break; }
			if (pf == SDL_PIXELFORMAT_ABGR8888) { mTextureEncoding = 2; break; }
			if (pf == SDL_PIXELFORMAT_XBGR8888) { mTextureEncoding = 3; break; }
			if (pf == SDL_PIXELFORMAT_RGBA8888) { mTextureEncoding = 4; break; }
			if (pf == SDL_PIXELFORMAT_RGBX8888) { mTextureEncoding = 5; break; }
			if (pf == SDL_PIXELFORMAT_BGRA8888) { mTextureEncoding = 6; break; }
			if (pf == SDL_PIXELFORMAT_BGRX8888) { mTextureEncoding = 7; break; }
		}
	}
}

	mSDLTexture = SDL_CreateTexture(mSDLRenderer,
									pf,
									texture_access,
									mScreenWidth,
									mScreenHeight + mToolbarHeight);
	(void)mToolbarHeight;
	if (!mSDLTexture) {
		mLog->FLogLine("SDL_CreateTexture() failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
SDL_SetTextureBlendMode(mSDLTexture, SDL_BLENDMODE_NONE);

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
				if (e->state & SDL_BUTTON_LMASK) {
					int mx = e->x * mScreenScaleX;
					int my = e->y * mScreenScaleY - mToolbarHeight;
					if (mPenDown == 1) {
						scrn->PenDown(std::clamp(mx, 0, mScreenWidth-1),
									  std::clamp(my, 0, mScreenHeight-1));
					} else if (mPenDown == 2) {
						mPenMoveN = (mScreenWidth - mx + mToolbarHeight/2) / mToolbarHeight;
						if ((my >= 0) || (my <= -mToolbarHeight)) mPenMoveN = -1;
						// mLog->FLogLine("Pen moved to button %d", mPenMoveN);
						// Visualize if the button is still selected
					}
				}
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (gApp && gApp->GetScreenManager() && mSDLTexture) {
				auto scrn = gApp->GetScreenManager();
				SDL_MouseButtonEvent *e = (SDL_MouseButtonEvent*)event;
				if (e->button == SDL_BUTTON_LEFT) {
					int mx = e->x * mScreenScaleX;
					int my = e->y * mScreenScaleY - mToolbarHeight;
					if (my >= 0) {
						mPenDown = 1;
						scrn->PenDown(std::clamp(mx, 0, mScreenWidth-1),
									  std::clamp(my, 0, mScreenHeight-1));
					} else {
						mPenDown = 2;
						mPenDownN = mPenMoveN = (mScreenWidth - mx + mToolbarHeight/2) / mToolbarHeight;
						// mLog->FLogLine("Pen down on button %d", mPenDownN);
						// Visualize if the button is still selected
					}
				}
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			if (gApp && gApp->GetScreenManager() && mSDLTexture) {
				auto scrn = gApp->GetScreenManager();
				SDL_MouseButtonEvent *e = (SDL_MouseButtonEvent*)event;
				if (e->button == SDL_BUTTON_LEFT) {
					//int mx = e->x * mScreenScaleX;
					int my = e->y * mScreenScaleY - mToolbarHeight;
					if (mPenDown == 1) {
						scrn->PenUp();
					} else if (mPenDown == 2) {
						if (   (mPenDownN == mPenMoveN)
							&& (my < 0) && (my > -mToolbarHeight))
						{
							switch (mPenDownN) {
								case 1:
									PickPackageFile();
									break;
							}
						}
					}
					mPenDown = 0;
				}
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
		case BootState::PickPackage:
			PickPackageFile();
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
	// TODO: ask for full screen refresh
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

	mScreenManager = new TSDLScreenManager(mLog,
										   mScreenWidth,
										   mScreenHeight
										   );

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
	// Ignore this call if less than a 60th of a second has passed
	static Uint64 prev_call = 0;
	Uint64 now = SDL_GetTicks();
	if (now-prev_call < 14) return SDL_APP_CONTINUE; // 1/60th = 16.66
	prev_call = now;

	if (mSDLTexture && mScreenManager) {
		bool changed = mScreenManager->UpdateTexture(mSDLTexture, mTextureEncoding, mToolbarHeight);
		if (changed) {
			// Copy the texture to the screen (SDL_SCALEMODE_NEAREST or SDL_SCALEMODE_LINEAR)
			SDL_SetTextureScaleMode(mSDLTexture, SDL_SCALEMODE_NEAREST);
			SDL_RenderTexture(mSDLRenderer, mSDLTexture, NULL, NULL);
			// We can still do some overly rendering here if we like
			SDL_SetRenderTarget(mSDLRenderer, NULL);
			// Test having a menu button:
			SDL_SetRenderDrawColor(mSDLRenderer, 255, 0, 0, 255);
			SDL_FRect rf = {
				(mScreenWidth - mToolbarHeight - mToolbarHeight/2)*mPixelScaleX/mScreenScaleX,
				0,
				mToolbarHeight*mPixelScaleX/mScreenScaleX,
				mToolbarHeight*mPixelScaleY/mScreenScaleY };
			SDL_RenderFillRect(mSDLRenderer, &rf);
			SDL_RenderPresent(mSDLRenderer);
		}
	} else if (mSDLRenderer) {
		SDL_SetRenderTarget(mSDLRenderer, nullptr);
		SDL_SetRenderDrawColor(mSDLRenderer, 0xaa, 0xaa, 0xaa, 255);
		SDL_RenderClear(mSDLRenderer);
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


void TSDLApp::UserActionInstallPackage()
{
	ChangeBootState(BootState::PickPackage);
}

SDL_AppResult TSDLApp::PickPackageFile()
{
	auto callback = [](void*, const char * const *filelist, int filter) -> void
	{ gApp->PackageFilePicked(filelist, filter); };
	static const SDL_DialogFileFilter filters[] = {
		{ "Newton Packages",	"pkg;newtonpkg" },
		{ "All files",   		"*" }
	};
	SDL_ShowOpenFileDialog(callback, nullptr, mSDLWindow, filters, 2, nullptr, true);
	ChangeBootState(BootState::Running);
	return SDL_APP_CONTINUE;
}

void TSDLApp::PackageFilePicked(const char * const *filelist, int filter)
{
	(void)filter;
	// TODO: ask for full screen refresh
	if (filelist == nullptr) return;
		ChangeBootState(BootState::ROMNotFound); // or Exit
	for (int i = 0; ; ++i) {
		const char *filename = filelist[i];
		if (filename == nullptr) break;
		mLog->FLogLine("%d: Installing package %s", i, filename);
		size_t size { 0 };
		void *pkg = SDL_LoadFile(filename, &size);
		if (pkg) {
			mPlatformManager->InstallPackage((KUInt8*)pkg, (KUInt32)size);
			::free(pkg);
		} else {
			mLog->FLogLine("Can't load file: %s", SDL_GetError());
		}
		mPlatformManager->InstallPackage(filename);
	}
	ChangeBootState(BootState::Running);
}



// =================== //
// "Knock, knock."     //
// "Who's there?"      //
//                     //
//                     //
//                     //
//  .                  //
//                     //
//  .                  //
//                     //
//  .                  //
//                     //
//                     //
//                     //
//                     //
// "Java"              //
// =================== //
