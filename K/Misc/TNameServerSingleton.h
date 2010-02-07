// ==============================
// Fichier:			TNameServerSingleton.h
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
// The Original Code is TNameServerSingleton.h.
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

#ifndef __TNAMESERVERSINGLETON__
#define __TNAMESERVERSINGLETON__

#include <K/Defines/KDefinitions.h>

// NewtonOS
#include <Newton.h>

///
/// Classe pour un singleton enregistré auprès du serveur de nom.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.5 $
///
/// \test	aucun test défini.
///
class TNameServerSingleton
{
protected:
	// MrCpp a besoin que cette constante soit définie avant.
	static const char* const kType;		///< Chaîne commune pour le type.

public:
	///
	/// Supprime le singleton et l'entrée dans le serveur de nom.
	///
	/// \param	inName		nom de l'objet (première chaîne dans
	///						le serveur de nom)
	/// \param	inType		type de l'objet (deuxième chaîne dans
	///						le serveur de nom)
	/// \return un code d'erreur si un problème est survenu:
	///			- kError_Not_Registered si le singleton n'existe pas.
	///
	static NewtonErr				DeleteObject(
										const char* inName,
										const char* inType = kType );

protected:
	///
	/// Constructeur par défaut.
	///
	/// Remarque importante: si le code est copié, il faut reloger la
	/// table des fonctions virtuelles. Sinon, gare à la casse.
	///
	TNameServerSingleton() {};

	///
	/// Destructeur.
	///
	virtual ~TNameServerSingleton() {};
	
	///
	/// Récupère le singleton. Retourne \c nil s'il n'existe pas.
	///
	/// \param	inName		nom de l'objet (première chaîne dans
	///						le serveur de nom)
	/// \param	inType		type de l'objet (deuxième chaîne dans
	///						le serveur de nom)
	/// \return \c nil si le serveur de nom a retourné une erreur
	///			(ce qui signifie généralement que le singleton
	///			n'existe pas).
	///
	static TNameServerSingleton*	GetObject(
										const char* inName,
										const char* inType = kType );

	///
	/// Enregistre le singleton dans le serveur de nom.
	///
	/// \param	inObject	objet à enregistrer
	/// \param	inName		nom de l'objet (première chaîne dans
	///						le serveur de nom)
	/// \param	inType		type de l'objet (deuxième chaîne dans
	///						le serveur de nom)
	/// \return un code d'erreur si un problème est survenu:
	///			- kError_Already_Registered si le singleton existe déjà.
	///
	static NewtonErr				CreateObject(
										TNameServerSingleton* inObject,
										const char* inName,
										const char* inType = kType );
};

#endif
		// __TNAMESERVERSINGLETON__

// =================================================================== //
// The bugs you have to avoid are the ones that give the user not only //
// the inclination to get on a plane, but also the time.               //
//                 -- Kay Bostic                                       //
// =================================================================== //