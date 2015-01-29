// ==============================
// Fichier:			U3DES.h
// Projet:			K
// 
// Créé le:			6/9/2003
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
// The Original Code is U3DES.h.
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
// $Id: U3DES.h,v 1.3 2004/11/23 15:11:22 paul Exp $
// ===========

#ifndef _U3DES_H
#define _U3DES_H

#include <K/Defines/KDefinitions.h>
#include <K/Crypto/UDES.h>

///
/// Classe pour gérer le cryptage en triple DES.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	aucun test défini.
///
class U3DES
	:
		protected UDES
{
public:
private:
	///
	/// Constructeur par défaut volontairement privé.
	/// Cette classe est statique.
	///
	U3DES( void );
};

#endif
		// _U3DES_H

// ====================================================================== //
// If the automobile had followed the same development as the computer, a //
// Rolls-Royce would today cost $100, get a million miles per per gallon, //
// and explode once a year killing everyone inside.                       //
//                 -- Robert Cringely, InfoWorld                          //
// ====================================================================== //
