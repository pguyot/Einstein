// ==============================
// File:			gfx/gfx_stubs.h
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

#ifndef ALBERT_GFX_GFX_STUBS_H
#define ALBERT_GFX_GFX_STUBS_H

// Albert
#include "font.h"
#include "point.h"

// Einstein
#include "JIT.h"
#include "TROMImage.h"
#include "TARMProcessor.h"
#include "TJITGeneric_Macros.h"


namespace Albert {

  
  // font.cp
  JITInstructionProto(GetInkWordFontSizeStub);
  
  // point.cp
  JITInstructionProto(SetPtStub);
    
}


#endif
// ALBERT_GFX_GFX_STUBS_H

