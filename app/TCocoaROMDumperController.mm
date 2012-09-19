// ==============================
// File:			TCocoaROMDumperController.mm
// Project:			Einstein
//
// Copyright 2004-2007 by Paul Guyot (pguyot@kallisys.net).
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

#include <K/Defines/KDefinitions.h>
#import "TCocoaAppController.h"

// ANSI C & POSIX
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>

// Einstein
#import "TCocoaUserDefaults.h"
#import "TCocoaSetupController.h"

@implementation TCocoaROMDumperController

// -------------------------------------------------------------------------- //
//  * init
// -------------------------------------------------------------------------- //
- (id)init
{
#ifdef __MWERKS__
	#pragma unused ( _cmd )
#endif
	if ((self = [super init]) != NULL)
	{
		mIP = 0;
		mIPSet = kIPSetNone;
		mSocket = 0;
		mSocketPair[0] = 0;
		mSocketPair[1] = 0;
		mRunning = NO;
		mROMFileFd = 0;
		mROMFilePath = nil;
		mROMSize = 0;

		// RŽcupŽration de la boucle courante.
		mRunLoop = [NSRunLoop currentRunLoop];
	}
	
	return self;
}

// -------------------------------------------------------------------------- //
//  * dealloc
// -------------------------------------------------------------------------- //
- (void) dealloc
{
	if (mROMFilePath)
	{
		[mROMFilePath release];
	}
	
	[super dealloc];
}

// -------------------------------------------------------------------------- //
//  * (IBAction) startStopAction:(id)
// -------------------------------------------------------------------------- //
- (IBAction) startStopAction:(id)sender
{
	if (mRunning)
	{
		// Stop.
		[self setRunning: NO];
		
		// Write a byte in the socket pair.
		if (mSocketPair[0] > 0)
		{
			char someByte = 0;
			(void) write( mSocketPair[0], &someByte, sizeof(someByte) );
		}
	} else {
		// Start.
		[self setRunning: YES];

		// Ask the user where they want to save the ROM dump file.
		NSSavePanel* thePanel = [NSSavePanel savePanel];
		
		// If the user already tried to save the ROM, use this old path.
		NSString* theFilePath;
		if (mROMFilePath)
		{
			theFilePath = mROMFilePath;
		} else {
			theFilePath =
				[[mUserDefaultsController values]
					valueForKey: kROMImagePathKey];
		}
		
		
		NSString* theDir = [theFilePath stringByDeletingLastPathComponent];
		NSString* theName = [theFilePath lastPathComponent];
		
		[thePanel setNameFieldStringValue:theName];
		[thePanel setDirectoryURL:[NSURL fileURLWithPath:theDir]];
		
		[thePanel beginSheetModalForWindow:mDumpROMPanel completionHandler:^(NSInteger result) {
			do {
				if (result != NSOKButton)
				{
					[self setRunning: NO];
					break;
				}

				if (mROMFilePath)
				{
					[mROMFilePath release];
				}
				mROMFilePath = [[thePanel URL] path];
				[mROMFilePath retain];
				
				[self addDeferredSend: @selector(performDump)];
			} while (false);
		}];
		
	}
}

// -------------------------------------------------------------------------- //
//  * (void) performDump
// -------------------------------------------------------------------------- //
- (void) performDump
{
	do {
		// Try to open the file for writing.
		const char* theDumpFilePathCStr = [mROMFilePath UTF8String];
		mROMFileFd =
			open( theDumpFilePathCStr, O_WRONLY | O_CREAT | O_TRUNC, 0644 );
		
		if (mROMFileFd < 0)
		{
			[self errorMessage: @"Couldn't open ROM File"
				withInformativeText:
					[NSString stringWithFormat:
						@"The OS returned the following error: %i", errno]];
			[self cleanUp];
			break;
		}
	
		// Good. Create the socket.
		mSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (mSocket < 0)
		{
			[self errorMessage: @"Couldn't open socket"
				withInformativeText:
					[NSString stringWithFormat:
						@"The OS returned the following error: %i", errno]];
			[self cleanUp];
			break;
		}
	
		// Bind the socket.
        struct sockaddr_in name;
        bzero(&name, sizeof(name));
        name.sin_len = sizeof(name);
        name.sin_family = AF_INET;
        name.sin_port = 0;
        name.sin_addr.s_addr = INADDR_ANY;
        if (bind(mSocket, (const struct sockaddr*) &name, sizeof(name)) < 0)
		{
			[self errorMessage: @"Couldn't bind socket"
				withInformativeText:
					[NSString stringWithFormat:
						@"The OS returned the following error: %i", errno]];
			[self cleanUp];
			break;
		}
		
		// Connect to the Newton.
		name.sin_addr.s_addr = htonl(mIP);
		name.sin_port = htons(kROMDumperPort);
		if (connect(mSocket, (const struct sockaddr*) &name, sizeof(name)) < 0)
		{
			[self errorMessage: @"Couldn't connect to the Newton"
				withInformativeText:
					[NSString stringWithFormat:
						@"Couldn't connect to the Newton: please check that "
						"ROM Dumper is running and that the IP address "
						"matches what it displays (OS Error: %i)", errno]];
			[self cleanUp];
			break;
		}
		
		// Change the determinacy of the progress indicator.
		[mProgressIndicator setMinValue: 0.0];
		[mProgressIndicator setMaxValue: kROMSize];
		[mProgressIndicator setDoubleValue: 0.0];
		[mProgressIndicator setIndeterminate: NO];
		
		// Create a socketpair to handle cancelations.
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, mSocketPair) < 0)
		{
			[self errorMessage: @"Couldn't create the socket pair"
				withInformativeText:
					[NSString stringWithFormat:
						@"The OS returned the following error: %i", errno]];
			[self cleanUp];
			break;
		}
		
		// Create a thread to suck the 8 MB.
		[NSThread detachNewThreadSelector:
			@selector(performDumpInThreadWithIgnoredData:)
			toTarget: self
			withObject: nil];
	} while (false);
}

// -------------------------------------------------------------------------- //
//  * (void) performDumpInThreadWithIgnoredData:(id)
// -------------------------------------------------------------------------- //
- (void) performDumpInThreadWithIgnoredData:(id) ignored
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	// Init the size of what we dumped so far.
	mROMSize = 0;
	char buffer[1024];
	fd_set theFds;
	int maxFd = mSocketPair[1];
	if (maxFd < mSocket)
	{
		maxFd = mSocket;
	}
	maxFd++;
	int everythingIsOK = true;
	
	// Loop with a select.
	while (everythingIsOK)
	{
		FD_ZERO(&theFds);
		FD_SET(mSocket, &theFds);
		FD_SET(mSocketPair[1], &theFds);
		
		int nb = select( maxFd, &theFds, nil, nil, nil );
		if (nb > 0)
		{
			if (FD_ISSET(mSocketPair[1], &theFds))
			{
				break;
			}
			
			if (FD_ISSET(mSocket, &theFds))
			{
				// Read up to 1 KB.
				ssize_t amount = read(mSocket, buffer, sizeof(buffer));
				if (amount > 0)
				{
					if (write(mROMFileFd, buffer, amount) != amount)
					{
						everythingIsOK = false;
						break;
					}
					mROMSize += amount;

					if (mROMSize > kROMSize)
					{
						everythingIsOK = false;
						break;
					}
					
					[self addDeferredSend: @selector(reportProgress)];
				} else {
					everythingIsOK = false;
				}

				if (mROMSize == kROMSize)
				{
					break;
				}
			}
		}
	}
	
	// Close the file and the socket.
	(void) close(mROMFileFd);
	mROMFileFd = 0;
	(void) close(mSocket);
	mSocket = 0;

	// Also close our part of the socket pair.
	(void) close(mSocketPair[1]);
	mSocketPair[1] = 0;
	
	if (everythingIsOK)
	{
		[self addDeferredSend: @selector(reportEnd)];
	} else {
		[self addDeferredSend: @selector(reportError)];
	}

	[pool release];
}

// -------------------------------------------------------------------------- //
//  * (void) reportProgress
// -------------------------------------------------------------------------- //
- (void) reportProgress
{
	[mProgressIndicator setDoubleValue: (double) mROMSize];
	[mProgressIndicator displayIfNeeded];
}

// -------------------------------------------------------------------------- //
//  * (void) reportError
// -------------------------------------------------------------------------- //
- (void) reportError
{
	[self errorMessage: @"Some error occurred during the transfer"
		withInformativeText:
			[NSString stringWithFormat:
				@"The OS returned the following error: %i", errno]];
	[self cleanUp];
}

// -------------------------------------------------------------------------- //
//  * (void) reportEnd
// -------------------------------------------------------------------------- //
- (void) reportEnd
{
	(void) close(mSocketPair[0]);
	mSocketPair[0] = 0;
	
	[self setRunning: NO];
	
	if (mROMSize == kROMSize)
	{
		// Finished: dispose the dialog.
		[mDumpROMPanel close];
		
		// Update the preferences
		[[mUserDefaultsController values]
			setValue: mROMFilePath
			forKey: kROMImagePathKey];
		
		[mSetupController updateStartButtonState];
	} else {
		// Unlink the file.
		(void) unlink( [mROMFilePath UTF8String] );
	}
}

// -------------------------------------------------------------------------- //
//  * (IBAction) updateIPField:(id)
// -------------------------------------------------------------------------- //
- (IBAction) updateIPField:(id)sender
{
	if (sender == mIP1)
	{
		if ([[mIP1 stringValue] isEqualToString: @""])
		{
			mIPSet &= ~kIPSetOctet1;
		} else {
			mIP = (mIP & 0x00FFFFFF) | (([mIP1 intValue] & 0xFF) << 24);
			mIPSet |= kIPSetOctet1;
		}
	} else if (sender == mIP2) {
		if ([[mIP2 stringValue] isEqualToString: @""])
		{
			mIPSet &= ~kIPSetOctet2;
		} else {
			mIP = (mIP & 0xFF00FFFF) | (([mIP2 intValue] & 0xFF) << 16);
			mIPSet |= kIPSetOctet2;
		}
	} else if (sender == mIP3) {
		if ([[mIP3 stringValue] isEqualToString: @""])
		{
			mIPSet &= ~kIPSetOctet3;
		} else {
			mIP = (mIP & 0xFFFF00FF) | (([mIP3 intValue] & 0xFF) << 8);
			mIPSet |= kIPSetOctet3;
		}
	} else if (sender == mIP4) {
		if ([[mIP4 stringValue] isEqualToString: @""])
		{
			mIPSet &= ~kIPSetOctet4;
		} else {
			mIP = (mIP & 0xFFFFFF00) | ([mIP4 intValue] & 0xFF);
			mIPSet |= kIPSetOctet4;
		}
	}
	
	[mStartStopButton setEnabled: (mIPSet == kIPSetAll)];
}

// -------------------------------------------------------------------------- //
//  * (BOOL) windowShouldClose:(id)
// -------------------------------------------------------------------------- //
- (BOOL) windowShouldClose:(id)sender
{
	// Prevent the window from being closed if we're currently
	// performing the dump.
	if (mRunning)
	{
		[self errorMessage: @"Dump is in progress" withInformativeText:
			@"Dump is in progress. Please click stop before closing this window"];
		return NO;
	} else {
		return YES;
	}
}

// -------------------------------------------------------------------------- //
//  * (void) errorMessage:(NSString*) withInformativeText:(NSString*)
// -------------------------------------------------------------------------- //
- (void) errorMessage:(NSString*) message
			withInformativeText:(NSString*) informativeText
{
	NSAlert* theAlert = [[NSAlert alloc] init];
	[theAlert setMessageText: message];
	[theAlert setInformativeText: informativeText];
	[theAlert setAlertStyle: NSCriticalAlertStyle];
	[theAlert beginSheetModalForWindow: mDumpROMPanel
			modalDelegate: nil
			didEndSelector: nil
			contextInfo: nil];
}

// -------------------------------------------------------------------------- //
//  * (void) cleanUp
// -------------------------------------------------------------------------- //
- (void) cleanUp
{
	if (mROMFileFd)
	{
		(void) close(mROMFileFd);
		mROMFileFd = 0;
		// Unlink the file.
		(void) unlink( [mROMFilePath UTF8String] );
	}
	if (mSocket)
	{
		(void) close(mSocket);
		mSocket = 0;
	}
	if (mSocketPair[0])
	{
		(void) close(mSocketPair[0]);
		mSocketPair[0] = 0;
	}
	
	[self setRunning: NO];
}

// -------------------------------------------------------------------------- //
//  * (void) setRunning:(BOOL)
// -------------------------------------------------------------------------- //
- (void) setRunning:(BOOL) running
{
	mRunning = running;
	if (mRunning)
	{
		[mStartStopButton setTitle: @"Stop"];
		[mProgressIndicator setIndeterminate: YES];
		[mProgressIndicator setHidden: NO];
		[mProgressIndicator startAnimation: self];
	} else {
		[mStartStopButton setTitle: @"Start"];
		[mProgressIndicator stopAnimation: self];
		[mProgressIndicator setHidden: YES];
	}
}

// -------------------------------------------------------------------------- //
//  * (void) addDeferredSend:(SEL)
// -------------------------------------------------------------------------- //
- (void) addDeferredSend:(SEL)message
{
	// Create some invocation
	NSInvocation* theInvocation
		= [self createInvocationWithTarget: self withSelector: message];
	
	// Create a timer.
	NSTimer* theTimer =
		[NSTimer
			timerWithTimeInterval: 0.0
			invocation: theInvocation
			repeats: NO];
	
	// Send the invocation
	[mRunLoop addTimer: theTimer forMode: NSDefaultRunLoopMode];	
}

// ------------------------------------------------------------------------- //
//  * createInvocationWithTarget: (id) inTarget withSelector: (SEL)
// ------------------------------------------------------------------------- //
- (NSInvocation*) createInvocationWithTarget: (id) inTarget
								withSelector: (SEL) inSelector
{
	NSMethodSignature* theSignature;
	NSInvocation* theResult;
	
	// RŽcupŽration de la signature.
	theSignature = [inTarget methodSignatureForSelector: inSelector];
	
	// CrŽation de l'invocation.
	theResult = [NSInvocation invocationWithMethodSignature: theSignature];
	
	// On rgle le sŽlecteur.
	[theResult setSelector: inSelector];
	
	// On rgle la cible.
	[theResult setTarget: inTarget];
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * (void)awakeFromNib
// -------------------------------------------------------------------------- //
- (void)awakeFromNib
{
	[mDumpROMPanel center];
}

@end

// ========================================================================== //
// AmigaDOS Beer: The company has gone out of business, but their recipe has  //
// been picked up by some weird German company, so now this beer will be an   //
// import.  This beer never really sold very well because the original        //
// manufacturer didn't understand marketing. Like Unix Beer, AmigaDOS Beer    //
// fans are an extremely loyal and loud group. It originally came in a        //
// 16-oz. can, but now comes in 32-oz.  cans too.  When this can was          //
// originally introduced, it appeared flashy and colorful, but the design     //
// hasn't changed much over the years, so it appears dated now.  Critics of   //
// this beer claim that it is only meant for watching TV anyway.              //
// ========================================================================== //
