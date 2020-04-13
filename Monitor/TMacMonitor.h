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

@class TCocoaMonitorController;

class TMacMonitor : public TMonitor
{
public:
	TMacMonitor(
			TBufferLog* inLog,
			TEmulator* inEmulator,
			TSymbolList* inSymbolList,
			const char *inROMPath );

	virtual ~TMacMonitor( void );
	
	virtual void PrintLine(const char *inLine, int type);
	void SetController(TCocoaMonitorController *inController);

	bool ProcessBreakpoint( KUInt16 inBPID, KUInt32 inBPAddr );

	NSString* GetAlarm();
	NSString* GetCPSR();
	NSString* GetDisasmLine(int offset);
	NSString* GetFIQMask();
	NSString* GetFrozenTimer();
	NSString* GetIntCtrlReg();
	NSString* GetIntRaised();
	NSString* GetIntEDReg1();
	NSString* GetIntEDReg2();
	NSString* GetIntEDReg3();
	NSString* GetRealTimeClock();
	NSString* GetRegister(int rn);
	NSString* GetSPSR();
	NSString* GetSymbol();
	NSString* GetTimerMatchRegister(int rn);
	
private:
	TCocoaMonitorController *controller;
};

#endif
		// _TMACMONITOR_H
