// ==============================
// File:			TSerialPorts.h
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

#ifndef _T_SERIAL_PORTS_H
#define _T_SERIAL_PORTS_H

#include <K/Defines/KDefinitions.h>

#include "Emulator/Platform/TNewt.h"

#include <functional>
#include <utility>
#include <vector>

class TLog;
class TEmulator;
class TSerialPortManager;
class TSerialHostPort;

/**
 The serial port superviser manages the four port of the MessagePad and their respective drivers.

 This class manages serial port for the rest of the system. It provides a NewtonScript interface
 to allow NS app access to the driver settings.
 */
class TSerialPorts
{
public:
	// Index into serial ports
	enum EPortIndex {
		kExtr = 0,
		kInfr,
		kTblt,
		kMdem,
		kNPortIndex
	};

	// Implemented driver types
	enum EDriverID {
		kDefaultDriver = -1,
		kNullDriver,
		kPipesDriver,
		kPtyDriver,
		kBasiliskIIDriver,
		kTcpClientDriver,
		kDirectDriver,
		kNDriverID
	};

	enum EOption {
		kEnd = 0,
		kServer, // followed by a string
		kPort // followed by an integer
	};

	// Static information that can be retreived before the class is instantiated
	// This is useful for user interfaces
	static KUInt32 NDrivers;
	static std::vector<const char*> DriverNames;
	static KUInt32 NPorts;
	static std::vector<const char*> PortNames;
	static std::vector<const char*> ShortPortNames;
	static EDriverID* ValidDriversByPort[];
	static EDriverID ValidDrivers[];
	static EDriverID NoValidDrivers[];

	// Constructor
	TSerialPorts(TEmulator* inEmulator, TLog* inLog);

	// Destructor.
	~TSerialPorts();

	// Return a driver for a given index
	TSerialPortManager* GetDriverFor(EPortIndex ix);

	// Return the driver for a dynamically allocated port
	TSerialHostPort* GetDriverFor(KUInt32 location);

	// Initialize all drivers and run them
	void Initialize(EDriverID extrDriver,
		EDriverID infrDriver,
		EDriverID tbltDriver,
		EDriverID mdemDriver);

	// Replace an existing driver with a new driver
	TSerialPortManager* ReplaceDriver(EPortIndex inPort, EDriverID inDriverId);

	// Set the driver for any hardware location
	TSerialHostPort* SetDriver(KUInt32 inLocation, EDriverID inDriverId, std::string inConfigData);

	// NewtonScript call to return all driver names
	static NewtRef NSGetDriverNames(TNewt::RefArg arg);

	// NewtonScript call to return all valid drivers for a port
	static NewtRef NSGetDriverList(TNewt::RefArg arg);

	// NewtonScript call to retrive the current driver and its settings
	NewtRef NSGetDriverAndOptions(TNewt::RefArg arg);

	// NewtonScript call to change the current driver and/or its settings
	NewtRef NSSetDriverAndOptions(TNewt::RefArg arg);

	void
	PortChangedCallback(std::function<void(int)> inCallback)
	{
		mPortChangedCallback = std::move(inCallback);
	}

	void
	SetHostPortSettings(KUInt32 inLocation, std::pair<EDriverID, std::string> inSettings);

private:
	/// Current driver for every available port
	TSerialPortManager* mDriver[4] { nullptr, nullptr, nullptr, nullptr };

	/// Log output goes here
	TLog* mLog { nullptr };

	/// Reference to emulator
	TEmulator* mEmulator { nullptr };

	/// This is called if any of the ports gets another driver
	std::function<void(int)> mPortChangedCallback;

	/// (not sure anymore)
	std::map<KUInt32, TSerialHostPort*> mHostPorts;

	/// (not sure anymore)
	std::map<KUInt32, std::pair<EDriverID, std::string>> mHostPortSettings;
};

#endif
// _T_SERIAL_PORTS_H

// ================= //
// Byte your tongue. //
// ================= //
