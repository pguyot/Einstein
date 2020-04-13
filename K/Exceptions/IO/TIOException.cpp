// ==============================
// Fichier:			TIOException.cp
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
// The Original Code is TIOException.cp.
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
// $Id: TIOException.cp,v 1.1 2005/06/10 23:01:05 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TIOException.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TIOException( void )
// -------------------------------------------------------------------------- //
TIOException::TIOException( void )
{
}

// -------------------------------------------------------------------------- //
//  * TIOException( const char*, KUInt32 )
// -------------------------------------------------------------------------- //
TIOException::TIOException( const char* inFileName, KUInt32 inLine )
	:
		TException( inFileName, inLine )
{
}

// -------------------------------------------------------------------------- //
//  * ~TIOException( void )
// -------------------------------------------------------------------------- //
TIOException::~TIOException( void ) throw ()
{
}


// ================================================================== //
// You can do this in a number of ways.  IBM chose to do all of them. //
// Why do you find that funny?                                        //
//                 -- D. Taylor, Computer Science 350                 //
// ================================================================== //
