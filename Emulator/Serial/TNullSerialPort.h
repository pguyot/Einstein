// ==============================
// File:			TNullSerialPort.h
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

#ifndef _TNULLSERIALPORT_H
#define _TNULLSERIALPORT_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TInterruptManager;
class TDMAManager;

///
/// Emulation of serial ports when we just ignore them.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 107 $
///
/// \test	no test defined.
///
class TNullSerialPort : public TVoyagerSerialPort
{
public:
	///
	/// Constructor from a log, a location ID, the DMA manager and the
	/// interrupt manager.
	///
	TNullSerialPort(
			TLog* inLog,
			ELocationID inLocationID,
			TInterruptManager* inInterruptManager,
			TDMAManager* inDMAManager );

	///
	/// Destructor.
	///
	~TNullSerialPort( void );

private:
};

#endif
		// _TNULLSERIALPORT_H

// ============================================ //
// The first time, it's a KLUDGE!               //
// The second, a trick.                         //
// Later, it's a well-established technique!    //
//                 -- Mike Broido, Intermetrics //
// ============================================ //
