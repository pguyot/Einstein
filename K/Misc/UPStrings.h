// ==============================
// Fichier:			UPStrings.h
// Projet:			K
//
// Créé le:			22/1/2001
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
// The Original Code is UPStrings.h.
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

#ifndef _UPSTRINGS_H
#define _UPSTRINGS_H

#include <K/Defines/KDefinitions.h>

///
/// Classe d'utilitaires pour gérer les chaînes au format pascal utilisé sous MacOS.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.4 $
///
/// \test	aucun test défini.
///
class UPStrings
{
public:
	///
	/// Conversion d'une chaîne pascal en une chaîne C.
	///
	/// \param outDestString	mémoire tampon pour mettre le résultat
	/// \param inSourceString	chaîne Pascal à convertir
	/// \param inMaxLength		taille maximale de la chaîne en caractère (taille
	///							de outDestString moins 1)
	///
	static void		P2CStrCopy(
						char* outDestString,
						ConstStr255Param inSourceString,
						long inMaxLength = 255 );

	///
	/// Conversion d'une chaîne C en une chaîne pascal.
	///
	/// \param outDestString	mémoire tampon pour mettre le résultat
	/// \param inSourceString	chaîne C à convertir
	/// \param inMaxLength		taille maximale de la chaîne en caractère (taille
	///							de outDestString moins 1)
	///
	static void		C2PStrCopy(
						Str255 outDestString,
						const char* inSourceString,
						long inMaxLength = 255 );

	///
	/// Conversion sur place d'une chaîne pascal en une chaîne C.
	/// La chaîne passée en entrée est modifiée.
	///
	/// \param ioString			chaîne Pascal à convertir
	/// \return un pointeur sur la chaîne, désormais au format C.
	///
	static char*			P2CStr( Str255 ioString );

	///
	/// Conversion sur place d'une chaîne C en une chaîne pascal.
	/// La chaîne passée en entrée est modifiée.
	///
	/// \param ioString			chaîne C à convertir
	/// \return un pointeur sur la chaîne, désormais au format pascal.
	///
	static unsigned char*	C2PStr( char* ioString );
};

#endif
		// __UPSTRINGS__

// =========================== //
// /* Halley */                //
//                             //
//         (Halley's comment.) //
// =========================== //