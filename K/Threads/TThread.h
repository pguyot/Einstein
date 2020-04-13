// ==============================
// Fichier:			TThread.h
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
// The Original Code is TThread.h.
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
// $Id: TThread.h,v 1.6 2007/07/11 01:36:30 pguyot Exp $
// ===========

#ifndef _TTHREAD_H
#define _TTHREAD_H

#include <K/Defines/KDefinitions.h>

// ANSI C & POSIX
#if TARGET_OS_WIN32
	#include <windows.h>
	#include <process.h>
#else
	#include <pthread.h>
#endif
#include <assert.h>

// K
#include <K/Threads/TMutex.h>
#include <K/Threads/TCondVar.h>

///
/// Class for a thread.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.6 $
///
/// \test	aucun test défini.
///
class TThread
{
public:
	///
	/// Constants.
	///
	enum {
		kForever = 0,	///< Sleep forever, until WakeUp is called.
	};
	
	///
	/// States of a thread.
	///
	enum EState {
		kStopped,		///< Thread is stopped.
		kRunning,		///< Thread is running.
		kSuspended,		///< Thread is suspended (waiting for Resume).
		kSelfSuspended,	///< Thread is self-suspended (waiting for Resume).
		kSleeping		///< Thread is sleeping (waiting for WakeUp).
	};

	///
	/// Constructor from a runnable object.
	/// Starts the thread.
	///
	template<class TRunnable>
	TThread( TRunnable* inRunnable );

	///
	/// Destructor.
	/// Detaches and joins the thread.
	///
	~TThread( void );

	///
	/// Determine if the thread is the current thread.
	///
	bool IsCurrentThread( void ) const;

	///
	/// Suspend the thread.
	/// Uses a counter: each suspend call should be balanced by a resume call.
	///
	void Suspend( void );

	///
	/// Resume the thread.
	/// Uses a counter: each suspend call should be balanced by a resume call.
	///
	void Resume( void );

	///
	/// Sleep (for a given time, in milliseconds, or until WakeUp is called).
	/// The thread can actually sleep longer.
	/// This method MUST be called from the current thread.
	/// Decrease the wake up counter or sleep if it's zero.
	///
	/// \param inMillisecs		time to sleep.
	/// \return	\c true if we slept all the time, \c false otherwise.
	///
	bool Sleep( KUInt32 inMilliseconds = kForever );

	///
	/// Wakes the thread up.
	/// Uses a counter with the wake up calls.
	///
	void	WakeUp( void );

private:
	///
	/// Thread entry point (static).
	///
	/// \param inUserData		pointer to the runnable.
	///
	template<class TRunnable>
	static void*	SEntryPoint( void* inUserData )
		{
#if !TARGET_OS_OPENSTEP && !TARGET_OS_WIN32
			PreRun();
#endif
			((TRunnable*) inUserData)->Run();
			return NULL;
		}

	///
	/// Suspend ourselves.
	///
	void	SuspendSelf( void );

#if !TARGET_OS_OPENSTEP && !TARGET_OS_WIN32
	///
	/// Setup the thread (register signal actions).
	///
	static void	PreRun( void );

	///
	/// Handler for SIGUSR1.
	/// NOP.
	///
	/// \param inSignal	received signal (always SIGUSR1).
	///
	static void SignalUSR1( int inSignal );

	///
	/// Handler for SIGUSR2.
	/// Calls sigsuspend.
	///
	/// \param inSignal	received signal (always SIGUSR2).
	///
	static void SignalUSR2( int inSignal );
#endif

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TThread( const TThread& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TThread& operator = ( const TThread& inCopy );

	/// \name Variables
#if TARGET_OS_WIN32
	HANDLE				mThread;			///< Thread.
	DWORD				mThreadId;			///< Unique ID per thread
#else
	pthread_t			mThread;			///< Thread.
#endif
	TMutex				mMutex;				///< Mutex.
	TCondVar			mSleepCondVar;		///< CondVar (for sleep).
	TCondVar			mSuspendCondVar;	///< CondVar (for suspend).
	EState				mState;				///< State of the thread.
	KUInt32				mWakeCount;			///< Wake counter.
	KUInt32				mSuspendCount;		///< Count suspend calls.
};

// -------------------------------------------------------------------------- //
//  * TThread( TRunnable* )
// -------------------------------------------------------------------------- //
template<class TRunnable>
TThread::TThread( TRunnable* inRunnable )
	:
		mState( kRunning ),
		mWakeCount( 0 ),
		mSuspendCount( 0 )
{
#if TARGET_OS_WIN32
	// step-by-step conversion convinces Visual Studio and Cygwin
	typedef void *(*dcall)( void * );
	dcall tt = (dcall)(SEntryPoint<TRunnable>);
	//typedef unsigned (__stdcall *fcall)( void * );
	//mThread = (HANDLE)::_beginthreadex(0L, 0, (fcall)SEntryPoint<TRunnable>, inRunnable, 0, &mThreadId );
	mThread = ::CreateThread(0L, 0, (LPTHREAD_START_ROUTINE )tt, inRunnable, 0, &mThreadId );
	assert( mThread!=0L );
#else
	int err = ::pthread_create( &mThread, NULL, SEntryPoint<TRunnable>, inRunnable );
	assert( err == 0 );
	(void) err;
#endif
}

#endif
		// _TTHREAD_H

// ============================================================================= //
//         There was once a programmer who was attached to the court of the      //
// warlord of Wu.  The warlord asked the programmer: "Which is easier to design: //
// an accounting package or an operating system?"                                //
//         "An operating system," replied the programmer.                        //
//         The warlord uttered an exclamation of disbelief.  "Surely an          //
// accounting package is trivial next to the complexity of an operating          //
// system," he said.                                                             //
//         "Not so," said the programmer, "when designing an accounting package, //
// the programmer operates as a mediator between people having different ideas:  //
// how it must operate, how its reports must appear, and how it must conform to  //
// the tax laws.  By contrast, an operating system is not limited by outside     //
// appearances.  When designing an operating system, the programmer seeks the    //
// simplest harmony between machine and ideas.  This is why an operating system  //
// is easier to design."                                                         //
//         The warlord of Wu nodded and smiled.  "That is all good and well, but //
// which is easier to debug?"                                                    //
//         The programmer made no reply.                                         //
//                 -- Geoffrey James, "The Tao of Programming"                   //
// ============================================================================= //
