// ==============================
// Fichier:			TGPType.h
// Projet:			K
// 
// Cr�� le:			15/10/2003
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
// The Original Code is TGPType.h.
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
// $Id: TGPType.h,v 1.3 2005/02/14 20:55:02 pguyot Exp $
// ===========

#ifndef _TGPTYPE_H
#define _TGPTYPE_H

#include <K/Defines/KDefinitions.h>

// K
#include <K/Unicode/TStringRef.h>

class TGPDataRef;

///
/// Classe pour un type (une classe) de donn�es dans la
/// hi�rarchie des types.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	aucun test d�fini.
///
class TGPType
{
public:
	///
	/// Destructeur.
	///
	virtual ~TGPType( void );

	///
	/// Accesseur sur le nom de la classe de cette objet.
	/// Cette m�thode doit �tre d�riv�e pour toute classe concr�te.
	/// Elle n'a pas besoin de l'�tre pour les classes abstraites.
	///
	/// \return une cha�ne repr�sentant la classe de l'objet.
	///
	virtual TStringRef GetName( void ) const = 0;

	///
	/// D�termine si cette classe est �gale ou une sous-classe
	/// d'une classe donn�e.
	/// Cette m�thode r�cup�re le nom et appelle la m�thode suivante.
	///
	/// \param inOtherClass	classe pour le test.
	/// \return \c true si cet objet est un �l�ment de la classe \c inClassName
	///
	virtual Boolean IsTypeOf( const TGPType& inOtherClass ) const = 0;

	///
	/// R�cup�re une valeur � partir d'une cha�ne.
	///
	/// \param outValue	valeur (en sortie).
	/// \return \c true si la cha�ne a pu �tre analys�e correctement.
	///
	virtual Boolean	ValueFromString( TGPDataRef& outValue, const char* inString ) const = 0;

protected:
	///
	/// Constructeur par d�faut.
	///
	TGPType( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPType( const TGPType& inCopy );

	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPType& operator = ( const TGPType& inCopy );
};

#endif
		// _TGPTYPE_H

// ================================== //
// I bet the human brain is a kludge. //
//                 -- Marvin Minsky   //
// ================================== //
