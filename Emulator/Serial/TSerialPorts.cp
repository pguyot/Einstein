// ==============================
// File:			TSerialPortSuperviser.cp
// Project:			Einstein
//
// Copyright 2003-2020 by Paul Guyot (pguyot@kallisys.net) and Matthias Melcher.
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

#include "TSerialPorts.h"

// POSIX
#include <sys/types.h>
#include <signal.h>
#include <string.h>

#if !TARGET_OS_WIN32
	#include <unistd.h>
	#include <sys/time.h>
#endif

#include "../Log/TLog.h"
#include "../TInterruptManager.h"
#include "../TDMAManager.h"

#include "TEmulator.h"

#include "Emulator/Serial/TBasicSerialPortManager.h"
#if TARGET_OS_MAC
#include "Emulator/Serial/TPipesSerialPortManager.h"
#include "Emulator/Serial/TPtySerialPortManager.h"
#include "Emulator/Serial/TBasiliskIISerialPortManager.h"
#endif
#if TARGET_OS_MAC || TAGRET_OS_ANDROID
#include "Emulator/Serial/TTcpClientSerialPortManager.h"
#endif


/**
 Create a serial port supervisor.

 @param inEmulator this is our owner
 @param inLog write errors and wornings to this log
 */
TSerialPorts::TSerialPorts(TEmulator *inEmulator, TLog* inLog)
:	mLog( inLog )
,	mEmulator( inEmulator )
{
}


/**
 Destroy the serial port supervisor and all managed drivers.
 */
TSerialPorts::~TSerialPorts()
{
	for (int i=0; i<4; ++i)
		delete mDriver[i];
}


/**
 Return the driver for the given port.

 @param ix index into driver list
 @return a pointer to the driver or null
 */
TSerialPortManager *TSerialPorts::GetDriverFor(EPortIndex ix)
{
	assert(ix>=0 && ix<4);
	return mDriver[ix];
}

/**
 Initialize all drivers and run them

 @param exterDriver the driver for the external port
 @param infrDriver the driver for the infrared port
 @param tbltDriver the driver for the internal pin connector
 @param mdemDriver the driver for the internal fully equipped modem port
 */
void TSerialPorts::Initialize(EDriverID extrDriver,
							  EDriverID infrDriver,
							  EDriverID tbltDriver,
							  EDriverID mdemDriver)
{
	ReplaceDriver(kExtr, extrDriver);
	ReplaceDriver(kInfr, infrDriver);
	ReplaceDriver(kTblt, tbltDriver);
	ReplaceDriver(kMdem, mdemDriver);
}

/**
 Replace an existing driver with a new driver.

 @param port the index into the port that we will emulate
 @param driver create thsi driver, attach it, and run it
 */
void TSerialPorts::ReplaceDriver(EPortIndex inPort, EDriverID inDriverId)
{
	TSerialPortManager *&currentDriver = mDriver[inPort];
	if (currentDriver) {
		delete currentDriver;
		currentDriver = nullptr;
	}
	switch (inDriverId) {
		case kNullDriver:
			currentDriver = new TSerialPortManager(mLog, inPort);
			break;
#if TARGET_OS_MAC
		case kPipesDriver:
			currentDriver = new TPipesSerialPortManager(mLog, inPort);
			break;
		case kPtyDriver:
			currentDriver = new TPtySerialPortManager(mLog, inPort);
			break;
		case kBasiliskIIDriver:
			currentDriver = new TBasiliskIISerialPortManager(mLog, inPort);
			break;
#endif
#if TARGET_OS_MAC || TAGRET_OS_ANDROID
		case kTcpClientDriver:
			currentDriver = new TTcpClientSerialPortManager(mLog, inPort);
			break;
#endif
		default:
			currentDriver = new TSerialPortManager(mLog, inPort);
			mLog->FLogLine("ERROR: request for unsupported serial driver type %d on port %d\n", inDriverId, inPort);
	}
	currentDriver->run(mEmulator->GetInterruptManager(),
					   mEmulator->GetDMAManager(),
					   mEmulator->GetMemory());
}

KUInt32 TSerialPorts::NDrivers = kNDriverID;

std::vector<const char*>TSerialPorts::DriverNames =
{
	"None", "Named Pipes", "Pseudoterminal", "BasiliskII", "Network Client"
};

KUInt32 TSerialPorts::NPorts = kNPortIndex;

std::vector<const char*>TSerialPorts::PortNames = {
	"external", "infrared", "internal", "modem"
};

std::vector<const char*>TSerialPorts::ShortPortNames =
{
	"extr", "infr", "tblt", "mdem"
};

TSerialPorts::EDriverID TSerialPorts::ValidDrivers[] = {
#if TARGET_OS_MAC
	kNullDriver, kPipesDriver, kPtyDriver, kBasiliskIIDriver, kTcpClientDriver, (EDriverID)-1
#elif TARGET_OS_ANDROID
	kNullDriver, kTcpClientDriver, (EDriverID)-1
#else
	kNullDriver, (EDriverID)-1
#endif
};

TSerialPorts::EDriverID TSerialPorts::NoValidDrivers[] =
	{ (EDriverID)-1 };

TSerialPorts::EDriverID *TSerialPorts::ValidDriversByPort[] =
{
	// Depending
	ValidDrivers, NoValidDrivers, NoValidDrivers, NoValidDrivers
};


// ================================================================== //
// We are experiencing system trouble -- do not adjust your terminal. //
// ================================================================== //
