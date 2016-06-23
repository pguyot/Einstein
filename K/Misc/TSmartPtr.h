// ==============================
// Fichier:			TSmartPtr.h
// Projet:			K
//
// Créé le:			4/2/2003
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
// The Original Code is TSmartPtr.h.
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
// $Id: TSmartPtr.h,v 1.5 2005/02/24 06:49:58 pguyot Exp $
// ===========

#ifndef _TSMARTPTR_H
#define _TSMARTPTR_H

#include <K/Defines/KDefinitions.h>

// 	ISO C++
#include <new>
#include <cctype>

///
/// Patron pour libérer une zone de mémoire automatiquement.
/// Permet d'éviter la duplication de code et les try/catch qui
/// ne servent qu'à faire le ménage.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.5 $
///
/// \test	aucun test défini.
///
template< class TClass >
class TSmartPtr
{
public:
	///
	/// Constructeur par défaut.
	///
	TSmartPtr( void );

	///
	/// Constructeur à partir d'un pointeur.
	///
	TSmartPtr( TClass* inObject );

	///
	/// Destructeur.
	///
	~TSmartPtr( void );

	///
	/// Supprime l'objet.
	///
	void Delete( void );

	///
	/// Déréférencement.
	///
	/// \return l'objet.
	///
	inline const TClass& operator * ( void ) const
		{
			return *mObject;
		}
	inline TClass& operator * ( void )
		{
			return *mObject;
		}
	inline const TClass* operator -> ( void ) const
		{
			return mObject;
		}
	inline TClass* operator -> ( void )
		{
			return mObject;
		}

	///
	/// Accesseur.
	///
	/// \return la référence sur l'objet.
	///
	inline const TClass* Get( void ) const
		{
			return mObject;
		}
	TClass* Get( void )
		{
			return mObject;
		}

	///
	/// Assignation à une variable.
	/// Supprime l'élément s'il n'est pas nul.
	///
	/// \param inObject		objet à récupérer
	///
	TSmartPtr& operator = ( TClass* inObject );

private:
	///
	/// Opérateurs new volontairement indisponibles.
	///
	void*	operator new ( size_t inSize );
	void*	operator new[] ( size_t inSize );

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TSmartPtr( const TSmartPtr& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TSmartPtr& operator = ( const TSmartPtr& inCopy );

	/// \name Variables
	TClass*	mObject;
};

// --------------------------------------------------------------------------------	//
//  * TSmartPtr( void )
// --------------------------------------------------------------------------------	//
template< class TClass >
inline
TSmartPtr<TClass>::TSmartPtr( void )
	:
		mObject( nil )
{
}

// --------------------------------------------------------------------------------	//
//  * TSmartPtr( TClass* inObject )
// --------------------------------------------------------------------------------	//
template< class TClass >
inline
TSmartPtr<TClass>::TSmartPtr( TClass* inObject )
	:
		mObject( inObject )
{
}

// --------------------------------------------------------------------------------	//
//  * ~TSmartPtr( void )
// --------------------------------------------------------------------------------	//
template< class TClass >
inline
TSmartPtr<TClass>::~TSmartPtr( void )
{
	Delete();
}

// --------------------------------------------------------------------------------	//
//  * Delete( void )
// --------------------------------------------------------------------------------	//
template< class TClass >
inline
void
TSmartPtr<TClass>::Delete( void )
{
	if (mObject)
	{
		TClass* theObject = mObject;
		mObject = nil;
		delete theObject;
	}
}

// --------------------------------------------------------------------------------	//
//  * operator = ( TClass* )
// --------------------------------------------------------------------------------	//
template< class TClass >
inline
TSmartPtr<TClass>&
TSmartPtr<TClass>::operator = ( TClass* inObject )
{
	Delete();
	mObject = inObject;
	
	return *this;
}

#endif
		// _TSMARTPTR_H

// ========================================================================== //
// If a group of _N persons implements a COBOL compiler, there will be _N-1 //
// passes.  Someone in the group has to be the manager.                       //
//                 -- T. Cheatham                                             //
// ========================================================================== //
