// ==============================
// File:			EinsteinInstallPackageCommand.mm
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

#import "EinsteinInstallPackageCommand.h"
#import "TCocoaAppController.h"

@implementation EinsteinInstallPackageCommand : NSScriptCommand

-(id)performDefaultImplementation
{
	id		directParameter = [self directParameter];

	if ((directParameter == NULL)
		|| ([directParameter isKindOfClass:[NSURL class]] == NO)
		|| ([((NSURL*) directParameter) isFileURL] == NO))
	{
		[self setScriptErrorNumber: errAECoercionFail];
	} else {
		[[TCocoaAppController getInstance] commandInstallPackage: ((NSURL*) directParameter)];
	}
	
	return nil;
}

@end

// ==================================================================== //
// The goal of Computer Science is to build something that will last at //
// least until we've finished building it.                              //
// ==================================================================== //
