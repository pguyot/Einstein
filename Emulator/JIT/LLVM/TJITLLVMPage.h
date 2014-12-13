// ==============================
// File:			TJITLLVMPage.h
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

#ifndef _TJITLLVMPAGE_H
#define _TJITLLVMPAGE_H

#include "TJITPage.h"
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include "JITLLVM.h"
#include "TJITLLVMTranslator.h"

// MCJIT is the modern JIT engine.
// However, it seems quite slower now…
#define LLVM_USE_MCJIT 0

///
/// Class for a JIT page for LLVM-based emulation.
///
class TJITLLVMPage
	:
		public TJITPage< TJITLLVMPage >
{

public:
	///
	/// Access from TJITLLVM
	///
	friend class TJITLLVM;
	
	///
	/// Default constructor.
	///
	TJITLLVMPage( void );

	///
	/// Destructor.
	///
	~TJITLLVMPage( void );

	///
	/// Init with the memory interface, a virtual address and a physical address.
	/// Pages are re-used and this method resets the execution engine.
	///
	void Init(
			TMemory* inMemoryIntf,
			KUInt32 inVAddr,
			KUInt32 inPAddr );

	///
	/// Get the function for a given offset.
	///
	JITFuncPtr GetJITFuncForOffset(TMemory* inMemoryIntf, KUInt32 inPC);

	///
	/// Get the function for a single instruction at given offset.
	/// This is used with Step.
	///
	JITFuncPtr GetJITFuncForSingleInstructionAtOffset(TMemory* inMemoryIntf, KUInt32 inPC);

private:
	///
	/// Prevent copies.
	///
	TJITLLVMPage( const TJITLLVMPage& );
	
	///
	/// Function returned by generated code to re-enter generated code a the next
	/// loop iteration.
	///
	/// \param inInstruction	current instruction.
	///
	static JITFuncPtr Continue(TARMProcessor* ioCPU, volatile bool* inSignal);
	
	///
	/// DebuggerUND exit
	///
	/// \param inInstruction	current instruction.
	///
	static JITFuncPtr DebuggerUND(TARMProcessor* ioCPU, volatile bool* inSignal);
	
	///
	/// Breakpoint exit
	///
	/// \param inInstruction	current instruction.
	///
	static JITFuncPtr Breakpoint(TARMProcessor* ioCPU, volatile bool* inSignal);
	
	///
	/// Undefined instruction exit
	///
	/// \param inInstruction	current instruction.
	///
	static JITFuncPtr UndefinedInstruction(TARMProcessor* ioCPU, volatile bool* inSignal);
	
	///
	/// SWI exit
	///
	/// \param inInstruction	current instruction.
	///
	static JITFuncPtr SWI(TARMProcessor* ioCPU, volatile bool* inSignal);
	
	///
	/// Software Breakpoint exit
	///
	/// \param inInstruction	current instruction.
	///
	static JITFuncPtr SoftwareBreakpoint(TARMProcessor* ioCPU, volatile bool* inSignal);
	
	///
	/// System (CP15) coprocessor register transfer exit.
	///
	/// \param inInstruction	current instruction.
	///
	static JITFuncPtr SystemCoprocRegisterTransfer(TARMProcessor* ioCPU, volatile bool* inSignal);
	
	///
	/// Native coprocessor register transfer exit.
	///
	/// \param inInstruction	current instruction.
	///
	static JITFuncPtr NativeCoprocRegisterTransfer(TARMProcessor* ioCPU, volatile bool* inSignal);
	
	///
	/// Data abort handler exit.
	///
	/// \param inInstruction	current instruction.
	///
	static JITFuncPtr DataAbort(TARMProcessor* ioCPU, volatile bool* inSignal);

	///
	/// Signal Interrupt handler exit.
	///
	/// \param inInstruction	current instruction.
	///
	static JITFuncPtr SignalInterrupt(TARMProcessor* ioCPU, volatile bool* inSignal);

	///
	/// Read a byte.
	///
	static bool ReadB(TARMProcessor* ioCPU, KUInt32 address, KUInt8* outByte);
	
	///
	/// Write a byte.
	///
	static bool WriteB(TARMProcessor* ioCPU, KUInt32 address, KUInt8 inByte);
	
	///
	/// Read a word.
	///
	static bool Read(TARMProcessor* ioCPU, KUInt32 address, KUInt32* outWord);
	
	///
	/// Write a word.
	///
	static bool Write(TARMProcessor* ioCPU, KUInt32 address, KUInt32 inWord);
	
	///
	/// Set privilege memory access.
	///
	static void SetPrivilege(TARMProcessor* ioCPU, bool privilege);
	
	/// \name Constants
	enum {
		kInstructionCount = (TJITPage< TJITLLVMPage >::kPageSize / 4),
	};
	
	///
	/// For traditional JIT: get the single module.
	/// For MCJIT, create a new module.
	///
	/// In both cases, the execution engine is created if required.
	/// (modules are owned by execution engines).
	///
	llvm::Module* GetFunctionModule(const std::string& inName );

#if LLVM_USE_MCJIT
	///
	/// We need our own memory manager with MCJIT
	/// as InstallLazyFunctionCreator does not work.
	///
	class MemoryManager : public llvm::SectionMemoryManager {
		/// This method returns the address of the specified function or variable.
		/// It is used to resolve symbols during module linking.
		uint64_t getSymbolAddress(const std::string &Name) override;
	};
#endif
	
	static void* LazyFunctionCreator(const std::string& inName);
	
	/// \name Variables
	std::unique_ptr<llvm::ExecutionEngine> mExecutionEngine;	///< Execution engine for the page.
#if !LLVM_USE_MCJIT
	llvm::Module*						mSingleModule;			///< With JIT, we have a single module.
#endif
	std::map<std::string, JITFuncPtr>	mFunctions;				///< Available functions.
	TJITLLVMTranslator                  mTranslator;
};

#endif
		// _TJITLLVMPAGE_H

#endif

// ======================================== //
// Ask not for whom the <CONTROL-G> tolls. //
// ======================================== //
