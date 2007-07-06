;  ==============================
;  File:			JITPPCMachO.s
;  Project:			Einstein
; 
;  Copyright 2004-2007 by Paul Guyot (pguyot@kallisys.net).
; 
;  This program is free software; you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation; either version 2 of the License, or
;  (at your option) any later version.
; 
;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
; 
;  You should have received a copy of the GNU General Public License along
;  with this program; if not, write to the Free Software Foundation, Inc.,
;  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
;  ==============================
;  $Id$
;  ==============================

linkageArea:	set 24		# size in PowerPC environment
params:			set 32		# callee parameter area
localVars:		set 0		# callee local variables
numGPRs:		set 0		# volatile GPRs used by callee
numFPRs:		set 0		# volatile FPRs used by callee)

spaceToSave:	set linkageArea + params + localVars
spaceToSave:	set spaceToSave + 4*numGPRs + 8*numFPRs

.enterJIT:
	mflr	r0						# extract return address
	stw		r0,8(SP)				# save it to stack
	stwu	SP,-spaceToSave(SP)

.exitJIT:
    lwz		r0,spaceToSave(SP)+8	# get the return address
    mtlr	r0						#   into the Link Register
    addic	sp,sp,spaceToSave		# restore stack pointer
    blr								# and branch to the return address

; ========================================================================
; Coding is easy;  All you do is sit staring at a terminal until the drops
; of blood form on your forehead.                                         
; ========================================================================
