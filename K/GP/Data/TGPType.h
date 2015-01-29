// ==============================
// Fichier:			TGPType.h
// Projet:			K
// 
// Créé le:			15/10/2003
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
/// Classe pour un type (une classe) de données dans la
/// hiérarchie des types.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	aucun test défini.
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
	/// Cette méthode doit être dérivée pour toute classe concrète.
	/// Elle n'a pas besoin de l'être pour les classes abstraites.
	///
	/// \return une chaîne représentant la classe de l'objet.
	///
	virtual TStringRef GetName( void ) const = 0;

	///
	/// Détermine si cette classe est égale ou une sous-classe
	/// d'une classe donnée.
	/// Cette méthode récupère le nom et appelle la méthode suivante.
	///
	/// \param inOtherClass	classe pour le test.
	/// \return \c true si cet objet est un élément de la classe \c inClassName
	///
	virtual Boolean IsTypeOf( const TGPType& inOtherClass ) const = 0;

	///
	/// Récupère une valeur à partir d'une chaîne.
	///
	/// \param outValue	valeur (en sortie).
	/// \return \c true si la chaîne a pu être analysée correctement.
	///
	virtual Boolean	ValueFromString( TGPDataRef& outValue, const char* inString ) const = 0;

protected:
	///
	/// Constructeur par défaut.
	///
	TGPType( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPType( const TGPType& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPType& operator = ( const TGPType& inCopy );
};

#endif
		// _TGPTYPE_H

// ================================== //
// I bet the human brain is a kludge. //
//                 -- Marvin Minsky   //
// ================================== //
