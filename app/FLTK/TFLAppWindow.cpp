// ==============================
// File:			TFLAppWindow.cpp
// Project:			Einstein
//
// Copyright 2022 by Matthias Melcher.
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

#include "TFLAppWindow.h"
#include "TFLApp.h"

#include <FL/Fl.H>
#include <FL/fl_draw.H>

/**
 This is the window containing the emulator screen and possibly a menubar, a toolbar, and a status bar
 */
TFLAppWindow::TFLAppWindow(int ww, int hh, const char* ll) :
		super(ww, hh, ll)
{
}

/**
 This is the window containing the emulator screen and possibly a menubar, a toolbar, and a status bar
 */
TFLAppWindow::TFLAppWindow(int xx, int yy, int ww, int hh, const char* ll) :
		super(xx, yy, ww, hh, ll)
{
}

/**
 A customized event handler.
 */
int
TFLAppWindow::handle(int event)
{
	static bool draggingWindow = false;
	static int dx = 0, dy = 0;

	switch (event)
	{
		case FL_PUSH:
			if ((Fl::event_button() == 3)
				|| ((Fl::event_button() == 1) && ((Fl::event_state() & (FL_SHIFT | FL_CTRL | FL_ALT | FL_META)) == FL_CTRL)))
			{
				gApp->UserActionPopupMenu();
				return 1;
			}
			break;
		case FL_ENTER:
			if (mMouseHidden)
				fl_cursor(FL_CURSOR_NONE);
			break;
		case FL_LEAVE:
			fl_cursor(FL_CURSOR_DEFAULT);
			break;
	}
	int ret = super::handle(event);
	if (ret)
		return ret;
	switch (event)
	{
		case FL_PUSH:
			if (mDraggable
				&& (Fl::event_button() == 1)
				&& ((Fl::event_state() & (FL_SHIFT | FL_CTRL | FL_ALT | FL_META)) == 0))
			{
				dx = Fl::event_x_root() - x();
				dy = Fl::event_y_root() - y();
				draggingWindow = true;
				return 1;
			}
			break;
		case FL_DRAG:
		case FL_RELEASE:
			if (draggingWindow)
			{
				position(Fl::event_x_root() - dx, Fl::event_y_root() - dy);
				if (event == FL_RELEASE)
					draggingWindow = false;
			}
			break;
	}
	return 0;
}

/**
 For devices with pen input, we want to be able to hide the mouse pointer.
 */
void
TFLAppWindow::HideMousePointer()
{
	mMouseHidden = true;
	fl_cursor(FL_CURSOR_NONE);
}

/**
 For devices with pen input, we want to be able to hide the mouse pointer.
 */
void
TFLAppWindow::ShowMousePointer()
{
	mMouseHidden = true;
	fl_cursor(FL_CURSOR_DEFAULT);
}

/**
 Make sure that we can draw a backdrop image if one is set.
 */
void
TFLAppWindow::show()
{
#if 1
	// Get around an issue in FLTK that does not allow backgroud images on
	// top level windows, just in case there is a scheme with a
	// background window.
	Fl_Image* bgImage = image();
	super::show();
	if (bgImage)
	{
		image(bgImage);
		labeltype(FL_NORMAL_LABEL);
		align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
	}
#else
	// Alternative kludge by temporarily setting a backgroud, however the old
	// scheme_bg_ should be restored.
	Fl_Image* bgImage = image();
	if (bgImage)
		Fl::scheme_bg_ = bgImage;
	super::show();
	Fl::scheme_bg_ = nullptr;
#endif
}
