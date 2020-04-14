// ==============================
// Fichier:			TCondVar.cp
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// CrŽŽ le:			26/5/2005
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
// The Original Code is TCondVar.cp.
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
// $Id: TCondVar.cp,v 1.3 2007/07/11 01:36:30 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TCondVar.h"

// ANSI C & POSIX
#include <assert.h>
#include <sys/types.h>
#if TARGET_OS_WIN32
	#include <ctime>
	#include <sys/types.h>
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
	#include <unistd.h>
#endif
#include <errno.h>

// K
#include <K/Threads/TMutex.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// pthread conditions in WIN32 native code
// -------------------------------------------------------------------------- //

#if TARGET_OS_WIN32

// based on code by Douglas C. Schmidt and Irfan Pyarali
// http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
// A great alternative is here:
// http://sourceware.org/pthreads-win32/

int pthread_cond_init(win_pthread_cond_t *cv, const void*)
{
	cv->waiters_count_ = 0;
	cv->was_broadcast_ = 0;
	cv->sema_ = CreateSemaphore (NULL, 0, 0x7fffffff, NULL);
	InitializeCriticalSection (&cv->waiters_count_lock_);
	cv->waiters_done_ = CreateEvent (NULL, FALSE, FALSE, NULL);
	return 0; // FIXME error handling is missing!
}

int pthread_cond_destroy(win_pthread_cond_t *cv)
{
	CloseHandle(cv->waiters_done_);
	DeleteCriticalSection(&cv->waiters_count_lock_);
	CloseHandle(cv->sema_);
	return 0; // FIXME error handling is missing!
}

int pthread_cond_wait(win_pthread_cond_t *cv, HANDLE *external_mutex)
{
	EnterCriticalSection (&cv->waiters_count_lock_);
	cv->waiters_count_++;
	LeaveCriticalSection (&cv->waiters_count_lock_);
	SignalObjectAndWait (*external_mutex, cv->sema_, INFINITE, FALSE);
	EnterCriticalSection (&cv->waiters_count_lock_);
	cv->waiters_count_--;
	int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;
	LeaveCriticalSection (&cv->waiters_count_lock_);
	if (last_waiter)
		SignalObjectAndWait (cv->waiters_done_, *external_mutex, INFINITE, FALSE);
	else
		WaitForSingleObject (*external_mutex, INFINITE);
	return 0; // FIXME error handling is missing!
}

int pthread_cond_timedwait_relative_np(win_pthread_cond_t *cv, HANDLE *external_mutex, const struct timespec *trel)
{
	int ret = 0;
	DWORD msTime = trel->tv_sec*1000 + trel->tv_nsec/1000000;
	EnterCriticalSection (&cv->waiters_count_lock_);
	cv->waiters_count_++;
	LeaveCriticalSection (&cv->waiters_count_lock_);
	DWORD sig = SignalObjectAndWait (*external_mutex, cv->sema_, msTime, FALSE);
	if (sig==WAIT_TIMEOUT)
		ret = ETIMEDOUT;
	EnterCriticalSection (&cv->waiters_count_lock_);
	cv->waiters_count_--;
	int last_waiter = cv->was_broadcast_ && cv->waiters_count_ == 0;
	LeaveCriticalSection (&cv->waiters_count_lock_);
	if (last_waiter)
		SignalObjectAndWait (cv->waiters_done_, *external_mutex, INFINITE, FALSE);
	else
		WaitForSingleObject (*external_mutex, INFINITE);
	return ret;
}

int pthread_cond_timedwait(win_pthread_cond_t *cv, HANDLE *external_mutex, const struct timespec *tabs)
{
#if _MSC_VER
	struct _timeb msTime;
	_ftime(&msTime);
	unsigned __int64 t_now = msTime.time*1000 + msTime.millitm;
#else
	struct timeval msTime;
	gettimeofday( &msTime, NULL );
	unsigned __int64 t_now = msTime.tv_sec*1000 + msTime.tv_usec/1000;
#endif
	unsigned __int64 t_abs = tabs->tv_sec*1000 + tabs->tv_nsec/1000000;
	if (t_now>=t_abs)
		return ETIMEDOUT;
	unsigned __int64 t_rel = t_abs-t_now;
	struct timespec trel;
	trel.tv_sec = t_rel/1000;
	trel.tv_nsec = (t_rel%1000)*1000000;
	return pthread_cond_timedwait_relative_np(cv, external_mutex, &trel);
}

int pthread_cond_signal (win_pthread_cond_t *cv)
{
	EnterCriticalSection (&cv->waiters_count_lock_);
	int have_waiters = cv->waiters_count_ > 0;
	LeaveCriticalSection (&cv->waiters_count_lock_);
	if (have_waiters)
		ReleaseSemaphore (cv->sema_, 1, 0);
	return 0; // FIXME error handling is missing!
}

int pthread_cond_broadcast(win_pthread_cond_t *cv)
{
	EnterCriticalSection (&cv->waiters_count_lock_);
	int have_waiters = 0;
	if (cv->waiters_count_ > 0) {
		cv->was_broadcast_ = 1;
		have_waiters = 1;
	}
	if (have_waiters) {
		ReleaseSemaphore (cv->sema_, cv->waiters_count_, 0);
		LeaveCriticalSection (&cv->waiters_count_lock_);
		WaitForSingleObject (cv->waiters_done_, INFINITE);
		cv->was_broadcast_ = 0;
	} else
		LeaveCriticalSection (&cv->waiters_count_lock_);
	return 0; // FIXME error handling is missing!
}
#endif // TARGET_OS_WIN32

// -------------------------------------------------------------------------- //
//  * TCondVar( void )
// -------------------------------------------------------------------------- //
TCondVar::TCondVar( void )
{
	int err = ::pthread_cond_init( &mCondVar, NULL );
	assert( err == 0 );
	(void) err;
}

// -------------------------------------------------------------------------- //
//  * ~TCondVar( void )
// -------------------------------------------------------------------------- //
TCondVar::~TCondVar( void )
{
	int err = ::pthread_cond_destroy( &mCondVar );
	assert( err == 0 );
	(void) err;
}

// -------------------------------------------------------------------------- //
//  * Broadcast( void )
// -------------------------------------------------------------------------- //
void
TCondVar::Broadcast( void )
{
	int err = ::pthread_cond_broadcast( &mCondVar );
	assert( err == 0 );
	(void) err;
}

// -------------------------------------------------------------------------- //
//  * Signal( void )
// -------------------------------------------------------------------------- //
void
TCondVar::Signal( void )
{
	int err = ::pthread_cond_signal( &mCondVar );
	assert( err == 0 );
	(void) err;
}

// -------------------------------------------------------------------------- //
//  * TimedWait( TMutex*, const struct timespec* )
// -------------------------------------------------------------------------- //
bool
TCondVar::TimedWait( TMutex* inMutex, const struct timespec* inAbsTime )
{
	int err = ::pthread_cond_timedwait( &mCondVar, &inMutex->mMutex, inAbsTime );
	if (err == ETIMEDOUT)
	{
		return false;
	} else {
		assert( err == 0 );
		return true;
	}
}

// -------------------------------------------------------------------------- //
//  * TimedWaitRelative( TMutex*, const struct timespec* )
// -------------------------------------------------------------------------- //
bool
TCondVar::TimedWaitRelative( TMutex* inMutex, const struct timespec* inRelTime )
{
#if HAS_COND_TIMEDWAIT_RELATIVE_NP
	int err = ::pthread_cond_timedwait_relative_np(
					&mCondVar, &inMutex->mMutex, inRelTime );
#else
	struct timespec abs_time;
	struct timeval now;
	(void) gettimeofday( &now, NULL );
	abs_time.tv_nsec = (now.tv_usec * 1000) + inRelTime->tv_nsec;
	abs_time.tv_sec = now.tv_sec + (abs_time.tv_nsec / 1000000000);
	abs_time.tv_nsec %= 1000000000;
	int err = ::pthread_cond_timedwait(
					&mCondVar, &inMutex->mMutex, &abs_time );
#endif

	if (err == ETIMEDOUT)
	{
		return false;
	} else {
		assert( err == 0 );
		return true;
	}
}

// -------------------------------------------------------------------------- //
//  * Wait( TMutex*, const struct timespec* )
// -------------------------------------------------------------------------- //
void
TCondVar::Wait( TMutex* inMutex )
{
	int err = ::pthread_cond_wait( &mCondVar, &inMutex->mMutex );
	assert( err == 0 );
	(void) err;
}

// =============================================================================== //
// Niklaus Wirth has lamented that, whereas Europeans pronounce his name correctly //
// (Ni-klows Virt), Americans invariably mangle it into (Nick-les Worth).  Which   //
// is to say that Europeans call him by name, but Americans call him by value.     //
// =============================================================================== //
