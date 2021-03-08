// ==============================
// File:			TARMProcessor.h
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

#ifndef _TARMPROCESSOR_H
#define _TARMPROCESSOR_H

#include <K/Defines/KDefinitions.h>

#include "Emulator/TMemory.h"
#include "TNativePrimitives.h"
#include "Emulator/JIT/JIT.h"

class TStream;


///
/// Class for the ARM processor.
///
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TARMProcessor
{
public:
    // These are macros referring to the current implementation.
    // (we can only have one JIT implementation)
	friend class JITClass;
	friend class JITPageClass;

	///
	/// Constructor from the access to the memory space.
	/// Initializes the processor in boot state.
	///
	/// \param inLog		interface for logging.
	/// \param inMemory		interface to the memory space.
	///
	TARMProcessor(
				TLog* inLog,
				TMemory* inMemory );

	///
	/// List of registers.
	///
	enum {
		kR0					= 0,
		kR1					= 1,
		kR2					= 2,
		kR3					= 3,
		kR4					= 4,
		kR5					= 5,
		kR6					= 6,
		kR7					= 7,
		kR8					= 8,
		kR9					= 9,
		kR10				= 10,
		kR11				= 11,
		kR12				= 12,
		kR13				= 13,
		kR14				= 14,
		kR15				= 15
	};

	///
	/// List of modes.
	///
	enum EMode {
		kUserMode			= 0x10,		// 0b10000
		kFIQMode			= 0x11,		// 0b10001 - FIQ
		kIRQMode			= 0x12,		// 0b10010 - IRQ
		kSupervisorMode		= 0x13,		// 0b10011 - SVC
		kAbortMode			= 0x17,		// 0b10111 - ABT
		kUndefinedMode		= 0x1B,		// 0b11011 - UND
		kSystemMode			= 0x1F		// 0b11111
	};

	///
	/// Bits of PSR
	///
	enum {
		kPSR_NBit			= 0x80000000,	///< 31
		kPSR_ZBit			= 0x40000000,	///< 30
		kPSR_CBit			= 0x20000000,	///< 29
		kPSR_VBit			= 0x10000000,	///< 28
		kPSR_IBit			= 0x00000080,	///< 7
		kPSR_FBit			= 0x00000040,	///< 6
		kPSR_TBit			= 0x00000020,	///< 5
		kPSR_ModeMask		= 0x0000001F	///< 0-4
	};
	
	///
	/// Interrupt bits.
	///
	enum {
		kResetInterrupt		= 0x0000004,
		kFIQInterrupt		= 0x0000002,
		kIRQInterrupt		= 0x0000001
	};
	
	///
	/// Set the emulator.
	///
	/// \param inEmulator emulator (access to hardware managers).
	///
	void		SetEmulator( TEmulator* inEmulator )
		{
			mEmulator = inEmulator;
			mNativePrimitives.SetEmulator( inEmulator );
		}

	///
	/// Accessor on the emulator.
	///
	/// \return the emulator (access to hardware managers).
	///
	TEmulator*	GetEmulator( void ) const
		{
			return mEmulator;
		}

	///
	/// Accessor on the memory interface.
	///
	/// \return the memory interface.
	///
	TMemory*	GetMemory( void ) const
		{
			return mMemory;
		}

	///
	/// Accessor on the mode.
	///
	/// \return the current mode.
	///
	EMode		GetMode( void ) const
		{
			return mMode;
		}

	///
	/// Print a complete status of the processor to stdout.
	///
	void	PrintRegisters( void );

	///
	/// Return the value of a given register.
	///
	/// \param inIndex  index of the register to get.
	///
	inline KUInt32 GetRegister( int inIndex )
		{
			return mCurrentRegisters[inIndex];
		}

	///
	/// Set the value of a given register.
	///
	/// \param inIndex  index of the register to set.
	/// \param inValue  value to set.
	///
	inline void SetRegister( KUInt32 inIndex, KUInt32 inValue )
		{
			mCurrentRegisters[inIndex] = inValue;
		}

	///
	/// Get the CPSR from the various bits and mode.
	/// Remark: in our emulator, DNM bits are always forgotten by the core.
	///
	/// \return the value of the CPSR (32 bits)
	///
	KUInt32	GetCPSR( void );
	
	///
	/// Get the current SPSR.
	/// Return 0 if the current mode is user or system.
	///
	/// \return the value of the SPSR (32 bits) for the current mode.
	///
	KUInt32	GetSPSR( void );
	
	///
	/// Set the CPSR and update the mode accordingly.
	///
	/// \param inNewValue   the new value of the CPSR (32 bits)
	///
	void	SetCPSR( KUInt32 inNewValue );
	
	///
	/// Set the current SPSR.
	/// Does nothing if the current mode is user or system.
	///
	/// \param inNewSPSR new value for the SPSR (32 bits) for the current mode.
	///
	void	SetSPSR( KUInt32 inNewSPSR );

	///
	/// Undefined Instruction handling.
	/// Pre-requirement: PC is the address of the undefined instruction + 8.
	///
	void DoUndefinedInstruction( void );
	
	///
	/// SWI handling.
	/// Pre-requirement: PC is the address of the SWI instruction + 8.
	///
	void DoSWI( void );
	
	///
	/// Reset hardware Interrupt.
	/// The interrupt will be handled at the beginning of the next step cycle.
	///
	void	ResetInterrupt( void );
	
	///
	/// Assert FIQ hardware Interrupt.
	/// The interrupt will be handled at the beginning of the next step cycle.
	///
	void	FIQInterrupt( void );
	
	///
	/// Clear FIQ hardware Interrupt.
	///
	void	ClearFIQInterrupt( void );
	
	///
	/// Assert IRQ hardware Interrupt.
	/// The interrupt will be handled at the beginning of the next step cycle.
	///
	void	IRQInterrupt( void );
	
	///
	/// Clear IRQ hardware Interrupt.
	///
	void	ClearIRQInterrupt( void );
	
	///
	/// Determine if there is a hardware interrupt currently asserted.
	///
	/// \return \c true if there is, \c false otherwise.
	///
	Boolean IsThereAnyHardwareInterruptAsserted( void ) const
		{
			return mPendingInterrupts != 0;
		}
	
	///
	/// Save or retore the state to a stream.
	///
	void	TransferState( TStream* inStream );
		
	///
	/// System Coproc Register Transfer (MMU & Co).
	///
	/// \param inInstruction	current instruction.
	///
	void SystemCoprocRegisterTransfer( KUInt32 inInstruction );
	
	///
	/// Coproc Register Transfer for native primitives.
	///
	/// \param inInstruction	current instruction.
	///
	void NativeCoprocRegisterTransfer( KUInt32 inInstruction );
	
	///
	/// Backup bank registers before the mode (and the bank) changes.
	/// This operation may backup more than required. However, I don't think
	/// it's here that we will spend most of our time.
	///
	inline void BackupBankRegisters( void );
	
	///
	/// Reset handling.
	///
	void Reset( void );
	
	///
	/// PrefetchAbort handling.
	/// Pre-requirement: PC is the address of the aborted instruction + 4.
	///
	void PrefetchAbort( void );
	
	///
	/// DataAbort handling.
	/// Pre-requirement: PC is the address of the aborted instruction + 8.
	///
	void DataAbort( void );
	
	///
	/// IRQ handling.
	/// Doesn't check interrupt disable flags.
	/// Pre-requirement: PC is the address of the current instruction + 8.
	///
	void IRQ( void );
	
	///
	/// FIQ handling.
	/// Doesn't check interrupt disable flags.
	/// Pre-requirement: PC is the address of the current instruction + 8.
	///
	void FIQ( void );

	///
	/// Inline functions for all tests in the condition field of ARM commands.
	/// Used in precompiled code.
	///
	Boolean TestEQ() { return mCPSR_Z; }
	Boolean TestNE() { return !mCPSR_Z; }
	Boolean TestCS() { return mCPSR_C; }
	Boolean TestCC() { return !mCPSR_C; }
	Boolean TestMI() { return mCPSR_N; }
	Boolean TestPL() { return !mCPSR_N; }
	Boolean TestVS() { return mCPSR_V; }
	Boolean TestVC() { return !mCPSR_V; }
	Boolean TestHI() { return ((mCPSR_C) && !(mCPSR_Z)); }
	Boolean TestLS() { return (!(mCPSR_C) || (mCPSR_Z)); }
	Boolean TestGE() { return (mCPSR_N == mCPSR_V); }
	Boolean TestLT() { return (mCPSR_N != mCPSR_V); }
	Boolean TestGT() { return ((!mCPSR_Z) && (mCPSR_N == mCPSR_V)); }
	Boolean TestLE() { return ((mCPSR_Z) || (mCPSR_N != mCPSR_V)); }

	/// \name Registers
	
	// At any time, 16 general purpose registers (R0-R15) may be visible.
	// The mCurrentRegisters array refers to them.
	// These registers may be switched with backup copies when the mode
	// changes.
	KUInt32		mCurrentRegisters[16];		///< The current bank with the
											///< currently visible 16 general
											///< purpose registers.

	Boolean		mCPSR_N;					///< N Flag of CPSR.
	Boolean		mCPSR_Z;					///< Z Flag of CPSR.
	Boolean		mCPSR_C;					///< C Flag of CPSR.
	Boolean		mCPSR_V;					///< V Flag of CPSR.
	Boolean		mCPSR_I;					///< I Bit of CPSR.
	Boolean		mCPSR_F;					///< F Bit of CPSR.
	Boolean		mCPSR_T;					///< T Bit of CPSR.
	KUInt32		mR8_Bkup;					///< A copy of R8
	KUInt32		mR9_Bkup;					///< A copy of R9
	KUInt32		mR10_Bkup;					///< A copy of R10
	KUInt32		mR11_Bkup;					///< A copy of R11
	KUInt32		mR12_Bkup;					///< A copy of R12
	KUInt32		mR13_Bkup;					///< A copy of R13
	KUInt32		mR14_Bkup;					///< A copy of R14
	KUInt32		mR13svc_Bkup;				///< A copy of R13svc
	KUInt32		mR14svc_Bkup;				///< A copy of R14svc
	KUInt32		mR13abt_Bkup;				///< A copy of R13abt
	KUInt32		mR14abt_Bkup;				///< A copy of R14abt
	KUInt32		mR13und_Bkup;				///< A copy of R13und
	KUInt32		mR14und_Bkup;				///< A copy of R14und
	KUInt32		mR13irq_Bkup;				///< A copy of R13irq
	KUInt32		mR14irq_Bkup;				///< A copy of R14irq
	KUInt32		mR8fiq_Bkup;				///< A copy of R8fiq
	KUInt32		mR9fiq_Bkup;				///< A copy of R9fiq
	KUInt32		mR10fiq_Bkup;				///< A copy of R10fiq
	KUInt32		mR11fiq_Bkup;				///< A copy of R11fiq
	KUInt32		mR12fiq_Bkup;				///< A copy of R12fiq
	KUInt32		mR13fiq_Bkup;				///< A copy of R13fiq
	KUInt32		mR14fiq_Bkup;				///< A copy of R14fiq
	KUInt32		mSPSRsvc;					///< Saved PSR for svc mode
	KUInt32		mSPSRabt;					///< Saved PSR for abt mode
	KUInt32		mSPSRund;					///< Saved PSR for und mode
	KUInt32		mSPSRirq;					///< Saved PSR for irq mode
	KUInt32		mSPSRfiq;					///< Saved PSR for fiq mode

	///
	/// return true if the IRQ interrupt is enabled
	///
	bool IsIRQEnabled();

	///
	/// return true if the FIQ interrupt is enabled
	///
	bool IsFIQEnabled();

	///
	/// return true if any of the interrupts is enabled
	///
	bool IsAnyInterruptEnabled();

private:
	/// \name Variables
	EMode		mMode;						///< Current mode.
	KUInt32		mPendingInterrupts;			///< Waiting interrupts.

	TLog*		mLog;						///< Interface for logging.
	TMemory*	mMemory;					///< Reference to the access to memory.
	TNativePrimitives   mNativePrimitives;  ///< Interface for native primitives.
	TEmulator*  mEmulator;					///< Interface to the emulator.
};

#endif
		// _TARMPROCESSOR_H

// ============================================================================= //
// **** IMPORTANT ****  ALL USERS PLEASE NOTE ****                               //
//                                                                               //
// Due to a recent systems overload error your recent disk files have been       //
// erased.  Therefore, in accordance with the UNIX Basic Manual, University of   //
// Washington Geophysics Manual, and Bylaw 9(c), Section XII of the Revised      //
// Federal Communications Act, you are being granted Temporary Disk Space,       //
// valid for three months from this date, subject to the restrictions set forth  //
// in Appendix II of the Federal Communications Handbook (18th edition) as well  //
// as the references mentioned herein.  You may apply for more disk space at any //
// time.  Disk usage in or above the eighth percentile will secure the removal   //
// of all restrictions and you will immediately receive your permanent disk      //
// space.  Disk usage in the sixth or seventh percentile will not effect the     //
// validity of your temporary disk space, though its expiration date may be      //
// extended for a period of up to three months.  A score in the fifth percentile //
// or below will result in the withdrawal of your Temporary Disk space.          //
// ============================================================================= //
