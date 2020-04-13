// ==============================
// Fichier:			UUTF16Conv.cp
// Projet:			K
//
// Créé le:			01/09/2001
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
// The Original Code is UUTF16Conv.cp.
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

#include <K/Unicode/UUTF16Conv.h>

#include <K/Unicode/UnicodeDefinitions.h>
#include <K/Defines/UByteSex.h>

// OK, I shouldn't duplicate code, but there is a balance between code duplication rule and speed.
// And unicode conversion functions, especially in some projects such as NIL, has to be done quickly.

// ========================	//
//			Common			//
// ========================	//

#if ARMCPP
	#define kSubstituteChar 0x001A
#else
	const KUInt16 UUTF16Conv::kSubstituteChar = 0x001A;
#endif


// ============================	//
//			ISO 8859-1			//
// ============================	//
// http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-1.TXT

// ----------------------------------------------------------------	//
// ToISO88591
// ----------------------------------------------------------------	//
// Convert a string from UTF-16 to ISO 8859-1

UUTF16Conv::EResult
UUTF16Conv::ToISO88591(
	const KUInt16*	inInputBuffer,		// Input Unicode string
	size_t*			ioInputCount,		// On input: number of unicode 16 bits chars in the buffer
										// On output: number of unicode 16 bits chars eaten from the buffer
	KUInt8*			outOutputBuffer,	// Output ISO8859-1 string
	size_t*			ioOutputCount,		// On input: number of bytes/chars available in the buffer
										// On output: number of bytes/chars used in the buffer
	EToOpt			inConvertToOpt		// Option for the transcoder.
	)
{
	EResult theResult = kInputExhausted;	// Default is to exhaust input.
	
	size_t inputSize = *ioInputCount;	// This is (nearly) just an optimization of the loop.
	size_t outputLeft = *ioOutputCount;	// Number of bytes left in the output buffer.
	
	size_t index_done;					// Number of UTF-16 words eaten so far.
	for (index_done = 0; index_done < inputSize; index_done++)
	{
		// Check that we have enough space left in the output buffer.
		if (outputLeft == 0)
		{
			theResult = kOutputExhausted;
			break;
		}
		
		KUInt16 theUnicodeChar = UByteSex_FromBigEndian( *inInputBuffer );	// Current character.
												// Input buffer is big endian, so I might need to convert it.
		if (theUnicodeChar >= 0x0100)	// is MSB not null?
		{
			// Two cases here.
			if ((theUnicodeChar >= 0xD800) && (theUnicodeChar < 0xE000))
			{
				// (a) the character is a multi-byte
				// I should eat this character entirely (and check that I can).
				if (inputSize - index_done - 1)
				{
					index_done++;
				} else {
					// inputSize == index_done - 1, i.e. last character of the input string.
					theResult = kMiddleOfMultiChar;
					break;
				}
			} // else
				// (b) it's not.
				// I don't do anything in that case.
			
			// In any case, this is an unrepresentable char.
			// What's the option?
			if (inConvertToOpt == kRepCharOnUnrepChar)
			{
				theUnicodeChar = kSubstituteChar;
			} else {
				// Stop.
				theResult = kUnrepCharEncountered;
				break;
			}
		}

		*outOutputBuffer = (KUInt8) theUnicodeChar;	// Fill the output buffer.

		inInputBuffer++;	// Next character
		outOutputBuffer++;
		outputLeft--;
	}
	
	// Update counts
	*ioInputCount = index_done;
	*ioOutputCount = *ioOutputCount - outputLeft;
	
	return theResult;
}

// ----------------------------------------------------------------	//
// FromISO88591
// ----------------------------------------------------------------	//
// Convert a string from ISO 8859-1 to UTF-16

void
UUTF16Conv::FromISO88591(
	const KUInt8*	inInputBuffer,		// Input ISO-8859-1 string
	KUInt16*			outOutputBuffer,	// Output Unicode string
	size_t			inCount				// Number of bytes/words available in both buffers
	)
{
	for (; inCount; inCount--)
	{
		*outOutputBuffer = UByteSex_ToBigEndian( (KUInt16) *inInputBuffer );
											// Fill the output buffer.
											// Output buffer is big endian.

		inInputBuffer++;	// Next character
		outOutputBuffer++;
	}
}

// ============================	//
//			ISO 8859-2			//
// ============================	//
// http://www.unicode.org/Public/MAPPINGS/ISO8859/8859-2.TXT

// Mappings for chars which aren't mapped with a simple extension of the size.
// Sorted (in reverse order) by unicode chars.
// Last element is 0,0 so I'm sure to get out of the loop with unrepresentable
// set to true for chars < 0x0102
const KUInt16 UUTF16Conv::kToISO88592Table[58][2] = 
{
	{ 0xBD, 0x02DD }, { 0xB2, 0x02DB }, { 0xFF, 0x02D9 }, { 0xA2, 0x02D8 },
	{ 0xB7, 0x02C7 }, { 0xBE, 0x017E }, { 0xAE, 0x017D }, { 0xBF, 0x017C },
	{ 0xAF, 0x017B }, { 0xBC, 0x017A }, { 0xAC, 0x0179 }, { 0xFB, 0x0171 },
	{ 0xDB, 0x0170 }, { 0xF9, 0x016F }, { 0xD9, 0x016E }, { 0xBB, 0x0165 },
	{ 0xAB, 0x0164 }, { 0xFE, 0x0163 }, { 0xDE, 0x0162 }, { 0xB9, 0x0161 },
	{ 0xA9, 0x0160 }, { 0xBA, 0x015F }, { 0xAA, 0x015E }, { 0xB6, 0x015B },
	{ 0xA6, 0x015A }, { 0xF8, 0x0159 }, { 0xD8, 0x0158 }, { 0xE0, 0x0155 },
	{ 0xC0, 0x0154 }, { 0xF5, 0x0151 }, { 0xD5, 0x0150 }, { 0xF2, 0x0148 },
	{ 0xD2, 0x0147 }, { 0xF1, 0x0144 }, { 0xD1, 0x0143 }, { 0xB3, 0x0142 },
	{ 0xA3, 0x0141 }, { 0xB5, 0x013E }, { 0xA5, 0x013D }, { 0xE5, 0x013A },
	{ 0xC5, 0x0139 }, { 0xEC, 0x011B }, { 0xCC, 0x011A }, { 0xEA, 0x0119 },
	{ 0xCA, 0x0118 }, { 0xF0, 0x0111 }, { 0xD0, 0x0110 }, { 0xEF, 0x010F },
	{ 0xCF, 0x010E }, { 0xE8, 0x010D }, { 0xC8, 0x010C }, { 0xE6, 0x0107 },
	{ 0xC6, 0x0106 }, { 0xB1, 0x0105 }, { 0xA1, 0x0104 }, { 0xE3, 0x0103 },
	{ 0xC3, 0x0102 }, { 0x00, 0x0000 }
};

// Mappings for chars from 0x00A0
// Also used in the to conversion to check that a char 00XX is indeed within ISO 8859-2
const KUInt16 UUTF16Conv::kFromISO88592Table[0x60] =
{
	0x00A0, 0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7,
	0x00A8, 0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B,
	0x00B0, 0x0105, 0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7,
	0x00B8, 0x0161, 0x015F, 0x0165, 0x017A, 0x02DD, 0x017E, 0x017C,
	0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
	0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
	0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
	0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
	0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
	0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
	0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
	0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9
};

// ----------------------------------------------------------------	//
// ToISO88592
// ----------------------------------------------------------------	//
// Convert a string from UTF-16 to ISO 8859-2

UUTF16Conv::EResult
UUTF16Conv::ToISO88592(
	const KUInt16*	inInputBuffer,		// Input Unicode string
	size_t*			ioInputCount,		// On input: number of unicode 16 bits chars in the buffer
										// On output: number of unicode 16 bits chars eaten from the buffer
	KUInt8*			outOutputBuffer,	// Output ISO8859-2 string
	size_t*			ioOutputCount,		// On input: number of bytes/chars available in the buffer
										// On output: number of bytes/chars used in the buffer
	EToOpt			inConvertToOpt		// Option for the transcoder.
	)
{
	EResult theResult = kInputExhausted;	// Default is to exhaust input.
	
	size_t inputSize = *ioInputCount;	// This is (nearly) just an optimization of the loop.
	size_t outputLeft = *ioOutputCount;	// Number of bytes left in the output buffer.
	
	size_t index_done;					// Number of UTF-16 words eaten so far.
	for (index_done = 0; index_done < inputSize; index_done++)
	{
		// Check that we have enough space left in the output buffer.
		if (outputLeft == 0)
		{
			theResult = kOutputExhausted;
			break;
		}
		
		KUInt16 theUnicodeChar = UByteSex_FromBigEndian( *inInputBuffer );	// Current character.
												// Input buffer is big endian, so I might need to convert it.
		if (theUnicodeChar > 0x00A0)			// 0x00A0 and anything below is translated to the same value casted to 8 bits.
		{
			Boolean unrepresentable = false;		// Flag to know if an unrepresentable character has been met.
			// Two cases: the Unicode char is < 0x0100: check that we have 0x00XX -> 0xXX with the From table.
			// the Unicode char is >= 0x0100: use the To table.

			if (theUnicodeChar >= 0x0100)
			{
				// Two cases here.
				if ((theUnicodeChar >= 0xD800) && (theUnicodeChar < 0xE000))
				{
					// (a) the character is a multi-byte
					// I should eat this character entirely (and check that I can).
					if (inputSize - index_done - 1)
					{
						index_done++;
					} else {
						// inputSize == index_done - 1, i.e. last character of the input string.
						theResult = kMiddleOfMultiChar;
						break;
					}
				} else {
					// (b) it's not.

					// Iterate in the To table.
					int indexToTable = 0;
					while(1)	// Because last element of the table is 0,0, I'm sure to get out of the loop.
					{
						KUInt16 theChar	// Element of the table.
							= kToISO88592Table[ indexToTable ][1];
						if (theUnicodeChar == theChar)
						{
							theUnicodeChar = kToISO88592Table[ indexToTable ][0];
							break;
						}
						if (theUnicodeChar > theChar)	// Characters in the to table are sorted (in descending order)
						{
							// Not found.
							unrepresentable = true;
							break;
						}
					
						indexToTable++;
					}
				} // if ((theUnicodeChar >= 0xD800) && (theUnicodeChar < 0xE000))
			} else {
				// Check that we have 0x00XX -> 0xXX with the From table.
				if (kFromISO88592Table[theUnicodeChar - 0xA0] != theUnicodeChar)
					unrepresentable = true;
			} // if (theUnicodeChar >= 0x0100)
			
			if (unrepresentable)
			{
				// Unrepresentable char.
				// What's the option?
				if (inConvertToOpt == kRepCharOnUnrepChar)
				{
					theUnicodeChar = kSubstituteChar;
				} else {
					// Stop.
					theResult = kUnrepCharEncountered;
					break;
				}
			}
		} // else { ... } : nothing to do as 0x0000-0x00A0 -> same value truncated to 8 bits.

		*outOutputBuffer = (KUInt8) theUnicodeChar;	// Fill the output buffer.

		inInputBuffer++;	// Next character
		outOutputBuffer++;
		outputLeft--;
	}
	
	// Update counts
	*ioInputCount = index_done;
	*ioOutputCount = *ioOutputCount - outputLeft;
	
	return theResult;
}

// ----------------------------------------------------------------	//
// FromISO88592
// ----------------------------------------------------------------	//
// Convert a string from ISO 8859-2 to UTF-16

void
UUTF16Conv::FromISO88592(
	const KUInt8*	inInputBuffer,		// Input ISO-8859-2 string
	KUInt16*			outOutputBuffer,	// Output Unicode string
	size_t			inCount				// Number of bytes/words available in both buffers
	)
{
	for (; inCount; inCount--)
	{
		KUInt16 theISOChar = (KUInt16) *inInputBuffer;	// Current character.
		if (theISOChar > 0xA0)	// 0x00A0 and anything below is translated to the same value casted to 16 bits.
		{
			theISOChar = kFromISO88592Table[theISOChar - 0xA0];
		}
		*outOutputBuffer = UByteSex_ToBigEndian( theISOChar );
											// Fill the output buffer.
											// Output buffer is big endian.

		inInputBuffer++;	// Next character
		outOutputBuffer++;
	}
}

// ========================	//
//			ASCII			//7
// ========================	//
// Anything greater than 0x7F is unrepresentable.

// ----------------------------------------------------------------	//
// ToASCII
// ----------------------------------------------------------------	//
// Convert a string from UTF-16 to US ASCII

UUTF16Conv::EResult
UUTF16Conv::ToASCII(
	const KUInt16*	inInputBuffer,		// Input Unicode string
	size_t*			ioInputCount,		// On input: number of unicode 16 bits chars in the buffer
										// On output: number of unicode 16 bits chars eaten from the buffer
	KUInt8*			outOutputBuffer,	// Output ASCII string
	size_t*			ioOutputCount,		// On input: number of bytes/chars available in the buffer
										// On output: number of bytes/chars used in the buffer
	EToOpt			inConvertToOpt		// Option for the transcoder.
	)
{
	EResult theResult = kInputExhausted;	// Default is to exhaust input.
	
	size_t inputSize = *ioInputCount;		// This is (nearly) just an optimization of the loop.
	size_t outputLeft = *ioOutputCount;		// Number of bytes left in the output buffer.
	
	size_t index_done;						// Number of UTF-16 words eaten so far.
	for (index_done = 0; index_done < inputSize; index_done++)
	{
		// Check that we have enough space left in the output buffer.
		if (outputLeft == 0)
		{
			theResult = kOutputExhausted;
			break;
		}
		
		KUInt16 theUnicodeChar = UByteSex_FromBigEndian( *inInputBuffer );	// Current character.
												// Input buffer is big endian, so I might need to convert it.
		if (theUnicodeChar >= 0x0080)			// Anything greater than 0x7F is unrepresentable.
		{
			// Two cases here.
			if ((theUnicodeChar >= 0xD800) && (theUnicodeChar < 0xE000))
			{
				// (a) the character is a multi-byte
				// I should eat this character entirely (and check that I can).
				if (inputSize - index_done - 1)
				{
					index_done++;
				} else {
					// inputSize == index_done - 1, i.e. last character of the input string.
					theResult = kMiddleOfMultiChar;
					break;
				}
			} // else
				// (b) it's not.
				// I don't do anything in that case.
			
			// In any case, this is an unrepresentable char.
			// What's the option?
			if (inConvertToOpt == kRepCharOnUnrepChar)
			{
				theUnicodeChar = kSubstituteChar;
			} else {
				// Stop.
				theResult = kUnrepCharEncountered;
				break;
			}
		}

		*outOutputBuffer = (KUInt8) theUnicodeChar;	// Fill the output buffer.

		inInputBuffer++;	// Next character
		outOutputBuffer++;
		outputLeft--;
	}
	
	// Update counts
	*ioInputCount = index_done;
	*ioOutputCount = *ioOutputCount - outputLeft;
	
	return theResult;
}

// ----------------------------------------------------------------	//
// FromASCII
// ----------------------------------------------------------------	//
// Convert a string from US ASCII to UTF-16

// Additional notes:
// This function works as expected if inInputBuffer is only made of
// ASCII chars. Otherwise, result is undocumented.
// If you really want to know, it will map chars greater or equal to 0x80
// to the same value extended to 16 bits. But don't rely on it, this
// may change in the future.

void
UUTF16Conv::FromASCII(
	const KUInt8*	inInputBuffer,		// Input ASCII string
	KUInt16*			outOutputBuffer,	// Output Unicode string
	size_t			inCount				// Number of bytes/words available in both buffers
	)
{
	FromISO88591( inInputBuffer, outOutputBuffer, inCount );
}

// ============================	//
//			MacRoman			//
// ============================	//
// http://www.unicode.org/Public/MAPPINGS/VENDORS/APPLE/ROMAN.TXT

// Mappings for chars which aren't mapped with a simple extension of the size.
// Sorted (in reverse order) by unicode chars.
// Last element is 0,0 so I'm sure to get out of the loop with unrepresentable
// set to true for chars < 0x080
const KUInt16 UUTF16Conv::kToMacRomanTable[129][2] = 
{
	{ 0xDE, 0xFB01 }, { 0xDF, 0xFB02 }, { 0xF0, 0xF8FF }, { 0xD7, 0x25CA },
	{ 0xB3, 0x2265 }, { 0xB2, 0x2264 }, { 0xAD, 0x2260 }, { 0xC5, 0x2248 },
	{ 0xBA, 0x222B }, { 0xB0, 0x221E }, { 0xC3, 0x221A }, { 0xB7, 0x2211 },
	{ 0xB8, 0x220F }, { 0xC6, 0x2206 }, { 0xB6, 0x2202 }, { 0xAA, 0x2122 },
	{ 0xDB, 0x20AC }, { 0xDA, 0x2044 }, { 0xDD, 0x203A }, { 0xDC, 0x2039 },
	{ 0xE4, 0x2030 }, { 0xC9, 0x2026 }, { 0xA5, 0x2022 }, { 0xE0, 0x2021 },
	{ 0xA0, 0x2020 }, { 0xE3, 0x201E }, { 0xD3, 0x201D }, { 0xD2, 0x201C },
	{ 0xE2, 0x201A }, { 0xD5, 0x2019 }, { 0xD4, 0x2018 }, { 0xD1, 0x2014 },
	{ 0xD0, 0x2013 }, { 0xB9, 0x03C0 }, { 0xBD, 0x03A9 }, { 0xFD, 0x02DD },
	{ 0xF7, 0x02DC }, { 0xFE, 0x02DB }, { 0xFB, 0x02DA }, { 0xFA, 0x02D9 },
	{ 0xF9, 0x02D8 }, { 0xFF, 0x02C7 }, { 0xF6, 0x02C6 }, { 0xC4, 0x0192 },
	{ 0xD9, 0x0178 }, { 0xCF, 0x0153 }, { 0xCE, 0x0152 }, { 0xF5, 0x0131 },
	{ 0xD8, 0x00FF }, { 0x9F, 0x00FC }, { 0x9E, 0x00FB }, { 0x9C, 0x00FA },
	{ 0x9D, 0x00F9 }, { 0xBF, 0x00F8 }, { 0xD6, 0x00F7 }, { 0x9A, 0x00F6 },
	{ 0x9B, 0x00F5 }, { 0x99, 0x00F4 }, { 0x97, 0x00F3 }, { 0x98, 0x00F2 },
	{ 0x96, 0x00F1 }, { 0x95, 0x00EF }, { 0x94, 0x00EE }, { 0x92, 0x00ED },
	{ 0x93, 0x00EC }, { 0x91, 0x00EB }, { 0x90, 0x00EA }, { 0x8E, 0x00E9 },
	{ 0x8F, 0x00E8 }, { 0x8D, 0x00E7 }, { 0xBE, 0x00E6 }, { 0x8C, 0x00E5 },
	{ 0x8A, 0x00E4 }, { 0x8B, 0x00E3 }, { 0x89, 0x00E2 }, { 0x87, 0x00E1 },
	{ 0x88, 0x00E0 }, { 0xA7, 0x00DF }, { 0x86, 0x00DC }, { 0xF3, 0x00DB },
	{ 0xF2, 0x00DA }, { 0xF4, 0x00D9 }, { 0xAF, 0x00D8 }, { 0x85, 0x00D6 },
	{ 0xCD, 0x00D5 }, { 0xEF, 0x00D4 }, { 0xEE, 0x00D3 }, { 0xF1, 0x00D2 },
	{ 0x84, 0x00D1 }, { 0xEC, 0x00CF }, { 0xEB, 0x00CE }, { 0xEA, 0x00CD },
	{ 0xED, 0x00CC }, { 0xE8, 0x00CB }, { 0xE6, 0x00CA }, { 0x83, 0x00C9 },
	{ 0xE9, 0x00C8 }, { 0x82, 0x00C7 }, { 0xAE, 0x00C6 }, { 0x81, 0x00C5 },
	{ 0x80, 0x00C4 }, { 0xCC, 0x00C3 }, { 0xE5, 0x00C2 }, { 0xE7, 0x00C1 },
	{ 0xCB, 0x00C0 }, { 0xC0, 0x00BF }, { 0xC8, 0x00BB }, { 0xBC, 0x00BA },
	{ 0xFC, 0x00B8 }, { 0xE1, 0x00B7 }, { 0xA6, 0x00B6 }, { 0xB5, 0x00B5 },
	{ 0xAB, 0x00B4 }, { 0xB1, 0x00B1 }, { 0xA1, 0x00B0 }, { 0xF8, 0x00AF },
	{ 0xA8, 0x00AE }, { 0xC2, 0x00AC }, { 0xC7, 0x00AB }, { 0xBB, 0x00AA },
	{ 0xA9, 0x00A9 }, { 0xAC, 0x00A8 }, { 0xA4, 0x00A7 }, { 0xB4, 0x00A5 },
	{ 0xA3, 0x00A3 }, { 0xA2, 0x00A2 }, { 0xC1, 0x00A1 }, { 0xCA, 0x00A0 },
	{ 0x00, 0x0000 }
};


// Mappings for chars from 0x0080
// Also used in the to conversion to check that a char 00XX is indeed within MacRoman
const KUInt16 UUTF16Conv::kFromMacRomanTable[0x80] =
{
	0x00C4, 0x00C5, 0x00C7, 0x00C9, 0x00D1, 0x00D6, 0x00DC, 0x00E1,
	0x00E0, 0x00E2, 0x00E4, 0x00E3, 0x00E5, 0x00E7, 0x00E9, 0x00E8,
	0x00EA, 0x00EB, 0x00ED, 0x00EC, 0x00EE, 0x00EF, 0x00F1, 0x00F3,
	0x00F2, 0x00F4, 0x00F6, 0x00F5, 0x00FA, 0x00F9, 0x00FB, 0x00FC,
	0x2020, 0x00B0, 0x00A2, 0x00A3, 0x00A7, 0x2022, 0x00B6, 0x00DF,
	0x00AE, 0x00A9, 0x2122, 0x00B4, 0x00A8, 0x2260, 0x00C6, 0x00D8,
	0x221E, 0x00B1, 0x2264, 0x2265, 0x00A5, 0x00B5, 0x2202, 0x2211,
	0x220F, 0x03C0, 0x222B, 0x00AA, 0x00BA, 0x03A9, 0x00E6, 0x00F8,
	0x00BF, 0x00A1, 0x00AC, 0x221A, 0x0192, 0x2248, 0x2206, 0x00AB,
	0x00BB, 0x2026, 0x00A0, 0x00C0, 0x00C3, 0x00D5, 0x0152, 0x0153,
	0x2013, 0x2014, 0x201C, 0x201D, 0x2018, 0x2019, 0x00F7, 0x25CA,
	0x00FF, 0x0178, 0x2044, 0x20AC, 0x2039, 0x203A, 0xFB01, 0xFB02,
	0x2021, 0x00B7, 0x201A, 0x201E, 0x2030, 0x00C2, 0x00CA, 0x00C1,
	0x00CB, 0x00C8, 0x00CD, 0x00CE, 0x00CF, 0x00CC, 0x00D3, 0x00D4,
	0xF8FF, 0x00D2, 0x00DA, 0x00DB, 0x00D9, 0x0131, 0x02C6, 0x02DC,
	0x00AF, 0x02D8, 0x02D9, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7
	
};

// ----------------------------------------------------------------	//
//	* ToMacRoman( const KUInt16*, size_t*, KUInt8*, size_t*, EToOpt )
// ----------------------------------------------------------------	//
UUTF16Conv::EResult
UUTF16Conv::ToMacRoman(
	const KUInt16*	inInputBuffer,		// Input Unicode string
	size_t*			ioInputCount,		// On input: number of unicode 16 bits chars in the buffer
										// On output: number of unicode 16 bits chars eaten from the buffer
	KUInt8*			outOutputBuffer,	// Output MacRoman string
	size_t*			ioOutputCount,		// On input: number of bytes/chars available in the buffer
										// On output: number of bytes/chars used in the buffer
	EToOpt			inConvertToOpt		// Option for the transcoder.
	)
{
	EResult theResult = kInputExhausted;	// Default is to exhaust input.
	
	size_t inputSize = *ioInputCount;	// This is (nearly) just an optimization of the loop.
	size_t outputLeft = *ioOutputCount;	// Number of bytes left in the output buffer.
	
	size_t index_done;					// Number of UTF-16 words eaten so far.
	for (index_done = 0; index_done < inputSize; index_done++)
	{
		// Check that we have enough space left in the output buffer.
		if (outputLeft == 0)
		{
			theResult = kOutputExhausted;
			break;
		}
		
		KUInt16 theUnicodeChar = UByteSex_FromBigEndian( *inInputBuffer );	// Current character.
												// Input buffer is big endian, so I might need to convert it.

		if (theUnicodeChar >= 0x0080)
			// anything below 0x0080 is translated to the same value casted to 8 bits.
			// unicode characters between 0x80 and 0x9F cannot be represented in MacRoman
		{
			Boolean unrepresentable = false;	// Whether the char is unrepresentable or not.
			
			if (theUnicodeChar >= 0x00A0)
			{
				// Two cases here.
				if ((theUnicodeChar >= 0xD800) && (theUnicodeChar < 0xE000))
				{
					// (a) the character is a multi-byte
					// I should eat this character entirely (and check that I can).
					if (inputSize - index_done - 1)
					{
						index_done++;
					} else {
						// inputSize == index_done - 1, i.e. last character of the input string.
						theResult = kMiddleOfMultiChar;
						break;
					}
				} else {
					// (b) it's not.

					// Iterate in the To table.
					int indexToTable = 0;
					while(1)	// Because last element of the table is 0,0, I'm sure to get out of the loop.
					{
						KUInt16 theChar	// Element of the table.
							= kToMacRomanTable[ indexToTable ][1];
						if (theUnicodeChar == theChar)
						{
							theUnicodeChar = kToMacRomanTable[ indexToTable ][0];
							break;
						}
						if (theUnicodeChar > theChar)	// Characters in the to table are sorted (in descending order)
						{
							// Not found.
							unrepresentable = true;
							break;
						}
					
						indexToTable++;
					}
				} // if ((theUnicodeChar >= 0xD800) && (theUnicodeChar < 0xE000))
			} else {
				unrepresentable = true;
			}
			
			if (unrepresentable)
			{
				// Unrepresentable char.
				// What's the option?
				if (inConvertToOpt == kRepCharOnUnrepChar)
				{
					theUnicodeChar = kSubstituteChar;
				} else {
					// Stop.
					theResult = kUnrepCharEncountered;
					break;
				}
			}
		} // else { ... } : nothing to do as 0x0000-0x007F -> same value truncated to 8 bits.

		*outOutputBuffer = (KUInt8) theUnicodeChar;	// Fill the output buffer.

		inInputBuffer++;	// Next character
		outOutputBuffer++;
		outputLeft--;
	}
	
	// Update counts
	*ioInputCount = index_done;
	*ioOutputCount = *ioOutputCount - outputLeft;
	
	return theResult;
}

// ----------------------------------------------------------------	//
//	* FromMacRoman( const KUInt8*, KUInt16*, size_t )
// ----------------------------------------------------------------	//
void
UUTF16Conv::FromMacRoman(
	const KUInt8*	inInputBuffer,		// Input MacRoman string
	KUInt16*			outOutputBuffer,	// Output Unicode string
	size_t			inCount				// Number of bytes/words available in both buffers
	)
{
	for (; inCount; inCount--)
	{
		KUInt16 theMacRomanChar = (KUInt16) *inInputBuffer;	// Current character.
		if (theMacRomanChar >= 0x80)	// Anything below 0x80 is translated to the same value casted to 16 bits.
		{
			theMacRomanChar = kFromMacRomanTable[theMacRomanChar - 0x80];
		}
		*outOutputBuffer =  UByteSex_ToBigEndian( theMacRomanChar );
											// Fill the output buffer.
											// Output buffer is big endian.

		inInputBuffer++;	// Next character
		outOutputBuffer++;
	}
}

// ========================	//
//			UCS4			//
// ========================	//

// ----------------------------------------------------------------	//
//	* ToUCS4( const KUInt16*, size_t*, KUInt32*, size_t* )
// ----------------------------------------------------------------	//
UUTF16Conv::EResult
UUTF16Conv::ToUCS4(
	const KUInt16*	inInputBuffer,
	size_t*			ioInputCount,
	KUInt32*			outOutputBuffer,
	size_t*			ioOutputCount
	)
{
	EResult theResult = kInputExhausted;
	
	size_t inputCount = *ioInputCount;
	size_t outputCount = *ioOutputCount;
	
	while ((inputCount > 0) && (outputCount > 0))
	{
		KUInt16 theChar = UByteSex_FromBigEndian( *inInputBuffer );
		inInputBuffer++;
		
		if ((theChar < 0xD800) || (theChar >= 0xE000))
		{
			*outOutputBuffer = (KUInt32) theChar;
			outOutputBuffer++;
			outputCount--;
			inputCount--;
		} else {
			if (inputCount > 1)
			{
				KUInt32 theUCS4Char = (KUInt32) (
							((theChar - 0xD800) * 0x400)
							+ (*inInputBuffer - 0xDC00)
							+ 0x00010000 );
				*outOutputBuffer = UByteSex_ToBigEndian( theUCS4Char );
				outOutputBuffer++;
				inInputBuffer++;
				
				outputCount--;
				inputCount -= 2;
			} else {
				// Pas assez de caractères dans la mémoire d'entrée.
				theResult = kMiddleOfMultiChar;
				break;
			}
		}
	}
	
	*ioInputCount -= inputCount;
	*ioOutputCount -= outputCount;
	
	if (theResult == kInputExhausted)
	{
		if ((inputCount > 0) && (outputCount == 0))
		{
			theResult = kOutputExhausted;
		}
	}
	
	return theResult;
}

// ----------------------------------------------------------------	//
//	* FromUCS4( const KUInt32*, size_t*, KUInt16*, size_t* )
// ----------------------------------------------------------------	//
UUTF16Conv::EResult
UUTF16Conv::FromUCS4(
	const KUInt32*	inInputBuffer,
	size_t*			ioInputCount,
	KUInt16*			outOutputBuffer,
	size_t*			ioOutputCount
	)
{
	// Résultat (par défaut, on a terminé).
	EResult theResult = kInputExhausted;
	
	// Nombre de mots de 32 bits en entrée.
	size_t inputCount = *ioInputCount;
	// Nombre de mots de 16 bits en entrée.
	size_t outputCount = *ioOutputCount;
	
	// Tant qu'il en reste.
	while ((inputCount > 0) && (outputCount > 0))
	{
		// Le caractère UCS-4 en entrée.
		KUInt32 theChar = UByteSex_FromBigEndian( *inInputBuffer );
		inInputBuffer++;
		
		if (theChar < 0x00010000)
		{
			*outOutputBuffer = (KUInt16) theChar;
			outOutputBuffer++;
			outputCount--;
			inputCount--;
		} else {
			if (outputCount > 1)
			{
				theChar -= 0x00010000;
				*outOutputBuffer = UByteSex_ToBigEndian( (KUInt16) ((theChar / 0x400) + 0xD800) );
				outOutputBuffer++;
				*outOutputBuffer = UByteSex_ToBigEndian( (KUInt16) ((theChar % 0x400) + 0xDC00) );
				outOutputBuffer++;
				
				outputCount -= 2;
				inputCount--;
			} else {
				// Pas assez de caractères dans la mémoire de sortie.
				theResult = kMiddleOfMultiChar;
				break;
			}
		}
	}
	
	// Mise à jour des valeurs de sortie.
	*ioInputCount -= inputCount;
	*ioOutputCount -= outputCount;
	
	if (theResult == kInputExhausted)
	{
		if ((inputCount > 0) && (outputCount == 0))
		{
			theResult = kOutputExhausted;
		}
	}
	
	return theResult;
}

// ========================	//
//			UTF-8			//
// ========================	//
// http://www.faqs.org/rfc/rfc2279.txt

/*
UCS-4 range (hex.)           UTF-8 octet sequence (binary)
   0000 0000-0000 007F   0xxxxxxx
   0000 0080-0000 07FF   110xxxxx 10xxxxxx
   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
   0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx

   Encoding from UCS-4 to UTF-8 proceeds as follows:

   1) Determine the number of octets required from the character value
      and the first column of the table above.  It is important to note
      that the rows of the table are mutually exclusive, i.e. there is
      only one valid way to encode a given UCS-4 character.

   2) Prepare the high-order bits of the octets as per the second column
      of the table.

   3) Fill in the bits marked x from the bits of the character value,
      starting from the lower-order bits of the character value and
      putting them first in the last octet of the sequence, then the
      next to last, etc. until all x bits are filled in.

      The algorithm for encoding UCS-2 (or Unicode) to UTF-8 can be
      obtained from the above, in principle, by simply extending each
      UCS-2 character with two zero-valued octets.  However, pairs of
      UCS-2 values between D800 and DFFF (surrogate pairs in Unicode
      parlance), being actually UCS-4 characters transformed through
      UTF-16, need special treatment: the UTF-16 transformation must be
      undone, yielding a UCS-4 character that is then transformed as
      above.
*/

// ----------------------------------------------------------------	//
//	* ToUTF8( const KUInt16*, size_t*, void*, size_t* )
// ----------------------------------------------------------------	//
UUTF16Conv::EResult
UUTF16Conv::ToUTF8(
	const KUInt16*	inInputBuffer,
	size_t*			ioInputCount,
	void*			outOutputBuffer,
	size_t*			ioOutputCount
	)
{
	// Résultat (par défaut, on a terminé).
	EResult theResult = kInputExhausted;

	// Nombre de mots en entrée.
	size_t inputCount = *ioInputCount;
	// Nombre d'octets en sortie.	
	size_t outputCount = *ioOutputCount;
	// Curseur sur la sortie.
	KUInt8* theOutputBuffer = (KUInt8*) outOutputBuffer;
	
	// Si tout va bien, nouvelle valeur du nombre de mots
	// disponibles en entrée.
	size_t newInputCount;
	
	while ((inputCount > 0) && (outputCount > 0))
	{
		// Le caractère UTF-16 à convertir.
		KUInt16 theChar = UByteSex_FromBigEndian( *inInputBuffer );
		// Le même en UCS-4
		KUInt32 theUCS4Char;
		inInputBuffer++;
		
		// Extraction du caractère UCS4
		if ((theChar < 0xD800) || (theChar >= 0xE000))
		{
			theUCS4Char = (KUInt32) theChar;
			newInputCount = inputCount - 1;
		} else {
			if (inputCount > 1)
			{
				theUCS4Char = (KUInt32) (
							((theChar - 0xD800) * 0x400)
							+ (*inInputBuffer - 0xDC00)
							+ 0x00010000 );
				inInputBuffer++;
				newInputCount = inputCount - 2;
			} else {
				// Pas assez de caractères dans la mémoire d'entrée.
				theResult = kMiddleOfMultiChar;
				break;
			}
		}

		// Conversion en UTF-8
		// En vérifiant qu'il y a assez de place.
		size_t theCharSize;	// Taille du caractère en UTF-8
		if (theUCS4Char <= 0x0000007F)
		{
			theCharSize = 1;
		} else if (theUCS4Char <= 0x000007FF) {
			theCharSize = 2;
		} else if (theUCS4Char <= 0x0000FFFF) {
			theCharSize = 3;
		} else if (theUCS4Char <= 0x001FFFFF) {
			theCharSize = 4;
		} else if (theUCS4Char <= 0x03FFFFFF) {
			theCharSize = 5;
		} else {
			theCharSize = 6;
		}
		
		if (theCharSize == 1)
		{
			*theOutputBuffer = (KUInt8) theUCS4Char;
			theOutputBuffer++;
			outputCount--;
		} else {
			if (outputCount < theCharSize)
			{
				// Pas assez de caractères dans la mémoire de sortie.
				theResult = kMiddleOfMultiChar;
				break;
			}
		
			// Ensuite, on écrit les octets
			// en partant de la droite (c'est plus simple).
			// Je commence par les n-1 octets.
			int indexUTF8Char;
			for (indexUTF8Char = ((int) theCharSize) - 1; indexUTF8Char > 0; indexUTF8Char--)
			{
				theOutputBuffer[indexUTF8Char] = (KUInt8) ((theUCS4Char & 0x3F) | 0x80);
				theUCS4Char = theUCS4Char >> 6;
			}
		
			// Puis le dernier.
			*theOutputBuffer = (KUInt8) (theUCS4Char | (0xFF00 >> theCharSize));
			
			// Mise à jour des pointeurs.
			theOutputBuffer += theCharSize;
			outputCount -= theCharSize;
		}

		// Tout c'est bien passé, on note la nouvelle valeur de inputCount.
		inputCount = newInputCount;
	}

	// Fini. On écrit les valeurs de sortie.	
	*ioInputCount -= inputCount;
	*ioOutputCount -= outputCount;
	
	// Reste-t-il des données?
	if (theResult == kInputExhausted)
	{
		if ((inputCount > 0) && (outputCount == 0))
		{
			theResult = kOutputExhausted;
		}
	}
	
	return theResult;
}

// ----------------------------------------------------------------	//
//	* FromUTF8( const void*, size_t*, KUInt16*, size_t* )
// ----------------------------------------------------------------	//
UUTF16Conv::EResult
UUTF16Conv::FromUTF8(
	const void*		inInputBuffer,
	size_t*			ioInputCount,
	KUInt16*		outOutputBuffer,
	size_t*			ioOutputCount
	)
{
	// Résultat (par défaut, on a terminé).
	EResult theResult = kInputExhausted;
	
	// Nombre d'octets en entrée.
	size_t inputCount = *ioInputCount;
	// Nombre de mots de 16 bits en sortie.
	size_t outputCount = *ioOutputCount;
	// Curseur sur l'entrée
	const KUInt8* theInputBuffer = (const KUInt8*) inInputBuffer;
	
	// Si tout va bien, nouvelle valeur du nombre d'octets
	// disponibles en entrée.
	size_t newInputCount;

	while ((inputCount > 0) && (outputCount > 0))
	{
		// Extraction du caractère UCS-4.
		KUInt32 theUCS4Char;
		
		// Premier caractère UTF-8.
		KUInt8 theFirstChar = *theInputBuffer;
		size_t theCharSize;	// Taille du caractère en UTF-8
		theInputBuffer++;
		if ((theFirstChar & 0x80) == 0)
		{
			theCharSize = 1;
			theUCS4Char = theFirstChar;
		} else if ((theFirstChar & 0xE0) == 0xC0) {
			theCharSize = 2;
			theUCS4Char = (KUInt32) theFirstChar & 0x1F;
		} else if ((theFirstChar & 0xF0) == 0xE0) {
			theCharSize = 3;
			theUCS4Char = (KUInt32) theFirstChar & 0x0F;
		} else if ((theFirstChar & 0xF8) == 0xF0) {
			theCharSize = 4;
			theUCS4Char = (KUInt32) theFirstChar & 0x07;
		} else if ((theFirstChar & 0xFC) == 0xF8) {
			theCharSize = 5;
			theUCS4Char = (KUInt32) theFirstChar & 0x03;
		} else /* if ((theFirstChar & 0xFE) == 0xFC) */ {
			theCharSize = 6;
			theUCS4Char = (KUInt32) theFirstChar & 0x01;
		}
		
		if (inputCount < theCharSize)
		{
			// Pas assez de caractères dans la mémoire d'entrée.
			theResult = kMiddleOfMultiChar;
			break;
		}
		
		newInputCount = inputCount - theCharSize;
		
		for ( ; theCharSize > 1; theCharSize--)
		{
			theUCS4Char = (theUCS4Char << 6) | (*theInputBuffer & 0x3F);
			theInputBuffer++;
		}

		if (theUCS4Char < 0x00010000)
		{
			*outOutputBuffer = UByteSex_ToBigEndian( (KUInt16) theUCS4Char );
			outOutputBuffer++;
			outputCount--;
		} else {
			if (outputCount > 1)
			{
				theUCS4Char -= 0x00010000;
				*outOutputBuffer = UByteSex_ToBigEndian( (KUInt16) ((theUCS4Char / 0x400) + 0xD800) );
				outOutputBuffer++;
				*outOutputBuffer = UByteSex_ToBigEndian( (KUInt16) ((theUCS4Char % 0x400) + 0xDC00) );
				outOutputBuffer++;
				
				outputCount -= 2;
			} else {
				// Pas assez de caractères dans la mémoire de sortie.
				theResult = kMiddleOfMultiChar;
				break;
			}
		}

		// Tout c'est bien passé, on note la nouvelle valeur de inputCount.
		inputCount = newInputCount;
	}
	
	*ioInputCount -= inputCount;
	*ioOutputCount -= outputCount;
	
	if (theResult == kInputExhausted)
	{
		if ((inputCount > 0) && (outputCount == 0))
		{
			theResult = kOutputExhausted;
		}
	}
	
	return theResult;
}

// ======================================================= //
// When Dexter's on the Internet, can Hell be far behind?" //
// ======================================================= //
