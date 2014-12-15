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

//			ldfd    f0, [pc, #16]                   @ 0x001E7268 0xED9F8104 - ....
//			sfm     f4, 1, [sp, #-12]!				@ 0x000D2780 0xED2DC203 - .-..
//sfm     f4, 1, [sp, #-12]!              @ 0x000ED69C 0xED2DC203 - .-..
//ldfd    f4, [r11, #4]                   @ 0x000ED6A0 0xED9BC101 - ....
//  --> L0038E2A4

// ldf ->load fp register from memory
// ldf%c%Q\t%12-14f, %A   0x0c000100, 0x0e100f00
// stf%c%Q\t%12-14f, %A   0x0c100100, 0x0e100f00

// 3 3 2 2  2 2 2 2  2 2 2 2  1 1 1 1  1 1 1 1  1 1
// 1 0 9 8  7 6 5 4  3 2 1 0  9 8 7 6  5 4 3 2  1 0 9 8  7 6 5 4  3 2 1 0
// --cond-  1 1 0 p  u P n l  ---r---  Q --f--  --CP#--  -----offset-----
// PQ: 0x000000 = single, 0x00008000 = double, 0x00400000 = e, 0x0040800 = p
// f: index of floating point register
// p: pre/post index bit 0: add offset after transfer
// u: up/down bit: 0: down, subtract offset from base
// n: writeback: 1: write address into base
// l: load/store bit: 0: store to memory
// r: base register
// CP#: always 1

// sfm/lfm: 0x0c000200, 0x0e100f00, "sfm%c\t%12-14f, %F, %A"

#include "TJITGenericRetarget.h"
#include "JIT.h"

#ifdef JITTARGET_GENERIC

// Einstein
#include "TMemory.h"
#include "TSymbolList.h"

#include "TJITGenericRetargetMap.h"
#include "Emulator/JIT/GEneric/TJITGenericROMPatch.h"

#include "TROMImage.h"
#include "UDisasm.h"

// This code enters the debugger right where we need it (OS X Intel)
static const char *Debug = "__asm__(\"int $3\\n\" : : );";

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

/// Test bits.
enum ETestKind {
	kTestEQ = 0x0,
	kTestNE = 0x1,
	kTestCS = 0x2,
	kTestCC = 0x3,
	kTestMI = 0x4,
	kTestPL = 0x5,
	kTestVS = 0x6,
	kTestVC = 0x7,
	kTestHI = 0x8,
	kTestLS = 0x9,
	kTestGE = 0xA,
	kTestLT = 0xB,
	kTestGT = 0xC,
	kTestLE = 0xD,
	kTestAL = 0xE,
	kTestNV = 0xF,
};

static KUInt32 CountBits(KUInt16 inWord)
{
	// http://www.caam.rice.edu/~dougm/twiddle/BitCount.html
#define T unsigned
#define ONES ((T)(-1))
#define TWO(k) ((T)1 << (k))
#define CYCL(k) (ONES/(1 + (TWO(TWO(k)))))
#define BSUM(x,k) ((x)+=(x) >> TWO(k), (x) &= CYCL(k))
	inWord = (inWord & CYCL(0)) + ((inWord>>TWO(0)) & CYCL(0));
	inWord = (inWord & CYCL(1)) + ((inWord>>TWO(1)) & CYCL(1));
	BSUM(inWord,2);
	BSUM(inWord,3);
	
	return inWord;
}


TJITGenericRetarget::TJITGenericRetarget(TMemory *inMemory, TSymbolList *inSymbolList) :
	pMemory(inMemory),
	pSymbolList(inSymbolList),
	pCOut(stderr),
	pHOut(stdout)
{
#if 0
	KUInt32 i, nSym = 0;
	char sym[1024];
	for (i=0; i<1024*1024*8; i+=4) {
		if (gJITGenericRetargetMap[i>>2]==1) {
			// code
			if (TSymbolList::List->GetSymbolExact(i, sym)) {
				nSym++;
			}
		}
	}
	fprintf(stderr, "%d entry points found\n", nSym);
#endif
}


TJITGenericRetarget::~TJITGenericRetarget()
{
}


void TJITGenericRetarget::SetRetargetMap(KUInt32 first, KUInt32 last, KUInt8 type)
{
	for ( ; first<last; first+=4 ) {
		gJITGenericRetargetMap[first>>2] = type;
	}
}

#if EINSTEIN_RETARGET
extern bool MemoryRead( KUInt32 inAddress, KUInt32& outWord );
Boolean TJITGenericRetarget::MemoryRead( KUInt32 inAddress, KUInt32& outWord )
{
	return ::MemoryRead(inAddress, outWord);
}
#else
Boolean TJITGenericRetarget::MemoryRead( KUInt32 inAddress, KUInt32& outWord )
{
	return pMemory->Read(inAddress, outWord);
}
#endif


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
	char fileID[1024];
	const char *s = strstr(filePathAndName, "Newt/");
	if (!s) s = strrchr(filePathAndName, '/');
	if (!s) s = filePathAndName;
	char *d = fileID;
	for(;;) {
		if (*s==0) { *d = 0; break; }
		if (*s=='/')
			*d = '_';
		else
			*d = *s;
		d++; s++;
	}
	
	fprintf(pHOut, "/**\n * Collection of transcoded functions\n */\n\n");
	fprintf(pHOut, "#ifndef %s_H\n", fileID);
	fprintf(pHOut, "#define %s_H\n\n", fileID);
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


void TJITGenericRetarget::TranslateFunction(KUInt32 inFirst, KUInt32 inLast, const char *inName, bool cont, bool dontLink)
{
	pFunctionBegin = inFirst;
	pFunctionEnd = inLast;
	fprintf(pCOut, "\n/**\n * Transcoded function %s\n * ROM: 0x%08X - 0x%08X\n */\n", inName, (unsigned int)inFirst, (unsigned int)inLast);
	if (pHOut!=stdout)
		fprintf(pHOut, "extern void Func_0x%08X(TARMProcessor* ioCPU, KUInt32 ret); // %s\n", (unsigned int)inFirst, inName);
	fprintf(pCOut, "void Func_0x%08X(TARMProcessor* ioCPU, KUInt32 ret)\n{\n", (unsigned int)inFirst);
	KUInt32 instr = 0;
	KUInt32 addr = inFirst;
	for ( ; addr<inLast; addr+=4) {
		MemoryRead(addr, instr);
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
					fprintf(pCOut, "\t// KUInt32 D_%08X = 0x%08X; // %s\n", (unsigned int)addr, (unsigned int)instr, sym);
				} else {
					fprintf(pCOut, "\t// KUInt32 D_%08X = 0x%08X;\n", (unsigned int)addr, (unsigned int)instr);
				}
				break;
			}
		}
	}
	if (cont) {
		// If the following commands are translated as well, this line will avoid
		// falling back to the interpreter and instead generate a simple "goto"
		// instruction (still having the cost of the function prolog and epilog though)
		fprintf(pCOut, "\tioCPU->mCurrentRegisters[15] = 0x%08X + 4;\n", (unsigned int)inLast);
		fprintf(pCOut, "\treturn Func_0x%08X(ioCPU, ret);\n", (unsigned int)inLast);
	} else {
		// If no 
		fprintf(pCOut, "\t%s // There was no return instruction found\n", Debug);
	}
	fprintf(pCOut, "}\n");
	if (!dontLink) {
		// This line creates a ROM patch that diverts the interpreter into running the code above
		fprintf(pCOut, "T_ROM_SIMULATION3(0x%08X, \"%s\", Func_0x%08X)\n\n", (unsigned int)inFirst, inName, (unsigned int)inFirst);
	}
}


void TJITGenericRetarget::ReturnToEmulator(KUInt32 inFirst, const char *inName)
{
	pFunctionBegin = inFirst;
	fprintf(pCOut, "\n/**\n * ReturnToEmulator Stub function %s\n * ROM: 0x%08X\n */\n", inName, (unsigned int)inFirst);
	if (pHOut!=stdout)
		fprintf(pHOut, "extern void Func_0x%08X(TARMProcessor* ioCPU, KUInt32 ret); // (RET) %s\n", (unsigned int)inFirst, inName);
	fprintf(pCOut, "void Func_0x%08X(TARMProcessor* ioCPU, KUInt32)\n{\n", (unsigned int)inFirst);
	fprintf(pCOut, "\tUJITGenericRetargetSupport::ReturnToEmulator(ioCPU, 0x%08X);\n", (unsigned int)inFirst);
	fprintf(pCOut, "}\n");
}


void TJITGenericRetarget::Translate(KUInt32 inVAddr, KUInt32 inInstruction)
{
	pAction = 0;
	pWarning = 0;
	
#ifndef EINSTEIN_RETARGET
	// Make sure that injections are handled in a transparent manner
	if ((inInstruction & 0xffe00000)==0xefc00000) {
		inInstruction = TJITGenericROMPatch::GetOriginalInstructionAt(inInstruction);
	} else if ((inInstruction & 0xffe00000)==0xefa00000) {
		inInstruction = TJITGenericROMPatch::GetOriginalInstructionAt(inInstruction);
	}
#endif
	
	// Generate some crude disassmbly for comparison
	char buf[2048];
	UDisasm::Disasm(buf, 2047, inVAddr, inInstruction);
	fprintf(pCOut, "L%08X: // 0x%08X  %s\n", (unsigned int)inVAddr, (unsigned int)inInstruction, buf);
	
	fprintf(pCOut, "\tif (ioCPU->mCurrentRegisters[15]!=0x%08X+4) %s // be paranoid about a correct PC\n", (unsigned int)inVAddr, Debug);
	fprintf(pCOut, "\tioCPU->mCurrentRegisters[15] += 4; // update the PC\n");
	
	// Always generate code for a condition, so we can use local variables
	int theTestKind = inInstruction >> 28;
	PutTestBegin(theTestKind);
	
	if (pAction==0) {
		// Translate the instructionusing the original JIT source as a guideline
		if (theTestKind!=kTestNV)
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
	}
	
	if (pWarning) {
		fprintf(pCOut, "#error %s\n", pWarning);
	}
	
	// End of condition
	PutTestEnd(theTestKind);
}


void TJITGenericRetarget::TranslateSwitchCase(KUInt32 inVAddr, KUInt32 inInstruction, int inBaseReg, int inFirstCase, int inLastCase)
{
	fprintf(pCOut, "\t\t// switch/case statement 0..%d\n", inLastCase);
	fprintf(pCOut, "\t\tswitch (ioCPU->mCurrentRegisters[%d]) {\n", inBaseReg);
	if (inFirstCase==-1)
		fprintf(pCOut, "\t\t\tdefault:  SETPC(0x%08X+4); goto L%08X;\n", (unsigned int)inVAddr+4, (unsigned int)inVAddr+4);
	for (int i=0; i<=inLastCase; i++) {
		fprintf(pCOut, "\t\t\tcase %3d: SETPC(0x%08X+4); goto L%08X;\n", i, (unsigned int)inVAddr+4*i+8, (unsigned int)inVAddr+4*i+8);
	}
	fprintf(pCOut, "\t\t}\n");
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
			// Single Data Swap
			Translate_SingleDataSwap(inVAddr, inInstruction);
		} else {
			// Multiply
			Translate_Multiply(inVAddr, inInstruction);
		}
	} else {
		Translate_DataProcessingPSRTransfer(inVAddr, inInstruction);
	}
}


void TJITGenericRetarget::Translate_Multiply( KUInt32 inVAddr, KUInt32 inInstruction )
{
	// -Cond-- 0  0  0  0  0  0  A  S  --Rd--- --Rn--- --Rs--- 1  0  0  1  --Rm--- Multiply
	int FLAG_A = (inInstruction>>21)&1;
	int FLAG_S = (inInstruction>>20)&1;
	const KUInt32 Rd = (inInstruction & 0x000F0000) >> 16;
	const KUInt32 Rn = (inInstruction & 0x0000F000) >> 12;
	const KUInt32 Rs = (inInstruction & 0x00000F00) >> 8;
	const KUInt32 Rm = inInstruction & 0x0000000F;

	if (FLAG_A) { // multiply and add
		fprintf(pCOut, "\t\tconst KUInt32 theResult = (ioCPU->mCurrentRegisters[%d] * ioCPU->mCurrentRegisters[%d]) + ioCPU->mCurrentRegisters[%d];\n", (int)Rm, (int)Rs, (int)Rn);
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theResult;\n", (int)Rd);
	} else { // just multiply
		fprintf(pCOut, "\t\tconst KUInt32 theResult = ioCPU->mCurrentRegisters[%d] * ioCPU->mCurrentRegisters[%d];\n", (int)Rm, (int)Rs);
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theResult;\n", (int)Rd);
	}
	if (FLAG_S) {
		// We track status flags
		fprintf(pCOut, "\t\tif(theResult & 0x80000000) ioCPU->mCPSR_N = 1; else ioCPU->mCPSR_N = 0;\n");
		fprintf(pCOut, "\t\tif(theResult == 0) ioCPU->mCPSR_Z = 1; else ioCPU->mCPSR_Z = 0;\n");
	}
}


void TJITGenericRetarget::Translate_SingleDataSwap( KUInt32 inVAddr, KUInt32 inInstruction )
{
	// -Cond-- 0  0  0  1  0  B  0  0  --Rn--- --Rd--- 0 0 0 0 1  0  0  1  --Rm---
	int FLAG_B = (inInstruction>>22)&1;
	int Rn = (inInstruction>>16)&15;
	int Rd = (inInstruction>>12)&15;
	int Rm = (inInstruction>>0)&15;
	
	if ( (Rn!=15) || (Rd!=15) || (Rm!=15) ) {
		fprintf(pCOut, "\t\tKUInt32 theAddress = ioCPU->mCurrentRegisters[%d];\n", Rn);
		if (FLAG_B) {
			// Swap byte quantity.
			fprintf(pCOut, "\t\tKUInt8 theData = UJITGenericRetargetSupport::ManagedMemoryReadB(ioCPU, theAddress);\n");
			fprintf(pCOut, "\t\tUJITGenericRetargetSupport::ManagedMemoryWriteB(ioCPU, theAddress, (KUInt8)(ioCPU->mCurrentRegisters[%d] & 0xFF));\n", Rm);
			fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theData;\n", Rd);
		} else {
			// Swap word quantity.
			fprintf(pCOut, "\t\tKUInt32 theData = UJITGenericRetargetSupport::ManagedMemoryRead(ioCPU, theAddress);\n");
			fprintf(pCOut, "\t\tUJITGenericRetargetSupport::ManagedMemoryWrite(ioCPU, theAddress, ioCPU->mCurrentRegisters[%d]);\n", Rm);
			fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theData;\n", Rd);
		}
	}
}


void TJITGenericRetarget::DoTranslate_01(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// Single Data Transfer & Undefined
	if ((inInstruction & 0x02000010) == 0x02000010)
	{
		if ((inInstruction&0x0FFFFFFF)==0x06000510) {
			fprintf(pCOut, "\t\tUJITGenericRetargetSupport::ReturnToEmulator(ioCPU, 0x%08X); // throwSystemPanic\n", (unsigned int)inVAddr);
		} else {
			fprintf(pCOut, "\t\tUJITGenericRetargetSupport::ReturnToEmulator(ioCPU, 0x%08X); // undefined instruction\n", (unsigned int)inVAddr);
			// -Cond-- 0  1  1  -XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX- 1  -XXXXX-
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
	// -Cond-- 0  1  I  P  U  B  W  L  --Rn--- --Rd--- -----------offset----------
	
	//L003AD74C: 0xE790F101  ldr	pc, [r0, r1, lsl #2]
	if ( inInstruction==0xE790F101 && inVAddr==0x003AD74C) {
		// 0x23 jump vectors at 003AD568
		fprintf(pCOut, "\t\tswitch (ioCPU->mCurrentRegisters[1]) {\n");
		int i;
		for (i=0; i<35; i++) {
		//for (i=0; i<35; i++) {
			KUInt32 dest = 0;
			MemoryRead(0x003AD56C + 4*i, dest);
			fprintf(pCOut, "\t\t\tcase %2d: SETPC(0x%08X+4); Func_0x%08X(ioCPU, 0x003AD750+4); goto L003AD750;\n", i, (unsigned int)dest, (unsigned int)dest);
		}
		fprintf(pCOut, "\t\t}\n");
	}

	// ldr     pc, [r2, r1, lsl #2]      	@ 0x00199624 0xE792F101
	
	// special handling for reading a word form ROM
	if ( (inInstruction & 0x0fff0000) == 0x059F0000 && ((inInstruction&0x00000fff)+inVAddr+8)<0x00800000) {
		KUInt32 Rd = ((inInstruction & 0x0000F000) >> 12);
		KUInt32 theAddress = (inInstruction&0x00000fff)+inVAddr+8;
		KUInt32 theValue = 0;
		MemoryRead(theAddress, theValue);
		char sym[512], cmt[512];
		int off;
		if (pSymbolList->GetSymbolExact(theValue, sym, cmt, &off)) {
#if 0
			if (theValue>=0x0c100800 && theValue<=0x0F243000) {
				// know global variables - use a symbolic expression for simulation!
				fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = ptr_%s; // 0x%08X\n", (int)Rd, sym, (unsigned int)theValue);
			} else if (theValue>=0x0F000000 && theValue<=0x0c107e14) {
				// know hardware address range - we need to do some manual labour here!
				fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = ptr_%s; // 0x%08X\n", (int)Rd, sym, (unsigned int)theValue);
			} else {
				fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = 0x%08X; // %s\n", (int)Rd, theValue, sym);
			}
#else
			fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = 0x%08X; // %s\n", (int)Rd, (unsigned int)theValue, sym);
#endif
		} else {
			fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = 0x%08X;\n", (int)Rd, (unsigned int)theValue);
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
				fprintf(pCOut, "\t\tKUInt32 offset = GetShiftNoCarryNoR15( 0x%08X, ioCPU->mCurrentRegisters, ioCPU->mCPSR_C );\n", (unsigned int)inInstruction);
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
				fprintf(pCOut, "\t\tKUInt8 theData = UJITGenericRetargetSupport::ManagedMemoryReadB(ioCPU, theAddress);\n");
			} else {
				fprintf(pCOut, "\t\tKUInt32 theData = UJITGenericRetargetSupport::ManagedMemoryRead(ioCPU, theAddress);\n");
			}
		
			if (Rd == 15) {
				if (FLAG_B) {
					fprintf(pCOut, "\t\t#error UNPREDICTABLE CASE (R15)\n");
				} else {
					fprintf(pCOut, "\t\tSETPC(theData + 4);\n");
				}
			} else {
				fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theData;\n", (int)Rd);
			}
		} else {
			if (Rd == 15) {
				fprintf(pCOut, "\t\tKUInt32 theValue = 0x%08X + 8;\n", (unsigned int)inVAddr);
			} else {
				fprintf(pCOut, "\t\tKUInt32 theValue = ioCPU->mCurrentRegisters[%d];\n", (int)Rd);
			}
		
			if (FLAG_B) {
				fprintf(pCOut, "\t\tUJITGenericRetargetSupport::ManagedMemoryWriteB(ioCPU, theAddress, (KUInt8)(theValue & 0xFF));\n");
			} else {
				fprintf(pCOut, "\t\tUJITGenericRetargetSupport::ManagedMemoryWrite(ioCPU, theAddress, theValue);\n");
			}
		}
		
		if (WRITEBACK) {
			if (Rn == 15) {
				fprintf(pCOut, "\t\t#error UNPREDICTABLE CASE (R15)\n");
			} else {
				if (!FLAG_P) {
					if (FLAG_U) {
						fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theAddress + offset;\n", (int)Rn);
					} else {
						fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theAddress - offset;\n", (int)Rn);
					}
				} else {
					fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theAddress;\n", (int)Rn);
				}
			}
		}
		
		if (UNPRIVILEDGED) {
			fprintf(pCOut, "\t\tif (ioCPU->GetMode() != TARMProcessor::kUserMode) {\n");
			fprintf(pCOut, "\t\t\ttheMemoryInterface->SetPrivilege( true );\n");
			fprintf(pCOut, "\t\t}\n");
		}
		
		if ((Rd == 15) && FLAG_L) {
			Translate_JumpToCalculatedAddress(inVAddr, inInstruction);
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
			LogicalOp(inVAddr, inInstruction,
						 EOR, theMode, theFlagS,
						 (inInstruction & 0x000F0000) >> 16 /* Rn */,
						 (inInstruction & 0x0000F000) >> 12 /* Rd */,
						 thePushedValue);
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
			ArithmeticOp(inVAddr, inInstruction,
						 ADC, theMode, theFlagS,
						 (inInstruction & 0x000F0000) >> 16 /* Rn */,
						 (inInstruction & 0x0000F000) >> 12 /* Rd */,
						 thePushedValue);
			break;
			
		case 0x6:	// 0b01100
					// SBC
			ArithmeticOp(inVAddr, inInstruction,
						 SBC, theMode, theFlagS,
						 (inInstruction & 0x000F0000) >> 16 /* Rn */,
						 (inInstruction & 0x0000F000) >> 12 /* Rd */,
						 thePushedValue);
			break;
			
		case 0x7:	// 0b0111
					// RSC
			ArithmeticOp(inVAddr, inInstruction,
						 RSC, theMode, theFlagS,
						 (inInstruction & 0x000F0000) >> 16 /* Rn */,
						 (inInstruction & 0x0000F000) >> 12 /* Rd */,
						 thePushedValue);
			break;
			
		case 0x8:	// 0b1000
					// MRS (CPSR) & TST
			if (theFlagS == 0)
			{
				if (theMode != NoShift)
				{
					fprintf(pCOut, "#error Undefined Instruction (there is no MRS with Imm bit set or low bits set)\n");
				} else {
					Translate_MRS(inVAddr, inInstruction);
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
					Translate_MSR(inVAddr, inInstruction);
				}
			} else {
				TestOp(inVAddr, inInstruction, TEQ, theMode, (inInstruction & 0x000F0000) >> 16 /* Rn */, thePushedValue);
			}
			break;
			
		case 0xA:	// 0b1010
					// MRS (SPSR) & CMP
			if (theFlagS == 0)
			{
				if (theMode != NoShift)
				{
					fprintf(pCOut, "#error Undefined Instruction (there is no MRS with Imm bit set or low bits set)\n");
				} else {
					Translate_MRS(inVAddr, inInstruction);
				}
			} else {
				TestOp(inVAddr, inInstruction, CMP, theMode, (inInstruction & 0x000F0000) >> 16 /* Rn */, thePushedValue);
			}
			break;
			
		case 0xB:	// 0b1011
					// MSR (SPSR) & CMN
			if (theFlagS == 0)
			{
				if (theMode == Regular)
				{
					fprintf(pCOut, "#error Undefined Instruction (there is no MSR with shift)\n");
				} else {
					Translate_MSR(inVAddr, inInstruction);
				}
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


void TJITGenericRetarget::Translate_MSR(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// -Cond--  0 0 0 1  0 B 1 0  1 0 0 1  1 1 1 1  0 0 0 0  0 0 0 0  --Rm---
	// -Cond--  0 0 0 1  0 B 1 0  1 0 0 0  1 1 1 1  0 0 0 0  0 0 0 0  --Rm---
	// -Cond--  0 0 1 1  0 B 1 0  1 0 0 0  1 1 1 1  --rot--  -------imm------

	KUInt32 FLAG_I = ((inInstruction>>25)&1);
	KUInt32 FLAG_R = ((inInstruction>>22)&1);
	KUInt32 FLAG_F = ((inInstruction>>16)&1);
	KUInt32 Rm = (inInstruction&15);
	
	if (FLAG_F) { // set the entire register
		if (FLAG_I) {
			KUInt32 rot = (inInstruction>>8)&15;
			KUInt32 imm = (inInstruction&255);
			fprintf(pCOut, "\t\tconst KUInt32 Opnd2 = 0x%08X;\n", (unsigned int) imm<<(rot*2));
		} else {
			fprintf(pCOut, "\t\tconst KUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", (int)Rm);
		}
		if (FLAG_R) {
			fprintf(pCOut, "\t\tif (ioCPU->GetMode()==TARMProcessor::kUserMode) {\n");
			fprintf(pCOut, "\t\t\tconst KUInt32 oldValue = ioCPU->GetSPSR();\n");
			fprintf(pCOut, "\t\t\tioCPU->SetSPSR((Opnd2 & 0xF0000000) | (oldValue & 0x0FFFFFFF));\n");
			fprintf(pCOut, "\t\t} else {\n");
			fprintf(pCOut, "\t\t\tioCPU->SetSPSR(Opnd2);\n");
			fprintf(pCOut, "\t\t}\n");
		} else {
			fprintf(pCOut, "\t\tif (ioCPU->GetMode()==TARMProcessor::kUserMode) {\n");
			fprintf(pCOut, "\t\t\tconst KUInt32 oldValue = ioCPU->GetCPSR();\n");
			fprintf(pCOut, "\t\t\tioCPU->SetCPSR((Opnd2 & 0xF0000000) | (oldValue & 0x0FFFFFFF));\n");
			fprintf(pCOut, "\t\t} else {\n");
			fprintf(pCOut, "\t\t\tioCPU->SetCPSR(Opnd2);\n");
			fprintf(pCOut, "\t\t}\n");
		}
	} else { // set only the flags
		if (FLAG_I) {
			KUInt32 rot = (inInstruction>>16)&15;
			KUInt32 imm = (inInstruction&255);
			fprintf(pCOut, "\t\tconst KUInt32 Opnd2 = 0x%08X;\n", (unsigned int)imm<<(rot*2));
		} else {
			fprintf(pCOut, "\t\tconst KUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", (int)Rm);
		}
		if (FLAG_R) {
			fprintf(pCOut, "\t\tconst KUInt32 oldValue = ioCPU->GetSPSR();\n");
			fprintf(pCOut, "\t\tioCPU->SetSPSR((Opnd2 & 0xF0000000) | (oldValue & 0x0FFFFFFF));\n");
		} else {
			fprintf(pCOut, "\t\tconst KUInt32 oldValue = ioCPU->GetCPSR();\n");
			fprintf(pCOut, "\t\tioCPU->SetCPSR((Opnd2 & 0xF0000000) | (oldValue & 0x0FFFFFFF));\n");
		}
	}
}


void TJITGenericRetarget::Translate_MRS(KUInt32 inVAddr, KUInt32 inInstruction)
{
	KUInt32 FLAG_R = (inInstruction >> 22) & 1;
	KUInt32 Rd = (inInstruction & 0x0000F000) >> 12;
	if (Rd != 15) {
		if (FLAG_R) {
			fprintf(pCOut, "\t\tconst KUInt32 theResult = ioCPU->GetSPSR();\n");
		} else {
			fprintf(pCOut, "\t\tconst KUInt32 theResult = ioCPU->GetCPSR();\n");
		}
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theResult;\n", (int)Rd);
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
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08X + 8; // (PC)\n", (unsigned int)inVAddr);
		} else {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", (int)Rm);
		}
	} else if ((OP == TST) || (OP == TEQ)) {
		fprintf(pCOut, "\t\tBoolean carry = false;\n");
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = GetShift(ioCPU, 0x%08X, &carry, 0x%08X + 8);\n", (unsigned int)inInstruction, (unsigned int)inVAddr);
	} else {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = GetShiftNoCarry(ioCPU, 0x%08X, ioCPU->mCPSR_C, 0x%08X + 8);\n", (unsigned int)inInstruction, (unsigned int)inVAddr);
	}
	if (Rn == 15) {
		fprintf(pCOut, "\t\tKUInt32 Opnd1 = 0x%08X + 8;\n", (unsigned int)inVAddr);
	} else {
		fprintf(pCOut, "\t\tKUInt32 Opnd1 = ioCPU->mCurrentRegisters[%d];\n", (int)Rn);
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
			fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
		} else if (MODE == ImmC) {
			fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_C = ((Opnd2&0x80000000)!=0);\n");
		} else {
			fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_C = carry;\n");
		}
	} else if ((OP == CMP) || (OP == CMN)) {
		if (OP == CMP) {
			fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_C = ( ((Opnd1&~Opnd2)|(Opnd1&~theResult)|(~Opnd2&~theResult)) >> 31);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_V = ( ((Opnd1&~Opnd2&~theResult)|(~Opnd1&Opnd2&theResult)) >> 31);\n");
		} else if (OP == CMN) {
			fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_C = ( ((Opnd1&Opnd2)|((Opnd1|Opnd2)&~theResult)) >> 31);\n");
			fprintf(pCOut, "\t\tioCPU->mCPSR_V = ( ((~(Opnd1^Opnd2))&(Opnd1^theResult)) >> 31);\n");
		}
	}
}


void TJITGenericRetarget::LogicalOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 FLAG_S, KUInt32 Rn, KUInt32 Rd, KUInt32 thePushedValue)
{
	KUInt32 Rm = thePushedValue;
	if ((MODE == Imm) || (MODE == ImmC)) {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08X;\n", (unsigned int)thePushedValue);
	} else if (MODE == NoShift) {
		if (Rm == 15) {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08X + 8; // PC\n", (unsigned int)inVAddr);
		} else {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rm);
		}
	} else if (FLAG_S) {
		fprintf(pCOut, "\t\tBoolean carry = false;\n");
		fprintf(pCOut, "\t\tconst KUInt32 Opnd2 = GetShift( ioCPU, 0x%08X, &carry, 0x%08X + 8 );\n", (unsigned int)inInstruction, (unsigned int)inVAddr);
	} else {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = GetShiftNoCarry( ioCPU, 0x%08X, ioCPU->mCPSR_C, 0x%08X + 8 );\n", (unsigned int)inInstruction, (unsigned int)inVAddr);
	}
	if (Rn == 15) {
		fprintf(pCOut, "\t\tKUInt32 Opnd1 = 0x%08X + 8;\n", (unsigned int)inVAddr);
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
				fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
			} else if (MODE == ImmC) {
				fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_C = ((Opnd2&0x80000000)!=0);\n");
			} else {
				fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_C = carry;\n");
			}
		}
	}
	if (Rd == 15) {
		if (FLAG_S) {
			Translate_JumpToCalculatedAddress(inVAddr, inInstruction);
		} else {
			Translate_JumpToCalculatedAddress(inVAddr, inInstruction);
		}
	} else {
	}
}


void TJITGenericRetarget::ArithmeticOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 FLAG_S, KUInt32 Rn, KUInt32 Rd, KUInt32 thePushedValue)
{
	// FIXME: a very untypical, handcoded switch case statement:
	// add     pc, pc, r1, lsr #24             @ 0x003ADD80 0xE08FFC21
	if ( (inVAddr==0x003ADD80) && (inInstruction==0xE08FFC21) ) {
		fprintf(pCOut, "\t\t// hardcoded switch/case statement\n");
		fprintf(pCOut, "\t\tswitch (ioCPU->mCurrentRegisters[1]>>28) {\n");
		for (int i=0; i<16; i++) {
			fprintf(pCOut, "\t\t\tcase %3d: SETPC(0x%08X+4); goto L%08X;\n", i, (unsigned int)inVAddr+16*i+8, (unsigned int)inVAddr+16*i+8);
		}
		fprintf(pCOut, "\t\t}\n");
	}
		  
	// find the typical pattern for switch/case statements:
	//     0xE35x00nn  cmp    rx, #n
	//     0x908FF10x  addls  pc, pc, rx, lsl #2
	if ( (inInstruction&0xFFFFFFF0)==0x908FF100 ) {
		KUInt32 x = (inInstruction & 0x0000000F);
		KUInt32 prevInstruction = 0; MemoryRead(inVAddr-4, prevInstruction);
		if ( (prevInstruction&0xFFFFFF00)==(0xE3500000|(x<<16)) ) {
			KUInt32 n = prevInstruction & 0x000000FF;
			TranslateSwitchCase(inVAddr, inInstruction, x, -1, n);
			return;
		}
	}
	
	// generate default code
	KUInt32 Rm = thePushedValue;
	if ((MODE == Imm) || (MODE == ImmC)) {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08X;\n", (unsigned int)thePushedValue);
	} else if (MODE == NoShift) {
		if (Rm == 15) {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08X + 8; // PC\n", (unsigned int)inVAddr);
		} else {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rm);
		}
	} else {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = GetShiftNoCarry( ioCPU, 0x%08X, ioCPU->mCPSR_C, 0x%08X + 8 );\n", (unsigned int)inInstruction, (unsigned int)inVAddr);
	}
	if (Rn == 15) {
		fprintf(pCOut, "\t\tKUInt32 Opnd1 = 0x%08X + 8;\n", (unsigned int)inVAddr);
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
			if ((OP == SUB) || (OP == SBC)) {
				fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_C = ( ((Opnd1&~Opnd2)|(Opnd1&~theResult)|(~Opnd2&~theResult)) >> 31);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_V = ( ( (Opnd1&~Opnd2&~theResult)|(~Opnd1&Opnd2&theResult) ) >> 31);\n");
			} else if ((OP == RSB) || (OP == RSC)) {
				fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_C = ( ((Opnd2&~Opnd1)|(Opnd2&~theResult)|(~Opnd1&~theResult)) >> 31);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_V = ( ((Opnd2&~Opnd1&~theResult)|(~Opnd2&Opnd1&theResult)) >> 31);\n");
			} else if ((OP == ADD) || (OP == ADC)) {
				fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_C = ( ((Opnd1&Opnd2)|((Opnd1|Opnd2)&~theResult)) >> 31);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_V = ( ((~(Opnd1^Opnd2))&(Opnd1^theResult)) >> 31);\n");
			}
		}
	}
	if (Rd == 15) {
		if (FLAG_S) {
			Translate_JumpToCalculatedAddress(inVAddr, inInstruction);
		} else {
			Translate_JumpToCalculatedAddress(inVAddr, inInstruction);
		}
	} else {
//		fprintf(pCOut, "\t\tCALLNEXTUNIT;\n");
	}
}


void TJITGenericRetarget::MoveOp(KUInt32 inVAddr, KUInt32 inInstruction, KUInt32 OP, KUInt32 MODE, KUInt32 FLAG_S, KUInt32 Rd, KUInt32 thePushedValue)
{
	KUInt32 Rm = thePushedValue;
	if ((MODE == Imm) || (MODE == ImmC)) {
		fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08X;\n", (unsigned int)thePushedValue);
	} else if (MODE == NoShift) {
		if (Rm == 15) {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = 0x%08X + 8;\n", (unsigned int)inVAddr);
		} else {
			fprintf(pCOut, "\t\tKUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", (int)Rm);
		}
	} else if (FLAG_S) {
		fprintf(pCOut, "\t\tBoolean carry = false;\n");
		fprintf(pCOut, "\t\tconst KUInt32 Opnd2 = GetShift( ioCPU, 0x%08X, &carry, 0x%08X + 8 );\n", (unsigned int)inInstruction, (unsigned int)inVAddr);
	} else {
		fprintf(pCOut, "\t\tconst KUInt32 Opnd2 = GetShiftNoCarry( ioCPU, 0x%08X, ioCPU->mCPSR_C, 0x%08X + 8 );\n", (unsigned int)inInstruction, (unsigned int)inVAddr);
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
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = theResult;\n", (int)Rd);
		if (FLAG_S) {
			if ((MODE == NoShift) || (MODE == Imm)) {
				fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
			} else if (MODE == ImmC) {
				fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_C = ((Opnd2&0x80000000)!=0);\n");
			} else {
				fprintf(pCOut, "\t\tioCPU->mCPSR_Z = (theResult==0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_N = ((theResult&0x80000000)!=0);\n");
				fprintf(pCOut, "\t\tioCPU->mCPSR_C = carry;\n");
			}
		}
	}
	if (Rd == 15) {
		if (FLAG_S) {
			if ( (inInstruction&0x0FFFFFFF)==0x01B0F00E ) { // movs pc, lr
				GenerateReturnInstruction(true);
			} else {
				Translate_JumpToCalculatedAddress(inVAddr, inInstruction);
			}
		} else {
			if ( (inInstruction&0x0FFFFFFF)==0x01A0F00E ) { // mov pc, lr
				GenerateReturnInstruction(false);
			} else {
				Translate_JumpToCalculatedAddress(inVAddr, inInstruction);
			}
		}
	} else {
		// just continue
	}
}


/**
 * Generate code that returns from a simulated function into an unknown function.
 *
 * Typically, this is 'mov pc,lr'.
 *
 * \param withFlags the instruction 'movs pc,lr' may jump legally to a completely 
 *        different address than the expected one. This typically happens when a
 *        task switch occurs.
 */
void TJITGenericRetarget::GenerateReturnInstruction(bool withFlags)
{
	// This is typically called for 'mov pc, lr'
	// If the caller of this function was the emulator, simply return to the emulator
	fprintf(pCOut, "\t\tif (ret==0xFFFFFFFF)\n");
	fprintf(pCOut, "\t\t\treturn; // Return to emulator\n");
	// If the caller of this function was a simulated function, and the return address
	// is not the expected address, jump into the debugger and find the reason for this.
	fprintf(pCOut, "\t\tif (ioCPU->mCurrentRegisters[15]!=ret)\n");
	if (withFlags) {
		fprintf(pCOut, "\t\treturn UJITGenericRetargetSupport::JumpToCalculatedAddress(ioCPU, ioCPU->mCurrentRegisters[15], ret);\n");
	} else {
		fprintf(pCOut, "\t\t\t%s // Unexpected return address\n", Debug);
	}
	// If the caller was a simulated function, and the return address equals the
	// expected address, we can simply return.
	fprintf(pCOut, "\t\treturn;\n");
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


void TJITGenericRetarget::Translate_JumpToCalculatedAddress(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// We have to decide from the context if this is a true jump, or a
	// function call that returns here throu the lr register.
	KUInt32 prevInstruction;
	MemoryRead(inVAddr-4, prevInstruction);
	KUInt32 prevPrevInstruction;
	MemoryRead(inVAddr-8, prevPrevInstruction);
	if (prevInstruction==0xE1A0E00F || prevPrevInstruction==0xE28FE004) { // mov lr,pc
		// the previous instruct sets a return address, so this is a call
		fprintf(pCOut, "\t\tUJITGenericRetargetSupport::JumpToCalculatedAddress(ioCPU, ioCPU->mCurrentRegisters[15], 0x%08X);\n", (unsigned int)inVAddr+8);
	} else {
		fprintf(pCOut, "\t\treturn UJITGenericRetargetSupport::JumpToCalculatedAddress(ioCPU, ioCPU->mCurrentRegisters[15], ret);\n");
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
	KUInt32 jumpTableDest = 0, jumpTableInstr = 0;

	// Replace all jumps into jump tables with jumps to the original ROM code
	// Jump table ranges from 0x01A00000 - 0x01CFFFFF
	// Expand to Func_0x01E00000 etc. (jump table for REx)
	if (dest>=0x01A00000 && dest<0x02000000) {
		KUInt32 jtInstruction;
		MemoryRead(dest, jtInstruction);
		jumpTableDest = dest;
		jumpTableInstr = jtInstruction;
		offset = (jtInstruction & 0x007FFFFF) << 2;
		if (jtInstruction & 0x00800000)
		{
			offset |= 0xFE000000;
		}
		delta = offset + 8;
		dest = dest + delta;
		fprintf(pCOut, "\t\tKUInt32 jumpInstr = UJITGenericRetargetSupport::ManagedMemoryRead(ioCPU, 0x%08X);\n", (unsigned int)jumpTableDest);
		fprintf(pCOut, "\t\tif (jumpInstr!=0x%08X) {\n", (unsigned int)jumpTableInstr);
		fprintf(pCOut, "\t\t\t%s // unexpected jump table entry\n", Debug);
		fprintf(pCOut, "\t\t\tUJITGenericRetargetSupport::ReturnToEmulator(ioCPU, 0x%08X);\n", (unsigned int)jumpTableDest);
		fprintf(pCOut, "\t\t}\n");
	}
	
	if (inInstruction & 0x01000000)
	{
		// branch with link
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[14] = 0x%08X + 4;\n", (unsigned int)inVAddr);
		char sym[512];
		if (pSymbolList->GetSymbolExact(dest, sym)) {
			fprintf(pCOut, "\t\t// rt cjitr %s\n", sym);
		} else {
			fprintf(pCOut, "\t\t// rt cjitr %08X\n", (unsigned int)dest);
		}
		fprintf(pCOut, "\t\tSETPC(0x%08X+4);\n", (unsigned int)dest);
		fprintf(pCOut, "\t\tFunc_0x%08X(ioCPU, 0x%08X);\n", (unsigned int)dest, (unsigned int)inVAddr+8);
		fprintf(pCOut, "\t\tif (ioCPU->mCurrentRegisters[15]!=0x%08X) {\n", (unsigned int)inVAddr+8);
		fprintf(pCOut, "\t\t	RT_PANIC_UNEXPECTED_RETURN_ADDRESS\n"); // throws an exception that leaves simulation and returns to JIT
		fprintf(pCOut, "\t\t}\n");
	} else {
		KUInt32 destination = inVAddr + delta;
		if (destination<pFunctionBegin||destination>=pFunctionEnd) {
			// branch to some address outside of the current function range
			char sym[512];
			if (pSymbolList->GetSymbolExact(dest, sym)) {
				fprintf(pCOut, "\t\t// rt cjitr %s\n", sym);
			} else {
				fprintf(pCOut, "\t\t// rt cjitr %08X\n", (unsigned int)dest);
			}
			fprintf(pCOut, "\t\tSETPC(0x%08X+4);\n", (unsigned int)dest);
			fprintf(pCOut, "\t\treturn Func_0x%08X(ioCPU, ret);\n", (unsigned int)dest);
		} else {
			fprintf(pCOut, "\t\tSETPC(0x%08X+4);\n", (unsigned int)dest);
			fprintf(pCOut, "\t\tgoto L%08X;\n", (unsigned int)dest);
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
				// LDM2 ( ldmneia r0, {r0-lr}^ )
				Translate_BlockDataTransfer_LDM2(inVAddr, inInstruction);
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
			Translate_BlockDataTransfer_STM2(inVAddr, inInstruction);
		} else {
			// STM1
			Translate_BlockDataTransfer_STM1(inVAddr, inInstruction);
		}
	}
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
	fprintf(pCOut, "\t\tKUInt32 baseAddress = ioCPU->mCurrentRegisters[%d];\n", (int)Rn);
	
	if (FLAG_W) { // Write back
		if (FLAG_U) {
			fprintf(pCOut, "\t\tKUInt32 wbAddress = baseAddress + (%d * 4);\n", (int)nbRegisters);
		} else {
			fprintf(pCOut, "\t\tKUInt32 wbAddress = baseAddress - (%d * 4);\n", (int)nbRegisters);
		}
	}
	
	if (FLAG_U) { // Up.
		if (FLAG_P) { // Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	} else {	// Down.
		fprintf(pCOut, "\t\tbaseAddress -= (%d * 4);\n", (int)nbRegisters);
		if (!FLAG_P) { // Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	}
	int indexReg = 0;
	while (curRegList) {
		if (curRegList & 1) {
			fprintf(pCOut, "\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[%d]);\n", indexReg);
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
		curRegList >>= 1;
		indexReg++;
	}
	if (theRegList & 0x8000)
	{
		// PC is special.
		// Stored value is PC + 12 (verified)
		fprintf(pCOut, "\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, 0x%08X + 12);\n", (unsigned int)inVAddr);
	}
	
	if (FLAG_W) {
	// Write back.
	// Rn == 15 -> UNPREDICTABLE
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = wbAddress;\n", (int)Rn);
	}
}


void TJITGenericRetarget::Translate_BlockDataTransfer_STM2(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// example: stmneia	r1, {r8-lr}^
	KUInt32 FLAG_P = (inInstruction>>24) & 1;
	KUInt32 FLAG_U = (inInstruction>>23) & 1;
	//KUInt32 FLAG_W = (inInstruction>>21) & 1;
	KUInt32 Rn = (inInstruction>>16) & 0x0F;
	KUInt32 theRegList = inInstruction & 0xFFFF;
	//KUInt32 curRegList = theRegList & 0x7FFF;
	KUInt32 nbRegisters = CountBits(theRegList);
	
	if (Rn==15) fprintf(pCOut, "#error Rn == 15 -> UNPREDICTABLE\n");
	
	fprintf(pCOut, "\t\tKUInt32 curRegList;\n");
	fprintf(pCOut, "\t\tKUInt32 bankRegList;\n");
	
	// Use bank and current registers.
	fprintf(pCOut, "\t\tif (ioCPU->GetMode() == TARMProcessor::kFIQMode) {\n");
	fprintf(pCOut, "\t\t\tcurRegList = 0x%04X & 0xFF;\n", (unsigned int)theRegList);
	fprintf(pCOut, "\t\t\tbankRegList = 0x%04X & 0x7F00;\n", (unsigned int)theRegList);
	fprintf(pCOut, "\t\t} else {\n");
	fprintf(pCOut, "\t\t\tcurRegList = 0x%04X & 0x1FFF;\n", (unsigned int)theRegList);
	fprintf(pCOut, "\t\t\tbankRegList = 0x%04X & 0x6000;\n", (unsigned int)theRegList);
	fprintf(pCOut, "\t\t}\n");
	fprintf(pCOut, "\t\tKUInt32 baseAddress = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rn);
	
	if (FLAG_U) {
		// Up.
		if (FLAG_P) {
			// Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	} else {
		// Down.
		fprintf(pCOut, "\t\tbaseAddress -= (%d * 4);\n", (unsigned int)nbRegisters);
		
		if (!FLAG_P) {
			// Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	}
	
	// Store.
	fprintf(pCOut, "\t\tif (curRegList) {\n");
	if (theRegList & 0x0001) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0001) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[0]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0002) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0002) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[1]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0004) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0004) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[2]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0008) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0008) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[3]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0010) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0010) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[4]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0020) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0020) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[5]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0040) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0040) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[6]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0080) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0080) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[7]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0100) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0100) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[8]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0200) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0200) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[9]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0400) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0400) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[10]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0800) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0800) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[11]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x1000) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x1000) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mCurrentRegisters[12]);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	fprintf(pCOut, "\t\t}\n");
	
	
	fprintf(pCOut, "\t\tif (bankRegList) {\n");
	if (theRegList & 0x0100) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x0100) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mR8_Bkup);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0200) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x0200) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mR9_Bkup);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0400) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x0400) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mR10_Bkup);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0800) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x0800) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mR11_Bkup);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x1000) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x1000) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mR12_Bkup);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x2000) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x2000) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mR13_Bkup);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x4000) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x4000) {\n");
		fprintf(pCOut, "\t\t\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, ioCPU->mR14_Bkup);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	fprintf(pCOut, "\t\t}\n");

	if (theRegList & 0x8000)
	{
		// PC is special.
		// Stored value is PC + 12 (verified)
		fprintf(pCOut, "\t\tUJITGenericRetargetSupport::ManagedMemoryWriteAligned(ioCPU, baseAddress, 0x%08X + 12);\n", (unsigned int)inVAddr);
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
	fprintf(pCOut, "\t\tKUInt32 baseAddress = ioCPU->mCurrentRegisters[%d];\n", (int)Rn);
	if (FLAG_W) { // Write back
		if (FLAG_U) {
			fprintf(pCOut, "\t\tKUInt32 wbAddress = baseAddress + (%d * 4);\n", (int)nbRegisters);
		} else {
			fprintf(pCOut, "\t\tKUInt32 wbAddress = baseAddress - (%d * 4);\n", (int)nbRegisters);
		}
	}
	
	if (FLAG_U) { // Up.
		if (FLAG_P) { // Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	} else { // Down.
		fprintf(pCOut, "\t\tbaseAddress -= (%d * 4);\n", (int)nbRegisters);
		if (!FLAG_P) { // Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	}
	
	int indexReg = 0;
	while (curRegList) {
		if (curRegList & 1) {
			fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n", indexReg);
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
		curRegList >>= 1;
		indexReg++;
	}
	
	if (theRegList & 0x8000)
	{
		// PC is special.
		fprintf(pCOut, "\t\tSETPC( UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress)) + 4;\n");
		// don't mark as error because this is usually used to return from a function
	}
	
	if (FLAG_W) {
	// Write back.
	// Rn == 15 -> UNPREDICTABLE
		fprintf(pCOut, "\t\tioCPU->mCurrentRegisters[%d] = wbAddress;\n", (int)Rn);
	}
	
	if (theRegList & 0x8000)
	{
		fprintf(pCOut, "\t\treturn; //MMUCALLNEXT_AFTERSETPC;\n");
	} else {
		//CALLNEXTUNIT;
	}
}


void TJITGenericRetarget::Translate_BlockDataTransfer_LDM2(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer
	KUInt32 FLAG_P = (inInstruction>>24) & 1;
	KUInt32 FLAG_U = (inInstruction>>23) & 1;
//	KUInt32 FLAG_W = (inInstruction>>21) & 1;
	KUInt32 Rn = (inInstruction>>16) & 0x0F;
	KUInt32 theRegList = inInstruction & 0xFFFF;
//	KUInt32 curRegList = theRegList & 0x7FFF;
	KUInt32 nbRegisters = CountBits(theRegList);
	if (Rn==15) fprintf(pCOut, "#error Rn == 15 -> UNPREDICTABLE\n");

	fprintf(pCOut, "\t\tKUInt32 curRegList;\n");
	fprintf(pCOut, "\t\tKUInt32 bankRegList;\n");
	
	// Use bank and current registers.
	fprintf(pCOut, "\t\tif (ioCPU->GetMode() == TARMProcessor::kFIQMode) {\n");
	fprintf(pCOut, "\t\t\tcurRegList = 0x%04X & 0xFF;\n", (unsigned int)theRegList);
	fprintf(pCOut, "\t\t\tbankRegList = 0x%04X & 0x7F00;\n", (unsigned int)theRegList);
	fprintf(pCOut, "\t\t} else {\n");
	fprintf(pCOut, "\t\t\tcurRegList = 0x%04X & 0x1FFF;\n", (unsigned int)theRegList);
	fprintf(pCOut, "\t\t\tbankRegList = 0x%04X & 0x6000;\n", (unsigned int)theRegList);
	fprintf(pCOut, "\t\t}\n");
	fprintf(pCOut, "\t\tKUInt32 baseAddress = ioCPU->mCurrentRegisters[%d];\n", (unsigned int)Rn);
	
	if (FLAG_U) {
		// Up.
		if (FLAG_P) {
			// Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	} else {
		// Down.
		fprintf(pCOut, "\t\tbaseAddress -= (%d * 4);\n", (unsigned int)nbRegisters);
		
		if (!FLAG_P) {
			// Post: add 4.
			fprintf(pCOut, "\t\tbaseAddress += 4;\n");
		}
	}
	
	// Load.
	fprintf(pCOut, "\t\tif (curRegList) {\n");
	if (theRegList & 0x0001) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0001) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[0] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0002) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0002) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[1] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0004) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0004) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[2] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0008) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0008) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[3] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0010) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0010) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[4] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0020) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0020) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[5] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0040) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0040) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[6] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0080) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0080) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[7] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0100) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0100) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[8] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0200) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0200) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[9] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0400) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0400) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[10] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0800) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x0800) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[11] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x1000) {
		fprintf(pCOut, "\t\t\tif (curRegList & 0x1000) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mCurrentRegisters[12] = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	fprintf(pCOut, "\t\t}\n");

	fprintf(pCOut, "\t\tif (bankRegList) {\n");
	if (theRegList & 0x0100) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x0100) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mR8_Bkup = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0200) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x0200) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mR9_Bkup = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0400) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x0400) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mR10_Bkup = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x0800) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x0800) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mR11_Bkup = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x1000) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x1000) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mR12_Bkup = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x2000) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x2000) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mR13_Bkup = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	if (theRegList & 0x4000) {
		fprintf(pCOut, "\t\t\tif (bankRegList & 0x4000) {\n");
		fprintf(pCOut, "\t\t\t\tioCPU->mR14_Bkup = UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress);\n");
		fprintf(pCOut, "\t\t\t\tbaseAddress += 4;\n");
		fprintf(pCOut, "\t\t\t}\n");
	}
	fprintf(pCOut, "\t\t}\n");

	if (theRegList & 0x8000)
	{
		// PC is special.
		fprintf(pCOut, "\t\tSETPC( UJITGenericRetargetSupport::ManagedMemoryReadAligned(ioCPU, baseAddress)) + 4;\n");
		// don't mark as error because this is usually used to return from a function
		fprintf(pCOut, "#warn this is setting an arbitrary return address! Does this work in simulation?\n");
	}
	
	if (theRegList & 0x8000)
	{
		fprintf(pCOut, "\t\treturn;\n");
	} else {
	}

}


void TJITGenericRetarget::DoTranslate_11(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// -Cond-- 1  1  0  P  U  N  W  L  --Rn--- --CRd-- --CP#-- -----offset-------- Coproc Data Transfer
	// -Cond-- 1  1  1  0  --CP Opc--- --CRn-- --CRd-- --CP#-- ---CP--- 0  --CRm-- Coproc Data Operation
	// -Cond-- 1  1  1  0  -CPOpc-- L  --CRn-- --Rd--- --CP#-- ---CP--- 1  --CRm-- Coproc Register Transfer
	// -Cond-- 1  1  1  1  -----------------ignored by processor------------------ Software Interrupt
	// Extension for native calls:
	// -Cond-- 1  1  1  1  1  0  ---------------------index----------------------- Call Native Code at Index
	// -Cond-- 1  1  1  1  1  1  ---------------------index----------------------- Call Injection at Index
	if (inInstruction & 0x02000000)
	{
		if (inInstruction & 0x01000000)
		{
			if ((inInstruction & 0x00C00000)==0x00800000) {
				// quick host native call
				fprintf(pCOut, "#error Not yet implemented 14.1\n");
//				PUSHFUNC(CallHostNative);
//				PUSHVALUE(inInstruction & 0x003fffff);
				return; // do not push the current PC
			} else {
				// SWI.
				fprintf(pCOut, "\t\tioCPU->DoSWI();\n");
				fprintf(pCOut, "\t\tFunc_0x00000008(ioCPU, 0x%08X);\n", (unsigned int)inVAddr+8);
				fprintf(pCOut, "\t\tif (ioCPU->mCurrentRegisters[15]!=0x%08X) {\n", (unsigned int)inVAddr+8);
				fprintf(pCOut, "\t\t\tRT_PANIC_UNEXPECTED_RETURN_ADDRESS\n");
				fprintf(pCOut, "\t\t}\n");
			}
		} else {
			if (inInstruction & 0x00000010)
			{
				CoprocRegisterTransfer(inVAddr, inInstruction);
			} else {
				fprintf(pCOut, "\t\tioCPU->DoUndefinedInstruction(); // (1)\n");
				fprintf(pCOut, "\t\tFunc_0x00000004(ioCPU, 0x%08X);\n", (unsigned int)inVAddr+8);
				fprintf(pCOut, "\t\tif (ioCPU->mCurrentRegisters[15]!=0x%08X) {\n", (unsigned int)inVAddr+8);
				fprintf(pCOut, "\t\t\tRT_PANIC_UNEXPECTED_RETURN_ADDRESS\n");
				fprintf(pCOut, "\t\t}\n");
			}
		}
	} else {
		fprintf(pCOut, "\t\tioCPU->DoUndefinedInstruction(); // (2)\n");
		fprintf(pCOut, "\t\tFunc_0x00000004(ioCPU, 0x%08X);\n", (unsigned int)inVAddr+8);
		fprintf(pCOut, "\t\tif (ioCPU->mCurrentRegisters[15]!=0x%08X) {\n", (unsigned int)inVAddr+8);
		fprintf(pCOut, "\t\t\tRT_PANIC_UNEXPECTED_RETURN_ADDRESS\n");
		fprintf(pCOut, "\t\t}\n");
	}
}


// -------------------------------------------------------------------------- //
//  * CoprocRegisterTransfer
// -------------------------------------------------------------------------- //
void TJITGenericRetarget::CoprocRegisterTransfer(KUInt32 inVAddr, KUInt32 inInstruction)
{
	// -Cond-- 1  1  1  0  -CPOpc-- L  --CRn-- --Rd--- --CP#-- ---CP--- 1  --CRm-- Coproc Register Transfer
	// FIXME: this could read the PC register. Be smart about it!
	fprintf(pCOut, "\t\tSETPC(0x%08X+8);\n", (unsigned int)inVAddr);
	KUInt32 CPNumber = (inInstruction & 0x00000F00) >> 8;
	if (CPNumber == 0xF) {
		fprintf(pCOut, "\t\tioCPU->SystemCoprocRegisterTransfer(0x%08X);\n", (unsigned int)inInstruction);
	} else if (CPNumber == 10) {
		// Native primitives.
		fprintf(pCOut, "\t\tioCPU->NativeCoprocRegisterTransfer(0x%08X);\n", (unsigned int)inInstruction);
	} else {
		fprintf(pCOut, "\t\tioCPU->DoUndefinedInstruction(); // (3)\n");
		fprintf(pCOut, "\t\tFunc_0x00000004(ioCPU, 0x%08X);\n", (unsigned int)inVAddr+8);
		fprintf(pCOut, "\t\tif (ioCPU->mCurrentRegisters[15]!=0x%08X) {\n", (unsigned int)inVAddr+8);
		fprintf(pCOut, "\t\t\tRT_PANIC_UNEXPECTED_RETURN_ADDRESS\n");
		fprintf(pCOut, "\t\t}\n");
	}
	// FIXME: this could write the PC register
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
