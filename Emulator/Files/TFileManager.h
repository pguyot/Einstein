// ==============================
// File:			TFileManager.h
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

#ifndef _TFILEMANAGER_H
#define _TFILEMANAGER_H

#include <K/Defines/KDefinitions.h>
#include <string>

class TLog;
class TMemory;

typedef struct {
	char*       fName;			///Filename, as given by the Newton
	FILE*       fHostFile;		///File descriptor used on the host
	KUInt32	    fNewtDesc;		///File descriptor used on the Newton
	KUInt32     fNotifyAddr;	///Address used to notify the Newton about available input
} SFileDescriptors;

class TFileManager
{
public:
	///
	/// Constructor from a log and memory.
	///
	/// \param inLog				log interface (can be null)
	/// \param inMemory				memory interface
	///
	TFileManager(
				   TLog* inLog = nil,
				   TMemory *inMemory = nil);
	
	///
	/// Destructor.
	///
	virtual ~TFileManager( void );

	
	///
	/// Opens a file on the host system. The file path is specified either as relative to the
	/// current directory of the host process, or absolute, using the path conventions of the
	/// host operating system.
	///
	/// This method is called by the emulator in response to a TapFileCntl
	///
	/// \return
	/// a nonzero handle if the call is successful
	/// -1 if the call is not successful.
	///
	KSInt32 do_sys_open( const char *name, const char *mode );

	///
	/// Closes a file on the host system. The handle must reference a file that was opened with SYS_OPEN.
	///
	/// This method is called by the emulator in response to a TapFileCntl
	///
	/// \return
	/// 0 if the call is successful
	/// -1 if the call is not successful.
	///
	KSInt32 do_sys_close( KUInt32 fp );
	
	///
	/// Checks whether a file is connected to an interactive device.
	///
	/// This method is called by the emulator in response to a TapFileCntl
	///
	/// \return
	/// 1 if the handle identifies an interactive device
	/// 0 if the handle identifies a file
	/// a value other than 1 or 0 if an error occurs.
	///
	KSInt32 do_sys_istty( KUInt32 fp );
	
	///
	/// Reads the contents of a file into a buffer.
	///
	/// This method is called by the emulator in response to a TapFileCntl
	///
	/// \return
	/// The number of bytes that are read, or -1 if there is an error.
	///
	KSInt32 do_sys_read( KUInt32 fp, void *buf, KUInt32 nbyte );
	
	///
	/// Writes the contents of a buffer to a specified file at the current file position.
	///
	/// This method is called by the emulator in response to a TapFileCntl
	///
	/// \return
	/// The number of bytes that are written, or -1 if there is an error.
	///
	KSInt32 do_sys_write( KUInt32 fp, const void *buf, KUInt32 nbyte );
	
	///
	/// XXX: DOCUMENTME
	///
	/// This method is called by the emulator in response to a TapFileCntl
	///
	/// \return
	/// 0 if the call is successful
	/// -1 if an error occurs.
	///
	KSInt32 do_sys_set_input_notify( KUInt32 fp, KUInt32 address );
	
	///
	/// Seeks to a specified position in a file using an offset specified from the
	/// start of the file. The file is assumed to be a byte array and the offset is
	/// given in bytes.
	///
	/// This method is called by the emulator in response to a TapFileCntl
	///
	/// \return
	/// 0 if the request is successful
	/// A negative value if the request is not successful.
	///
	KSInt32 do_sys_seek( KUInt32 fp, KUInt32 pos );
	
	///
	/// Returns the length of a specified file.
	///
	/// This method is called by the emulator in response to a TapFileCntl
	///
	/// \return
	/// the current length of the file object, if the call is successful
	/// -1 if an error occurs.
	///
	KSInt32 do_sys_flen( KUInt32 fp );

	///
	/// This method is called by the platform to send data to the Newton.
	///
	void set_listener_has_input( KUInt32 desc, Boolean has_input );
		
	///
	/// Set the memory interface.
	/// This method is called once the memory interface is created.
	///
	/// \param inManager	reference to the memory interface
	///
	void	SetMemory( TMemory* inMemory )
	{
		mMemory = inMemory;
	}

	///
	/// Set the memory interface.
	/// This method is called once the memory interface is created.
	///
	/// \param inManager	reference to the memory interface
	///
	void	SetLog( TLog* inLog )
	{
		mLog = inLog;
	}

	///
	/// Set the file system path.
	/// If the Newton needs to read/write files, it will look for them in this path.
	///
	/// \param inPath	reference to the file path
	///
	void	SetPath( std::string inPath )
	{
		mPath = inPath;
	}

protected:

	///
	/// Accessor on the interface to the log (may be null).
	///
	TLog*	GetLog( void ) const
	{
		return mLog;
	}
	
private:
	///
	/// XXX: DOCUMENTME
	///
	SFileDescriptors *descriptor_for_newton_desc( KUInt32 newt_desc );

	///
	/// XXX: DOCUMENTME
	///
	SFileDescriptors *descriptor_for_newton_name( const char *name );

	///
	/// XXX: DOCUMENTME
	///
	SFileDescriptors *new_descriptor_with_name( const char *name );

	///
	/// XXX: DOCUMENTME
	///
	virtual void open_listener( const char *name, KUInt32 desc );

	///
	/// XXX: DOCUMENTME
	///
	virtual void close_listener( KUInt32 desc );
	
	///
	/// XXX: DOCUMENTME
	///
	virtual KSInt32 write_listener( KUInt32 desc, const void *buf, KUInt32 nbytes );

	///
	/// XXX: DOCUMENTME
	///
	virtual KSInt32 read_listener( KUInt32 desc, void *buf, KUInt32 nbytes );
	
public:
	/// \name Variables
	TLog*				mLog;				///< Reference to the log.
	TMemory*			mMemory;			///< Interface to the memory.
	std::string         mPath;
	
	KUInt32				mFDCount;			///< Number of file descriptors.
	SFileDescriptors*	mFileDescriptors;	///< File descriptors.
	// probably want an FP mapping...
	// buffer writes from platform...
	
};

#endif // _TFILEMANAGER_H