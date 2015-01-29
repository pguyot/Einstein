// ==============================
// Fichier:			TGPVoidData.h
// Projet:			K
// 
// Créé le:			16/10/2003
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
// The Original Code is TGPVoidData.h.
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
// $Id: TGPVoidData.h,v 1.3 2005/02/14 20:55:02 pguyot Exp $
// ===========

#ifndef _TGPVOIDDATA_H
#define _TGPVOIDDATA_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/TGPData.h>
#include <K/GP/Data/TGPType.h>

///
/// Classe pour une absence de données.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	aucun test défini.
///
class TGPVoidData
	:
		public TGPData
{
protected:
	///
	/// Classe de TGPVoidData
	///
	class TClass
		:
			public TGPType
	{
	public:
		///
		/// Accesseur sur le nom de la classe de cette objet.
		///
		/// \return une chaîne représentant la classe de l'objet.
		///
		virtual TStringRef GetName( void ) const
			{
				return kName;
			}

		///
		/// Détermine si cette classe est égale ou une sous-classe
		/// d'une classe donnée.
		///
		/// \param inOtherClass	classe pour le test.
		/// \return \c true si cet objet est un élément de la classe \c inOtherClass
		///
		virtual Boolean IsTypeOf( const TGPType& inOtherClass ) const
			{
				return &TGPVoidData::kClass == &inOtherClass;
			}
		
		///
		/// Récupère une valeur à partir d'une chaîne.
		///
		/// \param outValue	valeur (en sortie).
		/// \return \c true si la chaîne a pu être analysée correctement.
		///
		virtual Boolean	ValueFromString(
							TGPDataRef& outValue,
							const char* inString ) const;
	private:
		/// Nom de la classe.
		static const TStringRef	kName;
	};

public:
	///
	/// Objet qui représente la classe de TGPVoidData.
	///	
	static const TClass	kClass;	

	///
	/// Constructeur à partir d'une classe.
	///
	/// \param inClass	classe pour cette donnée.
	///
	TGPVoidData( const TGPType& inClass = kClass );

	///
	/// Test d'égalité.
	///
	/// \param inAlter		données avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est égal à \c inAlter
	///
	virtual Boolean Equal(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const;

	///
	/// Relation d'ordre.
	///
	/// \param inAlter		données avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est strictement supérieur à \c inAlter
	///
	virtual Boolean GreaterThan(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const;

	///
	/// Écrit la donnée sur un flux.
	///
	/// \param inStream	flux considéré.
	///
	virtual void	PrintToStream( std::ostream& inStream ) const;
};

#endif
		// _TGPVOIDDATA_H

// ============================================== //
// Marvelous!  The super-user's going to boot me! //
// What a finely tuned response to the situation! //
// ============================================== //
