// ==============================
// Fichier:			UUTF16Conv.h
// Projet:			K
//
// Créé le:			1/09/2001
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
// The Original Code is UUTF16Conv.h.
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

#ifndef __UUNICODE__
#define __UUNICODE__

#include <K/Unicode/UnicodeDefinitions.h>

///
/// Classe pour convertir vers/depuis UTF-16.
///
/// Les encodages implémentés pour le moment sont:
/// - ISO-8859-1
/// - ISO-8859-2
/// - ASCII
/// - x-mac-roman
/// - UCS-4
/// - UTF-8
///
/// Format UTF-16:
/// - UTF-16 est, pour le Basic Multilingual Plan, comme UCS-2 (utilisé dans NewtonOS), i.e. un caractère
///   est représenté sur 16 bits. La plupart des ensembles de caractères gérés ici sont inclus dans le
///   BMP. Par conséquent, pour les encodages avec des caractères de taille fixe, les fonctions de
///   conversion peuvent être grandement simplifiées.
/// - UTF-16 est du grand indien. Ceci est géré par UByteSex si KDefinition.h a su deviner le sexe des
///   octets sur votre plateforme.
///
/// Utilisation de EToOpt:
/// Si un caractère dans une chaîne unicode ne peut pas être représenté dans l'encodage de destination,
/// deux comportements sont possibles:
/// - mettre le caractère de substitution (\c kSubstituteChar)
/// - arrêter la conversion.
/// EToOpt permet de décider quel comportement adopter.
///
/// Chaînes invalides en entrée:
/// Si une chaîne invalide est fournie en entrée (e.g. 0x80 dans une chaîne US ASCII), le comportement
/// n'est pas défini (il peut y avoir des exceptions, cf la documentation des méthodes de conversion).
/// Si la chaîne en entrée est dans un codage à taille de caractère variable (e.g. UTF, Shift, etc.)
/// et que le(s) dernier(s) octet(s)/mot(s) sont le début d'une suite de caractères plus longue (ou
/// d'une séquence d'échapement) la conversion ne se termine pas correctement. Ces derniers octets ne
/// sont pas marqués comme lus (ioInputCount ne les comptera pas) et le résultat sera kMiddleOfMultiChar.
/// Ceci signifie que la mémoire tampon doit pouvoir contenir tous les caractères multi-octets ou les
/// séquences d'échapement de la chaîne fournie en entrée.
///
/// \author	Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.6 $
///
/// \test	aucun test défini.
///
class UUTF16Conv
{
public:
	///
	/// Indique la politique à suivre lorsqu'un caractère non représentable est trouvé.
	///
	enum EToOpt {
		kRepCharOnUnrepChar,	///< Utilise le caractère de remplacement.
		kStopOnUnrepChar		///< Arrête la conversions.
	};

	///
	/// Résultat des méthodes de conversion
	///
	enum EResult {
		kInputExhausted = 0,	///< La conversion est terminée (tous les caractères en entrée ont été convertis).
								///< Ne signifie pas que la mémoire tampon de sortie est pleine.
		kOutputExhausted,		///< Il n'y a plus de place dans la mémoire tampon de sortie (et la chaîne en
								///< entrée n'a pas été entièrement convertie)
		kUnrepCharEncountered,	///< Un caractère non représentable a été trouvé (et l'option est \c kStopOnUnrepChar)
		kMiddleOfMultiChar		///< Les derniers octets de la mémoire tampon d'entrée/de sortie constituent le début
								///< d'un caractère ou d'une séquence d'échappement.
	};

	///
	/// Convertit une chaîne UTF-16 en ISO-8859-1
	///
	/// \param inInputBuffer	chaîne unicode à convertir.
	/// \param ioInputCount		en entrée: nombre de caractères (16 bits) dans la chaîne unicode.
	///							en sortie: nombre de caractères (16 bits) convertis dans cette chaîne.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne ISO-8859-1.
	/// \param ioOutputCount	en entrée: taille en octets/caractères de cette mémoire tampon.
	///							en sortie: nombre d'octets/caractères écrits dans cette mémoire tampon.
	/// \param inConvertToOpt	option pour la conversion. Cf le commentaire sur la classe.
	/// \return un code indiquant comment la conversion a pris fin. (Cf le commentaire sur la classe et
	///			sur EResult)
	///
	static EResult	ToISO88591(
		const KUInt16*	inInputBuffer,
		size_t*			ioInputCount,
		KUInt8*			outOutputBuffer,
		size_t*			ioOutputCount,
		EToOpt			inConvertToOpt
		);

	///
	/// Convertit une chaîne ISO-8859-1 en UTF-16
	///
	/// \param inInputBuffer	chaîne ISO-8859-1 à convertir.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne UTF-16.
	/// \param inCount			taille en octets (entrée) / mots (sortie) de chaque mémoire tampon.
	///
	static void		FromISO88591(
		const KUInt8*	inInputBuffer,
		KUInt16*		outOutputBuffer,
		size_t			inCount
		);

	///
	/// Convertit une chaîne UTF-16 en ISO-8859-2
	///
	/// \param inInputBuffer	chaîne unicode à convertir.
	/// \param ioInputCount		en entrée: nombre de caractères (16 bits) dans la chaîne unicode.
	///							en sortie: nombre de caractères (16 bits) convertis dans cette chaîne.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne ISO-8859-2.
	/// \param ioOutputCount	en entrée: taille en octets/caractères de cette mémoire tampon.
	///							en sortie: nombre d'octets/caractères écrits dans cette mémoire tampon.
	/// \param inConvertToOpt	option pour la conversion. Cf le commentaire sur la classe.
	/// \return un code indiquant comment la conversion a pris fin. (Cf le commentaire sur la classe et
	///			sur EResult)
	///
	static EResult	ToISO88592(
		const KUInt16*	inInputBuffer,
		size_t*			ioInputCount,
		KUInt8*			outOutputBuffer,
		size_t*			ioOutputCount,
		EToOpt			inConvertToOpt
		);

	///
	/// Convertit une chaîne ISO-8859-2 en UTF-16
	///
	/// \param inInputBuffer	chaîne ISO-8859-2 à convertir.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne UTF-16.
	/// \param inCount			taille en octets (entrée) / mots (sortie) de chaque mémoire tampon.
	///
	static void		FromISO88592(
		const KUInt8*	inInputBuffer,
		KUInt16*		outOutputBuffer,
		size_t			inCount
		);

	///
	/// Convertit une chaîne UTF-16 en US-ASCII
	///
	/// \param inInputBuffer	chaîne unicode à convertir.
	/// \param ioInputCount		en entrée: nombre de caractères (16 bits) dans la chaîne unicode.
	///							en sortie: nombre de caractères (16 bits) convertis dans cette chaîne.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne US-ASCII.
	/// \param ioOutputCount	en entrée: taille en octets/caractères de cette mémoire tampon.
	///							en sortie: nombre d'octets/caractères écrits dans cette mémoire tampon.
	/// \param inConvertToOpt	option pour la conversion. Cf le commentaire sur la classe.
	/// \return un code indiquant comment la conversion a pris fin. (Cf le commentaire sur la classe et
	///			sur EResult)
	///
	static EResult	ToASCII(
		const KUInt16*	inInputBuffer,
		size_t*			ioInputCount,
		KUInt8*			outOutputBuffer,
		size_t*			ioOutputCount,
		EToOpt			inConvertToOpt
		);

	///
	/// Convertit une chaîne US-ASCII en UTF-16
	///
	/// \param inInputBuffer	chaîne US-ASCII à convertir.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne UTF-16.
	/// \param inCount			taille en octets (entrée) / mots (sortie) de chaque mémoire tampon.
	///
	static void		FromASCII(
		const KUInt8*	inInputBuffer,
		KUInt16*			outOutputBuffer,
		size_t			inCount
		);

	///
	/// Convertit une chaîne UTF-16 en MacRoman.
	///
	/// \param inInputBuffer	chaîne unicode à convertir.
	/// \param ioInputCount		en entrée: nombre de caractères (16 bits) dans la chaîne unicode.
	///							en sortie: nombre de caractères (16 bits) convertis dans cette chaîne.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne MacRoman.
	/// \param ioOutputCount	en entrée: taille en octets/caractères de cette mémoire tampon.
	///							en sortie: nombre d'octets/caractères écrits dans cette mémoire tampon.
	/// \param inConvertToOpt	option pour la conversion. Cf le commentaire sur la classe.
	/// \return un code indiquant comment la conversion a pris fin. (Cf le commentaire sur la classe et
	///			sur EResult)
	///
	static EResult	ToMacRoman(
		const KUInt16*	inInputBuffer,
		size_t*			ioInputCount,
		KUInt8*			outOutputBuffer,
		size_t*			ioOutputCount,
		EToOpt			inConvertToOpt
		);

	///
	/// Convertit une chaîne MacRoman en UTF-16
	///
	/// \param inInputBuffer	chaîne US-ASCII à convertir.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne UTF-16.
	/// \param inCount			taille en octets (entrée) / mots (sortie) de chaque mémoire tampon.
	///
	static void		FromMacRoman(
		const KUInt8*	inInputBuffer,
		KUInt16*		outOutputBuffer,
		size_t			inCount
		);

	///
	/// Convertit une chaîne UTF-16 en UCS-4.
	///
	/// Si tous les caractères sont dans le BMP, cela revient à étendre les caractères de 16 à 32 bits.
	///
	/// \param inInputBuffer	chaîne UTF-16 à convertir.
	/// \param ioInputCount		en entrée: nombre de mots de 16 bits dans la chaîne UTF-16.
	///							en sortie: nombre de mots de 16 bits convertis dans cette chaîne.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne UCS-4.
	/// \param ioOutputCount	en entrée: taille en mots de 32 bits/caractères de cette mémoire tampon.
	///							en sortie: nombre de mots de 32 bits/caractères écrits dans cette mémoire tampon.
	/// \return un code indiquant comment la conversion a pris fin. (Cf le commentaire sur la classe et
	///			sur EResult)
	///
	static EResult	ToUCS4(
		const KUInt16*	inInputBuffer,
		size_t*			ioInputCount,
		KUInt32*		outOutputBuffer,
		size_t*			ioOutputCount
		);

	///
	/// Convertit une chaîne UCS-4 en UTF-16.
	///
	/// Si tous les caractères sont dans le BMP, cela revient à réduire les caractères de 32 à 16 bits.
	///
	/// \param inInputBuffer	chaîne UCS-4 à convertir.
	/// \param ioInputCount		en entrée: nombre de mots de 32 bits/caractères dans la chaîne UCS-4.
	///							en sortie: nombre de mots de 32 bits/caractères convertis dans cette chaîne.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne UTF-16.
	/// \param ioOutputCount	en entrée: taille en mots de 16 bits de cette mémoire tampon.
	///							en sortie: nombre de mots de 16 bits écrits dans cette mémoire tampon.
	/// \return un code indiquant comment la conversion a pris fin. (Cf le commentaire sur la classe et
	///			sur EResult)
	///
	static EResult	FromUCS4(
		const KUInt32*	inInputBuffer,
		size_t*			ioInputCount,
		KUInt16*		outOutputBuffer,
		size_t*			ioOutputCount
		);

	///
	/// Convertit une chaîne UTF-16 en UTF-8.
	///
	/// La chaîne UTF-8 fait au plus 3 fois la taille de la chaîne UTF-16 (en nombre de caractères).
	///
	/// \param inInputBuffer	chaîne UTF-16 à convertir.
	/// \param ioInputCount		en entrée: nombre de mots de 16 bits dans la chaîne UTF-16.
	///							en sortie: nombre de mots de 16 bits convertis dans cette chaîne.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne UTF-8.
	/// \param ioOutputCount	en entrée: taille en caractères (8 bits) de cette mémoire tampon.
	///							en sortie: nombre de caractères écrits dans cette mémoire tampon.
	/// \return un code indiquant comment la conversion a pris fin. (Cf le commentaire sur la classe et
	///			sur EResult)
	///
	static EResult	ToUTF8(
		const KUInt16*	inInputBuffer,
		size_t*			ioInputCount,
		void*			outOutputBuffer,
		size_t*			ioOutputCount
		);

	///
	/// Convertit une chaîne UTF-8 en UTF-16.
	///
	/// La chaîne UTF-16 fait au plus 1 fois la taille de la chaîne UTF-8 (en nombre de caractères).
	///
	/// \param inInputBuffer	chaîne UTF-8 à convertir.
	/// \param ioInputCount		en entrée: taille en caractères (8 bits) de la chaîne UTF-8.
	///							en sortie: nombre de caractères (8 bits) convertis de cette chaîne.
	/// \param outOutputBuffer	mémoire tampon pour la chaîne UTF-16.
	/// \param ioOutputCount	en entrée: taille en mots de 16 bits de cette mémoire tampon.
	///							en sortie: nombre de mots de 16 bits écrits dans cette mémoire tampon.
	/// \return un code indiquant comment la conversion a pris fin. (Cf le commentaire sur la classe et
	///			sur EResult)
	///
	static EResult	FromUTF8(
		const void*		inInputBuffer,
		size_t*			ioInputCount,
		KUInt16*		outOutputBuffer,
		size_t*			ioOutputCount
		);

	///
	/// Convertit un caractère ISO-8859-2 en UTF-16 via la table.
	///
	/// \param inISOChar	caractère ISO-8859-2
	/// \return un caractère UTF-16 équivalent.
	///
	inline static KUInt16	FromISO88592( const KUInt8 inISOChar )
		{
			if (inISOChar > 0xA0)
			{
				// 0x00A0 et toutes les valeurs inférieures sont les mêmes en UTF-16
				return kFromISO88592Table[inISOChar - 0xA0];
			} else {
				return (KUInt16) inISOChar;
			}
		}

	///
	/// Convertit un caractère MacRoman en UTF-16 via la table.
	///
	/// \param inMacRomanChar	caractère MacRoman
	/// \return un caractère UTF-16 équivalent.
	///
	inline static KUInt16	FromMacRoman( const KUInt8 inMacRomanChar )
		{
			if (inMacRomanChar >= 0x80)
			{
				// 0x0080 et toutes les valeurs inférieures sont les mêmes en UTF-16
				return kFromISO88592Table[inMacRomanChar - 0x80];
			} else {
				return (KUInt16) inMacRomanChar;
			}
		}

private:
	static const KUInt16	kToISO88592Table[58][2];	///< Table pour la conversion vers ISO-8859-2.
	static const KUInt16	kFromISO88592Table[0x60];	///< Table pour la conversion depuis ISO-8859-2.
	static const KUInt16	kToMacRomanTable[129][2];	///< Table pour la conversion vers MacRoman.
	static const KUInt16	kFromMacRomanTable[0x80];	///< Table pour la conversion depuis MacRoman.
#if !ARMCPP
	static const KUInt16 	kSubstituteChar;			///< Caractère de substitution.
#endif
};

#endif
		// __UUNICODE__

// ============================================================================ //
// Mac Airways:                                                                 //
// The cashiers, flight attendants and pilots all look the same, feel the same  //
// and act the same. When asked questions about the flight, they reply that you //
// don't want to know, don't need to know and would you please return to your   //
// seat and watch the movie.                                                    //
// ============================================================================ //
