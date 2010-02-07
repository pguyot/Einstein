// ==============================
// Fichier:			TMonitorMethodClient.h
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
// The Original Code is TMonitorMethodClient.h.
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

#ifndef __TMONITORMETHODCLIENT__
#define __TMONITORMETHODCLIENT__

#include <K/Defines/KDefinitions.h>

class TFunctionMonitor;

///
/// Classe pour noter le début et la fin d'une méthode.
/// S'utilise normalement avec des macros, par exemple:
///
///	MONITOR_METHOD;
/// ou
/// MONITOR_FUNCTION;
/// ou encore
/// MONITOR_CONSTRUCTOR;
///
/// La première macro prend l'objet monitor dans un des champs
/// de l'objet, la seconde dans une variable globale (ou via
/// une fonction). La troisième récupère/crée l'objet et le
/// stocke dans un champ (pour utiliser la première macro).
/// Etc.
///
/// L'objet moniteur est prévenu de la fin de la fonction/méthode
/// lorsque l'objet TMonitorMethodClient est détruit (i.e. lors
/// de la fin normale ou pas de la méthode).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TMonitorMethodClient
{
public:
	///
	/// Constructeur à partir d'un nom de fichier et d'un numéro de ligne.
	/// Le pointeur sur le nom du fichier doit être le même à chaque appel
	/// pour le profilage.
	///
	/// \param	inMonitor		objet prévenu du début et de la fin de la
	///							méthode
	/// \param	inFileName		nom du fichier
	/// \param	inLineNumber	numéro de ligne
	///
	TMonitorMethodClient(
					TFunctionMonitor* inMonitor,
					const char* inFileName,
					unsigned int inLineNumber );

	///
	/// Destructeur.
	///
	~TMonitorMethodClient( void );

private:
	TFunctionMonitor*	mMonitor;		///< objet moniteur, prévenu du
										///< début et de la fin
	const char*			mFileName;		///< nom du fichier
	const unsigned int	mLineNumber;	///< numéro de ligne
};

#endif
		// __TMONITORMETHODCLIENT__

// ======================================================================= //
// Around computers it is difficult to find the correct unit of time to    //
// measure progress.  Some cathedrals took a century to complete.  Can you //
// imagine the grandeur and scope of a program that would take as long?    //
//                 -- Epigrams in Programming, ACM SIGPLAN Sept. 1982      //
// ======================================================================= //