// ==============================
// File:			NullScreenManagerTests.cpp
// Project:			Einstein
//
// Copyright 2026 by Paul Guyot and contributors.
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

#include "Emulator/Log/TBufferLog.h"
#include "Emulator/Screen/TNullScreenManager.h"
#include <gtest/gtest.h>

TEST(NullScreenManagerTests, SafeWithNullLog)
{
	TNullScreenManager screen(nullptr, 320, 480);

	EXPECT_EQ(screen.GetScreenWidth(), 320U);
	EXPECT_EQ(screen.GetScreenHeight(), 480U);
	EXPECT_NE(screen.GetScreenBuffer(), nullptr);

	screen.PowerOn();
	screen.PowerOnScreen();
	screen.BacklightChanged(true);
	screen.ContrastChanged(10);
	screen.ScreenOrientationChanged(TScreenManager::kOrientation_AppleRight);
	screen.TabletOrientationChanged(TScreenManager::kOrientation_Default);

	TScreenManager::SRect rect { 0, 0, 16, 16 };
	screen.UpdateScreenRect(&rect);

	screen.PowerOffScreen();
	screen.PowerOff();
}

TEST(NullScreenManagerTests, LogsEventsWhenLogProvided)
{
	TBufferLog log;
	TNullScreenManager screen(&log, 320, 480);

	screen.PowerOn();
	screen.BacklightChanged(true);
	screen.PowerOff();

	EXPECT_STREQ(log.GetLine(29), "TNullScreenManager::PowerOn()");
	EXPECT_STREQ(log.GetLine(30), "TNullScreenManager::BacklightChanged(1)");
	EXPECT_STREQ(log.GetLine(31), "TNullScreenManager::PowerOff()");
}
