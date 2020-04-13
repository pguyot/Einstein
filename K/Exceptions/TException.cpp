// ==============================
// Fichier:			TException.cp
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// CrŽŽ le:			10/6/2005
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
// The Original Code is TException.cp.
// 
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2005 the
// Initial Developer. All Rights Reserved.
// 
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
// 
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TException.cp,v 1.2 2006/01/18 09:01:38 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TException.h"

// ISO C++
#include <typeinfo>

#include <K/Tests/KDebug.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TException( void )
// -------------------------------------------------------------------------- //
TException::TException( void )
{
	KDEBUG1( "New Exception - %s\n", typeid(*this).name() );
}

// -------------------------------------------------------------------------- //
//  * TException( const char*, KUInt32 )
// -------------------------------------------------------------------------- //
TException::TException( const char* inFileName, KUInt32 inLine )
	:
		mFileName( inFileName ),
		mLine( inLine )
{
	KDEBUG3( "New Exception - %s at %s:%u\n",
				typeid(*this).name(),
				inFileName,
				(unsigned int) inLine );
}

// -------------------------------------------------------------------------- //
//  * TException( const TException& )
// -------------------------------------------------------------------------- //
TException::TException( const TException& inCopy )
	:
		std::exception( inCopy ),
		mFileName( inCopy.mFileName ),
		mLine( inCopy.mLine )
{
}

// -------------------------------------------------------------------------- //
//  * ~TException( void ) throw ()
// -------------------------------------------------------------------------- //
TException::~TException( void ) throw ()
{
}

// -------------------------------------------------------------------------- //
//  * what( void ) const throw ()
// -------------------------------------------------------------------------- //
const char*
TException::what( void ) const throw ()
{
	return typeid(*this).name();
}

// =========================================== //
// * * * * * THIS TERMINAL IS IN USE * * * * * //
// =========================================== //
