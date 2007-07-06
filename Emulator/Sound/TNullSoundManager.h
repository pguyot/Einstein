// ==============================
// File:			TNullSoundManager.h
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

#ifndef _TNULLSOUNDMANAGER_H
#define _TNULLSOUNDMANAGER_H

#include <K/Defines/KDefinitions.h>

// Einstein
#include "TSoundManager.h"

///
/// Class to handle sound input/output and redirect them to nothingness.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 111 $
///
/// \test	aucun test défini.
///
class TNullSoundManager
	:	
		public TSoundManager
{
public:
	///
	/// Constructor from a log.
	///
	TNullSoundManager( TLog* inLog = nil );

	///
	/// Destructor.
	///
	virtual ~TNullSoundManager( void );

	///
	/// Schedule output of some buffer.
	///
	virtual void	ScheduleOutputBuffer( KUInt32 inBufferAddr, KUInt32 inSize );
	
	///
	/// Start output.
	///
	virtual void	StartOutput( void );

	///
	/// Stop output.
	///
	virtual void	StopOutput( void );

	///
	/// Is output running?
	///
	virtual Boolean	OutputIsRunning( void );

private:
	/// \name Variables
	Boolean			mOutputIsRunning;
};

#endif
		// _TNULLSOUNDMANAGER_H

// ========================================= //
// TRANSACTION CANCELLED - FARECARD RETURNED //
// ========================================= //
