// ==============================
// Fichier:			TMutableStringRef.h
// Projet:			K
// 
// Cr�� le:			1/10/2003
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
/// Classe pour une r�f�rence sur une cha�ne modifiable.
/// En fait, c'est exactement comme une r�f�rence sur une cha�ne constante
/// sauf que les accesseurs retournent une r�f�rence modifiable.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.2 $
///
/// \test	aucun test d�fini.
///
class TMutableStringRef
{
public:
	///
	/// Constructeur � partir d'un objet TString.
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
	/// Incr�mente la r�f�rence.
	///
	/// \param inCopy		objet � copier
	///
	TMutableStringRef( const TMutableStringRef& inCopy );

	///
	/// Op�rateur d'assignation.
	/// Idem.
	///
	/// \param inCopy		objet � copier
	///
	TMutableStringRef& operator = ( const TMutableStringRef& inCopy );

	///
	/// Op�rateur de d�r�f�rencement
	///
	/// \return une r�f�rence sur l'objet TString
	///
	inline TString& operator * ( void ) const
		{
			return *mString;
		}

	///
	/// Op�rateur de d�r�f�rencement
	///
	/// \return une r�f�rence sur l'objet TString
	///
	inline TString* operator -> ( void ) const
		{
			return mString;
		}

	///
	/// Accesseur sur l'objet TString
	///
	/// \return une r�f�rence sur l'objet TString
	///
	inline TString* GetString( void ) const
		{
			return mString;
		}

private:
	/// \name Variables
	TString*		mString;	///< Objet r�f�renc�.
};

#endif
		// _TMUTABLESTRINGREF_H

// ================================================== //
// Remember the good old days, when CPU was singular? //
// ================================================== //
