// ==============================
// File:			TFLAppWindow.cpp
// Project:			Einstein
//
// Copyright 2020 by Matthias Melcher.
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
TFLAppWindow::TFLAppWindow(int ww, int hh, const char *ll)
:	Fl_Window(ww, hh, ll)
{
}


/**
 This is the window containing the emulator screen and possibly a menubar, a toolbar, and a status bar
 */
TFLAppWindow::TFLAppWindow(int xx, int yy, int ww, int hh, const char *ll)
:	Fl_Window(xx, yy, ww, hh, ll)
{
}


/**
 A customized event handler.
 */
int TFLAppWindow::handle(int event)
{
    switch (event) {
        case FL_PUSH:
            if (   (Fl::event_button()==3)
                || ((Fl::event_state()&(FL_SHIFT|FL_CTRL|FL_ALT|FL_META))==FL_CTRL) )
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
    return Fl_Window::handle(event);
}


/**
 For devices with pen input, we want to be able to hide the mouse pointer.
 */
void TFLAppWindow::HideMousePointer()
{
    mMouseHidden = true;
    fl_cursor(FL_CURSOR_NONE);
}


/**
 For devices with pen input, we want to be able to hide the mouse pointer.
 */
void TFLAppWindow::ShowMousePointer()
{
    mMouseHidden = true;
    fl_cursor(FL_CURSOR_DEFAULT);
}


