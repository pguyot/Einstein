// ==============================
// File:                        objects/TDoubleQItem.h
// Project:                     Albert
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

#ifndef ALBERT_OBJECTS_TDOUBLEQITEM_H
#define ALBERT_OBJECTS_TDOUBLEQITEM_H

#include <K/Defines/KDefinitions.h>
#include <Albert/sys/types.h>
#include <Albert/sys/einstein.h>
#include <Albert/sys/macros.h>


namespace Albert {

  class TDoubleQContainer;
  
  class TDoubleQItem
  {
  public:
    ALBERT_CLASS_MEMBER( TDoubleQItem*, Next );                           // +00
    ALBERT_CLASS_MEMBER( TDoubleQItem*, Prev );                           // +04
    ALBERT_CLASS_MEMBER( TDoubleQContainer*, Container );                 // +08
  };
  

} // namespace


#endif
// ALBERT_OBJECTS_TDOUBLEQITEM_H

