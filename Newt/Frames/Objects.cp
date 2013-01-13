// ==============================
// File:			Newt/Frames/Objects.cp
// Project:			Einstein
//
// Copyright 2012 by Newton Research Group and others
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

#include "Objects.h"

ObjectHeader *NoFaultObjectPtr()
{
	ObjectHeader *r;
	NEWT_PUSH_REGISTERS
	NewtCallJIT(0x0031ddac);
	r = (ObjectHeader *) gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return r;
}

RefHandle* AllocateRefHandle(Ref r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	NewtCallJIT(0x0031D26C);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return (RefHandle*)ret;
}

KUInt32 DisposeRefHandle(RefHandle* r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	NewtCallJIT(0x0031D2B0);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}

Ref FollowFaultBlock(RefHandle **r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, (KUInt32)r0);
	NewtCallJIT(0x002e01d8);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return ret;
}

ObjectHeader* ResolveMagicPtr(Ref r0)
{
	KUInt32 ret;
	NEWT_PUSH_REGISTERS
	gCurrentCPU->SetRegister(0, r0);
	NewtCallJIT(0x0031DAD4);
	ret = gCurrentCPU->GetRegister(0);
	NEWT_POP_REGISTERS
	return (ObjectHeader*)ret;
}

Ref ForwardReference1(ForwardingObject *f)
{
	ForwardingObject *orig = f;
	bool error = false;
	Ref next;

	do {
		next = f->GetForwardRef();
		if (!ISPTR(next)) {
			next = MAKEPTR(NoFaultObjectPtr());
			error = true;
		}
		f = PTRFROMREF(ForwardingObject, next);
	} while (!error && f->GetFlags() & kRefFlagForward);

	if (!error) {
		orig->SetForwardRef(next);
	}
	return next;
}

Ref ForwardReference(Ref f)
{
	if (ISPTR(f) && PTRFROMREF(ObjectHeader, f)->GetFlags() & kRefFlagForward) {
		f = ForwardReference1(PTRFROMREF(ForwardingObject, f));
	}
	return f;
}

ForwardingObject *ObjectPtr1(Ref ref, long type, int handleFault);

ForwardingObject* ObjectPtr(Ref inRef)
{
	ForwardingObject *r = NULL;
	long type = inRef & kRefTagMask;

	if (type == kTagPointer) { // pointer
		if ((inRef < 0x3800000) || (inRef >= 0x60000000 && inRef < 0x68000000)) {
			// in ROM of packet memory? Directly convert into a pointer
			r = PTRFROMREF(ForwardingObject, inRef);
		} else {
			// Cached?
			if (inRef == GlobalGetCacheObjPtrRef()) {
				r = (ForwardingObject *) GlobalGetCacheObjPtrPtr();
			} else {
				r = ObjectPtr1(inRef, type, 0);
			}
		}
	} else if (type == kTagMagicPtr) {
		r = (ForwardingObject*) ResolveMagicPtr(inRef);
	}
	return r;
}

ForwardingObject *ObjectPtr1(Ref ref, long type, int ignoreFault)
{
	ForwardingObject *r = NULL;
	if (type == kTagPointer) {
		GlobalSetCacheObjPtrRef(ref);
		r = PTRFROMREF(ForwardingObject, ForwardReference(ref));
		if (r->GetFlags() & kTagPointer && r->GetForwardRef() == 0x22) {
			GlobalSetCacheObjPtrRef(0);
			Ref f = r->GetUnknown_18();
			if (f != NILREF) {
				r = ObjectPtr(f);
			} else if (!ignoreFault) {
				NEWT_LOCAL(RefHandle *, h);
				NEWT_LOCAL_SET_W(h, (KUInt32)AllocateRefHandle(MAKEPTR(r)));
				r = ObjectPtr(FollowFaultBlock(NEWT_LOCAL_PTR(h)));
				DisposeRefHandle(NEWT_LOCAL_GET_W(RefHandle *, h));
			}
		} else {
			GlobalSetCacheObjPtrPtr(r);
		}
	} else if (ref == 0x42) {
		printf("*** ObjectPtr: Error -48221: reference to deactivated package! ***\n");
	} else {
		printf("*** ObjectPtr: Error -48200: Expected frame, array or binary object ***\n");
	}
	return r;
}

NEWT_INJECTION(0x0031d524, "ForwardReference(Ref)") {
	NEWT_RETVAL ForwardReference(NEWT_ARG0(Ref));
	NEWT_RETURN;
}

NEWT_INJECTION(0x0031DD54, "ObjectPtr(Ref)") {
	NEWT_RETVAL ObjectPtr(NEWT_ARG0(Ref));
	NEWT_RETURN;
}
