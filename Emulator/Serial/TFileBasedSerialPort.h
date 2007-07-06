// ==============================
// File:			TFileBasedSerialPort.h
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

#ifndef _TFILEBASEDSERIALPORT_H
#define _TFILEBASEDSERIALPORT_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TInterruptManager;
class TDMAManager;

///
/// Emulation of serial ports with a (unix) file interface.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 107 $
///
/// \test	no test defined.
///
class TFileBasedSerialPort : public TVoyagerSerialPort
{
public:
	///
	/// Constructor from a log, a location ID, the DMA manager, the
	/// interrupt manager and a path.
	///
	TFileBasedSerialPort(
			TLog* inLog,
			ELocationID inLocationID,
			TInterruptManager* inInterruptManager,
			TDMAManager* inDMAManager,
			const char* inPath );

	///
	/// Destructor.
	///
	~TFileBasedSerialPort( void );

private:
	/// \name Variables
	int					mFileDescriptor;	///< FD for the file
											///< (or -1)
};

#endif
		// _TFILEBASEDSERIALPORT_H

// ==================================================================== //
// ... one of the main causes of the fall of the Roman Empire was that, //
// lacking zero, they had no way to indicate successful termination of  //
// their C programs.                                                    //
//                 -- Robert Firth                                      //
// ==================================================================== //
