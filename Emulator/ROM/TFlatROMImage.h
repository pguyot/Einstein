// ==============================
// File:			TFlatROMImage.h
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

#ifndef _TFLATROMIMAGE_H
#define _TFLATROMIMAGE_H

#include <K/Defines/KDefinitions.h>

#include "TROMImage.h"

///
/// Class for a 8/16 MB ROM Dump.
/// If the file is 8 MB, it is duplicated.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TFlatROMImage
	:
		public TROMImage
{
public:
	///
	/// Constructor from a file path and a machine string.
	///
	/// \param inROMPath		path to the ROM file.
	/// \param inMachineString	machine string.
	///
	TFlatROMImage(
		const char* inROMPath,
		const char inMachineString[6],
		Boolean inMonitorMode = false );

	///
	/// Destructor.
	///
	~TFlatROMImage( void );
};

#endif
		// _TFLATROMIMAGE_H

// =============================================================== //
// The system was down for backups from 5am to 10am last Saturday. //
// =============================================================== //
