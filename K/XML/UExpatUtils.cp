// ==============================
// Fichier:			UExpatUtils.cp
// Projet:			K/XML
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			4/2/2004
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
// The Original Code is UExpatUtils.cp.
// 
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2004-2005 the
// Initial Developer. All Rights Reserved.
// 
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
// 
// ***** END LICENSE BLOCK *****
// ===========
// $Id: UExpatUtils.cp,v 1.1 2005/02/14 20:55:07 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "UExpatUtils.h"

// ANSI C
#include <string.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * GetAttrValue( const XML_Char**, const XML_Char* )
// -------------------------------------------------------------------------- //
const XML_Char* const
UExpatUtils::GetAttrValue(
					const XML_Char** const inAttributes,
					const XML_Char* const inAttrName )
{
	const XML_Char* theResult = nil;
	const XML_Char** theAttributes = inAttributes;
	
	// Itération dans la liste.
	while (*theAttributes != nil)
	{
		if (::strcmp( *theAttributes, inAttrName ) == 0)
		{
			theResult = theAttributes[1];
			break;
		}
		
		theAttributes += 2;
	}
	
	return theResult;
}

// ===================== //
// You're at Witt's End. //
// ===================== //
