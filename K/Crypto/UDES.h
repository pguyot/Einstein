// ==============================
// Fichier:			UDES.h
// Projet:			K
// 
// Cr�� le:			26/8/2001
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
/// Cette classe suit les sp�cifications FIPS PUB 46 & FIPS PUB 81.
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
	/// \param inKey		cl� pour le cryptage.
	/// \param ioData		donn�es (en entr�e, donn�es � crypter, en sortie,
	///						donn�es crypt�es)
	///
	static inline void EncodeBlock( const KUInt64& inKey, KUInt64* ioData )
		{
			SSubKey theSubKeys[16];
			KeySchedule( inKey, theSubKeys );
			EncodeBlock( theSubKeys, ioData );
		}
	
	///
	/// D�crypte un bloc de 64 bits.
	///
	/// \param inKey		cl� pour le d�cryptage.
	/// \param ioData		donn�es (en entr�e, donn�es � d�crypter, en sortie,
	///						donn�es d�crypt�es)
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
	/// \param inKey		cl� pour le cryptage.
	/// \param ioData		donn�es (en entr�e, donn�es � crypter, en sortie,
	///						donn�es crypt�es)
	///
	static inline void NewtonEncodeBlock( const KUInt64& inKey, KUInt64* ioData )
		{
			SSubKey theSubKeys[16];
			NewtonKeySchedule( inKey, theSubKeys );
			EncodeBlock( theSubKeys, ioData );
		}
	
	///
	/// D�crypte un bloc de 64 bits.
	///
	/// \param inKey		cl� pour le d�cryptage.
	/// \param ioData		donn�es (en entr�e, donn�es � d�crypter, en sortie,
	///						donn�es d�crypt�es)
	///
	static void	NewtonDecodeBlock( const KUInt64& inKey, KUInt64* ioData )
		{
			SSubKey theSubKeys[16];
			NewtonKeySchedule( inKey, theSubKeys );
			DecodeBlock( theSubKeys, ioData );
		}

	///
	/// Cr�e une cl� � partir d'une cha�ne unicode termin�e par un caract�re
	/// nul. Utilise la cl� de cr�ation par d�faut.
	///
	/// \param inString			cha�ne unicode consid�r�e.
	/// \param outKey			en sortie, cl� correspondant � la cha�ne.
	///
	static inline void CreateKey(
					const KUInt16* inString,
					KUInt64* outKey )
		{
			CreateKey( inString, outKey, kDefaultCreationKey, false );
		}

	///
	/// Cr�e une cl� � partir d'une cha�ne unicode termin�e par un caract�re
	/// nul.
	///
	/// \param inString			cha�ne unicode consid�r�e.
	/// \param outKey			en sortie, cl� correspondant � la cha�ne.
	/// \param inCreationKey	cl� pour la cr�ation (doit avoir une parit�
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
	/// Cr�e une cl� � partir d'une cha�ne unicode termin�e par un caract�re
	/// nul. Ceci est une version compatible avec le pseudo DES du newton.
	/// (et donc avec la m�thode NewtonScript DESCreatePasswordKey).
	///
	/// \param inString	cha�ne unicode consid�r�e.
	/// \param outKey	en sortie, cl� correspondant � la cha�ne.
	///
	static inline void CreateNewtonKey( const KUInt16* inString, KUInt64* outKey )
		{
			CreateKey(
				inString, outKey, kNewtonCreationKey, true );
		}

	///
	/// Crypte avec le mode ECB.
	/// Si les donn�es ne sont pas dans un multiple de 64 bits, alors le reste
	/// est compl�t� de mani�re al�atoire. La m�moire tampon de sortie doit
	/// avoir comme taille celle des donn�es en entr�e arrondie aux 64 bits
	/// sup�rieurs.
	///
	/// \param inKey		cl� pour le cryptage.
	/// \param inData		donn�es � crypter
	/// \param inSize		taille des donn�es � crypter
	/// \param outBuffer	m�moire tampon pour la sortie.
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
	/// Structure pour les sous-cl�s de 48 bits.
	///
	struct SSubKey
	{
		KUInt32	fHigh;	/// 24 bits de poids fort.
		KUInt32	fLow;	/// 24 bits de poids faible.
	};

	///
	/// Calcule les sous-cl�s � partir d'une cl� donn�e.
	///
	/// \param inKey		cl�
	/// \param outSubKeys	en sortie, sous-cl�s
	///
	static void	KeySchedule( const KUInt64& inKey, SSubKey outSubKeys[16] );
	
	///
	/// La m�me chose, mais avec un d�calage des cl�s au d�but. Ceci
	/// est con�u pour �tre compatible avec le pseudo-DES dans NewtonOS.
	///
	/// \param inKey		cl�
	/// \param outSubKeys	en sortie, sous-cl�s
	///
	static void	NewtonKeySchedule( const KUInt64& inKey, SSubKey outSubKeys[16] );
	
	///
	/// Crypte un bloc de 64 bits.
	///
	/// \param inSubKeys	sous-cl�s pour le cryptage.
	/// \param ioData		donn�es (en entr�e, donn�es � crypter, en sortie,
	///						donn�es crypt�es)
	///
	static void EncodeBlock( const SSubKey inSubKeys[16], KUInt64* ioData );
	
	///
	/// D�crypte un bloc de 64 bits.
	///
	/// \param inSubKeys	sous-cl�s pour le d�cryptage.
	/// \param ioData		donn�es (en entr�e, donn�es � d�crypter, en sortie,
	///						donn�es d�crypt�es)
	///
	static void	DecodeBlock( const SSubKey inSubKeys[16], KUInt64* ioData );

	///
	/// Crypte avec le mode ECB.
	/// Si les donn�es ne sont pas dans un multiple de 64 bits, alors le reste
	/// est compl�t� de mani�re al�atoire. La m�moire tampon de sortie doit
	/// avoir comme taille celle des donn�es en entr�e arrondie aux 64 bits
	/// sup�rieurs.
	///
	/// \param inSubKeys	sous-cl�s pour le cryptage.
	/// \param inData		donn�es � crypter
	/// \param inSize		taille des donn�es � crypter
	/// \param outBuffer	m�moire tampon pour la sortie.
	///
	static void EncodeECB(
							const SSubKey inSubKeys[16],
							const KUInt8* inData,
							size_t inSize,
							KUInt8* outBuffer );

private:
	///
	/// Constructeur par d�faut volontairement priv�.
	/// Cette classe est statique.
	///
	UDES( void );

	///
	/// Cr�e une cl� � partir d'une cha�ne unicode termin�e par un caract�re
	/// nul. La table pour la parit� est un param�tre.
	///
	/// \param inString	cha�ne unicode consid�r�e.
	/// \param outKey	en sortie, cl� correspondant � la cha�ne.
	/// \param inCreationKey	cl� pour la cr�ation (doit avoir une parit�
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
	/// Cl� par d�faut pour la cr�ation d'une cl� � partir d'une cha�ne.
	///
	static const KUInt64 kDefaultCreationKey;
	
	///
	/// Cl� compatible Newton pour la cr�ation d'une cl� � partir d'une cha�ne.
	///
	static const KUInt64 kNewtonCreationKey;
	
	///
	/// Table pour calculer la parit� des octets.
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
