// ==============================
// File:			TSnapshotFile.h
// Project:			Einstein
//
// Copyright 2021, Matthias Melcher.
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

#ifndef _T_SNAPSHOT_FILE_H_
#define _T_SNAPSHOT_FILE_H_

#include <K/Streams/TFileStream.h>

#include <exception>


/**
 * \brief TSnapshotFile storage type.
 * \see TSnaphotFile
 */
enum class TSnapshotType
{
	kMinimal,	/// Store RAM content and references to the ROM and Flash file content
	kFull		/// Store a full image of the NetwonOS state
};


/**
 * \brief Thrown if snapshot is not compatible or can;t be read.
 */
class TSnapshotException : public std::exception
{
public:
	/**
	 * \brief Create a snapshot exception with a descriptive text.
	 */
	TSnapshotException(const char* inDescription);

	/**
	 * \brief Return a text describing the details of this exception.
	 */
	const char* GetDescription() { return mDescription; }

protected:
	const char* mDescription = nullptr;
};


/**
 * \brief Read or write a snapshot of the current state of the emulator.
 */
class TSnapshotFile : public TFileStream
{
public:
	/** 
	 * \brief Constructor using fopen style parameters
	 */
	TSnapshotFile(
		const char *inFilename, 
		const char *inMode,
		TSnapshotType inType,
		KUInt32 inVersion);
	
	/** 
	 * \brief Destructor, also flushes and closes the stream.
	 */
	~TSnapshotFile() override;

	/**
	 * \brief Return true if we are reading or writing a 'minimal' snapshot.
	 */
	Boolean IsMinimal() { return (mType == TSnapshotType::kMinimal); }

	/**
	 * \brief Return true if we are reading or writing a 'full' snapshot.
	 */
	Boolean IsFull() { return (mType == TSnapshotType::kMinimal); }

	/**
	 * \brief Write the filename and path, and the last modification date of the file.
	 */
	void PutFileRef(const char* inFilepath);

	/**
	 * \brief Read a filename and path, and return true, if it references the same file.
	 */
	Boolean VerifyFileRef(const char* inFilepath);



protected:
	TSnapshotType	mType = TSnapshotType::kMinimal;
};

#endif // _T_SNAPSHOT_FILE_H_