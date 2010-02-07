// ==============================
// Fichier:			TFunctionMonitor.h
// Projet:			K
//
// Créé le:			05/09/2002
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
// The Original Code is TFunctionMonitor.h.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2002-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========

#ifndef __TFUNCTIONMONITOR__
#define __TFUNCTIONMONITOR__

#include <K/Defines/KDefinitions.h>

///
/// Classe abstraite pour un moniteur de fonctions.
/// Utilisée avec TFunctionMonitor.
/// Peut servir pour le profilage ou pour tracer les méthodes.
/// Ou encore pour obtenir une trace de la pile.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
/// \see	TFunctionMonitor
///
class TFunctionMonitor
{
public:
	///
	/// Constructeur par défaut.
	///
	TFunctionMonitor( void ) {};

	///
	/// Destructeur.
	///
	virtual ~TFunctionMonitor( void ) {};

	///
	/// Début d'une fonction.
	/// Cette méthode est appelée par le constructeur de TMethodMonitor.
	/// inFileName n'est pas copié.
	///
	/// \param inFileName		nom du fichier. A priori, ce pointeur
	///							reste le même entre les appels.
	/// \param inLineNumber		numéro de la ligne.
	///
	virtual void FunctionBegin(
						const char* inFileName,
						unsigned int inLineNumber ) = 0;

	///
	/// Fin d'une fonction.
	/// Cette méthode est appelée par le destructeur de TMethodMonitor.
	/// inFileName n'est pas copié.
	///
	/// \param inFileName		nom du fichier. A priori, ce pointeur
	///							reste le même entre les appels.
	/// \param inLineNumber		numéro de la ligne.
	///
	virtual void FunctionEnd(
						const char* inFileName,
						unsigned int inLineNumber ) = 0;
};

#endif
		// __TFUNCTIONMONITOR__

// ======================================================================== //
// Real Programmers don't write in PL/I.  PL/I is for programmers who can't //
// decide whether to write in COBOL or FORTRAN.                             //
// ======================================================================== //