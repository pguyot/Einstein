// ==============================
// File:			TSDLScreenManager.cp
// Project:			Einstein
//
// Copyright 2025 by Matthias Melcher
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

#include "TSDLScreenManager.h"

// Einstein
#include "Emulator/Log/TLog.h"

// SDL3
#include <SDL3/SDL.h>


// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TSDLScreenManager::PowerOn(void)
{
	GetLog()->LogLine("TNullScreenManager::PowerOn()");
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TSDLScreenManager::PowerOff(void)
{
	GetLog()->LogLine("TNullScreenManager::PowerOff()");
}

// -------------------------------------------------------------------------- //
//  * PowerOnScreen( void )
// -------------------------------------------------------------------------- //
void
TSDLScreenManager::PowerOnScreen(void)
{
	GetLog()->LogLine("TNullScreenManager::PowerOnScreen()");
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TSDLScreenManager::PowerOffScreen(void)
{
	GetLog()->LogLine("TNullScreenManager::PowerOffScreen()");
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( Boolean )
// -------------------------------------------------------------------------- //
void
TSDLScreenManager::BacklightChanged(Boolean backlight)
{
	GetLog()->FLogLine("TNullScreenManager::BacklightChanged(%d)", (int) backlight);
	dirty_rect_mutex.lock();
	mDirtyRect = { 0, 0, static_cast<int>(GetScreenWidth()), static_cast<int>(GetScreenHeight()) };
	mIsDirty = true;
	dirty_rect_mutex.unlock();
}

// -------------------------------------------------------------------------- //
//  * ContrastChanged( KUInt32 )
// -------------------------------------------------------------------------- //
void
TSDLScreenManager::ContrastChanged(KUInt32 contrast)
{
	GetLog()->FLogLine("TNullScreenManager::ContrastChanged(%u)", (unsigned int) contrast);
}

// -------------------------------------------------------------------------- //
//  * ScreenOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TSDLScreenManager::ScreenOrientationChanged(EOrientation inNewOrientation)
{
	GetLog()->FLogLine("TNullScreenManager::ScreenOrientationChanged(%u)", (unsigned int) inNewOrientation);
}

// -------------------------------------------------------------------------- //
//  * TabletOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TSDLScreenManager::TabletOrientationChanged(EOrientation inNewOrientation)
{
	GetLog()->FLogLine("TNullScreenManager::TabletOrientationChanged(%u)", (unsigned int) inNewOrientation);
}

// -------------------------------------------------------------------------- //
//  * UpdateScreenRect( SRect* )
// -------------------------------------------------------------------------- //
void
TSDLScreenManager::UpdateScreenRect(SRect* inUpdateRect)
{
	int w = inUpdateRect->fRight - inUpdateRect->fLeft;
	if (w <= 0) return;
	int h = inUpdateRect->fBottom - inUpdateRect->fTop;
	if (h <= 0) return;
	int x = inUpdateRect->fLeft;
	int y = inUpdateRect->fTop;

	dirty_rect_mutex.lock();
	if (mIsDirty) {
		SDL_Rect r = { x, y, w, h };
		SDL_GetRectUnion(&mDirtyRect, &r, &mDirtyRect);
	} else {
		mDirtyRect = { x, y, w, h };
		mIsDirty = true;
	}
	dirty_rect_mutex.unlock();
}

static bool rgb_lut_initialized = false;
static uint32_t rgb_lut_gray[16] = { };
static uint32_t rgb_lut_green[16] = { };

bool
TSDLScreenManager::UpdateTexture(SDL_Texture *texture, int encoding)
{
	if (!rgb_lut_initialized) {
		for (int i=0; i<16; ++i) {
			uint32_t f = i | (i<<4);
			uint32_t g = f*2/3;
			uint32_t h = f;
			switch (encoding) {
				case 0: case 1: // ARGB
					rgb_lut_gray[i]  = 0xff000000 | f << 16 | f << 8 | f;
					rgb_lut_green[i] = 0xff000000 | g << 16 | h << 8 | g;
					break;
				case 2: case 3: // ABGR
					rgb_lut_gray[i]  = 0xff000000 | f << 16 | f << 8 | f;
					rgb_lut_green[i] = 0xff000000 | g << 16 | h << 8 | g;
					break;
				case 4: case 5: // RGBA
					rgb_lut_gray[i]  = 0x000000ff | f << 24 | f << 16 | f << 8;
					rgb_lut_green[i] = 0x000000ff | g << 24 | h << 16 | g << 8;
					break;
				default: // BGRA
					rgb_lut_gray[i]  = 0x000000ff | f << 24 | f << 16 | f << 8;
					rgb_lut_green[i] = 0x000000ff | g << 24 | h << 16 | g << 8;
					break;
			}
			GetLog()->FLogLine("RGB %d = 0x%08x 0x%08x", i, rgb_lut_gray[i], rgb_lut_green[i]);
		}
		rgb_lut_initialized = true;
	}

	if (!mIsDirty) {
		return false;
	}

	uint32_t *rgb_lut = GetBacklight() ? rgb_lut_green : rgb_lut_gray;

	dirty_rect_mutex.lock();
	int dl = mDirtyRect.x & ~1; // round down to an even number
	int dr = (mDirtyRect.x + mDirtyRect.w + 1) & ~1; // round up to an even number
	SDL_Rect dirty = { dl, mDirtyRect.y, dr-dl, mDirtyRect.h };
	dl /= 2; dr /= 2;
	mIsDirty = false;
	dirty_rect_mutex.unlock();

	const int ww = 320; // FIXME: NO!
	const int hh = 480; // FIXME: NO!

#if USE_SDL_LOCK_TEXTURE

	(void)hh;
	uint32_t *buf = nullptr;
	int pitch = 0;
	SDL_LockTexture(texture, &dirty, (void**)&buf, &pitch);

	uint8_t *src = GetScreenBuffer();
	int src_pitch = ww/2;
	int dst_pitch = pitch / sizeof(uint32_t);;

	int dt = dirty.y, db = dirty.y + dirty.h;
	for (int y=dt; y<db; y++) {
		uint8_t *s = src + y * src_pitch + dl;
		uint32_t *d = buf + (y-dt) * dst_pitch;
		for (int x=dl; x<dr; x++) {
			uint8_t pp = *s++;
			uint8_t lt = (pp >> 4);
			*d++ = rgb_lut[lt];
			uint8_t rt = (pp & 0x0f);
			*d++ = rgb_lut[rt];
		}
	}
	SDL_UnlockTexture(texture);

#else

	static uint32_t buf[ww*hh]; // FIXME: allocate when needed at the correct size

	uint8_t *src = GetScreenBuffer();
	int src_pitch = ww/2;
	int dst_pitch = ww;

	int dt = dirty.y, db = dirty.y + dirty.h;
	for (int y=dt; y<db; y++) {
		uint8_t *s = src + y * src_pitch + dl;
		uint32_t *d = buf + y * dst_pitch + 2*dl;
		for (int x=dl; x<dr; x++) {
			uint8_t pp = *s++;
			uint8_t lt = (pp >> 4);
			*d++ = rgb_lut[lt];
			uint8_t rt = (pp & 0x0f);
			*d++ = rgb_lut[rt];
		}
	}
	SDL_UpdateTexture(texture,
					  &dirty,
					  buf + dt * dst_pitch + 2*dl,
					  ww*4); //TScreenManager::kDefaultPortraitWidth*4);

#endif

	return true;
}


// ========================================================================= //
// ... Any resemblance between the above views and those of my employer,     //
// my terminal, or the view out my window are purely coincidental.  Any      //
// resemblance between the above and my own views is non-deterministic.  The //
// question of the existence of views in the absence of anyone to hold them  //
// is left as an exercise for the reader.  The question of the existence of  //
// the reader is left as an exercise for the second god coefficient.  (A     //
// discussion of non-orthogonal, non-integral polytheism is beyond the scope //
// of this article.)                                                         //
// ========================================================================= //
