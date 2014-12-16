// ==============================
// File:			TJITPage.h
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

#ifndef _TJITPAGE_H
#define _TJITPAGE_H

#include <K/Defines/KDefinitions.h>

// Einstein
#include "TMemoryConsts.h"

template< class TImplementation, class TPage > class TJIT;
class TMemory;

///
/// Page for emulation.
/// A page has:
/// - a physical address
/// - a logical address
/// - a size (depending on MMU settings)
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 150 $
///
/// \test	aucun test défini.
///
template< class TImplementation, class TPage >
class TJITPage
{
public:
	///
	/// Access from TJIT
	///
	friend class TJIT< TImplementation, TPage >;

	enum {
		kPageSize = TMemoryConsts::kMMUSmallestPageSize,
		kPageMask = TMemoryConsts::kMMUSmallestPageMask,
	};

	///
	/// Default constructor.
	///
	TJITPage( void );

	///
	/// Destructor.
	///
	~TJITPage( void );

	///
	/// Init with the memory interface, a virtual address and a physical address.
	///
	void Init(
			TMemory* inMemoryIntf,
			KUInt32 inVAddr,
			KUInt32 inPAddr );

	///
	/// Accessor on the virtual address of the page.
	///
	/// \return the virtual address of the page.
	///
	inline KUInt32 GetVAddr( void ) const
		{
			return mVAddr;
		}

	///
	/// Accessor on the physical address of the page.
	///
	/// \return the physical address of the page.
	///
	inline KUInt32 GetPAddr( void ) const
		{
			return mPAddr;
		}

	///
	/// Accessor on an instruction in the page.
	///
	/// \param inOffset	offset of the instruction (address / 4)
	/// \return the instruction.
	///
	inline KUInt32 GetInstruction( KUInt32 inOffset ) const
		{
			return mPointer[inOffset];
		}

protected:
	///
	/// Accessor on the pointer.
	///
	inline KUInt32* GetPointer( void )
		{
			return mPointer;
		}

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TJITPage( const TJITPage& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TJITPage& operator = ( const TJITPage& inCopy );

	/// \name Variables
	KUInt32*	mPointer;	///< Pointer to instructions, null if the page
							///< is dirty.
	KUInt32		mVAddr;		///< Virtual address of the page.
	KUInt32		mPAddr;		///< Physical address of the page.
};

#endif
		// _TJITPAGE_H

// ===================================================================== //
// The New Testament offers the basis for modern computer coding theory, //
// in the form of an affirmation of the binary number system.            //
//                                                                       //
//         But let your communication be Yea, yea; nay, nay:             //
//         for whatsoever is more than these cometh of evil.             //
//                 -- Matthew 5:37                                       //
// ===================================================================== //
