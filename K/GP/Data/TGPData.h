// ==============================
// Fichier:			TGPData.h
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
/// Classe de base pour des donn�es.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
class TGPData
	:
		private TRefCounter
{
	///
	/// La classe pour la r�f�rence sur les donn�es est une amie.
	///
	friend class TGPDataRef;

public:
	///
	/// Constructeur � partir d'une classe.
	///
	TGPData( const TGPType& inClass );

	///
	/// Destructeur.
	///
	virtual ~TGPData( void );

	///
	/// R�cup�ration de la classe de ces donn�es.
	///
	/// \return une r�f�rence sur la classe de ces donn�es.
	///
	inline const TGPType&	GetClass( void ) const
		{
			return mClass;
		}

	///
	/// D�termine si cet objet est d'une classe donn�e (�ventuellement
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
	/// Conversion de ces donn�es en cha�ne.
	/// Par d�faut, lance une exception.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return une repr�sentation sous forme de cha�ne des donn�es.
	/// \throws runtime_error si l'objet ne peut �tre converti en cha�ne.
	///
	virtual const std::string&	AsString( TGPEvaluationContext* inContext ) const;
	
	///
	/// Conversion de ces donn�es en bool�en.
	/// Par d�faut, lance une exception.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return un bool�en qui repr�sente les donn�es.
	/// \throws runtime_error si l'objet ne peut �tre converti en bool�en.
	///
	virtual Boolean				AsBoolean( TGPEvaluationContext* inContext ) const;
	
	///
	/// Conversion de ces donn�es en entier (sign�, 32 bits).
	/// Par d�faut, lance une exception.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return une repr�sentation sous forme d'un entier des donn�es.
	/// \throws runtime_error si l'objet ne peut �tre converti en entier.
	///
	virtual KSInt32				AsInteger( TGPEvaluationContext* inContext ) const;
	
	///
	/// Test d'�galit�.
	///
	/// \param inAlter		donn�es avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est �gal � \c inAlter
	///
	virtual Boolean Equal(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const = 0;

	///
	/// Relation d'ordre.
	///
	/// \param inAlter		donn�es avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est strictement sup�rieur � \c inAlter
	///
	virtual Boolean GreaterThan(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const = 0;

private:
	///
	/// Op�rateur d'�galit�.
	/// D�suet.
	///
	/// \param inAlter donn�es avec lesquelles comparer \c this
	/// \return \c true si \c this est �gal � \c inAlterData
	///
	Boolean operator == ( const TGPData& inAlter ) const;

	///
	/// Op�rateur de comparaison.
	/// D�suet.
	///
	/// \param inAlter donn�es avec lesquelles comparer \c this
	/// \return \c true si \c this est strictement sup�rieur � \c inAlter.
	///
	Boolean operator > ( const TGPData& inAlter ) const;

public:
	///
	/// �crit la donn�e sur un flux.
	///
	/// \param inStream	flux consid�r�.
	///
	virtual void	PrintToStream( std::ostream& inStream ) const = 0;

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPData( const TGPData& inCopy );

	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TGPData& operator = ( const TGPData& inCopy );
	
	/// \name Variables
	const TGPType&	mClass;	///< Classe de cette donn�e.
};

// -------------------------------------------------------------------------- //
//  * operator << ( std::ostream&, const TGPData& )
// -------------------------------------------------------------------------- //
///
/// Op�rateur pour �crire sur un flux.
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
