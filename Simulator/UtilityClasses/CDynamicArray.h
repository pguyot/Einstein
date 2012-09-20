// ==============================
// File:			CDynamicArray.h
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


#ifndef C_DYNAMIC_ARRAY_H
#define C_DYNAMIC_ARRAY_H


#include "Sim.h"


class CArrayIterator;


class CDynamicArray : public SingleObject
{
public:
//
//		enum Parameters {
//			kDefaultElementSize = 4,
//			kDefaultChunkSize = 4
//		};
//		
//		CDynamicArray();
//		CDynamicArray(Size elementSize, ArrayIndex chunkSize);
//		
//		~CDynamicArray();
//		
//		// array manipulation primitives
//		
//		ArrayIndex	GetArraySize(void);
//		NewtonErr	SetArraySize(ArrayIndex theSize);
//		NewtonErr	SetElementCount(ArrayIndex theSize);		// like SetArraySize, but sets logical size, too
//		
//		void*		ElementPtrAt(ArrayIndex index);
//		void*		SafeElementPtrAt(ArrayIndex index);
//		NewtonErr	GetElementsAt(ArrayIndex index, void* elemPtr, ArrayIndex count);
//		NewtonErr	InsertElementsBefore(ArrayIndex startHere, void* elemPtr, ArrayIndex count);
//		NewtonErr	ReplaceElementsAt(ArrayIndex index, void* elemPtr, ArrayIndex count);
//		NewtonErr	RemoveElementsAt(ArrayIndex index, ArrayIndex count);
//		NewtonErr	RemoveAll(void);
//		
//		// miscellaneous functions
//		
//		Boolean		IsEmpty(void);
//		
//		NewtonErr	Merge(CDynamicArray* aDynamicArray);
//		
//		// don't call this unless you know what you are doing.  Just say NO!
//		
//		void		NukeIterator(void);		// Dangerous
//		
//	protected:
//		
//		Size			ComputeByteCount(ArrayIndex count);
		
	SIM_GET_SET_W(ArrayIndex, Size)				//  +0: logical size of array
	SIM_GET_SET_W(Size, ElementSize)			//  +4: size of a single element
	SIM_GET_SET_W(ArrayIndex, ChunkSize)		//  +8: grow/shrink array by this many elements
	SIM_GET_SET_W(ArrayIndex, AllocatedSize)	// +12: physical size of array
	SIM_GET_SET_W(void*, ArrayBlock)			// +16: element storage
	SIM_GET_SET_W(CArrayIterator*, Iterator)	// +20: linked list of iterators active on this array

//	inline ArrayIndex CDynamicArray::GetArraySize()
//	{ return fSize; }
//	
//	inline Boolean CDynamicArray::IsEmpty()
//	{ return (fSize == 0); }
//	
//	inline void* CDynamicArray::ElementPtrAt(ArrayIndex index)
//	{ return (void*)((long)fArrayBlock + (fElementSize * index)); }
//	
//	inline Size CDynamicArray::ComputeByteCount(ArrayIndex count)
//	{ return (fElementSize * count); }
//	
//	inline NewtonErr CDynamicArray::RemoveAll()
//	{ return RemoveElementsAt(0, fSize); }
//	
//	inline void CDynamicArray::NukeIterator()
//	{ fIterator = nil; }
};


#endif	// C_ARRAY_ITERATOR_H

