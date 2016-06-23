// ==============================
// Fichier:			TGPStringData.h
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
// The Original Code is TGPStringData.h.
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
// $Id: TGPStringData.h,v 1.4 2005/02/14 20:55:02 pguyot Exp $
// ===========

#ifndef _TGPSTRINGDATA_H
#define _TGPSTRINGDATA_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/TGPData.h>

// ISO C++
#include <string>

///
/// Classe pour des donn�es sous forme de cha�ne.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test d�fini.
///
class TGPStringData
	:
		public TGPData
{
protected:
	///
	/// Classe de TGPStringData
	///
	class TClass
		:
			public TGPType
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
				return &TGPStringData::kClass == &inOtherClass;
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
	/// Constructeur � partir d'une cha�ne et d'une classe.
	///
	/// \param inString		cha�ne pour ces donn�es (copi�e).
	/// \param inClass		classe pour ces donn�es.
	///
	TGPStringData( const std::string inString, const TGPType& inClass = kClass );

	///
	/// R�cup�ration de la cha�ne pour ces donn�es.
	///
	/// \param inContext	contexte de l'�valuation.
	/// \return la cha�ne pour ces donn�es.
	///
	virtual const std::string&	AsString( TGPEvaluationContext* inContext ) const;
	
	///
	/// Test d'�galit�.
	/// Compare les cha�nes.
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
	/// Compare les cha�nes.
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
	const std::string	mString;	///< Cha�ne
};

#endif
		// _TGPSTRINGDATA_H

// =============================================================================== //
//         A manager was about to be fired, but a programmer who worked for him    //
// invented a new program that became popular and sold well.  As a result, the     //
// manager retained his job.                                                       //
//         The manager tried to give the programmer a bonus, but the programmer    //
// refused it, saying, "I wrote the program because I though it was an interesting //
// concept, and thus I expect no reward."                                          //
//         The manager, upon hearing this, remarked, "This programmer, though he   //
// holds a position of small esteem, understands well the proper duty of an        //
// employee.  Lets promote him to the exalted position of management consultant!"  //
//         But when told this, the programmer once more refused, saying, "I exist  //
// so that I can program.  If I were promoted, I would do nothing but waste        //
// everyone's time.  Can I go now?  I have a program that I'm working on."         //
//                 -- Geoffrey James, "The Tao of Programming"                     //
// =============================================================================== //
