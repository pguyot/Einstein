// ==============================
// File:			TJITGeneric.h
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

#ifndef _TJITGENERIC_H
#define _TJITGENERIC_H

#include <K/Defines/KDefinitions.h>
#include "TJIT.h"

// Einstein
#include "TJITGenericPage.h"

class TMemory;
class TARMProcessor;
union JITUnit;

///
/// Class for generic JIT interface.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 150 $
///
/// \test	aucun test défini.
///
class TJITGeneric
	:
		public TJIT< TJITGenericPage >
{
public:
	///
	/// Constructor from interfaces to memory and MMU.
	///
	/// \param inMemoryIntf	interface to memory.
	/// \param inMMUIntf	interface to the MMU.
	///
	TJITGeneric(
		TMemory* inMemoryIntf,
		TMMU* inMMUIntf );
	
	///
	/// Destructor.
	///
	virtual ~TJITGeneric( void );
	
	///
	/// One or more steps with JIT.
	///
	/// \param ioObject			ARM CPU.
	/// \param inCount			number of instructions to execute.
	///
	virtual void	Step(
						TARMProcessor* ioObject,
						KUInt32 inCount );

	///
	/// Run with JIT.
	///
	/// \param ioObject			ARM CPU.
	/// \param inSignal			pointer to a signal set to false to stop
	///							execution.
	///
	virtual void	Run(
						TARMProcessor* ioObject,
						volatile KUInt32* inSignal );

	///
	/// Get a JIT unit for a given PC.
	///
	JITUnit* GetJITUnitForPC(
					TARMProcessor* ioCPU,
					TMemory* inMemoryInterface,
					KUInt32 inPC );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TJITGeneric( const TJITGeneric& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TJITGeneric& operator = ( const TJITGeneric& inCopy );

	enum {
		kPoolSize = 512,	///< 512 pages in pool.
	};
	
	/// \name Variables
	
	TJITGenericPage*	mPagesPool;	///< Array with all the pages.
};

#endif
		// _TJITGENERIC_H

// ====================================================================== //
// A complex system that works is invariably found to have evolved from a //
// simple system that works.                                              //
// ====================================================================== //
