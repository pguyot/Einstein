// ==============================
// File:			TJITGenericRetarget.cp
// Project:			Einstein
//
// Copyright 2014 by Matthias Melcher (einstein@matthiasm.com).
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
#include "TJITGenericRetarget.h"

// Einstein
#include "TARMProcessor.h"
#include "TMemory.h"
#include "TSymbolList.h"

#include "TJITGeneric_Macros.h"

#include "TJITGeneric_Test.h"
#include "TJITGeneric_Other.h"
#include "TJITGeneric_DataProcessingPSRTransfer.h"
#include "TJITGeneric_SingleDataTransfer.h"
#include "TJITGeneric_SingleDataSwap.h"
#include "TJITGeneric_Multiply.h"
#include "TJITGeneric_MultiplyAndAccumulate.h"
#include "TJITGeneric_BlockDataTransfer.h"
#include "TJITGenericRetargetMap.h"

#include "TROMImage.h"
#include "UDisasm.h"



const KUInt32 AND = 0;
const KUInt32 EOR = 1;
const KUInt32 SUB = 2;
const KUInt32 RSB = 3;
const KUInt32 ADD = 4;
const KUInt32 ADC = 5;
const KUInt32 SBC = 6;
const KUInt32 RSC = 7;
const KUInt32 TST = 8;
const KUInt32 TEQ = 9;
const KUInt32 CMP = 10;
const KUInt32 CMN = 11;
const KUInt32 ORR = 12;
const KUInt32 MOV = 13;
const KUInt32 BIC = 14;
const KUInt32 MVN = 15;

const KUInt32 Regular = 0;
const KUInt32 Imm     = 1;
const KUInt32 ImmC    = 2;
const KUInt32 NoShift = 3;


TJITGenericRetarget::TJITGenericRetarget(TMemory *inMemory, TSymbolList *inSymbolList) :
	pMemory(inMemory),
	pSymbolList(inSymbolList),
	pCOut(stderr),
	pHOut(stdout)
{
}


TJITGenericRetarget::~TJITGenericRetarget()
{
}


bool TJITGenericRetarget::OpenFiles(const char *filePathAndName)
{
	const char *fileName = strrchr(filePathAndName, '/');
	if (fileName) {
		fileName++;
	} else {
		fileName = filePathAndName;
	}
	
	char buf[2048];
	snprintf(buf, 2047, "%s.cp", filePathAndName);
	pCOut = fopen(buf, "wb");
	if (!pCOut) {
		return true;
	}
	fprintf(pCOut, "/**\n * Collection of transcoded functions\n */\n\n");
	fprintf(pCOut, "#include \"%s.h\"\n\n", fileName);

	snprintf(buf, 2047, "%s.h", filePathAndName);
	pHOut = fopen(buf, "wb");
	if (!pHOut) {
		return true;
	}
	fprintf(pHOut, "/**\n * Collection of transcoded functions\n */\n\n");
	fprintf(pHOut, "#ifndef %s_H\n", fileName);
	fprintf(pHOut, "#define %s_H\n\n", fileName);
	fprintf(pHOut, "#include \"SimulatorGlue.h\"\n\n\n");
	return false;
}


void TJITGenericRetarget::CloseFiles()
{

	fprintf(pCOut, "\n\n/**\n * End of transcoded functions\n */\n");
	if (pCOut!=stderr) {
		fclose(pCOut);
		pCOut = stderr;
	}
	
	fprintf(pHOut, "\n#endif\n\n");
	fprintf(pHOut, "\n\n/**\n * End of transcoded functions\n */\n");
	if (pHOut!=stdout) {
		fclose(pHOut);
		pHOut = stdout;
	}
}


void TJITGenericRetarget::TranslateFunction(KUInt32 inFirst, KUInt32 inLast, const char *inName, bool inSafeMode)
{
	pFunctionBegin = inFirst;
	pFunctionEnd = inLast;
	fprintf(pCOut, "\n/**\n * Transcoded function %s\n * ROM: 0x%08lX - 0x%08lX\n */\n", inName, inFirst, inLast);
	if (pHOut!=stdout)
		fprintf(pHOut, "extern void Func_0x%08lX(TARMProcessor* ioCPU); // %s\n", inFirst, inName);
	fprintf(pCOut, "void Func_0x%08lX(TARMProcessor* ioCPU)\n{\n", inFirst);
	KUInt32 instr = 0;
	KUInt32 addr = inFirst;
	for ( ; addr<inLast; addr+=4) {
		pMemory->Read(addr, instr);
		switch (gJITGenericRetargetMap[addr>>2]) {
			case 1:
				Translate(addr, instr);
				break;
			case 2: // byte
			case 3: // word
			case 4: // string
			case 5:
			case 6:
			default:
			{
				char sym[512], cmt[512];
				int off;
				if (pSymbolList->GetSymbolExact(instr, sym, cmt, &off)) {
					fprintf(pCOut, "\t// KUInt32 D_%08lX = 0x%08lX; // %s\n", addr, instr, sym);
				} else {
					fprintf(pCOut, "\t// KUInt32 D_%08lX = 0x%08lX;\n", addr, instr);
				}
				break;
			}
		}
	}
	// This is a safety fallback. If no return instruction was found,
	// simply continue at the next instruction after the translated code block.
	fprintf(pCOut, "\tioCPU->mCurrentRegisters[15] = 0x%08lX + 4;\n", inLast);
	fprintf(pCOut, "}\n");
	if (inSafeMode) {
		// create a stub that only allows supervisor mode calls
		fprintf(pCOut, "T_ROM_SIMULATION(0x%08lX, \"%s\")\n\n", inFirst, inName);
	} else {
		// create a stub that will wor in user mode as well
		fprintf(pCOut, "T_ROM_PATCH(0x%08lX, \"%s\") {\n", inFirst, inName);
		fprintf(pCOut, "\tFunc_0x00120010(ioCPU);\n");
		fprintf(pCOut, "\tMMUCALLNEXT_AFTERSETPC;\n");
		fprintf(pCOut, "}\n\n\n");
	}
}


void TJITGenericRetarget::Translate(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// Make sure that injections are handled in atransparent manner
	if ((inInstruction & 0xffe00000)==0xefc00000) {
		inInstruction = TROMPatch::GetOriginalInstructionAt(inInstruction);
	} else if ((inInstruction & 0xffe00000)==0xefa00000) {
		inInstruction = TROMPatch::GetOriginalInstructionAt(inInstruction);
	}

	// Generate some crude disassmbly for comparison
	char buf[2048];
	UDisasm::Disasm(buf, 2047, inVAddr, inInstruction);
	fprintf(pCOut, "L%08X: // 0x%08X  %s\n", (unsigned int)inVAddr, (unsigned int)inInstruction, buf);
	
	// Always generate code for a condition, so we can use local variables
	int theTestKind = inInstruction >> 28;
	PutTestBegin(theTestKind);
	
	// Translate the instructionusing the original JIT source as a guideline
	if (theTestKind != kTestNV)
	{
		switch ((inInstruction >> 26) & 0x3)				// 27 & 26
		{
			case 0x0:	// 00
				DoTranslate_00(inVAddr, inInstruction);
				break;
				
			case 0x1:	// 01
				DoTranslate_01(inVAddr, inInstruction);
				break;
				
			case 0x2:	// 10
				DoTranslate_10(inVAddr, inInstruction);
				break;
				
			case 0x3:	// 11
				DoTranslate_11(inVAddr, inInstruction);
				break;
		} // switch 27 & 26
	}
	
	// End of condition
	PutTestEnd(theTestKind);
}


void TJITGenericRetarget::PutTestBegin( int inTest )
{
	// Test the condition.
	switch (inTest)
	{
			// 0000 = EQ - Z set (equal)
		case kTestEQ:
			fprintf(pCOut, "\tif (ioCPU->TestEQ()) {\n");
			break;
			
			// 0001 = NE - Z clear (not equal)
		case kTestNE:
			fprintf(pCOut, "\tif (ioCPU->TestNE()) {\n");
			break;
			
			// 0010 = CS - C set (unsigned higher or same)
		case kTestCS:
			fprintf(pCOut, "\tif (ioCPU->TestCS()) {\n");
			break;
			
			// 0011 = CC - C clear (unsigned lower)
		case kTestCC:
			fprintf(pCOut, "\tif (ioCPU->TestCC()) {\n");
			break;
			
			// 0100 = MI - N set (negative)
		case kTestMI:
			fprintf(pCOut, "\tif (ioCPU->TestMI()) {\n");
			break;
			
			// 0101 = PL - N clear (positive or zero)
		case kTestPL:
			fprintf(pCOut, "\tif (ioCPU->TestPL()) {\n");
			break;
			
			// 0110 = VS - V set (overflow)
		case kTestVS:
			fprintf(pCOut, "\tif (ioCPU->TestVS()) {\n");
			break;
			
			// 0111 = VC - V clear (no overflow)
		case kTestVC:
			fprintf(pCOut, "\tif (ioCPU->TestVC()) {\n");
			break;
			
			// 1000 = HI - C set and Z clear (unsigned higher)
		case kTestHI:
			fprintf(pCOut, "\tif (ioCPU->TestHI()) {\n");
			break;
			
			// 1001 = LS - C clear or Z set (unsigned lower or same)
		case kTestLS:
			fprintf(pCOut, "\tif (ioCPU->TestLS()) {\n");
			break;
			
			// 1010 = GE - N set and V set, or N clear and V clear (greater or equal)
		case kTestGE:
			fprintf(pCOut, "\tif (ioCPU->TestGE()) {\n");
			break;
			
			// 1011 = LT - N set and V clear, or N clear and V set (less than)
		case kTestLT:
			fprintf(pCOut, "\tif (ioCPU->TestLT()) {\n");
			break;
			
			// 1100 = GT - Z clear, and either N set and V set, or N clear and V clear (greater than)
		case kTestGT:
			fprintf(pCOut, "\tif (ioCPU->TestGT()) {\n");
			break;
			
			// 1101 = LE - Z set, or N set and V clear, or N clear and V set (less than or equal)
		case kTestLE:
			fprintf(pCOut, "\tif (ioCPU->TestLE()) {\n");
			break;
			
			// 1110 = AL - always
		case kTestAL:
			// 1111 = NV - never
		case kTestNV:
		default:
			fprintf(pCOut, "\t{\n");
			break;
	}
}


void TJITGenericRetarget::PutTestEnd( int inTest )
{
	fprintf(pCOut, "\t}\n");
}


void TJITGenericRetarget::DoTranslate_00(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  I  --Opcode--- S  --Rn--- --Rd--- ----------Operand 2-------- Data Processing PSR Transfer
	// -Cond-- 0  0  0  0  0  0  A  S  --Rd--- --Rn--- --Rs--- 1  0  0  1  --Rm--- Multiply
	// -Cond-- 0  0  0  1  0  B  0  0  --Rn--- --Rd--- 0 0 0 0 1  0  0  1  --Rm---
	if ((inInstruction & 0x020000F0) == 0x90)	// I=0 & 0b1001----
	{
		if (inInstruction & 0x01000000)
		{
			fprintf(pCOut, "#error Not yet implemented 01\n");
			// Single Data Swap
//			Translate_SingleDataSwap(
//									 this,
//									 ioUnitCrsr,
//									 inInstruction,
//									 inVAddr );
		} else {
			fprintf(pCOut, "#error Not yet implemented 02\n");
			// Multiply
//			if (inInstruction & 0x00200000)
//			{
//				Translate_MultiplyAndAccumulate(
//												this,
//												ioUnitCrsr,
//												inInstruction,
//												inVAddr);
//			} else {
//				Translate_Multiply(
//								   this,
//								   ioUnitCrsr,
//								   inInstruction,
//								   inVAddr);
//			}
		}
	} else {
		Translate_DataProcessingPSRTransfer(inVAddr, inInstruction);
	}
}


void TJITGenericRetarget::DoTranslate_01(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// Single Data Transfer & Undefined
	if ((inInstruction & 0x02000010) == 0x02000010)
	{
		if ((inInstruction&0x0FFFFFFF)==0x06000510) {
			fprintf(pCOut, "\t\tioCPU->ReturnToEmualtor(0x%08lX); // throwSystemPanic\n", inVAddr);
		} else {
			fprintf(pCOut, "#error Not yet implemented 03\n");
			// -Cond-- 0  1  1  -XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX- 1  -XXXXX-
			// DA WINNER
			//		if (inInstruction == 0xE6000510)
			//		{
			//			JITUnitBegin();
			//			PushSymbol("DebuggerUND");
			//			PushAddress(inVAddr - GetVAddr() + GetPAddr());
			//			PushAddress(inVAddr + 8);
			//			JITUnitEnd();
			//		} else {
			//			JITUnitBegin();
			//			PushSymbol("UndefinedInstruction");
			//			PushAddress(inVAddr + 8);
			//			JITUnitEnd();
			//		}
		}
	} else {
		// -Cond-- 0  1  I  P  U  B  W  L  --Rn--- --Rd--- -----------offset----------
		Translate_SingleDataTransfer(inVAddr, inInstruction);
	}
}


void TJITGenericRetarget::Translate_SingleDataTransfer(
													   KUInt32 inVAddr,
													   KUInt32 inInstruction)
{
	// special handling for reading a word form ROM
	if ( (inInstruction & 0x0fff0000) == 0x059F0000 && ((inInstruction&0x00000fff)+inVAddr+8)<0x00800000) {
		KUInt32 Rd = ((inInstruction & 0x0000F000) >> 12);
		KUInt32 theAddress = (inInstruction&0x00000fff)+inVAddr+8;
		KUInt32 theValue = 0;
		pMemory->Read(theAddress, theValue);
		char sym[512], cmt[512];
		int off;
		if (pSymbolList->GetSymbolExact(theValue, sym, cmt, &off)) {
			fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%ld] = 0x%08lX; // %s\n", Rd, theValue, sym);
		} else {
			fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%ld] = 0x%08lX;\n", Rd, theValue);
		}
	} else {
		KUInt32 Rn = ((inInstruction & 0x000F0000) >> 16);
		KUInt32 Rd = ((inInstruction & 0x0000F000) >> 12);
		KUInt32 BITS_FLAGS = ((inInstruction & 0x03F00000) >> 20);
		bool FLAG_I = ((BITS_FLAGS & 0x20) != 0);
		bool FLAG_P = ((BITS_FLAGS & 0x10) != 0);
		bool FLAG_U	= ((BITS_FLAGS & 0x08) != 0);
		bool FLAG_B	= ((BITS_FLAGS & 0x04) != 0);
		bool FLAG_W	= ((BITS_FLAGS & 0x02) != 0);
		bool FLAG_L	= ((BITS_FLAGS & 0x01) != 0);
		bool WRITEBACK = (!FLAG_P || FLAG_W);
		bool UNPRIVILEDGED = (!FLAG_P && FLAG_W);

		if (FLAG_I) {
			if ((inInstruction & 0x00000FFF) >> 4)
			{
				fprintf(pCOut, "\t\tKUInt32 offset = GetShiftNoCarryNoR15( 0x%08lX, ioCPU->mCurrentRegisters, ioCPU->mCPSR_C );\n", inInstruction);
			} else {
				fprintf(pCOut, "\t\tKUInt32 offset = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)(inInstruction & 0x0000000F));
			}
		} else {
			fprintf(pCOut, "\t\tKUInt32 offset = 0x%08X;\n", (unsigned int)(inInstruction & 0x00000FFF));
		}
		if (Rn == 15) {
			fprintf(pCOut, "\t\tKUInt32 theAddress = 0x%08X + 8", (unsigned int)(inVAddr));
		} else {
			fprintf(pCOut, "\t\tKUInt32 theAddress = ioCPU->mCurrentRegisters[%d]", (unsigned int)Rn);
		}
		
		if (FLAG_P) {
			if (FLAG_U) {
				fprintf(pCOut, " + offset");
			} else {
				fprintf(pCOut, " - offset");
			}
		}
		fprintf(pCOut, ";\n");
		
		if (UNPRIVILEDGED) {
			fprintf(pCOut, "\t\tif (ioCPU->GetMode() != TARMProcessor::kUserMode) {\n");
			fprintf(pCOut, "\t\t\ttheMemoryInterface->SetPrivilege( false );\n");
			fprintf(pCOut, "\t\t}\n");
		}
		
		if (FLAG_L) {
			if (FLAG_B){
				fprintf(pCOut, "\t\tKUInt8 theData = ioCPU->ManagedMemoryReadB(theAddress, 0x%08lX);\n", inVAddr);
			} else {
				fprintf(pCOut, "\t\tKUInt32 theData = ioCPU->ManagedMemoryRead(theAddress, 0x%08lX);\n", inVAddr);
			}
		
			if (Rd == 15) {
				if (FLAG_B) {
					fprintf(pCOut, "\t\t#error UNPREDICTABLE CASE (R15)\n");
				} else {
					fprintf(pCOut, "\t\tSETPC(theData + 4);\n");
				}
			} else {
				fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theData;\n", (unsigned int)Rd);
			}
		} else {
			if (Rd == 15) {
				fprintf(pCOut, "\t\tKUInt32 theValue = 0x%08lX + 8;\n", inVAddr);
			} else {
				fprintf(pCOut, "\t\tKUInt32 theValue = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rd);
			}
		
			if (FLAG_B) {
				fprintf(pCOut, "\t\tioCPU->ManagedMemoryWriteB(theAddress, (KUInt8)(theValue & 0xFF), 0x%08lX);\n", inVAddr);
			} else {
				fprintf(pCOut, "\t\tioCPU->ManagedMemoryWrite(theAddress, theValue, 0x%08lX);\n", inVAddr);
			}
		}
		
		if (WRITEBACK) {
			if (Rn == 15) {
				fprintf(pCOut, "\t\t#error UNPREDICTABLE CASE (R15)\n");
			} else {
				if (!FLAG_P) {
					if (FLAG_U) {
						fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theAddress + offset;\n", (unsigned int)Rn);
					} else {
						fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theAddress - offset;\n", (unsigned int)Rn);
					}
				} else {
					fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theAddress;\n", (unsigned int)Rn);
				}
			}
		}
		
		if (UNPRIVILEDGED) {
			fprintf(pCOut, "\t\tif (ioCPU->GetMode() != TARMProcessor::kUserMode) {\n");
			fprintf(pCOut, "\t\t\ttheMemoryInterface->SetPrivilege( true );\n");
			fprintf(pCOut, "\t\t}\n");
		}
		
		if ((Rd == 15) && FLAG_L) {
			fprintf(pCOut, "\t\treturn;\n");
		}
	}
}


void TJITGenericRetarget::Translate_DataProcessingPSRTransfer(KUInt32 inVAddr, KUInt32 inInstruction)
{
	const Boolean theFlagS = (inInstruction & 0x00100000) != 0;
	KUInt32 theMode;
	KUInt32 thePushedValue;
//	Boolean doPush = true;
//	Boolean doPushPC = false;
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
		thePushedValue = (inInstruction & 0x0000000F); // Rm
	}
	
	switch ((inInstruction & 0x01E00000) >> 21)
	{
		case 0x0:	// 0b0000
					// AND
			LogicalOp(inVAddr, inInstruction,
						 AND, theMode, theFlagS,
						 (inInstruction & 0x000F0000) >> 16 /* Rn */,
						 (inInstruction & 0x0000F000) >> 12 /* Rd */,
						 thePushedValue);
			break;
			
		case 0x1:	// 0b0001
					// EOR
			fprintf(pCOut, "#error Not yet implemented 04\n");
//			PUSHFUNC(EOR_Func(theMode, theFlagS, __RnRd));
//			doPushPC = ShouldPushPC_LogicalOp(theMode, __Rn);
			break;
			
		case 0x2:	// 0b0010
					// SUB
			ArithmeticOp(inVAddr, inInstruction,
						 SUB, theMode, theFlagS,
						 (inInstruction & 0x000F0000) >> 16 /* Rn */,
						 (inInstruction & 0x0000F000) >> 12 /* Rd */,
						 thePushedValue);
			break;
			
		case 0x3:	// 0b0011
					// RSB
			ArithmeticOp(inVAddr, inInstruction,
						 RSB, theMode, theFlagS,
						 (inInstruction & 0x000F0000) >> 16 /* Rn */,
						 (inInstruction & 0x0000F000) >> 12 /* Rd */,
						 thePushedValue);
			break;
			
		case 0x4:	// 0b01000
					// ADD
			ArithmeticOp(inVAddr, inInstruction,
						 ADD, theMode, theFlagS,
						 (inInstruction & 0x000F0000) >> 16 /* Rn */,
						 (inInstruction & 0x0000F000) >> 12 /* Rd */,
						 thePushedValue);
			break;
			
		case 0x5:	// 0b01010
					// ADC
			fprintf(pCOut, "#error Not yet implemented 06\n");
//			PUSHFUNC(ADC_Func(theMode, theFlagS, __RnRd));
//			doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
			break;
			
		case 0x6:	// 0b01100
					// SBC
			fprintf(pCOut, "#error Not yet implemented 07\n");
//			PUSHFUNC(SBC_Func(theMode, theFlagS, __RnRd));
//			doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
			break;
			
		case 0x7:	// 0b0111
					// RSC
			fprintf(pCOut, "#error Not yet implemented 08\n");
//			PUSHFUNC(RSC_Func(theMode, theFlagS, __RnRd));
//			doPushPC = ShouldPushPC_ArithmeticOp(theMode, __Rn);
			break;
			
		case 0x8:	// 0b1000
					// MRS (CPSR) & TST
			if (theFlagS == 0)
			{
				if (theMode != NoShift)
				{
					fprintf(pCOut, "#error Not yet implemented 09.1\n");
//					// Undefined Instruction (there is no MRS with Imm bit set or low bits set)
//					PUSHFUNC(UndefinedInstruction);
//					doPush = false;
//					doPushPC = true;
				} else {
					Translate_MRS(inVAddr, inInstruction,
								  (inInstruction >> 22) & 1, /* FLAG_R */
								  (inInstruction & 0x0000F000) >> 12 /* Rd */);
					
				}
			} else {
				TestOp(inVAddr, inInstruction, TST, theMode, (inInstruction & 0x000F0000) >> 16 /* Rn */, thePushedValue);
			}
			break;
			
		case 0x9:	// 0b1001
					// MSR (CPSR) & TEQ
			if (theFlagS == 0)
			{
				if (theMode == Regular)
				{
					fprintf(pCOut, "#error Not yet implemented 0A\n");
//					// Software breakpoint
//					KUInt16 theID = inInstruction & 0x0000000F;
//					theID |= (inInstruction & 0x000FFF00) >> 4;
//					thePushedValue = theID;
//					PUSHFUNC(SoftwareBreakpoint);
//					doPushPC = true;
				} else {
					fprintf(pCOut, "#error Not yet implemented 0B\n");
//					if (theMode == NoShift)
//					{
//						PUSHFUNC(MSR_NoShift_Func(0, (inInstruction & 0x000F0000) >> 16, __Rm));
//						doPushPC = ShouldPushPC_MSR_NoShift(__Rm);
//						doPush = false;
//					} else {
//						PUSHFUNC(MSR_Imm_Func(0, (inInstruction & 0x000F0000) >> 16));
//						doPushPC = false;
//					}
				}
			} else {
				TestOp(inVAddr, inInstruction, TEQ, theMode, (inInstruction & 0x000F0000) >> 16 /* Rn */, thePushedValue);
			}
			break;
			
		case 0xA:	// 0b1010
					// MRS (SPSR) & CMP
			if (theFlagS == 0)
			{
				fprintf(pCOut, "#error Not yet implemented 0C\n");
//				if (theMode != NoShift)
//				{
//					// Undefined Instruction (there is no MRS with Imm bit set or low bits set)
//					PUSHFUNC(UndefinedInstruction);
//					doPush = false;
//					doPushPC = true;
//				} else {
//					PUSHFUNC(MRS_Func(1, __Rd));
//					doPush = false;
//				}
			} else {
				TestOp(inVAddr, inInstruction, CMP, theMode, (inInstruction & 0x000F0000) >> 16 /* Rn */, thePushedValue);
			}
			break;
			
		case 0xB:	// 0b1011
					// MSR (SPSR) & CMN
			if (theFlagS == 0)
			{
				fprintf(pCOut, "#error Not yet implemented 0D\n");
//				if (theMode == Regular)
//				{
//					// Undefined Instruction (there is no MSR with shift)
//					PUSHFUNC(UndefinedInstruction);
//					doPush = false;
//					doPushPC = true;
//				} else {
//					if (theMode == NoShift)
//					{
//						PUSHFUNC(MSR_NoShift_Func(1, (inInstruction & 0x000F0000) >> 16, __Rm));
//						doPush = false;
//					} else {
//						PUSHFUNC(MSR_Imm_Func(1, (inInstruction & 0x000F0000) >> 16));
//					}
//				}
			} else {
				TestOp(inVAddr, inInstruction, CMN, theMode, (inInstruction & 0x000F0000) >> 16 /* Rn */, thePushedValue);
			}
			break;
			
		case 0xC:	// 0b1100
					// ORR
			LogicalOp(inVAddr, inInstruction,
					  ORR, theMode, theFlagS,
					  (inInstruction & 0x000F0000) >> 16 /* Rn */,
					  (inInstruction & 0x0000F000) >> 12 /* Rd */,
					  thePushedValue);
			break;
			
		case 0xD:	// 0b11010
					// MOV
			MoveOp(inVAddr, inInstruction,
				   MOV, theMode, theFlagS,
				   (inInstruction & 0x0000F000) >> 12 /* Rd */,
				   thePushedValue);
			break;
			
		case 0xE:	// 0b1110
					// BIC
			LogicalOp(inVAddr, inInstruction,
					  BIC, theMode, theFlagS,
					  (inInstruction & 0x000F0000) >> 16 /* Rn */,
					  (inInstruction & 0x0000F000) >> 12 /* Rd */,
					  thePushedValue);
			break;
			
		case 0xF:	// 0b11110
					// MVN
			MoveOp(inVAddr, inInstruction,
				   MVN, theMode, theFlagS,
				   (inInstruction & 0x0000F000) >> 12 /* Rd */,
				   thePushedValue);
			break;
	}
}


void TJITGenericRetarget::Translate_MRS(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 FLAG_R, KUInt32 Rd)
{
	if (Rd != 15) {
		if (FLAG_R) {
			fprintf(pCOut, "\t\tconst KUInt32 theResult = ioCPU->GetSPSR();\n");
		} else {
			fprintf(pCOut, "\t\tconst KUInt32 theResult = ioCPU->GetCPSR();\n");
		}
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%ld] = theResult;\n", Rd);
	} else {
		fprintf(pCOut, "\t\t#error Can't use R15 as a destination here!\n");
	}
}


void TJITGenericRetarget::TestOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 Rn, KUInt32 thePushedValue)
{
	KUInt32 Rm = (inInstruction & 0x0000000F); // only valid if MODE is NoShift
	if ((MODE == Imm) || (MODE == ImmC)) {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08X;\n", (unsigned int)thePushedValue);
	} else if (MODE == NoShift) {
		if (Rm == 15)
		{
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08lX + 8; // (PC)\n", inVAddr);
		} else {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rm);
		}
	} else if ((OP == TST) || (OP == TEQ)) {
		fprintf(pCOut, "\t\tBoolean carry = false;\n");
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = GetShift(ioCPU, 0x%08lX, &carry, 0x%08lX + 8);\n", inInstruction, inVAddr);
	} else {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = GetShiftNoCarry(ioCPU, 0x%08lX, ioCPU->mCPSR_C, 0x%08lX + 8);\n", inInstruction, inVAddr);
	}
	if (Rn == 15) {
		fprintf(pCOut, "\t\tKUInt32 Opnd1 = 0x%08lX + 8;\n", inVAddr);
	} else {
		fprintf(pCOut, "\t\tKUInt32 Opnd1 = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rn);
	}
	if (OP == TST) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 & Opnd2;\n");
	} else if (OP == TEQ) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 ^ Opnd2;\n");
	} else if (OP == CMP) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 - Opnd2;\n");
	} else if (OP == CMN) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 + Opnd2;\n");
	}
	if ((OP == TST) || (OP == TEQ)) {
		if ((MODE == NoShift) || (MODE == Imm)) {
			fprintf(pCOut, "\t\tSetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );\n");
		} else if (MODE == ImmC) {
			fprintf(pCOut, "\t\tSetCPSRBitsForLogicalOp( ioCPU, theResult, Opnd2 & 0x80000000 );\n");
		} else {
			fprintf(pCOut, "\t\tSetCPSRBitsForLogicalOp( ioCPU, theResult, carry );\n");
		}
	} else if ((OP == CMP) || (OP == CMN)) {
		fprintf(pCOut, "\t\tconst KUInt32 Negative1 = Opnd1 & 0x80000000;\n");
		fprintf(pCOut, "\t\tconst KUInt32 Negative2 = Opnd2 & 0x80000000;\n");
		fprintf(pCOut, "\t\tconst KUInt32 NegativeR = theResult & 0x80000000;\n");
		if (OP == CMP) {
			fprintf(pCOut, "\t\tSetCPSRBitsForArithmeticOp(\n"
							   "\t\t\tioCPU,\n"
							   "\t\t\ttheResult,\n"
							   "\t\t\t(Negative1 && !Negative2)\n"
							   "\t\t\t|| (Negative1 && !NegativeR)\n"
							   "\t\t\t|| (!Negative2 && !NegativeR),\n"
							   "\t\t\t(Negative1 && !Negative2 && !NegativeR)\n"
							   "\t\t\t|| (!Negative1 && Negative2 && NegativeR));\n");
		} else if (OP == CMN) {
			fprintf(pCOut, "\t\tSetCPSRBitsForArithmeticOp(\n"
							   "\t\t\tioCPU,\n"
							   "\t\t\ttheResult,\n"
							   "\t\t\t(Negative1 && Negative2)\n"
							   "\t\t\t|| ((Negative1 || Negative2) && !NegativeR),\n"
							   "\t\t\t(Negative1 == Negative2)\n"
					           "\t\t\t&& (Negative1 != NegativeR));\n");
		}
	}
//	fprintf(pCOut, "\t\tCALLNEXTUNIT;\n");
}


void TJITGenericRetarget::LogicalOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 FLAG_S, KUInt32 Rn, KUInt32 Rd, KUInt32 thePushedValue)
{
	KUInt32 Rm = thePushedValue;
	if ((MODE == Imm) || (MODE == ImmC)) {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08X;\n", (unsigned int)thePushedValue);
	} else if (MODE == NoShift) {
		if (Rm == 15) {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08lX + 8; // PC\n", inVAddr);
		} else {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rm);
		}
	} else if (FLAG_S) {
		fprintf(pCOut, "\t\tBoolean carry = false;\n");
		fprintf(pCOut, "\t\tconst KUInt32 Opnd2 = GetShift( ioCPU, 0x%08lX, &carry, 0x%08lX + 8 );\n", inInstruction, inVAddr);
	} else {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = GetShiftNoCarry( ioCPU, 0x%08lX, ioCPU->mCPSR_C, 0x%08lX + 8 );\n", inInstruction, inVAddr);
	}
	if (Rn == 15) {
		fprintf(pCOut, "\t\tKUInt32 Opnd1 = 0x%08lX + 8;\n", inVAddr);
	} else {
		fprintf(pCOut, "\t\tKUInt32 Opnd1 = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rn);
	}
	if (OP == AND) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 & Opnd2;\n");
	} else if (OP == EOR) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 ^ Opnd2;\n");
	} else if (OP == ORR) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 | Opnd2;\n");
	} else if (OP == BIC) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 & ~ Opnd2;\n");
	} else {
		fprintf(pCOut, "ERROR: wrong operation for this OP\n");
	}
	if (Rd == 15) {
		fprintf(pCOut, "\t\tSETPC(theResult + 4);\n");
		if (FLAG_S) {
			fprintf(pCOut, "\t\tioCPU->SetCPSR( ioCPU->GetSPSR() );\n");
		}
	} else {
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theResult;\n", (unsigned int)Rd);
		if (FLAG_S) {
			if ((MODE == NoShift) || (MODE == Imm)) {
				fprintf(pCOut, "\t\tSetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );\n");
			} else if (MODE == ImmC) {
				fprintf(pCOut, "\t\tSetCPSRBitsForLogicalOp( ioCPU, theResult, Opnd2 & 0x80000000 );\n");
			} else {
				fprintf(pCOut, "\t\tSetCPSRBitsForLogicalOp( ioCPU, theResult, carry );\n");
			}
		}
	}
	if (Rd == 15) {
		if (FLAG_S) {
			fprintf(pCOut, "\t\treturn; //MMUCALLNEXT_AFTERSETPC;\n");
		} else {
			fprintf(pCOut, "\t\treturn; //FURTHERCALLNEXT_AFTERSETPC;\n");
		}
	} else {
		//		fprintf(pCOut, "\t\tCALLNEXTUNIT;\n");
	}
}


void TJITGenericRetarget::ArithmeticOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 FLAG_S, KUInt32 Rn, KUInt32 Rd, KUInt32 thePushedValue)
{
	KUInt32 Rm = thePushedValue;
	if ((MODE == Imm) || (MODE == ImmC)) {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08X;\n", (unsigned int)thePushedValue);
	} else if (MODE == NoShift) {
		if (Rm == 15) {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08lX + 8; // PC\n", inVAddr);
		} else {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rm);
		}
	} else {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = GetShiftNoCarry( ioCPU, 0x%08lX, ioCPU->mCPSR_C, 0x%08lX + 8 );\n", inInstruction, inVAddr);
	}
	if (Rn == 15) {
		fprintf(pCOut, "\t\tKUInt32 Opnd1 = 0x%08lX + 8;\n", inVAddr);
	} else {
		fprintf(pCOut, "\t\tKUInt32 Opnd1 = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rn);
	}
	if (OP == SUB) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 - Opnd2;\n");
	} else if (OP == RSB) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd2 - Opnd1;\n");
	} else if (OP == ADD) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 + Opnd2;\n");
	} else if (OP == ADC) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 + Opnd2 + ioCPU->mCPSR_C;\n");
	} else if (OP == SBC) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd1 - Opnd2 - 1 + ioCPU->mCPSR_C;\n");
	} else if (OP == RSC) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd2 - Opnd1 - 1 + ioCPU->mCPSR_C;\n");
	} else {
		fprintf(pCOut, "ERROR: wrong operation for this OP\n");
	}
	if (Rd == 15) {
		fprintf(pCOut, "\t\tSETPC(theResult + 4);\n");
		if (FLAG_S) {
			fprintf(pCOut, "\t\tioCPU->SetCPSR( ioCPU->GetSPSR() );\n");
		}
	} else {
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theResult;\n", (unsigned int)Rd);
		if (FLAG_S) {
			fprintf(pCOut, "\t\tconst KUInt32 Negative1 = Opnd1 & 0x80000000;\n");
			fprintf(pCOut, "\t\tconst KUInt32 Negative2 = Opnd2 & 0x80000000;\n");
			fprintf(pCOut, "\t\tconst KUInt32 NegativeR = theResult & 0x80000000;\n");
			if ((OP == SUB) || (OP == SBC)) {
				fprintf(pCOut, "\t\tSetCPSRBitsForArithmeticOp(\n"
							   "\t\t\tioCPU,\n"
							   "\t\t\ttheResult,\n"
							   "\t\t\t(Negative1 && !Negative2)\n"
							   "\t\t\t|| (Negative1 && !NegativeR)\n"
							   "\t\t\t|| (!Negative2 && !NegativeR),\n"
							   "\t\t\t(Negative1 && !Negative2 && !NegativeR)\n"
					           "\t\t\t|| (!Negative1 && Negative2 && NegativeR));\n");
			} else if ((OP == RSB) || (OP == RSC)) {
				fprintf(pCOut, "\t\tSetCPSRBitsForArithmeticOp(\n"
							   "\t\t\tioCPU,\n"
							   "\t\t\ttheResult,\n"
							   "\t\t\t(Negative2 && !Negative1)\n"
							   "\t\t\t|| (Negative2 && !NegativeR)\n"
							   "\t\t\t|| (!Negative1 && !NegativeR),\n"
							   "\t\t\t(Negative2 && !Negative1 && !NegativeR)\n"
						       "\t\t\t|| (!Negative2 && Negative1 && NegativeR));\n");
			} else if ((OP == ADD) || (OP == ADC)) {
				fprintf(pCOut, "\t\tSetCPSRBitsForArithmeticOp(\n"
							   "\t\t\tioCPU,\n"
							   "\t\t\ttheResult,\n"
							   "\t\t\t(Negative1 && Negative2)\n"
							   "\t\t\t|| ((Negative1 || Negative2) && !NegativeR),\n"
							   "\t\t\t(Negative1 == Negative2)\n"
						       "\t\t\t&& (Negative1 != NegativeR));\n");
			}
		}
	}
	if (Rd == 15) {
		if (FLAG_S) {
			fprintf(pCOut, "\t\treturn; //MMUCALLNEXT_AFTERSETPC;\n");
		} else {
			fprintf(pCOut, "\t\treturn; //FURTHERCALLNEXT_AFTERSETPC;\n");
		}
	} else {
//		fprintf(pCOut, "\t\tCALLNEXTUNIT;\n");
	}
}


void TJITGenericRetarget::MoveOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 FLAG_S, KUInt32 Rd, KUInt32 thePushedValue)
{
	KUInt32 Rm = thePushedValue;
	if ((MODE == Imm) || (MODE == ImmC)) {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08lX;\n", thePushedValue);
	} else if (MODE == NoShift) {
		if (Rm == 15) {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08lX + 8;\n", inVAddr);
		} else {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = ioCPU->mCurrentRegisters[%ld];\n", Rm);
		}
	} else if (FLAG_S) {
		fprintf(pCOut, "\t\tBoolean carry = false;\n");
		fprintf(pCOut, "\t\tconst KUInt32 Opnd2 = GetShift( ioCPU, 0x%08lX, &carry, 0x%08lX + 8 );\n", inInstruction, inVAddr);
	} else {
		fprintf(pCOut, "\t\tconst KUInt32 Opnd2 = GetShiftNoCarry( ioCPU, 0x%08lX, ioCPU->mCPSR_C, 0x%08lX + 8 );\n", inInstruction, inVAddr);
	}
	if (OP == MOV) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = Opnd2;\n");
	} else if (OP == MVN) {
		fprintf(pCOut, "\t\tconst KUInt32 theResult = ~ Opnd2;\n");
	}
	if (Rd == 15) {
//		if (!FLAG_S) {
//			fprintf(pCOut, "\t\tCALLNEXT_SAVEPC;\n");
//		}
		fprintf(pCOut, "\t\tSETPC(theResult + 4);\n");
		if (FLAG_S) {
			fprintf(pCOut, "\t\tioCPU->SetCPSR( ioCPU->GetSPSR() );\n");
		}
	} else {
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%ld] = theResult;\n", Rd);
		if (FLAG_S) {
			if ((MODE == NoShift) || (MODE == Imm)) {
				fprintf(pCOut, "\t\tSetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );\n");
			} else if (MODE == ImmC) {
				fprintf(pCOut, "\t\tSetCPSRBitsForLogicalOp( ioCPU, theResult, Opnd2 & 0x80000000 );\n");
			} else {
				fprintf(pCOut, "\t\tSetCPSRBitsForLogicalOp( ioCPU, theResult, carry );\n");
			}
		}
	}
	if (Rd == 15) {
		if (FLAG_S) {
			fprintf(pCOut, "\t\treturn; //MMUCALLNEXT_AFTERSETPC;\n");
		} else {
			fprintf(pCOut, "\t\treturn; //FURTHERCALLNEXT_AFTERSETPC;\n");
		}
	} else {
//		fprintf(pCOut, "\t\tCALLNEXTUNIT;\n");
	}
}


void TJITGenericRetarget::DoTranslate_10(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// Block Data Transfer
	// Branch
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer
	// -Cond-- 1  0  1  L  ---------------------offset---------------------------- Branch
	if (inInstruction & 0x02000000)
	{
		Translate_Branch(inVAddr, inInstruction);
	} else {
		// Block Data Transfer.
		Translate_BlockDataTransfer(inVAddr, inInstruction);
	}
}


void TJITGenericRetarget::Translate_Branch(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// -Cond-- 1  0  1  L  ---------------------offset---------------------------- Branch
	
	// PC prefetch strategy: PC currently is 8 bytes ahead.
	// When we enter this method, it should be 4 bytes ahead.
	// The offset of the branch should be added to the PC and then PC will
	// point exactly to the next instruction.
	// E.g. here B here is coded EAFFFFFE, i.e. branch to offset -8.
	// So what we need to do, for this emulator, is to add 4 to the
	// final result.
	
	KUInt32 offset = (inInstruction & 0x007FFFFF) << 2;
	if (inInstruction & 0x00800000)
	{
		offset |= 0xFE000000;
	}
	KUInt32 delta = offset + 8;
	KUInt32 dest = inVAddr + delta;

	// Replace all jumps into jump tables with jumps to the original ROM code
	// Jump table ranges from 0x01A00000 - 0x01CFFFFF
	if (dest>=0x01A00000 && dest<0x01D00000) {
		KUInt32 jtInstruction;
		pMemory->Read(dest, jtInstruction);
		offset = (jtInstruction & 0x007FFFFF) << 2;
		if (jtInstruction & 0x00800000)
		{
			offset |= 0xFE000000;
		}
		delta = offset + 8;
		dest = dest + delta;
	}
	
	if (inInstruction & 0x01000000)
	{
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[14] = 0x%08lX + 4;\n", inVAddr);
		fprintf(pCOut, "\t\tFunc_0x%08lX(ioCPU);\n", dest);
		fprintf(pCOut, "\t\tif (ioCPU->mCurrentRegisters[15]!=0x%08lX) {\n", inVAddr+8);
		fprintf(pCOut, "\t\t	RT_PANIC_UNEXPECTED_RETURN_ADDRESS\n"); // throws an exception that leaves simulation and returns to JIT
		fprintf(pCOut, "\t\t}\n");
	} else {
		KUInt32 destination = inVAddr + delta;
		if (destination<pFunctionBegin||destination>=pFunctionEnd) {
			fprintf(pCOut, "\t\treturn Func_0x%08lX(ioCPU);\n", dest);
		} else {
			fprintf(pCOut, "\t\tgoto L%08lX;\n", dest);
		}
	}
}


void TJITGenericRetarget::Translate_BlockDataTransfer(KUInt32 inVAddr, KUInt32 inInstruction)
{
//	KUInt32 flag_pu =	(inInstruction & 0x01800000) >> (23 - 4);
//	KUInt32 flag_puw =	flag_pu << 1 | ((inInstruction & 0x00200000) >> (21 - 4));
//	KUInt32 Rn =		(inInstruction & 0x000F0000) >> 16;
//	KUInt32 regList =	(inInstruction & 0x0000FFFF);
//	KUInt32 nbRegs = CountBits(regList);
	// Different cases.
	if (inInstruction & 0x00100000)
	{
		// Load
		if (inInstruction & 0x00400000)
		{
			if (inInstruction & 0x00008000)
			{
				// LDM3
				fprintf(pCOut, "#error Not yet implemented 11\n");
//				Translate_BlockDataTransfer_LDM3(inVAddr, inInstruction);
			} else {
				// LDM2
				fprintf(pCOut, "#error Not yet implemented 12\n");
//				Translate_BlockDataTransfer_LDM2(inVAddr, inInstruction);
			}
		} else {
			// LDM1
			Translate_BlockDataTransfer_LDM1(inVAddr, inInstruction);
		}
	} else {
		// Store
		if (inInstruction & 0x00400000)
		{
			// STM2
			fprintf(pCOut, "#error Not yet implemented 13\n");
//			PUSHFUNC(BlockDataTransfer_STM2_Funcs[flag_pu | Rn]);
		} else {
			// STM1
			Translate_BlockDataTransfer_STM1(inVAddr, inInstruction);
		}
	}
	
	// Push the PC. We'll need it for data aborts.
//	PUSHVALUE(inVAddr + 8);
	// Push the reg list and the number of registers.
//	PUSHVALUE((nbRegs << 16) | regList);
}


void TJITGenericRetarget::Translate_BlockDataTransfer_STM1(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer
	KUInt32 FLAG_P = (inInstruction>>24) & 1;
	KUInt32 FLAG_U = (inInstruction>>23) & 1;
	KUInt32 FLAG_W = (inInstruction>>21) & 1;
	KUInt32 Rn = (inInstruction>>16) & 0x0F;
	KUInt32 theRegList = inInstruction & 0xFFFF;
	KUInt32 curRegList = theRegList & 0x7FFF;
	KUInt32 nbRegisters = CountBits(theRegList);
	if (Rn==15) fprintf(pCOut, "#error Rn == 15 -> UNPREDICTABLE\n");
	fprintf(pCOut, "\t\tKUInt32 baseAddress = ioCPU->mCurrentRegisters[%ld];\n", Rn);
	
	if (FLAG_W) { // Write back
		if (FLAG_U) {
			fprintf(pCOut, "\t\tKUInt32 wbAddress = baseAddress + (%ld * 4);\n", nbRegisters);
		} else {
			fprintf(pCOut, "\t\tKUInt32 wbAddress = baseAddress - (%ld * 4);\n", nbRegisters);
		}
	}
	
	if (FLAG_U) { // Up.
		if (FLAG_P) { // Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	} else {	// Down.
		fprintf(pCOut, "\t\tbaseAddress -= (%ld * 4);\n", nbRegisters);
		if (!FLAG_P) { // Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	}
	int indexReg = 0;
	while (curRegList) {
		if (curRegList & 1) {
			fprintf(pCOut, "\t\tioCPU->ManagedMemoryWriteAligned(baseAddress, ioCPU->mCurrentRegisters[%d], 0x%08lX);\n", indexReg, inVAddr);
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
		curRegList >>= 1;
		indexReg++;
	}
	if (theRegList & 0x8000)
	{
		// PC is special.
		// Stored value is PC + 12 (verified)
		fprintf(pCOut, "\t\tioCPU->ManagedMemoryWriteAligned(baseAddress, 0x%08lX + 12, 0x%08lX);\n", inVAddr, inVAddr);
	}
	
	if (FLAG_W) {
	// Write back.
	// Rn == 15 -> UNPREDICTABLE
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%ld] = wbAddress;\n", Rn);
	}
}


void TJITGenericRetarget::Translate_BlockDataTransfer_LDM1(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer
	KUInt32 FLAG_P = (inInstruction>>24) & 1;
	KUInt32 FLAG_U = (inInstruction>>23) & 1;
	KUInt32 FLAG_W = (inInstruction>>21) & 1;
	KUInt32 Rn = (inInstruction>>16) & 0x0F;
	KUInt32 theRegList = inInstruction & 0xFFFF;
	KUInt32 curRegList = theRegList & 0x7FFF;
	KUInt32 nbRegisters = CountBits(theRegList);
	if (Rn==15) fprintf(pCOut, "#error Rn == 15 -> UNPREDICTABLE\n");
	fprintf(pCOut, "\t\tKUInt32 baseAddress = ioCPU->mCurrentRegisters[%ld];\n", Rn);
	if (FLAG_W) { // Write back
		if (FLAG_U) {
			fprintf(pCOut, "\t\tKUInt32 wbAddress = baseAddress + (%ld * 4);\n", nbRegisters);
		} else {
			fprintf(pCOut, "\t\tKUInt32 wbAddress = baseAddress - (%ld * 4);\n", nbRegisters);
		}
	}
	
	if (FLAG_U) { // Up.
		if (FLAG_P) { // Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	} else { // Down.
		fprintf(pCOut, "\t\tbaseAddress -= (%ld * 4);\n", nbRegisters);
		if (!FLAG_P) { // Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	}
	
	int indexReg = 0;
	while (curRegList) {
		if (curRegList & 1) {
			fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = ioCPU->ManagedMemoryReadAligned(baseAddress, 0x%08lX);\n", indexReg, inVAddr);
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
		curRegList >>= 1;
		indexReg++;
	}
	
	if (theRegList & 0x8000)
	{
		// PC is special.
		fprintf(pCOut, "\t\tSETPC( ioCPU->ManagedMemoryReadAligned(baseAddress, 0x%08lX)) + 4;\n", inVAddr);
	}
	
	if (FLAG_W) {
	// Write back.
	// Rn == 15 -> UNPREDICTABLE
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%ld] = wbAddress;\n", Rn);
	}
	
	if (theRegList & 0x8000)
	{
		fprintf(pCOut, "\t\treturn; //MMUCALLNEXT_AFTERSETPC;\n");
	} else {
		//CALLNEXTUNIT;
	}
}


void TJITGenericRetarget::DoTranslate_11(KUInt32 inVAddr, KUInt32 inInstruction)
{
	fprintf(pCOut, "#error Not yet implemented 14\n");
//	Translate_SWIAndCoproc(inVAddr, inInstruction);
}


#if 0
// Einstein
#include "TARMProcessor.h"

#include "TJITGeneric_Macros.h"

#include "TJITGeneric_Test.h"
#include "TJITGeneric_Other.h"
#include "TJITGeneric_DataProcessingPSRTransfer.h"
#include "TJITGeneric_SingleDataTransfer.h"
#include "TJITGeneric_SingleDataSwap.h"
#include "TJITGeneric_Multiply.h"
#include "TJITGeneric_MultiplyAndAccumulate.h"
#include "TJITGeneric_BlockDataTransfer.h"

#ifdef JIT_PERFORMANCE
#include "TJITPerformance.h"
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
/// Stats...
#undef COLLECT_STATS_ON_PAGES

#ifdef COLLECT_STATS_ON_PAGES
static KUInt16 gMaxUnitsCount = 0;
#endif

// -------------------------------------------------------------------------- //
//  * TJITGenericPage( void )
// -------------------------------------------------------------------------- //
TJITGenericPage::TJITGenericPage( void )
{
	mUnits = (JITUnit*) ::malloc(sizeof(JITUnit) * kDefaultUnitCount);
	mUnitCount = kDefaultUnitCount;
}

// -------------------------------------------------------------------------- //
//  * ~TJITGenericPage( void )
// -------------------------------------------------------------------------- //
TJITGenericPage::~TJITGenericPage( void )
{
	::free(mUnits);
}

// -------------------------------------------------------------------------- //
//  * Init( KUInt32*, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TJITGenericPage::Init(
			TMemory* inMemoryIntf,
			KUInt32 inVAddr,
			KUInt32 inPAddr )
{
	TJITPage<TJITGenericPage>::Init( inMemoryIntf, inVAddr, inPAddr );
	KUInt32* thePointer = GetPointer();

	// Translate the page.
	KUInt32 indexInstr;
	KUInt32 theOffsetInPage = 0;
	KUInt16 unitCrsr = 0;
	for (indexInstr = 0; indexInstr < kInstructionCount; indexInstr++)
	{
		mUnitsTable[indexInstr] = unitCrsr;
		Translate(
		    inMemoryIntf,
			&unitCrsr,
			thePointer[indexInstr],
			inVAddr + theOffsetInPage );
		theOffsetInPage += 4;
	}

	PushUnit(&unitCrsr, TJITGenericPage::EndOfPage);
	PushUnit(&unitCrsr, inVAddr + theOffsetInPage + 4);	// PC + 8

#ifdef COLLECT_STATS_ON_PAGES
	if (unitCrsr > gMaxUnitsCount) {
		gMaxUnitsCount = unitCrsr;
		fprintf(stderr, "Max units count = %i\n", unitCrsr);
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * PushUnit( KUInt16*, KUIntPtr )
// -------------------------------------------------------------------------- //
void
TJITGenericPage::PushUnit(KUInt16* ioUnitCrsr, KUIntPtr inUnit)
{
	// Can we push it?
	KUInt16 theCrsr = *ioUnitCrsr;
	if (theCrsr == mUnitCount) {
		// We need to resize the table.
		mUnitCount += kUnitIncrement;
		mUnits = (JITUnit*) ::realloc(mUnits, mUnitCount * sizeof(JITUnit));
	}
	mUnits[theCrsr++].fPtr = inUnit;
	*ioUnitCrsr = theCrsr;
}


#ifdef JIT_PERFORMANCE
JITInstructionProto(instrCount)
{
	KUInt32 pc;
	POPVALUE(pc);
	COUNTHIT(branchDestCount, pc)
	EXECUTENEXTUNIT;
}
#endif


// -------------------------------------------------------------------------- //
//  * Translate( JITUnit*, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TJITGenericPage::Translate(
				TMemory* inMemoryIntf,
				KUInt16* ioUnitCrsr,
				KUInt32 inInstruction,
				KUInt32 inVAddr )
{
#ifdef JIT_PERFORMANCE
	PushUnit(ioUnitCrsr, instrCount);
	PushUnit(ioUnitCrsr, inVAddr);
#endif

	// handle injections before anything else
	if ((inInstruction & 0xffe00000)==0xefc00000) {
		inInstruction = Translate_Injection(
											this,
											ioUnitCrsr,
											inInstruction,
											inVAddr);
	} else if ((inInstruction & 0xffe00000)==0xefa00000) {
		inInstruction = Translate_SimulatorInjection(
											this,
											ioUnitCrsr,
											inInstruction,
											inVAddr);
	}
	
	int theTestKind = inInstruction >> 28;
	KUInt16 testUnitCrsr = *ioUnitCrsr;
	if ((theTestKind != kTestAL) && (theTestKind != kTestNV))
	{
		// We have a real test.
		// Save some room for it, but put a null for now.
		PushUnit(ioUnitCrsr, (KUIntPtr) 0L);
	}
	
	if (theTestKind != kTestNV)
	{
		switch ((inInstruction >> 26) & 0x3)				// 27 & 26
		{
			case 0x0:	// 00
				DoTranslate_00(
					ioUnitCrsr,
					inInstruction,
					inVAddr);
				break;
					
			case 0x1:	// 01
				DoTranslate_01(
					inMemoryIntf,
					ioUnitCrsr,
					inInstruction,
					inVAddr);
				break;
					
			case 0x2:	// 10
				DoTranslate_10(
					ioUnitCrsr,
					inInstruction,
					inVAddr);
				break;
					
			case 0x3:	// 11
				Translate_SWIAndCoproc(
					this,
					ioUnitCrsr,
					inInstruction,
					inVAddr);
				break;
		} // switch 27 & 26
	}
	
	// Finally put the test, based on the current unit crsr.
	if ((theTestKind != kTestAL) && (theTestKind != kTestNV))
	{
		KUInt16 nextInstrCrsr = *ioUnitCrsr;
		PutTest(testUnitCrsr, nextInstrCrsr - testUnitCrsr, theTestKind);
	}
}

// -------------------------------------------------------------------------- //
//  * PutTest( KUInt16*, KUInt32 )
// -------------------------------------------------------------------------- //
#define _template1(func, delta)	func ## delta
#define __PutTest_line(func, delta)										\
		case (delta):													\
			mUnits[inUnitCrsr].fFuncPtr = _template1(func, delta);		\
			break

// NOTICE: maximum number of units for an instruction is set to 6.....

#define __PutTest_packet(func)							\
		switch(inDelta) {								\
			__PutTest_line(func, 2);					\
			__PutTest_line(func, 3);					\
			__PutTest_line(func, 4);					\
			__PutTest_line(func, 5);					\
			__PutTest_line(func, 6);					\
			__PutTest_line(func, 7);					\
			default:									\
				fprintf(stderr, "Test overflow!\n");	\
				abort();								\
		}

inline void
TJITGenericPage::PutTest(
				KUInt16 inUnitCrsr,
				unsigned char inDelta,
				int inTest )
{
	// Test the condition.
	switch (inTest)
	{
		// 0000 = EQ - Z set (equal)
		case kTestEQ:
			__PutTest_packet(TestEQ);
			break;
			
			// 0001 = NE - Z clear (not equal)
		case kTestNE:
			__PutTest_packet(TestNE);
			break;
	
			// 0010 = CS - C set (unsigned higher or same)
		case kTestCS:
			__PutTest_packet(TestCS);
			break;
			
			// 0011 = CC - C clear (unsigned lower)
		case kTestCC:
			__PutTest_packet(TestCC);
			break;
			
			// 0100 = MI - N set (negative)
		case kTestMI:
			__PutTest_packet(TestMI);
			break;
			
			// 0101 = PL - N clear (positive or zero)
		case kTestPL:
			__PutTest_packet(TestPL);
			break;
			
			// 0110 = VS - V set (overflow)
		case kTestVS:
			__PutTest_packet(TestVS);
			break;
			
			// 0111 = VC - V clear (no overflow)
		case kTestVC:
			__PutTest_packet(TestVC);
			break;
			
			// 1000 = HI - C set and Z clear (unsigned higher)
		case kTestHI:
			__PutTest_packet(TestHI);
			break;
			
			// 1001 = LS - C clear or Z set (unsigned lower or same)
		case kTestLS:
			__PutTest_packet(TestLS);
			break;
			
			// 1010 = GE - N set and V set, or N clear and V clear (greater or equal)
		case kTestGE:
			__PutTest_packet(TestGE);
			break;
			
			// 1011 = LT - N set and V clear, or N clear and V set (less than)
		case kTestLT:
			__PutTest_packet(TestLT);
			break;
			
			// 1100 = GT - Z clear, and either N set and V set, or N clear and V clear (greater than)
		case kTestGT:
			__PutTest_packet(TestGT);
			break;
			
			// 1101 = LE - Z set, or N set and V clear, or N clear and V set (less than or equal)
		case kTestLE:
			__PutTest_packet(TestLE);
			break;
			
			// 1110 = AL - always
		case kTestAL:
			// 1111 = NV - never
		case kTestNV:
		default:
			break;
	}
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_00( KUInt32, JITFuncPtr*, JITUnit* )
// -------------------------------------------------------------------------- //
void
TJITGenericPage::DoTranslate_00(
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  I  --Opcode--- S  --Rn--- --Rd--- ----------Operand 2-------- Data Processing PSR Transfer
	// -Cond-- 0  0  0  0  0  0  A  S  --Rd--- --Rn--- --Rs--- 1  0  0  1  --Rm--- Multiply
	// -Cond-- 0  0  0  1  0  B  0  0  --Rn--- --Rd--- 0 0 0 0 1  0  0  1  --Rm---
	if ((inInstruction & 0x020000F0) == 0x90)	// I=0 & 0b1001----
	{
		if (inInstruction & 0x01000000)
		{
			// Single Data Swap
			Translate_SingleDataSwap(
					this,
					ioUnitCrsr,
					inInstruction,
					inVAddr );
		} else {
			// Multiply
			if (inInstruction & 0x00200000)
			{
				Translate_MultiplyAndAccumulate(
					this,
					ioUnitCrsr,
					inInstruction,
					inVAddr);
			} else {
				Translate_Multiply(
					this,
					ioUnitCrsr,
					inInstruction,
					inVAddr);
			}
		}
	} else {
		Translate_DataProcessingPSRTransfer(
					this,
					ioUnitCrsr,
					inInstruction,
					inVAddr );
	}
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_01( JITUnit*, KUInt32, KUInt32, KUInt32, JITFuncPtr* )
// -------------------------------------------------------------------------- //
inline
void
TJITGenericPage::DoTranslate_01(
					TMemory* inMemoryIntf,
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// Single Data Transfer & Undefined
	if ((inInstruction & 0x02000010) == 0x02000010)
	{
// -Cond-- 0  1  1  -XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX- 1  -XXXXX-
          // DA WINNER
		if (inInstruction == 0xE6000510)
		{
			PushUnit(ioUnitCrsr, DebuggerUND);
			PushUnit(ioUnitCrsr, inVAddr - GetVAddr() + GetPAddr());
			PushUnit(ioUnitCrsr, inVAddr + 8);
		} else {
			PushUnit(ioUnitCrsr, UndefinedInstruction);
			PushUnit(ioUnitCrsr, inVAddr + 8);
		}
	} else {
// -Cond-- 0  1  I  P  U  B  W  L  --Rn--- --Rd--- -----------offset----------
		Translate_SingleDataTransfer(this, inMemoryIntf, ioUnitCrsr, inInstruction, inVAddr);
	}
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_10( JITUnit*, KUInt32, KUInt32, JITFuncPtr* )
// -------------------------------------------------------------------------- //
inline void
TJITGenericPage::DoTranslate_10(
					KUInt16* ioUnitCrsr,
					KUInt32 inInstruction,
					KUInt32 inVAddr )
{
	// Block Data Transfer
	// Branch
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer
	// -Cond-- 1  0  1  L  ---------------------offset---------------------------- Branch
	if (inInstruction & 0x02000000)
	{
		Translate_Branch(this, ioUnitCrsr, inInstruction, inVAddr);
	} else {
		// Block Data Transfer.
		Translate_BlockDataTransfer(this, ioUnitCrsr, inInstruction, inVAddr);
	}
}

// -------------------------------------------------------------------------- //
//  * EndOfPage( JITUnit*, TARMProcessor* )
// -------------------------------------------------------------------------- //
JITUnit*
TJITGenericPage::EndOfPage(
				JITUnit* ioUnit,
				TARMProcessor* ioCPU )
{
	// PC shouldn't have been incremented.
	POPPC();

	// Don't execute next function.
	MMUCALLNEXT(GETPC());
}

// -------------------------------------------------------------------------- //
//  * Halt( JITUnit*, TARMProcessor* )
// -------------------------------------------------------------------------- //
JITUnit*
TJITGenericPage::Halt(
				JITUnit* ioUnit,
				TARMProcessor* ioCPU )
{
	// Halt is used for single steps.
	// If we are here, then the previous instruction tried to continue the
	// execution with us. And as a consequence, the PC is the previous PC + 4.
	SETPC(THEPC + 4);
	return ioUnit;
}

#endif



#if 0
// The code below seemingly works as it should for "good" cases.
// FIXME: what happens if a memory access exception is caused?
// FIXME: what happens if the pc register is modified?
// FIXME: how can we translate a jump or call function int a true "C" call?
// FIXME: how can we translate function returns?
// FIXME: how can we translate virtual jump tables?
// FIXME: what if we need to jump to code in RAM?

/*
 SuperStacks (0x04004000 or 0x8004000)
 KernelStack (GetKernelSTackVirtualTop()->0x0C000400)
 SpecialStacks (InitSpecialStacks())
	FIQStack (SetFIQSTack(0x0C003400))
	AbortStack (SetAbortStack(0x0C004C00))
	UndefStack (SetUndefStack(0x0C006000))
	IRQStack (SetIRQStack(0x0C002C00))
	UserStack (SetUserStack(0x0C007400))
 
 First fault at 0x0cc79ff4

 */

JITInstructionProto(JITGenericEnd) {
	return ioUnit;
}

/// FIXME: UGLY TEST
T_ROM_PATCH(0x00038600, "Retargeting test 1")
{
	// addr:0x00038600, instr:0xE5901000
	extern JITInstructionProto(SingleDataTransfer_25_0_1);
	JITUnit j[] = {
		{ .fFuncPtr=SingleDataTransfer_25_0_1 },
		{ .fValue=0xE5901000 },
		{ .fPtr=0x00038608 },
		{ .fFuncPtr=JITGenericEnd } };
	j[0].fFuncPtr(j, ioCPU);
	CALLNEXTUNIT;
	
#if 0
	// this is called whenever a read or write exception occurs
	SETPC(GETPC());
	ioCPU->DataAbort();
	MMUCALLNEXT_AFTERSETPC;
#endif

#if 0
	// this is called whenever r15 (pc) changes
	FURTHERCALLNEXT_AFTERSETPC; // (==MMUCALLNEXT_AFTERSETPC)
#endif
}
#endif


// ============================================================================= //
// Ever wondered about the origins of the term "bugs" as applied to computer     //
// technology?  U.S. Navy Capt. Grace Murray Hopper has firsthand explanation.   //
// The 74-year-old captain, who is still on active duty, was a pioneer in        //
// computer technology during World War II.  At the C.W. Post Center of Long     //
// Island University, Hopper told a group of Long Island public school adminis-  //
// trators that the first computer "bug" was a real bug--a moth.  At Harvard     //
// one August night in 1945, Hopper and her associates were working on the       //
// "granddaddy" of modern computers, the Mark I.  "Things were going badly;      //
// there was something wrong in one of the circuits of the long glass-enclosed   //
// computer," she said.  "Finally, someone located the trouble spot and, using   //
// ordinary tweezers, removed the problem, a two-inch moth.  From then on, when  //
// anything went wrong with a computer, we said it had bugs in it."  Hopper      //
// said that when the veracity of her story was questioned recently, "I referred //
// them to my 1945 log book, now in the collection of the Naval Surface Weapons  //
// Center, and they found the remains of that moth taped to the page in          //
// question."                                                                    //
//                 [actually, the term "bug" had even earlier usage in           //
//                 regard to problems with radio hardware.  Ed.]                 //
// ============================================================================= //
