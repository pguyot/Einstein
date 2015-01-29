// ==============================
// Fichier:			TGPDataRef.h
// Projet:			K
//
// Cr�� le:			13/2/2003
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
/// Classe pour une r�f�rence sur des donn�es.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
class TGPDataRef
{
public:
	///
	/// Constructeur par d�faut.
	/// Initialise le pointeur � \c nil
	///
	TGPDataRef( void );

	///
	/// Constructeur � partir d'un pointeur sur des donn�es.
	///
	/// \param inData		donn�es pour cette r�f�rence.
	///
	TGPDataRef( TGPData* inData );

	///
	/// Constructeur � partir de donn�es sur la pile.
	///
	/// \param inData		donn�es pour cette r�f�rence.
	///
	TGPDataRef( TGPData& inData );

	///
	/// Constructeur � partir d'une cha�ne.
	/// Cr�e un objet de type TGPStringData.
	///
	/// \param inString		cha�ne pour cette r�f�rence.
	///
	TGPDataRef( const std::string inString );

	///
	/// Constructeur � partir d'un bool�en.
	/// Cr�e un objet de type TGPBooleanData.
	///
	/// \param inBoolean	boole�n pour cette r�f�rence.
	///
	TGPDataRef( Boolean inBoolean );

	///
	/// Constructeur � partir d'un entier sign�.
	/// Cr�e un objet de type TGPSInt32Data.
	///
	/// \param inInteger	entier pour cette r�f�rence.
	///
	TGPDataRef( KSInt32 inInteger );

	///
	/// Constructeur par copie.
	/// Incr�mente le compteur de r�f�rences.
	///
	/// \param inCopy		objet � copier
	///
	TGPDataRef( const TGPDataRef& inCopy );

	///
	/// Destructeur.
	/// Non virtuel parce que cette classe se limite au pointeur sur les donn�es.
	///
	~TGPDataRef( void );

	///
	/// Op�rateur d'assignation avec un objet de type TGPDataRef.
	///
	/// \param inCopy		objet � copier
	///
	TGPDataRef& operator = ( const TGPDataRef& inCopy );

	///
	/// Op�rateur d'assignation avec des donn�es.
	///
	/// \param inData		donn�es pour cette r�f�rence.
	///
	TGPDataRef& operator = ( TGPData* inData );

	///
	/// Op�rateur d'assignation avec une cha�ne.
	/// Construit un objet TGPData associ�.
	///
	/// \param inString		cha�ne pour cette r�f�rence.
	///
	TGPDataRef& operator = ( const std::string inString );

	///
	/// Op�rateur d'assignation avec un bool�en.
	/// Construit un objet TGPData associ�.
	///
	/// \param inBoolean	bool�en pour cette r�f�rence.
	///
	TGPDataRef& operator = ( Boolean inBoolean );

	///
	/// Op�rateur d'assignation avec un entier.
	/// Construit un objet TGPData associ�.
	///
	/// \param inInteger	entier pour cette r�f�rence.
	///
	TGPDataRef& operator = ( KSInt32 inInteger );

	///
	/// Accesseur sur la classe des donn�es.
	///
	/// \return une r�f�rence sur la classe des donn�es.
	///
	inline	const TGPType& GetClass( void ) const
		{
			KASSERT( mData != nil );
			return mData->GetClass();
		}

	///
	/// D�termine si les donn�es sont d'une certaine classe.
	///
	/// \param inClass	classe pour le test.
	/// \return \c true si mData est de la classe \c inClass
	///
	inline	Boolean IsInstanceOf( const TGPType& inClass ) const
		{
			return ((mData != nil) && mData->IsInstanceOf( inClass ));
		}

	///
	/// D�termine si les donn�es sont nulles.
	///
	/// \return \c true si les donn�es sont nulles.
	///
	inline	Boolean IsNil( void ) const
		{
			return (mData == nil);
		}

	///
	/// Accesseur sur les donn�es.
	///
	/// \return un pointeur sur les donn�es (constant).
	///
	inline	const TGPData* GetData( void ) const
		{
			return mData;
		}

	///
	/// Accesseur sur les donn�es.
	///
	/// \return un pointeur sur les donn�es (variable).
	///
	inline	TGPData* GetData( void )
		{
			return mData;
		}

	///
	/// Op�rateur de d�r�f�rencement.
	///
	/// \return un pointeur sur les don�es (constant).
	///
	inline	const TGPData* operator -> ( void ) const
		{
			return mData;
		}

	///
	/// Op�rateur de d�r�f�rencement.
	///
	/// \return un pointeur sur les don�es (variable).
	///
	inline	TGPData* operator -> ( void )
		{
			return mData;
		}

	///
	/// Op�rateur de d�r�f�rencement.
	///
	/// \return un pointeur sur les don�es (constant).
	///
	inline	const TGPData* operator * ( void ) const
		{
			return mData;
		}

	///
	/// Op�rateur de d�r�f�rencement.
	///
	/// \return un pointeur sur les don�es (variable).
	///
	inline	TGPData* operator * ( void )
		{
			return mData;
		}

	///
	/// R�cup�ration de la cha�ne pour ces donn�es.
	/// Demande � la sous-classe de TGPData.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return une repr�sentation sous forme de cha�ne des donn�es.
	/// \throws runtime_error si l'objet n'est pas une cha�ne.
	///
	const std::string&	AsString( TGPEvaluationContext* inContext ) const;
	
	///
	/// Conversion des donn�es en bool�en.
	/// Demande � la sous-classe de TGPData.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return un bool�en qui repr�sente les donn�es.
	/// \throws runtime_error si l'objet n'est pas un bool�en.
	///
	Boolean				AsBoolean( TGPEvaluationContext* inContext ) const;
	
	///
	/// Conversion des donn�es en entier.
	/// Demande � la sous-classe de TGPData.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return une repr�sentation sous forme d'un entier des donn�es.
	/// \throws runtime_error si l'objet n'est pas un entier.
	///
	KSInt32				AsInteger( TGPEvaluationContext* inContext ) const;

	///
	/// Test d'�galit�.
	/// Compare les donn�es.
	///
	/// \param inAlterRef	donn�es avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est �gal � \c inAlterRef
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
	/// Compare les donn�es.
	///
	/// \param inAlterRef	donn�es avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est �gal � \c inAlterRef
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
	/// Op�rateur d'�galit�.
	/// D�suet.
	///
	/// \param inAlterRef donn�es avec lesquelles comparer \c this
	/// \return \c true si \c this est �gal � \c inAlterRef
	///
	inline Boolean operator == ( const TGPDataRef& inAlterRef ) const;

	///
	/// Op�rateur de comparaison.
	/// D�suet.
	///
	/// \param inAlterRef donn�es avec lesquelles comparer \c this
	/// \return \c true si \c this est strictement sup�rieur � \c inAlter.
	///
	inline Boolean operator > ( const TGPDataRef& inAlterRef ) const;

public:
	///
	/// �crit la donn�e sur un flux.
	///
	/// \param inStream	flux consid�r�.
	///
	void		PrintToStream( std::ostream& inStream ) const;

private:
	/// \name Variables
	TGPData*		mData;	///< Pointeur sur les donn�es.

	/// \name Constantes
	static const std::string	kNILString;	///< "&lt;nil&gt;" pour le nom d'une
											///< donn�e � \c nil
};

// -------------------------------------------------------------------------- //
//  * operator << ( std::ostream&, const TGPDataRef& )
// -------------------------------------------------------------------------- //
///
/// Op�rateur pour �crire sur un flux.
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
