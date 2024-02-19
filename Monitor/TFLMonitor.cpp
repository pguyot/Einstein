// ==============================
// File:            TFLMonitor.cp
// Project:            Einstein
//
// Copyright 2003-2007 by Matthias Melcher
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

#include "TFLMonitor.h"
#include "TSymbolList.h"
#include "UDisasm.h"

#include "Emulator/TARMProcessor.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Terminal.H>
#include <FL/fl_draw.H>
#include <FL/platform.H>

// Random Note:
// NewtonScript: get next bytecode: ldrb    r0, [r6], #1 @ 0x002EE1D8

class TFLTerminal : public Fl_Terminal
{
	std::vector<std::string> history;
	std::vector<std::string> future;
	std::string cmd;

public:
	TFLTerminal(int x, int y, int w, int h, const char* l = nullptr) :
			Fl_Terminal(x, y, w, h, l) { }
	int
	handle(int e) FL_OVERRIDE
	{
		char c;
		switch (e)
		{
			case FL_KEYBOARD:
				c = Fl::event_text()[0];
				if (c < 127 && c > 31 && isprint(c) && !Fl::event_command())
				{
					cmd += Fl::event_text();
					this->printf("%s", Fl::event_text());
				} else if (Fl::event_key() == FL_Enter)
				{
					do_callback();
					history.push_back(cmd);
					future.clear();
					cmd.clear();
					this->printf("\n");
				} else if (Fl::event_key() == FL_Escape)
				{
					cursor_left((int) cmd.length());
					this->printf("\033[K");
					cmd.clear();
				} else if (Fl::event_key() == FL_Up)
				{
					if (!history.empty())
					{
						if (!cmd.empty())
							future.push_back(cmd);
						cursor_left((int) cmd.length());
						this->printf("\033[K");
						cmd = history.back();
						history.pop_back();
						this->printf("%s", cmd.c_str());
					}
				} else if (Fl::event_key() == FL_Down)
				{
					if (!cmd.empty())
						history.push_back(cmd);
					cursor_left((int) cmd.length());
					this->printf("\033[K");
					if (future.empty())
					{
						cmd.clear();
					} else
					{
						cmd = future.back();
						future.pop_back();
					}
					this->printf("%s", cmd.c_str());
				} else if (Fl::event_key() == FL_BackSpace)
				{
					if (!cmd.empty())
					{
						cmd.pop_back();
						this->printf("\033[1D\033[K");
					}
				} else if (Fl::event_key() == 'v' && Fl::event_command())
				{
					Fl::paste(*this);
				} else
				{
					return Fl_Terminal::handle(e);
				}
				return 1;
			case FL_PASTE:
				cmd += Fl::event_text();
				this->printf("%s", Fl::event_text());
				return 1;
		}
		return Fl_Terminal::handle(e);
	}
	const char*
	value()
	{
		return cmd.c_str();
	}
};

TFLMonitor::TFLMonitor(TBufferLog* inLog,
	TEmulator* inEmulator,
	TSymbolList* inSymbolList,
	const char* inROMPath) :
		TMonitor(inLog, inEmulator, inSymbolList, inROMPath)
{
}

TFLMonitor::~TFLMonitor()
{
	Hide();
	delete mwWindow;
}

// -------------------------------------------------------------------------- //
// DrawScreen( void )
// -------------------------------------------------------------------------- //
Boolean
TFLMonitor::DrawScreen()
{
	if (!mwTerminal)
		return false;

	bool theResult = false;
	if (IsHalted())
	{
		if (!IsLastScreenHalted())
		{
			mwTerminal->clear();
			theResult = true;
		}
		SetLastScreenHalted(true);
		DrawScreenHalted();
	} else
	{
		if (IsLastScreenHalted())
		{
			mwTerminal->clear();
			theResult = true;
		}
		SetLastScreenHalted(false);
		DrawScreenRunning();
	}
	mwTerminal->redraw();
	return theResult;
}

// -------------------------------------------------------------------------- //
// DrawScreenHalted( void )
// -------------------------------------------------------------------------- //
void
TFLMonitor::DrawScreenHalted()
{
	Fl::lock();
	KUInt32 realPC = GetProcessor()->GetRegister(15) - 4;

	// Go to the uppermost position.
	mwTerminal->clear_screen_home();
	int indexRegisters;
	for (indexRegisters = 0; indexRegisters < 16; indexRegisters++)
	{
		mwTerminal->printf("%s= %.8X | %s\n",
			kRegisterNames[indexRegisters],
			(unsigned int) GetProcessor()->GetRegister(indexRegisters),
			GetLog()->GetLine(indexRegisters));
	}
	KUInt32 theCPSR = GetProcessor()->GetCPSR();
	KUInt32 theMode = theCPSR & TARMProcessor::kPSR_ModeMask;
	mwTerminal->printf("%c%c%c%c %c%c%c %s  | %s\n",
		theCPSR & TARMProcessor::kPSR_NBit ? 'N' : 'n',
		theCPSR & TARMProcessor::kPSR_ZBit ? 'Z' : 'z',
		theCPSR & TARMProcessor::kPSR_CBit ? 'C' : 'c',
		theCPSR & TARMProcessor::kPSR_VBit ? 'V' : 'v',
		theCPSR & TARMProcessor::kPSR_IBit ? 'I' : 'i',
		theCPSR & TARMProcessor::kPSR_FBit ? 'F' : 'f',
		theCPSR & TARMProcessor::kPSR_TBit ? 'T' : 't',
		kModesNames[theMode],
		GetLog()->GetLine(16));
	KUInt32 theSPSR = 0;
	if ((theMode != TARMProcessor::kSystemMode)
		&& (theMode != TARMProcessor::kUserMode))
	{
		theSPSR = GetProcessor()->GetSPSR();
	}
	if (theSPSR == 0)
	{
		mwTerminal->printf("---- --- ---  | %s\n",
			GetLog()->GetLine(17));
	} else
	{
		mwTerminal->printf("%c%c%c%c %c%c%c %s  | %s\n",
			theSPSR & TARMProcessor::kPSR_NBit ? 'N' : 'n',
			theSPSR & TARMProcessor::kPSR_ZBit ? 'Z' : 'z',
			theSPSR & TARMProcessor::kPSR_CBit ? 'C' : 'c',
			theSPSR & TARMProcessor::kPSR_VBit ? 'V' : 'v',
			theSPSR & TARMProcessor::kPSR_IBit ? 'I' : 'i',
			theSPSR & TARMProcessor::kPSR_FBit ? 'F' : 'f',
			theSPSR & TARMProcessor::kPSR_TBit ? 'T' : 't',
			kModesNames[theSPSR & TARMProcessor::kPSR_ModeMask],
			GetLog()->GetLine(17));
	}

	mwTerminal->printf("==============| %s\n",
		GetLog()->GetLine(18));

	mwTerminal->printf("Tmr= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetFrozenTimer(),
		GetLog()->GetLine(19));

	mwTerminal->printf("TM0= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetTimerMatchRegister(0),
		GetLog()->GetLine(20));

	mwTerminal->printf("TM1= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetTimerMatchRegister(1),
		GetLog()->GetLine(21));

	mwTerminal->printf("TM2= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetTimerMatchRegister(2),
		GetLog()->GetLine(22));

	mwTerminal->printf("TM3= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetTimerMatchRegister(3),
		GetLog()->GetLine(23));

	mwTerminal->printf("RTC= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetRealTimeClock(),
		GetLog()->GetLine(24));

	mwTerminal->printf("Alm= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetAlarm(),
		GetLog()->GetLine(25));

	mwTerminal->printf("IR = %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetIntRaised(),
		GetLog()->GetLine(26));

	mwTerminal->printf("ICR= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetIntCtrlReg(),
		GetLog()->GetLine(27));

	mwTerminal->printf("FM = %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetFIQMask(),
		GetLog()->GetLine(28));

	mwTerminal->printf("IC1= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetIntEDReg1(),
		GetLog()->GetLine(29));

	mwTerminal->printf("IC2= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetIntEDReg2(),
		GetLog()->GetLine(30));

	mwTerminal->printf("IC3= %.8X | %s\n",
		(unsigned int) GetInterruptManager()->GetIntEDReg3(),
		GetLog()->GetLine(31));

	mwTerminal->printf("-------------------------------------------------------------------------------\n");

	if (realPC == 0x002EE1D8)
	{
		// byte code interpreter: TInterpreter::FastRun1()
		// r4 = FastRunState, r4 + 0x01c points to the literals slot array
		// +0  0x00: pointer to current byte code
		// +4  0x04: ref ref to function name?
		// +12 0x0C: ref to function name?
		// +16 0x10: ref to binary class
		// +20 0x14: Magic Pointer @287
		// +24 0x18: ref to function frame
		// +28 0x1c: pointer array of literals (first literal is function name?)
		// +32 0x20: start address of byte code
		//
		// r5 = this (TInterpreter*)
		// r6 points to the current instruction
		// r7 points one after the top element on the stack, stack is growing up
		// mwTerminal->printf("%s%.8X Memory Error [%.8X]\n",
		// TMonitor::FormatNSRef(char* buffer, size_t bufferSize, KUInt32 inRef, int indent, int maxDepth)

		//		struct VMState
		//		{
		//			Ref		pc;			// +00 - int
		//			Ref		func;			// +04 - frame
		//			Ref		locals;		// +08 - array
		//			Ref		impl;			// +0C - frame
		//			Ref		rcvr;			// +10 - frame
		//			Ref		stackFrame;	// +14 - int stack index + flags
		//		};

		//		Locals index:
		//		■ _nextArgFrame
		//		■ _parent
		//		■ _implementor
		//		■ arguments, from left to right
		//		■ local variables

		int indexLines;
		char theLiteral[200];
		char theLine[16384]; // 768, but we have an overflow error that we need to find!
		VAddr ip = mProcessor->GetRegister(6);
		VAddr state = mProcessor->GetRegister(4);
		VAddr bc_start = 0;
		mMemory->Read(state + 0x20, bc_start);
		//		VAddr interpreter = mProcessor->GetRegister(6);
		for (indexLines = 0; indexLines < 5; indexLines++)
		{
			VAddr oip = ip;
			KUInt8 a = 0;
			if (mMemory->ReadB(ip++, a))
			{
				mwTerminal->printf("%s%.8X Memory Error [%.8X]\n",
					kEraseLine,
					(unsigned int) ip,
					(unsigned int) mMemory->GetFaultStatusRegister());
			} else
			{
				KUInt16 b = a & 7;
				KSInt16 sb = 0;
				if (b == 7)
				{
					KUInt8 ah, al;
					mMemory->ReadB(ip++, ah);
					mMemory->ReadB(ip++, al);
					b = (ah << 8) | al;
					sb = (ah << 8) | al;
				}
				mwTerminal->printf("%.8X%5d: ", (unsigned int) oip, oip - bc_start);
				switch (a & ~7)
				{
					case 0000:
						switch (b)
						{
							case 0:
								mwTerminal->printf("%03o        pop (%s)\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "x");
								break;
							case 1:
								mwTerminal->printf("%03o        dup (%s)\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "x");
								break;
							case 2:
								mwTerminal->printf("%03o        return\n", a);
								break;
							case 3:
								mwTerminal->printf("%03o        push-self\n", a);
								break;
							case 4:
								mwTerminal->printf("%03o        set-lex-scope %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "func");
								break;
							case 5:
								mwTerminal->printf("%03o        iter-next %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "iter");
								break;
							case 6:
								mwTerminal->printf("%03o        iter-done %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "iter");
								break;
							case 7:
								mwTerminal->printf("%03o        pop-handler\n", a);
								break;
							default:
								mwTerminal->printf("%03o        <undefined>\n", a);
								break;
						}
						break;
					case 0030:
						mwTerminal->printf("%03o %6d push %s\n", a, b, FormatNSLiteral(theLine, 767, b));
						break;
					case 0040:
						FormatNSRef(theLine, 767, sb, 0, 1);
						mwTerminal->printf("%03o %6d push-constant %s\n", a, b, theLine);
						break;
					case 0050:
						mwTerminal->printf("%03o %6d call %s\n", a, b, indexLines == 0 ? FormatNSCallArgs(theLine, 767, b, 1) : "...");
						break;
					case 0060:
						mwTerminal->printf("%03o %6d invoke %s\n", a, b, indexLines == 0 ? FormatNSCallArgs(theLine, 767, b, 1) : "...");
						break;
					case 0070:
						mwTerminal->printf("%03o %6d send %s\n", a, b, indexLines == 0 ? FormatNSCallArgs(theLine, 767, b, 2) : "recvr name");
						break;
					case 0100:
						mwTerminal->printf("%03o %6d send-if-defined %s\n", a, b, indexLines == 0 ? FormatNSCallArgs(theLine, 767, b, 2) : "recvr name");
						break;
					case 0110:
						mwTerminal->printf("%03o %6d resend %s\n", a, b, indexLines == 0 ? FormatNSCallArgs(theLine, 767, b, 1) : "...");
						break;
					case 0120:
						mwTerminal->printf("%03o %6d resend-if-defined %s\n", a, b, indexLines == 0 ? FormatNSCallArgs(theLine, 767, b, 1) : "...");
						break;
					case 0130:
						mwTerminal->printf("%03o %6d branch %d\n", a, b, b);
						break;
					case 0140:
						mwTerminal->printf("%03o %6d branch-if-true %d (%s)\n", a, b, b, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "bool");
						break;
					case 0150:
						mwTerminal->printf("%03o %6d branch-if-false %d (%s)\n", a, b, b, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "bool");
						break;
					case 0160:
						mwTerminal->printf("%03o %6d find-var %s\n", a, b, FormatNSLiteral(theLine, 767, b));
						break;
					case 0170:
						mwTerminal->printf("%03o %6d get-var local%04d\n", a, b, b);
						break; // TODO: locals array [b]
					case 0200:
						mwTerminal->printf("%03o %6d make-frame %s\n", a, b, indexLines == 0 ? FormatNSCallArgs(theLine, 767, b, 1) : "map val1 val2 valN");
						break; // TODO: val1 val2 valN map
					case 0210:
						if (b == 0xffff)
							mwTerminal->printf("%03o %6d make-array-with-size %s\n", a, b, indexLines == 0 ? FormatNSCallArgs(theLine, 767, 1, 1) : "class size");
						else
							mwTerminal->printf("%03o %6d make-array\n", a, b, indexLines == 0 ? FormatNSCallArgs(theLine, 767, b, 1) : "class val1 val2 valN");
						break;
					case 0220:
						switch (b)
						{
							case 0:
								mwTerminal->printf("%03o        get-path %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "object pathExpr");
								break;
							case 1:
								mwTerminal->printf("%03o        get-path-no-nil %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "object pathExpr");
								break;
							default:
								mwTerminal->printf("%03o        <undefined>\n", a);
								break;
						}
						break;
					case 0230:
						switch (b)
						{
							case 0:
								mwTerminal->printf("%03o        set-path %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 3) : "object pathExpr value");
								break;
							case 1:
								mwTerminal->printf("%03o        set-path-and-push %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 3) : "object pathExpr value");
								break;
							default:
								mwTerminal->printf("%03o        <undefined>\n", a);
								break;
						}
						break;
					case 0240:
						mwTerminal->printf("%03o %6d set-var local%04d %s\n", a, b, b, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "value");
						break; // TODO: locals array [b]
					case 0250:
						mwTerminal->printf("%03o %6d find-and-set-var %s %s\n", a, b, FormatNSLiteral(theLiteral, 199, b), indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "value");
						break; // TODO: literals array [b]
					case 0260:
						mwTerminal->printf("%03o %6d incr-var local%04d %s\n", a, b, b, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "addend");
						break; // TODO: locals array [b]
					case 0270:
						mwTerminal->printf("%03o %6d branch-if-loop-not-done %d %s\n", a, b, b, indexLines == 0 ? FormatNSValueStack(theLine, 767, 3) : "incr index limit");
						break;
					case 0300:
						switch (b)
						{
							case 0:
								mwTerminal->printf("%03o        add %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 1:
								mwTerminal->printf("%03o        subtract %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 2:
								mwTerminal->printf("%03o        aref %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "object index");
								break;
							case 3:
								mwTerminal->printf("%03o        set-aref %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 3) : "object index element");
								break;
							case 4:
								mwTerminal->printf("%03o        equals %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 5:
								mwTerminal->printf("%03o        not %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "x");
								break;
							case 6:
								mwTerminal->printf("%03o        not-equals %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 7:
								mwTerminal->printf("%03o        mul %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 8:
								mwTerminal->printf("%03o        divide %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 9:
								mwTerminal->printf("%03o        div %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 10:
								mwTerminal->printf("%03o        less-than %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 11:
								mwTerminal->printf("%03o        greater-than %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 12:
								mwTerminal->printf("%03o        greater-or-equal %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 13:
								mwTerminal->printf("%03o        less-or-equal %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 14:
								mwTerminal->printf("%03o        bit-and %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 15:
								mwTerminal->printf("%03o        bit-or %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 16:
								mwTerminal->printf("%03o        bit-not %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "x y");
								break;
							case 17:
								mwTerminal->printf("%03o        new-iter %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "object deeply");
								break;
							case 18:
								mwTerminal->printf("%03o        length %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "object");
								break;
							case 19:
								mwTerminal->printf("%03o        clone %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "object");
								break;
							case 20:
								mwTerminal->printf("%03o        set-class %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "object class");
								break;
							case 21:
								mwTerminal->printf("%03o        add-array-slot %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "array object");
								break;
							case 22:
								mwTerminal->printf("%03o        stringer %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "array");
								break;
							case 23:
								mwTerminal->printf("%03o        has-path %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2) : "object pathExpr");
								break;
							case 24:
								mwTerminal->printf("%03o        class-of %s\n", a, indexLines == 0 ? FormatNSValueStack(theLine, 767, 1) : "object");
								break;
							default:
								mwTerminal->printf("%03o %6d freq-func #\n", a, b);
								break;
						}
						break;
					case 0310:
						mwTerminal->printf("%03o %6d new-handlers %s\n", a, b, indexLines == 0 ? FormatNSValueStack(theLine, 767, 2 * b) : "sym1 pc1 ...");
						break;
					default:
						mwTerminal->printf("%03o        <undefined>\n", a);
						break;
				}
			}
			if ((a == 0002) || ((a & ~7) == 0130))
				break; // branch or return
		}
	} else
	{
		char theInstr[512];
		theInstr[0] = 0;

		char theSymbol[512];
		char theComment[512];
		int theOffset;
		char theLine[768];

		if (mSymbolList)
		{
			// Get the symbol.
			mSymbolList->GetNearestSymbolByAddress(
				realPC,
				theSymbol,
				theComment,
				&theOffset);
			mwTerminal->printf("%s+%X\n", theSymbol, theOffset);
		} else
		{
			theSymbol[0] = '\0';
			theComment[0] = '\0';
			theLine[0] = '\0';
			theOffset = 0;
			mwTerminal->printf("%X\n", realPC);
		}

		KUInt32 instruction;

		// Write 5 lines.
		int indexLines;
		for (indexLines = 0; indexLines < 20; indexLines += 4)
		{
			if (mMemory->Read(
					(TMemory::VAddr) realPC + indexLines, instruction))
			{
				mwTerminal->printf(
					"%.8X Memory Error [%.8X]\n",
					(unsigned int) realPC + indexLines,
					(unsigned int) mMemory->GetFaultStatusRegister());
			} else
			{
				bool instIsBP = false;
				if ((instruction & 0xFFF000F0) == 0xE1200070)
				{
					if (!mMemory->ReadBreakpoint(
							(TMemory::VAddr) realPC + indexLines, instruction))
					{
						instIsBP = true;
					}
				}
				UDisasm::Disasm(
					theInstr,
					sizeof(theInstr),
					realPC + indexLines,
					instruction,
					mSymbolList);

				char status[32];
				status[0] = '\0';
				if (indexLines == 0)
				{
					if (instruction >> 28 != 0xE)
					{
						KUInt32 skip = 0;
						switch (instruction >> 28)
						{
								// 0000 = EQ - Z set (equal)
							case 0x0:
								skip = !(theCPSR & TARMProcessor::kPSR_ZBit);
								break;

								// 0001 = NE - Z clear (not equal)
							case 0x1:
								skip = theCPSR & TARMProcessor::kPSR_ZBit;
								break;
								// 0010 = CS - C set (unsigned higher or same)
							case 0x2:
								skip = !(theCPSR & TARMProcessor::kPSR_CBit);
								break;

								// 0011 = CC - C clear (unsigned lower)
							case 0x3:
								skip = theCPSR & TARMProcessor::kPSR_CBit;
								break;

								// 0100 = MI - N set (negative)
							case 0x4:
								skip = !(theCPSR & TARMProcessor::kPSR_NBit);
								break;

								// 0101 = PL - N clear (positive or zero)
							case 0x5:
								skip = theCPSR & TARMProcessor::kPSR_NBit;
								break;

								// 0110 = VS - V set (overflow)
							case 0x6:
								skip = !(theCPSR & TARMProcessor::kPSR_VBit);
								break;

								// 0111 = VC - V clear (no overflow)
							case 0x7:
								skip = theCPSR & TARMProcessor::kPSR_VBit;
								break;

								// 1000 = HI - C set and Z clear (unsigned higher)
							case 0x8:
								skip = !(theCPSR & TARMProcessor::kPSR_CBit)
									|| (theCPSR & TARMProcessor::kPSR_ZBit);
								break;

								// 1001 = LS - C clear or Z set (unsigned lower or same)
							case 0x9:
								skip = (theCPSR & TARMProcessor::kPSR_CBit)
									&& !(theCPSR & TARMProcessor::kPSR_ZBit);
								break;

								// 1010 = GE - N set and V set, or N clear and V clear (greater or equal)
							case 0xA:
								skip = ((theCPSR & TARMProcessor::kPSR_NBit) != 0)
									!= ((theCPSR & TARMProcessor::kPSR_VBit) != 0);
								break;

								// 1011 = LT - N set and V clear, or N clear and V set (less than)
							case 0xB:
								skip = ((theCPSR & TARMProcessor::kPSR_NBit) != 0)
									== ((theCPSR & TARMProcessor::kPSR_VBit) != 0);
								break;

								// 1100 = GT - Z clear, and either N set and V set, or N clear and V clear (greater than)
							case 0xC:
								skip = (theCPSR & TARMProcessor::kPSR_ZBit) || (((theCPSR & TARMProcessor::kPSR_NBit) != 0) != ((theCPSR & TARMProcessor::kPSR_VBit) != 0));
								break;

								// 1101 = LE - Z set, or N set and V clear, or N clear and V set (less than or equal)
							case 0xD:
								skip = (!(theCPSR & TARMProcessor::kPSR_ZBit))
									&& (((theCPSR & TARMProcessor::kPSR_NBit) != 0)
										== ((theCPSR & TARMProcessor::kPSR_VBit) != 0));
								break;

								// 1111 = NV - never
							case 0xF:
							default:
								skip = 1;
						}

						if (skip)
						{
							(void) ::snprintf(status, 31, " (will skip)");
						} else
						{
							(void) ::snprintf(status, 31, " (will do it)");
						}
					}
				}
				if (instIsBP)
				{
					(void) ::snprintf(theLine, sizeof(theLine), "%.8X * %s",
						(unsigned int) realPC + indexLines,
						theInstr);
				} else
				{
					(void) ::snprintf(theLine, sizeof(theLine), "%.8X   %s",
						(unsigned int) realPC + indexLines,
						theInstr);
				} // if (indexLines == 0)
				mwTerminal->printf("%s%s\n",
					theLine,
					status);
			}
		}
	}

	// Footer.
	mwTerminal->printf("-------------------------------------------------------------------------------\n");
	Fl::unlock();
}

const char*
TFLMonitor::FormatNSCallArgs(char* buffer, size_t size, int nArgs, int nNames)
{
	const char* original_buffer = buffer;
	VAddr sp = mProcessor->GetRegister(7);
	KUInt32 ref;
	int i = 0, n = 0;

	for (i = 0; i < nNames; i++)
	{
		mMemory->Read(sp - 4 - 4 * i, ref);
		n = FormatNSRef(buffer, size, ref, 0, 0);
		buffer += n;
		size -= n;
		if (size <= 1)
			break;
		*buffer++ = ' ';
		buffer[1] = '\0';
		size--;
	}

	if ((size > 0) && nNames && nArgs)
	{
		*buffer++ = ':';
		*buffer++ = ':';
		*buffer++ = ' ';
		size -= 3;
	}
	if (size <= 1)
		return original_buffer;

	for (i = nArgs; i > 0; i--)
	{
		mMemory->Read(sp - 4 * i - 4 * nNames, ref);
		n = FormatNSRef(buffer, size, ref, 0, 0);
		buffer += n;
		size -= n;
		if (size <= 1)
			break;
		*buffer++ = ' ';
		buffer[1] = '\0';
		size--;
	}

	return original_buffer;
}

const char*
TFLMonitor::FormatNSValueStack(char* buffer, size_t size, int nArgs)
{
	return FormatNSCallArgs(buffer, size, nArgs, 0);
}

const char*
TFLMonitor::FormatNSLiteral(char* buffer, size_t size, int index)
{
	const char* original_buffer = buffer;
	VAddr state = mProcessor->GetRegister(4);
	VAddr literals = 0;
	mMemory->Read(state + 0x1c, literals);
	KUInt32 ref;
	mMemory->Read(literals + 4 * index, ref);
	FormatNSRef(buffer, size, ref, 0, 1);
	return original_buffer;
}

// -------------------------------------------------------------------------- //
// DrawScreenRunning( void )
// -------------------------------------------------------------------------- //
void
TFLMonitor::DrawScreenRunning()
{
	Fl::lock();
	mwTerminal->printf("Machine is running. Use stop to halt it.\n");
	mwTerminal->printf("-------------------------------------------------------------------------------\n");
	int indexLog;
	for (indexLog = 0; indexLog < 32; indexLog++)
	{
		mwTerminal->printf("%s\n", GetLog()->GetLine(indexLog));
	}
	mwTerminal->printf("-------------------------------------------------------------------------------\n");
	Fl::unlock();
}

static void
set_attributes(Fl_Button* w)
{
	w->box(FL_FLAT_BOX);
	w->labelcolor(FL_DARK2);
	w->labelsize(12);
	w->visible_focus(0);
}

class TFLMonitorWindow : public Fl_Double_Window
{
public:
	TFLMonitorWindow(int x, int y, int w, int h, const char* l = nullptr) :
			Fl_Double_Window(x, y, w, h, l) { }
	void
	hide() override
	{
		Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
		Fl_Preferences monitor(prefs, "Monitor");
		Fl_Preferences monWindow(monitor, "Window");
		monWindow.set("x", x());
		monWindow.set("y", y());
		monWindow.set("w", w());
		monWindow.set("h", h());
		Fl_Double_Window::hide();
	}
};

void
TFLMonitor::Show()
{
	if (!mwWindow)
	{
		// TODO: this should probably be done in Fluid
		const int ch = 13, wh = 42;
		fl_open_display();
		fl_font(FL_COURIER, ch);
		int cw = int(fl_width("X") + 0.5);
		int xp = 0;
		// ---- use the previous location of the monitor screen again
		Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
		Fl_Preferences monitor(prefs, "Monitor");
		Fl_Preferences monWindow(monitor, "Window");
		int mx, my, mw, mh;
		int initial_mw = 4 + cw * 80 + Fl::scrollbar_size() + Fl::box_dw(FL_DOWN_FRAME);
		int initial_mh = 4 + ch * wh + 2 * ch + Fl::box_dh(FL_DOWN_FRAME);
		monWindow.get("x", mx, 400);
		monWindow.get("y", my, 80);
		monWindow.get("w", mw, initial_mw);
		monWindow.get("h", mh, initial_mh);
		// ---- create the monitor window
		mwWindow = new TFLMonitorWindow(mx, my, mw, mh, "Einstein Monitor");
		// ---- terminal window for all text output
		mwTerminal = new TFLTerminal(0, 0, mwWindow->w(), mwWindow->h() - 2 * ch);
		mwTerminal->box(FL_DOWN_BOX);
		mwTerminal->textfont(FL_COURIER);
		mwTerminal->textsize(ch);
		mwTerminal->callback([](Fl_Widget* w, void* m) {
			TFLTerminal* term = static_cast<TFLTerminal*>(w);
			((TMonitor*) m)->ExecuteCommand(term->value());
			((TFLMonitor*) m)->DrawScreen();
		},
			this);
		mwTerminal->ansi(true);
		//		mwTerminal->stay_at_bottom(true);
		// --- group all this for perfect resizing
		Fl_Group* mwToolbar = new Fl_Group(0, mwWindow->h() - 2 * ch, mwWindow->w(), 2 * ch);
		// --- the stop button stops the emulation
		xp += 1 * cw;
		mwPause = new Fl_Button(xp, mwToolbar->y(), 3 * cw, 2 * ch, "@||");
		mwPause->tooltip("(Shift F5) stop");
		mwPause->shortcut(FL_SHIFT | (FL_F + 5));
		xp += 3 * cw;
		mwPause->callback([](Fl_Widget*, void* m) {
			((TMonitor*) m)->ExecuteCommand("stop");
			((TFLMonitor*) m)->DrawScreen();
		},
			this);
		set_attributes(mwPause);
		// --- continue running the app
		mwRun = new Fl_Button(xp, mwToolbar->y(), 3 * cw, 2 * ch, "@|>");
		mwRun->tooltip("(F5) run");
		mwRun->shortcut(FL_F + 5);
		xp += 3 * cw;
		mwRun->callback([](Fl_Widget*, void* m) {
			((TMonitor*) m)->ExecuteCommand("run");
			Fl::wait(0.1);
			((TFLMonitor*) m)->DrawScreen();
		},
			this);
		set_attributes(mwRun);
		// --- step over
		mwStepOver = new Fl_Button(xp, mwToolbar->y(), 3 * cw, 2 * ch, "@3redo");
		mwStepOver->tooltip("(F10) trace");
		mwStepOver->shortcut(FL_F + 10);
		xp += 3 * cw;
		mwStepOver->callback([](Fl_Widget*, void* m) {
			((TMonitor*) m)->ExecuteCommand("trace");
			((TFLMonitor*) m)->DrawScreen();
		},
			this);
		set_attributes(mwStepOver);
		// --- step into
		mwStep = new Fl_Button(xp, mwToolbar->y(), 3 * cw, 2 * ch, "@2->|");
		mwStep->tooltip("(F11) step");
		mwStep->shortcut(FL_F + 11);
		xp += 3 * cw;
		mwStep->callback([](Fl_Widget*, void* m) {
			((TMonitor*) m)->ExecuteCommand("step");
			((TFLMonitor*) m)->DrawScreen();
		},
			this);
		set_attributes(mwStep);
		// --- leave
		mwLeave = new Fl_Button(xp, mwToolbar->y(), 3 * cw, 2 * ch, "@8->|");
		mwLeave->tooltip("(Shift F11) return to r14");
		mwLeave->shortcut(FL_SHIFT | (FL_F + 11));
		xp += 3 * cw;
		mwLeave->callback([](Fl_Widget*, void* m) {
			((TMonitor*) m)->ExecuteCommand("mr");
			((TFLMonitor*) m)->DrawScreen();
		},
			this);
		set_attributes(mwLeave);
		// --- bytecode stop
		xp += 1 * cw;
		mwBCPause = new Fl_Button(xp, mwToolbar->y(), 3 * cw, 2 * ch, "@||");
		mwBCPause->tooltip("(Shift F1) BC stop");
		mwBCPause->shortcut(FL_SHIFT | (FL_F + 1));
		xp += 3 * cw;
		mwBCPause->callback([](Fl_Widget*, void* m) {
			((TMonitor*) m)->ExecuteCommand(".stop");
			((TFLMonitor*) m)->DrawScreen();
		},
			this);
		set_attributes(mwBCPause);
		mwBCPause->labelcolor(FL_BLUE);
		// --- bytecode continue
		xp += 1 * cw;
		mwBCRun = new Fl_Button(xp, mwToolbar->y(), 3 * cw, 2 * ch, "@|>");
		mwBCRun->tooltip("(F1) BC run");
		mwBCRun->shortcut(FL_F + 1);
		xp += 3 * cw;
		mwBCRun->callback([](Fl_Widget*, void* m) {
			((TMonitor*) m)->ExecuteCommand(".run");
			((TFLMonitor*) m)->DrawScreen();
		},
			this);
		set_attributes(mwBCRun);
		mwBCRun->labelcolor(FL_BLUE);
		// --- resizable part
		mwBox = new Fl_Box(xp, mwToolbar->y(), mwWindow->w() - xp - 4 * cw, 2 * ch);
		// --- help
		mwHelp = new Fl_Button(mwWindow->w() - 4 * cw, mwToolbar->y(), 3 * cw, 2 * ch, "?");
		mwHelp->callback([](Fl_Widget*, void* m) {
			((TMonitor*) m)->ExecuteCommand("help");
			((TFLMonitor*) m)->DrawScreen();
		},
			this);
		set_attributes(mwHelp);
		mwHelp->labelcolor(FL_BLACK);
		// --- set resizing properties
		mwToolbar->resizable(mwBox);
		mwToolbar->end();
		mwWindow->resizable(mwTerminal);
		mwWindow->end();
		Fl::focus(mwTerminal);
	}
	DrawScreen();
	mwWindow->show();
}

void
TFLMonitor::Hide()
{
	if (mwWindow)
	{
		mwWindow->hide();
	}
}

// void
// TFLMonitor::PrintLine(const char* inLine, int type)
//{
//	(void) type;
//	if (mwTerminal)
//	{
//		Fl::lock();
//		mwTerminal->append(inLine);
//		mwTerminal->append("\n");
//		Fl::unlock();
//	}
// }
