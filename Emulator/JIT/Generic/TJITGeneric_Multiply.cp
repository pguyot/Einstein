// ==============================
// File:			TJITGeneric_Multiply.cp
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

/* MATT: optimization
 
 Here is a tiny insight for some possible optimisations on ARM targets:
 
 This is our C++ function:
 
 JITUnit* multiply_0_1_2_3(JITUnit* ioUnit, TARMProcessor* ioCPU) {
  const KUInt32 theResult = ioCPU->mCurrentRegisters[3] * ioCPU->mCurrentRegisters[2];
  ioCPU->mCurrentRegisters[1] = theResult;
 }

 The gcc compiler generates this code (in optimized mode)

 __Z16Multiply_0_1_2_3P7JITUnitP13TARMProcessor:
 push	{r7, lr}        ; not needed, we quit this function by jumping to the next one
 add	r7, sp, #0      ; not needed, we have no stack frame
 mov	r2, r0          ; store arg0 in r2 to free r0 as a working register
 ldr	r3, [r1, #12]   ; read ioCPU->mRegister[3] (#12 = 3 * sizeof(regsiter) with mRegister being the first memebr of TARMProcessro)
 ldr	r0, [r1, #8]    ; read ioCPU->mRegister[2]
 mul	r3, r3, r0      ; multiply into r3
 adds	r0, r2, #4      ; prepare the return value, the address of the next ioUnit
 str	r3, [r1, #4]    ; store the result of the multiplication in ioCPU->mRegister[2]
 ldr	r3, [r2, #4]    ; get the jump address of the next JIT instruction
 blx	r3              ; could be the cheaper "bx r3"
 pop	{r7, pc}        ; not needed, we quit this function by jumping to the next one
 
 So yes, we can hand-optimize, and in this particular example, we would probably 
 double the execution speed. The shortest I found goes from 11 to 8 instructions,
 dropping five expensive stack operations, leaving only four memory reads and 
 writes. By not using the stack frame at all, we avoid data cache misses, and we 
 do not build up a call stack that needs to be unwound later, avoiding instruction
 cache misses. 
 
 On the bad side, this is a lot of work, applies only to a single CPU/compiler 
 combination, and will break badly if any of the classes involved ever change.
 
 All in all, this optimization may be a larger win than it seems 
 at first.
 
 mov	r2, r0          ; store arg0 in r2 to free r0 as a working register
 ldr	r3, [r1, #12]   ; read ioCPU->mRegister[3] (#12 = 3 * sizeof(regsiter) with mRegister being the first memebr of TARMProcessro)
 ldr	r0, [r1, #8]    ; read ioCPU->mRegister[2]
 mul	r3, r3, r0      ; multiply into r3
 adds	r0, r2, #4      ; prepare the return value, the address of the next ioUnit
 str	r3, [r1, #4]    ; store the result of the multiplication in ioCPU->mRegister[2]
 ldr	r3, [r2, #4]    ; get the jump address of the next JIT instruction
 bx	r3              ; jump right away to the next emulated instruction
 
 But how do we use this? Well, we can hand-optimize every template for very 
 specific CPU/compiler combinations. gcc provides some helpers:
 
 JITUnit* multiply_0_1_2_3(JITUnit* ioUnit, TARMProcessor* ioCPU) 
    __attribute__((naked))
 {
    asm("ldr r3, [r1, #12]");
    ...
    asm("bx r3");
 }
 
 */
 
#include <K/Defines/KDefinitions.h>
#include <K/Tests/KDebug.h>
#include "JIT.h"

// Einstein
#include "TARMProcessor.h"

#include "TJITGeneric_Macros.h"
#include "TJITGeneric_Other.h"

#define IMPLEMENTATION	1
#include "TJITGeneric_Multiply_template.t"
#undef IMPLEMENTATION

static JITFuncPtr Multiply_Funcs[] = {
#define TRANSLATION_ARRAY	1
#include "TJITGeneric_Multiply_template.t"
#undef TRANSLATION_ARRAY
};

void
Translate_Multiply(
					JITPageClass* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// Get the index.
	const KUInt32 flag_s = (inInstruction & 0x00100000) >> 20;
	const KUInt32 Rd = (inInstruction & 0x000F0000) >> 16;
	const KUInt32 Rs = (inInstruction & 0x00000F00) >> 8;
	const KUInt32 Rm = inInstruction & 0x0000000F;
	if ((Rd == 15) || (Rs == 15) || (Rm == 15))
	{
		PUSHFUNC(UndefinedInstruction);
		PUSHVALUE(inVAddr + 8);
	} else {
		const KUInt32 theIndex =
			(((((flag_s * 15) + Rd) * 15) + Rs) * 15) + Rm;
		PUSHFUNC(Multiply_Funcs[theIndex]);
	}
}

void TJITGeneric_Multiply_assertions( void );

void TJITGeneric_Multiply_assertions( void )
{
	// Check that the array has the expected size.
    KCOMPILE_TIME_ASSERT_SIZE(
    	Multiply_Funcs, 2 * 15 * 15 * 15 * sizeof(void*) );
}

// =============================== //
// 1: No code table for op: ++post //
// =============================== //
