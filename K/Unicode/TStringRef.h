// ==============================
// Fichier:			TStringRef.h
// Projet:			K
// 
// Créé le:			3/8/2003
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
// The Original Code is TStringRef.h.
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
// $Id: TStringRef.h,v 1.11 2005/02/14 20:55:06 pguyot Exp $
// ===========

#ifndef _TSTRINGREF_H
#define _TSTRINGREF_H

#include <K/Unicode/UnicodeDefinitions.h>

// ISO C++
#include <iostream>
#include <string>

// K
#include <K/Unicode/TString.h>
#include <K/Unicode/TMutableStringRef.h>

///
/// Classe pour une référence sur une chaîne de caractères.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.11 $
///
/// \test	aucun test défini.
///
class TStringRef
{
public:
	///
	/// Constructeur par défaut.
	///
	TStringRef( void );

	///
	/// Constructeur à partir d'un objet TString.
	///
	/// \param inString	objet TString
	///
	TStringRef( TString* inString );

	///
	/// Constructeur à partir d'un objet TString sur la pile.
	///
	/// \param inString	objet TString
	///
	TStringRef( TString& inString );

	///
	/// Constructeur par copie.
	/// Incrémente la référence.
	///
	/// \param inCopy		objet à copier
	///
	TStringRef( const TStringRef& inCopy );

	///
	/// Constructeur à partir d'une chaîne C.
	///
	/// \param inString	chaîne C.
	///
	TStringRef( const char* inString );

	///
	/// Constructeur à partir d'une chaîne C++.
	///
	/// \param inString	chaîne C++.
	///
	TStringRef( const std::string& inString );

	///
	/// Constructeur à partir d'une référence modifiable.
	/// Incrémente la référence.
	///
	/// \param inCopy		objet à copier
	///
	TStringRef( const TMutableStringRef& inCopy );

	///
	/// Destructeur.
	///
	~TStringRef( void );

	///
	/// Opérateur d'assignation (TString*)
	///
	/// \param inCopy		objet à copier
	///
	TStringRef& operator = ( TString* inCopy );

	///
	/// Opérateur d'assignation (const char*)
	///
	/// \param inString		chaîne à copier
	///
	TStringRef& operator = ( const char* inString );

	///
	/// Opérateur d'assignation (const string)
	///
	/// \param inString		chaîne à copier
	///
	TStringRef& operator = ( const std::string& inString );

	///
	/// Opérateur d'assignation (const TStringRef&)
	///
	/// \param inCopy		objet à copier
	///
	TStringRef& operator = ( const TStringRef& inCopy );

	///
	/// Opérateur d'assignation avec une référence modifiable.
	///
	/// \param inCopy		objet à copier
	///
	TStringRef& operator = ( const TMutableStringRef& inCopy );

	///
	/// Opérateur de comparaison (ordre quelconque compatible avec l'égalité).
	///
	/// \param inAlter		objet à comparer avec \c this.
	/// \return \c true si \c this est strictement inférieur à \c inAlter, \c false sinon.
	///
	inline Boolean operator < ( const TStringRef& inAlter ) const
		{
			return
				((mString != inAlter.mString)
				&& (mString != nil)
				&& (inAlter.mString != nil)
				&& (*mString < *inAlter.mString));
		}

	///
	/// Opérateur de comparaison (ordre quelconque compatible avec l'égalité).
	///
	/// \param inAlter		objet à comparer avec \c this.
	/// \return \c true si \c this est strictement inférieur à \c inAlter, \c false sinon.
	///
	inline Boolean operator < ( const TMutableStringRef& inAlter ) const
		{
			return
				((mString != inAlter.GetString())
				&& (mString != nil)
				&& (inAlter.GetString() != nil)
				&& (*mString < *inAlter.GetString()));
		}

	///
	/// Opérateur d'égalité.
	///
	/// \param inAlter		objet à comparer avec \c this.
	/// \return \c true si \c this est égal à \c inAlter, \c false sinon.
	///
	inline Boolean operator == ( const TStringRef& inAlter ) const
		{
			return
				((mString == inAlter.mString)
				||
					(mString != nil)
					&& (inAlter.mString != nil)
					&& (*mString == *inAlter.mString));
		}

	///
	/// Opérateur d'égalité.
	///
	/// \param inAlter		objet à comparer avec \c this.
	/// \return \c true si \c this est égal à \c inAlter, \c false sinon.
	///
	inline Boolean operator == ( const TMutableStringRef& inAlter ) const
		{
			return
				((mString == inAlter.GetString())
				||
					(mString != nil)
					&& (inAlter.GetString() != nil)
					&& (*mString == *inAlter.GetString()));
		}

	///
	/// Opérateur de déréférencement
	///
	/// \return une référence sur l'objet TString
	///
	inline const TString& operator * ( void ) const
		{
			return *mString;
		}

	///
	/// Opérateur de déréférencement
	///
	/// \return une référence sur l'objet TString
	///
	inline const TString* operator -> ( void ) const
		{
			return mString;
		}

	///
	/// Accesseur sur l'objet TString
	///
	/// \return une référence sur l'objet TString
	///
	inline const TString* GetString( void ) const
		{
			return mString;
		}

	///
	/// Écrit la chaîne sur un flux.
	///
	/// \param inStream	flux considéré.
	///
	inline void		PrintToStream( std::ostream& inStream ) const
		{
			if (mString)
			{
				mString->PrintToStream( inStream );
			}
		}

	///
	/// Convertit la chaîne et la copie dans une mémoire tampon.
	/// Cette méthode peut convertir mString en UTF-16.
	///
	/// \param outBuffer 	mémoire tampon
	/// \param ioLength		en entrée, nombre d'octets disponibles,
	///						en sortie, nombre d'octets écrits.
	/// \param inEncoding	encodage cible.
	/// \return le résultat de la conversion.
	///
	inline UUTF16Conv::EResult	Convert(
							void* outBuffer,
							size_t* ioLength,
							TString::EEncoding inEncoding ) const
		{
			if (mString)
			{
				return mString->Convert( outBuffer, ioLength, inEncoding );
			} else {
				*ioLength = 0;
				return UUTF16Conv::kInputExhausted;
			}
		}

private:
	/// \name Variables
	TString*		mString;	///< Objet référencé.
};

// -------------------------------------------------------------------------- //
//  * operator << ( std::ostream&, const TStringRef& )
// -------------------------------------------------------------------------- //
///
/// Opérateur pour écrire sur un flux.
///
inline std::ostream&
operator << ( std::ostream& inStream, const TStringRef& inStringRef )
{
	inStringRef.PrintToStream( inStream );
	return inStream;
}

#endif
		// _TSTRINGREF_H

// ============================== //
// APL hackers do it in the quad. //
// ============================== //
