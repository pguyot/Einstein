// ==============================
// File:			TBufferLog.h
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TBUFFERLOG_H
#define _TBUFFERLOG_H

#include <K/Defines/KDefinitions.h>
#include "Emulator/Log/TLog.h"

#include <stdio.h>

///
/// Class for a log in a buffer (used for the monitor).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 111 $
///
/// \test	aucun test défini.
///
class TBufferLog
	:
		public TLog
{
public:
	///
	/// Default constructor.
	///
	TBufferLog( void );

	///
	/// Destructor.
	///
	virtual ~TBufferLog( void );

	///
	/// Accessor on a line.
	/// The index starts at the top (latest line is line #18).
	///
	/// \param inIndex	index of the line to return.
	/// \return the ith line.
	///
	inline const char*	GetLine( int inIndex ) const
		{
			return mBuffer[(mTopLineIndex + inIndex) % 32];
		}

	///
	/// Open the log file (for writing).
	///
	/// \param inPath   path to the log file.
	///
	void			OpenLog( const char* inPath );
	
	///
	/// Close the log file.
	///
	void			CloseLog( void );
	
	///
	/// Bind the log with a socket (for screen refreshes)
	///
	void			BindWithRefreshSocket( int inRefreshSocket )
		{
			mRefreshSocket = inRefreshSocket;
		}
	
private:
	///
	/// Log a line.
	///
	/// \param inLine	line to log.
	///
	virtual void	DoLogLine( const char* inLine );

	/// \name Variables
	KUInt32			mTopLineIndex;		///< Index of the top line.
	char			mBuffer[32][80];	///< Buffer: 18 lines of 80 characters.
	FILE*			mLogFile;			///< Log file or NULL.
	int				mRefreshSocket;		///< Socket to write to for refreshes.
};

#endif
		// _TBUFFERLOG_H

// ====================================== //
// Logic doesn't apply to the real world. //
//                 -- Marvin Minsky       //
// ====================================== //
