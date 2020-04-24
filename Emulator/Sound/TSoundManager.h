// ==============================
// File:			TSoundManager.h
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

#ifndef _TSOUNDMANAGER_H
#define _TSOUNDMANAGER_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TInterruptManager;
class TMemory;

///
/// Class to handle sound input/output.
/// This class is subclassed by host implementations.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 113 $
///
/// \test	aucun test défini.
///
class TSoundManager
{
public:
	///
	/// Constructor from a log.
	///
	/// \param inLog				log interface (can be null)
	///
	TSoundManager( TLog* inLog = nil );

	///
	/// Destructor.
	///
	virtual ~TSoundManager( void );

	///
	/// Set the memory interface.
	/// This method is called once the memory interface is created.
	///
	/// \param inManager	reference to the memory interface
	///
	void	SetMemory( TMemory* inMemory )
		{
			mMemory = inMemory;
		}

	///
	/// Set the interrupt manager.
	/// This method is called once the interrupt manager is created.
	///
	/// \param inManager	reference to the interrupt manager
	///
	void	SetInterruptManager( TInterruptManager* inManager )
		{
			mInterruptManager = inManager;
		}

	///
	/// Set interrupts to ask for more data.
	///
	/// \param inInputMask	input interrupt mask.
	/// \param inOutputMask	output interrupt mask.
	///
	void	SetInterruptMask( KUInt32 inInputMask, KUInt32 inOutputMask )
		{
			mInputIntMask = inInputMask;
			mOutputIntMask = inOutputMask;
		}

	///
	/// Schedule output of some buffer.
	///
	virtual void	ScheduleOutputBuffer( KUInt32 inBufferAddr, KUInt32 inSize ) = 0;
	
	///
	/// Start output.
	///
	virtual void	StartOutput( void ) = 0;

	///
	/// Stop output.
	///
	virtual void	StopOutput( void ) = 0;

	///
	/// Is output running?
	///
	virtual bool	OutputIsRunning( void ) = 0;

	///
	/// Method called to signal a change in the output volume.
	///
	virtual void	OutputVolumeChanged( void ) {}

	///
	/// Get output volume.
	///
	KUInt32			OutputVolume( void )
		{
			return mOutputVolume;
		}

    ///
    /// Get output volume in the range from 0.0 to 1.0
    ///
    float OutputVolumeNormalized();

	///
	/// Set output volume.
	///
	void			OutputVolume( KUInt32 inVolume )
		{
			mOutputVolume = inVolume;
			OutputVolumeChanged();
		}

protected:
	enum {
		kOutputVolume_Zero	= 0x80000000,
        kOutputVolume_Min	= 0xFFDDBD71, // Matt: was 0xFFE24EDA, but I found a lower value
		kOutputVolume_Max	= 0x00000000
	};
	
	enum {
		kNewtonBufferSizeInFrames	= 0x750,
		kNewtonBufferSize			= kNewtonBufferSizeInFrames * sizeof(KSInt16)
	};
	
	///
	/// Accessor on the interface to the log (may be null).
	///
	TLog*	GetLog( void ) const
		{
			return mLog;
		}

	///
	/// Accessor on the interface to the memory.
	///
	TMemory*	GetMemory( void ) const
		{
			return mMemory;
		}

	///
	/// Raise input interrupt to ask for more data.
	///
	void	RaiseInputInterrupt( void ) const;

	///
	/// Raise output interrupt to ask for more data.
	///
	void	RaiseOutputInterrupt( void ) const;

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TSoundManager( const TSoundManager& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TSoundManager& operator = ( const TSoundManager& inCopy );

	/// \name Variables
	TLog*				mLog;				///< Reference to the log.
	TInterruptManager*	mInterruptManager;	///< Reference to the interrupt mgr.
	TMemory*			mMemory;			///< Interface to the memory.
	KUInt32				mInputIntMask;		///< Input interrupt mask.
	KUInt32				mOutputIntMask;		///< Output interrupt mask.
	KUInt32				mOutputVolume;		///< Output sound volume.
};

#endif
		// _TSOUNDMANAGER_H

// ========================================= //
// TRANSACTION CANCELLED - FARECARD RETURNED //
// ========================================= //
