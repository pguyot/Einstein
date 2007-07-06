// ==============================
// File:			TFileLog.h
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

#ifndef _TFILELOG_H
#define _TFILELOG_H

#include <K/Defines/KDefinitions.h>
#include "TLog.h"

#include <stdio.h>

///
/// Class for logging to a file.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 111 $
///
/// \test	aucun test défini.
///
class TFileLog
	:
		public TLog
{
public:
	///
	/// Constructor from a file path.
	/// The file is created and the log appended to it.
	///
	TFileLog( const char* inFilePath );

	///
	/// Destructor.
	///
	virtual ~TFileLog( void );

private:
	///
	/// Log a line.
	///
	/// \param inLine	line to log.
	///
	virtual void	DoLogLine( const char* inLine );

	/// \name Variables
	FILE*			mFile; ///< Reference to the file.
};

#endif
		// _TFILELOG_H

// ========================================================================= //
// Real software engineers don't like the idea of some inexplicable and      //
// greasy hardware several aisles away that may stop working at any          //
// moment.  They have a great distrust of hardware people, and wish that     //
// systems could be virtual at *___all* levels.  They would like personal //
// computers (you know no one's going to trip over something and kill your   //
// DFA in mid-transit), except that they need 8 megabytes to run their       //
// Correctness Verification Aid packages.                                    //
// ========================================================================= //
