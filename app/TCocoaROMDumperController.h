// ==============================
// File:			TCocoaROMDumperController.h
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

#ifndef _TCOCOAROMDUMPERCONTROLLER_H
#define _TCOCOAROMDUMPERCONTROLLER_H

#include <K/Defines/KDefinitions.h>
#import <Cocoa/Cocoa.h>
#include <netinet/in.h>

enum {
	kIPSetNone		= 0x0,
	kIPSetOctet1	= 0x1,
	kIPSetOctet2	= 0x2,
	kIPSetOctet3	= 0x4,
	kIPSetOctet4	= 0x8,
	kIPSetAll		= 0xF
};

enum {
	kROMDumperPort	= 10080,
	kROMSize		= 8 * 1024 * 1024
};

@class TCocoaSetupController;

///
/// Protocol pour le contrôleur pour le téléchargement de la ROM.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
@interface TCocoaROMDumperController : NSObject
{
	IBOutlet NSWindow* mDumpROMPanel;
	IBOutlet NSTextField* mIP1;
	IBOutlet NSTextField* mIP2;
	IBOutlet NSTextField* mIP3;
	IBOutlet NSTextField* mIP4;
	IBOutlet NSProgressIndicator* mProgressIndicator;
	IBOutlet NSButton* mStartStopButton;
	IBOutlet NSUserDefaultsController* mUserDefaultsController;
	IBOutlet TCocoaSetupController* mSetupController;
	
	in_addr_t	mIP;			// IP address
	int			mIPSet;			// If the IP address is set.
	int			mSocket;		// Socket fd.
	int			mSocketPair[2];	// Pair to handle cancellation.
	BOOL		mRunning;		// If we're running.
	int			mROMFileFd;		// ROM File fd.
	NSString*	mROMFilePath;	// Path of the ROM file.
	int			mROMSize;		// Amount downloaded so far.
	NSRunLoop*	mRunLoop;		// Boucle d'exécution courante.
};

// init
- (id)init;

// release
- (void)dealloc;

// NSNibAwaking
- (void)awakeFromNib;

// Actions.
- (IBAction) startStopAction:(id) sender;
- (IBAction) updateIPField:(id) sender;

// Utilities.
- (void) errorMessage:(NSString*) message
			withInformativeText:(NSString*) informativeText;
- (void) setRunning:(BOOL) running;
- (NSInvocation*) createInvocationWithTarget: (id) inTarget
								withSelector: (SEL) inSelector;
- (void) addDeferredSend:(SEL)message;
- (void) reportProgress;
- (void) reportError;
- (void) reportEnd;
- (void) performDump;
- (void) cleanUp;

// Delegation
- (BOOL) windowShouldClose:(id)sender;
@end

#endif
		// _TCOCOAROMDUMPERCONTROLLER_H

// =================================== //
// Thrashing is just virtual crashing. //
// =================================== //
