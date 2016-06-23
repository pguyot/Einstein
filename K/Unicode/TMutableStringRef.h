// ==============================
// Fichier:			TMutableStringRef.h
// Projet:			K
// 
// Créé le:			1/10/2003
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
// The Original Code is TMutableStringRef.h.
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
// $Id: TMutableStringRef.h,v 1.2 2004/11/23 15:11:32 paul Exp $
// ===========

#ifndef _TMUTABLESTRINGREF_H
#define _TMUTABLESTRINGREF_H

#include <K/Unicode/UnicodeDefinitions.h>

// K
#include <K/Unicode/TString.h>

///
/// Classe pour une référence sur une chaîne modifiable.
/// En fait, c'est exactement comme une référence sur une chaîne constante
/// sauf que les accesseurs retournent une référence modifiable.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.2 $
///
/// \test	aucun test défini.
///
class TMutableStringRef
{
public:
	///
	/// Constructeur à partir d'un objet TString.
	///
	/// \param inString	objet TString
	///
	TMutableStringRef( TString* inString );

	///
	/// Destructeur.
	///
	~TMutableStringRef( void );

	///
	/// Constructeur par copie.
	/// Incrémente la référence.
	///
	/// \param inCopy		objet à copier
	///
	TMutableStringRef( const TMutableStringRef& inCopy );

	///
	/// Opérateur d'assignation.
	/// Idem.
	///
	/// \param inCopy		objet à copier
	///
	TMutableStringRef& operator = ( const TMutableStringRef& inCopy );

	///
	/// Opérateur de déréférencement
	///
	/// \return une référence sur l'objet TString
	///
	inline TString& operator * ( void ) const
		{
			return *mString;
		}

	///
	/// Opérateur de déréférencement
	///
	/// \return une référence sur l'objet TString
	///
	inline TString* operator -> ( void ) const
		{
			return mString;
		}

	///
	/// Accesseur sur l'objet TString
	///
	/// \return une référence sur l'objet TString
	///
	inline TString* GetString( void ) const
		{
			return mString;
		}

private:
	/// \name Variables
	TString*		mString;	///< Objet référencé.
};

#endif
		// _TMUTABLESTRINGREF_H

// ================================================== //
// Remember the good old days, when CPU was singular? //
// ================================================== //
