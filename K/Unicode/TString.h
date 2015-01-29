// ==============================
// Fichier:			TString.h
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
// The Original Code is TString.h.
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
// $Id: TString.h,v 1.8 2004/11/24 18:23:18 pguyot Exp $
// ===========

#ifndef _TSTRING_H
#define _TSTRING_H

#include <K/Unicode/UnicodeDefinitions.h>
#include <K/Misc/TRefCounter.h>

// ISO C++
#include <iostream>

// K
#include <K/Unicode/UUTF16Conv.h>

///
/// Classe pour une chaîne dans un encodage quelconque.
/// Cette classe gère l'égalité avec une valeur de hachage. En cas d'inégalité
/// de ces valeurs, elle procède à la comparaison.
/// À utiliser avec TStringRef.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.8 $
///
/// \test	aucun test défini.
///
class TString
	:
		private TRefCounter
{
	/// Les classes qui s'occupent des références sont des amies.
	friend class TStringRef;
	friend class TMutableStringRef;

public:
	///
	/// Encodages pour des chaînes.
	///
	enum EEncoding {
		kUTF8,						///< UTF-8
		kUTF16,						///< UTF-16
		kUSASCII,					///< US-ASCII
		kISO88591,					///< ISO 8859-1, aka Latin1
		kISO88592,					///< ISO 8859-2, aka Latin2
		kMacRoman					///< MacRoman
	};

	///
	/// Constructeur à partir d'une chaîne C (copiée) en ISO-8859-1.
	///
	/// \param inCString 	chaîne de caractères C.
	///
	TString( const char* inCString );

	///
	/// Constructeur à partir d'une chaîne C (copiée) sur 8 bits.
	///
	/// \param inCString 	chaîne de caractères C.
	/// \param inEncoding	encodage de la chaîne.
	///
	TString( const char* inCString, EEncoding inEncoding );

	///
	/// Constructeur à partir d'une chaîne C (copiée) en UTF-16.
	///
	/// \param inCString 	chaîne de caractères C.
	///
	TString( const KUInt16* inCString );

	///
	/// Constructeur à partir de caractères ISO 8859-1 et d'une taille.
	///
	/// \param inChars		caractères
	/// \param inSize		taille
	///
	TString( const char* inChars, KUInt32 inSize );

	///
	/// Constructeur à partir de caractères 8 bits et d'une taille.
	///
	/// \param inChars		caractères
	/// \param inSize		taille
	/// \param inEncoding	encodage de la chaîne.
	///
	TString( const char* inChars, KUInt32 inSize, EEncoding inEncoding );

	///
	/// Constructeur à partir de caractères UTF-16 et d'une taille.
	///
	/// \param inChars		caractères
	/// \param inSize		taille
	///
	TString( const KUInt16* inChars, KUInt32 inSize );

	///
	/// Constructeur par copie.
	///
	/// \param inCopy		objet à copier
	///
	TString( const TString& inCopy );

	///
	/// Opérateur d'assignation.
	///
	/// \param inCopy		objet à copier
	///
	TString& operator = ( const TString& inCopy );

	///
	/// Destructeur.
	/// Libère la mémoire tampon.
	///
	virtual ~TString( void );

	///
	/// Égalité.
	/// Compare la valeur de hachage et la taille s'ils sont valides.
	/// Sinon, les calcule. C'est pour ceci que l'autre chaîne n'est pas \c const.
	///
	/// \param inAlter		objet à comparer avec \c this.
	/// \return \c true si \c this est égal à \c inAlter, \c false sinon.
	///
	Boolean operator == ( TString& inAlter ) const;

	///
	/// Égalité.
	/// Compare la valeur de hachage et la taille s'ils sont valides.
	/// Sinon, fait sans. Cette fonction ne modifie pas inAlter.
	///
	/// \param inAlter		objet à comparer avec \c this.
	/// \return \c true si \c this est égal à \c inAlter, \c false sinon.
	///
	Boolean operator == ( const TString& inAlter ) const;

	///
	/// Comparaison (ordre quelconque compatible avec l'égalité).
	/// L'ordre est le suivant: d'abord la valeur de hachage, ensuite
	/// la taille et enfin l'ordre lexicographique.
	///
	/// \param inAlter		objet à comparer avec \c this.
	/// \return \c true si \c this est égal à \c inAlter, \c false sinon.
	///
	Boolean operator < ( TString& inAlter ) const;

	///
	/// Écrit la chaîne sur un flux (8 bits).
	/// En POSIX, détermine l'encodage par les variables globales.
	///
	/// \param inStream	flux considéré.
	///
	void		PrintToStream( std::ostream& inStream ) const;

	///
	/// Convertit la chaîne et la copie dans une mémoire tampon.
	/// Cette méthode peut convertir \c this en UTF-16.
	///
	/// \param outBuffer 	mémoire tampon
	/// \param ioLength		en entrée, nombre d'octets disponibles,
	///						en sortie, nombre d'octets écrits.
	/// \param inEncoding	encodage cible.
	/// \return le résultat de la conversion.
	///
	UUTF16Conv::EResult	Convert(
							void* outBuffer,
							size_t* ioLength,
							EEncoding inEncoding );

protected:
	///
	/// Constructeur par défaut. La chaîne est en UTF-16.
	///
	TString( void );

private:
	///
	/// Comparaison. \c this et \c inAlter doivent être en ISO-8859-1 ou en US ASCII
	///
	/// \param inAlter		chaîne (encodée en ISO-8859-1 ou US ASCII) à
	///						comparer avec \c this.
	/// \return \c 0 si les deux chaînes sont égales, \c -1 si *this < inAlter,
	///			\c 1 si *this > inAlter sinon.
	///
	int			CompareLatin1Strings( const TString& inAlter ) const;

	///
	/// Comparaison. \c this et \c inAlter doivent être en UTF-16.
	///
	/// \param inAlter		chaîne (encodée en UTF-16) à comparer avec \c this.
	/// \return \c true si les deux chaînes sont égales, \c false sinon.
	///
	int			CompareUTF16Strings( const TString& inAlter ) const;
	
	///
	/// Comparaison des caractères octet par octet. \c this et \c inAlter doivent
	/// être dans le même encodage. La valeur de hachage n'est pas mise à jour.
	///
	/// \param inAlter		chaîne (encodée comme \c this) à
	///						comparer avec \c this.
	/// \return \c true si les deux chaînes sont égales, \c false sinon.
	///
	int			Compare8BitsStrings( const TString& inAlter ) const;

	///
	/// Calcule la taille et la valeur de hachage.
	///

	/// \name Constantes privées
	enum {
		kHashMagic = 0x9E3779B9	///< Et oui, le hash, c'est magique.
	};

	/// \name Variables
	EEncoding			mEncoding;			///< Encodage de la chaîne.
	void*				mString;			///< Mémoire tampon.
	mutable KUInt32		mHashCode;			///< Valeur de hachage.
	mutable KUInt32		mStringSize;		///< Taille de la chaîne (en cache, en caractères).
	KUInt32				mBufferSize;		///< Taille de la mémoire tampon (en octets).
	mutable Boolean		mHashCodeIsValid;	///< Si la valeur de hachage a été calculée.
	mutable Boolean		mStringSizeIsValid;	///< Si la taille de la chaîne a été calculée.
};

// -------------------------------------------------------------------------- //
//  * operator << ( std::ostream&, const TString& )
// -------------------------------------------------------------------------- //
///
/// Opérateur pour écrire sur un flux.
///
inline std::ostream&
operator << ( std::ostream& inStream, const TString& inString )
{
	inString.PrintToStream( inStream );
	return inStream;
}

#endif
		// _TSTRING_H

// ================================================================= //
// UNIX was half a billion (500000000) seconds old on                //
// Tue Nov  5 00:53:20 1985 GMT (measuring since the time(2) epoch). //
//                 -- Andy Tannenbaum                                //
// ================================================================= //

