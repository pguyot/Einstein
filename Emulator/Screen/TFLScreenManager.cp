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

static const struct {unsigned short vk, fltk;} vktab[] = {
  { 49, ' ' }, { 39, '\'' }, { 43, ',' }, { 27, '-' }, { 47, '.' }, { 44, '/' }, 
  { 29, '0' }, { 18, '1'  }, { 19, '2'  }, { 20, '3'  }, 
  { 21, '4' }, { 23, '5'  }, { 22, '6'  }, { 26, '7'  }, 
  { 28, '8' }, { 25, '9'  }, { 41, ';'  }, { 24, '='  },
  {  0, 'A' }, { 11, 'B'  }, {  8, 'C'  }, {  2, 'D'  }, 
  { 14, 'E' }, {  3, 'F'  }, {  5, 'G'  }, {  4, 'H'  }, 
  { 34, 'I' }, { 38, 'J'  }, { 40, 'K'  }, { 37, 'L'  }, 
  { 46, 'M' }, { 45, 'N'  }, { 31, 'O'  }, { 35, 'P'  }, 
  { 12, 'Q' }, { 15, 'R'  }, {  1, 'S'  }, { 17, 'T'  }, 
  { 32, 'U' }, {  9, 'V'  }, { 13, 'W'  }, {  7, 'X'  }, 
  { 16, 'Y' }, {  6, 'Z'  }, 
  { 33, '[' }, { 30, ']' }, { 50, '`' },  { 42, '|' },
  { 51, FL_BackSpace }, { 48, FL_Tab }, { 36, FL_Enter }, { 127, FL_Pause },
  { 107, FL_Scroll_Lock }, { 53, FL_Escape }, { 0x73, FL_Home }, { 123, FL_Left },
  { 126, FL_Up }, { 124, FL_Right }, { 125, FL_Down }, { 0x74, FL_Page_Up },
  { 0x79, FL_Page_Down },  { 119, FL_End }, { 0x71, FL_Print }, { 127, FL_Insert },
  { 0x6e, FL_Menu }, { 114, FL_Help }, { 0x47, FL_Num_Lock }, 
  { 76, FL_KP_Enter }, { 67, FL_KP+'*' }, { 69, FL_KP+'+'}, { 78, FL_KP+'-' }, { 65, FL_KP+'.' }, { 75, FL_KP+'/' }, 
  { 82, FL_KP+'0' }, { 83, FL_KP+'1' }, { 84, FL_KP+'2' }, { 85, FL_KP+'3' }, 
  { 86, FL_KP+'4' }, { 87, FL_KP+'5' }, { 88, FL_KP+'6' }, { 89, FL_KP+'7' }, 
  { 91, FL_KP+'8' }, { 92, FL_KP+'9' }, { 81, FL_KP+'=' }, 
  { 0x7a, FL_F+1 }, { 0x78, FL_F+2  }, { 0x63, FL_F+3  }, { 0x76, FL_F+4  }, 
  { 0x60, FL_F+5 }, { 0x61, FL_F+6  }, { 0x62, FL_F+7  }, { 0x64, FL_F+8  }, 
  { 0x65, FL_F+9 }, { 0x6D, FL_F+10 }, { 0x67, FL_F+11 }, { 0x6f, FL_F+12 }, 
  { 56, FL_Shift_L }, { 56, FL_Shift_R }, { 59, FL_Control_L }, { 59, FL_Control_R }, 
  { 57, FL_Caps_Lock }, { 55, FL_Meta_L }, { 55, FL_Meta_R },
  { 58, FL_Alt_L }, { 58, FL_Alt_R }, { 0x75, FL_Delete },
};


///
/// This class generates an FLTK widget which is then automatically added to 
/// the active group widget.
///
class Fl_Newton_Screen_Widget : public Fl_Box
{
	unsigned char		*rgbData_;
	TFLScreenManager	*screenManager_;
	int					rgbWidth_, rgbHeight_;
	int					penX, penY, penIsDown;

public:
	Fl_Newton_Screen_Widget(int x, int y, int w, int h, const char *l, TFLScreenManager *s) 
		: Fl_Box(x, y, w, h, l),
		rgbData_(0L),
		screenManager_(s),
		penX(0), penY(0), penIsDown(0)
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
		fl_draw_image(rgbData_, x(), y(), rgbWidth_, rgbHeight_);
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
		penX = mx;
		return mx;
	}

	int penYPos()
	{
		int my = Fl::event_y()-y();
		if (my<0) my = 0;
		if (my>=rgbHeight_) my = rgbHeight_-1;
		penY = my;
		return my;
	}

	unsigned int eventKeyToMac() {
		unsigned int fltk = Fl::event_key();
		if (fltk<256 && isalpha(fltk))
			fltk = toupper(fltk);
		int a = 0;
		int b = sizeof(vktab)/sizeof(*vktab);
		while (a < b) {
			int c = (a+b)/2;
			if (vktab[c].fltk == fltk) {
				return vktab[c].vk;
			}
			if (vktab[c].fltk < fltk) a = c+1; else b = c;
		}
		return 127;
	}

	void penDownTimer()
	{
		if (penIsDown) {
			screenManager_->PenDown(penXPos(), penYPos());
			Fl::repeat_timeout(screenManager_->GetTabletSampleRate()/4000000.0, penDownTimerCB, this);
		} else {
			Fl::remove_timeout(penDownTimerCB, this);
		}
	}

	static void penDownTimerCB(void *me) {
		((Fl_Newton_Screen_Widget*)me)->penDownTimer();
	}

	int handle(int event) 
	{
		switch (event) {
			case FL_PUSH:
				screenManager_->PenDown(penXPos(), penYPos());
				penIsDown = true;
				Fl::add_timeout(screenManager_->GetTabletSampleRate()/4000000.0, penDownTimerCB, this);
				return 1;
			case FL_DRAG:
				screenManager_->PenDown(penXPos(), penYPos());
				return 1;
			case FL_RELEASE:
				screenManager_->PenUp();
				Fl::remove_timeout(penDownTimerCB, this);
				penIsDown = false;
				return 1;
			case FL_KEYDOWN:
				screenManager_->KeyDown(eventKeyToMac());
				return 1;
			case FL_KEYUP:
				screenManager_->KeyUp(eventKeyToMac());
				return 1;
			case FL_FOCUS:
				return 1;
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
	
	mWidget->label(
		"booting...\n"
		"\n"
		"right-click or\n"
		"ctrl-left-click\n"
		"to open the\n"
		"Einstein menu");
	mWidget->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	mWidget->labelfont(FL_HELVETICA_BOLD);
	mWidget->labelsize(11);
	mWidget->labelcolor(FL_WHITE);

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
	UpdateScreenRect(0L);
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
TFLScreenManager::ScreenOrientationChanged( EOrientation inNewOrientation )
{
	printf("New orientation %d is %dx%d\n", inNewOrientation, GetScreenWidth(), GetScreenHeight());
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
	static bool firstTime = true;
	if (firstTime) {
		mWidget->label("");
		firstTime = false;
	}
	int mBitsPerPixel = 24;

	KUInt16 top, left, height, width;
	if (inUpdateRect) {
		top = inUpdateRect->fTop;
		left = inUpdateRect->fLeft;
		height = inUpdateRect->fBottom - top;
		width = inUpdateRect->fRight - left;
	} else {
		top = 0; 
		left = 0;
		height = GetScreenHeight();
		width = GetScreenWidth();
	}

	KUInt8 rs, gs, bs; 
	if (GetBacklight()) {
		rs = 1; gs = 0; bs = 1;
	} else {
		rs = 0; gs = 0; bs = 0;
	}

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
			*dstCursor++ = thePixel>>rs;
			*dstCursor++ = thePixel>>gs;
			*dstCursor++ = thePixel>>bs;
			// Second pixel
			thePixel = (theByte<<4) | (theByte & 0x0f);
			*dstCursor++ = thePixel>>rs;
			*dstCursor++ = thePixel>>gs;
			*dstCursor++ = thePixel>>bs;
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
