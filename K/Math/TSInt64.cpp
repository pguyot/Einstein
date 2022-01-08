// ==============================
// Fichier:			TSInt64.cp
// Projet:			K
// 
// Créé le:			21/2/2004
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
// The Original Code is TSInt64.cp.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TSInt64.cp,v 1.5 2005/02/24 06:49:58 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Math/TSInt64.h>

// --------------------------------------------------------------------	//
//	* operator == ( const TSInt64&, KSInt32 )
// --------------------------------------------------------------------	//
bool
operator == ( const TSInt64& inArgOne, const KSInt32 inArgTwo )
{
	return (inArgOne.GetHi() == 0)
		&& (((KSInt32) inArgOne.GetLo()) == inArgTwo);
}

// --------------------------------------------------------------------	//
//	* operator == ( KSInt32, const TSInt64& )
// --------------------------------------------------------------------	//
bool
operator == ( const KSInt32 inArgOne, const TSInt64& inArgTwo )
{
	return (inArgTwo.GetHi() == 0)
		&& (((KSInt32) inArgTwo.GetLo()) == inArgOne);
}

// ==================================================== //
// Passwords are implemented as a result of insecurity. //
// ==================================================== //
