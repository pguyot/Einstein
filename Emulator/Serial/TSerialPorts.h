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

#include <vector>

class TLog;
class TEmulator;
class TSerialPortManager;


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
		kNullDriver = 0,
		kPipesDriver,
		kPtyDriver,
		kBasiliskIIDriver,
		kTcpClientDriver,
		kNDriverID
	};

	enum EOption {
		kEnd = 0,
		kServer, 	// followed by a string
		kPort		// followed by an integer
	};

	// Static information that can be retreived before the class is instantiated
	// This is useful for user interfaces
	static KUInt32 NDrivers;
	static std::vector<const char*>DriverNames;
	static KUInt32 NPorts;
	static std::vector<const char*>PortNames;
	static std::vector<const char*>ShortPortNames;
	static EDriverID *ValidDriversByPort[];
	static EDriverID ValidDrivers[];
	static EDriverID NoValidDrivers[];

	// Constructor
	TSerialPorts(TEmulator *inEmulator, TLog* inLog);

	// Destructor.
	~TSerialPorts();

	// Return a driver for a given index
	TSerialPortManager *GetDriverFor(EPortIndex ix);

	// Initialize all drivers and run them
	void Initialize(EDriverID extrDriver,
					EDriverID infrDriver,
					EDriverID tbltDriver,
					EDriverID mdemDriver);

	// Replace an existing driver with a new driver
	void ReplaceDriver(EPortIndex inPort, EDriverID inDriverId);


private:
	TSerialPortManager 	*mDriver[4] = { nullptr, nullptr, nullptr, nullptr };
	TLog				*mLog = nullptr;
	TEmulator 			*mEmulator = nullptr;

};

#endif
		// _T_SERIAL_PORTS_H

// ================= //
// Byte your tongue. //
// ================= //
