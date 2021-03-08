// ==============================
// File:			TARMProcessor.cp
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

#include "Emulator/TARMProcessor.h"

// POSIX
#include <sys/types.h>
#include <signal.h>

#if !TARGET_OS_WIN32
	#include <unistd.h>
#endif

// K
#include <K/Streams/TStream.h>

// Einstein
#include "Emulator/Log/TLog.h"
#include "Emulator/TEmulator.h"
#include "Emulator/JIT/JIT.h"
#include "Emulator/ROM/TROMImage.h"
#include "Monitor/TSymbolList.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TARMProcessor( TLog*, TMemory* )
// -------------------------------------------------------------------------- //
TARMProcessor::TARMProcessor(
						TLog* inLog,
						TMemory* inMemory )
	:
		mPendingInterrupts( 0 ),
		mLog( inLog ),
		mMemory( inMemory ),
		mNativePrimitives( inLog, inMemory ),
		mEmulator( nil )
{
	mMemory->SetProcessor( this );
	mNativePrimitives.SetProcessor( this );

	// Clear everything.
	KUInt32 indexRegisters;
	for (indexRegisters = 0; indexRegisters < 16; indexRegisters++)
	{
		mCurrentRegisters[indexRegisters] = 0;
	}
	
	mCPSR_N = 0;
	mCPSR_Z = 0;
	mCPSR_C = 0;
	mCPSR_V = 0;
	mCPSR_I = 0;
	mCPSR_F = 0;
	mCPSR_T = 0;
	mR8_Bkup = 0;
	mR9_Bkup = 0;
	mR10_Bkup = 0;
	mR11_Bkup = 0;
	mR12_Bkup = 0;
	mR13_Bkup = 0;
	mR14_Bkup = 0;
	mR13svc_Bkup = 0;
	mR14svc_Bkup = 0;
	mR13abt_Bkup = 0;
	mR14abt_Bkup = 0;
	mR13und_Bkup = 0;
	mR14und_Bkup = 0;
	mR13irq_Bkup = 0;
	mR14irq_Bkup = 0;
	mR8fiq_Bkup = 0;
	mR9fiq_Bkup = 0;
	mR10fiq_Bkup = 0;
	mR11fiq_Bkup = 0;
	mR12fiq_Bkup = 0;
	mR13fiq_Bkup = 0;
	mR14fiq_Bkup = 0;
	mSPSRsvc = 0;
	mSPSRabt = 0;
	mSPSRund = 0;
	mSPSRirq = 0;
	mSPSRfiq = 0;
	mMode = kSupervisorMode;
	
	mCurrentRegisters[kR15] = 0x00000004; // Prefetch.
}

// -------------------------------------------------------------------------- //
//  * SystemCoprocRegisterTransfer( KUInt32 )
// -------------------------------------------------------------------------- //
void
TARMProcessor::SystemCoprocRegisterTransfer( KUInt32 inInstruction )
{
	KUInt32 CRm = inInstruction & 0x0000000F;
	KUInt32 CP = (inInstruction & 0x000000E0) >> 5;
	KUInt32 Rd = (inInstruction & 0x0000F000) >> 12;
	KUInt32 CRn = (inInstruction & 0x000F0000) >> 16;
	KUInt32 CPOpc = (inInstruction & 0x00E00000) >> 21;
	KUInt32 theValue;

	if (inInstruction & 0x00100000)
	{
		theValue = 0;
		
		// Load from co-processor.
		switch (CRn)
		{
			case 0:
				if (CP == 0)
				{
					// It seems that NewtonOS expects either
					// 0x4401A10x (Intel) or 0x41047102 (0x4104710x) (DEC).
					// Main ID register.
					// 44 -> Intel (DEC=41)
					// 0     (UND)
					// 1  -> Architecture 4
					// 00 -> implementation defined.
					// 00 -> Revision number.
					// (I've found the doc of the Intel chip)
					theValue = 0x4401A100;
//					theValue = 0x41047102;
//					mEmulator->BreakInMonitor();
				} else {
					// Cache Type Register.
					// We don't have any cache.
					theValue = 0;
				}
				break;
			
			case 1:
				// Control register.
				theValue = 0x000000B0;
				// Bit 0: MMU state
				if (mMemory->IsMMUEnabled())
				{
					theValue |= 0x00000001;
				}
				// Bit 1: Alignment Fault Checking : 0 (disabled)
				// Bit 2: Cache bit : 0 (unimplemented)
				// Bit 3: Write buffer : 0 (unimplemented)
				// Bit 4: 26 bits backward compatibility : 1 (incompatible)
				// Bit 5: 26 bits backward compatibility : 1 (incompatible)
				// Bit 6: Abort model : 0 (early)
				// Bit 7: Big Endian : 1
				// Bit 8: System Protection Bit
				if (mMemory->GetSystemProtection())
				{
					theValue |= 0x00000100;
				}
				// Bit 9: ROM Protection Bit
				if (mMemory->GetROMProtection())
				{
					theValue |= 0x00000200;
				}
				// Bit 10: F Bit (Implementation Defined)
				// Bit 11: Branch Predication : 0 (disabled)
				// Bit 12: Instruction Cache Enable : 0 (disabled)
				// Bit 13: High Vector/Low Vector : 0 (low)
				// Bit 14: Cache Alternative Replacement Strategy : 0 (normal)
				// Bit 15: L4 bit : 0 (unimplemented)
				break;
							
			case 2:
				// MMU translation base.
				theValue = mMemory->GetTranslationTableBase();
				break;

			case 3:
				// Domain Access Control
				theValue = mMemory->GetDomainAccessControl();
				break;

			case 5:
				// Fault status register.
				theValue = mMemory->GetFaultStatusRegister();
				break;

			case 6:
				// Fault address register.
				theValue = mMemory->GetFaultAddressRegister();
				break;

			case 10:
				// TLB Lockdown
				// We don't have any TLB lockdown.
				theValue = 0;
				break;
				
			default:
				if (mLog)
				{
					mLog->FLogLine(
						"Unimplemented CP#15 access mrc 15, %i, R%i, cr%i, cr%i, {%i}",
						(int) CPOpc,
						(int) Rd,
						(int) CRn,
						(int) CRm,
						(int) CP);
				}
		}
		
		if (Rd == kR15)
		{
			mCPSR_N = (theValue & 0x80000000) != 0;
			mCPSR_Z = (theValue & 0x40000000) != 0;
			mCPSR_C = (theValue & 0x20000000) != 0;
			mCPSR_V = (theValue & 0x10000000) != 0;
		} else {
			mCurrentRegisters[Rd] = theValue;
		}
	} else {
		// Store to co-processor.
		theValue = mCurrentRegisters[Rd];
		if (Rd == kR15)
		{
			theValue += 4;
		}
		
		switch (CRn)
		{
			// Writing to CR0 is unpredictable.
			// case 0:
			
			case 1:
				// Control register.
				// NewtonOS first writes 10B0
				// i.e.: disable 26 bits backward compatibility
				// big endian and enable instruction cache.
				
				// Bit 0: enable MMU
				mMemory->SetMMUEnabled((theValue & 0x00000001) != 0);
				// Bit 1: Alignment Fault Checking (ignored)
				// Bit 2: Cache bit (ignored)
				// Bit 3: Write buffer (ignored)
				// Bit 4: 26 bits backward compatibility (ignored) (P)
				// Bit 5: 26 bits backward compatibility (ignored) (D)
				// Bit 6: Abort model (ignored)
				// Bit 7: Big Endian (ignored)
				// Bit 8: System Protection Bit
				mMemory->SetSystemProtection((theValue & 0x00000100) != 0);
				// Bit 9: ROM Protection Bit
				mMemory->SetROMProtection((theValue & 0x00000200) != 0);
				// Bit 10: F Bit (Implementation Defined)
				// Bit 11: Branch Predication (ignored)
				// Bit 12: Instruction Cache Enable (ignored)
				// Bit 13: High Vector/Low Vector (ignored)
				// Bit 14: Cache Alternative Replacement Strategy (ignored)
				// Bit 15: L4 bit (ignored)
				break;
			
			case 2:
				// MMU translation base.
				mMemory->SetTranslationTableBase( theValue & 0xFFFFC000 );
				break;
			
			case 3:
				// Domain Access Control
				mMemory->SetDomainAccessControl( theValue );
				break;

			case 5:
				// Fault status register.
				mMemory->SetFaultStatusRegister( theValue );
				break;

			case 6:
				// Fault address register.
				mMemory->SetFaultAddressRegister( theValue );
				break;

			case 7:
				// Cache Functions.
				if (((CRm == kR0) && (CP == 4))
					|| ((CRm == kR8) && (CP == 2)))
				{
					if (mLog)
						mLog->LogLine( "Wait for Interrupt" );
					mEmulator->BreakInMonitor();
				} // else: cache functions.
				break;
				
			case 8:
				// TLB (Translation Lookup Buffers) functions
				// We don't have any TLB.
				mMemory->InvalidateTLB();
//				if (mLog)
//					mLog->LogLine( "Write access to TLB functions" );
				break;
			
			case 9:
				// Cache Lockdown register.
//				if (mLog)
//					mLog->LogLine( "Write access to Cache Lockdown register" );
				break;
				
			case 10:
				// TLB Lockdown
				// We don't have any TLB.
//				if (mLog)
//					mLog->LogLine( "Write access to TLB Lockdown register" );
				break;

			case 15:
				// Test, Clock & Idle Control.
				if (CPOpc == 1)
				{
					// LFSR operations.
					break;
				} else if (CPOpc == 2) {
					if (CRm == 1)
					{
						// Enable clock switching.
						if (mLog)
							mLog->LogLine( "Enable clock switching" );
						break;
					} else if (CRm == 2) {
						// Disable clock switching.
						if (mLog)
							mLog->LogLine( "Disable clock switching" );
						break;
					} else if (CRm == 4) {
						// Disable nMCLK output.
						if (mLog)
							mLog->LogLine( "Disable nMCLK output" );
						break;
					} else if (CRm == 8) {
						// Wait for interrupt.
						if (mLog)
							mLog->LogLine( "Wait for Interrupt" );
						mEmulator->BreakInMonitor();
						break;
					}
				}
				
			default:
				if (mLog)
				{
					mLog->FLogLine(
						"Unimplemented CP#15 access mcr 15, %i, R%i, cr%i, cr%i, {%i}",
						(int) CPOpc,
						(int) Rd,
						(int) CRn,
						(int) CRm,
						(int) CP);
				}
		}
	}
}

// -------------------------------------------------------------------------- //
//  * NativeCoprocRegisterTransfer( KUInt32 )
// -------------------------------------------------------------------------- //
void
TARMProcessor::NativeCoprocRegisterTransfer( KUInt32 inInstruction )
{
	KUInt32 theValue;
	KUInt32 Rd = (inInstruction & 0x0000F000) >> 12;
	
	if (inInstruction & 0x00100000)
	{
		if (mLog)
		{
			KUInt32 CRm = inInstruction & 0x0000000F;
			KUInt32 CP = (inInstruction & 0x000000E0) >> 5;
			KUInt32 CRn = (inInstruction & 0x000F0000) >> 16;
			KUInt32 CPOpc = (inInstruction & 0x00E00000) >> 21;

			mLog->FLogLine(
				"Unimplemented CP#10 access mcr 10, %i, R%i, cr%i, cr%i, {%i}",
				(int) CPOpc,
				(int) Rd,
				(int) CRn,
				(int) CRm,
				(int) CP);
		}
	} else {
		// Store to co-processor.
		theValue = mCurrentRegisters[Rd];
		if (Rd == kR15)
		{
			theValue += 4;
		}
		
		// Coprocessor 10 is the gateway to the sound, screen, tablet, battery,
		// and other native hardware drivers, which are emulated in Einstein

		mNativePrimitives.ExecuteNative( theValue );
	}
}

// -------------------------------------------------------------------------- //
//  * Reset( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::Reset( void )
{
	mLog->LogLine( "Reset" );
	
	BackupBankRegisters();
	mSPSRsvc = GetCPSR();
	mR14svc_Bkup = mCurrentRegisters[kR15];
	if (mMode == kFIQMode)
	{
		mCurrentRegisters[kR8] = mR8_Bkup;
		mCurrentRegisters[kR9] = mR9_Bkup;
		mCurrentRegisters[kR10] = mR10_Bkup;
		mCurrentRegisters[kR11] = mR11_Bkup;
		mCurrentRegisters[kR12] = mR12_Bkup;
	}
	mCurrentRegisters[kR13] = mR13svc_Bkup;
	mCurrentRegisters[kR14] = mR14svc_Bkup;
	mMode = kSupervisorMode;
	mMemory->SetPrivilege( true );
	mCPSR_T = 0;
	mCPSR_F = 1;
	mCPSR_I = 1;
	mCurrentRegisters[kR15] = 0x00000004; // 0 + Prefetch.
	mPendingInterrupts = 0;
}

// -------------------------------------------------------------------------- //
//  * DoUndefinedInstruction( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::DoUndefinedInstruction( void )
{
	BackupBankRegisters();
	mSPSRund = GetCPSR();
	// mR14und_Bkup = address of the next instruction after the undefined
	// instruction.
	mR14und_Bkup = mCurrentRegisters[kR15] - 4;
	if (mMode == kFIQMode)
	{
		mCurrentRegisters[kR8] = mR8_Bkup;
		mCurrentRegisters[kR9] = mR9_Bkup;
		mCurrentRegisters[kR10] = mR10_Bkup;
		mCurrentRegisters[kR11] = mR11_Bkup;
		mCurrentRegisters[kR12] = mR12_Bkup;
	}
	mCurrentRegisters[kR13] = mR13und_Bkup;
	mCurrentRegisters[kR14] = mR14und_Bkup;
	mMode = kUndefinedMode;
	mMemory->SetPrivilege( true );
	mCPSR_T = 0;
	// mCPSR_F is unchanged.
	mCPSR_I = 1;
	mCurrentRegisters[kR15] = 0x00000008; // 4 + Prefetch.
}

// -------------------------------------------------------------------------- //
//  * DoSWI( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::DoSWI( void )
{
	BackupBankRegisters();
	mSPSRsvc = GetCPSR();
	// mR14svc_Bkup = address of the next instruction after the SWI
	// instruction.
	mR14svc_Bkup = mCurrentRegisters[kR15] - 4;
	if (mMode == kFIQMode)
	{
		mCurrentRegisters[kR8] = mR8_Bkup;
		mCurrentRegisters[kR9] = mR9_Bkup;
		mCurrentRegisters[kR10] = mR10_Bkup;
		mCurrentRegisters[kR11] = mR11_Bkup;
		mCurrentRegisters[kR12] = mR12_Bkup;
	}
	mCurrentRegisters[kR13] = mR13svc_Bkup;
	mCurrentRegisters[kR14] = mR14svc_Bkup;
	mMode = kSupervisorMode;
	mMemory->SetPrivilege( true );
	mCPSR_T = 0;
	// mCPSR_F is unchanged.
	mCPSR_I = 1;
	mCurrentRegisters[kR15] = 0x0000000C; // 8 + Prefetch.
}

// -------------------------------------------------------------------------- //
//  * PrefetchAbort( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::PrefetchAbort( void )
{
	BackupBankRegisters();
	mSPSRabt = GetCPSR();
	// mR14abt_Bkup = address of the aborted instruction + 4
	mR14abt_Bkup = mCurrentRegisters[kR15];
	if (mMode == kFIQMode)
	{
		mCurrentRegisters[kR8] = mR8_Bkup;
		mCurrentRegisters[kR9] = mR9_Bkup;
		mCurrentRegisters[kR10] = mR10_Bkup;
		mCurrentRegisters[kR11] = mR11_Bkup;
		mCurrentRegisters[kR12] = mR12_Bkup;
	}
	mCurrentRegisters[kR13] = mR13abt_Bkup;
	mCurrentRegisters[kR14] = mR14abt_Bkup;
	mMode = kAbortMode;
	mMemory->SetPrivilege( true );
	mCPSR_T = 0;
	// mCPSR_F is unchanged.
	mCPSR_I = 1;
	mCurrentRegisters[kR15] = 0x00000010; // C + Prefetch.
}

// -------------------------------------------------------------------------- //
//  * DataAbort( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::DataAbort( void )
{
#if 0
	KPrintf("Data Abort at 0x%08lx, accessing 0x%08lx\n", mCurrentRegisters[kR15]-8, mMemory->GetFaultAddressRegister());
	FILE *f = fopen("/Users/matt/dev/Einstein/mmu.txt", "wb");
	mMemory->FDump(f);
	fclose(f);
#endif
	//  002ddf2c         str      r1, [r2], #4                     | E4821004
	/*
	 Data Abort at 0x00311528
	 Data Abort at 0x0031152c
	 Data Abort at 0x00025d4c
	 Data Abort at 0x0031152c
	 Data Abort at 0x0031152c
	 Data Abort at 0x00310858
	 Data Abort at 0x003121b8
	 Data Abort at 0x00259d34
	 Data Abort at 0x001a4710
	 Data Abort at 0x001a4ba4
	 Data Abort at 0x0013b6e0
	 Data Abort at 0x0003b2a4
	 Data Abort at 0x000c0910
	 Data Abort at 0x001a46e8
	 Data Abort at 0x001a4ba4
	 Data Abort at 0x0025a2a4
	 Data Abort at 0x000c2934
	 Data Abort at 0x001a4710
	 Data Abort at 0x001a4ba4
	 Data Abort at 0x002f4288
	 Data Abort at 0x001a4ba4
	 Data Abort at 0x002ddf34
	 Data Abort at 0x001235dc
	 Data Abort at 0x002ddf34
	 Data Abort at 0x002ddf34
	 Data Abort at 0x002ddf34
	 Data Abort at 0x002ddf34
	 Data Abort at 0x002595c0
	 */
	BackupBankRegisters();
	mSPSRabt = GetCPSR();
	// mR14abt_Bkup = address of the aborted instruction + 8
	mR14abt_Bkup = mCurrentRegisters[kR15];
	if (mMode == kFIQMode)
	{
		mCurrentRegisters[kR8] = mR8_Bkup;
		mCurrentRegisters[kR9] = mR9_Bkup;
		mCurrentRegisters[kR10] = mR10_Bkup;
		mCurrentRegisters[kR11] = mR11_Bkup;
		mCurrentRegisters[kR12] = mR12_Bkup;
	}
	mCurrentRegisters[kR13] = mR13abt_Bkup;
	mCurrentRegisters[kR14] = mR14abt_Bkup;
	mMode = kAbortMode;
	mMemory->SetPrivilege( true );
	mCPSR_T = 0;
	// mCPSR_F is unchanged.
	mCPSR_I = 1;
	mCurrentRegisters[kR15] = 0x00000014; // 10 + Prefetch.
}

// -------------------------------------------------------------------------- //
//  * IRQ( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::IRQ( void )
{
	BackupBankRegisters();
	mSPSRirq = GetCPSR();
	// mR14irq_Bkup = address of the next instruction to be executed + 4
	mR14irq_Bkup = mCurrentRegisters[kR15];
	if (mMode == kFIQMode)
	{
		mCurrentRegisters[kR8] = mR8_Bkup;
		mCurrentRegisters[kR9] = mR9_Bkup;
		mCurrentRegisters[kR10] = mR10_Bkup;
		mCurrentRegisters[kR11] = mR11_Bkup;
		mCurrentRegisters[kR12] = mR12_Bkup;
	}
	mCurrentRegisters[kR13] = mR13irq_Bkup;
	mCurrentRegisters[kR14] = mR14irq_Bkup;
	mMode = kIRQMode;
	mMemory->SetPrivilege( true );
	mCPSR_T = 0;
	// mCPSR_F is unchanged.
	mCPSR_I = 1;
	mCurrentRegisters[kR15] = 0x0000001C; // 18 + Prefetch.
}

// -------------------------------------------------------------------------- //
//  * FIQ( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::FIQ( void )
{
	BackupBankRegisters();
	mSPSRfiq = GetCPSR();
	// mR14fiq_Bkup = address of the next instruction to be executed + 4
	mR14fiq_Bkup = mCurrentRegisters[kR15];
	mCurrentRegisters[kR8] = mR8fiq_Bkup;
	mCurrentRegisters[kR9] = mR9fiq_Bkup;
	mCurrentRegisters[kR10] = mR10fiq_Bkup;
	mCurrentRegisters[kR11] = mR11fiq_Bkup;
	mCurrentRegisters[kR12] = mR12fiq_Bkup;
	mCurrentRegisters[kR13] = mR13fiq_Bkup;
	mCurrentRegisters[kR14] = mR14fiq_Bkup;
	mMode = kFIQMode;
	mMemory->SetPrivilege( true );
	mCPSR_T = 0;
	mCPSR_F = 1;
	mCPSR_I = 1;
	mCurrentRegisters[kR15] = 0x00000020; // 1C + Prefetch.
}

// -------------------------------------------------------------------------- //
//  * GetCPSR( void )
// -------------------------------------------------------------------------- //
KUInt32
TARMProcessor::GetCPSR( void )
{
	KUInt32 theResult = 0;
	if (mCPSR_N)
	{
		theResult |= kPSR_NBit;
	}
	if (mCPSR_Z)
	{
		theResult |= kPSR_ZBit;
	}
	if (mCPSR_C)
	{
		theResult |= kPSR_CBit;
	}
	if (mCPSR_V)
	{
		theResult |= kPSR_VBit;
	}
	if (mCPSR_I)
	{
		theResult |= kPSR_IBit;
	}
	if (mCPSR_F)
	{
		theResult |= kPSR_FBit;
	}
	if (mCPSR_T)
	{
		theResult |= kPSR_TBit;
	}
	theResult |= mMode;
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * SetCPSR( KUInt32)
// -------------------------------------------------------------------------- //
void
TARMProcessor::SetCPSR( KUInt32 inNewValue )
{
	mCPSR_N = (inNewValue & kPSR_NBit) != 0;
	mCPSR_Z = (inNewValue & kPSR_ZBit) != 0;
	mCPSR_C = (inNewValue & kPSR_CBit) != 0;
	mCPSR_V = (inNewValue & kPSR_VBit) != 0;
	mCPSR_I = (inNewValue & kPSR_IBit) != 0;
	mCPSR_F = (inNewValue & kPSR_FBit) != 0;
	mCPSR_T = (inNewValue & kPSR_TBit) != 0;
	
	KUInt32 theMode = inNewValue & kPSR_ModeMask;
	if (theMode != (KUInt32) mMode)
	{
		BackupBankRegisters();
		if (theMode == kFIQMode)
		{
			mCurrentRegisters[kR8] = mR8fiq_Bkup;
			mCurrentRegisters[kR9] = mR9fiq_Bkup;
			mCurrentRegisters[kR10] = mR10fiq_Bkup;
			mCurrentRegisters[kR11] = mR11fiq_Bkup;
			mCurrentRegisters[kR12] = mR12fiq_Bkup;
		} else if (mMode == kFIQMode) {
			mCurrentRegisters[kR8] = mR8_Bkup;
			mCurrentRegisters[kR9] = mR9_Bkup;
			mCurrentRegisters[kR10] = mR10_Bkup;
			mCurrentRegisters[kR11] = mR11_Bkup;
			mCurrentRegisters[kR12] = mR12_Bkup;
		}

		switch (theMode)
		{
			case kUserMode:
				// Get back to user mode.
				mMode = kUserMode;
				mMemory->SetPrivilege( false );
				mCurrentRegisters[kR13] = mR13_Bkup;
				mCurrentRegisters[kR14] = mR14_Bkup;
				break;
			
			case kFIQMode:
				mMode = kFIQMode;
				mMemory->SetPrivilege( true );
				mCurrentRegisters[kR13] = mR13fiq_Bkup;
				mCurrentRegisters[kR14] = mR14fiq_Bkup;
				break;
			
			case kIRQMode:
				mMode = kIRQMode;
				mMemory->SetPrivilege( true );
				mCurrentRegisters[kR13] = mR13irq_Bkup;
				mCurrentRegisters[kR14] = mR14irq_Bkup;
				break;
			
			case kSupervisorMode:
				mMode = kSupervisorMode;
				mMemory->SetPrivilege( true );
				mCurrentRegisters[kR13] = mR13svc_Bkup;
				mCurrentRegisters[kR14] = mR14svc_Bkup;
				break;
			
			case kAbortMode:
				mMode = kAbortMode;
				mMemory->SetPrivilege( true );
				mCurrentRegisters[kR13] = mR13abt_Bkup;
				mCurrentRegisters[kR14] = mR14abt_Bkup;
				break;
			
			case kUndefinedMode:
				mMode = kUndefinedMode;
				mMemory->SetPrivilege( true );
				mCurrentRegisters[kR13] = mR13und_Bkup;
				mCurrentRegisters[kR14] = mR14und_Bkup;
				break;
			
			case kSystemMode:
				mMode = kSystemMode;
				mMemory->SetPrivilege( true );
				mCurrentRegisters[kR13] = mR13_Bkup;
				mCurrentRegisters[kR14] = mR14_Bkup;
				break;
			
			default:
				if (mLog)
				{
					mLog->FLogLine(
						"SetCPSR with unknown mode (%i)", (int) theMode );
				}
				mEmulator->BreakInMonitor();
		}
	}

	if (((!mCPSR_F) && (mPendingInterrupts & kFIQInterrupt))
		|| ((!mCPSR_I) && (mPendingInterrupts & kIRQInterrupt)))
	{
		mEmulator->SignalInterrupt();
	}
}

// -------------------------------------------------------------------------- //
//  * GetSPSR( void )
// -------------------------------------------------------------------------- //
KUInt32
TARMProcessor::GetSPSR( void )
{
	switch (mMode)
	{
		case kUserMode:
			// At MonitorEntryGlue and elsewhere, the OS accesses SPSR in User
			// mode and apparently gets CPSR.
			return GetCPSR();

		case kSystemMode:
		default:
			if (mLog)
			{
				mLog->FLogLine(
					"Accessing SPSR in Unknown/System mode (%i)", (int) mMode );
			}
			mEmulator->BreakInMonitor();
			return 0;

		case kUndefinedMode:
			return mSPSRund;

		case kFIQMode:
			return mSPSRfiq;

		case kIRQMode:
			return mSPSRirq;

		case kSupervisorMode:
			return mSPSRsvc;

		case kAbortMode:
			return mSPSRabt;
	}
}

// -------------------------------------------------------------------------- //
//  * SetSPSR( KUInt32 )
// -------------------------------------------------------------------------- //
void
TARMProcessor::SetSPSR( KUInt32 inNewSPSR )
{
	switch (mMode)
	{
		case kUserMode:
		case kSystemMode:
		default:
			return;

		case kUndefinedMode:
			mSPSRund = inNewSPSR;
			break;

		case kFIQMode:
			mSPSRfiq = inNewSPSR;
			break;

		case kIRQMode:
			mSPSRirq = inNewSPSR;
			break;

		case kSupervisorMode:
			mSPSRsvc = inNewSPSR;
			break;

		case kAbortMode:
			mSPSRabt = inNewSPSR;
			break;
	}
}

// -------------------------------------------------------------------------- //
//  * BackupBankRegisters( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::BackupBankRegisters( void )
{
	switch (mMode)
	{
		case kUserMode:
		case kSystemMode:
			mR13_Bkup = mCurrentRegisters[kR13];
			mR14_Bkup = mCurrentRegisters[kR14];
			break;
			
		case kSupervisorMode:
			mR13svc_Bkup = mCurrentRegisters[kR13];
			mR14svc_Bkup = mCurrentRegisters[kR14];
			break;
			
		case kAbortMode:
			mR13abt_Bkup = mCurrentRegisters[kR13];
			mR14abt_Bkup = mCurrentRegisters[kR14];
			break;
			
		case kUndefinedMode:
			mR13und_Bkup = mCurrentRegisters[kR13];
			mR14und_Bkup = mCurrentRegisters[kR14];
			break;
			
		case kIRQMode:
			mR13irq_Bkup = mCurrentRegisters[kR13];
			mR14irq_Bkup = mCurrentRegisters[kR14];
			break;
			
		case kFIQMode:
			mR8fiq_Bkup = mCurrentRegisters[kR8];
			mR9fiq_Bkup = mCurrentRegisters[kR9];
			mR10fiq_Bkup = mCurrentRegisters[kR10];
			mR11fiq_Bkup = mCurrentRegisters[kR11];
			mR12fiq_Bkup = mCurrentRegisters[kR12];
			mR13fiq_Bkup = mCurrentRegisters[kR13];
			mR14fiq_Bkup = mCurrentRegisters[kR14];
			break;
	}
	
	if (mMode != kFIQMode)
	{
		mR8_Bkup = mCurrentRegisters[kR8];
		mR9_Bkup = mCurrentRegisters[kR9];
		mR10_Bkup = mCurrentRegisters[kR10];
		mR11_Bkup = mCurrentRegisters[kR11];
		mR12_Bkup = mCurrentRegisters[kR12];
	}
}

// -------------------------------------------------------------------------- //
//  * ResetInterrupt( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::ResetInterrupt( void )
{
	KPrintf( "Reset Interrupt (Rebooting!)\n" );
	mPendingInterrupts |= kResetInterrupt;
}

// -------------------------------------------------------------------------- //
//  * FIQInterrupt( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::FIQInterrupt( void )
{
	mPendingInterrupts |= kFIQInterrupt;
	if (!mCPSR_F)
	{
		mEmulator->SignalInterrupt();
	}
}

// -------------------------------------------------------------------------- //
//  * ClearFIQInterrupt( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::ClearFIQInterrupt( void )
{
	mPendingInterrupts &= ~kFIQInterrupt;
}

// -------------------------------------------------------------------------- //
//  * IRQInterrupt( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::IRQInterrupt( void )
{
	mPendingInterrupts |= kIRQInterrupt;
	if (!mCPSR_I)
	{
		mEmulator->SignalInterrupt();
	}
}

// -------------------------------------------------------------------------- //
//  * ClearIRQInterrupt( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::ClearIRQInterrupt( void )
{
	mPendingInterrupts &= ~kIRQInterrupt;
}

// -------------------------------------------------------------------------- //
//  * PrintRegisters( void )
// -------------------------------------------------------------------------- //
void
TARMProcessor::PrintRegisters( void )
{
	if (mLog) {
		for (int indexRegisters = 0; indexRegisters < 16; indexRegisters++) {
			mLog->FLogLine("R%i = %.8X",
						indexRegisters,
						(unsigned int) mCurrentRegisters[indexRegisters] );
		}
		mLog->FLogLine( "CPSR = %.8X", (unsigned int) GetCPSR() );
	}
}


// -------------------------------------------------------------------------- //
//  * TransferState( TStream* )
// -------------------------------------------------------------------------- //
void
TARMProcessor::TransferState( TStream* inStream ) 
{
	KUInt32 tmp;
	
	// First, transfer the native primitives stuff.
	mNativePrimitives.TransferState( inStream );

	// Then transfer the CPU specific stuff.
	inStream->TransferInt32ArrayBE(
					mCurrentRegisters,
					sizeof(mCurrentRegisters) / sizeof(KUInt32) );
	inStream->TransferBoolean( mCPSR_N );
	inStream->TransferBoolean( mCPSR_Z );
	inStream->TransferBoolean( mCPSR_C );
	inStream->TransferBoolean( mCPSR_V );
	inStream->TransferBoolean( mCPSR_I );
	inStream->TransferBoolean( mCPSR_F );
	inStream->TransferBoolean( mCPSR_T );
	inStream->TransferInt32BE( mR8_Bkup );
	inStream->TransferInt32BE( mR9_Bkup );
	inStream->TransferInt32BE( mR10_Bkup );
	inStream->TransferInt32BE( mR11_Bkup );
	inStream->TransferInt32BE( mR12_Bkup );
	inStream->TransferInt32BE( mR13_Bkup );
	inStream->TransferInt32BE( mR14_Bkup );
	inStream->TransferInt32BE( mR13svc_Bkup );
	inStream->TransferInt32BE( mR14svc_Bkup );
	inStream->TransferInt32BE( mR13abt_Bkup );
	inStream->TransferInt32BE( mR14abt_Bkup );
	inStream->TransferInt32BE( mR13und_Bkup );
	inStream->TransferInt32BE( mR14und_Bkup );
	inStream->TransferInt32BE( mR13irq_Bkup );
	inStream->TransferInt32BE( mR14irq_Bkup );
	inStream->TransferInt32BE( mR8fiq_Bkup );
	inStream->TransferInt32BE( mR9fiq_Bkup );
	inStream->TransferInt32BE( mR10fiq_Bkup );
	inStream->TransferInt32BE( mR11fiq_Bkup );
	inStream->TransferInt32BE( mR12fiq_Bkup );
	inStream->TransferInt32BE( mR13fiq_Bkup );
	inStream->TransferInt32BE( mR14fiq_Bkup );
	inStream->TransferInt32BE( mSPSRsvc );
	inStream->TransferInt32BE( mSPSRabt );
	inStream->TransferInt32BE( mSPSRund );
	inStream->TransferInt32BE( mSPSRirq );
	inStream->TransferInt32BE( mSPSRfiq );
	
	tmp = (KUInt32)mMode; inStream->TransferInt32BE( tmp ); mMode = (EMode)tmp;
	inStream->TransferInt32BE( mPendingInterrupts );
}

// -------------------------------------------------------------------------- //
//  * IsIRQEnabled()
// -------------------------------------------------------------------------- //
bool TARMProcessor::IsIRQEnabled()
{
	return (mCPSR_I==0);
}


// -------------------------------------------------------------------------- //
//  * IsFIQEnabled()
// -------------------------------------------------------------------------- //
bool TARMProcessor::IsFIQEnabled()
{
	return (mCPSR_F==0);
}


// -------------------------------------------------------------------------- //
//  * AnyInterruptEnabled()
// -------------------------------------------------------------------------- //
bool TARMProcessor::IsAnyInterruptEnabled()
{
	return (mCPSR_F==0) || (mCPSR_I==0);
}






// =========================================================================== //
// Some programming languages manage to absorb change, but withstand progress. //
//                 -- Epigrams in Programming, ACM SIGPLAN Sept. 1982          //
// =========================================================================== //
