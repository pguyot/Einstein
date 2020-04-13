// ==============================
// Fichier:			TMemError.cp
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
// The Original Code is TMemError.cp.
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
// $Id: TMemError.cp,v 1.1 2005/06/10 23:01:05 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TMemError.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TMemError( void )
// -------------------------------------------------------------------------- //
TMemError::TMemError( void )
{
}

// -------------------------------------------------------------------------- //
//  * TMemError( const char*, KUInt32 )
// -------------------------------------------------------------------------- //
TMemError::TMemError( const char* inFileName, KUInt32 inLine )
	:
		TError( inFileName, inLine )
{
}

// -------------------------------------------------------------------------- //
//  * ~TMemError( void ) throw ()
// -------------------------------------------------------------------------- //
TMemError::~TMemError( void ) throw ()
{
}


// =============================================================================== //
// All programmers are optimists.  Perhaps this modern sorcery especially attracts //
// those who believe in happy endings and fairy godmothers.  Perhaps the hundreds  //
// of nitty frustrations drive away all but those who habitually focus on the end  //
// goal.  Perhaps it is merely that computers are young, programmers are younger,  //
// and the young are always optimists.  But however the selection process works,   //
// the result is indisputable:  "This time it will surely run," or "I just found   //
// the last bug."                                                                  //
//                 -- Frederick Brooks, "The Mythical Man Month"                   //
// =============================================================================== //
