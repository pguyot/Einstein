#include "Monitor/TMacMonitor.h"
#include "Monitor/TSymbolList.h"
#include "Monitor/UDisasm.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"
#include "TCocoaMonitorController.h"

TMacMonitor::TMacMonitor(
			TBufferLog* inLog,
			TEmulator* inEmulator,
			TSymbolList* inSymbolList,
			const char* inROMPath) :
	TMonitor(inLog, inEmulator, inSymbolList, inROMPath)
{
}

TMacMonitor::~TMacMonitor( void )
{
}


NSString* TMacMonitor::GetAlarm()
{
	return [NSString stringWithFormat:@"%.8X", mInterruptManager->GetAlarm()];
}


NSString* TMacMonitor::GetFrozenTimer()
{
	return [NSString stringWithFormat:@"%.8X", mInterruptManager->GetFrozenTimer()];
}


NSString* TMacMonitor::GetIntCtrlReg()
{
	return [NSString stringWithFormat:@"%.8X", mInterruptManager->GetIntCtrlReg()];
}


NSString* TMacMonitor::GetIntRaised()
{
	return [NSString stringWithFormat:@"%.8X", mInterruptManager->GetIntRaised()];
}


NSString* TMacMonitor::GetRealTimeClock()
{
	return [NSString stringWithFormat:@"%.8X", mInterruptManager->GetRealTimeClock()];
}


NSString* TMacMonitor::GetRegister(int rn)
{
	return [NSString stringWithFormat:@"%.8X", mProcessor->GetRegister(rn)];
}


NSString* TMacMonitor::GetTimerMatchRegister(int rn)
{
	return [NSString stringWithFormat:@"%.8X", mInterruptManager->GetTimerMatchRegister(rn)];
}


NSString* TMacMonitor::GetFIQMask()
{
	return [NSString stringWithFormat:@"%.8X", mInterruptManager->GetFIQMask()];
}


NSString* TMacMonitor::GetIntEDReg1()
{
	return [NSString stringWithFormat:@"%.8X", mInterruptManager->GetIntEDReg1()];
}


NSString* TMacMonitor::GetIntEDReg2()
{
	return [NSString stringWithFormat:@"%.8X", mInterruptManager->GetIntEDReg2()];
}


NSString* TMacMonitor::GetIntEDReg3()
{
	return [NSString stringWithFormat:@"%.8X", mInterruptManager->GetIntEDReg3()];
}


NSString* TMacMonitor::GetCPSR()
{
	KUInt32 theCPSR = mProcessor->GetCPSR();
	KUInt32 theMode = theCPSR & TARMProcessor::kPSR_ModeMask;
	return [NSString stringWithFormat:@"%c%c%c%c %c%c%c %s",
				theCPSR & TARMProcessor::kPSR_NBit ? 'N' : 'n',
				theCPSR & TARMProcessor::kPSR_ZBit ? 'Z' : 'z',
				theCPSR & TARMProcessor::kPSR_CBit ? 'C' : 'c',
				theCPSR & TARMProcessor::kPSR_VBit ? 'V' : 'v',
				theCPSR & TARMProcessor::kPSR_IBit ? 'I' : 'i',
				theCPSR & TARMProcessor::kPSR_FBit ? 'F' : 'f',
				theCPSR & TARMProcessor::kPSR_TBit ? 'T' : 't',
				kModesNames[theMode]];
}


NSString* TMacMonitor::GetSPSR()
{
	KUInt32 theCPSR = mProcessor->GetCPSR();
	KUInt32 theMode = theCPSR & TARMProcessor::kPSR_ModeMask;
	KUInt32 theSPSR = 0;
	if ((theMode != TARMProcessor::kSystemMode)
		&& (theMode != TARMProcessor::kUserMode))
	{
		theSPSR = mProcessor->GetSPSR();
	}
	if (theSPSR == 0)
	{
		return [NSString stringWithFormat:@"---- --- ---"];
	} else {
		return [NSString stringWithFormat:@"%c%c%c%c %c%c%c %s",
					theSPSR & TARMProcessor::kPSR_NBit ? 'N' : 'n',
					theSPSR & TARMProcessor::kPSR_ZBit ? 'Z' : 'z',
					theSPSR & TARMProcessor::kPSR_CBit ? 'C' : 'c',
					theSPSR & TARMProcessor::kPSR_VBit ? 'V' : 'v',
					theSPSR & TARMProcessor::kPSR_IBit ? 'I' : 'i',
					theSPSR & TARMProcessor::kPSR_FBit ? 'F' : 'f',
					theSPSR & TARMProcessor::kPSR_TBit ? 'T' : 't',
					kModesNames[theSPSR & TARMProcessor::kPSR_ModeMask]];
	}
}


NSString* TMacMonitor::GetSymbol()
{
	KUInt32 realPC = mProcessor->GetRegister(15) - 4;
	char theSymbol[512];
	char theComment[512];
	int theOffset;

	mSymbolList->GetSymbol(
					realPC,
					theSymbol,
					theComment,
					&theOffset );

	return [NSString stringWithFormat:@"%s+%X", theSymbol, theOffset];
}


NSString* TMacMonitor::GetDisasmLine(int offset)
{
	char theInstr[512];
	theInstr[0] = 0;
	char theLine[512];
	KUInt32 instruction;

	KUInt32 realPC = mProcessor->GetRegister(15) - 4;
	int indexLines = offset * 4;
	
	KUInt32 theCPSR = mProcessor->GetCPSR();

	NSMutableString *outString = [NSMutableString string];
	
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
		[outString appendFormat:@"%s%s",
					theLine,
					status];
	}
	
	return [[NSString alloc] initWithString:outString];
}


void TMacMonitor::PrintLine(const char* inLine, int type)
{
	if ( controller )
	{
		[controller addHistoryLine:[NSString stringWithCString:inLine encoding:NSUTF8StringEncoding] type:type];
	}
}


Boolean TMacMonitor::ProcessBreakpoint( KUInt16 inBPID, KUInt32 inBPAddr )
{
	Boolean stop = TMonitor::ProcessBreakpoint(inBPID, inBPAddr);
	
	if ( controller )
	{	
		[controller performSelectorInBackground:@selector(update) withObject:nil];
	}
	
	return stop;
}


void TMacMonitor::SetController(TCocoaMonitorController *inController)
{
	controller = inController;
}

