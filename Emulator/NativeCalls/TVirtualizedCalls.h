// ==============================
// File:			TVirtualizedCalls.h
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

#ifndef _TVIRTUALIZEDCALLS_H
#define _TVIRTUALIZEDCALLS_H

#include <K/Defines/KDefinitions.h>

class TEmulator;
class TMemory;
class TARMProcessor;

///
/// Class for virtualized calls.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TVirtualizedCalls
{
public:
	///
	/// Constructor from the emulator, the interface to memory and the processor.
	///
	TVirtualizedCalls(
		TEmulator* inEmulator,
		TMemory* inMemoryIntf,
		TARMProcessor* inProcessor );

	///
	/// Destructor.
	///
	~TVirtualizedCalls( void );

	///
	/// Execute an instruction.
	///
	void Execute( KUInt32 inInstruction );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TVirtualizedCalls( const TVirtualizedCalls& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TVirtualizedCalls& operator = ( const TVirtualizedCalls& inCopy );

	///
	/// __rt_sdiv
	///
	inline void __rt_sdiv( void );

	///
	/// __rt_udiv
	///
	inline void __rt_udiv( void );

	///
	/// memmove
	///
	inline void memmove( void );
	inline void memmove_slow( KUInt32 inDst, KUInt32 inSrc, KUInt32 inLen );

	///
	/// symcmp__FPcT1
	///
	inline void symcmp__FPcT1( void );

	/// \name Variables
	TEmulator*		mEmulator;		///< Interface to the emulator.
	TMemory*		mMemoryIntf;	///< Interface to memory.
	TARMProcessor*	mProcessor;		///< Interface to the processor.
};

#endif
		// _TVIRTUALIZEDCALLS_H

// ========================================================================= //
// A recent study has found that concentrating on difficult off-screen       //
// objects, such as the faces of loved ones, causes eye strain in computer   //
// scientists.  Researchers into the phenomenon cite the added concentration //
// needed to "make sense" of such unnatural three dimensional objects.       //
// ========================================================================= //
