// ==============================
// Fichier:			UPStrings.cp
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
// The Original Code is UPStrings.cp.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2001-2004,2003
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Misc/UPStrings.h>

// Routines de conversion de chaînes Pascal <-> C

#ifdef __MWERKS__
	#pragma	warn_resultnotused	off
	#pragma	warn_implicitconv	off
	#pragma	warn_extracomma		off
#endif

#if TARGET_OS_MACOS
	#include <Types.h>
	#include <Memory.h>
#elif TARGET_OS_OPENSTEP
	#include <CoreServices/CoreServices.h>
#endif

#ifdef __MWERKS__
	#pragma	warn_resultnotused	on
	#pragma	warn_implicitconv	reset
	#pragma	warn_extracomma		reset
#endif

// -------------------------------------------------------------------------- //
//  * P2CStrCopy( char*, ConstStr255Param, long )
// -------------------------------------------------------------------------- //
void
UPStrings::P2CStrCopy(
				char* outDestString,
				ConstStr255Param inSourceString,
				long inMaxLength )
{
	// Taille de la chaîne
	register long theLength = inSourceString[0];

	// On tronque si nécessaire.
	if (theLength > inMaxLength)
	{
		theLength = inMaxLength;
	}

	// Copie des caractères.
	::BlockMove( &inSourceString[1], outDestString, theLength );

	// Ajout du terminateur.
	outDestString[theLength] = '\0';
}

// -------------------------------------------------------------------------- //
//  * C2PStrCopy( Str255, const char*, long )
// -------------------------------------------------------------------------- //
void
UPStrings::C2PStrCopy(
				Str255 outDestString,
				const char* inSourceString,
				long inMaxLength )
{
	// On mesure la taille en même temps qu'on copie.

	// Taille de la chaîne
	register long theLength = 0;

	// Curseur sur la chaîne.
	register const char* theSourceString = inSourceString;
	do {
		register char theChar = *theSourceString;
		if (theChar == '\0')
		{
			break;
		}

		theSourceString++;
		theLength++;
		outDestString[theLength] = (unsigned char) theChar;
	} while (theLength <= inMaxLength);

	// Dépassement?
	if (theLength > inMaxLength )
	{
		theLength = inMaxLength;
	}

	// On écrit la taille.
	outDestString[0] = (unsigned char) theLength;
}

// -------------------------------------------------------------------------- //
//  * P2CStr( Str255 )
// -------------------------------------------------------------------------- //
char*
UPStrings::P2CStr( Str255 ioString )
{
	// Taille de la chaîne
	register long theLength = ioString[0];

	// Déplacement des caractères.
	// BlockMove sait gérer les zones de mémoire qui se recouvrent.
	::BlockMove( &ioString[1], ioString, theLength );

	// Ajout du terminateur.
	ioString[theLength] = '\0';

	return (char*) ioString;
}

// -------------------------------------------------------------------------- //
//  * P2CStr( Str255 )
// -------------------------------------------------------------------------- //
unsigned char*
UPStrings::C2PStr( char* ioString )
{
	// Taille de la chaîne
	register long theLength = 0;

	// Caractère précédent.
	register char thePreviousChar = '\0';

	// Curseur sur la chaîne.
	register char* theString = ioString;
	do {
		// Lecture du caractère courant.
		register char theChar = *theString;

		// On écrit le caractère précédent.
		*theString = thePreviousChar;

		// Si c'est la fin, on sort.
		if (theChar == '\0')
		{
			break;
		}

		// Au suivant.
		theString++;
		theLength++;
		thePreviousChar = theChar;
	} while (theLength < 256);

	// Détection du dépassement.
	if (theLength == 256)
	{
		theLength = 255;
	}

	// Ecriture de la taille.
	ioString[0] = (char) theLength;

	return (unsigned char*) ioString;
}

// ================================= //
// Disc space -- the final frontier! //
// ================================= //
