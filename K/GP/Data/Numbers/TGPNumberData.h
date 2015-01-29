// ==============================
// Fichier:			TGPNumberData.h
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
// The Original Code is TGPNumberData.h.
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
// $Id: TGPNumberData.h,v 1.3 2005/02/14 20:55:02 pguyot Exp $
// ===========

#ifndef _TGPNUMBERDATA_H
#define _TGPNUMBERDATA_H

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/TGPData.h>

// ISO C++
#include <string>

///
/// Classe (abstraite) pour des données sous forme de nombre.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	aucun test défini.
///
class TGPNumberData
	:
		public TGPData
{
protected:
	///
	/// Classe de TGPNumberData
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
				return &TGPNumberData::kClass == &inOtherClass;
			}
		
		///
		/// Récupère une valeur à partir d'une chaîne.
		/// Retourne \c false (utilisez TGPSInt32Data).
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

protected:
	///
	/// Constructeur à partir d'une classe.
	///
	/// \param inClass		classe pour ce nombre.
	///
	TGPNumberData( const TGPType& inClass = kClass );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPNumberData( const TGPNumberData& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TGPNumberData& operator = ( const TGPNumberData& inCopy );
};

#endif
		// _TGPNUMBERDATA_H

// ============================================================================= //
// The proof that IBM didn't invent the car is that it has a steering wheel      //
// and an accelerator instead of spurs and ropes, to be compatible with a horse. //
//                 -- Jac Goudsmit                                               //
// ============================================================================= //
