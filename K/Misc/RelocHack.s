// ==============================
// Fichier:			RelocHack.s
// Projet:			K
//
// Créé le:			26/06/2001
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
// The Original Code is RelocHack.a.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2001-2004, 2020
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========

// ======================================================================================================
// extern void RelocVTableHack( ULong inObject, ULong inRelocVTableHackPtr, VTableFuncPtr inVTablePtr );
// ======================================================================================================
// Function to relocate the VTable.
// Usage (in the constructor)
//	RelocVTableHack( (ULong) this, (ULong) &RelocVTableHack, &<VTABLE> );
// where <VTABLE> is the symbol of the VTable. (__VTABLE__ & size of class name & class name)

    .arch armv3
    .balign 4

    .section ".text.RelocVTableHack", "ax"
    .global RelocVTableHack
RelocVTableHack:	
	sub		r3, pc, #8		// pc is this line + 8
	add		r2, r2, r3		// r2 is now inVTablePtr + &thisFunc
	sub		r1, r2, r1		// r1 is now inVTablePtr + &thisFunc - inRelocVTableHackPtr
	str		r1, [r0, #0]	// store the pointer of the copy of the vtable
	mov		pc, lr			// return

// ======================================================================================================
// extern ULong RelocFuncPtrHack( ULong inRelocFuncPtrHack, ULong inFuncPtr );
// ======================================================================================================
// Function to relocate a func pointer.
// Usage:
//	ProcPtr myFuncPtr = (ProcPtr) RelocFuncPtrHack( (ULong) &RelocFuncPtrHack, (ULong) &myFunction );

    .section ".text.RelocFuncPtrHack", "ax"
    .global RelocFuncPtrHack
RelocFuncPtrHack:
	sub		r2, pc, #8		// pc is this line + 8
	add		r1, r1, r2		// r1 is now inFuncPtr + &thisFunc
	sub		r0, r1, r0		// r0 is now inFuncPtr + &thisFunc - inRelocFuncPtrHack
							//	this is the result
	mov		pc, lr			// return

// ==================================================================
// CCI Power 6/40: one board, a megabyte of cache, and an attitude...
// ==================================================================
