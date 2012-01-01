// ==============================
// File:                        gfx/point.cp
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


#include <Simulator/gfx/point.h>
#include <Simulator/sys/einstein.h>


namespace Simulator {

  
  void Point::left(KSInt16 l)
  {
    setMem8(((KUInt32)this)+0, l>>8);
    setMem8(((KUInt32)this)+1, l);
  }
  
  KSInt32 Point::left()
  {
    KSInt32 ret = getMem8(((KUInt32)this)+0)<<8;
    ret        |= getMem8(((KUInt32)this)+1);
    return ret;
  }
  
  void Point::top(KSInt16 t)
  {
    setMem8(((KUInt32)this)+2, t>>8);
    setMem8(((KUInt32)this)+3, t);
  }
  
  KSInt32 Point::top()
  {
    KSInt32 ret = getMem8(((KUInt32)this)+2)<<8;
    ret        |= getMem8(((KUInt32)this)+3);
    return ret;
  }
  
  void SetPt(Point *pt, KSInt32 l, KSInt32 t)
  {
    // printf("SetPt(%d, %d)\n", l, t);
    pt->left(l);
    pt->top(t);
  }

  
}

