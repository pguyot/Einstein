// ==============================
// File:			gfx/gfx_stubs.cp
// Project:			Albert
//
// Copyright 2003-2008 by Matthias Melcher (albert@matthiasm.com).
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


#include "gfx_stubs.h"


namespace Albert {

  
TROMPatch pGetInkWordFontSize(0x0014003c, GetInkWordFontSizeStub);

JITInstructionProto(GetInkWordFontSizeStub)
{
	// copy all register values into variables
	Fixed a = (Fixed)ioCPU->mCurrentRegisters[0];
  
	// call Albert
	Fixed result = GetInkWordFontSize(a);
  
	// copy variables back into registers
	ioCPU->mCurrentRegisters[0] = (KUInt32)result;
  
	// return for linked branch
	KUInt32 next = ioCPU->mCurrentRegisters[14]+4;
	MMUCALLNEXT(next);
}
  
  
} // namespace
