// ==============================
// Fichier:			TEOFException.cp
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
// The Original Code is TEOFException.cp.
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
// $Id: TEOFException.cp,v 1.1 2005/06/10 23:01:05 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TEOFException.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TEOFException( void )
// -------------------------------------------------------------------------- //
TEOFException::TEOFException( void )
{
}

// -------------------------------------------------------------------------- //
//  * TEOFException( const char*, KUInt32 )
// -------------------------------------------------------------------------- //
TEOFException::TEOFException( const char* inFileName, KUInt32 inLine )
	:
		TIOException( inFileName, inLine )
{
}

// -------------------------------------------------------------------------- //
//  * ~TEOFException( void )
// -------------------------------------------------------------------------- //
TEOFException::~TEOFException( void ) throw ()
{
}

// =========================================================================== //
// The use of COBOL cripples the mind; its teaching should, therefore, be      //
// regarded as a criminal offence.                                             //
//                 -- Edsger W. Dijkstra, SIGPLAN Notices, Volume 17, Number 5 //
// =========================================================================== //
