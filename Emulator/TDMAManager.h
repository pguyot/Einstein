// ==============================
// File:			TDMAManager.h
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

#ifndef _TDMAMANAGER_H
#define _TDMAMANAGER_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TInterruptManager;
class TMemory;

///
/// Class for the DMA Manager.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 107 $
///
/// \test	aucun test défini.
///
class TDMAManager
{
public:
	/// Channel numbers.
	enum {
		kSerialPort0Receive			= 0,
		kSerialPort0Transmit		= 1,
		kInfraredReceiveAndTransmit	= 2,
		kAudioTransmit				= 3,
		kAudioReceive				= 4,
		kTabletDigitizerReceive		= 5,
		kSerialPort3Receive			= 6,
		kSerialPort3Transmit		= 7
	};
	
	///
	/// Constructor from the memory and the interrupt manager.
	///
	TDMAManager(
		TLog* inLog,
		TMemory* inMemory,
		TInterruptManager* inInterruptManager );

	///
	/// Destructor.
	///
	~TDMAManager( void );

	///
	/// Read the channel assignment register.
	///
	/// \return the value of the channel assignment register.
	///
	KUInt32	ReadChannelAssignmentRegister( void );

	///
	/// Write the channel assignment register.
	///
	/// \param inValue	value to write to the channel assignment register.
	///
	void	WriteChannelAssignmentRegister( KUInt32 inValue );

	///
	/// Write the enable register.
	/// Bits set to 1 will start the DMA transfers.
	///
	/// \param inValue	value to write to the enable register.
	///
	void	WriteEnableRegister( KUInt32 inValue );

	///
	/// Read the status register.
	/// Bits set to 1 indicate pending dma transfers.
	///
	/// \return the value of the status register.
	///
	KUInt32	ReadStatusRegister( void );

	///
	/// Write the disable register.
	/// Bits set to 1 will abort the DMA transfers.
	///
	/// \param inValue	value to write to the disable register.
	///
	void	WriteDisableRegister( KUInt32 inValue );

	///
	/// Read the word status register.
	/// Bits set to 1 indicate words in channel word registers.
	///
	/// \return the value of the word status register.
	///
	KUInt32	ReadWordStatusRegister( void );
	
	///
	/// Read a channel register from first bank.
	///
	/// \param inChannel	ID of the channel
	/// \param inRegister	id of the register
	/// \return the value of the register.
	///
	KUInt32	ReadChannel1Register( KUInt32 inChannel, KUInt32 inRegister );
	
	///
	/// Write a channel register in first bank.
	///
	/// \param inChannel	ID of the channel
	/// \param inRegister	id of the register
	/// \param inValue		word to write to the register.
	/// \return the value of the register.
	///
	void	WriteChannel1Register(
					KUInt32 inChannel,
					KUInt32 inRegister,
					KUInt32 inValue );
	
	///
	/// Read a channel register from second bank.
	///
	/// \param inChannel	ID of the channel
	/// \param inRegister	id of the register
	/// \return the value of the register.
	///
	KUInt32	ReadChannel2Register( KUInt32 inChannel, KUInt32 inRegister );

	///
	/// Write a channel register in second bank.
	///
	/// \param inChannel	ID of the channel
	/// \param inRegister	id of the register
	/// \param inValue		word to write to the register.
	/// \return the value of the register.
	///
	void	WriteChannel2Register(
					KUInt32 inChannel,
					KUInt32 inRegister,
					KUInt32 inValue );

private:
	struct SChannelRegisters {
		KUInt32			fControlRegister;
		KUInt32			fBaseRegister;
		KUInt32			fPointerRegister;
		KUInt32			fCountRegister;
		KUInt32			fSizeRegister;
		KUInt32			fWordRegister;
		KUInt32			fCompareRegister;
	};
	
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TDMAManager( const TDMAManager& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TDMAManager& operator = ( const TDMAManager& inCopy );

	/// \name Variables
	TLog*				mLog;				///< Interface for logging.
	TMemory*			mMemory;			///< Reference on the memory.
	TInterruptManager*	mInterruptManager;	///< Reference on the interrupt mgr.
	
	KUInt32				mAssignmentReg;		///< Assignment register.
};

#endif
		// _TDMAMANAGER_H

// ========================== //
// Memory fault - where am I? //
// ========================== //
