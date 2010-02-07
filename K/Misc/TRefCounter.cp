// ==============================
// Fichier:			TRefCounter.cp
// Projet:			K
//
// CrŽŽ le:			10/1/2003
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
// The Original Code is TRefCounter.cp.
//
// The Initial Developers of the Original Code are Paul Guyot, Michael Vac’k
// and Nicolas Zinovieff. Portions created by the Initial Developers are
// Copyright (C) 2003-2004 the Initial Developers. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//   Michael Vac’k <mici@metastasis.net> (original author)
//   Nicolas Zinovieff <krugazor@poulet.org> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TRefCounter.cp,v 1.3 2004/11/23 15:11:31 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Misc/TRefCounter.h>

// K
#include <K/Tests/KDebug.h>

// -------------------------------------------------------------------------- //
// TRefCounter( void )
// -------------------------------------------------------------------------- //
TRefCounter::TRefCounter( void )
	:
		mIsNotOnStack( true ),
		mRefCount( 0 )
{
}

// -------------------------------------------------------------------------- //
// TRefCounter( const TRefCounter& )
// -------------------------------------------------------------------------- //
TRefCounter::TRefCounter( const TRefCounter& /* inCopy */ )
	:
		mIsNotOnStack( true ),
		mRefCount( 0 )
{
}

// -------------------------------------------------------------------------- //
// ~TRefCounter( void )
// -------------------------------------------------------------------------- //
TRefCounter::~TRefCounter( void )
{
	KASSERT(mRefCount == 0);
}

// ------------------------------------------------------------------------- //
//  * IsOnStack( void )
// ------------------------------------------------------------------------- //
void
TRefCounter::IsOnStack( void )
{
	if (mRefCount == 0)
	{
		mIsNotOnStack = false;
	}
}

// ------------------------------------------------------------------------- //
//  * IncrementRef( void )
// ------------------------------------------------------------------------- //
void
TRefCounter::IncrementRef( void )
{
	mRefCount++;
}

// ------------------------------------------------------------------------- //
//  * DecrementRef( void )
// ------------------------------------------------------------------------- //
Boolean
TRefCounter::DecrementRef( void )
{
	KASSERT(mRefCount > 0);

	return ((--mRefCount == 0) && mIsNotOnStack);
}

// =============================================================== //
// Science is to computer science as hydrodynamics is to plumbing. //
// =============================================================== //
