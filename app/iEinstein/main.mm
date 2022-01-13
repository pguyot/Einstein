// ==============================
// File:			main.mm
// Project:			Einstein
//
// Copyright 2010 by Matthias Melcher (einstein@matthiasm.com).
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

#import <UIKit/UIKit.h>

int
main(int argc, char* argv[])
{
#if !__has_feature(objc_arc)
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
#endif
#ifdef USE_STORYBOARDS
	// For storyboards, set the app delegate here
	int retVal = UIApplicationMain(argc, argv, nil, @"iEinsteinAppDelegate");
#else
	// For NIBs, use the app delegate defined in the NIB
	int retVal = UIApplicationMain(argc, argv, nil, nil);
#endif
#if !__has_feature(objc_arc)
	[pool release];
#endif
	return retVal;
}

void
iOSDoLogLine(const char* inLine)
{
	NSLog(@"%@", [NSString stringWithUTF8String:inLine]);
}

// TODO: how to get the ROM from the user into the system:
//  - see section "Sharing Files with the User" in "iOS Application Programming Guide"
//  - and http://www.raywenderlich.com/1948/how-integrate-itunes-file-sharing-with-your-ios-app
// TODO: screen rotation
// ...
