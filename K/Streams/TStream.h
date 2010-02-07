// ==============================
// Fichier:			TStream.h
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			10/6/2005
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
// The Original Code is TStream.h.
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
// $Id: TStream.h,v 1.2 2006/01/18 09:00:13 pguyot Exp $
// ===========

#ifndef _TSTREAM_H
#define _TSTREAM_H

#include <K/Defines/KDefinitions.h>
#include <K/Defines/UByteSex.h>

///
/// Class for an object to write and/or read bytes.
/// This class also handles endianness conversions transparently.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.2 $
///
/// \test	aucun test défini.
///
class TStream
{
public:
	///
	/// Destructor.
	///
	virtual ~TStream( void ) {}

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
	virtual	void		Read( void* outBuffer, KUInt32* ioCount ) = 0;

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
	virtual	void		Write( const void* inBuffer, KUInt32* ioCount ) = 0;

	///
	/// Flush the output buffer.
	///
	/// \throws an exception if a problem occurred.
	///
	virtual void		FlushOutput( void ) = 0;

	///
	/// Get next byte without advancing the cursor.
	///
	/// \return the byte read.
	/// \throws an exception if a problem occurred (including EOF).
	///
	virtual	KUInt8		PeekByte( void ) = 0;

	/// \name Utility functions
	
	///
	/// Read n bytes in C format. A null character is added at the end.
	///
	/// The string is allocated with malloc(3).
	///
	/// \param	inNbChars	number of characters to read.
	/// \return the string, allocated with malloc(3).
	/// \throws an exception if a problem occurred (including EOF).
	///
	virtual	KUInt8*		GetCString( KUInt32 inNbChars );

	///
	/// Read 8 bits characters and stop once a null byte is encountered.
	/// Put this null character at the end of the string which is returned.	
	///
	/// The string is allocated with malloc(3).
	///
	/// \return the string, allocated with malloc(3).
	/// \throws an exception if a problem occurred (including EOF).
	///
	virtual	KUInt8*		GetCString( void );

	///
	/// Write a 8 bits character string (in C format) terminated by a null
	/// character.
	///
	/// \param inString	the string to write.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		PutCString( const KUInt8* inString );

	///
	/// Write a 8 bits character string (in C format), without the terminating
	/// null character.
	///
	/// \param inString	the string to write.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		PutString( const char* inString );

	///
	/// Read a unicode (UTF-8) string, terminated by a null character.
	///
	/// The string is allocated with malloc(3).
	///
	/// \return the string is allocated with malloc(3).
	/// \throws an exception if a problem occurred (including EOF).
	///
	virtual	KUInt16*	GetUniString( void );

	///
	/// Write a unicode (UTF-8) string, terminated by a null character.
	///
	/// \param inString	the string to write.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		PutUniString( const KUInt16* inString );

	///
	/// Read a 32 bits word (in native byte order).
	///
	/// \return the word that was read.
	/// \throws an exception if a problem occurred (including EOF).
	///
	virtual	KUInt32		GetInt32( void );

	///
	/// Read a 32 bits word (in big endian).
	///
	/// \return the word that was read.
	/// \throws an exception if a problem occurred (including EOF).
	///
	KUInt32				GetInt32BE( void )
		{
			return UByteSex_ToLittleEndian( GetInt32() );
		}

	///
	/// Read a 32 bits word (in little endian).
	///
	/// \return the word that was read.
	/// \throws an exception if a problem occurred (including EOF).
	///
	KUInt32				GetInt32LE( void )
		{
			return UByteSex_FromLittleEndian( GetInt32() );
		}

	///
	/// Write a 32 bits word (in native byte order).
	///
	/// \param	inWord	the word to read.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		PutInt32( const KUInt32 inWord );

	///
	/// Write a 32 bits word (in big endian).
	///
	/// \param	inWord	the word to read.
	/// \throws an exception if a problem occurred.
	///
	void				PutInt32BE( const KUInt32 inWord )
		{
			PutInt32( UByteSex_ToBigEndian( inWord ) );
		}

	///
	/// Write a 32 bits word (in little endian).
	///
	/// \param	inWord	the word to read.
	/// \throws an exception if a problem occurred.
	///
	void				PutInt32LE( const KUInt32 inWord )
		{
			PutInt32( UByteSex_ToLittleEndian( inWord ) );
		}

	///
	/// Read a 32 bits word in the XLong format.
	/// If the word is between 0 and 254, it occupies one byte.
	/// Otherwise, it's 0xFF followed by the word in big endian (5 bytes).
	///
	/// \return the word that was read.
	/// \throws an exception if a problem occurred (including EOF).
	///
	virtual	KUInt32		GetXLong( void );

	///
	/// Write a 32 bits word in the XLong format.
	/// If the word is between 0 and 254, it occupies one byte.
	/// Otherwise, it's 0xFF followed by the word in big endian (5 bytes).
	///
	/// \param	inLong	the word to write.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		PutXLong( const KUInt32 inLong );

	///
	/// Read a 16 bits word (in native byte order).
	///
	/// \return the word that was read.
	/// \throws an exception if a problem occurred (including EOF).
	///
	virtual	KUInt16		GetInt16( void );

	///
	/// Read a 16 bits word (in big endian).
	///
	/// \return the word that was read.
	/// \throws an exception if a problem occurred (including EOF).
	///
	KUInt16				GetInt16BE( void )
		{
			return UByteSex_FromBigEndian( GetInt16() );
		}

	///
	/// Read a 16 bits word (in little endian).
	///
	/// \return the word that was read.
	/// \throws an exception if a problem occurred (including EOF).
	///
	KUInt16				GetInt16LE( void )
		{
			return UByteSex_FromLittleEndian( GetInt16() );
		}

	///
	/// Write a 16 bits word (in native byte order).
	///
	/// \param	inShort	the word to write.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		PutInt16( const KUInt16 inShort );

	///
	/// Write a 16 bits word (in big endian).
	///
	/// \param	inShort	the word to write.
	/// \throws an exception if a problem occurred.
	///
	void				PutInt16BE( const KUInt16 inShort )
		{
			PutInt16( UByteSex_ToBigEndian( inShort ) );
		}

	///
	/// Write a 16 bits word (in little endian).
	///
	/// \param	inShort	the word to write.
	/// \throws an exception if a problem occurred.
	///
	void				PutInt16LE( const KUInt16 inShort )
		{
			PutInt16( UByteSex_ToLittleEndian( inShort ) );
		}

	///
	/// Read an array of 32 bits words (in big endian).
	///
	/// \param	inArray	the buffer where to put the data.
	/// \param	inCount	the number of words to read.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		GetInt32ArrayBE(
							KUInt32* inArray,
							const KUInt32 inCount );

	///
	/// Read an array of 32 bits words (in little endian).
	///
	/// \param	inArray	the buffer where to put the data.
	/// \param	inCount	the number of words to read.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		GetInt32ArrayLE(
							KUInt32* inArray,
							const KUInt32 inCount );

	///
	/// Write an array of 32 bits words (in big endian).
	///
	/// \param	inArray	the array to write.
	/// \param	inCount	the number of words to write.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		PutInt32ArrayBE(
							const KUInt32* inArray,
							const KUInt32 inCount );

	///
	/// Write an array of 32 bits words (in little endian).
	///
	/// \param	inArray	the array to write.
	/// \param	inCount	the number of words to write.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		PutInt32ArrayLE(
							const KUInt32* inArray,
							const KUInt32 inCount );

	///
	/// Read a byte.
	///
	/// \return the byte read.
	/// \throws an exception if a problem occurred (including EOF).
	///
	virtual	KUInt8		GetByte( void );

	///
	/// Write a byte.
	///
	/// \param	inByte	the byte to write.
	/// \throws an exception if a problem occurred.
	///
	virtual	void		PutByte( const KUInt8 inByte );
};

#endif
		// _TSTREAM_H

// ========================================================================== //
// There is is no reason for any individual to have a computer in their home. //
//                 -- Ken Olsen (President of Digital Equipment Corporation), //
//                    Convention of the World Future Society, in Boston, 1977 //
// ========================================================================== //
