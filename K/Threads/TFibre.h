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

#ifndef _TFIBRE_H
#define _TFIBRE_H


#undef FIBRE_USE_UCONTEXT
#undef FIBRE_USE_PTHREAD
#define FIBRE_USE_LONGJMP

#define FIBRE_STACK_SIZE 0x100000
// SIGSTKSZ

#include <K/Defines/KDefinitions.h>

// ANSI C & POSIX
#if TARGET_OS_WIN32
	#include <windows.h>
	#include <process.h>
#else
	#ifdef FIBRE_USE_UCONTEXT
		#define _XOPEN_SOURCE
		#include <ucontext.h>
	#endif
	#ifdef FIBRE_USE_PTHREAD
		#include <pthread.h>
	#endif
	#ifdef FIBRE_USE_LONGJMP
		#include <pthread.h>
		#include <setjmp.h>
	#endif
#endif
#include <assert.h>

// K
#include <K/Threads/TMutex.h>
#include <K/Threads/TCondVar.h>

/**
 Class for a fibre.
 
 A Fibre is a light-weight thread that allows a task to use different stacks and 
 environments quickly, but prohibts running consecutively, avoiding collisions
 and race conditions.
 
 A Fibre is created at a non-timecritical initialisation phase, putting the 
 fibre in "stopped" state.
 
 When stopped, the main program can launch a new Fibre using Run(). The main 
 thread is suspended and the fibre is set to "running".
 
 When runnign, the Fibre may suspend itself. This will cause Run() to return
 a non-zero value. The main thread will resume in the main context. The Fibre
 is in "suspended" mode and can only continue via Resume().
 
 When running the Fibre call will eventually return, putting the Fibre back into
 "stopped" mode. Run() will return 0 and the main thread will resume in 
 the main context.

 \author Matthias Melcher
 \version $Revision: 1.0 $
 */
class TFibre
{
public:
	///
	/// Constants.
	///
	
	///
	/// States of a thread.
	///
	enum EState {
		kStopped,		///< Fibre is stopped.
		kRunning,		///< Fibre is running.
		kSuspended		///< Fibre is suspended (waiting for Resume).
	};
	
	bool IsStopped() { return mState==kStopped; }
	bool IsRunning() { return mState==kRunning; }
	bool IsSuspended() { return mState==kSuspended; }

	///
	/// Constructor.
	///
	TFibre( KUInt32 inStackSize=0 );

	///
	/// Destructor.
	///
	virtual ~TFibre( void );

	/**
	 Run this task.
	 
	 This is the method that will be started when calling Run(). Override this
	 method to create your own task.
	 */
	virtual KSInt32 Task(KSInt32 inReason=0, void* inUserData=0L) = 0;
	
	/**
	 Run a fibre.
	 
	 This method is called by the main thread. The fibre must be stopped.
	 
	 This call will switch context and call the given task.
	 
	 \return -1 if the Fibre was not in stopped mode.
	 \return 0 if the fibre function exited normally.
	 \return a user defined value given as a prameter to Suspend().
	 */
	KSInt32 Run( KSInt32 inReason=0, void* inUserData=0L );

	/**
	 Suspend a fibre.
	 
	 This method is called by the Fibre Task. The Fibre must be running.
	 
	 This call swaps to the original context and exits Run(), returnig the value
	 in reason. Reason should not be 0 or -1. Suspend() will retun as soon as
	 the main thread calls Resume().
	 
	 \return Returns the reason given by the call to Resume().
	 */
	KSInt32 Suspend( KSInt32 inReason );
	
	/**
	 Resume a fibre.
	 
	 This method is called by the main thread. The fibre must be suspended.

	 This call swaps context to the Fibre Task. The Suspend() call by the fibre 
	 will return with the value given in Resume(). This call returns when the
	 Fibre call exits or the Fibre suspends.

	 \return -1 if the Fibre was not in suspend mode.
	 \return 0 if the fibre function exited normally.
	 \return a user defined value given as a prameter to a further Suspend().
	 */
	KSInt32 Resume( KSInt32 inReason );
	
private:
	
	EState				mState;				///< State of the fibre.
	KSInt32				mReason;			///< Reason for last call.
	void*				mUserData;			///< User Data exchange location.
	
#ifdef FIBRE_USE_UCONTEXT
	static TFibre*		sCurrentFibre;
    static void			TaskCaller(TFibre*);
	ucontext_t			mFibreContext;
	ucontext_t			mCallerContext;
#endif
#ifdef FIBRE_USE_PTHREAD
	static void*		TaskCaller(void*);
	pthread_t			mFibreThread;
	pthread_cond_t		mFibreWait;
	pthread_mutex_t		mFibreWaitMutex;
	pthread_cond_t		mCallerWait;
	pthread_mutex_t		mCallerWaitMutex;
#endif
#ifdef FIBRE_USE_LONGJMP
	static void*		TaskCaller(void*);
	pthread_t			mFibreThread;
	pthread_cond_t		mFibreWait;
	pthread_mutex_t		mFibreWaitMutex;
    pthread_cond_t		mCallerWait;
    pthread_mutex_t		mCallerWaitMutex;
	jmp_buf				mFibreContext;
	jmp_buf				mSuspendContext;
	jmp_buf				mCallerContext;
#endif
};


#endif
		// _TFIBRE_H

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
