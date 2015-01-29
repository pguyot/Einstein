// ==============================
// Fichier:			UExpatUtils.h
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
// The Original Code is UExpatUtils.h.
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
// $Id: UExpatUtils.h,v 1.1 2005/02/14 20:55:07 pguyot Exp $
// ===========

#ifndef _UEXPATUTILS_H
#define _UEXPATUTILS_H

#include <K/Defines/KDefinitions.h>

// expat
#include <expat.h>

///
/// Classe pour des utilitaires relatifs à expat.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.1 $
///
/// \test	aucun test défini.
///
class UExpatUtils
{
public:
	///
	/// Récupère la valeur d'un attribut à partir
	/// de son nom.
	///
	/// \param inAttributes		tableau des attributs
	/// \param inAttrName		nom de l'attribut
	/// \return la valeur de l'attribut ou \c nil si l'attribut
	///			n'est pas dans la liste.
	///
	static const XML_Char* const
				GetAttrValue(
					const XML_Char** const inAttributes,
					const XML_Char* const inAttrName );
};

#endif
		// _UEXPATUTILS_H

// ============================================================================ //
// Unix Express:                                                                //
// All passenger bring a piece of the aeroplane and a box of tools with them to //
// the airport. They gather on the tarmac, arguing constantly about what kind   //
// of plane they want to build and how to put it together. Eventually, the      //
// passengers split into groups and build several different aircraft, but give  //
// them all the same name. Some passengers actually reach their destinations.   //
// All passengers believe they got there.                                       //
// ============================================================================ //
