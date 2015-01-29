// ==============================
// Fichier:			TGPBooleanData.h
// Projet:			K
//
// Créé le:			23/2/2003
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
// The Original Code is TGPBooleanData.h.
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
// $Id: TGPBooleanData.h,v 1.4 2005/02/14 20:55:02 pguyot Exp $
// ===========

#ifndef _TGPBOOLEANDATA_H
#define _TGPBOOLEANDATA_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/TGPData.h>

// ISO C++
#include <string>

///
/// Classe pour des données sous forme de booléen.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPBooleanData
	:
		public TGPData
{
protected:
	///
	/// Classe de TGPBooleanData
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
				return &TGPBooleanData::kClass == &inOtherClass;
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
	/// Objet qui représente cette classe.
	///	
	static const TClass	kClass;	

	///
	/// Constructeur à partir d'un booléen et d'une classe.
	///
	/// \param inBoolean	booléen pour cette donnée.
	/// \param inClass		classe pour cette donnée.
	///
	TGPBooleanData( Boolean inBoolean, const TGPType& inClass = kClass );

	///
	/// Récupération de la chaîne pour ces données.
	///
	/// \param inContext	contexte de l'évaluation.
	/// \return une représentation sous forme de chaîne des données.
	///
	virtual const std::string&	AsString( TGPEvaluationContext* inContext ) const;

	///
	/// Récupération du booléen pour ces données.
	///
	/// \param inContext	contexte de l'évaluation.
	/// \return un booléen qui représente les données.
	///
	virtual Boolean				AsBoolean( TGPEvaluationContext* inContext ) const;

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

private:
	/// \name Constantes
	static const std::string kTrueStr;	///< "true"
	static const std::string kFalseStr;	///< "false"

	/// \name Variables
	Boolean				mBoolean;
};

#endif
		// _TGPBOOLEANDATA_H

// ======================= //
// With your bare hands?!? //
// ======================= //
