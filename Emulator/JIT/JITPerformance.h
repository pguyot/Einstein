// ==============================
// File:			JITPerformance.h
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
//                     and Matthias Melcher (m.melcher@robowerk.com)
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

#ifndef _JITPERFORMANCE_H
#define _JITPERFOMRANCE_H

#include <K/Defines/KDefinitions.h>

// the following definition sets the fastest calling convention for C functions
// for a given platform

#if defined(_MSC_VER) && defined(_M_IX86)
  // Matt: after much testing, I came to the conclusion that the default
  // calling convention is actually the fastest in VisualC. Nevertheless I want
  // to leave this code in for potentially better calling conventions
  // on other compilers or platforms.
  #define FASTCALL
#else
  #define FASTCALL
#endif


#endif
		// _JITPERFORMANCE_H

// ==============================
