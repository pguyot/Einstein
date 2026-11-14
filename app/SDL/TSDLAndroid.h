
// ==============================
// File:			TSDLAndroid.h
// Project:			Einstein
//
// Copyright 2025 by Matthias Melcher.
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

// =============================================================================
// NOTE: This file is no longer used, but kept for reference.
// =============================================================================

#ifndef _TSDLANDROID_H
#define _TSDLANDROID_H

#include "app/Version.h"

#include <K/Defines/KDefinitions.h>

#include <string>
#include <vector>

extern void AndroidPickROMFile();

/// Show a native list menu (a single choice list if inChecked >= 0). The
/// chosen index (or -1 for Cancel) arrives as an SDL user event of type
/// inEventType, with the index in user.code and inMenuId in user.data1.
/// Download a URL with Android's HTTP stack (https works); any thread.
/// Returns false and sets outError on failure.
extern bool AndroidHttpGet(const std::string& inURL, std::vector<unsigned char>& outData,
						   std::string& outError);

extern void AndroidShowMenu(int inMenuId, const char* inTitle,
							const std::vector<std::string>& inItems,
							int inChecked, unsigned int inEventType);
extern void AndroidPickPackageFile();

extern void AndroidDownloadROMFile();
extern void AndroidDownloadPackageFile();

#endif
// _TSDLANDROID_H

// ============================================================================ //
// “Nothing is as permanent as a temporary solution."                           //
// ============================================================================ //
