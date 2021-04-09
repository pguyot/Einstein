// ==============================
// File:			TSnapshotFile.cpp
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

#include "Emulator/Files/TSnapshotFile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <cstring>
#include <string>

#if TARGET_OS_WIN32
#include <io.h>
#else
#include <sys/uio.h>
#include <unistd.h>
#endif

/**
 * \class TSnapshotFile
 *
 * Snapshot files come in two flavours. Minimal snapshots store links to files where
 * possible (ROM/Flash/PCMCIA). By storing the modification date of files, snapshots
 * can only be restored if none of the dependent files have been touched. Minimal
 * snapshots are used to store the machine state when EInstein shuts down, and allow
 * instant restart of NewtonOS when Einstein is launched again, avoiding a 
 * lengthy NewtonOS boot sequence.
 *
 * Snapshot files start with a \a header followed by the \a body. The header contains
 * enough information to determine if the body of the snapshot is compatible to the current
 * Einstein version and driver configuration, or if reading should be aborted now,
 * and instead Einstein should launch safely by rebooting NewtonOS.
 *
 * Not yet implemented: 
 * Full snapshots can be created and restored by the user. They store a complete
 * image of all NewtonOS resources at the given point in time. They can be used 
 * to restore older states of the OS, or to restore machine states across
 * multiple Einstein host machines. They are also great for debugging.  
 */


 /**
  * Throw this exception if anything goes wrong when reading a snapshot file.
  * 
  * \param inDescription must point to a static text, explaining to the user
  *		why a snapshot file could not be loaded.
  */
TSnapshotException::TSnapshotException(const char* inDescription)
	: mDescription(inDescription)
{
}

/**
 * Create or open a file for saving or restoring a snapshot of the current
 * Einstein machine state.
 * 
 * \param inFilename a filname for the snapshot file that will be opened with \a fopen
 * \param inMode the file mode as in \a fopen, usually "rb" or "wb".
 * \param inType select a minimal or full snapshot
 * \param inVersion if writing a file, set this version number
 *		if reading, trigger an exception if the file version is newer than inVersion 
 * \throw TIOException if file access failed
 * \throw TEOFException if the file ends unexpectedly
 * \throw TMemError if an allocation failed
 */
TSnapshotFile::TSnapshotFile(
	const char* inFilename,
	const char* inMode,
	TSnapshotType inType,
	KUInt32 inVersion
)
:	TFileStream(inFilename, inMode),
	mType(inType)
{

	if (IsWriting()) {
		Version(inVersion);
		PutInt32BE('EINI');
		PutInt32BE('SNAP');
		PutInt32BE(Version());
	} else {
		KUInt32 id = GetInt32BE();
		if (id != 'EINI')
			throw TSnapshotException("File type not recognized");

		KUInt32 type = GetInt32BE();
		if (type != 'SNAP') 
			throw TSnapshotException("File content not recognized");

		Version(GetInt32BE());
		if (Version()<inVersion) 
			throw TSnapshotException("Incompatible file version");
	}
}

/**
 * Flush and close the file and return all resources.
 */
TSnapshotFile::~TSnapshotFile()
{

}


/**
 * Write the filepath, followed by the last modification date of the given file.
 * If no filename exists, we write an emty string.
 * If the file does not exist or isn't readable, the modification time will be 0.
 * 
 * \param[in] inFilepath the full name and path to the file in utf-8
 */
void TSnapshotFile::PutFileRef(const char* inFilepath)
{
	struct stat theInfos = { };

	if (inFilepath) {
		PutInt32BE('FILE');
		PutCString((const KUInt8*)inFilepath); // utf8 ok.
		int err = ::stat(inFilepath, &theInfos);
		if (err != -1) {
			PutInt32BE('TIME');
		} else {
			PutInt32BE('NULL');
		}
	} else {
		PutInt32BE('NULL');
		PutCString((const KUInt8*)""); // empty string.
		PutInt32BE('NULL');
	}
	Transfer(theInfos.st_mtime);
}

/**
 * Read a filepath and the last modification data from the stream.
 *
 * \param[in] inFilepath the full name and path to the file in utf-8
 *
 * \return true, if file content is the same for the reference and the current file
 * \return false, if the filenames don't match (if both filenaes are empty, they match)
 * \return false, if the file modification date can't be read
 * \return false, if the stored modfication date differs from the current modification date
 */
Boolean TSnapshotFile::VerifyFileRef(const char* inFilepath)
{
	struct stat oldInfos = { };
	struct stat newInfos = { };
	KUInt32 sizeOfTimeT = sizeof(newInfos.st_mtime);

	// Read all data from the stream
	GetInt32BE(); // Marker: FILE: filename follows, NULL: no filename was given, empty string follows
	char *tmp = (char*)GetCString();
	std::string oldFilepath = tmp;
	::free(tmp);
	GetInt32BE(); // Marker: TIME: file modification follows, NULL: file modification date invalid
	Transfer(oldInfos.st_mtime);

	// Compare the file paths, return false if they don't match
	if (!inFilepath) {
		if (strcmp(oldFilepath.c_str(), "") != 0)
			return false;
		else
			// if both filepaths are empty, we assum we reference the same internal data structure
			return true;
	} else {
		if (strcmp(oldFilepath.c_str(), inFilepath) != 0)
			return false;
	}

	// Compare the last modification time, return false if they don't match
	int err = ::stat(inFilepath, &newInfos);
	if (err == -1)
		// if we can't get the mod date, the file may no longer exist
		return false;

	// modfication times differ
	// Note: Calling the destructor may generate a new snapshot, hence a new modification date.
	//       As a kludge, we allow modification dates to match within a few seconds.
	if (newInfos.st_mtime-oldInfos.st_mtime>5)
		return false;

	return true;
}

