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
/// Classe d'utilitaires pour g�rer les cha�nes au format pascal utilis� sous MacOS.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.4 $
///
/// \test	aucun test d�fini.
///
class UPStrings
{
public:
	///
	/// Conversion d'une cha�ne pascal en une cha�ne C.
	///
	/// \param outDestString	m�moire tampon pour mettre le r�sultat
	/// \param inSourceString	cha�ne Pascal � convertir
	/// \param inMaxLength		taille maximale de la cha�ne en caract�re (taille
	///							de outDestString moins 1)
	///
	static void		P2CStrCopy(
						char* outDestString,
						ConstStr255Param inSourceString,
						long inMaxLength = 255 );

	///
	/// Conversion d'une cha�ne C en une cha�ne pascal.
	///
	/// \param outDestString	m�moire tampon pour mettre le r�sultat
	/// \param inSourceString	cha�ne C � convertir
	/// \param inMaxLength		taille maximale de la cha�ne en caract�re (taille
	///							de outDestString moins 1)
	///
	static void		C2PStrCopy(
						Str255 outDestString,
						const char* inSourceString,
						long inMaxLength = 255 );

	///
	/// Conversion sur place d'une cha�ne pascal en une cha�ne C.
	/// La cha�ne pass�e en entr�e est modifi�e.
	///
	/// \param ioString			cha�ne Pascal � convertir
	/// \return un pointeur sur la cha�ne, d�sormais au format C.
	///
	static char*			P2CStr( Str255 ioString );

	///
	/// Conversion sur place d'une cha�ne C en une cha�ne pascal.
	/// La cha�ne pass�e en entr�e est modifi�e.
	///
	/// \param ioString			cha�ne C � convertir
	/// \return un pointeur sur la cha�ne, d�sormais au format pascal.
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
