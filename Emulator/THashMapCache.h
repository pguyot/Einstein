// ==============================
// File:			THashMapCache.h
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

#ifndef _THASHMAPCACHE_H
#define _THASHMAPCACHE_H

#include <K/Defines/KDefinitions.h>

// POSIX & ANSI C
#include <stdlib.h>
#include <string.h>

///
/// Single key hash-backed cache for virtual adresses.
/// This isn't a "real" hash-map:
/// - the hash function is extremely simple;
/// - collision are not handled here.
///
/// Instead, all entries are allocated here and are stored within a double
/// linked list. These links are used as a double end queue through
/// MakeFirst and MakeLast.
///
/// "Insert" actually stores a new entry as the head of the
/// given bucket, and "Erase" erases the entry at the bucket. Consequently,
/// "Lookup" may return NULL while the entry is still live. The entry should
/// be accessible from another structure, typically a map by physical
/// addresses.
///
///
template <class TValue>
class THashMapCache
{
public:
	///
	/// Initialization (links the values together)
	///
	inline THashMapCache( void );
	
	///
	/// Destruction.
	///
	inline ~THashMapCache( void );
	
	///
	/// Insert.
	///
	/// \param inKey		key.
	/// \param inValue		value.
	///
	inline void	Insert( KUInt32 inKey, TValue* inValue );

	///
	/// Erase.
	///
	/// \param inKey		key.
	///
	inline void	Erase( KUInt32 inKey );

	///
	/// Lookup.
	///
	/// \param inKey		key.
	/// \return the value or NULL if no matching value was found.
	///
	inline TValue*	Lookup( KUInt32 inKey );
	
	///
	/// Clear the map.
	///
	inline void		Clear( void );

	///
	/// Touch the value, i.e. make it first.
	///
	/// \param inValue		value to touch.
	///
	inline void MakeFirst( TValue* inValue );

	///
	/// Make an value the last.
	///
	/// \param inValue		value to move to last position.
	///
	inline void MakeLast( TValue* inValue );

	///
	/// Accessor on the values.
	/// (useful for initialization).
	///
	TValue*		GetValues( void )
		{
			return mValues;
		}

	///
	/// Accessor on the last entry (typically to move it first).
	///
	TValue*		GetLastValue( void )
		{
			return mLastValue;
		}

	enum {
		kCacheSize				= 128,
		kHashFunctionMask		= 0x000FFC00,
		kHashFunctionShift		= 10,
		kHashTableSize			= (kHashFunctionMask >> kHashFunctionShift) + 1,
	};
	
	///
	/// Hash function.
	///
	static inline KUInt32 HashFunction(const KUInt32 val)
		{
			return (val & kHashFunctionMask) >> kHashFunctionShift;
		}

private:
	/// \name Variables
	TValue*		mFirstValue;				///< First element.
	TValue*		mLastValue;					///< Last element.
	TValue		mValues[kCacheSize];		///< Values.
	TValue**	mHashTable;					///< Hash table.
};

// -------------------------------------------------------------------------- //
//  * THashMapCache( void )
// -------------------------------------------------------------------------- //
template<class TValue>
THashMapCache<TValue>::THashMapCache( void )
{
	// Init the map.
	mHashTable = (TValue**) ::calloc(kHashTableSize, sizeof(TValue*));

	// Link the values.
	mFirstValue = &mValues[0];
	mValues[0].prev = NULL;
	mValues[0].next = &mValues[1];
	KUInt32 indexValue;
	for (indexValue = 1; indexValue < (kCacheSize - 1); indexValue++) {
		TValue* theValue = &mValues[indexValue];
		theValue->prev = &mValues[indexValue - 1];
		theValue->next = &mValues[indexValue + 1];
	}
	mValues[kCacheSize - 1].prev = &mValues[kCacheSize - 2];
	mValues[kCacheSize - 1].next = NULL;
	mLastValue = &mValues[kCacheSize - 1];
}

// -------------------------------------------------------------------------- //
//  * ~THashMapCache( void )
// -------------------------------------------------------------------------- //
template<class TValue>
THashMapCache<TValue>::~THashMapCache( void )
{
	// Free the map.
	::free(mHashTable);
}

// -------------------------------------------------------------------------- //
//  * Insert( KUInt32, TValue* )
// -------------------------------------------------------------------------- //
template<class TValue>
void
THashMapCache<TValue>::Insert( KUInt32 inKey, TValue* inValue )
{
	KUInt32 index = HashFunction( inKey );
	mHashTable[index] = inValue;
}

// -------------------------------------------------------------------------- //
//  * Erase( TValue* )
// -------------------------------------------------------------------------- //
template<class TValue>
void
THashMapCache<TValue>::Erase( KUInt32 inKey )
{
	KUInt32 index = HashFunction( inKey );
	if (mHashTable[index] != NULL && mHashTable[index]->key == inKey) {
		mHashTable[index] = NULL;
	}
}

// -------------------------------------------------------------------------- //
//  * Lookup( KUInt32 )
// -------------------------------------------------------------------------- //
template<class TValue>
TValue*
THashMapCache<TValue>::Lookup( KUInt32 inKey )
{
	KUInt32 index = HashFunction( inKey );
	TValue* theEntry = mHashTable[index];
	if (theEntry && (theEntry->key == inKey))
	{
		return theEntry;
	}
	return NULL;
}

// -------------------------------------------------------------------------- //
//  * Clear( void )
// -------------------------------------------------------------------------- //
template<class TValue>
void
THashMapCache<TValue>::Clear( void )
{
	memset(mHashTable, 0, kHashTableSize * sizeof(TValue*));
}

// -------------------------------------------------------------------------- //
//  * MakeFirst( TValue* )
// -------------------------------------------------------------------------- //
template<class TValue>
void
THashMapCache<TValue>::MakeFirst( TValue* inValue )
{
	if (inValue != mFirstValue) {
		// Consider the old next.
		TValue* oldNextValue = inValue->next;
		TValue* oldPrevValue = inValue->prev;
		if (oldNextValue) {
			oldNextValue->prev = oldPrevValue;
		} else {
			mLastValue = oldPrevValue;
		}
		// There always is an old previous value.
		oldPrevValue->next = oldNextValue;

		// Move the new value at first.		
		TValue* oldFirstValue = mFirstValue;
		inValue->next = oldFirstValue;
		oldFirstValue->prev = inValue;
		inValue->prev = NULL;
		mFirstValue = inValue;
	} // Otherwise, do nothing.
}

// -------------------------------------------------------------------------- //
//  * MakeLast( TValue* )
// -------------------------------------------------------------------------- //
template<class TValue>
void
THashMapCache<TValue>::MakeLast( TValue* inValue )
{
	if (inValue != mLastValue) {
		// Consider the old prev.
		TValue* oldPrevValue = inValue->prev;
		TValue* oldNextValue = inValue->next;
		if (oldPrevValue) {
			oldPrevValue->next = oldNextValue;
		} else {
			mFirstValue = oldNextValue;
		}
		// There always is an old next value.
		oldNextValue->prev = oldPrevValue;

		// Move the new value to last position.
		TValue* oldLastValue = mLastValue;
		inValue->prev = oldLastValue;
		oldLastValue->next = inValue;
		inValue->next = NULL;
		mLastValue = inValue;
	} // Otherwise, do nothing.
}

#endif
		// _THASHMAPCACHE_H

// ========================================================================== //
// There is is no reason for any individual to have a computer in their home. //
//                 -- Ken Olsen (President of Digital Equipment Corporation), //
//                    Convention of the World Future Society, in Boston, 1977 //
// ========================================================================== //
