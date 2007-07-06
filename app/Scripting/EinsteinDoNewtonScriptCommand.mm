// ==============================
// File:			EinsteinDoNewtonScriptCommand.mm
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

#import "EinsteinDoNewtonScriptCommand.h"
#import "TCocoaAppController.h"

@implementation EinsteinDoNewtonScriptCommand : NSScriptCommand

-(id)performDefaultImplementation
{
	id		directParameter = [self directParameter];

	if ((directParameter == NULL)
		|| ([directParameter isKindOfClass:[NSString class]] == NO))
	{
		[self setScriptErrorNumber: errAECoercionFail];
	} else {
		[[TCocoaAppController getInstance] commandDoNewtonScript: ((NSString*) directParameter)];
	}
	
	return nil;
}

@end

// ====================================================================== //
// Real computer scientists don't program in assembler.  They don't write //
// in anything less portable than a number two pencil.                    //
// ====================================================================== //
