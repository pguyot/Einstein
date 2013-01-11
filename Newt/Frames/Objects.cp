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

NEWT_INJECTION(0x0031d4d4, "ForwardReference1(ForwardingObject *)") {
	NEWT_RETVAL ForwardReference1(NEWT_ARG0(ForwardingObject *));
	NEWT_RETURN;
}

NEWT_INJECTION(0x0031d524, "ForwardReference(Ref)") {
	NEWT_RETVAL ForwardReference(NEWT_ARG0(Ref));
	NEWT_RETURN;
}
