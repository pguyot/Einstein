// ==============================
// Fichier:			UDES.cp
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
// The Original Code is UDES.cp.
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
// $Id: UDES.cp,v 1.4 2004/11/23 15:11:22 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Crypto/UDES.h>

// K Libs
#include <K/Defines/UByteSex.h>
#include <K/Math/UTInt64.h>
#include <K/Tests/KDebug.h>

// ANSI C
#include <stdlib.h>


// -------------------------------------------------------------------------- //
//  * Macros de déverminage
// -------------------------------------------------------------------------- //

#undef KERROR_ENABLED
#if defined(error_KCRYPTO_UDES) && error_KCRYPTO_UDES
#define KERROR_ENABLED 1
#else
#define KERROR_ENABLED 0
#endif

#undef KTRACE_ENABLED
#if defined(trace_KCRYPTO_UDES) && trace_KCRYPTO_UDES
#define KTRACE_ENABLED 1
#else
#define KTRACE_ENABLED 0
#endif

#undef KDEBUG_ENABLED
#if defined(debug_KCRYPTO_UDES) && debug_KCRYPTO_UDES
#define KDEBUG_ENABLED 1
#else
#define KDEBUG_ENABLED 0
#endif

// -------------------------------------------------------------------------- //
//  * KeySchedule( const KUInt64&, SSubKey[16] )
// -------------------------------------------------------------------------- //
void
UDES::KeySchedule( const KUInt64& inKey, SSubKey outSubKeys[16] )
{
	// Transforme une clef (dans inKey) en 16 sous-clefs (dans outSubKeys).
	// La parité de la clef n'est pas vérifiée.
	// L'opération se fait en deux étapes.
	// 1. Permuted Choice 1: génère C0 & D0
	// 2. Boucle sur les sous-clefs
	// 2.1. Génération de Ci & Di à partir de Ci-1 & Di-1
	// 2.2. Génération de la sous-clef i à partir de Ci & Di avec
	//			la table Permuted Choice 2.

	// Permuted choice 1.
	// Input: my 64 bits key.
	// Output: two halves of 28 bits (C0 & D0)
	
	// Représentation de la clef
	KUInt32 inKeyHigh = UTInt64::GetHi( inKey );
	KUInt32 inKeyLow = UTInt64::GetLo( inKey );

	KUInt32 Ci = 0;	// Tous les Ci dans l'algorithme.
					// En pratique, on n'a besoin que d'une seule variable.
					// C0 = 0
	
	int index_i;	// Curseur pour compter jusqu'à 56, utilisé ensuite dans d'autres boucles.
	
	// Première passe les Ci
	for (index_i = 0; index_i < 28; index_i++)
	{
		Ci <<= 1;	// No effect for first run because C0 is 0.

		if (kPC1Which[index_i])
		{
			if (inKeyHigh & kPC1Mask[ index_i ])
				Ci |= 0x01;
		} else {
			if (inKeyLow & kPC1Mask[ index_i ])
				Ci |= 0x01;
		}
	}

	KUInt32 Di = 0;	// Tous les Di dans l'algorithme.
					// Tout comme les Ci, une seule variable suffit.
					// D0 = 0
					// Autre remarque: les Di considérés sont D[index_i - 28]
	
	for ( /* index_i = 28 */; index_i < 56; index_i++)
	{
		Di <<= 1;	// No effect for first run because D0 is 0.

		if (kPC1Which[index_i])
		{
			if (inKeyHigh & kPC1Mask[ index_i ])
				Di |= 0x01;
		} else {
			if (inKeyLow & kPC1Mask[ index_i ])
				Di |= 0x01;
		}
	}

	int index_j;	// De 0 à 15: boucle sur les sous-clefs.
	
	// J'utilise un mot de 16 bits pour savoir combien de rotations il faut faire.
	// 1 2 3 4  5 6 7 8  9 A B C  D E F G
	// 1 1 2 2  2 2 2 2  1 2 2 2  2 2 2 1
	// dans l'autre sens:
    // G F E D  C B A 9  8 7 6 5  4 3 2 1
	// 1 2 2 2  2 2 2 1  2 2 2 2  2 2 1 1
	// je retranche 1:
	// 0 1 1 1  1 1 1 0  1 1 1 1  1 1 0 0
	KUInt16 numberRotations = 0x7EFC;
	
	for (index_j = 0; index_j < 16; index_j++)
	{
		if (numberRotations & 0x1)
		{
			// Deux rotations.
			Ci = ((Ci << 2) & 0x0FFFFFFF) | (Ci >> 26);
			Di = ((Di << 2) & 0x0FFFFFFF) | (Di >> 26);
		} else {
			// Une rotation.
			Ci = ((Ci << 1) & 0x0FFFFFFF) | (Ci >> 27);
			Di = ((Di << 1) & 0x0FFFFFFF) | (Di >> 27);
		}		
	
		// Sous-clef suivante.
		numberRotations >>= 1;
	
		KUInt32 theSubKeyWord = 0;	// Pour chacun des mots (poids fort & faible).
		
		// D'abord le mot de poids fort (i.e. les 24 premiers bits avec les Ci)
		// index_i sert d'index sur le masque.
		for ( index_i = 0; index_i < 24; index_i++)
		{
			theSubKeyWord <<= 1;	// No effect for first run because theSubKeyWord is 0.

			if (Ci & kPC2Mask[ index_i ])
			{
				theSubKeyWord |= 0x01;
			}
		}
		
		outSubKeys[index_j].fHigh = theSubKeyWord;	// On met le résultat dans outSubKeys.
		
		// Mot de poids faible (i.e. les 24 derniers bits avec les Di)
		
		theSubKeyWord = 0;
		for (/* index_i = 24 */; index_i < 48; index_i++)
		{
			theSubKeyWord <<= 1;	// No effect for first run because theSubKeyWord is 0.

			if (Di & kPC2Mask[ index_i ])
			{
				theSubKeyWord |= 0x01;
			}
		}
	
		outSubKeys[index_j].fLow = theSubKeyWord;
	} // for (index_j = 0; index_j < 16; index_j++)
}

// -------------------------------------------------------------------------- //
//  * NewtonKeySchedule( const KUInt64&, SSubKey[16] )
// -------------------------------------------------------------------------- //
void
UDES::NewtonKeySchedule( const KUInt64& inKey, SSubKey outSubKeys[16] )
{
	// Décalage de chaque moitié de la clé vers la gauche.
	KUInt32 theKeyHigh = UTInt64::GetHi( inKey );
	KUInt32 theKeyLow = UTInt64::GetLo( inKey );
	KUInt64 theDESKey;
	UTInt64::Set( &theDESKey, theKeyHigh << 1, theKeyLow << 1 );
	
	KeySchedule( theDESKey, outSubKeys );
}

// -------------------------------------------------------------------------- //
//  * EncodeBlock( const SSubKey[16], KUInt64* )
// -------------------------------------------------------------------------- //
void
UDES::EncodeBlock( const SSubKey inSubKeys[16], KUInt64* ioData )
{
	// Encode un bloc de 64 bits.
	// 1. Permutation initiale.
	// 2. Boucle sur les 16 clefs de 1 à 16
	// 3. Permutation initiale inverse.

	// Les données, séparées en gauche et droite.
	// (cf l'algorithme).
	KUInt32 ioDataLeft = UTInt64::GetHi( *ioData );
	KUInt32 ioDataRight = UTInt64::GetLo( *ioData );

	// 1. Permutation initiale.
	KUInt32 permutedDataLeft = 0;	// Les données permutées, à gauche.

	int index_i;	// Index sur les bits des données, utilisé dans d'autres boucles.
	for (index_i = 0; index_i < 32; index_i++)
	{
		permutedDataLeft <<= 1;	// No effect for first run because permutedDataLeft is 0.

		if (index_i & 0x4)
		{
			if (ioDataLeft & kIPMask[ index_i ])
				permutedDataLeft |= 0x01;
		} else {
			if (ioDataRight & kIPMask[ index_i ])
				permutedDataLeft |= 0x01;
		}
	}

	KUInt32 permutedDataRight = 0;	// Les données de droite permutées.
	for ( /* index_i = 32 */; index_i < 64; index_i++)
	{
		permutedDataRight <<= 1;	// No effect for first run because permutedDataRight is 0.

		if (index_i & 0x4)
		{
			if (ioDataLeft & kIPMask[ index_i ])
				permutedDataRight |= 0x01;
		} else {
			if (ioDataRight & kIPMask[ index_i ])
				permutedDataRight |= 0x01;
		}
	}

	// 2. Boucle sur les clefs.
	
	for (index_i = 0; index_i < 16; index_i++)
	{
		int index_j;	// Index sur les 48 bits de kESelMask.
		KUInt32 ExpandedHigh = 0;	// Mot de poids fort de E
		for (index_j = 0; index_j < 24; index_j++)
		{
			ExpandedHigh <<= 1;	// No effect for first run because ExpandedHigh is 0.

			if (permutedDataRight & kESelMask[ index_j ])
				ExpandedHigh |= 0x01;
		}
		ExpandedHigh ^= inSubKeys[index_i].fHigh;

		KUInt32 ExpandedLow = 0;	// Mot de poids faible de E
		for ( /* index_j = 24 */; index_j < 48; index_j++)
		{
			ExpandedLow <<= 1;	// No effect for first run because ExpandedLow is 0.

			if (permutedDataRight & kESelMask[ index_j ])
				ExpandedLow |= 0x01;
		}
		ExpandedLow ^= inSubKeys[index_i].fLow;
		
		// Division de Expanded en 8 blocks de 6 bits.

		KUInt32 theBBlocksMerged = 0;	// Les 8 BBlocks dans un mot de 32 bits (après CTS)
		
		{
			KUInt8 BBlocks[8];	// E découpés en 8 blocs de 6 bits.
			BBlocks[0] = (KUInt8) (ExpandedHigh >> 18);
			BBlocks[1] = (KUInt8) ((ExpandedHigh >> 12) & 0x3F);
			BBlocks[2] = (KUInt8) ((ExpandedHigh >> 6) & 0x3F);
			BBlocks[3] = (KUInt8) (ExpandedHigh & 0x3F);
			BBlocks[4] = (KUInt8) (ExpandedLow >> 18);
			BBlocks[5] = (KUInt8) ((ExpandedLow >> 12) & 0x3F);
			BBlocks[6] = (KUInt8) ((ExpandedLow >> 6) & 0x3F);
			BBlocks[7] = (KUInt8) (ExpandedLow & 0x3F);
		
			// Itération sur les 8 blocs de 6 bits.
			for (index_j = 0; index_j < 8; index_j++)
			{
				theBBlocksMerged <<= 4;	// No effect on first iteration, blah blah.
			
				int temp = BBlocks[index_j];	// Variable temporaire pour éviter de
												// déréférencer BBlocks[index_j] 5 fois
				int index_m = temp & 0x01;		// Index m dans les tables CTS
				if (temp & 0x20)	// 6th bit
				{
					index_m |= 0x02;
					temp &= 0x1F;	// Get rid of it (for temp >> 1 to be bits 2 to 5)
				}
				
				theBBlocksMerged |= kSBoxes[index_j][index_m][ temp >> 1 ];
			}
		}
		
		// Permute theBBlocksMerged with kPermMask.
		
		KUInt32 thePermutedBBlocksMerged = 0;
		
		for (index_j = 0; index_j < 32; index_j++)
		{
			thePermutedBBlocksMerged <<= 1;	// No effect.
			
			if (theBBlocksMerged & kPermMask[ index_j ])
			{
				thePermutedBBlocksMerged |= 1;
			}
		}
		
		// Use theBBlocksMerged as temporary memory to do the following:
		// R[i] = L[i-1] XOR thePermutedBBlocksMerged
		// L[i] = R[i-1]
		
		theBBlocksMerged = permutedDataRight;
		permutedDataRight = permutedDataLeft ^ thePermutedBBlocksMerged;
		permutedDataLeft = theBBlocksMerged;
		
	} // for (index_i = 0; index_i < 16; index_i++)

	// 3. Permutation initiale inverse de RL
	
	ioDataLeft = 0;	// Initialisation de la sortie.

	for (index_i = 0; index_i < 32; index_i++)
	{
		ioDataLeft <<= 1;	// No effect for first run because ioDataLeft is 0.

		if (index_i & 0x1)
		{
			if (permutedDataRight & kIPInvMask[ index_i ])
				ioDataLeft |= 0x01;
		} else {
			if (permutedDataLeft & kIPInvMask[ index_i ])
				ioDataLeft |= 0x01;
		}
	}

	ioDataRight = 0;
	for ( /* index_i = 32 */; index_i < 64; index_i++)
	{
		ioDataRight <<= 1;	// No effect for first run because ioDataRight is 0.

		if (index_i & 0x1)
		{
			if (permutedDataRight & kIPInvMask[ index_i ])
				ioDataRight |= 0x01;
		} else {
			if (permutedDataLeft & kIPInvMask[ index_i ])
				ioDataRight |= 0x01;
		}
	}
	
	// Résultat dans le tableau.
	UTInt64::Set( ioData, ioDataLeft, ioDataRight );
}

// -------------------------------------------------------------------------- //
//  * DecodeBlock( const SSubKey[16], KUInt64* )
// -------------------------------------------------------------------------- //
void
UDES::DecodeBlock( const SSubKey inSubKeys[16], KUInt64* ioData )
{
	// Décode un bloc de 64 bits.
	// 1. Permutation initiale.
	// 2. Boucle sur les 16 clefs de 16 à 1
	// 3. Permutation initiale inverse.

	// Les données, séparées en gauche et droite.
	// (cf l'algorithme).
	KUInt32 ioDataLeft = UTInt64::GetHi( *ioData );
	KUInt32 ioDataRight = UTInt64::GetLo( *ioData );

	// 1. Permutation initiale de LR
	KUInt32 permutedDataLeft = 0;	// Les données permutées, mot de gauche

	int index_i;	// Curseur pour compter jusqu'à 64, puis dans d'autres boucles.
	for (index_i = 0; index_i < 32; index_i++)
	{
		permutedDataLeft <<= 1;	// No effect for first run because permutedDataLeft is 0.

		if (index_i & 0x4)
		{
			if (ioDataLeft & kIPMask[ index_i ])
				permutedDataLeft |= 0x01;
		} else {
			if (ioDataRight & kIPMask[ index_i ])
				permutedDataLeft |= 0x01;
		}
	}

	KUInt32 permutedDataRight = 0;	// Les données permutées, mot de droite
	for ( /* index_i = 32 */; index_i < 64; index_i++)
	{
		permutedDataRight <<= 1;	// No effect for first run because permutedDataLeft is 0.

		if (index_i & 0x4)
		{
			if (ioDataLeft & kIPMask[ index_i ])
				permutedDataRight |= 0x01;
		} else {
			if (ioDataRight & kIPMask[ index_i ])
				permutedDataRight |= 0x01;
		}
	}

	// 2. Boucle sur les clefs.
	
	for (index_i = 15; index_i >= 0; index_i--)
	{
		KUInt32 ExpandedHigh = 0;	// E, 24 bits de poids fort
		int index_j;
		for (index_j = 0; index_j < 24; index_j++)
		{
			ExpandedHigh <<= 1;	// No effect for first run because ExpandedHigh is 0.

			if (permutedDataRight & kESelMask[ index_j ])
				ExpandedHigh |= 0x01;
		}
		ExpandedHigh ^= inSubKeys[index_i].fHigh;

		KUInt32 ExpandedLow = 0;		// E, 24 bits de poids faible
		for ( /* index_j = 24 */; index_j < 48; index_j++)
		{
			ExpandedLow <<= 1;	// No effect for first run because ExpandedLow is 0.

			if (permutedDataRight & kESelMask[ index_j ])
				ExpandedLow |= 0x01;
		}
		ExpandedLow ^= inSubKeys[index_i].fLow;
		
		// Division de Expanded en 8 blocs de 6 bits.

		KUInt32 theBBlocksMerged = 0;	// Les BB après CTS
		
		{
			KUInt8 BBlocks[8];			// E en 8 blocs de 6 bits
			BBlocks[0] = (KUInt8) (ExpandedHigh >> 18);
			BBlocks[1] = (KUInt8) ((ExpandedHigh >> 12) & 0x3F);
			BBlocks[2] = (KUInt8) ((ExpandedHigh >> 6) & 0x3F);
			BBlocks[3] = (KUInt8) (ExpandedHigh & 0x3F);
			BBlocks[4] = (KUInt8) (ExpandedLow >> 18);
			BBlocks[5] = (KUInt8) ((ExpandedLow >> 12) & 0x3F);
			BBlocks[6] = (KUInt8) ((ExpandedLow >> 6) & 0x3F);
			BBlocks[7] = (KUInt8) (ExpandedLow & 0x3F);
		
			for (index_j = 0; index_j < 8; index_j++)
			{
				theBBlocksMerged <<= 4;	// No effect on first iteration, blah blah.
			
				int temp = BBlocks[index_j];	// Variable temporaire pour éviter de
												// déréférencer BBlocks[index_j] 5 fois
				int index_m = temp & 0x01;		// Index m dans les tables CTS
				if (temp & 0x20)	// 6th bit
				{
					index_m |= 0x02;
					temp &= 0x1F;	// Get rid of it (for temp >> 1 to be bits 2 to 5)
				}
				
				theBBlocksMerged |= kSBoxes[index_j][index_m][ temp >> 1 ];
			}
		}
		
		// Permute theBBlocksMerged with kPermMask.
		
		KUInt32 thePermutedBBlocksMerged = 0;	// BB permutés.
		
		for (index_j = 0; index_j < 32; index_j++)
		{
			thePermutedBBlocksMerged <<= 1;	// No effect.
			
			if (theBBlocksMerged & kPermMask[ index_j ])
			{
				thePermutedBBlocksMerged |= 1;
			}
		}
		
		// Use theBBlocksMerged as temporary memory to do the following:
		// R[i] = L[i-1] XOR thePermutedBBlocksMerged
		// L[i] = R[i-1]
		
		theBBlocksMerged = permutedDataRight;
		permutedDataRight = permutedDataLeft ^ thePermutedBBlocksMerged;
		permutedDataLeft = theBBlocksMerged;
		
	} // for (index_i = 15; index_i >= 0; index_i--)

	// 3. Permutation initiale inverse de RL
	
	ioDataLeft = 0;	// Réinitialisation pour le résultat.

	for (index_i = 0; index_i < 32; index_i++)
	{
		ioDataLeft <<= 1;	// No effect for first run because ioDataLeft is 0.

		if (index_i & 0x1)
		{
			if (permutedDataRight & kIPInvMask[ index_i ])
				ioDataLeft |= 0x01;
		} else {
			if (permutedDataLeft & kIPInvMask[ index_i ])
				ioDataLeft |= 0x01;
		}
	}

	ioDataRight = 0;	// Idem.
	for ( /* index_i = 32 */; index_i < 64; index_i++)
	{
		ioDataRight <<= 1;	// No effect for first run because ioDataRight is 0.

		if (index_i & 0x1)
		{
			if (permutedDataRight & kIPInvMask[ index_i ])
				ioDataRight |= 0x01;
		} else {
			if (permutedDataLeft & kIPInvMask[ index_i ])
				ioDataRight |= 0x01;
		}
	}
	
	// On met le résultat dans le tableau.
	UTInt64::Set( ioData, ioDataLeft, ioDataRight );
}

// -------------------------------------------------------------------------- //
//  * EncodeECB( const SSubKey[16], const KUInt8*, size_t, KUInt8* )
// -------------------------------------------------------------------------- //
void
UDES::EncodeECB(
			const SSubKey inSubKeys[16],
			const KUInt8* inData,
			size_t inSize,
			KUInt8* outData )
{
	// Cryptage dans le mode ECB (Electronic Code Book).
	// C'est tout simplement les mots les uns à la suite des autres.
	KUInt64 theWord = 0;
	size_t theIndex = 0;
	while (theIndex < inSize)
	{
		(void) (theWord <<= 8);
		(void) (theWord |= inData[theIndex]);
		
		theIndex++;

		if ((theIndex % 4) == 0)
		{
			// Un mot de 64 bits est terminé.
			EncodeBlock( inSubKeys, &theWord );
			
			// On stocke les 8 octets.
			KUInt32 theFirst4Bytes = UTInt64::GetHi( theWord );
			KUInt32 theLast4Bytes = UTInt64::GetLo( theWord );
			*outData++ = (KUInt8) (theFirst4Bytes >> 24);
			*outData++ = (KUInt8) (theFirst4Bytes >> 16);
			*outData++ = (KUInt8) (theFirst4Bytes >> 8);
			*outData++ = (KUInt8) (theFirst4Bytes);
			*outData++ = (KUInt8) (theLast4Bytes >> 24);
			*outData++ = (KUInt8) (theLast4Bytes >> 16);
			*outData++ = (KUInt8) (theLast4Bytes >> 8);
			*outData++ = (KUInt8) (theLast4Bytes);
			
			theWord = 0;
		}
	}
	
	if ((theIndex % 4) != 0)
	{
		// Il faut remplir ce qui manque avec des valeurs aléatoires.
		// theIndex % 4 == 1 -> il manque 3 octets
		// theIndex % 4 == 2 -> il manque 2 octets
		// theIndex % 4 == 3 -> il manque 1 octets

		switch (theIndex % 4)
		{
			case 1:
				(void) (theWord |= (::rand() % 0xFF) << 16);
			case 2:
				(void) (theWord |= (::rand() % 0xFF) << 8);
			case 3:
				(void) (theWord |= (::rand() % 0xFF));
		}

		// On code.
		EncodeBlock( inSubKeys, &theWord );

		// On stocke les 8 octets.
		KUInt32 theFirst4Bytes = UTInt64::GetHi( theWord );
		KUInt32 theLast4Bytes = UTInt64::GetLo( theWord );
		*outData++ = (KUInt8) (theFirst4Bytes >> 24);
		*outData++ = (KUInt8) (theFirst4Bytes >> 16);
		*outData++ = (KUInt8) (theFirst4Bytes >> 8);
		*outData++ = (KUInt8) (theFirst4Bytes);
		*outData++ = (KUInt8) (theLast4Bytes >> 24);
		*outData++ = (KUInt8) (theLast4Bytes >> 16);
		*outData++ = (KUInt8) (theLast4Bytes >> 8);
		*outData++ = (KUInt8) (theLast4Bytes);
	}
}

// -------------------------------------------------------------------------- //
//  * CreateKey( const KUInt16*, KUInt64*, const KUInt64& )
// -------------------------------------------------------------------------- //
void
UDES::CreateKey(
			const KUInt16* inString,
			KUInt64* outKey,
			const KUInt64& inCreationKey,
			Boolean inNewtonCompat )
{
	// Crée une clef à partir d'une chaîne de caractère unicode.
	// La chaîne doit avoir un caractère nul comme terminateur.

	// J'ai besoin de deux clés.
	
	// Clef pour la création.
	// Doit normalement avoir une certaine parité.
	KUInt64 theKey = inCreationKey;
	
	// Si on a encore des caractères.
	bool charsLeft = true;
	
	// Indice sur la chaîne.
	int indexStr = 0;
	do
	{
		SSubKey theSubKey[16];					// Sous clefs dérivées de theKey.
		if (inNewtonCompat)
		{
			NewtonKeySchedule( theKey, theSubKey );
												// Calcul des sous clefs.
		} else {
			KeySchedule( theKey, theSubKey );	// Calcul des sous clefs.
		}

		// Lecture de 4 caractères de la chaîne dans la clef theKey2.
		
		KUInt16 theUniChar;	// Variable pour les caractères unicode.
		KUInt32 tmpKeyHi;
		KUInt32 tmpKeyLo;
		
		// Character #1
		theUniChar = UByteSex_FromBigEndian( inString[ indexStr++ ] );
		if (theUniChar)
		{
			KDEBUG1( "theUniChar = %.2X", (unsigned int) theUniChar );

			tmpKeyHi = (KUInt32) (theUniChar >> 8);
			tmpKeyHi <<= 8;
			tmpKeyHi |= (KUInt32) (theUniChar & 0xFF);
			tmpKeyHi <<= 8;
			
			KDEBUG1( "tmpKeyHi = %.8X", (unsigned int) tmpKeyHi );

			// Character #2
			theUniChar = UByteSex_FromBigEndian( inString[ indexStr++ ] );
			if (theUniChar)
			{
				KDEBUG1( "theUniChar = %.2X", (unsigned int) theUniChar );

				tmpKeyHi |= (KUInt32) (theUniChar >> 8);
				tmpKeyHi <<= 8;
				tmpKeyHi |= (KUInt32) (theUniChar & 0xFF);
				
				KDEBUG1( "tmpKeyHi = %.8X", (unsigned int) tmpKeyHi );

				// Character #3
				theUniChar = UByteSex_FromBigEndian( inString[ indexStr++ ] );
				if (theUniChar)
				{
					KDEBUG1( "theUniChar = %.2X", (unsigned int) theUniChar );

					tmpKeyLo = (KUInt32) (theUniChar >> 8);
					tmpKeyLo <<= 8;
					tmpKeyLo |= (KUInt32) (theUniChar & 0xFF);
					tmpKeyLo <<= 8;

					KDEBUG1( "tmpKeyLo = %.8X", (unsigned int) tmpKeyLo );

					// Character #4
					theUniChar = UByteSex_FromBigEndian( inString[ indexStr++ ] );
					if (theUniChar)
					{
						KDEBUG1( "theUniChar = %.2X", (unsigned int) theUniChar );

						tmpKeyLo |= (KUInt32) (theUniChar >> 8);
						tmpKeyLo <<= 8;
						tmpKeyLo |= (KUInt32) (theUniChar & 0xFF);

						KDEBUG1( "tmpKeyLo = %.8X", (unsigned int) tmpKeyLo );
					} else {
						charsLeft = false;
						tmpKeyLo <<= 8;
					}
				} else {
					charsLeft = false;
					tmpKeyLo = 0;
				}				
			} else {
				charsLeft = false;
				tmpKeyHi <<= 8;
				tmpKeyLo = 0;
			}
		} else {
			charsLeft = false;
			tmpKeyHi = 0;
			tmpKeyLo = 0;
		}
		
		// Cryptage de tmpKey avec la clé précédente.
		KUInt64 tmpKey;
		UTInt64::SetHi( &tmpKey, tmpKeyHi );
		UTInt64::SetLo( &tmpKey, tmpKeyLo );
		
		KDEBUG2( "tmpKey = %.8X%.8X",
					(unsigned int) tmpKeyHi,
					(unsigned int) tmpKeyLo );
		EncodeBlock( theSubKey, &tmpKey );

		tmpKeyHi = UTInt64::GetHi( tmpKey );
		tmpKeyLo = UTInt64::GetLo( tmpKey );
		
		KDEBUG2( ">tmpKey = %.8X%.8X",
					(unsigned int) tmpKeyHi,
					(unsigned int) tmpKeyLo );

		// On mets theKey2 dans theKey en vérifiant la parité.
		UTInt64::Set( &theKey, 
				(KUInt32) (kOddBitNumber[tmpKeyHi & 0xFF])
			|	(KUInt32) (kOddBitNumber[(tmpKeyHi >> 8) & 0xFF] << 8)
			|	(KUInt32) (kOddBitNumber[(tmpKeyHi >> 16) & 0xFF] << 16)
			|	(KUInt32) (kOddBitNumber[(tmpKeyHi >> 24) & 0xFF] << 24),
				(KUInt32) (kOddBitNumber[tmpKeyLo & 0xFF])
			|	(KUInt32) (kOddBitNumber[(tmpKeyLo >> 8) & 0xFF] << 8)
			|	(KUInt32) (kOddBitNumber[(tmpKeyLo >> 16) & 0xFF] << 16)
			|	(KUInt32) (kOddBitNumber[(tmpKeyLo >> 24) & 0xFF] << 24) );

		// "Version" bizarre de DES sur le Newton.
		if (inNewtonCompat)
		{
			(void) (theKey |= tmpKey);
		}
	} while (charsLeft);

	// On met le résultat dans outKey
	*outKey = theKey;
}

// ============================================================================== //
//         One of the questions that comes up all the time is: How enthusiastic   //
// is our support for UNIX?                                                       //
//         Unix was written on our machines and for our machines many years ago.  //
// Today, much of UNIX being done is done on our machines. Ten percent of our     //
// VAXs are going for UNIX use.  UNIX is a simple language, easy to understand,   //
// easy to get started with. It's great for students, great for somewhat casual   //
// users, and it's great for interchanging programs between different machines.   //
// And so, because of its popularity in these markets, we support it.  We have    //
// good UNIX on VAX and good UNIX on PDP-11s.                                     //
//         It is our belief, however, that serious professional users will run    //
// out of things they can do with UNIX. They'll want a real system and will end   //
// up doing VMS when they get to be serious about programming.                    //
//         With UNIX, if you're looking for something, you can easily and quickly //
// check that small manual and find out that it's not there.  With VMS, no matter //
// what you look for -- it's literally a five-foot shelf of documentation -- if   //
// you look long enough it's there.  That's the difference -- the beauty of UNIX  //
// is it's simple; and the beauty of VMS is that it's all there.                  //
//                 -- Ken Olsen, president of DEC, DECWORLD Vol. 8 No. 5, 1984    //
// [It's been argued that the beauty of UNIX is the same as the beauty of Ken     //
// Olsen's brain.  Ed.]                                                           //
// ============================================================================== //
