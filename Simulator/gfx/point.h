// ==============================
// File:                        gfx/point.h
// Project:                     Simulator
//
// Copyright 2003-2008 by Matthias Melcher (newton@matthiasm.com).
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

#ifndef SIMULATOR_GFX_POINT_H
#define SIMULATOR_GFX_POINT_H

#include <K/Defines/KDefinitions.h>
#include <Simulator/sys/types.h>


namespace Simulator {

  // TODO: this is bad because a Point is actaully a KUInt32 containing
  // two unsigned 16 bit values... see Quickdraw
  // But this function is not called anyways, or is it?
  class Point {
    KSInt16 left_;
    KSInt16 top_;
  public:
    void left(KSInt16 x);
    KSInt32 left();
    void top(KSInt16 y);
    KSInt32 top();    
    
  };
  
  extern void SetPt(Point *, KSInt32 x, KSInt32 y);
  

} // namespace


#endif
// SIMULATOR_GFX_POINT_H

