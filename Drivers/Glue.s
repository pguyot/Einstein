//  ==============================
// File:			Glue.s
// Project:			Einstein
//
// Copyright 2003-2013, 2002 by Paul Guyot (pguyot@kallisys.net).
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

// Glue for various platforms.
// We currently only support three platforms.

    .macro ROM_FUNC name, addr717006, addr737041, addr747129
    .section ".text.\name\()", "ax"
    .global \name
\name\():
    .balign 4
	mov			r12, sp
	stmdb		sp!, {r0-r3,lr}		// save registers and lr
	bl			_ZN5UGlue34DetermineMachineTypeWithoutGestaltEv
	teq			r0, #3				// 717006
	ldmeqia		sp!, {r0-r3,lr}		// restore registers & lr.
	ldreq		pc, label_to_addr717006_\name
	teq			r0, #4				// 737041
	ldmeqia		sp!, {r0-r3,lr}		// restore registers & lr.
	ldreq		pc, label_to_addr737041_\name
	teq			r0, #7				// 747129
	ldmeqia		sp!, {r0-r3,lr}		// restore registers & lr.
	ldreq		pc, label_to_addr747129_\name
	ldmia		sp!, {r0-r3,pc}		// restore registers & pc.
label_to_addr717006_\name\():
	.word		\addr717006
label_to_addr737041_\name\():
	.word		\addr737041
label_to_addr747129_\name\():
	.word		\addr747129
    .endm

	// Functions in the (private) jump table.
	ROM_FUNC _Z12GetPowerPortv, 0x01C0E778, 0x01BEF7A4, 0x01B36B64
	ROM_FUNC _ZN14TGPIOInterface17RegisterInterruptEhPvPFvS0_Em, 0x01A6D6F0, 0x01A6D6F0, 0x01A6D6FC
	ROM_FUNC _ZN14TGPIOInterface15EnableInterruptEP13GPIOIntObject, 0x01A6D6F8, 0x01A6D6F8, 0x01A6E704

	ROM_FUNC _Z19GetDMAManagerObjectv, 0x01A6A51C, 0x01A6A51C, 0x01A6A528
	ROM_FUNC _ZN11TDMAManager17RequestAssignmentEmP21TDMAChannelDiscriptor, 0x01A694A4, 0x01A694A4, 0x01A694B0

	// Functions not in the jump table
	// Actually, these are NONVIRTUAL methods of PSoundDriver interface.
	ROM_FUNC _ZN12PSoundDriver26OutputIntHandlerDispatcherEv, 0x001E60FC, 0x001C943C, 0x001E8514
	ROM_FUNC _ZN12PSoundDriver25InputIntHandlerDispatcherEv, 0x001E6130, 0x001C9470, 0x001E8548

// ======================================================================= //
// Programmers used to batch environments may find it hard to live without //
// giant listings; we would find it hard to use them.                      //
//                 -- D.M. Ritchie                                         //
// ======================================================================= //
