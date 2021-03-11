// ==============================
// Fichier:			TFiber
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
#include "TFiber.h"

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


#ifdef FIBER_USE_UCONTEXT
TFiber* TFiber::sCurrentFiber = 0;
#endif

// -------------------------------------------------------------------------- //
//  * TFiber( void )
// -------------------------------------------------------------------------- //
TFiber::TFiber( KUInt32 inStackSize )
{
	mState = kStopped;

	// ucontext has long been deprecated, but does exactly what we need!
#ifdef FIBER_USE_UCONTEXT
	// clear the structures
	memset(&mFiberContext, 0, sizeof(ucontext_t));
	memset(&mCallerContext, 0, sizeof(ucontext_t));

	// initialize everything that may waste time later
	getcontext(&mFiberContext);
	mFiberContext.uc_link = &mCallerContext;
	mFiberContext.uc_stack.ss_sp = calloc(1, FIBER_STACK_SIZE);
	mFiberContext.uc_stack.ss_size = FIBER_STACK_SIZE;
	makecontext(&mFiberContext, (void(*)())TaskCaller, 0);
#endif
	
	// pthreads are the recommended replacement for ucontext
#ifdef FIBER_USE_PTHREAD  
	// condition used to lock the fiber task when not needed
	pthread_cond_init(&mFiberWait, NULL);
	pthread_mutex_init(&mFiberWaitMutex, NULL);
	
	// condition to lock the calling task until the fiber is suspended or done
	pthread_cond_init(&mCallerWait, NULL);
	pthread_mutex_init(&mCallerWaitMutex, NULL);
		
	// set up the fiber
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	// launch the fiber, making sure that we wait before we signal!
	pthread_mutex_lock(&mCallerWaitMutex);
	pthread_create(&mFiberThread, &attr, TaskCaller, this);
	pthread_cond_wait(&mCallerWait, &mCallerWaitMutex);
	pthread_mutex_unlock(&mCallerWaitMutex);
	
	pthread_attr_destroy(&attr);
#endif

	// use pthread to create the alternate context, but switch contexts using longjmp
#ifdef FIBER_USE_LONGJMP
	// condition used to lock the fiber task when not needed
	pthread_cond_init(&mFiberWait, NULL);
	pthread_mutex_init(&mFiberWaitMutex, NULL);
	
	// condition to lock the calling task until the fiber is suspended or done
	pthread_cond_init(&mCallerWait, NULL);
	pthread_mutex_init(&mCallerWaitMutex, NULL);
	
	// set up the fiber
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	// launch the fiber, making sure that we wait before we signal!
	pthread_mutex_lock(&mCallerWaitMutex);
	pthread_create(&mFiberThread, &attr, TaskCaller, this);
	pthread_cond_wait(&mCallerWait, &mCallerWaitMutex);
	pthread_mutex_unlock(&mCallerWaitMutex);
	
	pthread_attr_destroy(&attr);
#endif
}

// -------------------------------------------------------------------------- //
//  * ~TFiber( void )
// -------------------------------------------------------------------------- //
TFiber::~TFiber( void )
{
#ifdef FIBER_USE_UCONTEXT
#endif
#ifdef FIBER_USE_PTHREAD
#endif
}

// -------------------------------------------------------------------------- //
//  * void Run( void )
// -------------------------------------------------------------------------- //
KSInt32 TFiber::Run( KSInt32 inReason, void* inUserData )
{
	if (mState!=kStopped) {
		KPrintf("ERROR in TFiber: Run() called, but fiber isn't stopped.\n");
		return -1;
	}
	mReason = inReason;
	mUserData = inUserData;
	mState = kRunning;
#ifdef FIBER_USE_UCONTEXT
	sCurrentFiber = this;
	swapcontext(&mCallerContext, &mFiberContext);
#endif
#ifdef FIBER_USE_PTHREAD
	// wake fiber from sleep
	pthread_mutex_lock(&mFiberWaitMutex);
	pthread_cond_signal(&mFiberWait);
	pthread_mutex_unlock(&mFiberWaitMutex);

	// wait for fiber to finish
	pthread_mutex_lock(&mCallerWaitMutex);
	pthread_cond_wait(&mCallerWait, &mCallerWaitMutex);
	pthread_mutex_unlock(&mCallerWaitMutex);
	
#endif
#ifdef FIBER_USE_LONGJMP
	if (_setjmp(mCallerContext)) {
		// fiber returned
	} else {
		_longjmp(mFiberContext, 1);
	}
#endif
	// mState is set by TaskCaller() or Suspend()
	return mReason;
}

// -------------------------------------------------------------------------- //
//  * void Suspend( void )
// -------------------------------------------------------------------------- //
KSInt32 TFiber::Suspend( KSInt32 inReason )
{
	if (mState!=kRunning) {
		KPrintf("ERROR in TFiber: Suspend() called, but fiber isn't running.\n");
		return -1;
	}
	mState = kSuspended;
	mReason = inReason;
#ifdef FIBER_USE_UCONTEXT
#endif
#ifdef FIBER_USE_PTHREAD
	// resume caller
	pthread_mutex_lock(&mCallerWaitMutex);
	pthread_cond_signal(&mCallerWait);
	pthread_mutex_unlock(&mCallerWaitMutex);
	
	// suspend self
	pthread_mutex_lock(&mFiberWaitMutex);
	pthread_cond_wait(&mFiberWait, &mFiberWaitMutex);
	pthread_mutex_unlock(&mFiberWaitMutex);
#endif
#ifdef FIBER_USE_LONGJMP
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
KSInt32 TFiber::Resume( KSInt32 inReason )
{
	if (mState!=kSuspended) {
		KPrintf("ERROR in TFiber: Resume() called, but fiber isn't suspended.\n");
		return -1;
	}
	mState = kRunning;
	mReason = inReason;
#ifdef FIBER_USE_UCONTEXT
#endif
#ifdef FIBER_USE_PTHREAD
	// wake fiber from sleep
	pthread_mutex_lock(&mFiberWaitMutex);
	pthread_cond_signal(&mFiberWait);
	pthread_mutex_unlock(&mFiberWaitMutex);
	
	// wait for fiber to finish
	pthread_mutex_lock(&mCallerWaitMutex);
	pthread_cond_wait(&mCallerWait, &mCallerWaitMutex);
	pthread_mutex_unlock(&mCallerWaitMutex);
#endif
#ifdef FIBER_USE_LONGJMP
	if (_setjmp(mCallerContext)) {
		// finished tasks and Suspend() jump here
	} else {
		_longjmp(mSuspendContext, 1);
	}
#endif
	return mReason;
}

#ifdef FIBER_USE_UCONTEXT

// -------------------------------------------------------------------------- //
//  * void TaskCaller( void )
// -------------------------------------------------------------------------- //
void TFiber::TaskCaller(TFiber* inFiber)
{
	TFiber* This = sCurrentFiber;
	This->mReason = This->Task(This->mReason, This->mUserData);
	setcontext(&This->mCallerContext);
}

#endif

#ifdef FIBER_USE_PTHREAD

// -------------------------------------------------------------------------- //
//  * void TaskCaller( void )
// -------------------------------------------------------------------------- //
void* TFiber::TaskCaller(void* inFiber)
{
	TFiber* This = (TFiber*)inFiber;
	
	for (;;) {
		// resume caller
		pthread_mutex_lock(&This->mCallerWaitMutex);
		pthread_cond_signal(&This->mCallerWait);
		pthread_mutex_unlock(&This->mCallerWaitMutex);
	
		// suspend self
		pthread_mutex_lock(&This->mFiberWaitMutex);
		pthread_cond_wait(&This->mFiberWait, &This->mFiberWaitMutex);
		pthread_mutex_unlock(&This->mFiberWaitMutex);

		// do our taks
		This->mState = kRunning;
		This->mReason = This->Task(This->mReason, This->mUserData);
		This->mState = kStopped;
		
		// repeat forever
	}
	return 0L;
}

#endif

#ifdef FIBER_USE_LONGJMP

// -------------------------------------------------------------------------- //
//  * void TaskCaller( void )
// -------------------------------------------------------------------------- //
void* TFiber::TaskCaller(void* inFiber)
{
	TFiber* This = (TFiber*)inFiber;
	
	if (_setjmp(This->mFiberContext)) {
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
		pthread_mutex_lock(&This->mFiberWaitMutex);
		pthread_cond_wait(&This->mFiberWait, &This->mFiberWaitMutex);
		pthread_mutex_unlock(&This->mFiberWaitMutex);
		
		// repeat forever
	}
	return 0L;
}

#endif

// -------------------------------------------------------------------------- //
//  * void Abort( KSInt32 reason )
// -------------------------------------------------------------------------- //
void TFiber::Abort(KSInt32 inReason)
{
	if (!IsSuspended()) {
		KPrintf("ERROR in TFiber: Abort() called, but fiber isn't suspended.\n");
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
