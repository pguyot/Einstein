// ==============================
// File:			TFLAppWindow.h
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

#ifndef T_FL_APP_WINDOW_H
#define T_FL_APP_WINDOW_H

#include <K/Defines/KDefinitions.h>

#include <FL/Fl_Window.H>


class TFLApp;


/**
 This is the the window that contain the Newton main screen.
 */
class TFLAppWindow : public Fl_Window
{
public:
    // constructor positioning the window through the window manager
    TFLAppWindow(int ww, int hh, const char *ll=nullptr);

    // constructor positioning the window at a given location
    TFLAppWindow(int xx, int yy, int ww, int hh, const char *ll=nullptr);

    // link this window to the app
    void SetApp(TFLApp *inApp) { mApp = inApp; }

    // call this to hide the mouse pointer whenever it is inside the main window
    void HideMousePointer();

    // call this to show the mouse pointer
    void ShowMousePointer();

    // custom event handler for some extra events
    int handle(int event) override;

private:
    TFLApp      *mApp = nullptr;
    Boolean        mMouseHidden = false;
};


#endif // T_FL_APP_WINDOW_H

