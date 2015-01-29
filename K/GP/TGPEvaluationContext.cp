// ==============================
// Fichier:			TGPEvaluationContext.cp
// Projet:			K
//
// Créé le:			25/2/2003
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
// The Original Code is TGPEvaluationContext.cp.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2003-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TGPEvaluationContext.cp,v 1.5 2004/11/23 15:11:29 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/TGPEvaluationContext.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPEvaluationContext( void )
// -------------------------------------------------------------------------- //
TGPEvaluationContext::TGPEvaluationContext( void )
	:
		mContinue( true )
{
}

// -------------------------------------------------------------------------- //
//  * ~TGPEvaluationContext( void )
// -------------------------------------------------------------------------- //
TGPEvaluationContext::~TGPEvaluationContext( void )
{
	// Suppression des éléments du cache.
	list<TCacheItem*>::iterator theIterator;
	list<TCacheItem*>::iterator theEnd;
	
	for (
			theIterator = mCachePool.begin(),
			theEnd = mCachePool.end();
			theIterator != theEnd;
			(void) (++theIterator) )
	{
		delete *theIterator;
	}
}

// -------------------------------------------------------------------------- //
//  * Reset( void )
// -------------------------------------------------------------------------- //
void
TGPEvaluationContext::Reset( void )
{
	mCookieJar.clear();
	
	// Suppression des éléments du cache.
	list<TCacheItem*>::iterator theIterator;
	list<TCacheItem*>::iterator theEnd;
	
	for (
			theIterator = mCachePool.begin(),
			theEnd = mCachePool.end();
			theIterator != theEnd;
			(void) (++theIterator) )
	{
		delete *theIterator;
	}
	
	mCachePool.clear();

	mContinue = true;
}

// ================================================ //
// ** MAXIMUM TERMINALS ACTIVE.  TRY AGAIN LATER ** //
// ================================================ //
