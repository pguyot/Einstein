// ==============================
// File:			TFLScreenManager.cp
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
#include "TFLScreenManager.h"

// FLTK interface
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/fl_draw.h>

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>

// K
#include <K/Defines/UByteSex.h>
#include <K/Threads/TThread.h>

// Einstein
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
// FLTK Class
// -------------------------------------------------------------------------- //

///
/// This class generates an FLTK widget which is then automatically added to 
/// the active group widget.
///
class Fl_Newton_Screen_Widget : public Fl_Box
{
	unsigned char		*rgbData_;
	TFLScreenManager	*screenManager_;
	int					rgbWidth_, rgbHeight_;

public:
	Fl_Newton_Screen_Widget(int x, int y, int w, int h, const char *l, TFLScreenManager *s) 
		: Fl_Box(x, y, w, h, l),
		rgbData_(0L),
		screenManager_(s)
	{
		rgbWidth_ = w;
		rgbHeight_ = h;
		rgbData_ = (unsigned char*)calloc(w*h, 3);
	}

	Fl_Newton_Screen_Widget::~Fl_Newton_Screen_Widget()
	{
		screenManager_->unlinkWidget();
	}

	int getRGBWidth()
	{
		return rgbWidth_;
	}

	int getRGBHeight()
	{
		return rgbHeight_;
	}

	void draw() 
	{
		// FIXME draw borders if the widget is larger than our bitmap
		// FIXME enable clipping if the widget is smaller
		// FIXME center the bitmap if it is smaller
		fl_draw_image_mono(rgbData_, x(), y(), rgbWidth_, rgbHeight_);
		draw_label();
	}

	unsigned char *getRGBData() 
	{
		return rgbData_;
	}

	void redraw()
	{
		// thread safe version of redraw
		Fl::lock();
		Fl_Box::redraw();
		Fl::unlock();
		Fl::awake();
	}

	void unlinkSelf()
	{
		screenManager_ = 0L;
		Fl::lock();
		if (parent()) {
			parent()->remove(this);
			parent()->redraw();
		}
		Fl::delete_widget(this);
		Fl::unlock();
		Fl::awake();
	}

	int penXPos()
	{
		int mx = Fl::event_x()-x();
		if (mx<0) mx = 0;
		if (mx>=rgbWidth_) mx = rgbWidth_-1;
		return mx;
	}

	int penYPos()
	{
		int my = Fl::event_y()-y();
		if (my<0) my = 0;
		if (my>=rgbHeight_) my = rgbHeight_-1;
		return my;
	}

	int handle(int event) 
	{
		switch (event) {
			case FL_PUSH:
				screenManager_->PenDown(penXPos(), penYPos());
				return 1;
			case FL_DRAG:
				screenManager_->PenDown(penXPos(), penYPos());
				return 1;
			case FL_RELEASE:
				screenManager_->PenUp();
				return 1;
				/*
				// FIXME convert X11 to Mac keycodes
			case FL_KEYDOWN:
				printf("Key %d down \n", Fl::event_key());
				screen->KeyDown(Fl::event_key());
				return 1;
			case FL_KEYUP:
				printf("Key %d up \n", Fl::event_key());
				screen->KeyUp(Fl::event_key());
				return 1;
				*/
		}
		return Fl_Box::handle(event);
	}

	void newRGBSize(int w, int h)
	{
		// FIXME bad interface!
		if (w*h != rgbWidth_*rgbHeight_) {
			free(rgbData_);
			rgbData_ = (unsigned char*)calloc(w*h, 3);
		} else {
			memset(rgbData_, 0, w*h*3);
		}
		rgbWidth_ = w;
		rgbHeight_ = h;
		window()->size(w, h);
		size(w, h);
	}

};


// -------------------------------------------------------------------------- //
//  * GetDisplaySize( void )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::GetScreenSize(
			KUInt32* outWidth,
			KUInt32* outHeight)
{
	*outWidth = Fl::w()-Fl::x();
	*outHeight = Fl::h()-Fl::y();
}


// -------------------------------------------------------------------------- //
//  * TFLScreenManager( TLog* )
// -------------------------------------------------------------------------- //
TFLScreenManager::TFLScreenManager(
			TLog* inLog /* = nil */,
			KUInt32 inPortraitWidth /* = kDefaultPortraitWidth */,
			KUInt32 inPortraitHeight /* = kDefaultPortraitHeight */,
			Boolean inFullScreen /* = false */,
			Boolean inScreenIsLandscape /* = true */)
	:
		TScreenManager(
			inLog,
			inPortraitWidth,
			inPortraitHeight,
			inFullScreen,
			inScreenIsLandscape ),
		mWidget(0L)
{
	bool createWindow = (Fl_Group::current()==0L);

	if (createWindow) {
		new Fl_Window(inPortraitWidth, inPortraitHeight, "Einstein");
	}
	Fl_Group *parent = Fl_Group::current();

	int xo = 0, yo = 0;
	if (parent->type()<FL_WINDOW) {
		xo = parent->x();
		yo = parent->y();
	}

	mWidget = new Fl_Newton_Screen_Widget(
		xo, yo, inPortraitWidth, inPortraitHeight, 
		0L, this);

	if (createWindow) {
		Fl::get_system_colors();
		((Fl_Window*)parent)->show();
	}
}

// -------------------------------------------------------------------------- //
//  * ~TX11ScreenManager( void )
// -------------------------------------------------------------------------- //
TFLScreenManager::~TFLScreenManager( void )
{
	//if (mWidget)
	//	mWidget->unlinkSelf();
}

// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::PowerOn( void )
{
	// This space for rent.
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::PowerOff( void )
{
	// This space for rent.
}

// -------------------------------------------------------------------------- //
//  * PowerOnScreen( void )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::PowerOnScreen( void )
{
	printf("Power on Screen\n");
}


// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::PowerOffScreen( void )
{
	mWidget->window()->hide();
	printf("Power off Screen\n");
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( Boolean )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::BacklightChanged( Boolean )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * ContrastChanged( KUInt32 )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::ContrastChanged( KUInt32 )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * ScreenOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::ScreenOrientationChanged( EOrientation /* inNewOrientation */ )
{
	mWidget->newRGBSize(GetScreenWidth(), GetScreenHeight());
}

// -------------------------------------------------------------------------- //
//  * TabletOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::TabletOrientationChanged( EOrientation )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * UpdateScreenRect( SRect* )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::UpdateScreenRect( SRect* inUpdateRect )
{
	int mBitsPerPixel = 8;

	KUInt16 top = inUpdateRect->fTop;
	KUInt16 left = inUpdateRect->fLeft;
	KUInt16 height = inUpdateRect->fBottom - top;
	KUInt16 width = inUpdateRect->fRight - left;
	
	// Update the buffer.
	// We copy more pixels than what we should.
	if (left & 0x1)
	{
		left -= 1;
		width += 1;
	}
	if (width & 0x1)
	{
		width += 1;
	}
	
	KUInt8* theScreenBuffer = GetScreenBuffer();
	KUInt32 theScreenWidth = GetScreenWidth();
	KUInt32 dstRowBytes = theScreenWidth * mBitsPerPixel / 8;
	KUInt32 srcRowBytes = theScreenWidth * kBitsPerPixel / 8;
	KUInt32 srcWidthInBytes = width * kBitsPerPixel / 8;

	KUInt8* srcRowPtr =
		theScreenBuffer
		+ (top * srcRowBytes)
		+ (left * kBitsPerPixel / 8);
	KUInt8* dstRowPtr =
		((KUInt8*)mWidget->getRGBData())
		+ (top * dstRowBytes)
		+ (left * mBitsPerPixel / 8);

	int indexRows;

	for (indexRows = height; indexRows != 0; indexRows--)
	{
		KUInt8* srcCursor = srcRowPtr;
		KUInt8* srcEnd = srcRowPtr + srcWidthInBytes;
		KUInt8* dstCursor = dstRowPtr;
		do {
			KUInt8 theByte = *srcCursor++;
			// First pixel
			KUInt8 thePixel = (theByte & 0xF0) | (theByte>>4);
			*dstCursor++ = thePixel;
			// Second pixel
			thePixel = (theByte<<4) | (theByte & 0x0f);
			*dstCursor++ = thePixel;
		} while (srcCursor < srcEnd);
		srcRowPtr += srcRowBytes;
		dstRowPtr += dstRowBytes;
	}

	mWidget->redraw();
}


// ========================================================================= //
// The most likely way for the world to be destroyed, most experts agree, is 
// by accident. That's where we come in; we're computer professionals. We 
// cause accidents.
// ========================================================================= //
