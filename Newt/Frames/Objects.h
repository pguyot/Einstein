// ==============================
// File:			Newt/Frames/Objects.h
// Project:			Einstein
//
// Copyright 1999-2012 by Newton Research Group and others
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


#ifndef NEWT_FRAMES_OBJECTS_H
#define NEWT_FRAMES_OBJECTS_H


#include "Newt/Common/Newt.h"


/// the lowest two bit of a Ref define the type of the Ref
const int kRefTagBits = 2;
const int kRefValueBits = (sizeof(Ref)*4)-kRefTagBits;
const Ref kRefValueMask = (-1 << kRefTagBits);
const Ref kRefTagMask = ~kRefValueMask;

const int kRefImmedBits = 2;
const Ref kRefImmedMask = (-1 << kRefImmedBits);

const int kRefFlagForward = 0x20;

enum
{
	kTagInteger,
	kTagPointer,
	kTagImmed,
	kTagMagicPtr
};

enum
{
	kImmedSpecial,
	kImmedChar,
	kImmedBoolean,
	kImmedReserved
};



#define MAKEINT(i)			(((Ref)(i)) << kRefTagBits)
#define MAKEIMMED(t, v)		((((((Ref)(v)) << kRefImmedBits) | ((Ref)(t))) << kRefTagBits) | kTagImmed)
#define MAKECHAR(c)			MAKEIMMED(kImmedChar, (unsigned)c)
#define MAKEBOOLEAN(b)		(b ? TRUEREF : FALSEREF)
#define MAKEPTR(p)			((Ref)((char*)p + 1))
#define MAKEMAGICPTR(index)	((Ref)(((Ref)(index)) << kRefTagBits) | kTagMagicPtr)

#define PTRFROMREF(c, r)	((c *)((char *)r - 1))

#define NILREF				MAKEIMMED(kImmedSpecial, 0)
#define TRUEREF				MAKEIMMED(kImmedBoolean, 1)
#define FALSEREF			NILREF
#define INVALIDPTRREF		MAKEINT(0)

#define ISINT(a)			(((a)&kRefTagMask)==kTagInteger)
#define ISPTR(a)			(((a)&kRefTagMask)==kTagPointer)


class ObjectHeader
{
public:
	KUInt32 GetSize() { return GetSizeAndFlags()>>8; }
	KUInt32 GetType() { return GetSizeAndFlags() & 3; }
	KUInt32 GetFlags() { return GetSizeAndFlags() & 255; }
public:
	NEWT_GET_SET_W(KUInt32, SizeAndFlags);
	NEWT_GET_SET_W(KUInt32, LocksAndSlots);
};


class ArrayObject : public ObjectHeader
{
public:
	KUInt32 GetNumSlots() { return (GetSize()-11)>>2; }
public:
	NEWT_GET_SET_W(Ref, ObjectClass);
	NEWT_GET_SET_W_ARRAY(Ref, Slot, 1);
};

class ForwardingObject: public ObjectHeader
{
public:
	NEWT_GET_SET_W(Ref, ForwardRef);
	NEWT_GET_SET_W(Ref, Unknown_0c);
	NEWT_GET_SET_W(Ref, Unknown_10);
	NEWT_GET_SET_W(Ref, Unknown_14);
	NEWT_GET_SET_W(Ref, Unknown_18);
};

#endif	// NEWT_FRAMES_OBJECTS_H

