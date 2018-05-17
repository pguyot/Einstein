// ==============================
// File:			TCocoaUserDefaults.h
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

#ifndef _TCOCOAUSERDEFAULTS_H
#define _TCOCOAUSERDEFAULTS_H

#define kROMImagePathKey			@"ROMImagePath"
#define kROMREx0PathKey				@"ROMREx0Path"
#define kInternalFlashPathKey		@"InternalFlashPath"
#define kScreenWidthKey				@"ScreenWidth"
#define kScreenHeightKey			@"ScreenHeight"
#define kFullScreenKey				@"FullScreen"
#define kRAMSizeKey					@"RAMSize"

#define kMachineKey					@"Machine"
#define k717006Tag					717006
#define k737041Tag					737041
#define k747129Tag					747129

#define kAudioDriverKey				@"AudioDriver"
#define kCoreAudioDriverTag			1
#define kPortAudioDriverTag			2
#define kNullSoundDriverTag			3

#define kScreenDriverKey			@"ScreenDriver"
#define kCocoaScreenDriverTag		1
#define kX11ScreenDriverTag			2

#define kNetworkDriverKey			@"NetworkDriver"
#define kUsermodeNetworkDriverTag	1
#define kTapNetworkDriverTag		2
#define kNullNetworkDriverTag		3

#define kSerialDriverKey			@"SerialDriver"
#define kBasiliskIISerialDriverTag	3
#define kPtySerialDriverTag			2
#define kPipesSerialDriverTag		1
#define kNullSerialDriverTag		0

#define kDontShowAtStartupKey		@"DontShowAtStartup"
#define kEnableListenersKey			@"EnableListeners"

#define kOpenMonitorAtLaunch		@"OpenMonitorAtLaunch"

#endif
		// _TCOCOAUSERDEFAULTS_H

// =============================================================== //
// Science is to computer science as hydrodynamics is to plumbing. //
// =============================================================== //
