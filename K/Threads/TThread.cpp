// ==============================
// Fichier:			TThread.cp
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Cr�� le:			26/5/2005
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
// The Original Code is TThread.cp.
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
// $Id: TThread.cp,v 1.6 2007/07/11 01:36:30 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TThread.h"

// ANSI C & POSIX
#if TARGET_OS_WIN32
	#include <ctime>
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
	#include <stdlib.h>
#endif
#include <errno.h>
#include <signal.h>

#if RASPBERRY_PI || TARGET_OS_LINUX
#include <stdlib.h>
#endif

// Mach
#if TARGET_OS_OPENSTEP
#include <mach/thread_act.h>
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * ~TThread( void )
// -------------------------------------------------------------------------- //
TThread::~TThread( void )
{
#if TARGET_OS_WIN32
	DWORD ret = WaitForSingleObject(mThread, INFINITE);
	assert( ret==WAIT_OBJECT_0 );
#else
	int err = ::pthread_detach( mThread );
	
	if ( err != 0 )
		::abort();

	err = ::pthread_join( mThread, NULL );
	
	if ( !((err == 0) || (err == EINVAL) || (err == ESRCH)) )
		::abort();
#endif
}

// -------------------------------------------------------------------------- //
//  * IsCurrentThread( void ) const
// -------------------------------------------------------------------------- //
Boolean
TThread::IsCurrentThread( void ) const
{
#if TARGET_OS_WIN32
	return GetCurrentThreadId() == mThreadId;
#else
	return (::pthread_equal(mThread, ::pthread_self()) != 0);
#endif
}

// -------------------------------------------------------------------------- //
//  * Suspend( void )
// -------------------------------------------------------------------------- //
void
TThread::Suspend( void )
{
	mMutex.Lock();
	
	mSuspendCount++;

	if (mState == kRunning)
	{
		if (IsCurrentThread()) {
			mState = kSelfSuspended;

			SuspendSelf();
		} else {
			mState = kSuspended;

#if TARGET_OS_OPENSTEP
			kern_return_t err =
				::thread_suspend(::pthread_mach_thread_np(mThread));
			assert( err == 0 );
			(void) err;
#elif TARGET_OS_WIN32
			DWORD ret = SuspendThread(mThread);
			assert( (int)ret != -1 );
#else
			// Send SIGUSR2
			int err = ::pthread_kill(mThread, SIGUSR2);
			assert( err == 0 );
			(void) err;
#endif
		}
	}
	
	mMutex.Unlock();
}

// -------------------------------------------------------------------------- //
//  * SuspendSelf( void )
// -------------------------------------------------------------------------- //
void
TThread::SuspendSelf( void )
{
	// We are *this.
	// And we have the mutex.
	mSuspendCondVar.Wait(&mMutex);
}

// -------------------------------------------------------------------------- //
//  * Resume( void )
// -------------------------------------------------------------------------- //
void
TThread::Resume( void )
{
	mMutex.Lock();
	
	if (mSuspendCount > 0)
	{
		mSuspendCount--;
		bool selfSuspended = mState == kSelfSuspended;
		if ((selfSuspended || (mState == kSuspended))
			&& (mSuspendCount == 0))
		{
			mState = kRunning;
			
			if (selfSuspended) {
				// Signal ourselves.
				mSuspendCondVar.Signal();
			} else {
#if TARGET_OS_OPENSTEP
				kern_return_t err = ::thread_resume(
					::pthread_mach_thread_np(mThread));
				assert( err == 0 );
				(void) err;
#elif TARGET_OS_WIN32
				DWORD ret = ResumeThread(mThread);
				assert( (int)ret != -1 );
#else
				// Send SIGUSR1
				int err = ::pthread_kill(mThread, SIGUSR1);
				assert( err == 0 );
				(void) err;
#endif
			}
		}
	}

	mMutex.Unlock();
}

#if !TARGET_OS_OPENSTEP && !TARGET_OS_WIN32
// ------------------------------------------------------------------------- //
//  * SignalUSR2( int )
// ------------------------------------------------------------------------- //
void
TThread::SignalUSR2( int /* inSignal */ )
{
	// Wait for SIGUSR1.
	sigset_t theSigset;
	
	(void) sigfillset( &theSigset );
	(void) sigdelset( &theSigset, SIGUSR1 );
	
	(void) sigsuspend( &theSigset );
}

// ------------------------------------------------------------------------- //
//  * SignalUSR1( int )
// ------------------------------------------------------------------------- //
void
TThread::SignalUSR1( int /* inSignal */ )
{
}

// ------------------------------------------------------------------------- //
//  * PreRun( void )
// ------------------------------------------------------------------------- //
void
TThread::PreRun( void )
{
	// SIGUSR2 (suspend).
	struct sigaction theSigAction;
	theSigAction.sa_flags = 0;
	theSigAction.sa_handler = SignalUSR2;
	(void) sigemptyset(&theSigAction.sa_mask);

	int err = sigaction(SIGUSR2, &theSigAction, NULL);
	assert( err == 0 );

	// SIGUSR1 (resume).
	// Block SIGUSR2 to avoid any suspend while we resume.
	theSigAction.sa_handler = SignalUSR1;
	(void) sigaddset(&theSigAction.sa_mask, SIGUSR2);
	err = sigaction(SIGUSR1, &theSigAction, NULL);
	assert( err == 0 );
	(void) err;
}
#endif

// ------------------------------------------------------------------------- //
//  * Sleep( KUInt32 )
// ------------------------------------------------------------------------- //
Boolean
TThread::Sleep( KUInt32 inMilliseconds /* = kForever */ )
{
	assert(IsCurrentThread());
	
	Boolean theResult;
	
	Boolean forever = (inMilliseconds == kForever);
	
	// Acquire the mutex.
	mMutex.Lock();

	// Number of wake counts.
	if (mWakeCount > 0)
	{
		mWakeCount--;
		theResult = false;	// On n'a pas dormi tout le temps demand�.

		// Release the mutex.
		mMutex.Unlock();
	} else {
		// On dort.
		mState = kSleeping;
		
		if (!forever)
		{
			// Determine when to wake up.
			timespec theTimeToWake;
			theTimeToWake.tv_sec  = (long) (inMilliseconds / 1000);
			theTimeToWake.tv_nsec = (long) ((inMilliseconds % 1000) * 1000000);
			theResult = !mSleepCondVar.TimedWaitRelative(&mMutex, &theTimeToWake);
		} else {
			theResult = false;
			mSleepCondVar.Wait(&mMutex);
		}

		// We're running or we're suspended.
		if (mSuspendCount > 0)
		{
			mState = kSelfSuspended;
		} else {
			mState = kRunning;
		}
		
		// Suspend the thread.
		if (mState == kSelfSuspended)
		{
			SuspendSelf();
		}

		// Release the mutex.
		mMutex.Unlock();
	}
	
	return theResult;
}

// ------------------------------------------------------------------------- //
//  * WakeUp( void )
// ------------------------------------------------------------------------- //
void
TThread::WakeUp( void )
{
	// Acquire the mutex.
	mMutex.Lock();

	// If the thread is sleeping, wake it up.
	if (mState == kSleeping)
	{
		// Signal the condition variable.
		mSleepCondVar.Signal();
	} else {
		// Otherwise, increase the wake counter.
		mWakeCount++;
	}
	
	// Release the mutex.
	mMutex.Unlock();
}


// ============================================================================= //
// `Lasu' Releases SAG 0.3 -- Freeware Book Takes Paves For New World Order      //
// by staff writers                                                              //
//                                                                               //
//         ...                                                                   //
//         The central Superhighway site called ``sunsite.unc.edu''              //
// collapsed in the morning before the release.  News about the release had      //
// been leaked by a German hacker group, Harmonious Hardware Hackers, who        //
// had cracked into the author's computer earlier in the week.  They had         //
// got the release date wrong by one day, and caused dozens of eager fans        //
// to connect to the sunsite computer at the wrong time.  ``No computer can      //
// handle that kind of stress,'' explained the mourning sunsite manager,         //
// Erik Troan.  ``The spinning disks made the whole computer jump, and           //
// finally it crashed through the floor to the basement.''  Luckily,             //
// repairs were swift and the computer was working again the same evening.       //
// ``Thank God we were able to buy enough needles and thread and patch it        //
// together without major problems.''  The site has also installed a new         //
// throttle on the network pipe, allowing at most four clients at the same       //
// time, thus making a new crash less likely.  ``The book is now in our          //
// Incoming folder'', says Troan, ``and you're all welcome to come and get it.'' //
//                 -- Lars Wirzenius <wirzeniu@cs.helsinki.fi>                   //
//                    [comp.os.linux.announce]                                   //
// ============================================================================= //
