// ==============================
// File:			TJITLLVMTranslator.h
// Project:			Einstein
//
// Copyright 2013-2014 by Paul Guyot (pguyot@kallisys.net).
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

#ifdef JITTARGET_LLVM

#ifndef _TJITLLVMTRANSLATOR_H
#define _TJITLLVMTRANSLATOR_H

#include "TJITPage.h"
#include "TJITLLVMPage.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>

#include <vector>
#include <deque>

using TJITLLVMPageFunctions = std::map<KUInt32, std::pair<llvm::Function*, JITFuncPtr>>;

///
/// Class for translating instructions into a function using LLVM.
///
class TJITLLVMTranslator
{
public:
    friend class TJITLLVMPage;
    
    ///
    /// Initialize or reset the translator for a new page.
    ///
	void Init(KUInt32 inVAddr, KUInt32* inPointer);
    
    ///
    /// Translate from an entry point.
	/// This function will create additional functions for possible re-entries.
	///
    /// \param offsetInPage     instruction offset in page
    /// \param inPageSize       page size (in instructions)
    /// \param inModule			module to generate the function into.
	/// \param ioFunctions		functions of the page, as
    ///
    void TranslateEntryPoint(KUInt32 offsetInPage, KUInt32 inPageSize, llvm::Module* inModule, TJITLLVMPageFunctions& ioFunctions);
    
    ///
    /// Translate for a single instruction (typically with Step).
    /// \param offsetInPage     instruction offset in page
    ///
    llvm::Function* TranslateSingleInstruction(KUInt32 offsetInPage, llvm::Module* inModule);

private:
	class FrameTranslator {
	public:
		///
		/// Setup prologue and epilogue, clearing register values.
		///
		FrameTranslator(KUInt32 baseVAddress, KUInt32* basePointer, KUInt32 offsetInPage, KUInt32 inPageSize, llvm::Module* inModule, TJITLLVMPageFunctions& ioFunctions);
		
		///
		/// Finish function construction.
		///
		void Finish();
		
		///
		/// Translate instructions from a given offset.
		///
		void Translate(KUInt32 offsetInPage);
		
		///
		/// Return a block at the given position, creating it if required
		/// and pushing it to the mPending stack.
		///
		llvm::BasicBlock* GetBlock(KUInt32 offsetInPage);
		
		///
		/// Add an entry point function if required that will call the inner
		/// function to jump to the provided offset.
		///
		void AddFunctionIfRequired(KUInt32 offsetInPage);
		
		///
		/// Translate a test, branching to next instruction if test fails.
		///
		void DoTranslateTest(llvm::BasicBlock* thisInstructionBlock, llvm::BasicBlock* nextInstructionBlock, int testKind);
		
		///
		/// Make sure a register is allocated, creating alloca instruction in prologue if it isn't.
		/// (actual copy will be created by Finish).
		///
		void EnsureAllocated(llvm::Value** reg, int numBits);
		
		///
		/// Prepare exit at the current builder insertion point.
		/// Saves back all used registers.
		///
		void PrepareExit(void);
		
		///
		/// Translate 00 instructions.
		///
		void DoTranslate_00(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate 01 instructions.
		///
		void DoTranslate_01(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate 10 instructions.
		///
		void DoTranslate_10(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate 11 instructions (SWI and Coproc)
		///
		void Translate_SWIAndCoproc(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate single data swap instructions.
		///
		void Translate_SingleDataSwap(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate Multiply and accumulate.
		///
		void Translate_MultiplyAndAccumulate(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate Multiply.
		///
		void Translate_Multiply(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate Data Processing insructions.
		///
		void Translate_DataProcessing(KUInt32 offsetInPage, KUInt32 inInstruction);

		///
		/// Translate MSR insructions.
		///
		void Translate_MSR(KUInt32 offsetInPage, KUInt32 inInstruction);

		///
		/// Translate MRS insructions.
		///
		void Translate_MRS(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate BKPT insructions.
		///
		void Translate_BKPT(KUInt32 offsetInPage, KUInt32 inInstruction);

		///
		/// Translate single data transfer insructions.
		///
		void Translate_SingleDataTransfer(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate branch instructions (including branch with links).
		///
		void Translate_Branch(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate LDM(1) and LDM(3) instructions.
		///
		void Translate_LDM13(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate LDM(2) instructions.
		///
		void Translate_LDM2(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate STM(1) instructions.
		///
		void Translate_STM1(KUInt32 offsetInPage, KUInt32 inInstruction);
		
		///
		/// Translate STM(2) instructions.
		///
		void Translate_STM2(KUInt32 offsetInPage, KUInt32 inInstruction);

		///
		/// Get the base address for a block data transfer.
		///
		llvm::Value* BuildGetBlockDataTransferBaseAddress(KUInt32 inInstruction);
		
		///
		/// Get the write back address for a block data transfer.
		///
		llvm::Value* BuildGetBlockDataTransferWriteback(KUInt32 inInstruction, llvm::Value* startAddress, llvm::Value* endAddress);
		
		///
		/// Generate a call to a native function.
		/// The function is imported in the module if required.
		///
		void BuildExitToFunction(const char* funcName, KUInt32 exitPC);

		///
		/// Generate a call to a native function.
		/// The function is imported in the module if required.
		/// This version is based on a dynamic PC.
		/// If exitPC is null, mPC is not written (we assume it was already set earlier).
		///
		void BuildExitToFunction(const char* funcName, llvm::Value* exitPC);
		
		///
		/// Build instructions to get CPSR.
		///
		llvm::Value* BuildGetCPSR();

		///
		/// Build instructions to get SPSR.
		///
		llvm::Value* BuildGetSPSR(llvm::Value* mode);

		///
		/// Build instructions to set CPSR with a given mask.
		///
		void BuildSetCPSR(llvm::Value* cpsr, llvm::Value* currentMode, llvm::Value* setControlBits, llvm::Value* setFlagsBits, llvm::Value* exitPC);

		///
		/// Build instructions to set SPSR with a given mask.
		///
		void BuildSetSPSR(llvm::Value* spsr, llvm::Value* currentMode);

		///
		/// Backup bank registers.
		///
		void BuildBackupBankRegisters(llvm::Value* currentMode);

		///
		/// Restore bank registers.
		///
		void BuildRestoreBankRegisters(llvm::Value* currentMode, llvm::Value* newMode);
		
		///
		/// Build instructions to fetch emulation mode.
		///
		llvm::Value* BuildGetEmulationMode();
		
		///
		/// Retrieve a given function. Import it in the module if required.
		///
		llvm::Function* EnsureFunction(llvm::FunctionType* funcType, const char* funcName);

		///
		/// Generate code to perform shift no carry of operand.
		///
		llvm::Value* GetShiftNoCarryNoR15(KUInt32 inShift);

	private:
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
		
		enum TARMProcessorField {
			i_mCurrentRegisters_R0 = 0,
			i_mCurrentRegisters_R1,
			i_mCurrentRegisters_R2,
			i_mCurrentRegisters_R3,
			i_mCurrentRegisters_R4,
			i_mCurrentRegisters_R5,
			i_mCurrentRegisters_R6,
			i_mCurrentRegisters_R7,
			i_mCurrentRegisters_R8,
			i_mCurrentRegisters_R9,
			i_mCurrentRegisters_R10,
			i_mCurrentRegisters_R11,
			i_mCurrentRegisters_R12,
			i_mCurrentRegisters_R13,
			i_mCurrentRegisters_R14,
			i_mCurrentRegisters_R15,
			i_mCPSR_N,
			i_mCPSR_Z,
			i_mCPSR_C,
			i_mCPSR_V,
			i_mCPSR_I,
			i_mCPSR_F,
			i_mCPSR_T,
			i_mR8_Bkup,
			i_mR9_Bkup,
			i_mR10_Bkup,
			i_mR11_Bkup,
			i_mR12_Bkup,
			i_mR13_Bkup,
			i_mR14_Bkup,
			i_mR13svc_Bkup,
			i_mR14svc_Bkup,
			i_mR13abt_Bkup,
			i_mR14abt_Bkup,
			i_mR13und_Bkup,
			i_mR14und_Bkup,
			i_mR13irq_Bkup,
			i_mR14irq_Bkup,
			i_mR8fiq_Bkup,
			i_mR9fiq_Bkup,
			i_mR10fiq_Bkup,
			i_mR11fiq_Bkup,
			i_mR12fiq_Bkup,
			i_mR13fiq_Bkup,
			i_mR14fiq_Bkup,
			i_mSPSRsvc,
			i_mSPSRabt,
			i_mSPSRund,
			i_mSPSRirq,
			i_mSPSRfiq,
		
			i_mMode,
			i_mPendingInterrupts
		};

	
		llvm::IRBuilder<>				mBuilder;				///< IR builder to generate code.
	    llvm::Module*                   mModule;
		TJITLLVMPageFunctions&			mModuleFunctions;		///< Module functions (modified by the translator).
		// FunctionPassManager is legacy, but what is it replaced with?
		llvm::FunctionPassManager		mFPM;
		
		/// The following values are used during translation
		llvm::Value*				    mProcessor;
		llvm::Value*				    mSignal;
		llvm::Value*				    mOffsetArg;
		llvm::BasicBlock*               mPrologue;
		llvm::BasicBlock*			    mMainExit;
		std::vector<llvm::BasicBlock*>  mLabels;
		std::map<KUInt32, llvm::BasicBlock*> mNewFunctionsBlocks;
		///< The block the inner function should switch to for the given offset parameter value.
		llvm::Function*					mFunction;				///< Inner function owning most blocks.
		KUInt32						    mCurrentVAddress;		///< Page base virtual address, unless we're
		///< translating a single instruction.
		KUInt32*					    mCurrentPointer;		///< Likewise, usually mBasePointer
		KUInt32						    mInstructionCount;		///< This is used to optimize (forward) branches
		                                                        ///< as well as load from page.
		KUInt32							mOffsetInPage;			///< offset in page.
		std::vector<llvm::Instruction*>	mExitInstructions;
		std::deque<KUInt32>				mPending;
		llvm::Value*                    mRegisters[15];
		llvm::Value*				    mPC;
		llvm::Value*                    mCPSR_N;
		llvm::Value*                    mCPSR_Z;
		llvm::Value*                    mCPSR_C;
		llvm::Value*                    mCPSR_V;
	};
	
	static KUInt32 CountBits( KUInt16 inWord );

	KUInt32                     mBaseVAddress;          ///< Page base virtual address
	KUInt32*                    mBasePointer;           ///< Page base pointer

	// LLVM types.
	static llvm::PointerType*  DefineTARMProcessorPtrType(void);
	static llvm::FunctionType* DefineEntryPointFuncType(void);
	static llvm::FunctionType* DefineInnerFuncType(void);
	static llvm::FunctionType* DefineReadBFuncType(void);
	static llvm::FunctionType* DefineWriteBFuncType(void);
	static llvm::FunctionType* DefineReadFuncType(void);
	static llvm::FunctionType* DefineWriteFuncType(void);
	static llvm::FunctionType* DefineSetPrivilegeFuncType(void);

	static llvm::PointerType*	gTARMProcessorPtrType;		///< TARMProcessor* type (JITFuncPtr)
	static llvm::FunctionType*	gEntryPointFuncType;		///< Type of entry point functions (JITFuncPtr)
	static llvm::FunctionType*	gInnerFuncType;				///< Inner function, like JITFuncPtr but taking the offset.
	static llvm::FunctionType*	gReadBFuncType;				///< Type of ReadB function
	static llvm::FunctionType*	gWriteBFuncType;			///< Type of WriteB function
	static llvm::FunctionType*	gReadFuncType;				///< Type of Read function
	static llvm::FunctionType*	gWriteFuncType;				///< Type of Write function
	static llvm::FunctionType*	gSetPrivilegeFuncType;		///< Type of SetPrivilege function
};

#endif

#endif
