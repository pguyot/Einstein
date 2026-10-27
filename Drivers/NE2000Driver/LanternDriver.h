// ==============================
// File:            LanternDriver.h
// Project:         Einstein
//
// Copyright 2026 by Matthias Melcher.
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

#ifndef _LANTERNDRIVER_H
#define _LANTERNDRIVER_H

// The two protocols between NIE's Lantern task and a network card driver,
// as far as a driver needs them. The order of the methods is the order of
// the protocols' branch tables and must not change.

#include <Newton.h>
#include <NewtErrors.h>
#include <Protocols.h>
#include <LanternTypes.h>
#include <LanternErrors.h>
#include <LanternEvents.h>

class TTime;
class TCardSocket;
class TCardPCMCIA;
class TLanternTask;

#define kDefaultPowerDownTime 10000 // Allow 10 seconds (AppleTalk likes this).

typedef void (*DriverProcPtr)(void*, ...);

///
/// The services of Lantern for the driver. Each method branches to Lantern's
/// implementation, see LanternDriverAPI.cpp.
///
PROTOCOL TLanternDriverAPI : public TProtocol
{
public:
	NewtonErr Init(TLanternTask*);
	NewtonErr PowerOn(Boolean* powerOn, Boolean intEnable = true);
	NewtonErr PowerOff(ULong msDelay = kDefaultPowerDownTime, Boolean intDisable = true);
	Ptr NewPacketPtr(Size size);
	NewtonErr RxReady(Ptr buf, Size size = 0);
	NewtonErr InterruptDone(void);
	NewtonErr CardHandlerSpecific(ULong selector, ...);
	NewtonErr AddEventHandler(kLanternEventType, DriverProcPtr);
	NewtonErr PostEvent(kLanternEventType, ...);
	NewtonErr PostLocalEvent(kLanternEventType, ...);
	NewtonErr PostLocalEvent(TTime*, kLanternEventType, ...);
	NewtonErr PostReply(NewtonErr);
	NewtonErr PostReply(NewtonErr, ULong nArgs, ...);
};

///
/// The driver, implemented by the card driver (TNE2000Sample).
///
PROTOCOL TLanternDriver : public TProtocol
{
public:
	NewtonErr Init(void);
	NewtonErr Enable(void);
	NewtonErr Disable(void);
	void InterruptHandler(void);

protected:
	// Set by Lantern after New(), so New() can't use them.
	TCardSocket* fCardSocket;
	TCardPCMCIA* fCardPCMCIA;
	TLanternDriverAPI* fDriverAPI;
};

#endif
// _LANTERNDRIVER_H
