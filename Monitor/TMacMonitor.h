// ==============================
// File:			TMonitor.h
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

#ifndef _TMACMONITOR_H
#define _TMACMONITOR_H

#import <Cocoa/Cocoa.h>

#include <K/Defines/KDefinitions.h>
#include "Monitor/TMonitor.h"

class TMacMonitor : public TMonitor
{
public:
	TMacMonitor(
			TBufferLog* inLog,
			TEmulator* inEmulator,
			TSymbolList* inSymbolList,
			const char *inROMPath );

	virtual ~TMacMonitor( void );

	NSString* GetScreen( void );

protected:

	NSString*
	GetScreenHalted( void );

	NSString*
	GetScreenRunning( void );

};

#endif
		// _TMACMONITOR_H
