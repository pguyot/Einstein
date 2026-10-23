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

#include "TFLSpecialKeys.h"

#include <FL/Fl.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/fl_draw.H>

#include "Emulator/Screen/TScreenManager.h"
#include "app/FLTK/TFLApp.h"

// Not a key code: these keys call the app instead of NewtonOS.
static constexpr int kLightKey = -1;
static constexpr int kPowerKey = -2;

// SVG icons on a 24 x 24 grid, drawn in white.
#define SVG_HEAD "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 24'>" \
				 "<g fill='none' stroke='#fff' stroke-width='1.8' stroke-linecap='round' stroke-linejoin='round'>"
#define SVG_TAIL "</g></svg>"
#define SVG_DOT(x, y) "<circle cx='" #x "' cy='" #y "' r='1.1' fill='#fff' stroke='none'/>"

struct SSpecialKey {
	const char* fLabel;
	int fKeyCode; // Mac key code as the eMate keyboard sends it, or kLightKey, kPowerKey
	int fGroup; // keys of one group sit next to each other
	const char* fIcon;
};

static const SSpecialKey kSpecialKeys[] = {
	{ "close", 53, 0, // Esc
		SVG_HEAD "<rect x='4' y='4' width='16' height='16'/>"
				 "<path d='M8 8 L16 16 M16 8 L8 16'/>" SVG_TAIL },
	{ "extras", 0x7A, 0, // F1
		SVG_HEAD "<path d='M3 11 L6 6 H18 L21 11'/>"
				 "<rect x='3' y='11' width='18' height='8'/>"
				 "<path d='M10 15 H14'/>" SVG_TAIL },
	{ "dock", 0x78, 1, // F2
		SVG_HEAD "<rect x='12' y='4' width='9' height='8'/>"
				 "<path d='M10 19 H22 M13 15 H20 M2 8 H9 M6 5 L9 8 L6 11'/>" SVG_TAIL },
	{ "send", 0x63, 1, // F3
		SVG_HEAD SVG_DOT(4, 6) SVG_DOT(8, 5) SVG_DOT(12, 4.5) SVG_DOT(16, 5) SVG_DOT(20, 6)
			SVG_DOT(6, 10) SVG_DOT(10, 9.5) SVG_DOT(14, 9.5) SVG_DOT(18, 10)
				SVG_DOT(8, 14) SVG_DOT(12, 13.5) SVG_DOT(16, 14)
					SVG_DOT(10, 18) SVG_DOT(14, 18) SVG_TAIL },
	{ "receive", 0x76, 1, // F4
		SVG_HEAD SVG_DOT(12, 5)
			SVG_DOT(10, 9) SVG_DOT(14, 9)
				SVG_DOT(8, 13) SVG_DOT(12, 13) SVG_DOT(16, 13)
					SVG_DOT(6, 17) SVG_DOT(10, 17) SVG_DOT(14, 17) SVG_DOT(18, 17)
						SVG_DOT(4, 21) SVG_DOT(8, 21) SVG_DOT(12, 21) SVG_DOT(16, 21) SVG_DOT(20, 21) SVG_TAIL },
	{ "scroll", 0x60, 2, // F5, scroll up
		SVG_HEAD "<path d='M12 3 L20 12 H15.5 V20 H8.5 V12 H4 Z'/>" SVG_TAIL },
	{ "overview", 0x61, 2, // F6
		SVG_HEAD "<circle cx='12' cy='12' r='7.5'/>" SVG_TAIL },
	{ "scroll", 0x62, 2, // F7, scroll down
		SVG_HEAD "<path d='M12 21 L20 12 H15.5 V4 H8.5 V12 H4 Z'/>" SVG_TAIL },
	{ "undo", 0x64, 3, // F8
		SVG_HEAD "<path d='M9 4 C17 4 17 11 11 11 C5 11 6 19 14 19'/>"
				 "<path d='M9 4 L12 2 M9 4 L11.5 7'/>" SVG_TAIL },
	{ "find", 0x65, 3, // F9
		SVG_HEAD "<circle cx='13' cy='10' r='6'/>"
				 "<path d='M8.8 14.2 L3.5 19.5'/>" SVG_TAIL },
	{ "assist", 0x6D, 3, // F10
		SVG_HEAD "<path d='M9 15 C9 12 7 11 7 8.5 A5 5 0 0 1 17 8.5 C17 11 15 12 15 15 Z'/>"
				 "<path d='M10 18 H14 M10.5 21 H13.5 M3 8 H1.5 M21 8 H22.5 M4.5 3 L3.5 2 M19.5 3 L20.5 2'/>" SVG_TAIL },
	{ "light", kLightKey, 4,
		SVG_HEAD "<circle cx='12' cy='12' r='4'/>"
				 "<path d='M12 2 V5 M12 19 V22 M2 12 H5 M19 12 H22 M4.9 4.9 L7 7 M17 17 L19.1 19.1"
				 " M4.9 19.1 L7 17 M17 7 L19.1 4.9'/>" SVG_TAIL },
	{ "power", kPowerKey, 4,
		SVG_HEAD "<path d='M7.5 6.5 A7.5 7.5 0 1 0 16.5 6.5 M12 3 V12'/>" SVG_TAIL },
};

static constexpr int kNumSpecialKeys = (int) (sizeof(kSpecialKeys) / sizeof(kSpecialKeys[0]));
static constexpr int kNumGroups = 5;

// The width of the gap between two groups, and of the margin left and right,
// in key widths.
static constexpr double kGroupGap = 0.35;
static constexpr double kMargin = 0.15;

// The colors of the eMate: dark green keys on a darker green, white writing.
static const Fl_Color kKeyColor = fl_rgb_color(86, 122, 125);
static const Fl_Color kPressedKeyColor = fl_rgb_color(68, 97, 100);
static const Fl_Color kBarColor = fl_rgb_color(18, 44, 34);

TFLSpecialKeys::TFLSpecialKeys(int x, int y, int w, int h, TFLApp* inApp, TScreenManager* inScreenManager) :
		Fl_Widget(x, y, w, h),
		mApp(inApp),
		mScreenManager(inScreenManager)
{
	mIcons = new Fl_SVG_Image*[kNumSpecialKeys];
	for (int i = 0; i < kNumSpecialKeys; i++)
		mIcons[i] = new Fl_SVG_Image(nullptr, kSpecialKeys[i].fIcon);
	// Clicks must not take the keyboard focus away from the Newton screen.
	clear_visible_focus();
}

TFLSpecialKeys::~TFLSpecialKeys()
{
	for (int i = 0; i < kNumSpecialKeys; i++)
		delete mIcons[i];
	delete[] mIcons;
}

int
TFLSpecialKeys::PreferredHeight(int inScale)
{
	return (int) (40 * inScale / 100.0 + 0.5);
}

void
TFLSpecialKeys::GetKeyRect(int inIndex, int& outX, int& outY, int& outW, int& outH) const
{
	double unit = w() / (kNumSpecialKeys + (kNumGroups - 1) * kGroupGap + 2 * kMargin);
	double left = x() + unit * (kMargin + inIndex + kSpecialKeys[inIndex].fGroup * kGroupGap);
	int gap = (int) (unit * 0.08 + 0.5);
	if (gap < 1)
		gap = 1;
	int top = (int) (h() * 0.1 + 0.5);
	outX = (int) (left + 0.5) + gap / 2;
	outW = (int) (left + unit + 0.5) - (int) (left + 0.5) - gap;
	outY = y() + top;
	outH = h() - 2 * top;
}

int
TFLSpecialKeys::KeyAt(int inX, int inY) const
{
	for (int i = 0; i < kNumSpecialKeys; i++)
	{
		int kx, ky, kw, kh;
		GetKeyRect(i, kx, ky, kw, kh);
		if (inX >= kx && inX < kx + kw && inY >= ky && inY < ky + kh)
			return i;
	}
	return -1;
}

void
TFLSpecialKeys::draw()
{
	fl_rectf(x(), y(), w(), h(), kBarColor);

	// All labels get the same size, the largest at which every label fits.
	// Small keys only show the icon.
	int kx, ky, kw, kh;
	GetKeyRect(0, kx, ky, kw, kh);
	int labelSize = (int) (kh * 0.28);
	for (; labelSize >= 7; labelSize--)
	{
		fl_font(FL_HELVETICA, labelSize);
		bool fits = true;
		for (int i = 0; i < kNumSpecialKeys && fits; i++)
			fits = (fl_width(kSpecialKeys[i].fLabel) <= kw);
		if (fits)
			break;
	}
	bool showLabel = (labelSize >= 7);

	for (int i = 0; i < kNumSpecialKeys; i++)
	{
		GetKeyRect(i, kx, ky, kw, kh);
		if (i == mPressed)
			fl_draw_box(FL_GTK_THIN_DOWN_BOX, kx, ky, kw, kh, kPressedKeyColor);
		else
			fl_draw_box(FL_GTK_THIN_UP_BOX, kx, ky, kw, kh, kKeyColor);

		int iconSize = showLabel ? (int) (kh * 0.5) : (int) (kh * 0.7);
		if (iconSize > kw - 4)
			iconSize = kw - 4;
		int iconY = showLabel ? ky + (int) (kh * 0.08) : ky + (kh - iconSize) / 2;
		Fl_SVG_Image* icon = mIcons[i];
		icon->scale(iconSize, iconSize, 1, 1);
		icon->draw(kx + (kw - icon->w()) / 2, iconY);

		if (showLabel)
		{
			fl_color(FL_WHITE);
			fl_font(FL_HELVETICA, labelSize);
			fl_draw(kSpecialKeys[i].fLabel, kx, ky + kh - labelSize - (int) (kh * 0.08), kw, labelSize,
				FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
		}
	}
}

void
TFLSpecialKeys::Press(int inIndex)
{
	mPressed = inIndex;
	int keyCode = kSpecialKeys[inIndex].fKeyCode;
	if (keyCode == kLightKey)
		mApp->UserActionToggleBacklight();
	else if (keyCode == kPowerKey)
		mApp->UserActionTogglePower();
	else
		mScreenManager->KeyDown((KUInt8) keyCode);
	redraw();
}

void
TFLSpecialKeys::Release()
{
	if (mPressed < 0)
		return;
	int keyCode = kSpecialKeys[mPressed].fKeyCode;
	if (keyCode >= 0)
		mScreenManager->KeyUp((KUInt8) keyCode);
	mPressed = -1;
	redraw();
}

int
TFLSpecialKeys::handle(int event)
{
	switch (event)
	{
		case FL_PUSH: {
			int key = KeyAt(Fl::event_x(), Fl::event_y());
			if (key >= 0)
				Press(key);
			return 1;
		}
		case FL_DRAG:
			// Like a real key: moving off it lets it go.
			if (mPressed >= 0 && KeyAt(Fl::event_x(), Fl::event_y()) != mPressed)
				Release();
			return 1;
		case FL_RELEASE:
			Release();
			return 1;
		default:
			return Fl_Widget::handle(event);
	}
}
