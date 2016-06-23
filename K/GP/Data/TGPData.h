// ==============================
// Fichier:			TGPData.h
// Projet:			K
//
// Créé le:			13/2/2003
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
// The Original Code is TGPData.h.
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
// $Id: TGPData.h,v 1.4 2005/02/14 20:55:02 pguyot Exp $
// ===========

#ifndef _TGPDATA_H
#define _TGPDATA_H

#include <K/Defines/KDefinitions.h>
#include <K/Misc/TRefCounter.h>

// ISO C++
#include <string>

// K
#include <K/GP/Data/TGPType.h>

class TGPEvaluationContext;

///
/// Classe de base pour des données.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPData
	:
		private TRefCounter
{
	///
	/// La classe pour la référence sur les données est une amie.
	///
	friend class TGPDataRef;

public:
	///
	/// Constructeur à partir d'une classe.
	///
	TGPData( const TGPType& inClass );

	///
	/// Destructeur.
	///
	virtual ~TGPData( void );

	///
	/// Récupération de la classe de ces données.
	///
	/// \return une référence sur la classe de ces données.
	///
	inline const TGPType&	GetClass( void ) const
		{
			return mClass;
		}

	///
	/// Détermine si cet objet est d'une classe donnée (éventuellement
	/// d'une sous-classe).
	///
	/// \param inClass	classe avec laquelle tester \c this
	/// \return \c true si \c this est de classe \c inClass
	///
	inline Boolean				IsInstanceOf( const TGPType& inClass ) const
		{
			return GetClass().IsTypeOf( inClass );
		}

	///
	/// Conversion de ces données en chaîne.
	/// Par défaut, lance une exception.
	///
	/// \param inContext	contexte de l'évaluation.
	/// \return une représentation sous forme de chaîne des données.
	/// \throws runtime_error si l'objet ne peut être converti en chaîne.
	///
	virtual const std::string&	AsString( TGPEvaluationContext* inContext ) const;
	
	///
	/// Conversion de ces données en booléen.
	/// Par défaut, lance une exception.
	///
	/// \param inContext	contexte de l'évaluation.
	/// \return un booléen qui représente les données.
	/// \throws runtime_error si l'objet ne peut être converti en booléen.
	///
	virtual Boolean				AsBoolean( TGPEvaluationContext* inContext ) const;
	
	///
	/// Conversion de ces données en entier (signé, 32 bits).
	/// Par défaut, lance une exception.
	///
	/// \param inContext	contexte de l'évaluation.
	/// \return une représentation sous forme d'un entier des données.
	/// \throws runtime_error si l'objet ne peut être converti en entier.
	///
	virtual KSInt32				AsInteger( TGPEvaluationContext* inContext ) const;
	
	///
	/// Test d'égalité.
	///
	/// \param inAlter		données avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est égal à \c inAlter
	///
	virtual Boolean Equal(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const = 0;

	///
	/// Relation d'ordre.
	///
	/// \param inAlter		données avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est strictement supérieur à \c inAlter
	///
	virtual Boolean GreaterThan(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const = 0;

private:
	///
	/// Opérateur d'égalité.
	/// Désuet.
	///
	/// \param inAlter données avec lesquelles comparer \c this
	/// \return \c true si \c this est égal à \c inAlterData
	///
	Boolean operator == ( const TGPData& inAlter ) const;

	///
	/// Opérateur de comparaison.
	/// Désuet.
	///
	/// \param inAlter données avec lesquelles comparer \c this
	/// \return \c true si \c this est strictement supérieur à \c inAlter.
	///
	Boolean operator > ( const TGPData& inAlter ) const;

public:
	///
	/// Écrit la donnée sur un flux.
	///
	/// \param inStream	flux considéré.
	///
	virtual void	PrintToStream( std::ostream& inStream ) const = 0;

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPData( const TGPData& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPData& operator = ( const TGPData& inCopy );
	
	/// \name Variables
	const TGPType&	mClass;	///< Classe de cette donnée.
};

// -------------------------------------------------------------------------- //
//  * operator << ( std::ostream&, const TGPData& )
// -------------------------------------------------------------------------- //
///
/// Opérateur pour écrire sur un flux.
///
inline std::ostream&
operator << ( std::ostream& inStream, const TGPData& inData )
{
	inData.PrintToStream( inStream );
	return inStream;
}

#endif
		// _TGPDATA_H

// ============================================ //
// Digital circuits are made from analog parts. //
//                 -- Don Vonada                //
// ============================================ //
