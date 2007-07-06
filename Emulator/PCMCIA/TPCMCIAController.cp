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
		mLog->FLogLine(
			"Read word access to PCMCIA #%i at +0x%.8X",
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
			mLog->LogLine( "Unknown PCMCIA word register read access" );
			mEmulator->BreakInMonitor();
		}
#endif
	}
	
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
			"Write word access to PCMCIA #%i at +0x%.8X (%.8X)",
			mSocketIx,
			(unsigned int) inOffset,
			(unsigned int) inValue );
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
			mLog->LogLine( "Unknown PCMCIA word register write access" );
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
			"Read byte access to PCMCIA #%i at +0x%.8X",
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
			mLog->LogLine( "Unknown PCMCIA byte register read access" );
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
			"Write byte access to PCMCIA #%i at +0x%.8X (%.8X)",
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
			mLog->LogLine( "Unknown PCMCIA byte register write access" );
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
	mLog->FLogLine( "mReg_0000 = %.4X", mReg_0000 );
	mLog->FLogLine( "mIntCtrlReg = %.4X", mIntCtrlReg );
	mLog->FLogLine( "mReg_0C00 = %.4X", mReg_0C00 );

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

// ============================================================= //
// In computing, the mean time to failure keeps getting shorter. //
// ============================================================= //
