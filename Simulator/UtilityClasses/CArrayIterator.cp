// ==============================
// File:			CArrayIterator.cp
// Project:			Einstein
//
// Copyright 1999-2012 by Newton Rsearch Group
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


#include "Simulator/UtilityClasses/CArrayIterator.h"


/**
 Advance to the next index in the array.
 */
void CArrayIterator::Advance()
{
	ArrayIndex currentIndex = GetCurrentIndex();
	if (GetIterateForward()) {
		if (currentIndex<GetHighBound()) {
			currentIndex++;
			SetCurrentIndex(currentIndex);
			return;
		}
	} else {
		if (currentIndex>GetLowBound()) {
			currentIndex--;
			SetCurrentIndex(currentIndex);
			return;
		}
	}
	SetCurrentIndex(kEmptyIndex);
	return;
}


/*--------------------------------------------------------------------------------
 C A r r a y I t e r a t o r
 --------------------------------------------------------------------------------*/

//CArrayIterator::CArrayIterator()
//{
//	init();
//}
//
//
//CArrayIterator::CArrayIterator(CDynamicArray * inDynamicArray)
//{
//	init(inDynamicArray, 0, inDynamicArray->getArraySize() - 1, kIterateForward);
//}
//
//
//CArrayIterator::CArrayIterator(CDynamicArray * inDynamicArray, BOOL inForward)
//{
//	init(inDynamicArray, 0, inDynamicArray->getArraySize() - 1, inForward);
//}
//
//
//CArrayIterator::CArrayIterator(CDynamicArray * inDynamicArray,
//							   ArrayIndex inLowBound,
//							   ArrayIndex inHighBound, BOOL inForward)
//{
//	init(inDynamicArray, inLowBound, inHighBound, inForward);
//}
//
//
//CArrayIterator::~CArrayIterator()
//{
//	if (fDynamicArray)
//		fDynamicArray->fIterator = removeFromList();
//}
//
//
//void
//CArrayIterator::init(void)
//{
//	fNextLink = fPreviousLink = this;
//	fCurrentIndex = fLowBound = fHighBound = kEmptyIndex;
//	fIterateForward = kIterateForward;
//	fDynamicArray = nil;
//}
//
//
//void
//CArrayIterator::init(CDynamicArray * inDynamicArray,
//					 ArrayIndex inLowBound,
//					 ArrayIndex inHighBound, BOOL inForward)
//{
//	fNextLink = fPreviousLink = this;
//	fDynamicArray = inDynamicArray;
//	inDynamicArray->fIterator = appendToList(inDynamicArray->fIterator);
//	initBounds(inLowBound, inHighBound, inForward);
//}
//
//
//void
//CArrayIterator::initBounds(ArrayIndex inLowBound, ArrayIndex inHighBound, BOOL inForward)
//{
//	fHighBound = (fDynamicArray->fSize > 0) ? MINMAX(0, inHighBound, fDynamicArray->fSize - 1) : kEmptyIndex;
//	fLowBound = (fHighBound != kEmptyIndex && fHighBound >= 0) ? MINMAX(0, inLowBound, fHighBound) : kEmptyIndex;
//	fIterateForward = inForward;
//	reset();
//}
//
//
//void
//CArrayIterator::resetBounds(BOOL inForward)
//{
//	fHighBound = (fDynamicArray->fSize > 0) ? fDynamicArray->fSize - 1 : kEmptyIndex;
//	fLowBound = (fHighBound != kEmptyIndex && fHighBound >= 0) ? 0 : kEmptyIndex;
//	fIterateForward = inForward;
//	reset();
//}
//
//
//void
//CArrayIterator::reset(void)
//{
//	fCurrentIndex = fIterateForward ? fLowBound : fHighBound;
//}
//
//
//void
//CArrayIterator::switchArray(CDynamicArray * inNewArray, BOOL inForward)
//{
//	if (fDynamicArray)
//	{
//		fDynamicArray->fIterator = removeFromList();
//		fDynamicArray = nil;
//	}
//	init(inNewArray, 0, inNewArray->fSize - 1, inForward);
//}
//
//
//ArrayIndex
//CArrayIterator::firstIndex(void)
//{
//	reset();
//	return more() ? fCurrentIndex : kEmptyIndex;
//}
//
//
//ArrayIndex
//CArrayIterator::nextIndex(void)
//{
//	advance();
//	return more() ? fCurrentIndex : kEmptyIndex;
//}
//
//
//ArrayIndex
//CArrayIterator::currentIndex(void)
//{
//	return fDynamicArray ? fCurrentIndex : kEmptyIndex;
//}
//
//
//void
//CArrayIterator::removeElementsAt(ArrayIndex index, ArrayIndex inCount)
//{
//	if (fLowBound > index)
//		fLowBound -= inCount;
//	if (fHighBound >= index)
//		fHighBound -= inCount;
//	if (fIterateForward)
//	{
//		if (fCurrentIndex >= index)
//			fCurrentIndex -= inCount;
//	}
//	else
//	{
//		if (fCurrentIndex > index)
//			fCurrentIndex -= inCount;
//	}
//	if (fDynamicArray && fNextLink != fPreviousLink)
//		fNextLink->removeElementsAt(index, inCount);
//}
//
//
//void
//CArrayIterator::insertElementsBefore(ArrayIndex index, ArrayIndex inCount)
//{
//}
//
//
//void
//CArrayIterator::deleteArray(void)
//{
//	if (fNextLink != fPreviousLink)
//		fNextLink->deleteArray();
//	fDynamicArray = nil;
//}
//
//
//BOOL
//CArrayIterator::more(void)
//{
//	return fDynamicArray != nil
//	&& fCurrentIndex != kEmptyIndex;
//}
//
//
//CArrayIterator *
//CArrayIterator::appendToList(CArrayIterator * inList)
//{
//	if (inList)
//	{
//		fPreviousLink = inList;
//		fNextLink = inList->fNextLink;
//		inList->fNextLink->fPreviousLink = this;
//		inList->fNextLink = this;
//	}
//	return this;
//}
//
//
//CArrayIterator *
//CArrayIterator::removeFromList(void)
//{
//	CArrayIterator *	result = (fNextLink != this) ? fNextLink : nil;
//	fNextLink->fPreviousLink = fPreviousLink;
//	fNextLink->fNextLink = fPreviousLink->fNextLink;
//	fNextLink = fPreviousLink = this;
//	return result;
//}

