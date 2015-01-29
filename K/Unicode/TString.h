// ==============================
// Fichier:			TString.h
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
/// Classe pour une cha�ne dans un encodage quelconque.
/// Cette classe g�re l'�galit� avec une valeur de hachage. En cas d'in�galit�
/// de ces valeurs, elle proc�de � la comparaison.
/// � utiliser avec TStringRef.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.8 $
///
/// \test	aucun test d�fini.
///
class TString
	:
		private TRefCounter
{
	/// Les classes qui s'occupent des r�f�rences sont des amies.
	friend class TStringRef;
	friend class TMutableStringRef;

public:
	///
	/// Encodages pour des cha�nes.
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
	/// Constructeur � partir d'une cha�ne C (copi�e) en ISO-8859-1.
	///
	/// \param inCString 	cha�ne de caract�res C.
	///
	TString( const char* inCString );

	///
	/// Constructeur � partir d'une cha�ne C (copi�e) sur 8 bits.
	///
	/// \param inCString 	cha�ne de caract�res C.
	/// \param inEncoding	encodage de la cha�ne.
	///
	TString( const char* inCString, EEncoding inEncoding );

	///
	/// Constructeur � partir d'une cha�ne C (copi�e) en UTF-16.
	///
	/// \param inCString 	cha�ne de caract�res C.
	///
	TString( const KUInt16* inCString );

	///
	/// Constructeur � partir de caract�res ISO 8859-1 et d'une taille.
	///
	/// \param inChars		caract�res
	/// \param inSize		taille
	///
	TString( const char* inChars, KUInt32 inSize );

	///
	/// Constructeur � partir de caract�res 8 bits et d'une taille.
	///
	/// \param inChars		caract�res
	/// \param inSize		taille
	/// \param inEncoding	encodage de la cha�ne.
	///
	TString( const char* inChars, KUInt32 inSize, EEncoding inEncoding );

	///
	/// Constructeur � partir de caract�res UTF-16 et d'une taille.
	///
	/// \param inChars		caract�res
	/// \param inSize		taille
	///
	TString( const KUInt16* inChars, KUInt32 inSize );

	///
	/// Constructeur par copie.
	///
	/// \param inCopy		objet � copier
	///
	TString( const TString& inCopy );

	///
	/// Op�rateur d'assignation.
	///
	/// \param inCopy		objet � copier
	///
	TString& operator = ( const TString& inCopy );

	///
	/// Destructeur.
	/// Lib�re la m�moire tampon.
	///
	virtual ~TString( void );

	///
	/// �galit�.
	/// Compare la valeur de hachage et la taille s'ils sont valides.
	/// Sinon, les calcule. C'est pour ceci que l'autre cha�ne n'est pas \c const.
	///
	/// \param inAlter		objet � comparer avec \c this.
	/// \return \c true si \c this est �gal � \c inAlter, \c false sinon.
	///
	Boolean operator == ( TString& inAlter ) const;

	///
	/// �galit�.
	/// Compare la valeur de hachage et la taille s'ils sont valides.
	/// Sinon, fait sans. Cette fonction ne modifie pas inAlter.
	///
	/// \param inAlter		objet � comparer avec \c this.
	/// \return \c true si \c this est �gal � \c inAlter, \c false sinon.
	///
	Boolean operator == ( const TString& inAlter ) const;

	///
	/// Comparaison (ordre quelconque compatible avec l'�galit�).
	/// L'ordre est le suivant: d'abord la valeur de hachage, ensuite
	/// la taille et enfin l'ordre lexicographique.
	///
	/// \param inAlter		objet � comparer avec \c this.
	/// \return \c true si \c this est �gal � \c inAlter, \c false sinon.
	///
	Boolean operator < ( TString& inAlter ) const;

	///
	/// �crit la cha�ne sur un flux (8 bits).
	/// En POSIX, d�termine l'encodage par les variables globales.
	///
	/// \param inStream	flux consid�r�.
	///
	void		PrintToStream( std::ostream& inStream ) const;

	///
	/// Convertit la cha�ne et la copie dans une m�moire tampon.
	/// Cette m�thode peut convertir \c this en UTF-16.
	///
	/// \param outBuffer 	m�moire tampon
	/// \param ioLength		en entr�e, nombre d'octets disponibles,
	///						en sortie, nombre d'octets �crits.
	/// \param inEncoding	encodage cible.
	/// \return le r�sultat de la conversion.
	///
	UUTF16Conv::EResult	Convert(
							void* outBuffer,
							size_t* ioLength,
							EEncoding inEncoding );

protected:
	///
	/// Constructeur par d�faut. La cha�ne est en UTF-16.
	///
	TString( void );

private:
	///
	/// Comparaison. \c this et \c inAlter doivent �tre en ISO-8859-1 ou en US ASCII
	///
	/// \param inAlter		cha�ne (encod�e en ISO-8859-1 ou US ASCII) �
	///						comparer avec \c this.
	/// \return \c 0 si les deux cha�nes sont �gales, \c -1 si *this < inAlter,
	///			\c 1 si *this > inAlter sinon.
	///
	int			CompareLatin1Strings( const TString& inAlter ) const;

	///
	/// Comparaison. \c this et \c inAlter doivent �tre en UTF-16.
	///
	/// \param inAlter		cha�ne (encod�e en UTF-16) � comparer avec \c this.
	/// \return \c true si les deux cha�nes sont �gales, \c false sinon.
	///
	int			CompareUTF16Strings( const TString& inAlter ) const;
	
	///
	/// Comparaison des caract�res octet par octet. \c this et \c inAlter doivent
	/// �tre dans le m�me encodage. La valeur de hachage n'est pas mise � jour.
	///
	/// \param inAlter		cha�ne (encod�e comme \c this) �
	///						comparer avec \c this.
	/// \return \c true si les deux cha�nes sont �gales, \c false sinon.
	///
	int			Compare8BitsStrings( const TString& inAlter ) const;

	///
	/// Calcule la taille et la valeur de hachage.
	///

	/// \name Constantes priv�es
	enum {
		kHashMagic = 0x9E3779B9	///< Et oui, le hash, c'est magique.
	};

	/// \name Variables
	EEncoding			mEncoding;			///< Encodage de la cha�ne.
	void*				mString;			///< M�moire tampon.
	mutable KUInt32		mHashCode;			///< Valeur de hachage.
	mutable KUInt32		mStringSize;		///< Taille de la cha�ne (en cache, en caract�res).
	KUInt32				mBufferSize;		///< Taille de la m�moire tampon (en octets).
	mutable Boolean		mHashCodeIsValid;	///< Si la valeur de hachage a �t� calcul�e.
	mutable Boolean		mStringSizeIsValid;	///< Si la taille de la cha�ne a �t� calcul�e.
};

// -------------------------------------------------------------------------- //
//  * operator << ( std::ostream&, const TString& )
// -------------------------------------------------------------------------- //
///
/// Op�rateur pour �crire sur un flux.
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

