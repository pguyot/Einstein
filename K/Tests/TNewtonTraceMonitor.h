// ==============================
// Fichier:			TNewtonTraceMonitor.h
// Projet:			K
//
// Créé le:			08/09/2002
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
// The Original Code is TNewtonTraceMonitor.h.
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

#ifndef __TNEWTONTRACEMONITOR__
#define __TNEWTONTRACEMONITOR__

#include <K/Defines/KDefinitions.h>
#include <K/Tests/TTraceMonitor.h>
#include <K/Misc/TNameServerSingleton.h>

///
/// Classe pour afficher la trace sur NewtonOS.
/// Le singleton est enregistré auprès du serveur de nom.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version 1.0
///
/// \test	aucun test défini.
///
class TNewtonTraceMonitor
				:
					public TTraceMonitor,
					public TNameServerSingleton
{
public:
	///
	/// Récupère le singleton. Le crée si nécessaire.
	///
	/// \param	inMode	mode à utiliser. Les bits sont définis dans EMode.
	///
	static TNewtonTraceMonitor* GetTraceMonitor( unsigned int inMode );

	///
	/// Supprime le singleton s'il existe.
	///
	static void RemoveTraceMonitor( void );

private:
	///
	/// Constructeur à partir d'un entier représentant le mode.
	///
	/// \param	inMode	mode à utiliser. Les bits sont définis dans EMode.
	///
	TNewtonTraceMonitor( unsigned int inMode );

	///
	/// Destructeur.
	///
	virtual ~TNewtonTraceMonitor( void );
	
	static const char* const	kName;			///< constante pour le serveur de noms
};

#endif
		// __TNEWTONTRACEMONITOR__

// =============================================================================== //
// Real Programmers don't write in FORTRAN.  FORTRAN is for pipe stress freaks and //
// crystallography weenies.  FORTRAN is for wimp engineers who wear white socks.   //
// =============================================================================== //