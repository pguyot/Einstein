// ==============================
// File:                        objects/TDoubleQContainer.h
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

#ifndef SIMULATOR_OBJECTS_TDOUBLEQCONTAINER_H
#define SIMULATOR_OBJECTS_TDOUBLEQCONTAINER_H

#include <K/Defines/KDefinitions.h>
#include <Simulator/sys/types.h>
#include <Simulator/sys/einstein.h>
#include <Simulator/sys/macros.h>


namespace Simulator {

  class TDoubleQItem;
  
  class TDoubleQContainer
  {
  public:
    //CDoubleQContainer();
    //CDoubleQContainer(ULong inOffsetToDoubleQItem);
    //CDoubleQContainer(ULong inOffsetToDoubleQItem, DestructorProcPtr, void*);
      
    //void		add(void * inItem);
    //void		addBefore(void * inItem, void * inBeforeItem);
    //void		addToFront(void * inItem);
    //void		checkBeforeAdd(void * inItem);
    
    //void *	peek(void);
    //void *	getNext(void * inItem);
    
    //void *	remove(void);
    BOOL      RemoveFromQueue(void *item);
    BOOL		  DeleteFromQueue(void *inItem);
      
  private:
    //void		init(ULong inOffsetToDoubleQItem);
      
    SIMULATOR_CLASS_MEMBER( TDoubleQItem*, Head );                           // +00
    SIMULATOR_CLASS_MEMBER( TDoubleQItem*, Tail );                           // +04
    SIMULATOR_CLASS_MEMBER( ULong, OffsetToDoubleQItem);                     // +08
    SIMULATOR_CLASS_MEMBER( DestructorProcPtr, Destructor);                  // +0C
    SIMULATOR_CLASS_MEMBER( void*, DestructorInstance);                      // +10
  };
  
} // namespace


#endif
// SIMULATOR_OBJECTS_TDOUBLEQCONTAINER_H

