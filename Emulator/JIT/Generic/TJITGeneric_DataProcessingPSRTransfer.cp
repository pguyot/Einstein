// ==============================
// File:			TJITGeneric_DataProcessingPSRTransfer.cp
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

#include <K/Defines/KDefinitions.h>
#include "JIT.h"

// Einstein
#include "TARMProcessor.h"

#include "TJITGeneric_Macros.h"
#include "RepeatMacros.h"

#include "TJITGeneric_DataProcessingPSRTransfer.h"

#include "TJITGeneric_DataProcessingPSRTransfer_LogicalOp.h"
#include "TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp.h"
#include "TJITGeneric_DataProcessingPSRTransfer_TestOp.h"
#include "TJITGeneric_Other.h"
#include "TJITGeneric_DataProcessingPSRTransfer_MoveOp.h"
#include "TJITGeneric_DataProcessingPSRTransfer_MSR.h"
#include "TJITGeneric_DataProcessingPSRTransfer_MRS.h"

// MODE (4), FLAG_S (2), Rn * 16 + Rd (256)
#define LogicalOp_ArrayEntry(op, mode, flag_s, rn, rd) \
	LogicalOp_TemplateName(op, mode, flag_s, rn, rd),
#define def_array_mode_s_rn_rd(op) \
static const JITFuncPtr DataProcessingPSRTransfer_ ## op ## _Funcs[] = {	\
	repeat_256p4(LogicalOp_ArrayEntry, op, Regular, 0)						\
	repeat_256p4(LogicalOp_ArrayEntry, op, Regular, 1)						\
	repeat_256p4(LogicalOp_ArrayEntry, op, Imm, 0)							\
	repeat_256p4(LogicalOp_ArrayEntry, op, Imm, 1)							\
	repeat_256p4(LogicalOp_ArrayEntry, op, ImmC, 0)							\
	repeat_256p4(LogicalOp_ArrayEntry, op, ImmC, 1)							\
	repeat_256p4(LogicalOp_ArrayEntry, op, NoShift, 0)						\
	repeat_256p4(LogicalOp_ArrayEntry, op, NoShift, 1)						\
}

// MODE(4), Rd (16)
#define mode_s_rd_ArrayEntry(op, mode, flag_s, rd) \
	MoveOp_TemplateName(op, mode, flag_s, rd),
#define def_array_mode_s_rd(op) \
static const JITFuncPtr DataProcessingPSRTransfer_ ## op ## _Funcs[] = {	\
	repeat_16p4(mode_s_rd_ArrayEntry, op, Regular, 0)						\
	repeat_16p4(mode_s_rd_ArrayEntry, op, Regular, 1)						\
	repeat_16p4(mode_s_rd_ArrayEntry, op, Imm, 0)							\
	repeat_16p4(mode_s_rd_ArrayEntry, op, Imm, 1)							\
	repeat_16p4(mode_s_rd_ArrayEntry, op, ImmC, 0)							\
	repeat_16p4(mode_s_rd_ArrayEntry, op, ImmC, 1)							\
	repeat_16p4(mode_s_rd_ArrayEntry, op, NoShift, 0)						\
	repeat_16p4(mode_s_rd_ArrayEntry, op, NoShift, 1)						\
}

#define ShouldPushPC_TestOp(mode, rn) \
	(((mode != Imm) && (mode != ImmC)) || (rn == 15))
#define ShouldPushPC_MoveOp(mode) \
	((mode != Imm) && (mode != ImmC))
#define ShouldPushPC_LogicalOp(mode, rn) \
	(((mode != Imm) && (mode != ImmC)) || (rn == 15))
#define ShouldPushPC_ArithmeticOp(mode, rn) \
	(((mode != Imm) && (mode != ImmC)) || (rn == 15))
#define ShouldPushPC_MSR_NoShift(rm)	\
	(rm == 15)


// MODE (4), Rn (16)
#define TestOp_ArrayEntry(op, mode, rn) \
	TestOp_TemplateName(op, mode, rn),
#define def_array_testop_mode_rn(op) \
static const JITFuncPtr DataProcessingPSRTransfer_ ## op ## _Funcs[] = {	\
	repeat_16p3(TestOp_ArrayEntry, op, Regular)								\
	repeat_16p3(TestOp_ArrayEntry, op, Imm)									\
	repeat_16p3(TestOp_ArrayEntry, op, ImmC)									\
	repeat_16p3(TestOp_ArrayEntry, op, NoShift)								\
}

// FLAG_R (2), Rd (16)
#define MRS_ArrayEntry(flag_r, rd) \
	MRS_TemplateName(flag_r, rd),
static const JITFuncPtr DataProcessingPSRTransfer_MRS_Funcs[] = {
	repeat_16p2(MRS_ArrayEntry, 0)
	repeat_16p2(MRS_ArrayEntry, 1)
};
// MODE (2), FLAG_R (2), FIELDS_MASK(16), Rd (16)
#define MSR_ArrayEntry(mode, flag_r, fields, rm) \
	MSR_TemplateName(mode, flag_r, fields, rm),
static const JITFuncPtr DataProcessingPSRTransfer_MSR_NoShift_Funcs[] = {
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0x00000000)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0x000000FF)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0x0000FF00)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0x0000FFFF)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0x00FF0000)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0x00FF00FF)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0x00FFFF00)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0x00FFFFFF)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0xFF000000)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0xFF0000FF)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0xFF00FF00)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0xFF00FFFF)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0xFFFF0000)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0xFFFF00FF)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0xFFFFFF00)
	repeat_16p4(MSR_ArrayEntry, 0, 0, 0xFFFFFFFF)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0x00000000)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0x000000FF)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0x0000FF00)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0x0000FFFF)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0x00FF0000)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0x00FF00FF)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0x00FFFF00)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0x00FFFFFF)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0xFF000000)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0xFF0000FF)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0xFF00FF00)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0xFF00FFFF)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0xFFFF0000)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0xFFFF00FF)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0xFFFFFF00)
	repeat_16p4(MSR_ArrayEntry, 0, 1, 0xFFFFFFFF)
};
static const JITFuncPtr DataProcessingPSRTransfer_MSR_Imm_Funcs[] = {
	MSR_TemplateName(1, 0, 0x00000000, Rm),
	MSR_TemplateName(1, 0, 0x000000FF, Rm),
	MSR_TemplateName(1, 0, 0x0000FF00, Rm),
	MSR_TemplateName(1, 0, 0x0000FFFF, Rm),
	MSR_TemplateName(1, 0, 0x00FF0000, Rm),
	MSR_TemplateName(1, 0, 0x00FF00FF, Rm),
	MSR_TemplateName(1, 0, 0x00FFFF00, Rm),
	MSR_TemplateName(1, 0, 0x00FFFFFF, Rm),
	MSR_TemplateName(1, 0, 0xFF000000, Rm),
	MSR_TemplateName(1, 0, 0xFF0000FF, Rm),
	MSR_TemplateName(1, 0, 0xFF00FF00, Rm),
	MSR_TemplateName(1, 0, 0xFF00FFFF, Rm),
	MSR_TemplateName(1, 0, 0xFFFF0000, Rm),
	MSR_TemplateName(1, 0, 0xFFFF00FF, Rm),
	MSR_TemplateName(1, 0, 0xFFFFFF00, Rm),
	MSR_TemplateName(1, 0, 0xFFFFFFFF, Rm),
	MSR_TemplateName(1, 1, 0x00000000, Rm),
	MSR_TemplateName(1, 1, 0x000000FF, Rm),
	MSR_TemplateName(1, 1, 0x0000FF00, Rm),
	MSR_TemplateName(1, 1, 0x0000FFFF, Rm),
	MSR_TemplateName(1, 1, 0x00FF0000, Rm),
	MSR_TemplateName(1, 1, 0x00FF00FF, Rm),
	MSR_TemplateName(1, 1, 0x00FFFF00, Rm),
	MSR_TemplateName(1, 1, 0x00FFFFFF, Rm),
	MSR_TemplateName(1, 1, 0xFF000000, Rm),
	MSR_TemplateName(1, 1, 0xFF0000FF, Rm),
	MSR_TemplateName(1, 1, 0xFF00FF00, Rm),
	MSR_TemplateName(1, 1, 0xFF00FFFF, Rm),
	MSR_TemplateName(1, 1, 0xFFFF0000, Rm),
	MSR_TemplateName(1, 1, 0xFFFF00FF, Rm),
	MSR_TemplateName(1, 1, 0xFFFFFF00, Rm),
	MSR_TemplateName(1, 1, 0xFFFFFFFF, Rm),
};

def_array_mode_s_rn_rd(AND);
#define AND_Func(mode, flag_s, rnrd) \
	DataProcessingPSRTransfer_AND_Funcs[((mode) * 512) + ((flag_s) * 256) + (rnrd)]
def_array_mode_s_rn_rd(EOR);
#define EOR_Func(mode, flag_s, rnrd) \
	DataProcessingPSRTransfer_EOR_Funcs[((mode) * 512) + ((flag_s) * 256) + (rnrd)]
def_array_mode_s_rn_rd(SUB);
#define SUB_Func(mode, flag_s, rnrd) \
	DataProcessingPSRTransfer_SUB_Funcs[((mode) * 512) + ((flag_s) * 256) + (rnrd)]
def_array_mode_s_rn_rd(RSB);
#define RSB_Func(mode, flag_s, rnrd) \
	DataProcessingPSRTransfer_RSB_Funcs[((mode) * 512) + ((flag_s) * 256) + (rnrd)]
def_array_mode_s_rn_rd(ADD);
#define ADD_Func(mode, flag_s, rnrd) \
	DataProcessingPSRTransfer_ADD_Funcs[((mode) * 512) + ((flag_s) * 256) + (rnrd)]
def_array_mode_s_rn_rd(ADC);
#define ADC_Func(mode, flag_s, rnrd) \
	DataProcessingPSRTransfer_ADC_Funcs[((mode) * 512) + ((flag_s) * 256) + (rnrd)]
def_array_mode_s_rn_rd(SBC);
#define SBC_Func(mode, flag_s, rnrd) \
	DataProcessingPSRTransfer_SBC_Funcs[((mode) * 512) + ((flag_s) * 256) + (rnrd)]
def_array_mode_s_rn_rd(RSC);
#define RSC_Func(mode, flag_s, rnrd) \
	DataProcessingPSRTransfer_RSC_Funcs[((mode) * 512) + ((flag_s) * 256) + (rnrd)]
def_array_testop_mode_rn(TST);
#define TST_Func(mode, rn) \
	DataProcessingPSRTransfer_TST_Funcs[((mode) * 16) + (rn)]
def_array_testop_mode_rn(TEQ);
#define TEQ_Func(mode, rn) \
	DataProcessingPSRTransfer_TEQ_Funcs[((mode) * 16) + (rn)]
def_array_testop_mode_rn(CMP);
#define CMP_Func(mode, rn) \
	DataProcessingPSRTransfer_CMP_Funcs[((mode) * 16) + (rn)]
def_array_testop_mode_rn(CMN);
#define CMN_Func(mode, rn) \
	DataProcessingPSRTransfer_CMN_Funcs[((mode) * 16) + (rn)]
def_array_mode_s_rn_rd(ORR);
#define ORR_Func(mode, flag_s, rnrd) \
	DataProcessingPSRTransfer_ORR_Funcs[((mode) * 512) + ((flag_s) * 256) + (rnrd)]
def_array_mode_s_rd(MOV);
#define MOV_Func(mode, flag_s, rd) \
	DataProcessingPSRTransfer_MOV_Funcs[((mode) * 32) + ((flag_s) * 16) + (rd)]
def_array_mode_s_rn_rd(BIC);
#define BIC_Func(mode, flag_s, rnrd) \
	DataProcessingPSRTransfer_BIC_Funcs[((mode) * 512) + ((flag_s) * 256) + (rnrd)]
def_array_mode_s_rd(MVN);
#define MVN_Func(mode, flag_s, rd) \
	DataProcessingPSRTransfer_MVN_Funcs[((mode) * 32) + ((flag_s) * 16) + (rd)]

#define MRS_Func(flag_r, rd) \
	DataProcessingPSRTransfer_MRS_Funcs[(flag_r * 16) + (rd)]
#define MSR_NoShift_Func(flag_r, fields_mask_ix, rm) \
	DataProcessingPSRTransfer_MSR_NoShift_Funcs[((flag_r) * 256) + ((fields_mask_ix) * 16) + (rm)]
#define MSR_Imm_Func(flag_r, fields_mask_ix) \
	DataProcessingPSRTransfer_MSR_Imm_Funcs[((flag_r) * 16) + (fields_mask_ix)]

#define __Rn	(inInstruction & 0x000F0000) >> 16
#define __Rd	(inInstruction & 0x0000F000) >> 12
#define __RnRd	(inInstruction & 0x000FF000) >> 12
#define __Rm	(inInstruction & 0x0000000F)

void
Translate_DataProcessingPSRTransfer(
					JITPageClass* inPage,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	const Boolean theFlagS = (inInstruction & 0x00100000) != 0;
	KUInt32 theMode;
	KUInt32 thePushedValue;
	Boolean doPush = true;
	Boolean doPushPC = false;
	if (inInstruction & 0x02000000)
	{
		KUInt32 theImmValue = inInstruction & 0xFF;
		KUInt32 theRotateAmount = ((inInstruction >> 8) & 0xF) * 2;
		if (theRotateAmount != 0)
		{
			theImmValue = 
				(theImmValue >> theRotateAmount)
				| (theImmValue << (32 - theRotateAmount));
			if (theFlagS)
			{
				theMode = ImmC;
			} else {
				theMode = Imm;
			}
		} else {
			theMode = Imm;
		}
		thePushedValue = theImmValue;
	} else if ((inInstruction & 0x00000FFF) >> 4) {
		theMode = Regular;
		thePushedValue = inInstruction;
	} else {
		theMode = NoShift;
		thePushedValue = __Rm;
	}

	switch ((inInstruction & 0x01E00000) >> 21)
	{
	case 0x0:	// 0b0000
				// AND
		PUSHFUNC(AND_Func(theMode, theFlagS, __RnRd));
		doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
		break;

	case 0x1:	// 0b0001
				// EOR
		PUSHFUNC(EOR_Func(theMode, theFlagS, __RnRd));
		doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
		break;

	case 0x2:	// 0b0010
				// SUB
		PUSHFUNC(SUB_Func(theMode, theFlagS, __RnRd));
		doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
		break;

	case 0x3:	// 0b0011
				// RSB
		PUSHFUNC(RSB_Func(theMode, theFlagS, __RnRd));
		doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
		break;

	case 0x4:	// 0b01000
				// ADD
		PUSHFUNC(ADD_Func(theMode, theFlagS, __RnRd));
		doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
		break;

	case 0x5:	// 0b01010
				// ADC
		PUSHFUNC(ADC_Func(theMode, theFlagS, __RnRd));
		doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
		break;

	case 0x6:	// 0b01100
				// SBC
		PUSHFUNC(SBC_Func(theMode, theFlagS, __RnRd));
		doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
		break;

	case 0x7:	// 0b0111
				// RSC
		PUSHFUNC(RSC_Func(theMode, theFlagS, __RnRd));
		doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
		break;

	case 0x8:	// 0b1000
				// MRS (CPSR) & TST
		if (theFlagS == 0)
		{
			if (theMode != NoShift)
			{
				// Undefined Instruction (there is no MRS with Imm bit set or low bits set)
				PUSHFUNC(UndefinedInstruction);
				doPush = false;
				doPushPC = true;
			} else {
				PUSHFUNC(MRS_Func(0, __Rd));
				doPush = false;
			}
		} else {
			PUSHFUNC(TST_Func(theMode, __Rn));
			doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
		}
		break;

	case 0x9:	// 0b1001
				// MSR (CPSR) & TEQ
		if (theFlagS == 0)
		{
			if (theMode == Regular)
			{
				// Software breakpoint
				KUInt16 theID = inInstruction & 0x0000000F;
				theID |= (inInstruction & 0x000FFF00) >> 4;
				thePushedValue = theID;
				PUSHFUNC(SoftwareBreakpoint);
				doPushPC = true;
			} else {
				if (theMode == NoShift)
				{
					PUSHFUNC(MSR_NoShift_Func(0, (inInstruction & 0x000F0000) >> 16, __Rm));
					doPushPC = ShouldPushPC_MSR_NoShift(__Rm);
					doPush = false;
				} else {
					PUSHFUNC(MSR_Imm_Func(0, (inInstruction & 0x000F0000) >> 16));
					doPushPC = false;
				}
			}
		} else {
			PUSHFUNC(TEQ_Func(theMode, __Rn));
			doPushPC = ShouldPushPC_TestOp(theMode, __Rn);
		}
		break;

	case 0xA:	// 0b1010
				// MRS (SPSR) & CMP
		if (theFlagS == 0)
		{
			if (theMode != NoShift)
			{
				// Undefined Instruction (there is no MRS with Imm bit set or low bits set)
				PUSHFUNC(UndefinedInstruction);
				doPush = false;
				doPushPC = true;
			} else {
				PUSHFUNC(MRS_Func(1, __Rd));
				doPush = false;
			}
		} else {
			PUSHFUNC(CMP_Func(theMode, __Rn));
			doPushPC = ShouldPushPC_TestOp(theMode, __Rn);
		}
		break;

	case 0xB:	// 0b1011
				// MSR (SPSR) & CMN
		if (theFlagS == 0)
		{
			if (theMode == Regular)
			{
				// Undefined Instruction (there is no MSR with shift)
				PUSHFUNC(UndefinedInstruction);
				doPush = false;
				doPushPC = true;
			} else {
				if (theMode == NoShift)
				{
					PUSHFUNC(MSR_NoShift_Func(1, (inInstruction & 0x000F0000) >> 16, __Rm));
					doPush = false;
				} else {
					PUSHFUNC(MSR_Imm_Func(1, (inInstruction & 0x000F0000) >> 16));
				}
			}
		} else {
			PUSHFUNC(CMN_Func(theMode, __Rn));
			doPushPC = ShouldPushPC_TestOp(theMode, __Rn);
		}
		break;

	case 0xC:	// 0b1100
				// ORR
		PUSHFUNC(ORR_Func(theMode, theFlagS, __RnRd));
		doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
		break;

	case 0xD:	// 0b11010
				// MOV
		PUSHFUNC(MOV_Func(theMode, theFlagS, __Rd));
		doPushPC = ShouldPushPC_MoveOp(theMode);
		break;

	case 0xE:	// 0b1110
				// BIC
		PUSHFUNC(BIC_Func(theMode, theFlagS, __RnRd));
		doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
		break;

	case 0xF:	// 0b11110
				// MVN
		PUSHFUNC(MVN_Func(theMode, theFlagS, __Rd));
		doPushPC = ShouldPushPC_MoveOp(theMode);
		break;
	}
	
	if (doPush)
	{
		PUSHVALUE(thePushedValue);
	}
	if (doPushPC)
	{
		PUSHVALUE(inVAddr + 8);
	}
}

// ====================================================================== //
// Established technology tends to persist in the face of new technology. //
//                 -- G. Blaauw, one of the designers of System 360       //
// ====================================================================== //
