// ==============================
// Fichier:			TDoubleLinkedList.h
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// CrŽŽ le:			2/10/2005
// Tabulation:		4 espaces
// 
// ***** BEGIN LICENSE BLOCK *****
// Version: MPL 1.1
// 
// The contents of this file are subject to the Mozilla Public License Version
// 1.1 (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
// http://www.mozilla.org/MPL/
// 
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the
// License.
// 
// The Original Code is TDoubleLinkedList.h.
// 
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2005-2006 the
// Initial Developer. All Rights Reserved.
// 
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
// 
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TDoubleLinkedList.h,v 1.2 2006/07/07 08:18:13 pguyot Exp $
// ===========

#ifndef _TDOUBLELINKEDLIST_H
#define _TDOUBLELINKEDLIST_H

///
/// Class for items in a double linked list.
///
class TDoubleLinkedElem
{
public:
	///
	/// Constructor.
	///
	inline TDoubleLinkedElem( void )
		:
			mPrevElem( NULL ),
			mNextElem( NULL )
		{
		}
	
	friend class TDoubleLinkedList;

	///
	/// Accessor on the previous item.
	///
	inline TDoubleLinkedElem*	GetPrevElem( void ) const
		{
			return mPrevElem;
		}

	///
	/// Accessor on the next item.
	///
	inline TDoubleLinkedElem*	GetNextElem( void ) const
		{
			return mNextElem;
		}
private:
	///
	/// Selector on the previous item.
	///
	inline void SetPrevElem( TDoubleLinkedElem* inNewPrevElem )
		{
			mPrevElem = inNewPrevElem;
		}

	///
	/// Selector on the next item.
	///
	inline void SetNextElem( TDoubleLinkedElem* inNewNextElem )
		{
			mNextElem = inNewNextElem;
		}

	TDoubleLinkedElem*	mPrevElem;	///< Previous element or NULL
	TDoubleLinkedElem*	mNextElem;	///< Next element or NULL
};

///
/// Class for a double linked list.
///
class TDoubleLinkedList
{
public:
	///
	/// Constructor.
	///
	inline TDoubleLinkedList( void )
		:
			mFirstElem( NULL ),
			mLastElem( NULL )
		{
		}

	///
	/// Accessor to the front item.
	///
	inline TDoubleLinkedElem* GetFirstItem( void )
		{
			return mFirstElem;
		}

	///
	/// Accessor to the back item.
	///
	inline TDoubleLinkedElem* GetLastItem( void )
		{
			return mLastElem;
		}

	///
	/// Make an item the first item.
	///
	inline void MoveItemToBeginning( TDoubleLinkedElem* inNewFirstElem )
		{
			Remove(inNewFirstElem);
			PushFront(inNewFirstElem);
		}

	///
	/// Make an item the last item.
	///
	inline void MoveItemToEnd( TDoubleLinkedElem* inNewLastElem )
		{
			Remove(inNewLastElem);
			PushBack(inNewLastElem);
		}

	///
	/// Pop item from the beginning.
	/// Return NULL if there is no such item.
	///
	inline TDoubleLinkedElem* PopFront( void )
		{
			TDoubleLinkedElem* theResult = mFirstElem;
			if (theResult != NULL)
			{
				mFirstElem = theResult->GetNextElem();
				if (mFirstElem != NULL)
				{
					mFirstElem->SetPrevElem(NULL);
				}
				theResult->SetPrevElem(NULL);
				theResult->SetNextElem(NULL);
			}
			return theResult;
		}

	///
	/// Pop item from the end.
	/// Return NULL if there is no such item.
	///
	inline TDoubleLinkedElem* PopBack( void )
		{
			TDoubleLinkedElem* theResult = mLastElem;
			if (theResult != NULL)
			{
				mLastElem = theResult->GetPrevElem();
				if (mLastElem != NULL)
				{
					mLastElem->SetNextElem(NULL);
				}
				theResult->SetPrevElem(NULL);
				theResult->SetNextElem(NULL);
			}
			return theResult;
		}

	///
	/// Push an item at the beginning.
	///
	inline void PushFront( TDoubleLinkedElem* inNewFirstElem )
		{
			inNewFirstElem->SetNextElem(mFirstElem);
			inNewFirstElem->SetPrevElem(NULL);
			if (mFirstElem != NULL)
			{
				mFirstElem->SetPrevElem(inNewFirstElem);
			} else {
				mLastElem = inNewFirstElem;
			}
			mFirstElem = inNewFirstElem;
		}

	///
	/// Push an item at the end.
	///
	inline void PushBack( TDoubleLinkedElem* inNewLastElem )
		{
			inNewLastElem->SetPrevElem(mLastElem);
			inNewLastElem->SetNextElem(NULL);
			if (mLastElem != NULL)
			{
				mLastElem->SetNextElem(inNewLastElem);
			} else {
				mFirstElem = inNewLastElem;
			}
			mLastElem = inNewLastElem;
		}

	///
	/// Remove an item.
	///
	inline void Remove( TDoubleLinkedElem* inElem )
		{
			TDoubleLinkedElem* thePrevious = inElem->GetPrevElem();
			TDoubleLinkedElem* theNext = inElem->GetNextElem();
			if (thePrevious == NULL)
			{
				if (mFirstElem == inElem)
				{
					mFirstElem = theNext;
				}
			} else {
				thePrevious->SetNextElem(theNext);
			}
			if (theNext == NULL)
			{
				if (mLastElem == inElem)
				{
					mLastElem = thePrevious;
				}
			} else {
				theNext->SetPrevElem(thePrevious);
			}
			inElem->SetPrevElem(NULL);
			inElem->SetNextElem(NULL);
		}

private:
	/// \name Variables
	TDoubleLinkedElem*	mFirstElem;	///< Pointer to the first element.
	TDoubleLinkedElem*	mLastElem;	///< Pointer to the last element.
};

#endif

// ============================================================================== //
// Dear Emily, what about test messages?                                          //
//                 -- Concerned                                                   //
//                                                                                //
// Dear Concerned:                                                                //
//         It is important, when testing, to test the entire net.  Never test     //
// merely a subnet distribution when the whole net can be done.  Also put "please //
// ignore" on your test messages, since we all know that everybody always skips   //
// a message with a line like that.  Don't use a subject like "My sex is female   //
// but I demand to be addressed as male." because such articles are read in depth //
// by all USEnauts.                                                               //
//                 -- Emily Postnews Answers Your Questions on Netiquette         //
// ============================================================================== //
