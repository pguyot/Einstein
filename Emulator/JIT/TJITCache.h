// ==============================
// File:			TJITCache.h
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

#ifndef _TJITCACHE_H
#define _TJITCACHE_H

#include <K/Defines/KDefinitions.h>

// Einstein
#include "TMemoryConsts.h"
#include "THashMapCache.h"

// Google Dense Hash Map
//#include "google/dense_hash_map"

// STL/GCC ext.
#if ( TARGET_OS_WIN32 || TARGET_OS_ANDROID )
	#include <hash_map>
#else
	#include <ext/hash_map>
#endif

class TMemory;
class TMMU;

///
/// Class for the JIT cache.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
template< class TPage >
class TJITCache
{
public:
	///
	/// Constructor from the memory and the MMU interfaces.
	///
	TJITCache(
		TMemory* inMemoryIntf,
		TMMU* inMMUIntf );

	///
	/// Destructor.
	///
	~TJITCache( void );
	
	///
	/// From a virtual address, return a new page or NULL if a translation
	/// error occurred (typically a MMU problem).
	///
	/// \param inVAddr	virtual address of the instruction belonging to the
	///					required page.
	///
	TPage*		GetPage( KUInt32 inVAddr );
	
	///
	/// Get the offset of an instruction in page.
	///
	/// \param inVAddr	virtual address of the instruction.
	///
	KUInt32		GetOffsetInPage( KUInt32 inVAddr )
		{
			return inVAddr & kOffsetMask;
		}
	
	///
	/// Invalidate all translations.
	///
	void		InvalidateTLB( void );

	///
	/// Invalidate a page by physical address.
	///
	/// \param inPAddr	physical address of the modified word.
	///
	void		InvalidatePage( KUInt32 inPAddr );

protected:
	struct SEntry {
		TPage			mPage;
		KUInt32			mPhysicalAddress;
		KUInt32			key;
		SEntry*			next;
		SEntry*			prev;
		SEntry*			mNextPAEntry;
	};

private:
	enum {
		kPageSize	= TMemoryConsts::kMMUSmallestPageSize,
		kPageMask	= TMemoryConsts::kMMUSmallestPageMask,
		kOffsetMask	= TMemoryConsts::kMMUSmallestPageMaskNeg,
	};

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TJITCache( const TJITCache& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TJITCache& operator = ( const TJITCache& inCopy );

	///
	/// Page miss, get oldest page and page it in.
	///
	/// \param inVAddr		new virtual address.
	/// \param inPAddr		new physical address.
	///
	inline TPage* PageMiss( KUInt32 inVAddr, KUInt32 inPAddr );

	///
	/// Insert into PMap.
	///
	/// \param inPAddr		physical address.
	/// \param inEntry		entry.
	///
	inline void	InsertInPMap( KUInt32 inPAddr, SEntry* inEntry );

	///
	/// Erase from PMap.
	///
	/// \param inEntry		entry.
	///
	inline void	EraseFromPMap( SEntry* inEntry );

	///
	/// Lookup in PMap.
	///
	/// \param inVAddr		virtual address.
	/// \param inPAddr		physical address.
	/// \return the entry or NULL if no matching entry was found.
	///
	inline SEntry*	LookupInPMap( KUInt32 inVAddr, KUInt32 inPAddr );

	///
	/// Init PMap.
	///
	void	InitPMap( void );

	///
	/// Delete PMap.
	///
	void	DeletePMap( void );

	///
	/// Get an index in PMap table.
	///
	SEntry**	GetPMapEntryPtr( KUInt32 inPAddr ) const
		{
			if (inPAddr & TMemoryConsts::kROMEndMask) {
				KUInt32 theOffset = (inPAddr
					+ TMemoryConsts::kHighROMEnd
					- TMemoryConsts::kRAMStart) / kPageSize;
				if (theOffset < mPMapSize)
				{
					return &mPMap[theOffset];
				} else {
					return NULL;
				}
			} else {
				return &mPMap[inPAddr / kPageSize];
			}
		}

	/// \name Variables
	TMemory*				mMemoryIntf;			///< Interface to memory.
	TMMU*					mMMUIntf;				///< Interface to MMU.
	THashMapCache<SEntry>	mVMap;					///< Cache.
	SEntry**				mPMap;					///< Association by
													///< physical address.
	KUInt32					mPMapSize;				///< Size of the PMap.
};

#endif
		// _TJITCACHE_H

// =================================================== //
// Staff meeting in the conference room in %d minutes. //
// =================================================== //
