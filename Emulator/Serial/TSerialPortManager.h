// ==============================
// File:			TSerialPortManager.h
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

#ifndef _TSERIALPORTMANAGER_H
#define _TSERIALPORTMANAGER_H

#include <K/Defines/KDefinitions.h>

#include "TSerialPorts.h"

class TLog;
class TInterruptManager;
class TDMAManager;
class TMemory;
class TEmulator;

///
/// Class for the 4 voyager serial ports.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 107 $
///
/// \test	aucun test défini.
///
class TSerialPortManager
{
public:

	///
	/// Constructor.
	///
	TSerialPortManager(
			TLog* inLog,
			TSerialPorts::EPortIndex inPortIx);

	///
	/// Destructor.
	///
	virtual ~TSerialPortManager( );

	///
	/// Return the Identification of this driver
	///
	virtual KUInt32 GetID() = 0;

	///
	/// Start emulation.
	///
	virtual void run(TInterruptManager* inInterruptManager,
					 TDMAManager* inDMAManager,
					 TMemory* inMemory) = 0;

	/// \name Low-level routines.

	///
	/// Write register.
	///
	virtual void WriteRegister( KUInt32 inOffset, KUInt8 inValue );

	///
	/// Read register.
	///
	virtual KUInt8 ReadRegister( KUInt32 inOffset );

	///
	/// Read DMA register.
	///
	virtual KUInt32 ReadDMARegister( KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister );

	///
	/// Write register.
	///
	virtual void WriteDMARegister( KUInt32 inBank, KUInt32 inChannel, KUInt32 inRegister, KUInt32 inValue );

	///
	/// GIve NewtonScrip access to our list of options
	///
	virtual void NSGetOptions(TNewt::RefArg /*frame*/) { }

	///
	/// Set options from NewtonScript
	///
	virtual void NSSetOptions(TNewt::RefArg /*frame*/) { }

    ///
    /// Constructeur par copie volontairement indisponible.
    ///
    /// \param inCopy		objet à copier
    ///
    TSerialPortManager( const TSerialPortManager& inCopy ) = delete;

    ///
    /// Opérateur d'assignation volontairement indisponible.
    ///
    /// \param inCopy		objet à copier
    ///
    TSerialPortManager& operator = ( const TSerialPortManager& inCopy ) = delete;

protected:

	///
	/// Registers.
	///
	enum {
		kSerReg_AckStatus	= 0x2000,		///< Write bit 0x01 to ack an error.
		kSerReg_BreakDuplex	= 0x2400,
	};

	/// \name Variables
	TLog*				mLog;				///< Reference to the log object
											///< (or NULL)
	TSerialPorts::EPortIndex mNewtPortIndex;///< remember to which port we are connected
	TInterruptManager*	mInterruptManager;	///< Interface to the interrupt mgr.
	TDMAManager*		mDMAManager;		///< Interface to the DMA mgr.
	TMemory*			mMemory;			///< Interface to the memory mgr.
};

#endif
		// _TSERIALPORTMANAGER_H

// ================= //
// Byte your tongue. //
// ================= //
