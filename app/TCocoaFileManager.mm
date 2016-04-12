// ==============================
// File:			TCocoaFileManager.mm
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

#include "TCocoaFileManager.h"
#import <Foundation/Foundation.h>
#import "TCocoaListenerWindow.h"

// -------------------------------------------------------------------------- //
//  * ~TCocoaFileManager( void )
// -------------------------------------------------------------------------- //
TCocoaFileManager::~TCocoaFileManager( void )
{
	[mListenerWindows release], mListenerWindows = nil;
}

// -------------------------------------------------------------------------- //
//  * open_listener( const char *, KUInt32 );
// -------------------------------------------------------------------------- //
void
TCocoaFileManager::open_listener( const char *name, KUInt32 desc )
{
	if (mListenerWindows == nil) {
		mListenerWindows = [[NSMutableArray alloc] init];
	}
	
	__block TCocoaListenerWindow *window = nil;
	dispatch_sync(dispatch_get_main_queue(), ^{
		window = [[TCocoaListenerWindow alloc] init];
		NSString *title = [[NSString alloc] initWithCString:name encoding:NSASCIIStringEncoding];
		[window setTitle:title];
		[title release];

		[window setFileManager:this];
		[window setNewt_fdesc:desc];
		[window showWindow:nil];
	});

	@synchronized(mListenerWindows) {
		[mListenerWindows addObject:window];
	}
}

// -------------------------------------------------------------------------- //
//  * close_listener( KUInt32 );
// -------------------------------------------------------------------------- //
void
TCocoaFileManager::close_listener( KUInt32 desc )
{
	NSArray *listenerWindows = nil;
	@synchronized(mListenerWindows) {
		listenerWindows = [NSArray arrayWithArray:mListenerWindows];
	}
	
	for (TCocoaListenerWindow *aWindow in listenerWindows) {
		if (aWindow.newt_fdesc == desc) {
			[aWindow close];
		}
	}
}

// -------------------------------------------------------------------------- //
//  * listener_was_closed( KUInt32 );
// -------------------------------------------------------------------------- //
void
TCocoaFileManager::listener_was_closed( KUInt32 desc )
{
	NSArray *listenerWindows = nil;
	@synchronized(mListenerWindows) {
		listenerWindows = [NSArray arrayWithArray:mListenerWindows];
	}

	for (TCocoaListenerWindow *aWindow in listenerWindows) {
		if (aWindow.newt_fdesc == desc) {
			@synchronized(mListenerWindows) {
				[mListenerWindows removeObject:aWindow];
			}
		}
	}
}


// -------------------------------------------------------------------------- //
//  * write_listener( KUInt32, const char *, KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TCocoaFileManager::write_listener( KUInt32 desc, const void *buf, KUInt32 nbytes )
{
	NSArray *listenerWindows = nil;
	@synchronized(mListenerWindows) {
		listenerWindows = [NSArray arrayWithArray:mListenerWindows];
	}
	
	for (TCocoaListenerWindow *window in listenerWindows) {
		if (window.newt_fdesc == desc) {
			NSString *buffer = [[NSString alloc] initWithBytes:buf length:nbytes encoding:NSASCIIStringEncoding];
			[window appendString:buffer];
			[buffer release];
			return nbytes;
		}
	}
	return -1;
}

// -------------------------------------------------------------------------- //
//  * read_listener( KUInt32, const char *, KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TCocoaFileManager::read_listener( KUInt32 desc, void *buf, KUInt32 nbytes )
{
	NSArray *listenerWindows = nil;
	@synchronized(mListenerWindows) {
		listenerWindows = [NSArray arrayWithArray:mListenerWindows];
	}
	
	for (TCocoaListenerWindow *window in listenerWindows) {
		if (window.newt_fdesc == desc) {
			return [window writeInputIntoBuffer:buf
									  maxLength:nbytes];
		}
	}
	return -1;
}