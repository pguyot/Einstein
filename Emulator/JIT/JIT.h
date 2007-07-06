// ==============================
// File:			JIT.h
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

#ifndef _JIT_H
#define _JIT_H

#include <K/Defines/KDefinitions.h>

// Auto-set the JITTARGET.
#ifndef JITTARGET
	#if defined (TARGET_RT_MAC_MACHO) && TARGET_RT_MAC_MACHO
		#define JITTARGET PPCMACHO
	#endif
#endif

// Includes the proper header depending on the platform and define the JIT
// class accordingly.
#if JITTARGET == ARMLE
	#include "TJITARMLE.h"
	#define JITClass		TJITARMLE
	#define JITPageClass	TJITARMLEPage
#else
	#include "TJITGeneric.h"
	#define	JITClass		TJITGeneric
	#define	JITPageClass	TJITGenericPage
#endif

#endif
		// _JIT_H

// ============================================================================== //
// Fortune suggests uses for YOUR favorite UNIX commands!                         //
//                                                                                //
// Try:                                                                           //
//         ar t "God"                                                             //
//         drink < bottle; opener                  (Bourne Shell)                 //
//         cat "food in tin cans"                  (all but 4.[23]BSD)            //
//         Hey UNIX!  Got a match?                 (V6 or C shell)                //
//         mkdir matter; cat > matter              (Bourne Shell)                 //
//         rm God                                                                 //
//         man: Why did you get a divorce?         (C shell)                      //
//         date me                                 (anything up to 4.3BSD)        //
//         make "heads or tails of all this"                                      //
//         who is smart                                                           //
//                                                 (C shell)                      //
//         If I had a ) for every dollar of the national debt, what would I have? //
//         sleep with me                           (anything up to 4.3BSD)        //
// ============================================================================== //
