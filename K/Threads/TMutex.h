// ==============================
// Fichier:			TMutex.h
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			26/5/2005
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
// The Original Code is TMutex.h.
// 
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2005 the
// Initial Developer. All Rights Reserved.
// 
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
// 
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TMutex.h,v 1.2 2007/07/11 01:36:30 pguyot Exp $
// ===========

#ifndef _TMUTEX_H
#define _TMUTEX_H

#include <K/Defines/KDefinitions.h>

// ANSI C & POSIX
#if TARGET_OS_WIN32
	#include <windows.h>
#else
	#include <pthread.h>
#endif

class TCondVar;

///
/// Class for a mutex.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.2 $
///
/// \test	aucun test défini.
///
class TMutex
{
public:
	friend class TCondVar;

	///
	/// Default constructor.
	///
	TMutex( void );

	///
	/// Destructor.
	///
	~TMutex( void );

	///
	/// Lock the mutex.
	///
	void Lock( void );

	///
	/// Unlock the mutex.
	///
	void Unlock( void );

	///
	/// Try to lock the mutex.
	///
	/// \return \c true if the mutex was locked.
	///
	Boolean TryLock( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMutex( const TMutex& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMutex& operator = ( const TMutex& inCopy );

	/// \name Variables
#if TARGET_OS_WIN32
	HANDLE				mMutex;		///< Mutex
#else
	pthread_mutex_t		mMutex;		///< Mutex.
#endif
};

#endif
		// _TMUTEX_H

// ======================================================================= //
// Trying to be happy is like trying to build a machine for which the only //
// specification is that it should run noiselessly.                        //
// ======================================================================= //
