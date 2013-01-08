// ==============================
// File:			TSymbolList.h
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

#ifndef _TSYMBOLLIST_H
#define _TSYMBOLLIST_H

#include <K/Defines/KDefinitions.h>

#include <stdio.h>

///
/// Class for a list of symbols. The symbol file format is:
/// addr name[\tcommment]
/// where addr is a 32 bit hex number, and the optional comment is preceeded
/// by a tab character. The file sorted by addr.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 113 $
///
/// \test	aucun test défini.
///
class TSymbolList
{
public:
	///
	/// Constructor from a path.
	///
	TSymbolList( const char* inSymbolPath );

	///
	/// Destructor.
	///
	~TSymbolList( void );

	///
	/// Accessor to a symbol.
	///
	void GetSymbol(
			KUInt32 inValue,
			char* outSymbol,
			char* outComment,
			int* outOffset );

private:
	struct SSymbolStruct
	{
		KUInt32	fSymbolValue;
		fpos_t	fFileCursor;
	};

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TSymbolList( const TSymbolList& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TSymbolList& operator = ( const TSymbolList& inCopy );

	///
	/// Load symbols.
	///
	void LoadSymbols( void );

	/// \name Variables
	SSymbolStruct*		mSymbolOffsets;
	KUInt32				mSymbolCount;
	FILE*				mFile;
};

#endif
		// _TSYMBOLLIST_H

// ============================================================================== //
//         The programmers of old were mysterious and profound.  We cannot fathom //
// their thoughts, so all we do is describe their appearance.                     //
//         Aware, like a fox crossing the water.  Alert, like a general on the    //
// battlefield.  Kind, like a hostess greeting her guests. Simple, like uncarved  //
// blocks of wood.  Opaque, like black pools in darkened caves.                   //
//         Who can tell the secrets of their hearts and minds?                    //
//         The answer exists only in the Tao.                                     //
//                 -- Geoffrey James, "The Tao of Programming"                    //
// ============================================================================== //
