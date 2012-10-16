// ==============================
// Fichier:			TFibre
// Projet:			K
// Ecrit par:		Matthias Melcher
//
// CrŽŽ le:			9/7/2012
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
// The Initial Developer of the Original Code is Matthias Melcher.
// Portions created by the Initial Developer are Copyright (C) 2012 the
// Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Matthias Melcher (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id$
// ===========

#include <K/Defines/KDefinitions.h>
#include "TFibre.h"

// ANSI C & POSIX
#if !TARGET_OS_WIN32
	#include <sys/time.h>
#endif
#include <errno.h>
#include <signal.h>
#include <string.h>

// Mach
#if TARGET_OS_OPENSTEP
#include <mach/thread_act.h>
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //


#ifdef FIBRE_USE_UCONTEXT
TFibre* TFibre::sCurrentFibre = 0;
#endif

// -------------------------------------------------------------------------- //
//  * TFibre( void )
// -------------------------------------------------------------------------- //
TFibre::TFibre( KUInt32 inStackSize )
{
	mState = kStopped;

	// ucontext has long been deprecated, but does exactly what we need!
#ifdef FIBRE_USE_UCONTEXT
	// clear the structures
	memset(&mFibreContext, 0, sizeof(ucontext_t));
	memset(&mCallerContext, 0, sizeof(ucontext_t));

	// initialize evrything that may waste time later
	getcontext(&mFibreContext);
	mFibreContext.uc_link = &mCallerContext;
	mFibreContext.uc_stack.ss_sp = calloc(1, FIBRE_STACK_SIZE);
	mFibreContext.uc_stack.ss_size = FIBRE_STACK_SIZE;
	makecontext(&mFibreContext, (void(*)())TaskCaller, 0);
#endif
	
	// pthreads are the recommended replacement for ucontext
#ifdef FIBRE_USE_PTHREAD  
	// condition used to lock the fibre task when not needed
	pthread_cond_init(&mFibreWait, NULL);
	pthread_mutex_init(&mFibreWaitMutex, NULL);
	
	// condition to lock the calling task until the fibre is suspended or done
	pthread_cond_init(&mCallerWait, NULL);
	pthread_mutex_init(&mCallerWaitMutex, NULL);
		
	// set up the fibre
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	// launch the fibre, making sure that we wait before we signal!
	pthread_mutex_lock(&mCallerWaitMutex);
	pthread_create(&mFibreThread, &attr, TaskCaller, this);
	pthread_cond_wait(&mCallerWait, &mCallerWaitMutex);
	pthread_mutex_unlock(&mCallerWaitMutex);
	
	pthread_attr_destroy(&attr);
#endif

	// use pthread to create the alternate context, but switch contexts using longjmp
#ifdef FIBRE_USE_LONGJMP
	// condition used to lock the fibre task when not needed
	pthread_cond_init(&mFibreWait, NULL);
	pthread_mutex_init(&mFibreWaitMutex, NULL);
	
	// condition to lock the calling task until the fibre is suspended or done
	pthread_cond_init(&mCallerWait, NULL);
	pthread_mutex_init(&mCallerWaitMutex, NULL);
	
	// set up the fibre
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	// launch the fibre, making sure that we wait before we signal!
	pthread_mutex_lock(&mCallerWaitMutex);
	pthread_create(&mFibreThread, &attr, TaskCaller, this);
	pthread_cond_wait(&mCallerWait, &mCallerWaitMutex);
	pthread_mutex_unlock(&mCallerWaitMutex);
	
	pthread_attr_destroy(&attr);
#endif
}

// -------------------------------------------------------------------------- //
//  * ~TFibre( void )
// -------------------------------------------------------------------------- //
TFibre::~TFibre( void )
{
#ifdef FIBRE_USE_UCONTEXT
#endif
#ifdef FIBRE_USE_PTHREAD
#endif
}

// -------------------------------------------------------------------------- //
//  * void Run( void )
// -------------------------------------------------------------------------- //
KSInt32 TFibre::Run( KSInt32 inReason, void* inUserData )
{
	if (mState!=kStopped) {
		fprintf(stderr, "ERROR in TFibre: Run() called, but fibre isn't stopped.\n");
		return -1;
	}
	mReason = inReason;
	mUserData = inUserData;
	mState = kRunning;
#ifdef FIBRE_USE_UCONTEXT
	sCurrentFibre = this;
	swapcontext(&mCallerContext, &mFibreContext);
#endif
#ifdef FIBRE_USE_PTHREAD
	// wake fibre from sleep
	pthread_mutex_lock(&mFibreWaitMutex);
	pthread_cond_signal(&mFibreWait);
	pthread_mutex_unlock(&mFibreWaitMutex);

	// wait for fibre to finish
	pthread_mutex_lock(&mCallerWaitMutex);
	pthread_cond_wait(&mCallerWait, &mCallerWaitMutex);
	pthread_mutex_unlock(&mCallerWaitMutex);
	
#endif
#ifdef FIBRE_USE_LONGJMP
	if (_setjmp(mCallerContext)) {
		// fibre returned
	} else {
		_longjmp(mFibreContext, 1);
	}
#endif
	// mState is set by TaskCaller() or Suspend()
	return mReason;
}

// -------------------------------------------------------------------------- //
//  * void Suspend( void )
// -------------------------------------------------------------------------- //
KSInt32 TFibre::Suspend( KSInt32 inReason )
{
	if (mState!=kRunning) {
		fprintf(stderr, "ERROR in TFibre: Suspend() called, but fibre isn't running.\n");
		return -1;
	}
	mState = kSuspended;
	mReason = inReason;
#ifdef FIBRE_USE_UCONTEXT
#endif
#ifdef FIBRE_USE_PTHREAD
	// resume caller
	pthread_mutex_lock(&mCallerWaitMutex);
	pthread_cond_signal(&mCallerWait);
	pthread_mutex_unlock(&mCallerWaitMutex);
	
	// suspend self
	pthread_mutex_lock(&mFibreWaitMutex);
	pthread_cond_wait(&mFibreWait, &mFibreWaitMutex);
	pthread_mutex_unlock(&mFibreWaitMutex);
#endif
#ifdef FIBRE_USE_LONGJMP
	if (_setjmp(mSuspendContext)) {
		// Resume() jumps here
	} else {
		_longjmp(mCallerContext, 1);
	}
#endif
	return mReason;
}

// -------------------------------------------------------------------------- //
//  * void Resume( void )
// -------------------------------------------------------------------------- //
KSInt32 TFibre::Resume( KSInt32 inReason )
{
	if (mState!=kSuspended) {
		fprintf(stderr, "ERROR in TFibre: Resume() called, but fibre isn't suspended.\n");
		return -1;
	}
	mState = kRunning;
	mReason = inReason;
#ifdef FIBRE_USE_UCONTEXT
#endif
#ifdef FIBRE_USE_PTHREAD
	// wake fibre from sleep
	pthread_mutex_lock(&mFibreWaitMutex);
	pthread_cond_signal(&mFibreWait);
	pthread_mutex_unlock(&mFibreWaitMutex);
	
	// wait for fibre to finish
	pthread_mutex_lock(&mCallerWaitMutex);
	pthread_cond_wait(&mCallerWait, &mCallerWaitMutex);
	pthread_mutex_unlock(&mCallerWaitMutex);
#endif
#ifdef FIBRE_USE_LONGJMP
	if (_setjmp(mCallerContext)) {
		// finished tasks and Suspend() jump here
	} else {
		_longjmp(mSuspendContext, 1);
	}
#endif
	return mReason;
}

#ifdef FIBRE_USE_UCONTEXT

// -------------------------------------------------------------------------- //
//  * void TaskCaller( void )
// -------------------------------------------------------------------------- //
void TFibre::TaskCaller(TFibre* inFibre)
{
	TFibre* This = sCurrentFibre;
	This->mReason = This->Task(This->mReason, This->mUserData);
	setcontext(&This->mCallerContext);
}

#endif

#ifdef FIBRE_USE_PTHREAD

// -------------------------------------------------------------------------- //
//  * void TaskCaller( void )
// -------------------------------------------------------------------------- //
void* TFibre::TaskCaller(void* inFibre)
{
	TFibre* This = (TFibre*)inFibre;
	
	for (;;) {
		// resume caller
		pthread_mutex_lock(&This->mCallerWaitMutex);
		pthread_cond_signal(&This->mCallerWait);
		pthread_mutex_unlock(&This->mCallerWaitMutex);
	
		// suspend self
		pthread_mutex_lock(&This->mFibreWaitMutex);
		pthread_cond_wait(&This->mFibreWait, &This->mFibreWaitMutex);
		pthread_mutex_unlock(&This->mFibreWaitMutex);

		// do our taks
		This->mState = kRunning;
		This->mReason = This->Task(This->mReason, This->mUserData);
		This->mState = kStopped;
		
		// repeat forever
	}
	return 0L;
}

#endif

#ifdef FIBRE_USE_LONGJMP

// -------------------------------------------------------------------------- //
//  * void TaskCaller( void )
// -------------------------------------------------------------------------- //
void* TFibre::TaskCaller(void* inFibre)
{
	TFibre* This = (TFibre*)inFibre;
	
	if (_setjmp(This->mFibreContext)) {
		// do our taks
		This->mState = kRunning;
		This->mReason = This->Task(This->mReason, This->mUserData);
		This->mState = kStopped;
		// return to caller
		_longjmp(This->mCallerContext, 1);
	} else {
		// go to sleep forever - we just need a valid context.
	}
	
	// resume caller
	pthread_mutex_lock(&This->mCallerWaitMutex);
	pthread_cond_signal(&This->mCallerWait);
	pthread_mutex_unlock(&This->mCallerWaitMutex);
	
	for (;;) {
		
		// suspend self
		pthread_mutex_lock(&This->mFibreWaitMutex);
		pthread_cond_wait(&This->mFibreWait, &This->mFibreWaitMutex);
		pthread_mutex_unlock(&This->mFibreWaitMutex);
		
		// repeat forever
	}
	return 0L;
}

#endif

// -------------------------------------------------------------------------- //
//  * void Abort( KSInt32 reason )
// -------------------------------------------------------------------------- //
void TFibre::Abort(KSInt32 inReason)
{
	if (!IsSuspended()) {
		fprintf(stderr, "ERROR in TFibre: Abort() called, but fibre isn't suspended.\n");
		return;
	}
	mReason = inReason;
	mState = kStopped;
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
