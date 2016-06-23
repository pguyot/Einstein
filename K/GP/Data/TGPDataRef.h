// ==============================
// Fichier:			TGPDataRef.h
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
// The Original Code is TGPDataRef.h.
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
// $Id: TGPDataRef.h,v 1.8 2005/02/14 20:55:02 pguyot Exp $
// ===========

#ifndef _TGPDATAREF_H
#define _TGPDATAREF_H

#include <K/Defines/KDefinitions.h>

// ISO C++
#include <string>
#include <iostream>

// ANSI C
#include <stddef.h>

// K
#include <K/GP/Data/TGPData.h>
#include <K/Tests/KDebug.h>

class TGPEvaluationContext;

///
/// Classe pour une référence sur des données.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TGPDataRef
{
public:
	///
	/// Constructeur par défaut.
	/// Initialise le pointeur à \c nil
	///
	TGPDataRef( void );

	///
	/// Constructeur à partir d'un pointeur sur des données.
	///
	/// \param inData		données pour cette référence.
	///
	TGPDataRef( TGPData* inData );

	///
	/// Constructeur à partir de données sur la pile.
	///
	/// \param inData		données pour cette référence.
	///
	TGPDataRef( TGPData& inData );

	///
	/// Constructeur à partir d'une chaîne.
	/// Crée un objet de type TGPStringData.
	///
	/// \param inString		chaîne pour cette référence.
	///
	TGPDataRef( const std::string inString );

	///
	/// Constructeur à partir d'un booléen.
	/// Crée un objet de type TGPBooleanData.
	///
	/// \param inBoolean	booleén pour cette référence.
	///
	TGPDataRef( Boolean inBoolean );

	///
	/// Constructeur à partir d'un entier signé.
	/// Crée un objet de type TGPSInt32Data.
	///
	/// \param inInteger	entier pour cette référence.
	///
	TGPDataRef( KSInt32 inInteger );

	///
	/// Constructeur par copie.
	/// Incrémente le compteur de références.
	///
	/// \param inCopy		objet à copier
	///
	TGPDataRef( const TGPDataRef& inCopy );

	///
	/// Destructeur.
	/// Non virtuel parce que cette classe se limite au pointeur sur les données.
	///
	~TGPDataRef( void );

	///
	/// Opérateur d'assignation avec un objet de type TGPDataRef.
	///
	/// \param inCopy		objet à copier
	///
	TGPDataRef& operator = ( const TGPDataRef& inCopy );

	///
	/// Opérateur d'assignation avec des données.
	///
	/// \param inData		données pour cette référence.
	///
	TGPDataRef& operator = ( TGPData* inData );

	///
	/// Opérateur d'assignation avec une chaîne.
	/// Construit un objet TGPData associé.
	///
	/// \param inString		chaîne pour cette référence.
	///
	TGPDataRef& operator = ( const std::string inString );

	///
	/// Opérateur d'assignation avec un booléen.
	/// Construit un objet TGPData associé.
	///
	/// \param inBoolean	booléen pour cette référence.
	///
	TGPDataRef& operator = ( Boolean inBoolean );

	///
	/// Opérateur d'assignation avec un entier.
	/// Construit un objet TGPData associé.
	///
	/// \param inInteger	entier pour cette référence.
	///
	TGPDataRef& operator = ( KSInt32 inInteger );

	///
	/// Accesseur sur la classe des données.
	///
	/// \return une référence sur la classe des données.
	///
	inline	const TGPType& GetClass( void ) const
		{
			KASSERT( mData != nil );
			return mData->GetClass();
		}

	///
	/// Détermine si les données sont d'une certaine classe.
	///
	/// \param inClass	classe pour le test.
	/// \return \c true si mData est de la classe \c inClass
	///
	inline	Boolean IsInstanceOf( const TGPType& inClass ) const
		{
			return ((mData != nil) && mData->IsInstanceOf( inClass ));
		}

	///
	/// Détermine si les données sont nulles.
	///
	/// \return \c true si les données sont nulles.
	///
	inline	Boolean IsNil( void ) const
		{
			return (mData == nil);
		}

	///
	/// Accesseur sur les données.
	///
	/// \return un pointeur sur les données (constant).
	///
	inline	const TGPData* GetData( void ) const
		{
			return mData;
		}

	///
	/// Accesseur sur les données.
	///
	/// \return un pointeur sur les données (variable).
	///
	inline	TGPData* GetData( void )
		{
			return mData;
		}

	///
	/// Opérateur de déréférencement.
	///
	/// \return un pointeur sur les donées (constant).
	///
	inline	const TGPData* operator -> ( void ) const
		{
			return mData;
		}

	///
	/// Opérateur de déréférencement.
	///
	/// \return un pointeur sur les donées (variable).
	///
	inline	TGPData* operator -> ( void )
		{
			return mData;
		}

	///
	/// Opérateur de déréférencement.
	///
	/// \return un pointeur sur les donées (constant).
	///
	inline	const TGPData* operator * ( void ) const
		{
			return mData;
		}

	///
	/// Opérateur de déréférencement.
	///
	/// \return un pointeur sur les donées (variable).
	///
	inline	TGPData* operator * ( void )
		{
			return mData;
		}

	///
	/// Récupération de la chaîne pour ces données.
	/// Demande à la sous-classe de TGPData.
	///
	/// \param inContext	contexte de l'évaluation.
	/// \return une représentation sous forme de chaîne des données.
	/// \throws runtime_error si l'objet n'est pas une chaîne.
	///
	const std::string&	AsString( TGPEvaluationContext* inContext ) const;
	
	///
	/// Conversion des données en booléen.
	/// Demande à la sous-classe de TGPData.
	///
	/// \param inContext	contexte de l'évaluation.
	/// \return un booléen qui représente les données.
	/// \throws runtime_error si l'objet n'est pas un booléen.
	///
	Boolean				AsBoolean( TGPEvaluationContext* inContext ) const;
	
	///
	/// Conversion des données en entier.
	/// Demande à la sous-classe de TGPData.
	///
	/// \param inContext	contexte de l'évaluation.
	/// \return une représentation sous forme d'un entier des données.
	/// \throws runtime_error si l'objet n'est pas un entier.
	///
	KSInt32				AsInteger( TGPEvaluationContext* inContext ) const;

	///
	/// Test d'égalité.
	/// Compare les données.
	///
	/// \param inAlterRef	données avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est égal à \c inAlterRef
	///
	inline Boolean Equal(
						const TGPDataRef& inAlterRef,
						TGPEvaluationContext* inContext ) const
		{
			return ((mData == *inAlterRef)
				|| ((mData != nil)
					&& (*inAlterRef != nil)
					&& (mData->Equal(**inAlterRef, inContext ))));
		}

	///
	/// Relation d'ordre.
	/// Compare les données.
	///
	/// \param inAlterRef	données avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est égal à \c inAlterRef
	///
	inline Boolean GreaterThan(
						const TGPDataRef& inAlterRef,
						TGPEvaluationContext* inContext ) const
		{
			return ((mData != nil)
					&& (*inAlterRef != nil)
					&& (mData->GreaterThan(**inAlterRef, inContext )));
		}

private:
	///
	/// Opérateur d'égalité.
	/// Désuet.
	///
	/// \param inAlterRef données avec lesquelles comparer \c this
	/// \return \c true si \c this est égal à \c inAlterRef
	///
	inline Boolean operator == ( const TGPDataRef& inAlterRef ) const;

	///
	/// Opérateur de comparaison.
	/// Désuet.
	///
	/// \param inAlterRef données avec lesquelles comparer \c this
	/// \return \c true si \c this est strictement supérieur à \c inAlter.
	///
	inline Boolean operator > ( const TGPDataRef& inAlterRef ) const;

public:
	///
	/// Écrit la donnée sur un flux.
	///
	/// \param inStream	flux considéré.
	///
	void		PrintToStream( std::ostream& inStream ) const;

private:
	/// \name Variables
	TGPData*		mData;	///< Pointeur sur les données.

	/// \name Constantes
	static const std::string	kNILString;	///< "&lt;nil&gt;" pour le nom d'une
											///< donnée à \c nil
};

// -------------------------------------------------------------------------- //
//  * operator << ( std::ostream&, const TGPDataRef& )
// -------------------------------------------------------------------------- //
///
/// Opérateur pour écrire sur un flux.
///
inline std::ostream&
operator << ( std::ostream& inStream, const TGPDataRef& inDataRef )
{
	inDataRef.PrintToStream( inStream );
	return inStream;
}

#endif
		// _TGPDATAREF_H

// ========================= //
// Your fault -- core dumped //
// ========================= //
