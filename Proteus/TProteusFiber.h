// ==============================
// File:			TProteusFiber.h
// Project:			Einstein
//
// Copyright 2019 by Matthais Melcher (proteus@matthiasm.com).
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


#ifndef T_PROTEUS_FIBER_H
#define T_PROTEUS_FIBER_H

#include "TProteusMacros.h"
#include "K/Threads/TFiber.h"


namespace NewtOS {


typedef void (*FPtr)();

enum {
	kFiberAbort = 1,
	kFiberIdle,
	kFiberCallNative,
	kFiberCallJIT,
	kFiberReturn,
	kFiberUnknown
};

class TProteusFiber : public TFiber
{
public:
	TProteusFiber() { }
	KSInt32 Task(KSInt32 inReason=0, void* inUserData=0L)
	{
		SwitchToJIT();
		return 0;
	}
	void SwitchToJIT(KUInt32 pc=0xFFFFFFFF) {
		LR = 0x007FFFF0;
		if (pc!=0xFFFFFFFF) PC = pc+4;
		for (;;) {
			FPtr fn;
			int reason = Suspend(kFiberCallJIT);
			switch (reason) {
				case kFiberReturn:
					return;
				case kFiberCallNative:
					fn = (FPtr)GetUserData();
					fn();
					break;
				default:
					fprintf(stderr, "ERROR: TSVCFiber::SwitchToJIT, unexpected Resume(%d)\n", reason);
			}
		}
	}
};

extern TProteusFiber *FindFiber();

extern TProteusFiber *svcFiber;

#define T_JIT_TO_NATIVE(addr, name) \
extern JITInstructionProto(p##addr); \
extern void P##addr(); \
TJITGenericPatchNativeInjection i##addr(addr, p##addr, name); \
JITInstructionProto(p##addr) { \
if (TProteusFiber *fiber = FindFiber()) { \
fiber->Resume(kFiberCallNative, (void*)P##addr); \
return nullptr; \
} else return ioUnit; \
} \
void P##addr()

} // namespace;

#endif /* T_PROTEUS_FIBER_H */

// ======================================================================= //
//                       - insert insane quote here -                      //
// ======================================================================= //
