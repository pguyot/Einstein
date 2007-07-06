// ==============================
// File:			TAIFROMImageWithREXes.h
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

#ifndef _TAIFROMIMAGEWITHREXES_H
#define _TAIFROMIMAGEWITHREXES_H

#include <K/Defines/KDefinitions.h>

#include "TROMImage.h"

///
/// Class for a ROM Image in an AIF file (debug images) with two rex files
/// (rex 0 and Einstein's rex).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TAIFROMImageWithREXes
	:
		public TROMImage
{
public:
	///
	/// Constructor from three file paths.
	///
	/// \param inAIFPath	path to the AIF file.
	/// \param inREX0Path	path to the REX0 file.
	/// \param inREX1Path	path to the REX1 file.
	///
	TAIFROMImageWithREXes(
					const char* inROMPath,
					const char* inREX0Path,
					const char* inREX1Path,
					const char inMachineString[6],
					Boolean inMonitorMode = false );

	///
	/// Destructeur.
	///
	~TAIFROMImageWithREXes( void );
};

#endif
		// _TAIFROMIMAGEWITHREXES_H

// ====================================================================== //
// Real computer scientists don't program in assembler.  They don't write //
// in anything less portable than a number two pencil.                    //
// ====================================================================== //
