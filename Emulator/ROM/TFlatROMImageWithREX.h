// ==============================
// File:			TFlatROMImageWithREX.h
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

#ifndef _TFLATROMIMAGEWITHREX_H
#define _TFLATROMIMAGEWITHREX_H

#include <K/Defines/KDefinitions.h>

#include "TROMImage.h"

///
/// Class for a Flat ROM Image with an additional REX put at +8MB.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TFlatROMImageWithREX
	:
		public TROMImage
{
public:
	///
	/// Constructor from two file paths.
	///
	/// \param inROMPath	path to the ROM file.
	/// \param inREXPath	path to the REX file.
	/// \param inMachineString	machine string.
	///
	TFlatROMImageWithREX(const char* inROMPath,
					const char* inREXPath);

	///
	/// Destructor.
	///
	~TFlatROMImageWithREX( void );
};

#endif
		// _TFLATROMIMAGEWITHREX_H

// ================================ //
// One picture is worth 128K words. //
// ================================ //
