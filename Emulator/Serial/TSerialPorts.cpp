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
#include <csignal>
#include <cstring>
#include <cassert>

#if !TARGET_OS_WIN32
	#include <ctime>
#endif

#include "Emulator/Log/TLog.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/TDMAManager.h"

#include "Emulator/TEmulator.h"

#include "Emulator/Serial/TBasicSerialPortManager.h"
#if TARGET_OS_MAC
#include "Emulator/Serial/TPipesSerialPortManager.h"
#include "Emulator/Serial/TPtySerialPortManager.h"
#include "Emulator/Serial/TBasiliskIISerialPortManager.h"
#endif
#if TARGET_OS_MAC || TAGRET_OS_ANDROID || TARGET_OS_LINUX || TARGET_OS_WIN32
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
	for (auto & i : mDriver)
		delete i;
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
 If the ID of the old and new driver are the same, the old driver is kept and nothing will happen.

 \param port the index into the port that we will emulate
 \param driver create this driver, attach it, and run it

 \return a pointer to the new driver, or the previous driver if the driver did not change
 */
TSerialPortManager *TSerialPorts::ReplaceDriver(EPortIndex inPort, EDriverID inDriverId)
{
	TSerialPortManager *currentDriver = mDriver[inPort];

	if (currentDriver && currentDriver->GetID() == inDriverId)
		return currentDriver;

	if (currentDriver) {
		delete currentDriver;
		mDriver[inPort] = nullptr;
	}
	switch (inDriverId) {
		case kNullDriver:
			currentDriver = new TBasicSerialPortManager(mLog, inPort);
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
#if TARGET_OS_MAC || TAGRET_OS_ANDROID || TARGET_OS_LINUX || TARGET_OS_WIN32
		case kTcpClientDriver:
			currentDriver = new TTcpClientSerialPortManager(mLog, inPort);
			break;
#endif
		default:
			currentDriver = new TBasicSerialPortManager(mLog, inPort);
			mLog->FLogLine("ERROR: request for unsupported serial driver type %d on port %d\n", inDriverId, inPort);
	}
	mDriver[inPort] = currentDriver;

	currentDriver->run(mEmulator->GetInterruptManager(),
					   mEmulator->GetDMAManager(),
					   mEmulator->GetMemory());

	return currentDriver;
}

KUInt32 TSerialPorts::NDrivers = kNDriverID;

std::vector<const char*>TSerialPorts::DriverNames = /* NOLINT */
{
	"None", "Named Pipes", "Pseudoterminal", "BasiliskII", "Network Client"
};

KUInt32 TSerialPorts::NPorts = kNPortIndex;

std::vector<const char*>TSerialPorts::PortNames = /* NOLINT */
{
    "external", "infrared", "internal", "modem"
};

std::vector<const char*>TSerialPorts::ShortPortNames = /* NOLINT */
{
    "extr", "infr", "tblt", "mdem"
};

TSerialPorts::EDriverID TSerialPorts::ValidDrivers[] = {
#if TARGET_OS_MAC
	kNullDriver, kPipesDriver, kPtyDriver, kBasiliskIIDriver, kTcpClientDriver, (EDriverID)-1
#elif TARGET_OS_ANDROID || TARGET_OS_LINUX || TARGET_OS_WIN32
	kNullDriver, kTcpClientDriver, (EDriverID)-1
#else
	kNullDriver, (EDriverID)-1
#endif
};

TSerialPorts::EDriverID TSerialPorts::NoValidDrivers[] =
	{ kNullDriver, (EDriverID)-1 };

TSerialPorts::EDriverID *TSerialPorts::ValidDriversByPort[] =
{
	// Depending
#if 0
	ValidDrivers, NoValidDrivers, NoValidDrivers, NoValidDrivers
#else
	ValidDrivers, ValidDrivers, ValidDrivers, ValidDrivers
#endif
};


/**
 Get a list of human-readable names for all available serial port drivers.

 \return Ref to an array of strings; the length of the array is also the number of available drivers
 */
NewtRef TSerialPorts::NSGetDriverNames(TNewt::RefArg arg)
{
	// ignore the arg (should be NIL)
	auto nNames = (unsigned)DriverNames.size();
	TNewt::RefVar array( TNewt::AllocateArray(nNames) );
	for (unsigned i=0; i<nNames; i++) {
		TNewt::RefVar str( TNewt::MakeString(DriverNames[i]) );
		TNewt::SetArraySlot(array, i,  str);
	}
	return array.Ref();
}


/**
 NewtonScript call to return all valid drivers for a port.
 Get a list of valid drivers for this particular emulation.
 \return Ref to an array of DriverIDs as integers
 */
NewtRef TSerialPorts::NSGetDriverList(TNewt::RefArg arg)
{
	using namespace TNewt;

	NewtRef argRef = arg.Ref();
	int portIndex = 0;
	if (RefIsInt(argRef)) {
		portIndex = RefToInt(argRef);
		if (portIndex<0 || portIndex>=kNPortIndex)
			portIndex = 0;
	} else if (argRef==kNewtRefNIL) {
		portIndex = 0;
	}

	EDriverID *validDrivers = ValidDriversByPort[portIndex];
	// Count valid drivers
	unsigned nDriver;
	for (nDriver = 0; (int)validDrivers[nDriver]!=-1; nDriver++) { }
	// Create the arrays
	RefVar array( AllocateArray(nDriver) );
	for (unsigned i=0; i<nDriver; i++) {
		TNewt::SetArraySlotRef(array.Ref(), i,  TNewt::MakeInt((int)validDrivers[i]));
	}
	return array.Ref();
}

/**
 NewtonScript call to retrieve the current driver and its settings.

 \param arg is an integer with the index of the serial port that we find interesting
 \return the minimal return value is the frame { driver: ix }; where ix is an integre
	index into the list of available drivers. Individual drivers may add more optiosns
	The TCP Client adds { tcpServer: "address", tcpPort: "numberAsText" };
 */
NewtRef TSerialPorts::NSGetDriverAndOptions(TNewt::RefArg arg)
{
	using namespace TNewt;

	NewtRef argRef = arg.Ref();
	int portIndex = 0;
	if (RefIsInt(argRef)) {
		portIndex = RefToInt(argRef);
		if (portIndex<0 || portIndex>=kNPortIndex)
			portIndex = 0;
	} else if (argRef==kNewtRefNIL) {
		portIndex = 0;
	}

	TSerialPortManager *driver = GetDriverFor((EPortIndex)portIndex);
	RefVar f( AllocateFrame() );
	SetFrameSlot(f, RefVar( MakeSymbol("driver")),  RefVar(MakeInt((KSInt32)driver->GetID())));
	driver->NSGetOptions(f);
	return f.Ref();
}

/**
 NewtonScript call to change the current driver and/or its settings

 \param arg is a record with a slot 'port that holds the index of the serail port to be updated,
	a slot 'driver, holding an integer with the index to the driver that we want for this slot,
	and possibly other options that are specific for that driver.
 */
NewtRef TSerialPorts::NSSetDriverAndOptions(TNewt::RefArg arg)
{
	using namespace TNewt;

	int portIndex = -1;
	int driverId = -1;

	NewtRef frame = arg.Ref();
	if (!RefIsFrame(frame)) {
		return MakeInt(kNSErrNotAFrame);
	}

	NewtRef fPort = GetFrameSlotRef(frame, MakeSymbol("Port"));
	if (RefIsInt(fPort))
		portIndex = (EPortIndex)RefToInt(fPort);

	NewtRef fDriver = GetFrameSlotRef(frame, MakeSymbol("Driver"));
	if (RefIsInt(fDriver))
		driverId = (EDriverID)RefToInt(fDriver);

	if (portIndex!=-1 && driverId!=-1) {
		TSerialPortManager *d = ReplaceDriver((EPortIndex)portIndex, (EDriverID)driverId);
		d->NSSetOptions(arg);
		if (mPortChangedCallback)
			mPortChangedCallback(portIndex);
	}

	return kNewtRefNIL;
}


// ================================================================== //
// Apple's port names are Thunderbolt and Lightning.                  //
// They're starting to very, very frighten me.                        //
// ================================================================== //
