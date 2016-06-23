// ==============================
// Fichier:			UBase64.h
// Projet:			K
//
// Créé le:			28/01/2003
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
// The Original Code is UBase64.h.
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

#ifndef __UBASE64__
#define __UBASE64__

#include <K/Defines/KDefinitions.h>

///
/// Classe pour l'encodage et le décodage en Base64.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	UBase64Test
///
class UBase64
{
public:
	///
	/// Encode un binaire en Base64.
	///
	/// La taille du flux Base64 est toujours
	/// ((taille + 2) / 3) * 4 (en calcul entier)
	///
	/// \param inBinary			binaire à encoder
	/// \param outBase64Data	en sortie, représentation du binaire en Base64
	/// \param inDataSize		taille du binaire à encoder
	///
	static void Encode(
					const void* inBinary,
					KUInt8* outBase64Data,
					KUInt32 inDataSize );

	///
	/// Décode un binaire en Base64.
	///
	/// La taille du flux binaire est variable (dépend
	/// des = à la fin du texte et des espaces, tabulations
	/// et fins de lignes qui sont ignorées ici).
	/// Mais c'est au plus:
	/// ((tailleB64 + 3) / 4) * 3 (en calcul entier).
	/// La mémoire tampon de sortie doit donc avoir
	/// au moins cette taille (sinon, gare aux plantages).
	///
	/// \param inBase64Data		données Base64 à décoder
	/// \param outBinary		mémoire tampon pour le binaire décodé
	/// \param inBase64Size		taille des données Base64 à décoder
	/// \param outBinarySize	en sortie, taille du binaire décodé
	///
	static void Decode(
					const KUInt8* inBase64Data,
					void* outBinary,
					KUInt32 inBase64Size,
					KUInt32* outBinarySize );

private:
	/// Alphabet Base64.
	static const KUInt8	kAlphabet[64];
};

#endif
		// __UBASE64__

// ================================== //
// The steady state of disks is full. //
//                 -- Ken Thompson    //
// ================================== //
