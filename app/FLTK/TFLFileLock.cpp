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

#include <K/Defines/KDefinitions.h>

#include "TFLFileLock.h"

#include <utility>

#if TARGET_OS_WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#endif

#if TARGET_OS_WIN32
// Lock one byte at 4 GB, so the lock does not keep anyone from reading the
// file (Windows locks are mandatory for the bytes they cover).
static void
SetLockOffset(OVERLAPPED& outOverlapped)
{
	ZeroMemory(&outOverlapped, sizeof(outOverlapped));
	outOverlapped.Offset = 0;
	outOverlapped.OffsetHigh = 1;
}
#endif

TFLFileLock::TFLFileLock(std::string inPath) :
		mPath(std::move(inPath))
{
}

TFLFileLock::~TFLFileLock()
{
	Unlock();
}

bool
TFLFileLock::TryLock()
{
	if (mLocked)
		return true;
#if TARGET_OS_WIN32
	HANDLE theFile = CreateFileA(mPath.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, nullptr);
	if (theFile == INVALID_HANDLE_VALUE)
		return false;
	OVERLAPPED theOverlapped;
	SetLockOffset(theOverlapped);
	if (!LockFileEx(theFile, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, 1, 0, &theOverlapped))
	{
		CloseHandle(theFile);
		return false;
	}
	mHandle = theFile;
#else
	int theFile = open(mPath.c_str(), O_RDWR | O_CREAT, 0644);
	if (theFile == -1)
		return false;
	if (flock(theFile, LOCK_EX | LOCK_NB) != 0)
	{
		close(theFile);
		return false;
	}
	mFile = theFile;
#endif
	mLocked = true;
	return true;
}

void
TFLFileLock::Unlock()
{
	if (!mLocked)
		return;
#if TARGET_OS_WIN32
	OVERLAPPED theOverlapped;
	SetLockOffset(theOverlapped);
	UnlockFileEx((HANDLE) mHandle, 0, 1, 0, &theOverlapped);
	CloseHandle((HANDLE) mHandle);
	mHandle = nullptr;
#else
	flock(mFile, LOCK_UN);
	close(mFile);
	mFile = -1;
#endif
	mLocked = false;
}

bool
TFLFileLock::IsHeldElsewhere(const std::string& inPath)
{
#if TARGET_OS_WIN32
	HANDLE theFile = CreateFileA(inPath.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, nullptr);
	if (theFile == INVALID_HANDLE_VALUE)
		return false; // no lock file, nobody holds it
	OVERLAPPED theOverlapped;
	SetLockOffset(theOverlapped);
	bool isHeld = !LockFileEx(theFile, LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY, 0, 1, 0, &theOverlapped);
	if (!isHeld)
		UnlockFileEx(theFile, 0, 1, 0, &theOverlapped);
	CloseHandle(theFile);
	return isHeld;
#else
	int theFile = open(inPath.c_str(), O_RDWR);
	if (theFile == -1)
		return false; // no lock file, nobody holds it
	bool isHeld = (flock(theFile, LOCK_EX | LOCK_NB) != 0);
	if (!isHeld)
		flock(theFile, LOCK_UN);
	close(theFile);
	return isHeld;
#endif
}
