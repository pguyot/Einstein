// ==============================
// Fichier:			UBase64.cp
// Projet:			K
//
// CrŽŽ le:			28/01/2003
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
// The Original Code is UBase64.cp.
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

#include <K/Defines/KDefinitions.h>
#include <K/Misc/UBase64.h>

/*
	RFC 2045
	
	Value Encoding  Value Encoding  Value Encoding  Value Encoding
         0 A            17 R            34 i            51 z
         1 B            18 S            35 j            52 0
         2 C            19 T            36 k            53 1
         3 D            20 U            37 l            54 2
         4 E            21 V            38 m            55 3
         5 F            22 W            39 n            56 4
         6 G            23 X            40 o            57 5
         7 H            24 Y            41 p            58 6
         8 I            25 Z            42 q            59 7
         9 J            26 a            43 r            60 8
        10 K            27 b            44 s            61 9
        11 L            28 c            45 t            62 +
        12 M            29 d            46 u            63 /
        13 N            30 e            47 v
        14 O            31 f            48 w         (pad) =
        15 P            32 g            49 x
        16 Q            33 h            50 y
*/

// Mappings for chars from 0x0080
const KUInt8 UBase64::kAlphabet[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

// ------------------------------------------------------------------------- //
//	* UBase64( void )
// ------------------------------------------------------------------------- //
void
UBase64::Encode(
			const void* inBinary,
			KUInt8* outBase64Data,
			KUInt32 inDataSize )
{
	const KUInt8* theBinary = (const KUInt8*) inBinary;
	KUInt8 theRemaining = 0;
	int theCursor = 0;	// Entre 0 et 2.
	
	while (inDataSize > 0)
	{
		// Caractre ˆ traiter.
		KUInt8 theByte = *theBinary;
		switch (theCursor)
		{
			case 0:
				*outBase64Data = kAlphabet[theByte >> 2];
					// ..123456
				outBase64Data++;
				theRemaining = (KUInt8) ((theByte << 4) & 0x3F);
					// 5678.... & 00111111
				break;

			case 1:
				*outBase64Data = kAlphabet[theRemaining | (theByte >> 4)];
					// ..xx.... | ....1234
				outBase64Data++;
				theRemaining = (KUInt8) ((theByte << 2) & 0x3F);
					// 345678.. & 00111111
				break;

			case 2:
				*outBase64Data = kAlphabet[theRemaining | (theByte >> 6)];
					// ..yyyy.. | ......12
				outBase64Data++;
				*outBase64Data = kAlphabet[theByte & 0x3F];
					// 12345678 | 00111111
				outBase64Data++;
				break;
		}
		theCursor = (theCursor + 1) % 3;

		// Caractres suivants.		
		inDataSize--;
		theBinary++;
	}
	
	switch (theCursor)
	{
		case 1:
			outBase64Data[2] = '=';
			
		case 2:
			outBase64Data[1] = '=';
			outBase64Data[0] = kAlphabet[ theRemaining ];
		
		default:
			break;
	}
}

// ------------------------------------------------------------------------- //
//	* Decode( const KUInt8*, void*, KUInt32, KUInt32* )
// ------------------------------------------------------------------------- //
void
UBase64::Decode(
			const KUInt8* inBase64Data,
			void* outBinary,
			KUInt32 inBase64Size,
			KUInt32* outBinarySize )
{
	KUInt8* theBinary = (KUInt8*) outBinary;
								// Curseur sur le binaire en sortie.
	int theCursor = 0;			// Curseur sur les 4 caractres Base64,
								// entre 0 et 3.
	KUInt8 theRemaining = 0;	// Ce qu'il reste entre les caractres Base64
	KUInt32 theBinarySize = 0;	// Taille du binaire.
	
	while (inBase64Size > 0)
	{
		do {
			KUInt8 theBase64Char = *inBase64Data;	// Caractre Base64.
			KUInt8 theValue;							// Valeur du caractre.
			
			if ((theBase64Char >= 'A') && (theBase64Char <= 'Z'))
			{
				theValue = theBase64Char - 'A';
			} else if ((theBase64Char >= 'a') && (theBase64Char <= 'z')) {
				theValue = theBase64Char + 26 - 'a';
			} else if ((theBase64Char >= '0') && (theBase64Char <= '9')) {
				theValue = theBase64Char + 52 - '0';
			} else if (theBase64Char == '+') {
				theValue = 63;
			} else if (theBase64Char == '-') {
				theValue = 64;
			} else {
				break;	// Autre caractre: on sort (et on l'ignore).
			}
			
			switch (theCursor)
			{
				case 0:
					theRemaining = (KUInt8) (theValue << 2);
						// xxxxxx..
					break;

				case 1:
					*theBinary = (KUInt8) (theRemaining | (theValue >> 4));
						// xxxxxx.. | ....00yy
					theBinary++;
					theBinarySize++;
					theRemaining = (KUInt8) (theValue << 4);
						// yyyy....
					break;

				case 2:
					*theBinary = (KUInt8) (theRemaining | (theValue >> 2));
						// yyyy.... | ..00zzzz
					theBinary++;
					theBinarySize++;
					theRemaining = (KUInt8) (theValue << 6);
						// zz......
					break;

				case 3:
					*theBinary = (KUInt8) (theRemaining | theValue);
						// zz...... | 00tttttt
					theBinary++;
					theBinarySize++;
					break;
			}
			theCursor = (theCursor + 1) % 4;
		} while ( false );
		
		// Caractre suivant.		
		inBase64Size--;
		inBase64Data++;
	}

	if (theCursor != 0)
	{
		*theBinary = (KUInt8) theRemaining;
//		theBinary++;
		theBinarySize++;
	}
	
	*outBinarySize = theBinarySize;
}

// ============================================================ //
// Do you guys know what you're doing, or are you just hacking? //
// ============================================================ //
