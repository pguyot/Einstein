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

/*
 \note On some Android devices (tablets), if the device is in landscape, but
       we ask for a portrait, we first get a landscape window and the SDL_EVENT_WINDOW_SHOWN,
       but then the display mode adapts to the requested portrait mode and window resizes,
       sending SDL_EVENT_WINDOW_RESIZED. The screen is still in landscape, but the window
       now only fills the center of the screen.

       This is all nice, but Einstein is now initialised for landscape and can't easily
       change into portrait mode.

 Order of actions and events:
    setOrientation
 onStart
 onResume
 surfaceCreated
 surfaceCHanged
 windowSize
 SDL_EVENT_WINDOW_MOUSE_ENTER
 SDL_EVENT_WINDOW_FOCUS_GAINED
 SDL_EVENT_WINDOW_SAFE_AREA_CHANGED
 SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED
 SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED
 SDL_EVENT_WINDOW_FOCUS_GAINED
 SDL_EVENT_WINDOW_SHOWN (still 2560 1464)
 SDL_EVENT_USER
    onConfigurationChanged, surfaceChanged, ..
 SDL_EVENT_AUDIO_DEVICE_ADDED
    SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED
    SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED
    SDL_EVENT_WINDOW_RESIZED
    SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED
    SDL_EVENT_WINDOW_SAFE_AREA_CHANGED
 SDL_EVENT_USER (TSerialManager)
 SDL_EVENT_USER (colors)
 ... Mouse Down, Finger down, Motion, Up
    Medium Tablet crashes in TScreenManager::Blit_0(), no issue if tablet is in portrait
 */

#define SDL_MAIN_USE_CALLBACKS

#include <K/Defines/KDefinitions.h>

#include <algorithm>

#include "TSDLApp.h"

#include "Emulator/TEmulator.h"
#include "Emulator/TARMProcessor.h"
#include "Emulator/Log/TSDLLog.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Network/TUsermodeNetwork.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/Screen/TSDLScreenManager.h"
#include "Emulator/Serial/TSerialPorts.h"
#include "Emulator/Sound/TNullSoundManager.h"
#include "app/SDL/TSDLSoundManager.h"

#if TARGET_OS_ANDROID
#include "app/SDL/TSDLAndroid.h"
#endif
// xxd --include Resources/icons/button_install.bmp > app/SDL/TSDLIconInstall.cpp
#include "app/SDL/TSDLIconInstall.h" // Resources_icons_button_install_bmp
#include "app/SDL/TSDLIconPower.h"   // Resources_icons_button_power_bmp
#include "app/SDL/TSDLIconMenu.h"    // Resources_icons_button_prefs_bmp

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_system.h>
#include <SDL3/SDL_events.h>
#include "app/SDL/TSDLEventDescription.h" // E_SDL_GetEventDescription

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
	TSDLLog::InstallCapture();
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

	mScreenWidth = (mScreenWidth + 1) & ~1; // Round up to an even number of pixels

    // TODO: before we create a window on a fullscreen device (Android, iOS), get
    // the display mode and launch Einstein in Landscape or Portrait as needed.
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

	int pix_w = win_w;
	int pix_h = win_h;
	SDL_GetWindowSizeInPixels(mSDLWindow, &pix_w, &pix_h);
	mPixelScaleX = (float)pix_w / win_w;
	mPixelScaleY = (float)pix_h / win_h;

	// If we get a different window size than requested, we can adjust the
	// emulation screen size for best size and aspect ratio. The original
	// MessagePad has 96 dpi.
#if TARGET_OS_ANDROID || TARGET_OS_IOS
	int dpi = SDL_GetWindowDisplayScale(mSDLWindow) * 160;
#else
	int dpi = SDL_GetWindowDisplayScale(mSDLWindow) * 96;
#endif
	if ((win_w != mScreenWidth) || (win_h != (mScreenHeight + mToolbarHeight))) {
		if (mAdjustToAspectRatio) {
			float src_ar = (float)mScreenWidth / (mScreenHeight + mToolbarHeight);
			float win_ar = (float)win_w / win_h;
			if (src_ar > win_ar) {
				mScreenHeight = (mScreenWidth / win_ar) - mToolbarHeight;
			} else {
				mScreenWidth = (mScreenHeight + mToolbarHeight) * win_ar;
			}
		}
		if (mAdjustToScreenDPI && (dpi != 0)) {
			float win_w_in = pix_w / dpi;	// width of window in inches
			int mp_width = win_w_in * 97;	// number of pixels at 97 dpi
			if (mp_width > mScreenWidth) {
				float size_ratio = (float)mp_width / mScreenWidth;
                mScreenWidth = mp_width;
				mScreenHeight *= size_ratio;
			}
		}
	}
	mScreenWidth = (mScreenWidth + 1) & ~1; // Round up to an even number of pixels

	UpdateInputScale();

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

	return CreateScreenTexture();
}

/**
 \brief Compute how window coordinates map to the emulated screen.
 The texture (toolbar plus Newton screen) is stretched over the whole window,
 so the scales follow the current window size. Called at launch and whenever
 the window is resized.
 */
void TSDLApp::UpdateInputScale()
{
	int win_w = 0, win_h = 0;
	SDL_GetWindowSize(mSDLWindow, &win_w, &win_h);
	if (win_w <= 0 || win_h <= 0) return;
	int pix_w = win_w, pix_h = win_h;
	SDL_GetWindowSizeInPixels(mSDLWindow, &pix_w, &pix_h);
	mPixelScaleX = (float)pix_w / win_w;
	mPixelScaleY = (float)pix_h / win_h;
	mScreenScaleX = (float)mScreenWidth / win_w;
	mScreenScaleY = (float)(mScreenHeight + mToolbarHeight) / win_h;
}

/**
 \brief Create the texture that holds the toolbar and the Newton screen.
 Called at launch, and again after SDL_EVENT_RENDER_DEVICE_RESET, when the
 old texture belonged to the lost device.
 \return SDL_APP_FAILURE if the texture could not be created.
 \return SDL_APP_CONTINUE if everything went well.
 */
SDL_AppResult TSDLApp::CreateScreenTexture()
{
	if (mSDLTexture) {
		SDL_DestroyTexture(mSDLTexture);
		mSDLTexture = nullptr;
	}

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
	mToolbarColor =  (mTextureEncoding < 4) ? 0xffcccccc : 0xccccccff;

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

    // For debugging, until we have the correct order of SDL events down
    static char buf[1024];
    E_SDL_GetEventDescription(event, buf, 1023);

    (void)mSDLWindow;
    int win_w;
    int win_h;
    SDL_GetWindowSize(mSDLWindow, &win_w, &win_h);
    mLog->FLogLine("Handle event %s : %d %d %d",
                   buf, win_w, win_h, (int)mBootState);
	if (mMenuEventType != 0 && event_type == mMenuEventType) {
		// A choice from the native Android menu (see ShowMenu()).
		HandleMenuChoice(event->user.code);
		return SDL_APP_CONTINUE;
	}
	if (event_type == mSDLBootStateEvent)
		return HandleBootStateChange();

	switch (event_type) {
		case SDL_EVENT_QUIT:  /* triggers on last window close and other things. End the program. */
			return SDL_APP_SUCCESS;

		case SDL_EVENT_WINDOW_RESIZED:
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			// The texture keeps its size and is stretched over the new
			// window, so taps must be mapped with the new scale.
			UpdateInputScale();
			RedrawAll();
			break;

		case SDL_EVENT_WINDOW_EXPOSED:
		case SDL_EVENT_RENDER_TARGETS_RESET:
			// The window surface may be new (Android recreates it after the
			// app was in the background), or the contents of render targets
			// were lost: the texture is still valid, draw everything again.
			RedrawAll();
			break;

		case SDL_EVENT_RENDER_DEVICE_RESET:
			// The render device was lost, and all its textures with it.
			// Make a new texture before redrawing into it.
			mLog->FLogLine("Render device reset, recreating the screen texture");
			if (CreateScreenTexture() != SDL_APP_CONTINUE)
				return SDL_APP_FAILURE;
			RedrawAll();
			break;

		case SDL_EVENT_WILL_ENTER_BACKGROUND:
			// Android may kill a background app without warning. Press the
			// power switch now, so NewtonOS saves what it keeps in memory and
			// the flash is not left in the middle of a write.
			SleepNewton(0);
			break;

		case SDL_EVENT_DID_ENTER_BACKGROUND:
		case SDL_EVENT_TERMINATING:
			// SDL dispatches these at once from its event pump. On Android,
			// right after this the SDL thread blocks until the app resumes
			// (SDL_HINT_ANDROID_BLOCK_ON_PAUSE), while the emulator thread
			// keeps running. Wait here until NewtonOS is really asleep, so
			// the app is not paused (or killed) halfway through going to
			// sleep. This blocks only the SDL thread, not Android's UI thread.
			SleepNewton(kSleepTimeoutMS);
			break;

		case SDL_EVENT_DID_ENTER_FOREGROUND:
			// Back from the background: the Newton may have gone to sleep
			// meanwhile, and there is no power button yet. SDL dispatches
			// this at once from inside its event pump (and SDL may deliver
			// app events from another thread on some platforms), so the
			// wake-up and redraw happen in IterateSDL().
			mForegroundPending = true;
			break;

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
						mPenMoveN = (mScreenWidth - mx + mToolbarHeight/4) / mToolbarHeight;
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
					if (mPlatformManager && !mPlatformManager->IsPowerOn()) {
						// A tap wakes a sleeping Newton, like its power switch.
						WakeNewton();
						RedrawAll();
						break;
					}
					if (my >= 0) {
						mPenDown = 1;
						scrn->PenDown(std::clamp(mx, 0, mScreenWidth-1),
									  std::clamp(my, 0, mScreenHeight-1));
					} else if (mx < mScreenWidth / 3) {
						// Left part of the toolbar: show or hide the log.
						mShowLog = !mShowLog;
						RedrawAll();
					} else {
						mPenDown = 2;
						mPenDownN = mPenMoveN = (mScreenWidth - mx + mToolbarHeight/4) / mToolbarHeight;
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
									UserActionInstallPackage();
									break;
								case 2:
									if (mPlatformManager) mPlatformManager->SendPowerSwitchEvent();
									break;
								case 3:
									ShowMenu();
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
		case BootState::LoadROM: {
			std::error_code ec;
			auto romSize = std::filesystem::file_size(mROMPath, ec);
			mLog->FLogLine("ROM file %s: %s, %lld bytes", mROMPath.string().c_str(),
						   ec ? ec.message().c_str() : "found", ec ? -1LL : (long long)romSize);
		}
			mROMImage = new TFlatROMImageWithREX(mROMPath.string().c_str(), nullptr);
			mLog->FLogLine("ROM image error code: %d", (int)mROMImage->GetErrorCode());
			{
				static const char* kROMNames[] = { "MessagePad 2x00 US (717006)", "MessagePad 2x00 German",
												   "eMate 300", "MessagePad 2100 French (Watson)" };
				KSInt32 id = mROMImage->GetROMId();
				mLog->FLogLine("ROM is %s, checksum %s", (id >= 0 && id < 4) ? kROMNames[id] : "UNKNOWN",
							   mROMImage->CRCValid() ? "matches" : "DOES NOT MATCH a known dump");
			}
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
	RefreshScreen();
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
		if (SDL_CopyFile(filelist[0], mROMPath.string().c_str())) {
			ChangeBootState(BootState::LoadROM);
		} else {
			mLog->FLogLine("Can't copy ROM file '%s' to '%s': %s", filelist[0], mROMPath.string().c_str(), SDL_GetError());
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
									 "Can't copy file",
									 "(details here)",
									 mSDLWindow);
			ChangeBootState(BootState::PickROM);
		}
	}
	RefreshScreen();
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
	SDL_CreateDirectory(mPublicDataPath.string().c_str());
	{
		std::error_code ec;
		auto flashSize = std::filesystem::file_size(mFlashPath, ec);
		mLog->FLogLine("Flash file %s: %s, %lld bytes", mFlashPath.string().c_str(),
					   ec ? "new" : "found", ec ? 0LL : (long long)flashSize);
		mLog->FLogLine("Emulated screen %d x %d", mScreenWidth, mScreenHeight);
	}

	mScreenManager = new TSDLScreenManager(mLog,
										   mScreenWidth,
										   mScreenHeight
										   );

	mSoundManager = new TSDLSoundManager(mLog);

	// Host networking for the NE2000 card (insert it from the menu).
	mNetworkManager = new TUsermodeNetwork(mLog);

	mEmulator = new TEmulator(mLog,
							  mROMImage,			// TODO
							  mFlashPath.string().c_str(),	// TODO
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

	// Back from the background (flag set on another thread).
	if (mForegroundPending.exchange(false)) {
		WakeNewton();
		RedrawAll();
	}

	// A sleep that was still on its way when we came back: wake again.
	if (mWakeUntil) {
		if (now > mWakeUntil) {
			mWakeUntil = 0;
		} else if (mPlatformManager && !mPlatformManager->IsPowerOn()) {
			mWakeUntil = 0;
			WakeNewton();
			RedrawAll();
		}
	}

	if (mSDLTexture && mScreenManager) {
		bool changed = mScreenManager->UpdateTexture(mSDLTexture, mTextureEncoding, mToolbarHeight);
		if (mRefreshToolbar) changed |= RedrawToolbar();
		bool overlay = mShowLog;
		if (changed || mRefreshScreen || overlay) {
			// Copy the texture to the screen (SDL_SCALEMODE_NEAREST or SDL_SCALEMODE_LINEAR)
			SDL_SetTextureScaleMode(mSDLTexture, SDL_SCALEMODE_NEAREST);
			SDL_RenderTexture(mSDLRenderer, mSDLTexture, NULL, NULL);
			// We can still do some overly rendering here if we like
			SDL_SetRenderTarget(mSDLRenderer, NULL);
			if (overlay) DrawLogOverlay();
			// Test having a menu button:
//			SDL_SetRenderDrawColor(mSDLRenderer, 255, 0, 0, 255);
//			SDL_FRect rf = {
//				(mScreenWidth - mToolbarHeight - mToolbarHeight/4)*mPixelScaleX/mScreenScaleX,
//				0,
//				mToolbarHeight*mPixelScaleX/mScreenScaleX,
//				mToolbarHeight*mPixelScaleY/mScreenScaleY };
//			SDL_RenderFillRect(mSDLRenderer, &rf);
			SDL_RenderPresent(mSDLRenderer);
			mRefreshScreen = false;
		}
	} else if (mSDLRenderer) {
		SDL_SetRenderTarget(mSDLRenderer, nullptr);
		SDL_SetRenderDrawColor(mSDLRenderer, 0xaa, 0xaa, 0xaa, 255);
		SDL_RenderClear(mSDLRenderer);
		if (mShowLog) DrawLogOverlay();
		SDL_RenderPresent(mSDLRenderer);
	}

	return SDL_APP_CONTINUE;
}

/**
 \brief Draw the emulator state and the last log lines over the screen.
 */
void TSDLApp::DrawLogOverlay()
{
	int out_w = 0, out_h = 0;
	SDL_GetCurrentRenderOutputSize(mSDLRenderer, &out_w, &out_h);
	if (out_w <= 0 || out_h <= 0) return;
	// About 60 columns of SDL's 8x8 debug font.
	float scale = (float)std::max(1, out_w / 480);
	// At least one column and row, so wrapping always advances.
	int cols = std::max(1, (int)(out_w / (8 * scale)));
	int rows = std::max(1, (int)(out_h / (10 * scale)));
	int top = (int)(mToolbarHeight * mPixelScaleY / mScreenScaleY / (10 * scale)) + 1;

	std::vector<std::string> lines;
	char header[160];
	static const char* kThread[] = { "not started", "running", "ENDED" };
	unsigned pc = 0;
	if (mEmulator && mEmulator->GetProcessor())
		pc = (unsigned)mEmulator->GetProcessor()->GetRegister(15);
	snprintf(header, sizeof(header), "emulator %s, power %s, updates %u",
			 kThread[mEmulatorState.load() % 3],
			 mPlatformManager ? (mPlatformManager->IsPowerOn() ? "on" : "off") : "-",
			 mScreenManager ? mScreenManager->mNewtonUpdates.load() : 0u);
	lines.emplace_back(header);
	// The PC now and four earlier samples, about half a second apart.
	static unsigned pcHistory[4] = { };
	static Uint64 lastSample = 0;
	if (SDL_GetTicks() - lastSample > 500) {
		lastSample = SDL_GetTicks();
		for (int i = 3; i > 0; --i) pcHistory[i] = pcHistory[i-1];
		pcHistory[0] = pc;
	}
	snprintf(header, sizeof(header), "screen %dx%d, pc %08X %08X %08X %08X",
			 mScreenWidth, mScreenHeight, pcHistory[0], pcHistory[1], pcHistory[2], pcHistory[3]);
	lines.emplace_back(header);
	for (auto& line : TSDLLog::RecentLines(rows)) {
		// wrap long lines
		for (size_t i = 0; i < line.size(); i += cols)
			lines.emplace_back(line.substr(i, cols));
	}
	int avail = rows - top;
	if (avail < 3) return;
	// Keep the two header lines, then the newest lines that fit.
	std::vector<std::string> shown(lines.begin(), lines.begin() + 2);
	size_t first = lines.size() > (size_t)avail ? lines.size() - (avail - 2) : 2;
	if (first < 2) first = 2;
	shown.insert(shown.end(), lines.begin() + first, lines.end());

	SDL_SetRenderScale(mSDLRenderer, scale, scale);
	SDL_SetRenderDrawBlendMode(mSDLRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(mSDLRenderer, 0, 0, 0, 200);
	SDL_FRect bg { 0, (float)(top * 10), (float)(out_w / scale), (float)(shown.size() * 10 + 4) };
	SDL_RenderFillRect(mSDLRenderer, &bg);
	for (size_t i = 0; i < shown.size(); ++i) {
		if (i < 2) SDL_SetRenderDrawColor(mSDLRenderer, 255, 220, 80, 255);
		else SDL_SetRenderDrawColor(mSDLRenderer, 255, 255, 255, 255);
		SDL_RenderDebugText(mSDLRenderer, 2, (float)((top + i) * 10 + 2), shown[i].c_str());
	}
	SDL_SetRenderScale(mSDLRenderer, 1, 1);
}

/**
 \brief Draw the whole screen and the toolbar again at the next frame.
 */
void TSDLApp::RedrawAll()
{
	if (mScreenManager) mScreenManager->InvalidateAll();
	mRefreshToolbar = true;
	mRefreshScreen = true;
}

/**
 \brief Press the Newton's power switch if the Newton is asleep.
 */
void TSDLApp::WakeNewton()
{
	if (mSleepRequested) mWakeUntil = SDL_GetTicks() + 5000;
	mSleepRequested = false;
	if (mPlatformManager && !mPlatformManager->IsPowerOn()) {
		mLog->FLogLine("Waking the Newton");
		mPlatformManager->SendPowerSwitchEvent();
	}
}

/**
 \brief Press the Newton's power switch once if the Newton is awake.
 \param inWaitMS if not zero, wait up to this many milliseconds until
 NewtonOS has gone to sleep (it saves its state first).
 */
void TSDLApp::SleepNewton(Uint32 inWaitMS)
{
	if (mEmulatorState != 1) return; // the emulator is not running
	if (!mPlatformManager || !mPlatformManager->IsPowerOn()) return;
	bool expected = false;
	if (mSleepRequested.compare_exchange_strong(expected, true)) {
		mLog->FLogLine("Putting the Newton to sleep");
		mPlatformManager->SendPowerSwitchEvent();
	}
	if (inWaitMS == 0) return;
	Uint64 deadline = SDL_GetTicks() + inWaitMS;
	while (mPlatformManager->IsPowerOn() && (mEmulatorState == 1) && (SDL_GetTicks() < deadline))
		SDL_Delay(10);
	if (mPlatformManager->IsPowerOn())
		mLog->FLogLine("The Newton did not go to sleep within %u ms", (unsigned)inWaitMS);
	else
		mLog->FLogLine("The Newton is asleep");
}

/**
 \brief Redraw the toolbar into the texture buffer.
 \return true if the toolbar was redrawn
 */
bool TSDLApp::RedrawToolbar()
{
	static bool icons_loaded = false;
	static SDL_Surface *button_install_bmp { nullptr };
	static SDL_Surface *button_power_bmp { nullptr };
	static SDL_Surface *button_menu_bmp { nullptr };

	if (!icons_loaded) {
		button_install_bmp = SDL_LoadBMP_IO(SDL_IOFromConstMem(Resources_icons_button_install_bmp,
								sizeof(Resources_icons_button_install_bmp)), true);
		button_power_bmp = SDL_LoadBMP_IO(SDL_IOFromConstMem(Resources_icons_button_power_bmp,
								sizeof(Resources_icons_button_power_bmp)), true);
		button_menu_bmp = SDL_LoadBMP_IO(SDL_IOFromConstMem(Resources_icons_button_prefs_bmp,
								sizeof(Resources_icons_button_prefs_bmp)), true);
		icons_loaded = true; // or at least we tried
	}

	if (mSDLTexture == nullptr) return false;
	SDL_Rect r { 0, 0, mScreenWidth, mToolbarHeight };
	SDL_Surface *surface;
	if (SDL_LockTextureToSurface(mSDLTexture, &r, &surface)) {
		SDL_FillSurfaceRect(surface, &r, mToolbarColor);
		// Buttons from the right: 1 install, 2 power, 3 menu (the mouse
		// handler counts them the same way).
		SDL_Surface *buttons[] = { button_install_bmp, button_power_bmp, button_menu_bmp };
		for (int n = 1; n <= 3; ++n) {
			if (!buttons[n-1]) continue;
			SDL_Rect icon_r { mScreenWidth-n*mToolbarHeight-mToolbarHeight/4+1, 1,
				mToolbarHeight-2, mToolbarHeight-2 };
			SDL_BlitSurfaceScaled(buttons[n-1], nullptr, surface, &icon_r, SDL_SCALEMODE_LINEAR);
		}
		SDL_UnlockTexture(mSDLTexture);
		mRefreshToolbar = false;
		return true;
	}
	return false;
}

/**
 \brief Emulator thread entry point.
 */
void
TSDLApp::EmulatorThreadEntry()
{
	mEmulatorState = 1;
	mLog->FLogLine("Emulator thread started");
	mEmulator->Run();
	mEmulatorState = 2;
	mLog->FLogLine("Emulator thread ended");
	ChangeBootState(BootState::Exit);
}


void TSDLApp::UserActionInstallPackage()
{
	ChangeBootState(BootState::PickPackage);
}

namespace {
	enum {
		kMenuPower = 0,
		kMenuBacklight,
		kMenuInstall,
		kMenuNetwork,
		kMenuReboot,
		kMenuLog,
		kMenuCount
	};
}

/**
 \brief Show the toolbar menu.
 On Android this is a native list dialog; the choice comes back as an SDL
 event (mMenuEventType). Elsewhere SDL's message box offers the same items
 as buttons.
 */
void TSDLApp::ShowMenu()
{
	if (mMenuEventType == 0)
		mMenuEventType = SDL_RegisterEvents(1);
	std::vector<std::string> items(kMenuCount);
	items[kMenuPower] = "Power switch (sleep / wake)";
	items[kMenuBacklight] = "Backlight on / off";
	items[kMenuInstall] = "Install package...";
	items[kMenuNetwork] = mNetworkCardInserted ? "Remove network card" : "Insert network card";
	items[kMenuReboot] = "Restart NewtonOS";
	items[kMenuLog] = mShowLog ? "Hide log" : "Show log";
#if TARGET_OS_ANDROID
	if (mMenuEventType != 0) {
		AndroidShowMenu("Einstein", items, mMenuEventType);
		return;
	}
#endif
	std::vector<SDL_MessageBoxButtonData> buttons;
	for (int i = 0; i < kMenuCount; ++i)
		buttons.push_back({ 0, i, items[i].c_str() });
	buttons.push_back({ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, -1, "Cancel" });
	SDL_MessageBoxData data { SDL_MESSAGEBOX_INFORMATION, mSDLWindow, "Einstein",
		"Choose an action", (int)buttons.size(), buttons.data(), nullptr };
	int choice = -1;
	if (SDL_ShowMessageBox(&data, &choice))
		HandleMenuChoice(choice);
}

/**
 \brief Carry out a menu choice (-1 means Cancel).
 */
void TSDLApp::HandleMenuChoice(int inItem)
{
	switch (inItem) {
		case kMenuPower:
			if (mPlatformManager) mPlatformManager->SendPowerSwitchEvent();
			break;
		case kMenuBacklight:
			if (mPlatformManager) mPlatformManager->SendBacklightEvent();
			break;
		case kMenuInstall:
			UserActionInstallPackage();
			break;
		case kMenuNetwork:
			if (mPlatformManager) {
				mPlatformManager->SendNetworkCardEvent();
				mNetworkCardInserted = !mNetworkCardInserted;
				mLog->FLogLine("Network card %s", mNetworkCardInserted ? "inserted" : "removed");
			}
			break;
		case kMenuReboot:
			if (mPlatformManager) mPlatformManager->EvalNewtonScript("Reboot();");
			break;
		case kMenuLog:
			mShowLog = !mShowLog;
			RedrawAll();
			break;
		default:
			break; // Cancel
	}
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
	if (filelist) {
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
	}
	ChangeBootState(BootState::Running);
	RefreshScreen();
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
