#include "Monitor/TMacMonitor.h"
#include "Monitor/TSymbolList.h"
#include "Monitor/UDisasm.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"

TMacMonitor::TMacMonitor(
			TBufferLog* inLog,
			TEmulator* inEmulator,
			TSymbolList* inSymbolList ) : TMonitor(inLog, inEmulator, inSymbolList)
{
}

TMacMonitor::~TMacMonitor( void )
{
}


NSString* TMacMonitor::GetScreen(void)
{
	Boolean theResult = false;
	if (mHalted)
	{
		if (!mLastScreenHalted)
		{
			// Clear the terminal.
			theResult = true;
		}
		mLastScreenHalted = true;
		return GetScreenHalted();
	} else {
		if (mLastScreenHalted)
		{
			// Clear the terminal.
			theResult = true;
		}
		mLastScreenHalted = false;
		return GetScreenRunning();
	}
	
	return @"...";
}


NSString*
TMacMonitor::GetScreenHalted( void )
{
	NSMutableString* outString = [NSMutableString string];
	KUInt32 realPC = mProcessor->GetRegister(15) - 4;

	int indexRegisters;
	for (indexRegisters = 0; indexRegisters < 16; indexRegisters++)
	{
		[outString appendFormat:@"%s= %.8X | %s\n",
				kRegisterNames[indexRegisters],
				(unsigned int) mProcessor->GetRegister(indexRegisters),
				mLog->GetLine(indexRegisters)];
	}
	KUInt32 theCPSR = mProcessor->GetCPSR();
	KUInt32 theMode = theCPSR & TARMProcessor::kPSR_ModeMask;
	[outString appendFormat:@"%c%c%c%c %c%c%c %s  | %s\n",
				theCPSR & TARMProcessor::kPSR_NBit ? 'N' : 'n',
				theCPSR & TARMProcessor::kPSR_ZBit ? 'Z' : 'z',
				theCPSR & TARMProcessor::kPSR_CBit ? 'C' : 'c',
				theCPSR & TARMProcessor::kPSR_VBit ? 'V' : 'v',
				theCPSR & TARMProcessor::kPSR_IBit ? 'I' : 'i',
				theCPSR & TARMProcessor::kPSR_FBit ? 'F' : 'f',
				theCPSR & TARMProcessor::kPSR_TBit ? 'T' : 't',
				kModesNames[theMode],
				mLog->GetLine(16)];
	KUInt32 theSPSR = 0;
	if ((theMode != TARMProcessor::kSystemMode)
		&& (theMode != TARMProcessor::kUserMode))
	{
		theSPSR = mProcessor->GetSPSR();
	}
	if (theSPSR == 0)
	{
		[outString appendFormat:@"---- --- ---  | %s\n",
					mLog->GetLine(17)];
	} else {
		[outString appendFormat:@"%c%c%c%c %c%c%c %s  | %s\n",
					theSPSR & TARMProcessor::kPSR_NBit ? 'N' : 'n',
					theSPSR & TARMProcessor::kPSR_ZBit ? 'Z' : 'z',
					theSPSR & TARMProcessor::kPSR_CBit ? 'C' : 'c',
					theSPSR & TARMProcessor::kPSR_VBit ? 'V' : 'v',
					theSPSR & TARMProcessor::kPSR_IBit ? 'I' : 'i',
					theSPSR & TARMProcessor::kPSR_FBit ? 'F' : 'f',
					theSPSR & TARMProcessor::kPSR_TBit ? 'T' : 't',
					kModesNames[theSPSR & TARMProcessor::kPSR_ModeMask],
					mLog->GetLine(17)];
	}

	[outString appendFormat:@"==============| %s\n",
				mLog->GetLine(18)];
	
	[outString appendFormat:@"Tmr= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetFrozenTimer(),
				mLog->GetLine(19)];
	
	[outString appendFormat:@"TM0= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetTimerMatchRegister(0),
				mLog->GetLine(20)];
	
	[outString appendFormat:@"TM1= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetTimerMatchRegister(1),
				mLog->GetLine(21)];
	
	[outString appendFormat:@"TM2= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetTimerMatchRegister(2),
				mLog->GetLine(22)];
	
	[outString appendFormat:@"TM3= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetTimerMatchRegister(3),
				mLog->GetLine(23)];
	
	[outString appendFormat:@"RTC= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetRealTimeClock(),
				mLog->GetLine(24)];
	
	[outString appendFormat:@"Alm= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetAlarm(),
				mLog->GetLine(25)];
	
	[outString appendFormat:@"IR = %.8X | %s\n",
				(unsigned int) mInterruptManager->GetIntRaised(),
				mLog->GetLine(26)];
	
	[outString appendFormat:@"ICR= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetIntCtrlReg(),
				mLog->GetLine(27)];
	
	[outString appendFormat:@"FM = %.8X | %s\n",
				(unsigned int) mInterruptManager->GetFIQMask(),
				mLog->GetLine(28)];
	
	[outString appendFormat:@"IC1= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetIntEDReg1(),
				mLog->GetLine(29)];
	
	[outString appendFormat:@"IC2= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetIntEDReg2(),
				mLog->GetLine(30)];
	
	[outString appendFormat:@"IC3= %.8X | %s\n",
				(unsigned int) mInterruptManager->GetIntEDReg3(),
				mLog->GetLine(31)];
	
	[outString appendFormat:@"-------------------------------------------------------------------------------\n"];

	char theInstr[512];
	theInstr[0] = 0;
	
	char theSymbol[512];
	char theComment[512];
	int theOffset;
	char theLine[512];

	// Get the symbol.
	mSymbolList->GetSymbol(
					realPC,
					theSymbol,
					theComment,
					&theOffset );

	[outString appendFormat:@"%s+%X\n",
		theSymbol, theOffset];
	
	KUInt32 instruction;
	
	// Write 5 lines.
	int indexLines;
	for (indexLines = 0; indexLines < 20; indexLines += 4)
	{
		if (mMemory->Read(
				(TMemory::VAddr) realPC + indexLines, instruction ))
		{
			[outString appendFormat:
				@"%.8X Memory Error [%.8X]\n",
				(unsigned int) realPC + indexLines,
				(unsigned int) mMemory->GetFaultStatusRegister()];
		} else {
			Boolean instIsBP = false;
			if ((instruction & 0xFFF000F0) == 0xE1200070)
			{
				if (!mMemory->ReadBreakpoint(
						(TMemory::VAddr) realPC + indexLines, instruction ))
				{
					instIsBP = true;
				}
			}
			UDisasm::Disasm(
						theInstr,
						sizeof(theInstr),
						realPC + indexLines,
						instruction,
						mSymbolList );

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
							skip = (theCPSR & TARMProcessor::kPSR_ZBit) ||
								(((theCPSR & TARMProcessor::kPSR_NBit) != 0)
									!= ((theCPSR & TARMProcessor::kPSR_VBit) != 0));
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
						(void) ::sprintf( status, " (will skip)" );
					} else {
						(void) ::sprintf( status, " (will do it)" );
					}
				}
			}
			if (instIsBP)
			{
				(void) ::sprintf( theLine, "%.8X * %s",
							(unsigned int) realPC + indexLines,
							theInstr );
			} else {
				(void) ::sprintf( theLine, "%.8X   %s",
							(unsigned int) realPC + indexLines,
							theInstr );
			} // if (indexLines == 0)
			[outString appendFormat:@"%s%s\n",
						theLine,
						status];
		}
	}

	// Footer.
	[outString appendFormat:@"-------------------------------------------------------------------------------"];
	return outString;
}


NSString*
TMacMonitor::GetScreenRunning( void )
{
	NSMutableString* outString = [NSMutableString string];
	[outString appendFormat:@"Machine is running. Use stop to halt it.\n"];
	[outString appendFormat:@"-------------------------------------------------------------------------------\n"];
	int indexLog;
	for (indexLog = 0; indexLog < 32; indexLog++)
	{
		[outString appendFormat:@"%s\n", mLog->GetLine(indexLog)];
	}
	[outString appendFormat:@"-------------------------------------------------------------------------------"];
	return outString;
}

