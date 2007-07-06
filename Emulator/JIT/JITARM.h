// ==============================
// File:			JITARMLinux.h
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

#ifndef _JITARMLINUX_H
#define _JITARMLINUX_H

// JIT for ARM/Linux
// Do not include this header directly. Include JIT.h instead.

class TJITARMLinux
	:
		public TJIT
{
public:
	///
	/// Function to translate an instruction and replace the unit.
	///
	/// \param ioObject			processor object.
	/// \param inPAddr			physical address of the instruction.
	/// \param ioUnit			unit to translate.
	///
	static JITUnit* Translate(
					JITUnit* ioUnit,
					TJITGeneric* inJITObject,
					TARMProcessor* ioObject );
	
};

#endif
		// _JITARMLINUX_H

// ============================================================== //
// As of next Thursday, UNIX will be flushed in favor of TOPS-10. //
// Please update your programs.                                   //
// ============================================================== //
