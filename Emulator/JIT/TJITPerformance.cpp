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

#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"
#include "Monitor/TSymbolList.h"

#include <stdarg.h>
#if TARGET_OS_WIN32
#else
#include <unistd.h>
#endif
#include <chrono>
#include <thread>



#ifdef JIT_PERFORMANCE

TJITPerfHitCounter branchDestCount(0, 8*1024*1024-1, 4);

TJITPerfHitCounter branchLinkDestCount(0, 8*1024*1024-1, 4);;

#endif


TJITPerfHitCounter::TJITPerfHitCounter(KUInt32 first, KUInt32 last, KUInt32 step)
:	mEmulator(0L),
	mFirst(0),
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


void TJITPerfHitCounter::SetEmulator(TEmulator *inEmulator)
{
	mEmulator = inEmulator;
}



KUInt64 TJITPerfHitCounter::get_hits(KUInt32 at)
{
	at = (at-mFirst)>>mShift;
	if (at>mSize) 
		return 0;
	return mArray[at];
}


void TJITPerfHitCounter::printOneHit(FILE *out, KUInt32 style, TSymbolList *inSymbols, KUInt32 addr, KUInt64 count)
{
	char symbol[512];
	char comment[80];
	int offset;
	Boolean exactSymbol = false;

	if (count > 0 || !(style & kStyleNonZeroOnly)) {
		if (inSymbols != NULL) exactSymbol = inSymbols->GetSymbolByAddress(addr, symbol, comment, &offset);
		if (exactSymbol) {
			fprintf(out, "%s\t%19llu\n", symbol, count);
		} else if ((style & kStyleSymbolsOnly) == 0) {
			if (style & kStyleHex) {
				fprintf(out, "%08X: ", (unsigned int)addr);
			} else {
				fprintf(out, "%8d: ", (unsigned int)addr);
			}
			fprintf(out, "%19llu\n", count);
		}
	}
}

//	kStyleAToB = 0,	///< show all values from a to b; varargs are UInt32 a, UInt32 b
//	kStyleMostHit,	///< show the first n addresses that were hit most often; varargs is UInt32 n
//	kStyleHex = 0x10000000 ///< show addresse in 8 byte hex notation instead of decimal

void TJITPerfHitCounter::print(FILE *out, KUInt32 style, TSymbolList *inSymbols, ...)
{
	if ( !out )
		return;
	
	if (mEmulator) {
		mEmulator->PauseSystem();
		mEmulator->GetInterruptManager()->SuspendTimer();
		std::this_thread::sleep_for(std::chrono::microseconds(1000000));
	}
		
	va_list vl;
	va_start(vl, inSymbols);
    static KUInt64 maxULLInt = 0xffffffffffffffffULL;
	KUInt32 a, b, i, j, n = 0, o, ix;
	KUInt64 m = 0;

	//TInterruptManager;;blockEmulatorThread();
	fprintf(out, "----- statistics ----\n");

	switch (style & 0x0000ffff) {
		case kStyleDontSort:
			a = 0; b = 0x00800000;
			goto AToB;
		case kStyleAToB:
			a = va_arg( vl, KUInt32 );
			b = va_arg( vl, KUInt32 );
		AToB:
			o = a;
			a = (a-mFirst)>>mShift; // TODO: check bounds
			b = (b-mFirst)>>mShift;
			for (i=a; i<b; i++) {
				KUInt64 v = mArray[i];
				printOneHit(out, style, inSymbols, (i<<mShift)+o, v);
			}
			break;
		case kStyleAllHit:
			for (j=0; j<mSize; j++) {
				KUInt64 v = mArray[j];
				if (v>0) {
					printOneHit(out, style, inSymbols, (j<<mShift)+mFirst, m);
				}
			}
		case kStyleMostHit:
			// TODO: using an insanely slow and destructive method to sort
			if ((style & 0x0000ffff)==kStyleMostHit)
				n = va_arg( vl, KUInt32 );
			else 
				n = mSize;
			o = 0; 
			m = 0;
			for (i=0; i<mSize; i++) {
				if (mArray[i]>0) {
					o++;
					m += mArray[i];
				}
			}
			fprintf(out, "%ld of %li memory addresses in %lld cycles executed.\n", (unsigned long)o, (unsigned long)mSize, m);
			for (i=0; i<n; i++) {
				m = 0; ix = 0;
				for (j=0; j<mSize; j++) {
					KUInt64 v = mArray[j];
					if (v==maxULLInt) 
						continue;
					if (v>m) {
						m = v;
						ix = j;
					}
				}
				if (m==0)
					break;
				mArray[ix] = maxULLInt;
				printOneHit(out, style, inSymbols, (ix<<mShift)+mFirst, m);
			}
			break;
	}
    va_end( vl );
	
	if (mEmulator) {
		mEmulator->GetInterruptManager()->ResumeTimer();
		mEmulator->Run();
	}
}


void TJITPerfHitCounter::hit(KUInt32 at)
{
    static KUInt64 maxULLInt = 0xffffffffffffffffULL;
	at = (at-mFirst)>>mShift;
	if (at>mSize) 
		return;
	if (mArray[at]<maxULLInt)
		mArray[at]++;
}





