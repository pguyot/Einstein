// ==============================
// Fichier:			UDES.h
// Projet:			K
// 
// Créé le:			26/8/2001
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
// The Original Code is UDES.h.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2001-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: UDES.h,v 1.3 2004/11/23 15:11:22 paul Exp $
// ===========

#ifndef _UDES_H
#define _UDES_H

#include <K/Defines/KDefinitions.h>

// ANSI C
#include <stddef.h>

///
/// Classe pour le cryptage DES.
///
/// Cette classe suit les spécifications FIPS PUB 46 & FIPS PUB 81.
/// Cf:	http://www.itl.nist.gov/fipspubs/fip46-2.htm
/// and	http://www.itl.nist.gov/fipspubs/fip81.htm
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	UDESTest
///
class UDES
{
public:
	///
	/// Crypte un bloc de 64 bits.
	///
	/// \param inKey		clé pour le cryptage.
	/// \param ioData		données (en entrée, données à crypter, en sortie,
	///						données cryptées)
	///
	static inline void EncodeBlock( const KUInt64& inKey, KUInt64* ioData )
		{
			SSubKey theSubKeys[16];
			KeySchedule( inKey, theSubKeys );
			EncodeBlock( theSubKeys, ioData );
		}
	
	///
	/// Décrypte un bloc de 64 bits.
	///
	/// \param inKey		clé pour le décryptage.
	/// \param ioData		données (en entrée, données à décrypter, en sortie,
	///						données décryptées)
	///
	static inline void	DecodeBlock( const KUInt64& inKey, KUInt64* ioData )
		{
			SSubKey theSubKeys[16];
			KeySchedule( inKey, theSubKeys );
			DecodeBlock( theSubKeys, ioData );
		}

	///
	/// Crypte un bloc de 64 bits, version compatible Newton.
	///
	/// \param inKey		clé pour le cryptage.
	/// \param ioData		données (en entrée, données à crypter, en sortie,
	///						données cryptées)
	///
	static inline void NewtonEncodeBlock( const KUInt64& inKey, KUInt64* ioData )
		{
			SSubKey theSubKeys[16];
			NewtonKeySchedule( inKey, theSubKeys );
			EncodeBlock( theSubKeys, ioData );
		}
	
	///
	/// Décrypte un bloc de 64 bits.
	///
	/// \param inKey		clé pour le décryptage.
	/// \param ioData		données (en entrée, données à décrypter, en sortie,
	///						données décryptées)
	///
	static void	NewtonDecodeBlock( const KUInt64& inKey, KUInt64* ioData )
		{
			SSubKey theSubKeys[16];
			NewtonKeySchedule( inKey, theSubKeys );
			DecodeBlock( theSubKeys, ioData );
		}

	///
	/// Crée une clé à partir d'une chaîne unicode terminée par un caractère
	/// nul. Utilise la clé de création par défaut.
	///
	/// \param inString			chaîne unicode considérée.
	/// \param outKey			en sortie, clé correspondant à la chaîne.
	///
	static inline void CreateKey(
					const KUInt16* inString,
					KUInt64* outKey )
		{
			CreateKey( inString, outKey, kDefaultCreationKey, false );
		}

	///
	/// Crée une clé à partir d'une chaîne unicode terminée par un caractère
	/// nul.
	///
	/// \param inString			chaîne unicode considérée.
	/// \param outKey			en sortie, clé correspondant à la chaîne.
	/// \param inCreationKey	clé pour la création (doit avoir une parité
	///							correcte)
	///
	static inline void CreateKey(
					const KUInt16* inString,
					KUInt64* outKey,
					const KUInt64& inCreationKey )
		{
			CreateKey( inString, outKey, inCreationKey, false );
		}

	///
	/// Crée une clé à partir d'une chaîne unicode terminée par un caractère
	/// nul. Ceci est une version compatible avec le pseudo DES du newton.
	/// (et donc avec la méthode NewtonScript DESCreatePasswordKey).
	///
	/// \param inString	chaîne unicode considérée.
	/// \param outKey	en sortie, clé correspondant à la chaîne.
	///
	static inline void CreateNewtonKey( const KUInt16* inString, KUInt64* outKey )
		{
			CreateKey(
				inString, outKey, kNewtonCreationKey, true );
		}

	///
	/// Crypte avec le mode ECB.
	/// Si les données ne sont pas dans un multiple de 64 bits, alors le reste
	/// est complété de manière aléatoire. La mémoire tampon de sortie doit
	/// avoir comme taille celle des données en entrée arrondie aux 64 bits
	/// supérieurs.
	///
	/// \param inKey		clé pour le cryptage.
	/// \param inData		données à crypter
	/// \param inSize		taille des données à crypter
	/// \param outBuffer	mémoire tampon pour la sortie.
	///
	static inline void EncodeECB(
							const KUInt64& inKey,
							const KUInt8* inData,
							size_t inSize,
							KUInt8* outBuffer )
		{
			SSubKey theSubKeys[16];
			KeySchedule( inKey, theSubKeys );
			EncodeECB( theSubKeys, inData, inSize, outBuffer );
		}

protected:
	friend class UDESTest;

	///
	/// Structure pour les sous-clés de 48 bits.
	///
	struct SSubKey
	{
		KUInt32	fHigh;	/// 24 bits de poids fort.
		KUInt32	fLow;	/// 24 bits de poids faible.
	};

	///
	/// Calcule les sous-clés à partir d'une clé donnée.
	///
	/// \param inKey		clé
	/// \param outSubKeys	en sortie, sous-clés
	///
	static void	KeySchedule( const KUInt64& inKey, SSubKey outSubKeys[16] );
	
	///
	/// La même chose, mais avec un décalage des clés au début. Ceci
	/// est conçu pour être compatible avec le pseudo-DES dans NewtonOS.
	///
	/// \param inKey		clé
	/// \param outSubKeys	en sortie, sous-clés
	///
	static void	NewtonKeySchedule( const KUInt64& inKey, SSubKey outSubKeys[16] );
	
	///
	/// Crypte un bloc de 64 bits.
	///
	/// \param inSubKeys	sous-clés pour le cryptage.
	/// \param ioData		données (en entrée, données à crypter, en sortie,
	///						données cryptées)
	///
	static void EncodeBlock( const SSubKey inSubKeys[16], KUInt64* ioData );
	
	///
	/// Décrypte un bloc de 64 bits.
	///
	/// \param inSubKeys	sous-clés pour le décryptage.
	/// \param ioData		données (en entrée, données à décrypter, en sortie,
	///						données décryptées)
	///
	static void	DecodeBlock( const SSubKey inSubKeys[16], KUInt64* ioData );

	///
	/// Crypte avec le mode ECB.
	/// Si les données ne sont pas dans un multiple de 64 bits, alors le reste
	/// est complété de manière aléatoire. La mémoire tampon de sortie doit
	/// avoir comme taille celle des données en entrée arrondie aux 64 bits
	/// supérieurs.
	///
	/// \param inSubKeys	sous-clés pour le cryptage.
	/// \param inData		données à crypter
	/// \param inSize		taille des données à crypter
	/// \param outBuffer	mémoire tampon pour la sortie.
	///
	static void EncodeECB(
							const SSubKey inSubKeys[16],
							const KUInt8* inData,
							size_t inSize,
							KUInt8* outBuffer );

private:
	///
	/// Constructeur par défaut volontairement privé.
	/// Cette classe est statique.
	///
	UDES( void );

	///
	/// Crée une clé à partir d'une chaîne unicode terminée par un caractère
	/// nul. La table pour la parité est un paramètre.
	///
	/// \param inString	chaîne unicode considérée.
	/// \param outKey	en sortie, clé correspondant à la chaîne.
	/// \param inCreationKey	clé pour la création (doit avoir une parité
	///							correcte)
	/// \param inNewtonCompat	si on veut faire du compatible Newton
	///							(vs standard DES)
	///
	static void CreateKey(
					const KUInt16* inString,
					KUInt64* outKey,
					const KUInt64& inCreationKey,
					Boolean inNewtonCompat );

	/// \name Les tables

	///
	/// -- Permuted Choice 1 --
	/// Quel mot doit-on permuter?
	///
	static const KUInt8 kPC1Which[56];
	
	///
	/// Masque pour un mot de 32 bits.
	///
	static const KUInt32 kPC1Mask[56];

	///
	/// -- Permuted Choice 2 --
	/// Masque pour un mot de 28 bits.
	/// Les premiers 24 bits s'appliquent au mot de poids fort, les 24
	/// suivants au mot de poids faible.
	///
	static const KUInt32 kPC2Mask[48];
	
	///
	/// -- Initial Permutation --
	/// Masque pour un mot de 32 bits.
	/// Les 4 premiers concernent le mot de poids faible, les 4 suivants
	/// celui de poids fort.
	///
	static const KUInt32 kIPMask[64];

	///
	/// -- Inverse Initial Permutation --
	/// Masque pour un mot de 32 bits.
	/// Un sur deux concerne le mot de poids faible.
	///
	static const KUInt32 kIPInvMask[64];
	
	///
	/// -- E Bit Selection --
	/// Masque pour un mot de 32 bits.
	///
	static const KUInt32 kESelMask[48];
	
	///
	/// -- S Boxes --
	///
	static const KUInt8 kSBoxes[8][4][16];
	
	///
	/// -- Permutation Table -- 
	/// Permute 32 bits dans 32 bits.
	/// Masque pour un mot de 32 bits.
	///
	static const KUInt32 kPermMask[32];

	///
	/// Clé par défaut pour la création d'une clé à partir d'une chaîne.
	///
	static const KUInt64 kDefaultCreationKey;
	
	///
	/// Clé compatible Newton pour la création d'une clé à partir d'une chaîne.
	///
	static const KUInt64 kNewtonCreationKey;
	
	///
	/// Table pour calculer la parité des octets.
	///
	static const KUInt8 kOddBitNumber[256];
};


#endif
		// _UDES_H

// ========================================================================= //
// As part of an ongoing effort to keep you, the Fortune reader, abreast of  //
// the valuable information the daily crosses the USENET, Fortune presents:  //
//                                                                           //
// News articles that answer *your* questions, #1:                           //
//                                                                           //
//       Newsgroups: comp.sources.d                                          //
//       Subject: how do I run C code received from sources                  //
//       Keywords: C sources                                                 //
//       Distribution: na                                                    //
//                                                                           //
//       I do not know how to run the C programs that are posted in the      //
//       sources newsgroup.  I save the files, edit them to remove the       //
//       headers, and change the mode so that they are executable, but I     //
//       cannot get them to run.  (I have never written a C program before.) //
//                                                                           //
//       Must they be compiled?  With what compiler?  How do I do this?  If  //
//       I compile them, is an object code file generated or must I generate //
//       it explicitly with the > character?  Is there something else that   //
//       must be done?                                                       //
// ========================================================================= //
