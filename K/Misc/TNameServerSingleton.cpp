// ==============================
// Fichier:			TNameServerSingleton.cp
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
// The Original Code is TNameServerSingleton.cp.
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

#include <K/Defines/KDefinitions.h>
#include <K/Misc/TNameServerSingleton.h>

// NewtonOS
#include <NameServer.h>

// ------------------------------------------------------------------------- //
//  * kType
// ------------------------------------------------------------------------- //
const char* const TNameServerSingleton::kType = "Kallisys";

// ------------------------------------------------------------------------- //
//  * GetObject( const char*, const char* )
// ------------------------------------------------------------------------- //
TNameServerSingleton*
TNameServerSingleton::GetObject(
								const char* inName,
								const char* inType /* = kType */ )
{
	// Création d'un client pour le serveur de nom.
	TUNameServer theNSClient;

	// On se moque de la version.	
	ULong theVersion;
	TNameServerSingleton* theResult = nil;
	
	(void) theNSClient.Lookup( (char*) inName, (char*) inType,
								(ULong*) &theResult, &theVersion );

	return theResult;
}

// ------------------------------------------------------------------------- //
//  * CreateObject( TNameServerSingleton*, const char*, const char* )
// ------------------------------------------------------------------------- //
NewtonErr
TNameServerSingleton::CreateObject(
								TNameServerSingleton* inObject,
								const char* inName,
								const char* inType /* = kType */ )
{
	// Création d'un client pour le serveur de nom.
	TUNameServer theNSClient;

	// Enregistrement avec 0 pour la version.
	return theNSClient.RegisterName( (char*) inName, (char*) inType,
								(ULong) inObject, 0 );	
}

// ------------------------------------------------------------------------- //
//  * DeleteObject( const char*, const char* )
// ------------------------------------------------------------------------- //
NewtonErr
TNameServerSingleton::DeleteObject(
								const char* inName,
								const char* inType /* = kType */ )
{
	// Création d'un client pour le serveur de nom.
	TUNameServer theNSClient;

	// On se moque de la version.	
	ULong theVersion;
	TNameServerSingleton* theObject = nil;
	
	NewtonErr theErr = theNSClient.Lookup( (char*) inName, (char*) inType,
								(ULong*) &theObject, &theVersion );

	// Si tout va bien, suppression de l'objet et de l'entrée dans le serveur
	// de nom.
	if (theErr == noErr)
	{
		// Suppression du serveur de nom.
		theErr = theNSClient.UnRegisterName( (char*) inName, (char*) inType );
		
		// Suppression de l'objet.
		delete theObject;
	}
	
	return theErr;
}

// ======================================= //
// Building translators is good clean fun. //
//                 -- T. Cheatham          //
// ======================================= //