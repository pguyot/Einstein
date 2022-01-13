// ==============================
// File:			TCocoaFileManager.h
// Project:			Einstein
//
// Copyright 2015 Steve White.
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

#ifndef _TCOCOAFILEMANAGER_H
#define _TCOCOAFILEMANAGER_H

#include <K/Defines/KDefinitions.h>
#include "TFileManager.h"

// CoreGraphics via ApplicationServices
#include <ApplicationServices/ApplicationServices.h>

// ObjC
#include <objc/objc.h>

class TCocoaFileManager
		: public TFileManager
{
public:
	///
	/// Destructor.
	///
	virtual ~TCocoaFileManager(void);

	///
	/// Invoked from the Newton when a listener window should be opened
	///
	void open_listener(const char* name, KUInt32 desc);

	///
	/// Invoked from the Newton when a listener window should be closed
	///
	void close_listener(KUInt32 desc);

	///
	/// Invoked from the Newton when there is data to be written to a
	/// listener
	///
	KSInt32 write_listener(KUInt32 desc, const void* buf, KUInt32 nbytes);

	///
	/// Invoked from the Newton when it wishes to read data from a
	/// listener
	///
	KSInt32 read_listener(KUInt32 desc, void* buf, KUInt32 nbytes);

	///
	/// Invoked by TCocoaListenerWindow when the Cocoa window is closed
	///
	void listener_was_closed(KUInt32 desc);

private:
	NSMutableArray* mListenerWindows;
};

#endif
// _TCOCOAFILEMANAGER_H
