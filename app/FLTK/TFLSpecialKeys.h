// ==============================
// File:			TFLSpecialKeys
// Project:			Einstein
//
// Copyright 2026 by Matthias Melcher.
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

#ifndef T_FL_SPECIAL_KEYS_H
#define T_FL_SPECIAL_KEYS_H

#include <FL/Fl_Widget.H>

class Fl_SVG_Image;
class TFLApp;
class TScreenManager;

/**
 \brief The row of Special Keys above the keyboard of an eMate 300.

 Thirteen keys in five groups: close, extras | dock, send, receive | scroll
 up, overview, scroll down | undo, find, assist | light, power. Clicking a key
 sends the key code that the eMate keyboard sends (Esc, F1 to F10). Light and
 power are not keys for NewtonOS; they toggle the backlight and the power
 switch, like the toolbar buttons.

 The widget draws all keys itself and spreads them over its whole width, so
 it follows the window when the screen is resized or rotated.
 */
class TFLSpecialKeys : public Fl_Widget
{
public:
	TFLSpecialKeys(int x, int y, int w, int h, TFLApp* inApp, TScreenManager* inScreenManager);
	~TFLSpecialKeys() override;

	// Height of the bar at the given screen scale (in percent).
	static int PreferredHeight(int inScale);

protected:
	void draw() override;
	int handle(int event) override;

private:
	// Find the key at a position in the widget, -1 if there is none.
	int KeyAt(int inX, int inY) const;

	// Position and size of a key.
	void GetKeyRect(int inIndex, int& outX, int& outY, int& outW, int& outH) const;

	// Press or release a key, and tell NewtonOS.
	void Press(int inIndex);
	void Release();

	TFLApp* mApp { nullptr };
	TScreenManager* mScreenManager { nullptr };

	// The key that is held down, -1 if none.
	int mPressed { -1 };

	// One icon per key, white on transparent.
	Fl_SVG_Image** mIcons { nullptr };
};

#endif // T_FL_SPECIAL_KEYS_H
