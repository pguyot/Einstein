// ==============================
// Fichier:			TCondVar.h
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
// The Original Code is TCondVar.h.
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
// $Id: TCondVar.h,v 1.2 2007/07/11 01:36:30 pguyot Exp $
// ===========

#ifndef _TCONDVAR_H
#define _TCONDVAR_H

#include <K/Defines/KDefinitions.h>

// ANSI C & POSIX
#if TARGET_OS_WIN32
	#include <windows.h>
	#include <process.h>
	typedef struct {
	  int waiters_count_;
	  CRITICAL_SECTION waiters_count_lock_;
	  HANDLE sema_;
	  HANDLE waiters_done_;
	  size_t was_broadcast_;
	} win_pthread_cond_t;
	#if _MSC_VER
//		typedef struct timespec {
//			long tv_sec;  // seconds
//			long tv_nsec; // nanoseconds
//		} timespec;
	#endif
	#ifndef ETIMEDOUT
		#define ETIMEDOUT 60
	#endif
#else
	#include <pthread.h>
#endif

class TMutex;

///
/// Class for a condition variable.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.2 $
///
/// \test	aucun test défini.
///
class TCondVar
{
public:
	///
	/// Default constructor.
	///
	TCondVar( void );

	///
	/// Destructor.
	///
	~TCondVar( void );

	///
	/// Signal the conditional variable.
	///
	void Signal( void );

	///
	/// Broadcast the conditional variable, i.e. unblock every thread.
	///
	void Broadcast( void );

	///
	/// Wait for a limited time for the conditional variable to be freed.
	///
	/// \param inMutex		mutex to unlock.
	/// \param inAbsTime	time to wait (wait until the system time reached
	///						this time).
	/// \return \c true if the system time didn't reach inAbsTime, \c false
	///			otherwise.
	///
	bool TimedWait( TMutex* inMutex, const struct timespec* inAbsTime );

	///
	/// Wait for a limited time for the conditional variable to be freed.
	///
	/// \param inMutex			mutex to unlock.
	/// \param inRelTime	time to wait (amount).
	/// \return \c true if the system time didn't reach now+inRelTime, \c false
	///			otherwise.
	///
	bool TimedWaitRelative(
						TMutex* inMutex,
						const struct timespec* inAbsTime );

	///
	/// Wait for the conditional variable to be freed.
	///
	/// \param inMutex		mutex to unlock.
	///
	void Wait( TMutex* inMutex );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TCondVar( const TCondVar& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TCondVar& operator = ( const TCondVar& inCopy );

	/// \name Variables
#if TARGET_OS_WIN32
	win_pthread_cond_t	mCondVar;	///< Condition variable.
#else
	pthread_cond_t		mCondVar;	///< Condition variable.
#endif
};

#endif
		// _TCONDVAR_H

// =========================================================================== //
// "But what we need to know is, do people want nasally-insertable computers?" //
// =========================================================================== //
