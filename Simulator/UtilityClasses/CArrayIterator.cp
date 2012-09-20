// ==============================
// File:			CArrayIterator.cp
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


#include "Simulator/Sim.h"
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


void CArrayIterator::Reset()
{
	if (GetIterateForward()) {
		SetCurrentIndex( GetLowBound() );
	} else {
		SetCurrentIndex( GetHighBound() );
	}
}


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


ArrayIndex CArrayIterator::FirstIndex()
{
	Reset();
	if (More()) {
		return GetCurrentIndex();
	} else {
		return kEmptyIndex;
	}
}


ArrayIndex CArrayIterator::NextIndex()
{
	Advance();
	if (More()) {
		return GetCurrentIndex();
	} else {
		return kEmptyIndex;
	}
}


ArrayIndex CArrayIterator::CurrentIndex()
{
	if (GetDynamicArray()) {
		return GetCurrentIndex();
	} else {
		return kEmptyIndex;
	}
}


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


Boolean CArrayIterator::More()
{
	if (GetDynamicArray()) {
		if (GetCurrentIndex()==kEmptyIndex) {
			return 0;
		} else {
			return 1;
		}
	} else {
		return 0;
	}
}


CArrayIterator* CArrayIterator::AppendToList(CArrayIterator * inList)
{
	if (inList) {
		CArrayIterator *nextLink = inList->GetNextLink();
		SetPreviousLink(inList);
		SetNextLink(nextLink);
		nextLink->SetPreviousLink(this);
		inList->SetNextLink(this);
		return this;
	} else {
		return 0L;
	}
}


CArrayIterator* CArrayIterator::RemoveFromList()
{
	CArrayIterator* nextLink = GetNextLink(); // r1
	CArrayIterator* prevLink = GetPreviousLink(); // r3
	CArrayIterator* returnLink;
	
	if (nextLink==this) {
		returnLink = 0L;
	} else {
		returnLink = nextLink;
	}
	nextLink->SetPreviousLink( prevLink );
	prevLink->SetNextLink(nextLink);
	SetNextLink(this);
	SetPreviousLink(this);
	
	return returnLink;
}


//T_SIM_INJECTION(0x000384E0, "CArrayIterator::Advance") {
//	SIM_CLASS(CArrayIterator)->Advance();
//	SIM_RETURN;
//}
//T_SIM_INJECTION(0x000383B4, "CArrayIterator::AppendToList(list)") {
//	SIM_RETVAL SIM_CLASS(CArrayIterator)->AppendToList(SIM_ARG1(CArrayIterator*));
//	SIM_RETURN;
//}
//T_SIM_INJECTION(0x00038600, "CArrayIterator::CurrentIndex") {
//	SIM_RETVAL SIM_CLASS(CArrayIterator)->CurrentIndex();
//	SIM_RETURN;
//}
//T_SIM_INJECTION(0x00038614, "CArrayIterator::FirstIndex") {
//	SIM_RETVAL SIM_CLASS(CArrayIterator)->FirstIndex();
//	SIM_RETURN;
//}
//T_SIM_INJECTION(0x00038478, "CArrayIterator::More") {
//	SIM_RETVAL SIM_CLASS(CArrayIterator)->More();
//	SIM_RETURN;
//}
//T_SIM_INJECTION(0x00038674, "CArrayIterator::NextIndex") {
//	SIM_RETVAL SIM_CLASS(CArrayIterator)->NextIndex();
//	SIM_RETURN;
//}
//T_SIM_INJECTION(0x00038640, "CArrayIterator::RemoveFromList") {
//	SIM_RETVAL SIM_CLASS(CArrayIterator)->RemoveFromList();
//	SIM_RETURN;
//}
//T_SIM_INJECTION(0x00038498, "CArrayIterator::Reset") {
//	SIM_CLASS(CArrayIterator)->Reset();
//	SIM_RETURN;
//}
