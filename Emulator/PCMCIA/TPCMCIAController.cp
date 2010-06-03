// ==============================
// File:			TPCMCIAController.cp
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
#include "TPCMCIAController.h"

// ANSI C & POSIX
#include <assert.h>

// Einstein
#include "Emulator/Log/TLog.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TARMProcessor.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/PCMCIA/TPCMCIACard.h"

#define DEBUGPCMCIA 1

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TPCMCIAController( TLog*, TEmulator*, int )
// -------------------------------------------------------------------------- //
TPCMCIAController::TPCMCIAController(
						TLog* inLog,
						TEmulator* inEmulator,
						int inSocketIx )
	:
		mLog( inLog ),
		mIntManager( inEmulator->GetInterruptManager() ),
		mEmulator( inEmulator ),
		mSocketIx( inSocketIx ),
		mCard( nil ),
		mReg_0000( 0 ),
		mIntCtrlReg( 0 ),
		mReg_0800( 0 ),
		mReg_0C00( 0 ),
		mReg_1000( 0 ),
		mReg_1400( 0 ),
		mReg_1800( 0 ),
		mReg_1C00( 0 ),
		mReg_2000( 0 ),
		mReg_2400( 0 ),
		mReg_2800( 0 ),
		mReg_2C00( 0 ),
		mReg_3000( 0 ),
		mReg_3400( 0 ),
		mReg_3800( 0 /* ID du chip */ ),
		mReg_3C00( 0 ),
		mReg_4000( 0 )
{
}

// -------------------------------------------------------------------------- //
//  * ~TPCMCIAController( void )
// -------------------------------------------------------------------------- //
TPCMCIAController::~TPCMCIAController( void )
{
}

// -------------------------------------------------------------------------- //
//  * Read( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TPCMCIAController::Read( KUInt32 inOffset )
{
#if DEBUGPCMCIA
	if (mLog)
	{
		TARMProcessor *cpu = mEmulator->GetProcessor();
		mLog->FLogLine(
			"TPCMCIAController(pc=0x%08x): Read word access to PCMCIA #%i at +0x%.8X",
			cpu->mCurrentRegisters[15],
			mSocketIx,
			(unsigned int) inOffset );
	}
#endif
	KUInt32 theResult = 0;
	if (!(inOffset & ~kAttrEndMask))
	{
		if (mCard)
		{
			theResult = mCard->ReadAttr( inOffset );
		}
	} else if (!(inOffset & ~kIOEndMask)) {
		if (mCard)
		{
			theResult = mCard->ReadIO( inOffset - kIOSpace );
		}
	} else if (!(inOffset & ~kMemEndMask)) {
		if (mCard)
		{
			theResult = mCard->ReadMem( inOffset - kMemSpace );
		}
	} else if (inOffset == kHdWr_Reg0000) {
		theResult = mReg_0000;
	} else if (inOffset == kHdWr_IntCtrlReg) {
		theResult = mIntCtrlReg;
	} else if (inOffset == kHdWr_Reg0C00) {
		theResult = mReg_0C00;
	} else if (inOffset == kHdWr_Reg1000) {
		theResult = mReg_1000;
	} else if (inOffset == kHdWr_Reg1400) {
		theResult = mReg_1400;
	} else if (inOffset == kHdWr_Reg1800) {
		theResult = mReg_1800;
	} else if (inOffset == kHdWr_Reg1C00) {
		if (mCard)
		{
			mReg_1C00 = mCard->GetVPCPins() &~ k1C00_CardIsPresent;
		} else {
			mReg_1C00 |= k1C00_CardIsPresent;
		}
		theResult = mReg_1C00;
	} else if (inOffset == kHdWr_Reg2000) {
		theResult = mReg_2000;
	} else if (inOffset == kHdWr_Reg2400) {
		theResult = mReg_2400;
	} else if (inOffset == kHdWr_Reg2800) {
		theResult = mReg_2800;
	} else if (inOffset == kHdWr_Reg2C00) {
		theResult = mReg_2C00;
	} else if (inOffset == kHdWr_Reg3000) {
		theResult = mReg_3000;
	} else if (inOffset == kHdWr_Reg3800) {
		theResult = mReg_3800;
	} else if (inOffset == kHdWr_Reg4400) {
		theResult = 0xFC;
#if DEBUGPCMCIA
	} else {
		if (mLog)
		{
			mLog->LogLine( "TPCMCIAController: Unknown PCMCIA word register read access" );
			mEmulator->BreakInMonitor();
		}
#endif
	}
#if DEBUGPCMCIA
	if (mLog) {
		if (inOffset>=kHdWr_Reg0000 && inOffset<=kHdWr_Reg4400)
			LogRegister(inOffset, theResult);
	}
#endif
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * Write( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TPCMCIAController::Write( KUInt32 inOffset, KUInt32 inValue )
{
#if DEBUGPCMCIA
	if (mLog)
	{
		mLog->FLogLine(
			"TPCMCIAController: Write word access to PCMCIA #%i at +0x%.8X (%.8X)",
			mSocketIx,
			(unsigned int) inOffset,
			(unsigned int) inValue );
		if (inOffset>=kHdWr_Reg0000 && inOffset<=kHdWr_Reg4400)
			LogRegister(inOffset, inValue);
	}
#endif
	if (!(inOffset & ~kAttrEndMask))
	{
		if (mCard)
		{
			mCard->WriteAttr( inOffset, inValue );
		}
	} else if (!(inOffset & ~kIOEndMask)) {
		if (mCard)
		{
			mCard->WriteIO( inOffset - kIOSpace, inValue );
		}
	} else if (!(inOffset & ~kMemEndMask)) {
		if (mCard)
		{
			mCard->WriteMem( inOffset - kMemSpace, inValue );
		}
	} else if (inOffset == kHdWr_IntCtrlReg) {
		mIntCtrlReg = inValue;
	} else if (inOffset == kHdWr_Reg0800) {
		mReg_0800 = inValue;
	} else if (inOffset == kHdWr_Reg0C00) {
		mReg_0C00 = inValue;
	} else if (inOffset == kHdWr_Reg1000) {
		mReg_1000 = inValue;
	} else if (inOffset == kHdWr_Reg1400) {
		mReg_1400 = inValue;
	} else if (inOffset == kHdWr_Reg1800) {
		mReg_1800 = inValue;
	} else if (inOffset == kHdWr_Reg1C00) {
		mReg_1C00 = inValue;
		if (mCard)
		{
			mCard->SetVPCPins( mReg_1C00 );
		}
	} else if (inOffset == kHdWr_Reg2000) {
		mReg_2000 = inValue;
	} else if (inOffset == kHdWr_Reg2400) {
		mReg_2400 = inValue;
	} else if (inOffset == kHdWr_Reg2800) {
		mReg_2800 = inValue;
	} else if (inOffset == kHdWr_Reg2C00) {
		mReg_2C00 = inValue;
	} else if (inOffset == kHdWr_Reg3000) {
		mReg_3000 = inValue;
	} else if (inOffset == kHdWr_Reg3400) {
		mReg_3400 = inValue;
	} else if (inOffset == kHdWr_Reg3800) {
		mReg_3800 = inValue;
	} else if (inOffset == kHdWr_Reg3C00) {
		mReg_3C00 = inValue;
	} else if (inOffset == kHdWr_Reg4000) {
		mReg_4000 = inValue;
#if DEBUGPCMCIA
	} else {
		if (mLog)
		{
			mLog->LogLine( "TPCMCIAController: Unknown PCMCIA word register write access" );
			mEmulator->BreakInMonitor();
		}
#endif
	}
}

// -------------------------------------------------------------------------- //
//  * ReadB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TPCMCIAController::ReadB( KUInt32 inOffset )
{
#if DEBUGPCMCIA
	if (mLog)
	{
		mLog->FLogLine(
			"TPCMCIAController: Read byte access to PCMCIA #%i at +0x%.8X",
			mSocketIx,
			(unsigned int) inOffset );
	}
#endif
	KUInt8 theResult = 0;
	if (!(inOffset & ~kAttrEndMask))
	{
		if (mCard)
		{
			theResult = mCard->ReadAttrB( inOffset );
		}
	} else if (!(inOffset & ~kIOEndMask)) {
		if (mCard)
		{
			theResult = mCard->ReadIOB( inOffset - kIOSpace );
		}
	} else if (!(inOffset & ~kMemEndMask)) {
		if (mCard)
		{
			theResult = mCard->ReadMemB( inOffset - kMemSpace );
		}
#if DEBUGPCMCIA
	} else {
		if (mLog)
		{
			mLog->LogLine( "TPCMCIAController: Unknown PCMCIA byte register read access" );
			mEmulator->BreakInMonitor();
		}
#endif
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * WriteB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TPCMCIAController::WriteB( KUInt32 inOffset, KUInt8 inValue )
{
#if DEBUGPCMCIA
	if (mLog)
	{
		mLog->FLogLine(
			"TPCMCIAController: Write byte access to PCMCIA #%i at +0x%.8X (%.8X)",
			mSocketIx,
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
#endif
	if (!(inOffset & ~kAttrEndMask))
	{
		if (mCard)
		{
			mCard->WriteAttrB( inOffset, inValue );
		}
	} else if (!(inOffset & ~kIOEndMask)) {
		if (mCard)
		{
			mCard->WriteIOB( inOffset - kIOSpace, inValue );
		}
	} else if (!(inOffset & ~kMemEndMask)) {
		if (mCard)
		{
			mCard->WriteMemB( inOffset - kMemSpace, inValue );
		}
#if DEBUGPCMCIA
	} else {
		if (mLog)
		{
			mLog->LogLine( "TPCMCIAController: Unknown PCMCIA byte register write access" );
			mEmulator->BreakInMonitor();
		}
#endif
	}
}

// -------------------------------------------------------------------------- //
//  * InsertCard( TPCMCIACard* )
// -------------------------------------------------------------------------- //
void
TPCMCIAController::InsertCard( TPCMCIACard* inCard )
{
	assert (mCard == nil);
	
	mCard = inCard;
	
	// Init the card.
	mCard->Init( this );
	
	RaiseInterrupt( kSocketCardDetectedIntVector );
}

// -------------------------------------------------------------------------- //
//  * RemoveCard( void )
// -------------------------------------------------------------------------- //
void
TPCMCIAController::RemoveCard( void )
{
	assert (mCard != nil);
	
	delete mCard;
	mCard = nil;
	
	// Raise interrupt.
	if (mSocketIx == 0)
	{
		// ??
		mEmulator->GetInterruptManager()->RaiseGPIO( 1 << 0x2 );
	} else if (mSocketIx == 1) {
		mEmulator->GetInterruptManager()->RaiseGPIO( 1 << 0x3 );
	}
}

// -------------------------------------------------------------------------- //
//  * RaiseInterrupt( int )
// -------------------------------------------------------------------------- //
void
TPCMCIAController::RaiseInterrupt( int inVector )
{
	if (mLog) {
		mLog->FLogLine( "TPCMCIAController: mReg_0000 = %.4X", mReg_0000 );
		mLog->FLogLine( "                   mIntCtrlReg = %.4X", mIntCtrlReg );
		mLog->FLogLine( "                   mReg_0C00 = %.4X", mReg_0C00 );
	}

	mReg_0000 |= inVector;
	mReg_0C00 |= inVector;

	if (mSocketIx == 0)
	{
		mEmulator->GetInterruptManager()->RaiseInterrupt(
			TInterruptManager::kPCMCIA0IntMask );
	} else if (mSocketIx == 1) {
		mEmulator->GetInterruptManager()->RaiseInterrupt(
			TInterruptManager::kPCMCIA1IntMask );
	}
}

// -------------------------------------------------------------------------- //
//  * RaiseInterrupt( int )
// -------------------------------------------------------------------------- //
void 
TPCMCIAController::LogRegister(KUInt32 reg, KUInt32 v) 
{
	return;
	
	if (!mLog)
		return;
	
	char logInterrupts = 0;
	switch (reg) {
		case kHdWr_Reg0000:
			mLog->FLogLine("---- PCMCIA Register 0000 (0x%04x) - active interrupts", v);
			logInterrupts = 1;
			break;
		case kHdWr_IntCtrlReg:
			mLog->FLogLine("---- PCMCIA Register 0400 (0x%04x) - enabled interrupts", v);
			logInterrupts = 1;
			break;
		case kHdWr_Reg0800:
			mLog->FLogLine("---- PCMCIA Register 0800 (0x%04x) - interrupts ?", v);
			logInterrupts = 1;
			break;
		case kHdWr_Reg0C00:
			mLog->FLogLine("---- PCMCIA Register 0C00 (0x%04x) - promote interrupts to FIQ", v);
			logInterrupts = 1;
			break;
		case kHdWr_Reg1000:
			mLog->FLogLine("---- PCMCIA Register 1000 (0x%04x) - interrupt on rising edge", v);
			logInterrupts = 1;
			break;
		case kHdWr_Reg1400:
			mLog->FLogLine("---- PCMCIA Register 1400 (0x%04x) - interrupt on falling edge", v);
			logInterrupts = 1;
			break;
		case kHdWr_Reg1800:
			mLog->FLogLine("---- PCMCIA Register 1800 (0x%04x) - interrupt causes wakeup", v);
			logInterrupts = 1;
			break;
		case kHdWr_Reg1C00:
			mLog->FLogLine("---- PCMCIA Register 1c00 (0x%04x) - Pin access", v);
			mLog->FLogLine("  63/BVD1/-STSCHG/-PDTAG = %d", (v&k1C00_Pin63_46)!=0);
			mLog->FLogLine("     62/BVD2/-SPKR/-DASP = %d", (v&k1C00_Pin62_45)!=0);
			mLog->FLogLine("                 36/-CD1 = %d", (v&k1C00_Pin36_26)!=0);
			mLog->FLogLine("                 67/-CD2 = %d", (v&k1C00_Pin67_25)!=0);
			mLog->FLogLine("                  43/VS1 = %d", (v&k1C00_Pin43_33)!=0);
			mLog->FLogLine("                  57/VS2 = %d", (v&k1C00_Pin57_40)!=0);
			mLog->FLogLine("            61/REG/VDREQ = %d", (v&k1C00_Pin61_44)!=0);
			mLog->FLogLine("          07/-CE1/VPCTL1 = %d", (v&k1C00_Pin07_07)!=0);
			mLog->FLogLine("          42/-CE2/VPCTL2 = %d", (v&k1C00_Pin42_32)!=0);
			mLog->FLogLine("           33/WP/-IOIS16 = %d", (v&k1C00_Pin33_24)!=0);
			mLog->FLogLine("        16/RDY/-BSY/IREQ = %d", (v&k1C00_Pin16_37)!=0);
			mLog->FLogLine("           FIFOThreshold = %d", (v&k1C00_VWrFIFOThreshld)!=0);
			mLog->FLogLine("               FIFOEmpty = %d", (v&k1C00_VWrFIFOEmpty)!=0);
			mLog->FLogLine("              Write Fail = %d", (v&k1C00_VWriteFailure)!=0);
			mLog->FLogLine("               Read Fail = %d", (v&k1C00_VReadFailure)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x8000)!=0);
			break;
		case kHdWr_Reg2000:
			mLog->FLogLine("---- PCMCIA Register 2000 (0x%04x) - FIFO", v);
			mLog->FLogLine("                      ?? = %d", (v&0x0001)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0002)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0004)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0008)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0008)!=0);
			mLog->FLogLine("  k2000_RdWrQueueCtrl = 0x%02x", (v&0x001f));
			mLog->FLogLine("     k2000_EndianConvert = %d", (v&0x0020)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0040)!=0);
			mLog->FLogLine("    k2000_HandshakeReady = %d", (v&0x0080)!=0);
			mLog->FLogLine("        k2000_Assembly32 = %d", (v&0x0100)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0200)!=0);
			mLog->FLogLine("          k2000_SwapSize = %d", (v&0x0400)!=0);
			mLog->FLogLine("       k2000_DynamicSwap = %d", (v&0x0800)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x1000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x2000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x4000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x8000)!=0);
			break;
		case kHdWr_Reg2400:
			mLog->FLogLine("---- PCMCIA Register 2400 (0x%04x) - pin access and direction", v);
			mLog->FLogLine("                      ?? = %d", (v&0x0001)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0002)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0004)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0008)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0010)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0020)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0040)!=0);
			mLog->FLogLine("      k2400_NoByteAccess = %d", (v&0x0080)!=0);
			mLog->FLogLine("          k2400_SelectIO = %d", (v&0x0100)!=0);
			mLog->FLogLine("    k2400_SWWriteProtect = %d", (v&0x0200)!=0);
			mLog->FLogLine("         k2400_EnableBus = %d", (v&0x0400)!=0);
			mLog->FLogLine("    k2400_EnableResetOut = %d", (v&0x0800)!=0);
			mLog->FLogLine("       k2400_ResetPCMCIA = %d", (v&0x1000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x2000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x4000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x8000)!=0);
			break;
		case kHdWr_Reg2800:
			mLog->FLogLine("---- PCMCIA Register 2800 (0x%04x) - pullups and power", v);
			mLog->FLogLine("                      ?? = %d", (v&0x0001)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0002)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0004)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0008)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0010)!=0);
			mLog->FLogLine("             k2800_VccOn = %d", (v&0x0020)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0040)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0080)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0100)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0200)!=0);
			mLog->FLogLine("   SetPullupControl = 0x%04x", (v&0x7C00));
			mLog->FLogLine("                      ?? = %d", (v&0x8000)!=0);
			break;
		case kHdWr_Reg2C00:
			mLog->FLogLine("---- PCMCIA Register 2C00 (0x%04x) - power, pin access", v);
			mLog->FLogLine("                      ?? = %d", (v&0x0001)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0002)!=0);
			mLog->FLogLine("                 k2C00_VppOn = %d", (v&0x0004)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0008)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0010)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0020)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0040)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0080)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0100)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0200)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0400)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0800)!=0);
			mLog->FLogLine("            k2C00_VS1Out = %d", (v&0x1000)!=0);
			mLog->FLogLine("            k2C00_VS2Out = %d", (v&0x2000)!=0);
			mLog->FLogLine("            k2C00_VS1Dir = %d", (v&0x4000)!=0);
			mLog->FLogLine("            k2C00_VS2Dir = %d", (v&0x8000)!=0);
			break;
		case kHdWr_Reg3000:
			mLog->FLogLine("---- PCMCIA Register 3000 (0x%04x) - memory speed", v);
			mLog->FLogLine("       Memory Speed = 0x%04x", (v&0x003f));
			mLog->FLogLine("                      ?? = %d", (v&0x0040)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0080)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0100)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0200)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0400)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0800)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x1000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x2000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x4000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x8000)!=0);
			break;
		case kHdWr_Reg3400:
			mLog->FLogLine("---- PCMCIA Register 3400 (0x%04x) - ??", v);
			mLog->FLogLine("                      ?? = %d", (v&0x0001)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0002)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0004)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0008)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0010)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0020)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0040)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0080)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0100)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0200)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0400)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0800)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x1000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x2000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x4000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x8000)!=0);
			break;
		case kHdWr_Reg3800:
			mLog->FLogLine("---- PCMCIA Register 3800 (0x%04x) - I/O speed", v);
			mLog->FLogLine("          I/O Speed = 0x%04x", (v&0x003f));
			mLog->FLogLine("                      ?? = %d", (v&0x0040)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0080)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0100)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0200)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0400)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0800)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x1000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x2000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x4000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x8000)!=0);
			break;
		case kHdWr_Reg3C00:
			mLog->FLogLine("---- PCMCIA Register 3C00 (0x%04x) - ??", v);
			mLog->FLogLine("                      ?? = %d", (v&0x0001)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0002)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0004)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0008)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0010)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0020)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0040)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0080)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0100)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0200)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0400)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0800)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x1000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x2000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x4000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x8000)!=0);
			break;
		case kHdWr_Reg4000:
			mLog->FLogLine("---- PCMCIA Register 4000 (0x%04x) - ??", v);
			mLog->FLogLine("                      ?? = %d", (v&0x0001)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0002)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0004)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0008)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0010)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0020)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0040)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0080)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0100)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0200)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0400)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0800)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x1000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x2000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x4000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x8000)!=0);
			break;
		case kHdWr_Reg4400:
			mLog->FLogLine("---- PCMCIA Register 4400 (0x%04x) - ??", v);
			mLog->FLogLine("                      ?? = %d", (v&0x0001)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0002)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0004)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0008)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0010)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0020)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0040)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0080)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0100)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0200)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0400)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x0800)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x1000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x2000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x4000)!=0);
			mLog->FLogLine("                      ?? = %d", (v&0x8000)!=0);
			break;
	}
	if (logInterrupts) {
		if (v & kSocketBusyIntVector)
			mLog->FLogLine("    kSocketBusyIntVector = 1");
		if (v & kSocketStatusChangedIntVector)
			mLog->FLogLine("    kSocketStatusChangedIntVector = 1");
		if (v & kSocketCardDetectedIntVector)
			mLog->FLogLine("    kSocketCardDetectedIntVector = 1");
		if (v & kSocketCardLockIntVector)
			mLog->FLogLine("    kSocketCardLockIntVector = 1");
		if (v & kSocketCardIREQIntVector)
			mLog->FLogLine("    kSocketCardIREQIntVector = 1");
		if (v & kSocketSpeakerIntVector)
			mLog->FLogLine("    kSocketSpeakerIntVector = 1");
		if (v & kSocketDREQIntVector)
			mLog->FLogLine("    kSocketDREQIntVector = 1");
		if (v & kSocketWPIntVector)
			mLog->FLogLine("    kSocketWPIntVector = 1");
		if (v & kSocketReadFailureIntVector)
			mLog->FLogLine("    kSocketReadFailureIntVector = 1");
		if (v & kSocketWriteFailureIntVector)
			mLog->FLogLine("    kSocketWriteFailureIntVector = 1");
		if (v & kSocketWrFIFOEmptyIntVector)
			mLog->FLogLine("    kSocketWrFIFOEmptyIntVector = 1");
		if (v & kSocketWrFIFOThreshldIntVector)
			mLog->FLogLine("    kSocketWrFIFOThreshldIntVector = 1");
	}
	mLog->FLogLine("");
}


// ============================================================= //
// In computing, the mean time to failure keeps getting shorter. //
// ============================================================= //
