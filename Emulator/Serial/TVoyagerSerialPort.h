// ==============================
// File:			TVoyagerSerialPort.h
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

#ifndef _TVOYAGERSERIALPORT_H
#define _TVOYAGERSERIALPORT_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TInterruptManager;
class TDMAManager;

///
/// Class for the 4 voyager serial ports.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 107 $
///
/// \test	aucun test défini.
///
class TVoyagerSerialPort
{
public:
	///
	/// Built-in location IDs
	///
	enum ELocationID {
		kExternalSerialPort		= 'extr',
		kInfraredSerialPort		= 'infr',
		kBuiltInExtraSerialPort	= 'tblt',
		kModemSerialPort		= 'mdem'
	};
	
	///
	/// Constructor from a log, a location ID, the DMA manager and the
	/// interrupt manager.
	///
	TVoyagerSerialPort(
			TLog* inLog,
			ELocationID inLocationID,
			TInterruptManager* inInterruptManager,
			TDMAManager* inDMAManager );

	///
	/// Destructor.
	///
	~TVoyagerSerialPort( void );

	/// \name Low-level routines.
	
	///
	/// Write register.
	///
	void	WriteRegister( KUInt32 inOffset, KUInt8 inValue );

	///
	/// Read register.
	///
	KUInt8	ReadRegister( KUInt32 inOffset );

private:

	///
	/// Registers.
	///
	enum {
		kSerReg_AckStatus	= 0x2000,		///< Write bit 0x01 to ack an error.
		kSerReg_BreakDuplex	= 0x2400,
	};
	
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TVoyagerSerialPort( const TVoyagerSerialPort& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TVoyagerSerialPort& operator = ( const TVoyagerSerialPort& inCopy );

	/// \name Variables
	TLog*				mLog;				///< Reference to the log object
											///< (or NULL)
	ELocationID			mLocationID;		///< Location ID (which serial port)
	TInterruptManager*	mInterruptManager;	///< Interface to the interrupt mgr.
	TDMAManager*		mDMAManager;		///< Interface to the DMA mgr.
};

#endif
		// _TVOYAGERSERIALPORT_H

// ================= //
// Byte your tongue. //
// ================= //
