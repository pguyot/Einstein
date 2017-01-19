// ==============================
// File:			TFileManager.cp
// Project:			Einstein
//
// Copyright 2015 Steve White.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

#include "TFileManager.h"

// Einstein
#include "TMemory.h"

#if RASPBERRY_PI || TARGET_OS_LINUX
#include "../Log/TLog.h"
#else
#include "TLog.h"
#endif

//
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_LOGGING 0

// -------------------------------------------------------------------------- //
//  * TFileManager( TLog* )
// -------------------------------------------------------------------------- //
TFileManager::TFileManager(
						   TLog* inLog /* = nil */,
						   TMemory* inMemory )
	:
		mLog( inLog ),
		mMemory( inMemory ),
		mFDCount( 0 )
{
	mFileDescriptors = (SFileDescriptors*) ::malloc( 1 );
	mPath = "";
}

// -------------------------------------------------------------------------- //
//  * ~TFileManager( void )
// -------------------------------------------------------------------------- //
TFileManager::~TFileManager( void )
{
	SFileDescriptors* endFD = &mFileDescriptors[mFDCount];
	SFileDescriptors* cursor;
	for (cursor = mFileDescriptors; cursor < endFD; cursor++)
	{
		if (cursor->fHostFile != NULL) {
			::fclose(cursor->fHostFile);
		}
		else if (cursor->fNewtDesc != 0) {
			close_listener(cursor->fNewtDesc);
		}
		if (cursor->fName != NULL) {
			::free(cursor->fName);
		}
	}

	::free( mFileDescriptors );
}

// -------------------------------------------------------------------------- //
//  * descriptor_for_newton_desc( KUInt32  );
// -------------------------------------------------------------------------- //
SFileDescriptors*
TFileManager::descriptor_for_newton_desc( KUInt32 newt_desc )
{
	SFileDescriptors* endFD = &mFileDescriptors[mFDCount];
	SFileDescriptors* cursor;
	for (cursor = mFileDescriptors; cursor < endFD; cursor++)
	{
		if (cursor->fNewtDesc == newt_desc)
		{
			return cursor;
		}
	}

	return NULL;
}

// -------------------------------------------------------------------------- //
//  * descriptor_for_newton_name( const char * );
// -------------------------------------------------------------------------- //
SFileDescriptors*
TFileManager::descriptor_for_newton_name( const char *name )
{
	SFileDescriptors* endFD = &mFileDescriptors[mFDCount];
	SFileDescriptors* cursor;
	for (cursor = mFileDescriptors; cursor < endFD; cursor++)
	{
		if (strcmp(cursor->fName, name) == 0) {
			return cursor;
		}
	}
	
	return NULL;
}

// -------------------------------------------------------------------------- //
//  * new_descriptor_with_name( const char * );
// -------------------------------------------------------------------------- //
SFileDescriptors*
TFileManager::new_descriptor_with_name( const char *name )
{
	KUInt32 theFDCount = mFDCount + 1;
	mFileDescriptors = (SFileDescriptors*) ::realloc( mFileDescriptors,
													 sizeof(SFileDescriptors) * theFDCount);
	mFDCount = theFDCount;
	SFileDescriptors *desc = &mFileDescriptors[mFDCount - 1];
	desc->fNewtDesc = theFDCount;
	desc->fHostFile = NULL;
	desc->fNotifyAddr = 0x0;
	desc->fName = (char *)::malloc(strlen(name) + 1);
	::strcpy(desc->fName, name);
	return desc;
}

// -------------------------------------------------------------------------- //
//  * do_sys_open( const char *, KUInt32 );
// -------------------------------------------------------------------------- //
void
TFileManager::open_listener( const char *name, KUInt32 desc )
{
	
}

// -------------------------------------------------------------------------- //
//  * do_sys_open( const char *, KUInt32 );
// -------------------------------------------------------------------------- //
void
TFileManager::close_listener( KUInt32 desc )
{
	
}

// -------------------------------------------------------------------------- //
//  * do_sys_open( const char *, KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TFileManager::write_listener( KUInt32 desc, const void *buf, KUInt32 nbytes )
{
	return -1;
}

// -------------------------------------------------------------------------- //
//  * do_sys_open( const char *, KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TFileManager::read_listener( KUInt32 desc, void *buf, KUInt32 nbytes )
{
	return -1;
}

// -------------------------------------------------------------------------- //
//  * do_sys_open( const char *, KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TFileManager::do_sys_open( const char *name, const char *mode )
{
	if (FILE_LOGGING) {
		fprintf(stdout, "%s name='%s', mode='%s'\n", __PRETTY_FUNCTION__, name, mode);
	}
	if (mLog) {
		mLog->LogLine( "do_sys_open" );
	}
	
	SFileDescriptors *desc = NULL;
	
	if (strlen(name) > 0 && name[0] == '%') {
		desc = descriptor_for_newton_name(name);
		if (desc == NULL) {
			desc = new_descriptor_with_name(name);
			open_listener(name + 1, desc->fNewtDesc);
		}
	}
	else {
		if (mPath.length() == 0) {
			return -1;
		}
		
		std::string file = mPath + "/" + name;
		FILE *fp = ::fopen(file.c_str(), mode);
		if (fp == NULL) {
			return -1;
		}
		
		desc = new_descriptor_with_name(name);
		desc->fHostFile = fp;
	}
	
	return desc->fNewtDesc;
}

// -------------------------------------------------------------------------- //
//  * do_sys_close( KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TFileManager::do_sys_close( KUInt32 fp )
{
	if (FILE_LOGGING) {
		fprintf(stdout, "%s fp=%i\n", __PRETTY_FUNCTION__, fp);
	}
	if (mLog) {
		mLog->LogLine( "do_sys_close" );
	}
	
	SFileDescriptors* desc = descriptor_for_newton_desc(fp);
	if (desc->fHostFile != NULL) {
		::fflush(desc->fHostFile);
		::fclose(desc->fHostFile);
	}
	else {
		close_listener(fp);
	}
	
	if (desc->fName != NULL) {
		::free(desc->fName);
	}
	
	desc->fNewtDesc = 0;
	desc->fNotifyAddr = 0;
	desc->fHostFile = NULL;
	desc->fName = NULL;

	return 0;
}

// -------------------------------------------------------------------------- //
//  * do_sys_istty( KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TFileManager::do_sys_istty( KUInt32 fp )
{
	if (FILE_LOGGING) {
		fprintf(stdout, "%s fp=%i\n", __PRETTY_FUNCTION__, fp);
	}
	if (mLog) {
		mLog->LogLine( "do_sys_istty" );
	}

	SFileDescriptors* desc = descriptor_for_newton_desc(fp);
	if (desc == NULL) {
		return -1;
	}
	else if (desc->fHostFile != NULL) {
		return 0;
	}
	return 1;
}

// -------------------------------------------------------------------------- //
//  * do_sys_read( KUInt32, const void *, KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TFileManager::do_sys_read( KUInt32 fp, void *buf, KUInt32 nbyte )
{
	if (FILE_LOGGING) {
		fprintf(stdout, "%s fp=%i, buf=%p, nbyte=%i\n", __PRETTY_FUNCTION__, fp, buf, nbyte);
	}
	if (mLog) {
		mLog->LogLine( "do_sys_read" );
	}

	SFileDescriptors* desc = descriptor_for_newton_desc(fp);
	if (desc == NULL) {
		return 0;
	}
	else if (desc->fHostFile != NULL) {
		ssize_t result = ::fread(buf, sizeof(KUInt8), nbyte, desc->fHostFile);
		/*
		if (result == 0) {
			if (feof(desc->fHostFile)) {
				return -1;
			}
			else {
				return 0;
			}
		}
		 */
		return (KSInt32)result;
	}
	else {
		return read_listener(fp, buf, nbyte);
	}
}

// -------------------------------------------------------------------------- //
//  * do_sys_write( KUInt32, const void *, KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TFileManager::do_sys_write( KUInt32 fp, const void *buf, KUInt32 nbyte )
{
	if (FILE_LOGGING) {
		fprintf(stdout, "%s fp=%i, buf=%p, nbyte=%i\n", __PRETTY_FUNCTION__, fp, buf, nbyte);
	}
	if (mLog) {
		mLog->LogLine( "do_sys_write" );
	}

	SFileDescriptors* desc = descriptor_for_newton_desc(fp);
	if (desc == NULL) {
		return 0;
	}
	else if (desc->fHostFile != NULL) {
		ssize_t result = ::fwrite(buf, sizeof(KUInt8), nbyte, desc->fHostFile);
		if (result > 0) {
			::fflush(desc->fHostFile);
		}
		return (KSInt32)result;
	}
	else {
		return write_listener(fp, buf, nbyte);
	}
}

// -------------------------------------------------------------------------- //
//  * do_sys_set_input_notify( KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TFileManager::do_sys_set_input_notify( KUInt32 fp, KUInt32 address )
{
	if (FILE_LOGGING) {
		fprintf(stdout, "%s fp=%i, address=0x%08x\n", __PRETTY_FUNCTION__, fp, address);
	}
	if (mLog) {
		mLog->LogLine( "do_sys_set_input_notify" );
	}

	SFileDescriptors* desc = descriptor_for_newton_desc(fp);
	if (desc == NULL) {
		return -1;
	}
	desc->fNotifyAddr = address;
	return 0;
}

// -------------------------------------------------------------------------- //
//  * do_sys_seek( KUInt32, KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TFileManager::do_sys_seek( KUInt32 fp, KUInt32 pos )
{
	if (FILE_LOGGING) {
		fprintf(stdout, "%s fp=%i, pos=%i\n", __PRETTY_FUNCTION__, fp, pos);
	}
	if (mLog) {
		mLog->LogLine( "do_sys_seek" );
	}

	SFileDescriptors* desc = descriptor_for_newton_desc(fp);
	if (desc == NULL) {
		return -1;
	}
	else if (desc->fHostFile != NULL) {
		// For some reason, the Newton opens the file for reading,
		// flen()s the file, then seeks to the end. Then it can't
		// read anything...
		//::fseek(desc->fHostFile, pos, SEEK_SET);
		return 0;
	}
	else {
		return 0;
	}
}

// -------------------------------------------------------------------------- //
//  * do_sys_flen( KUInt32 );
// -------------------------------------------------------------------------- //
KSInt32
TFileManager::do_sys_flen( KUInt32 fp )
{
	if (FILE_LOGGING) {
		fprintf(stdout, "%s fp=%i\n", __PRETTY_FUNCTION__, fp);
	}
	if (mLog) {
		mLog->LogLine( "do_sys_flen" );
	}

	SFileDescriptors* desc = descriptor_for_newton_desc(fp);
	if (desc == NULL) {
		return -1;
	}
	else if (desc->fHostFile != NULL) {
		struct stat buf;
		::fstat(fileno(desc->fHostFile), &buf);
		return (KSInt32)buf.st_size;
	}
	else {
		return 0;
	}
}

// -------------------------------------------------------------------------- //
//  * listener_has_input( KUInt32 );
// -------------------------------------------------------------------------- //
void
TFileManager::set_listener_has_input( KUInt32 fp, bool has_input )
{
	SFileDescriptors* desc = descriptor_for_newton_desc(fp);
	if (desc != NULL && desc->fNotifyAddr != 0) {
		// XXX: Is this thread safe? Cocoa would be invoking us on the main thread...
		mMemory->Write(desc->fNotifyAddr, (has_input ? 1 : 0));
	}
}

