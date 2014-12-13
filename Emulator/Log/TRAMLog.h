// ==============================
// File:			TRAMLog.h
// Project:			Einstein
//
// Copyright 2014 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TRAMLOG_H
#define _TRAMLOG_H

#include <K/Defines/KDefinitions.h>
#include "TLog.h"

#include <stdio.h>
#include <string>

///
/// Class for logging to an infinite buffer.
/// Compared to TBufferLog, this will log everything (while TBufferLog is
/// a circle buffer).
///
/// \author Paul Guyot <pguyot@kallisys.net>
///
class TRAMLog
	:
		public TLog
{
public:
	///
	/// Constructor.
	///
	TRAMLog();

	///
	/// Destructor.
	///
	virtual ~TRAMLog( void );

    ///
    /// Get the content as a C++ string (copied).
    ///
    std::string GetContent() {
        LockMutex();
        std::string result(mBuffer, mBufferSize);
        UnlockMutex();
        return result;
    }

private:
	///
	/// Log a line.
	///
	/// \param inLine	line to log.
	///
	virtual void	DoLogLine( const char* inLine );

	/// \name Variables
	char*			mBuffer;        ///< Actual buffer.
	ssize_t			mBufferSize;    ///< size of buffer.
};

#endif
		// _TRAMLOG_H
