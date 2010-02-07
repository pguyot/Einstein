// ==============================
// Fichier:			TFileStream.h
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			11/6/2005
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
// The Original Code is TFileStream.h.
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
// $Id: TFileStream.h,v 1.1 2006/01/18 09:05:01 pguyot Exp $
// ===========

#ifndef _TFILESTREAM_H
#define _TFILESTREAM_H

#include <K/Defines/KDefinitions.h>
#include <K/Streams/TRandomAccessStream.h>

// ANSI C
#include <stdio.h>

///
/// Class for a C File stream (based on FILE*).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.1 $
///
/// \test	aucun test défini.
///
class TFileStream
	:
		public TRandomAccessStream
{
public:
	///
	/// Constructor from a path and a mode.
	/// If a problem occurs, an exception is thrown.
	/// The file will be closed by the destructor.
	///
	TFileStream( const char* inPath, const char* inMode );

	///
	/// Constructor from a FILE*.
	/// The file will not be closed by the destructor.
	///
	/// \param inFile	file to work with.
	///
	TFileStream( FILE* inFile );

	///
	/// Destructor.
	/// Closes the file if the constructor opened it.
	///
	virtual ~TFileStream( void );

	/// \name Input/Output interface.

	///
	/// Read some bytes.
	///
	/// \param outBuffer	buffer for read bytes.
	/// \param ioCount		number of bytes to read on input, number of bytes
	///						actually read on output. This value is updated
	///						before any exception is thrown if ever a problem
	///						occurred.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		Read( void* outBuffer, KUInt32* ioCount );

	///
	/// Write some bytes.
	///
	/// \param inBuffer		buffer for bytes to write.
	/// \param ioCount		number of bytes to write on input, number of bytes
	///						actually written on output. This value is updated
	///						before any exception is thrown if ever a problem
	///						occurred.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		Write( const void* inBuffer, KUInt32* ioCount );

	///
	/// Flush the output buffer.
	///
	/// \throws an exception if a problem occurred.
	///
	virtual void		FlushOutput( void );

	///
	/// Get next byte without advancing the cursor.
	///
	/// \return the byte read.
	/// \throws an exception if a problem occurred (including EOF).
	///
	virtual	KUInt8		PeekByte( void );

	///
	/// Determine the position of the cursor in the stream.
	///
	/// \return the position from the start of the stream.
	/// \throws an exception if the operation isn't supported.
	///
	virtual KSInt64 	GetCursor( void ) const;

	///
	/// Move the cursor in the stream.
	/// Can throw an exception if the end of the stream was reached.
	///
	/// \param inPos	new position of the cursor.
	/// \param inMode	defines the origin of \c inPos
	/// \throws TPositionException if the end (or the beginning) of the stream
	///			was reached.
	/// \throws an exception if the operation isn't supported.
	///
	virtual void 		SetCursor( KSInt64 inPos, ECursorMode inMode );

private:
	/// \name Variables
	FILE*		mFile;				///< Pointer to the file.
	Boolean		mWeOpenedTheFile;	///< If we opened the file (and if we should
									///< close it).
};

#endif
		// _TFILESTREAM_H

// ======================================================================== //
// Our informal mission is to improve the love life of operators worldwide. //
//                 -- Peter Behrendt, president of Exabyte                  //
// ======================================================================== //
