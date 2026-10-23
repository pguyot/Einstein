// ==============================
// File:			TFLFileLock
// Project:			Einstein
//
// Copyright 2026 by Matthias Melcher.
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

#ifndef T_FL_FILE_LOCK_H
#define T_FL_FILE_LOCK_H

#include <K/Defines/KDefinitions.h>

#include <string>

/**
 \brief An exclusive lock on a file, shared by all Einstein processes.

 Several Einstein processes may run at the same time, one per machine. A lock
 file tells the others that a machine runs, that the Settings are open, or
 that a PC Card image is in use. The operating system releases the lock when
 the process ends, also after a crash, so a lock never stays behind.

 macOS and Linux use flock(), Windows uses LockFileEx() on one byte far behind
 the end of the file, so the lock does not keep others from reading it.
 */
class TFLFileLock
{
public:
	explicit TFLFileLock(std::string inPath);
	~TFLFileLock();

	// Try to get the lock, without waiting. Creates the file if needed.
	// \return true if this object holds the lock now
	bool TryLock();

	// Release the lock (the file stays).
	void Unlock();

	bool
	IsLocked() const
	{
		return mLocked;
	}

	const std::string&
	GetPath() const
	{
		return mPath;
	}

	// Is the lock on this file held by another process (or another object)?
	// Does not create the file.
	static bool IsHeldElsewhere(const std::string& inPath);

private:
	TFLFileLock(const TFLFileLock&) = delete;
	TFLFileLock& operator=(const TFLFileLock&) = delete;

	std::string mPath;
	bool mLocked { false };
#if TARGET_OS_WIN32
	void* mHandle { nullptr };
#else
	int mFile { -1 };
#endif
};

#endif // T_FL_FILE_LOCK_H
