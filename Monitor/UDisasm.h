// ==============================
// File:			UDisasm.h
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

#ifndef _UDISASM_H
#define _UDISASM_H

#include <K/Defines/KDefinitions.h>

class TSymbolList;

///
/// Class for a disassembler.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 111 $
///
/// \test	aucun test défini.
///
class UDisasm
{
public:
	///
	/// Disassemble one line.
	///
	static void Disasm(
					char* outString,
					int inStringSize,
					KUInt32 inAddr,
					KUInt32 inInstruction,
					TSymbolList* inSymbolList=0L );
	
	///
	/// make symbol list access easy
	///
	static void setSymbolList(TSymbolList*);
	
private:
	static TSymbolList *pSymbolList;
};

#endif
		// _UDISASM_H

// ============================================================================ //
// Windows Airlines:                                                            //
// The terminal is very neat and clean, the attendants all very attractive, the //
// pilots very capable. The fleet of Learjets the carrier operates is immense.  //
// Your jet takes off without a hitch, pushing above the clouds, and at 20,000  //
// feet it explodes without warning.                                            //
// ============================================================================ //
