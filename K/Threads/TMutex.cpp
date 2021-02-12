// ==============================
// Fichier:			TMutex.cp
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
// The Original Code is TMutex.cp.
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
// $Id: TMutex.cp,v 1.3 2007/07/11 01:36:30 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TMutex.h"

// ANSI C & POSIX
#include <assert.h>
#include <errno.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TMutex( void )
// -------------------------------------------------------------------------- //
TMutex::TMutex( void )
{
#if TARGET_OS_WIN32
	mMutex = CreateMutex(0L, FALSE, 0L);
	assert(mMutex);
#else
	int err = ::pthread_mutex_init( &mMutex, NULL );
	assert( err == 0 );
	(void) err;
#endif
}

// -------------------------------------------------------------------------- //
//  * ~TMutex( void )
// -------------------------------------------------------------------------- //
TMutex::~TMutex( void )
{
#if TARGET_OS_WIN32
	BOOL ret = CloseHandle(mMutex);
	mMutex = INVALID_HANDLE_VALUE;
	assert( ret != 0 );
#else
	int err = ::pthread_mutex_destroy( &mMutex );
	assert( err == 0 );
	(void) err;
#endif
}

// -------------------------------------------------------------------------- //
//  * Lock( void )
// -------------------------------------------------------------------------- //
void
TMutex::Lock( void )
{
#if TARGET_OS_WIN32
	if (mMutex != INVALID_HANDLE_VALUE) {
		DWORD ret = WaitForSingleObject(mMutex, INFINITE);
		assert(ret == WAIT_OBJECT_0);
	}
#else
	int err = ::pthread_mutex_lock( &mMutex );
	assert( err == 0 );
	(void) err;
#endif
}

// -------------------------------------------------------------------------- //
//  * Unlock( void )
// -------------------------------------------------------------------------- //
void
TMutex::Unlock( void )
{
#if TARGET_OS_WIN32
	if (mMutex != INVALID_HANDLE_VALUE) {
		BOOL ret = ReleaseMutex(mMutex);
		//assert(ret != 0);
	}
#else
	int err = ::pthread_mutex_unlock( &mMutex );
	assert( err == 0 );
	(void) err;
#endif
}

// -------------------------------------------------------------------------- //
//  * TryLock( void )
// -------------------------------------------------------------------------- //
bool
TMutex::TryLock( void )
{
#if TARGET_OS_WIN32
	DWORD ret = WaitForSingleObject(mMutex, 0);
	if (ret == WAIT_TIMEOUT) {
		return true;
	} else {
		assert( ret == WAIT_OBJECT_0 );
		return false;
	}
#else
	int err = ::pthread_mutex_trylock( &mMutex );
	if (err == EBUSY)
	{
		return true;
	} else {
		assert( err == 0 );
		return false;
	}
#endif
}

// ========================================= //
// egrep -n '^[a-z].*\(' $ | sort -t':' +2.0 //
// ========================================= //
