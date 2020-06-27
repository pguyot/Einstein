// ==============================
// Fichier:			RelocHack.h
// Projet:			K
//
// CrŽŽ le:			26/06/2001
// Tabulation:		4 espaces
//
// ***** BEGIN LICENSE BLOCK *****
// Version: MPL 1.1
//
// The contents of this file are subject to the Mozilla Public License Version
// 1.1 (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
// http://www.mozilla.org/MPL/
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the
// License.
//
// The Original Code is RelocHack.h.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2001-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========

#ifndef __RELOCHACK__
#define __RELOCHACK__

#ifndef __NEWTON_H
	#include <Newton.h>
#endif

typedef void (*VTableFuncPtr)( void );
typedef void (*funcPtr)( void );

// ----------------	//
// RelocVTableHack	//
// ----------------	//

// Sur Mac: pas de RelocVTableHack.

#ifndef __surMac

// This is a big hack to fix the problem of copy of code (to avoid page fault mechanism) and virtual functions.
// Cf the technote on the subject (Virtual functions and page fault mechanism)

// Prototype for the function:

// Umm. This is what I would like to code:
// typedef void (*RelocVTableHackFuncPtr)( ULong, RelocVTableHackFuncPtr, VTableFuncPtr );
// extern void RelocVTableHack( ULong inObject, RelocVTableHackFuncPtr inRelocVTableHackPtr, VTableFuncPtr inVTablePtr );

extern "C" void RelocVTableHack( void* inObject, funcPtr inRelocVTableHackPtr, VTableFuncPtr inVTablePtr );

// You can use this template for your convenience (this way you're sure that the parameters will be passed in the correct order)
#define RelocVTable( inVTablePtr )	RelocVTableHack( (void*) this, (funcPtr) &RelocVTableHack, inVTablePtr )

#else

#define RelocVTable( inVTablePtr )
#endif

// ----------------	//
// RelocFuncPtrHack	//
// ----------------	//

// This is another hack, although less dirty :)
// It works like the previous one and is useful if you need to pass a function pointer to the copy of the function.

extern "C" funcPtr RelocFuncPtrHack( funcPtr inRelocFuncPtrHack, funcPtr inFuncPtr );

// You can use a template in this case, too (beware, you'll have to cast the result to a func pointer)
#define RelocFuncPtr( inFuncPtr )	RelocFuncPtrHack( (funcPtr) &RelocFuncPtrHack, (funcPtr) inFuncPtr )

#endif
		// __RELOCHACK__

// ====================================================================== //
// Thus spake the master programmer:                                      //
//         "Without the wind, the grass does not move.  Without software, //
//         hardware is useless."                                          //
//                 -- Geoffrey James, "The Tao of Programming"            //
// ====================================================================== //