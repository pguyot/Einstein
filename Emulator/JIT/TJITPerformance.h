// ==============================
// File:			JITPerformance.h
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
//                     and Matthias Melcher (m.melcher@robowerk.com)
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

#ifndef _TJITPERFORMANCE_H
#define _TJITPERFORMANCE_H

#include <K/Defines/KDefinitions.h>
#include <stdio.h>


class TEmulator;
class TSymbolList;


///
/// Implement a fast hit counter.
///
/// This class is helpful to count any kind of array access. It was written 
/// to count the command executions in the ROM, but can be used for many
/// other purposes, like hash buffer performance measurments.
///
class TJITPerfHitCounter {
public:
	/// Create a hit counter setup.
	/// \param first lowest value that will be counted, lower values will be ignored
	/// \param last highest value that will be counted
	/// \param step count only values that divide by step; legal values are 1, 2, and 4
	TJITPerfHitCounter(KUInt32 first, KUInt32 last, KUInt32 step=1);

	/// free all associated resources
	~TJITPerfHitCounter();
  
    void SetEmulator(TEmulator *inEmulator);

	/// Count a hit at the given index
	/// \param at index into hit counter array; out-of-bounds indices are safe
	void		hit(KUInt32 at);

	/// Return the number of hits for a given index
	/// \param at the index of the counter that we want to inspect
	KUInt64		get_hits(KUInt32 at);

	/// Print the collected data to a file
	/// \param out a destination file that the caller must manage
	/// \param style one of the output styles defined below
	/// \param inSymbols optional symbol file used to map hits to symbols
	void		print(FILE *out, KUInt32 style, TSymbolList *inSymbols = NULL, ...);

	enum {
		kStyleAToB = 0,		///< show all values from a to b; varargs are UInt32 a, UInt32 b
		kStyleMostHit,		///< show the first n addresses that were hit most often; varargs is UInt32 n
		kStyleAllHit,		///< show all addresses that were hit, sorted by number of hit; varargs is unused
		kStyleDontSort,		///< list all addresses that were hit
		kStyleHex = 0x10000000, ///< show addresse in 8 byte hex notation instead of decimal
		kStyleSymbolsOnly = 0x20000000,	///< show only hits for symbols
		kStyleNonZeroOnly = 0x40000000	///< print only hits greater than zero
	};

private:
	TEmulator	*mEmulator;
	KUInt32		mFirst;
	KUInt32		mSize;
	KUInt32		mShift;
	KUInt64		*mArray;

	/// Print one hit according to the style and optional symbol information
	/// \param out a destination file that the caller must manage
	/// \param style one of the output styles defined below
	/// \param inSymbols symbol file used to map hits to symbols (can be NULL)
	/// \param addr address to print
	/// \param count hits
	void		printOneHit(FILE *out, KUInt32 style, TSymbolList *inSymbols, KUInt32 addr, KUInt64 count);
};


extern TJITPerfHitCounter branchDestCount;
extern TJITPerfHitCounter branchLinkDestCount;


#endif
// _JITPERFORMANCE_H

// ==============================
