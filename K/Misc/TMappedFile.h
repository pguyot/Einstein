// ==============================
// Fichier:			TMappedFile.h
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			14/2/2005
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
// The Original Code is TMappedFile.h.
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
// $Id: TMappedFile.h,v 1.3 2005/06/07 05:53:31 pguyot Exp $
// ===========

#ifndef _TMAPPEDFILE_H
#define _TMAPPEDFILE_H

#include <K/Defines/KDefinitions.h>

// ANSI C & POSIX
#include <sys/types.h>
#include <fcntl.h>

///
/// Class for a file read entirely (with mmap if available).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.3 $
///
/// \test	aucun test défini.
///
class TMappedFile
{
public:
	///
	/// Constructor from a path and a boolean to determine if the file
	/// could be written.
	///
	/// \param inFilePath	path to the file.
	/// \param inSize		amount of the file mapped (also used when the file
	///						is created), 0 means map all the file.
	/// \param inFlags		flags for the file (passed to open(2),
	///						if O_CREAT is set, the file is created with inSize
	///						bytes, O_RDONLY/O_WRONLY/O_RDWR determine how to
	///						map the file.
	/// \param preferredAddress	address where to map the file, if possible.
	///
	TMappedFile(
			const char* inFilePath,
			size_t inSize = 0,
			int inFlags = O_RDONLY,
			void* preferredAddress = NULL );

	///
	/// Destructor.
	/// Close the file (write it if mmap failed).
	///
	~TMappedFile( void );
	
	///
	/// Accessor on the file buffer pointer, NULL if a problem occurred.
	///
	/// \return the buffer.
	///
	void*	GetBuffer( void )
		{
			return mBuffer;
		}
	
	///
	/// Accessor on the size of the file.
	///
	/// \return the size of the file.
	///
	size_t	GetSize( void ) const
		{
			return mSize;
		}
	
	///
	/// Determine whether the file was created or grown.
	///
	/// \return \c true if the file was created/grown, \c false otherwise.
	///
	Boolean	GetCreated( void ) const
		{
			return mCreated;
		}

	///
	/// Sync the whole region (flush changes to disk).
	///
	void	Sync( void ) const;

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMappedFile( const TMappedFile& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMappedFile& operator = ( const TMappedFile& inCopy );

	/// \name Variables
	void*		mBuffer;	///< Buffer to access the file.
	size_t		mSize;		///< Size of the file.
	Boolean		mMapped;	///< If file is actually mapped.
	Boolean		mReadOnly;	///< If file is read only.
	Boolean		mCreated;	///< Whether the file was created/grown.
	int			mFileFd;	///< fd to the file.
};

#endif
		// _TMAPPEDFILE_H

// ============================================================= //
// You are in a maze of little twisting passages, all different. //
// ============================================================= //
