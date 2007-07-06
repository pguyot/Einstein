// ==============================
// File:			TBufferedSoundManager.h
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

#ifndef _TBUFFEREDSOUNDMANAGER_H
#define _TBUFFEREDSOUNDMANAGER_H

#include <K/Defines/KDefinitions.h>

#include "TSoundManager.h"

///
/// Class for buffering sound.
/// Useful on little endian hosts.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
class TBufferedSoundManager
	:
		public TSoundManager
{
public:
	///
	/// Constructor from a log.
	///
	/// \param inLog				log interface (can be null)
	///
	TBufferedSoundManager(TLog* inLog = nil);

	///
	/// Destructor.
	///
	virtual ~TBufferedSoundManager( void );

	///
	/// Schedule output of some buffer (using address).
	///
	virtual void	ScheduleOutputBuffer( KUInt32 inBufferAddr, KUInt32 inSize );

	///
	/// Schedule output of some buffer.
	///
	virtual void	ScheduleOutput( const KUInt8* inBuffer, KUInt32 inSize ) = 0;

private:
	/// \name Variables
	KUInt8*			mBuffer;	///< Buffer (for little endian hosts).
};

#endif
		// _TBUFFEREDSOUNDMANAGER_H

// ========================================= //
// You are in the hall of the mountain king. //
// ========================================= //
