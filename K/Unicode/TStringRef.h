// ==============================
// Fichier:			TStringRef.h
// Projet:			K
// 
// Cr�� le:			3/8/2003
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
/// Classe pour une r�f�rence sur une cha�ne de caract�res.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.11 $
///
/// \test	aucun test d�fini.
///
class TStringRef
{
public:
	///
	/// Constructeur par d�faut.
	///
	TStringRef( void );

	///
	/// Constructeur � partir d'un objet TString.
	///
	/// \param inString	objet TString
	///
	TStringRef( TString* inString );

	///
	/// Constructeur � partir d'un objet TString sur la pile.
	///
	/// \param inString	objet TString
	///
	TStringRef( TString& inString );

	///
	/// Constructeur par copie.
	/// Incr�mente la r�f�rence.
	///
	/// \param inCopy		objet � copier
	///
	TStringRef( const TStringRef& inCopy );

	///
	/// Constructeur � partir d'une cha�ne C.
	///
	/// \param inString	cha�ne C.
	///
	TStringRef( const char* inString );

	///
	/// Constructeur � partir d'une cha�ne C++.
	///
	/// \param inString	cha�ne C++.
	///
	TStringRef( const std::string& inString );

	///
	/// Constructeur � partir d'une r�f�rence modifiable.
	/// Incr�mente la r�f�rence.
	///
	/// \param inCopy		objet � copier
	///
	TStringRef( const TMutableStringRef& inCopy );

	///
	/// Destructeur.
	///
	~TStringRef( void );

	///
	/// Op�rateur d'assignation (TString*)
	///
	/// \param inCopy		objet � copier
	///
	TStringRef& operator = ( TString* inCopy );

	///
	/// Op�rateur d'assignation (const char*)
	///
	/// \param inString		cha�ne � copier
	///
	TStringRef& operator = ( const char* inString );

	///
	/// Op�rateur d'assignation (const string)
	///
	/// \param inString		cha�ne � copier
	///
	TStringRef& operator = ( const std::string& inString );

	///
	/// Op�rateur d'assignation (const TStringRef&)
	///
	/// \param inCopy		objet � copier
	///
	TStringRef& operator = ( const TStringRef& inCopy );

	///
	/// Op�rateur d'assignation avec une r�f�rence modifiable.
	///
	/// \param inCopy		objet � copier
	///
	TStringRef& operator = ( const TMutableStringRef& inCopy );

	///
	/// Op�rateur de comparaison (ordre quelconque compatible avec l'�galit�).
	///
	/// \param inAlter		objet � comparer avec \c this.
	/// \return \c true si \c this est strictement inf�rieur � \c inAlter, \c false sinon.
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
	/// Op�rateur de comparaison (ordre quelconque compatible avec l'�galit�).
	///
	/// \param inAlter		objet � comparer avec \c this.
	/// \return \c true si \c this est strictement inf�rieur � \c inAlter, \c false sinon.
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
	/// Op�rateur d'�galit�.
	///
	/// \param inAlter		objet � comparer avec \c this.
	/// \return \c true si \c this est �gal � \c inAlter, \c false sinon.
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
	/// Op�rateur d'�galit�.
	///
	/// \param inAlter		objet � comparer avec \c this.
	/// \return \c true si \c this est �gal � \c inAlter, \c false sinon.
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
	/// Op�rateur de d�r�f�rencement
	///
	/// \return une r�f�rence sur l'objet TString
	///
	inline const TString& operator * ( void ) const
		{
			return *mString;
		}

	///
	/// Op�rateur de d�r�f�rencement
	///
	/// \return une r�f�rence sur l'objet TString
	///
	inline const TString* operator -> ( void ) const
		{
			return mString;
		}

	///
	/// Accesseur sur l'objet TString
	///
	/// \return une r�f�rence sur l'objet TString
	///
	inline const TString* GetString( void ) const
		{
			return mString;
		}

	///
	/// �crit la cha�ne sur un flux.
	///
	/// \param inStream	flux consid�r�.
	///
	inline void		PrintToStream( std::ostream& inStream ) const
		{
			if (mString)
			{
				mString->PrintToStream( inStream );
			}
		}

	///
	/// Convertit la cha�ne et la copie dans une m�moire tampon.
	/// Cette m�thode peut convertir mString en UTF-16.
	///
	/// \param outBuffer 	m�moire tampon
	/// \param ioLength		en entr�e, nombre d'octets disponibles,
	///						en sortie, nombre d'octets �crits.
	/// \param inEncoding	encodage cible.
	/// \return le r�sultat de la conversion.
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
	TString*		mString;	///< Objet r�f�renc�.
};

// -------------------------------------------------------------------------- //
//  * operator << ( std::ostream&, const TStringRef& )
// -------------------------------------------------------------------------- //
///
/// Op�rateur pour �crire sur un flux.
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
