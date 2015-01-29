// ==============================
// Fichier:			TRefCounter.h
// Projet:			K
//
// Cr�� le:			10/1/2003
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
// The Original Code is TRefCounter.h.
//
// The Initial Developers of the Original Code are Paul Guyot, Michael Vac�k
// and Nicolas Zinovieff. Portions created by the Initial Developers are
// Copyright (C) 2003-2004 the Initial Developers. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//   Michael Vac�k <mici@metastasis.net> (original author)
//   Nicolas Zinovieff <krugazor@poulet.org> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TRefCounter.h,v 1.3 2004/11/23 15:11:31 paul Exp $
// ===========

#ifndef _TREFCOUNTER_H
#define _TREFCOUNTER_H

#include <K/Defines/KDefinitions.h>

///
/// Classe pour un compteur de r�f�rence.
///
/// Les objets de cette classe ont plusieurs pointeurs sur eux
/// (ou bien ces pointeurs sont strictement contr�l�s au sein
/// d'une classe, ou bien ce sont des objets sp�cifiques qui
/// appellent les m�thodes \c IncrementRef et \c DecrementRef).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	aucun test d�fini.
///
class TRefCounter
{
public:
	///
	/// Constructeur par d�faut.
	///
	TRefCounter( void );

	///
	/// Constructeur par copie.
	/// Ne copie pas le compteur de r�f�rences.
	///
	/// \param inCopy		objet � copier
	///
	TRefCounter( const TRefCounter& inCopy );

	///
	/// Destructeur.
	///
	virtual ~TRefCounter( void );

protected:
	/// \name Compteur de r�f�rence

	///
	/// Indique que l'objet est sur la pile.
	/// Cette m�thode ne fait rien si la r�f�rence n'est pas nulle.
	///
	void IsOnStack( void );

	///
	/// Incr�mente la r�f�rence.
	///
	void IncrementRef( void );

	///
	/// D�cr�mente la r�f�rence et retourne true si elle est � z�ro
	/// et si la r�f�rence n'est pas sur la pile (i.e. s'il faut supprimer
	/// l'objet)
	///
	/// \return \c true si on doit supprimer l'objet,
	///			\c false sinon.
	///
	Boolean DecrementRef( void );

private:
	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TRefCounter& operator = ( const TRefCounter& inCopy );

	Boolean		mIsNotOnStack;	///< Si l'objet est sur la pile.
	KSInt32		mRefCount;		///< Compteur de r�f�rences.
};

#endif
		// _TREFCOUNTER_H

// =============================================================== //
// If it's worth hacking on well, it's worth hacking on for money. //
// =============================================================== //
