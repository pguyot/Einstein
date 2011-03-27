// ==============================
// File:			JITPerformance.cp
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

#include "TJITPerformance.h"

#include <stdarg.h>


#ifdef JIT_PERFORMANCE

TJITPerfHitCounter branchDestCount(0, 8*1024*1024-1, 4);

TJITPerfHitCounter branchLinkDestCount(0, 8*1024*1024-1, 4);;

#endif


TJITPerfHitCounter::TJITPerfHitCounter(KUInt32 first, KUInt32 last, KUInt32 step)
:	mFirst(0),
	mSize(0),
	mShift(0),
	mArray(0L)
{
	mFirst = first;
	switch (step) {
		case 2: mShift = 1; break;
		case 4: mShift = 2; break;
		default: mShift = 0; break;
	}
	mSize = (last+1-first)>>mShift;
	mArray = new KUInt64[mSize];
}


TJITPerfHitCounter::~TJITPerfHitCounter()
{
	delete[] mArray;
}


KUInt64 TJITPerfHitCounter::get_hits(KUInt32 at)
{
	at = (at-mFirst)>>mShift;
	if (at>mSize) 
		return 0;
	return mArray[at];
}


//	kStyleAToB = 0,	///< show all values from a to b; varargs are UInt32 a, UInt32 b
//	kStyleMostHit,	///< show the first n addresses that were hit most often; varargs is UInt32 n
//	kStyleHex = 0x10000000 ///< show addresse in 8 byte hex notation instead of decimal

void TJITPerfHitCounter::print(FILE *out, KUInt32 style, ...)
{
	va_list vl;
	va_start(vl, style);
	KUInt32 a, b, i, j, n, o, ix;
	KUInt64 m;

	fprintf(out, "----- statistics ----\n");

	switch (style & 0x0000ffff) {
		case kStyleAToB:
			a = va_arg( vl, KUInt32 );
			b = va_arg( vl, KUInt32 );
			o = a; 
			a = (a-mFirst)>>mShift; // TODO: check bounds
			b = (b-mFirst)>>mShift;
			for (i=a; i<b; i++) {
				KUInt64 v = mArray[i];
				if (style & kStyleHex) {
					fprintf(out, "%08X: %19llu\n", (unsigned int)((i<<mShift)+o), v);
				} else {
					fprintf(out, "%8d: %19llu\n", (unsigned int)((i<<mShift)+o), v);
				}
			}
			break;
		case kStyleAllHit:
			n = 0;
			for (j=0; j<mSize; j++) {
				KUInt64 v = mArray[j];
				if (v>0) 
					n++;
			}
		case kStyleMostHit:
			// TODO: using an insanely slow and destructive method to sort
			if ((style & 0x0000ffff)==kStyleMostHit)
				n = va_arg( vl, KUInt32 );
			for (i=0; i<n; i++) {
				m = 0; ix = 0;
				for (j=0; j<mSize; j++) {
					KUInt64 v = mArray[j];
					if (v>m) {
						m = v;
						ix = j;
					}
				}
				if (m==0)
					break;
				mArray[ix] = 0;
				if (style & kStyleHex) {
					fprintf(out, "%08X: %19llu\n", (unsigned int)((ix<<mShift)+mFirst), m);
				} else {
					fprintf(out, "%8d: %19llu\n", (unsigned int)((ix<<mShift)+mFirst), m);
				}
			}
			break;
	}
    va_end( vl );
}


void TJITPerfHitCounter::hit(KUInt32 at)
{
    KUInt64 maxULLInt = 0xffffffffffffffffULL;
	at = (at-mFirst)>>mShift;
	if (at>mSize) 
		return;
	if (mArray[at]<maxULLInt)
		mArray[at]++;
}





