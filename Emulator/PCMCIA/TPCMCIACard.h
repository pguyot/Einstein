// ==============================
// File:			TPCMCIACard.h
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

#ifndef _TPCMCIACARD_H
#define _TPCMCIACARD_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TEmulator;
class TPCMCIAController;

///
/// Interface for PCMCIA cards.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TPCMCIACard
{
public:
	///
	/// Constructor from a PCMCIA controller.
	///
	TPCMCIACard( void );

	///
	/// Destructor.
	///
	virtual ~TPCMCIACard( void );
	
	///
	/// Called by the controller to say we've been inserted.
	///
	virtual void		Init( TPCMCIAController* inController );
	
	///
	/// Get PCMCIA pins.
	///
	virtual KUInt32		GetVPCPins( void ) = 0;
	
	///
	/// Set PCMCIA pins.
	///
	virtual void		SetVPCPins( KUInt32 inPins ) = 0;

	///
	/// Read attribute space.
	///
	virtual KUInt32		ReadAttr( KUInt32 inOffset ) = 0;

	///
	/// Read attribute space (byte).
	///
	virtual KUInt8		ReadAttrB( KUInt32 inOffset ) = 0;

	///
	/// Read I/O space.
	///
	virtual KUInt32		ReadIO( KUInt32 inOffset ) = 0;

	///
	/// Read I/O space (byte).
	///
	virtual KUInt8		ReadIOB( KUInt32 inOffset ) = 0;

	///
	/// Read memory space.
	///
	virtual KUInt32		ReadMem( KUInt32 inOffset ) = 0;

	///
	/// Read memory space (byte).
	///
	virtual KUInt8		ReadMemB( KUInt32 inOffset ) = 0;

	///
	/// Write attribute space.
	///
	virtual void		WriteAttr( KUInt32 inOffset, KUInt32 inValue ) = 0;

	///
	/// Write attribute space (byte).
	///
	virtual void		WriteAttrB( KUInt32 inOffset, KUInt8 inValue ) = 0;

	///
	/// Write I/O space.
	///
	virtual void		WriteIO( KUInt32 inOffset, KUInt32 inValue ) = 0;

	///
	/// Write I/O space (byte).
	///
	virtual void		WriteIOB( KUInt32 inOffset, KUInt8 inValue ) = 0;

	///
	/// Write memory space.
	///
	virtual void		WriteMem( KUInt32 inOffset, KUInt32 inValue ) = 0;

	///
	/// Write memory space (byte).
	///
	virtual void		WriteMemB( KUInt32 inOffset, KUInt8 inValue ) = 0;

protected:
	inline TLog*				GetLog( void )
		{
			return mLog;
		}

	inline TEmulator*			GetEmulator( void )
		{
			return mEmulator;
		}

	inline TPCMCIAController*	GetPCMCIAController( void )
		{
			return mPCMCIAController;
		}
private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TPCMCIACard( const TPCMCIACard& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TPCMCIACard& operator = ( const TPCMCIACard& inCopy );

	/// \name Variables
	TLog*				mLog;				///< Ref to the log object.
	TEmulator*			mEmulator;			///< Ref to the emulator.
	TPCMCIAController*	mPCMCIAController;	///< Ref to the controller.
};

#endif
		// _TPCMCIACARD_H

// ================================================================== //
// The next person to mention spaghetti stacks to me is going to have //
// his head knocked off.                                              //
//                 -- Bill Conrad                                     //
// ================================================================== //
