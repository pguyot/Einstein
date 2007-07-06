// ==============================
// File:			TJITCache.cp
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

#include <K/Defines/KDefinitions.h>
#include "TJITCache.h"

// STL
#include <utility>

// Einstein
#include "TMemory.h"
#include "TMMU.h"
#include "JIT.h"

using namespace std;

// -------------------------------------------------------------------------- //
// Constantes & globales.
// -------------------------------------------------------------------------- //
#define kTJITCacheStats	0

#if kTJITCacheStats
static KUInt32 gNbHits = 0;					///< Nombre de hits.
static KUInt32 gNbInvalidatePage = 0;		///< Nombre d'invalidations (pages)
static KUInt32 gNbInvalidateTLB = 0;		///< Nombre d'invalidations (tlb)
static KUInt32 gNbMiss = 0;					///< Nombre de ratŽs.
#endif

// -------------------------------------------------------------------------- //
//  * InsertInPMap( KUInt32, SEntry* )
// -------------------------------------------------------------------------- //
template<>
void
TJITCache<JITPageClass>::InsertInPMap( KUInt32 inPAddr, SEntry* inEntry )
{
	SEntry** theEntryPtr = GetPMapEntryPtr( inPAddr );
	inEntry->mNextPAEntry = *theEntryPtr;
	*theEntryPtr = inEntry;
	inEntry->mPhysicalAddress = inPAddr;
}

// -------------------------------------------------------------------------- //
//  * EraseFromPMap( SEntry* )
// -------------------------------------------------------------------------- //
template<>
void
TJITCache<JITPageClass>::EraseFromPMap( SEntry* inEntry )
{
	SEntry** theEntryPtr = GetPMapEntryPtr( inEntry->mPhysicalAddress );
	SEntry* theEntry = *theEntryPtr;
	SEntry* thePrevEntry = NULL;
	while (theEntry)
	{
		if (theEntry == inEntry)
		{
			if (thePrevEntry)
			{
				thePrevEntry->mNextPAEntry = theEntry->mNextPAEntry;
			} else {
				*theEntryPtr = theEntry->mNextPAEntry;
			}

			break;
		}
		
		thePrevEntry = theEntry;
		theEntry = theEntry->mNextPAEntry;
	}
}

// -------------------------------------------------------------------------- //
//  * LookupInPMap( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
template<>
TJITCache<JITPageClass>::SEntry*
TJITCache<JITPageClass>::LookupInPMap( KUInt32 inVAddr, KUInt32 inPAddr )
{
	SEntry** theEntryPtr = GetPMapEntryPtr( inPAddr );
	SEntry* theEntry = NULL;
	if (theEntryPtr)
	{
		theEntry = *theEntryPtr;
		while (theEntry)
		{
			if (theEntry->key == inVAddr)
			{
				break;
			}
			
			theEntry = theEntry->mNextPAEntry;
		}
	}
	return theEntry;
}

// -------------------------------------------------------------------------- //
//  * InitPMap( void )
// -------------------------------------------------------------------------- //
template<>
void
TJITCache<JITPageClass>::InitPMap( void )
{
	KUInt32 ramSize = mMemoryIntf->GetRAMSize();
	mPMapSize = (TMemoryConsts::kROMEnd + ramSize) / kPageSize;
	mPMap = (SEntry**) ::calloc(mPMapSize, sizeof(SEntry*));
}

// -------------------------------------------------------------------------- //
//  * DeletePMap( void )
// -------------------------------------------------------------------------- //
template<>
void
TJITCache<JITPageClass>::DeletePMap( void )
{
	::free(mPMap);
}

// -------------------------------------------------------------------------- //
//  * TJITCache( TMemory*, TMMU* )
// -------------------------------------------------------------------------- //
template<>
TJITCache<JITPageClass>::TJITCache(
		TMemory* inMemoryIntf,
		TMMU* inMMUIntf )
	:
		mMemoryIntf( inMemoryIntf ),
		mMMUIntf( inMMUIntf )
{
	InitPMap();
	
	// Init the entries.
	SEntry* theEntries = mVMap.GetValues();
	KUInt32 indexEntry;
	KUInt32 theAddress = 0;
	for (indexEntry = 0; indexEntry < THashMapCache<SEntry>::kCacheSize; indexEntry++) {
		SEntry* theEntry = &theEntries[indexEntry];
		theEntry->key = theAddress;
		theEntry->mPhysicalAddress = theAddress;
		theEntry->mPage.Init( inMemoryIntf, theAddress, theAddress);
		
		mVMap.Insert(theAddress, theEntry);
		InsertInPMap(theAddress, theEntry);
		theAddress += kPageSize;
	}
}

// -------------------------------------------------------------------------- //
//  * ~TJITCache( void )
// -------------------------------------------------------------------------- //
template<>
TJITCache<JITPageClass>::~TJITCache( void )
{
	DeletePMap();
}

// -------------------------------------------------------------------------- //
//  * PageMiss( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
template<>
JITPageClass*
TJITCache<JITPageClass>::PageMiss( KUInt32 inVAddr, KUInt32 inPAddr )
{
#if kTJITCacheStats
	gNbMiss++;
#endif

	SEntry** theEntryPtr = GetPMapEntryPtr( inPAddr );
	if (theEntryPtr == NULL)
	{
		return NULL;
	}

	// Take last page.
	SEntry* theEntry = mVMap.GetLastValue();
	
	// Remove it from tables.
	mVMap.Erase( theEntry->key );
	EraseFromPMap( theEntry );

	// Modify the entry.
	theEntry->mPage.Init( mMemoryIntf, inVAddr, inPAddr );
	theEntry->key = inVAddr;
	
	// Add it into the tables.
	mVMap.Insert(inVAddr, theEntry);
	InsertInPMap(inPAddr, theEntry);
	
	// Finally touch the entry.
	mVMap.MakeFirst( theEntry );
	
	return &theEntry->mPage;
}

// -------------------------------------------------------------------------- //
//  * GetPage( KUInt32 )
// -------------------------------------------------------------------------- //
template<>
JITPageClass*
TJITCache<JITPageClass>::GetPage( KUInt32 inVAddr )
{
#if kTJITCacheStats
	if ((gNbHits & 0xFF) == 0) {
		fprintf(
			stderr,
			"Hits: %i, Miss: %i, InvP: %i, InvT: %i\n",
			gNbHits,
			gNbMiss,
			gNbInvalidatePage,
			gNbInvalidateTLB);
	}
	gNbHits++;
#endif

	KUInt32 baseVAddr = inVAddr & kPageMask;
	SEntry* theEntry;
	KUInt32 thePAddr;

	// Get a page.
	// Let's look if we already have it in the cache.
	theEntry = mVMap.Lookup( baseVAddr );
	if (theEntry)
	{
		// Touch the entry.
		mVMap.MakeFirst(theEntry);
		return &theEntry->mPage;
	}

	if (mMMUIntf->IsMMUEnabled())
	{	
		// Resolve the address.
		if (mMMUIntf->TranslateInstruction(baseVAddr, &thePAddr)) {
			// An error occurred.
			return NULL;
		}
	} else {
		thePAddr = baseVAddr;
	}
	
	theEntry = LookupInPMap( baseVAddr, thePAddr );
	if (theEntry)
	{
		// If the VMap matches, only the mapping was altered.
		if (theEntry->key == baseVAddr) {
			// Re-branch the cache in the table of virtual adresses.
			mVMap.Insert(baseVAddr, theEntry);

			// Touch the entry.
			mVMap.MakeFirst(theEntry);
			return &theEntry->mPage;
		}
	}
	
	// The page is not in the cache.
	return PageMiss( baseVAddr, thePAddr );
}

// -------------------------------------------------------------------------- //
//  * InvalidateTLB( void )
// -------------------------------------------------------------------------- //
template<>
void
TJITCache<JITPageClass>::InvalidateTLB( void )
{
#if kTJITCacheStats
	gNbInvalidateTLB++;
#endif

	// Erase all bindings.
	mVMap.Clear();
}

// -------------------------------------------------------------------------- //
//  * InvalidatePage( KUInt32 )
// -------------------------------------------------------------------------- //
template<>
void
TJITCache<JITPageClass>::InvalidatePage( KUInt32 inPAddr )
{
	KUInt32 basePAddr = inPAddr & kPageMask;
	
	// Look for page(s).
	// Remove it/them from the tables.
	SEntry** theEntryPtr = GetPMapEntryPtr( basePAddr );
	SEntry* theEntry = *theEntryPtr;
#if kTJITCacheStats
	if (theEntry)
	{
		gNbInvalidatePage++;
	}
#endif
	while (theEntry)
	{
		// Erase the bindings.
		mVMap.Erase( theEntry->key );

		// Move the page at the end.
		mVMap.MakeLast(theEntry);

		// Next.
		SEntry* theNewEntry = theEntry->mNextPAEntry;
		theEntry->mNextPAEntry = NULL;
		theEntry = theNewEntry;
	}
	*theEntryPtr = NULL;
}

// ============================================================ //
// [We] use bad software and bad machines for the wrong things. //
//                 -- R.W. Hamming                              //
// ============================================================ //
