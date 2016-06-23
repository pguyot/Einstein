// ==============================
// Fichier:			TGPSInt32Data.h
// Projet:			K
//
// Cr�� le:			23/2/2003
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
// The Original Code is TGPSInt32Data.h.
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
// $Id: TGPSInt32Data.h,v 1.3 2005/02/14 20:55:02 pguyot Exp $
// ===========

#ifndef _TGPSINT32DATA_H
#define _TGPSINT32DATA_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/Numbers/TGPNumberData.h>

// ISO C++
#include <string>

///
/// Classe pour des donn�es sous forme d'entier.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
class TGPSInt32Data
	:
		public TGPNumberData
{
protected:
	///
	/// Classe de TGPSInt32Data
	///
	class TClass
		:
			public TGPNumberData::TClass
	{
	public:
		///
		/// Accesseur sur le nom de la classe de cette objet.
		///
		/// \return une cha�ne repr�sentant la classe de l'objet.
		///
		virtual TStringRef GetName( void ) const
			{
				return kName;
			}

		///
		/// D�termine si cette classe est �gale ou une sous-classe
		/// d'une classe donn�e.
		///
		/// \param inOtherClass	classe pour le test.
		/// \return \c true si cet objet est un �l�ment de la classe \c inOtherClass
		///
		virtual Boolean IsTypeOf( const TGPType& inOtherClass ) const
			{
				return (&TGPSInt32Data::kClass == &inOtherClass)
					|| (TGPNumberData::TClass::IsTypeOf( inOtherClass ));
			}
		
		///
		/// R�cup�re une valeur � partir d'une cha�ne.
		///
		/// \param outValue	valeur (en sortie).
		/// \return \c true si la cha�ne a pu �tre analys�e correctement.
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
	/// Objet qui repr�sente cette classe.
	///	
	static const TClass	kClass;	

	///
	/// Constructeur � partir d'un entier et d'une classe.
	///
	/// \param inInteger	entier pour cette donn�e.
	/// \param inClass		classe pour cette donn�e.
	///
	TGPSInt32Data(
				KSInt32 inInteger,
				const TGPType& inClass = kClass );

	///
	/// R�cup�ration de la cha�ne pour ces donn�es.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return une repr�sentation sous forme de cha�ne des donn�es.
	///
	virtual const std::string&	AsString( TGPEvaluationContext* inContext ) const;

	///
	/// R�cup�ration de l'entier pour ces donn�es.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return une repr�sentation sous forme d'un entier des donn�es.
	///
	virtual KSInt32			AsInteger( TGPEvaluationContext* inContext ) const;

	///
	/// Test d'�galit�.
	///
	/// \param inAlter		donn�es avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est �gal � \c inAlter
	///
	virtual Boolean Equal(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const;

	///
	/// Relation d'ordre.
	///
	/// \param inAlter		donn�es avec lesquelles comparer \c this
	/// \param inContext	contexte pour la comparaison.
	/// \return \c true si \c this est strictement sup�rieur � \c inAlter
	///
	virtual Boolean GreaterThan(
				const TGPData& inAlter,
				TGPEvaluationContext* inContext ) const;

	///
	/// �crit la donn�e sur un flux.
	///
	/// \param inStream	flux consid�r�.
	///
	virtual void	PrintToStream( std::ostream& inStream ) const;

private:
	/// \name Variables
	KSInt32					mInteger;
	std::string				mIntStr;
};

#endif
		// _TGPSINT32DATA_H

// ======================================================================= //
// The nicest thing about the Alto is that it doesn't run faster at night. //
// ======================================================================= //
