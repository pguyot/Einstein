// ==============================
// File:			TJITLLVM.cp
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

// Einstein
#include "TMemory.h"
#include "TJITLLVMPage.h"
#include "TJITLLVM.h"
#include "TARMProcessor.h"
#include "TEmulator.h"
#include "TMemoryConsts.h"
#include "Emulator/NativeCalls/TVirtualizedCallsPatches.h"
#include "Emulator/JIT/LLVM/TJITLLVMObjectCache.h"

#include "K/Threads/TThread.h"
#include "K/Threads/TCondVar.h"
#include "K/Threads/TMutex.h"

#include <llvm/ADT/Triple.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Target/TargetLibraryInfo.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetSubtargetInfo.h>

// C++
#include <set>
#include <string>
#include <sstream>

// C & POSIX
#include <dlfcn.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>

using namespace llvm;

// -------------------------------------------------------------------------- //
// Constants
// -------------------------------------------------------------------------- //

#define kCompilationThreads	8

// This is required before constructing the first TJITLLVM object.
__attribute__((constructor))
static void InitLLVM() {
	InitializeNativeTarget();
	InitializeNativeTargetAsmParser();
	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetDisassembler();
}

// -------------------------------------------------------------------------- //
//  * JIT Imported functions prototypes.
// -------------------------------------------------------------------------- //

extern "C" {
	///
	/// Function returned by generated code to re-enter generated code a the next
	/// loop iteration.
	///
	/// \param inInstruction	current instruction.
	///
	JITFuncPtr JIT_Continue(TARMProcessor* ioCPU, volatile bool* inSignal)
	__attribute__ ((visibility ("default")));
	
	///
	/// DebuggerUND exit
	///
	/// \param inInstruction	current instruction.
	///
	JITFuncPtr JIT_DebuggerUND(TARMProcessor* ioCPU, volatile bool* inSignal)
	__attribute__ ((visibility ("default")));
	
	///
	/// Breakpoint exit
	///
	/// \param inInstruction	current instruction.
	///
	JITFuncPtr JIT_Breakpoint(TARMProcessor* ioCPU, volatile bool* inSignal)
	__attribute__ ((visibility ("default")));
	
	///
	/// Undefined instruction exit
	///
	/// \param inInstruction	current instruction.
	///
	JITFuncPtr JIT_UndefinedInstruction(TARMProcessor* ioCPU, volatile bool* inSignal)
	__attribute__ ((visibility ("default")));
	
	///
	/// SWI exit
	///
	/// \param inInstruction	current instruction.
	///
	JITFuncPtr JIT_SWI(TARMProcessor* ioCPU, volatile bool* inSignal)
	__attribute__ ((visibility ("default")));
	
	///
	/// Software Breakpoint exit
	///
	/// \param inInstruction	current instruction.
	///
	JITFuncPtr JIT_SoftwareBreakpoint(TARMProcessor* ioCPU, volatile bool* inSignal)
	__attribute__ ((visibility ("default")));
	
	///
	/// System (CP15) coprocessor register transfer exit.
	///
	/// \param inInstruction	current instruction.
	///
	JITFuncPtr JIT_SystemCoprocRegisterTransfer(TARMProcessor* ioCPU, volatile bool* inSignal)
	__attribute__ ((visibility ("default")));
	
	///
	/// Native coprocessor register transfer exit.
	///
	/// \param inInstruction	current instruction.
	///
	JITFuncPtr JIT_NativeCoprocRegisterTransfer(TARMProcessor* ioCPU, volatile bool* inSignal)
	__attribute__ ((visibility ("default")));
	
	///
	/// Data abort handler exit.
	///
	/// \param inInstruction	current instruction.
	///
	JITFuncPtr JIT_DataAbort(TARMProcessor* ioCPU, volatile bool* inSignal)
	__attribute__ ((visibility ("default")));
	
	///
	/// Signal Interrupt handler exit.
	///
	/// \param inInstruction	current instruction.
	///
	JITFuncPtr JIT_SignalInterrupt(TARMProcessor* ioCPU, volatile bool* inSignal)
	__attribute__ ((visibility ("default")));
	
	///
	/// Read a byte.
	///
	bool JIT_ReadB(TARMProcessor* ioCPU, KUInt32 address, KUInt8* outByte)
	__attribute__ ((visibility ("default")));
	
	///
	/// Write a byte.
	///
	bool JIT_WriteB(TARMProcessor* ioCPU, KUInt32 address, KUInt8 inByte)
	__attribute__ ((visibility ("default")));
	
	///
	/// Read a word.
	///
	bool JIT_Read(TARMProcessor* ioCPU, KUInt32 address, KUInt32* outWord)
	__attribute__ ((visibility ("default")));
	
	///
	/// Write a word.
	///
	bool JIT_Write(TARMProcessor* ioCPU, KUInt32 address, KUInt32 inWord)
	__attribute__ ((visibility ("default")));
	
	///
	/// Set privilege memory access.
	///
	void JIT_SetPrivilege(TARMProcessor* ioCPU, bool privilege)
	__attribute__ ((visibility ("default")));
	
}

// -------------------------------------------------------------------------- //
//  * TJITLLVM( void )
// -------------------------------------------------------------------------- //
TJITLLVM::TJITLLVM(
		TMemory* inMemoryIntf,
		TMMU* inMMUIntf,
		const TROMImage* inROMImage)
	:
		TJIT<TJITLLVM, TJITLLVMPage>( inMemoryIntf, inMMUIntf ),
		mGluesTable(CreateGluesTable())
{
	if (inROMImage) {
		mObjectCache = new TJITLLVMObjectCache(*inROMImage, new TJITLLVMRecordingMemoryManager(mGluesTable));
	} else {
		mObjectCache = nullptr;
	}
	std::string err;
	mTarget = TargetRegistry::lookupTarget(LLVM_HOST_TRIPLE, err);
	if (mTarget == nullptr) {
		fprintf(stderr, "Cannot find LLVM target %s\n", err.c_str());
		abort();
	}
	
	// Finally init cache.
	InitCache();
}

// -------------------------------------------------------------------------- //
//  * ~TJITLLVM( void )
// -------------------------------------------------------------------------- //
TJITLLVM::~TJITLLVM( void )
{
	if (mObjectCache) {
		delete mObjectCache;
	}
}

// -------------------------------------------------------------------------- //
//  * Run( TARMProcessor*, volatile KUInt32* )
// -------------------------------------------------------------------------- //
void
TJITLLVM::Run( TARMProcessor* ioCPU, volatile bool* inSignal )
{
	KUInt32* pcPtr = &ioCPU->mCurrentRegisters[TARMProcessor::kR15];
	TMemory* theMemoryInterface = ioCPU->mMemory;
	TEmulator* theEmulator = ioCPU->mEmulator;
	JITFuncPtr theJITFunc = GetJITFuncForPC( ioCPU, theMemoryInterface, *pcPtr );
	while (true)
	{
		while (*inSignal)
		{
			// Here we go, iterating...
			theJITFunc = theJITFunc(ioCPU, inSignal);
		}
		
		// We may have been signaled because there was an interrupt.
		if (theEmulator->IsInterrupted())
		{
			KUInt32 theInterrupt = ioCPU->mPendingInterrupts;
			theEmulator->AckInterrupt();
			if (theInterrupt & TARMProcessor::kResetInterrupt)
			{
				ioCPU->Reset();
				// Unmaskable.
				theJITFunc = GetJITFuncForPC( ioCPU, theMemoryInterface, *pcPtr );
			} else if ((theInterrupt & TARMProcessor::kFIQInterrupt) && !ioCPU->mCPSR_F) {
				ioCPU->FIQ();
				theJITFunc = GetJITFuncForPC( ioCPU, theMemoryInterface, *pcPtr );
			} else if ((theInterrupt & TARMProcessor::kIRQInterrupt) && !ioCPU->mCPSR_I) {
				ioCPU->IRQ();
				theJITFunc = GetJITFuncForPC( ioCPU, theMemoryInterface, *pcPtr );
			}
		} else {
			break;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * Step( TARMProcessor*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TJITLLVM::Step( TARMProcessor* ioCPU, KUInt32 count )
{
	KUInt32* pendingInterrupts = &ioCPU->mPendingInterrupts;
	KUInt32* pcPtr = &ioCPU->mCurrentRegisters[TARMProcessor::kR15];
	TMemory* theMemoryInterface = ioCPU->mMemory;
	while (count-- > 0)
	{
		bool signal = 0;
    	JITFuncPtr theJITFunc = GetJITFuncForSingleInstructionAtPC(ioCPU, theMemoryInterface, *pcPtr);
    	(void) theJITFunc(ioCPU, &signal);

		// Maybe there is an interrupt...
		KUInt32 theInterrupt = *pendingInterrupts;
		if (theInterrupt)
		{
			if (theInterrupt & TARMProcessor::kResetInterrupt)
			{
				ioCPU->Reset();
				// Unmaskable.
			} else if ((theInterrupt & TARMProcessor::kFIQInterrupt) && !ioCPU->mCPSR_F) {
				ioCPU->FIQ();
			} else if ((theInterrupt & TARMProcessor::kIRQInterrupt) && !ioCPU->mCPSR_I) {
				ioCPU->IRQ();
			}
		}
	} // while
}

// -------------------------------------------------------------------------- //
//  * GetJITFuncForPC( TARMProcessor*, TMemory*, KUInt32 )
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVM::GetJITFuncForPC(
					TARMProcessor* ioCPU,
					TMemory* inMemoryInterface,
					KUInt32 inPC )
{
	// Get the page from the cache.
	KUInt32 pc = inPC - 4;
	TJITLLVMPage* thePage = GetPage(pc);

	if (thePage == NULL)	
	{
		// Let's manage the exception
		ioCPU->PrefetchAbort();
		
		// Redo the translation.
		pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15];
		return GetJITFuncForPC( ioCPU, inMemoryInterface, pc );
	}

	KUInt32 indexInPage = GetOffsetInPage(pc) / sizeof( KUInt32 );

	// Return the function.
	return thePage->GetJITFuncForOffset(inMemoryInterface, indexInPage);
}

// -------------------------------------------------------------------------- //
//  * GetJITFuncForSingleInstructionAtPC( TARMProcessor*, TMemory*, KUInt32 )
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVM::GetJITFuncForSingleInstructionAtPC(
					TARMProcessor* ioCPU,
					TMemory* inMemoryInterface,
					KUInt32 inPC )
{
	// Get the page from the cache.
	KUInt32 pc = inPC - 4;
	TJITLLVMPage* thePage = GetPage(pc);

	if (thePage == NULL)
	{
		// Let's manage the exception
		ioCPU->PrefetchAbort();
		
		// Redo the translation.
		pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15];
		return GetJITFuncForPC( ioCPU, inMemoryInterface, pc );
	}

	KUInt32 indexInPage = GetOffsetInPage(pc) / sizeof( KUInt32 );

	// Return the function.
	return thePage->GetJITFuncForSingleInstructionAtOffset(inMemoryInterface, indexInPage);
}

// -------------------------------------------------------------------------- //
//  * DoPatchROM(KUInt32* romPtr, const std::string& inMachineName)
// -------------------------------------------------------------------------- //
void
TJITLLVM::DoPatchROM(KUInt32* romPtr, const std::string& inMachineName) {
	TVirtualizedCallsPatches::DoPatchROM(romPtr, inMachineName);
}

// -------------------------------------------------------------------------- //
//  * CreateExecutionEngine()
// -------------------------------------------------------------------------- //
ExecutionEngine*
TJITLLVM::CreateExecutionEngine() {
	TargetOptions options;
	TargetMachine* targetMachine = mTarget->createTargetMachine(LLVM_HOST_TRIPLE, "", "", options);
	std::string engineBuilderError;
	// We do this with a dummy module.
	Module* emptyModule = new Module("", getGlobalContext());
	ExecutionEngine* theResult = EngineBuilder(emptyModule)
										.setEngineKind(EngineKind::JIT)
										.setUseMCJIT(true)
										.setErrorStr(&engineBuilderError)
										.setMCJITMemoryManager(new TJITLLVMGlueMemoryManager(mGluesTable))
										.create(targetMachine);
	if (theResult == nullptr) {
		fprintf(stderr, "Could not create MCJIT execution engine: %s\n", engineBuilderError.c_str());
		abort();
	}
	theResult->DisableSymbolSearching();
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * CreateRuntimeDyld()
// -------------------------------------------------------------------------- //
RuntimeDyld*
TJITLLVM::CreateRuntimeDyld() {
	return new RuntimeDyld(new TJITLLVMGlueMemoryManager(mGluesTable));
}

// -------------------------------------------------------------------------- //
//  * CreateGluesTable()
// -------------------------------------------------------------------------- //
std::map<std::string, uint64_t>
TJITLLVM::CreateGluesTable() {
	std::map<std::string, uint64_t> result;
	result["Continue"] = (uint64_t)(void*)JIT_Continue;
	result["DebuggerUND"] = (uint64_t)(void*)JIT_DebuggerUND;
	result["Breakpoint"] = (uint64_t)(void*)JIT_Breakpoint;
	result["UndefinedInstruction"] = (uint64_t)(void*)JIT_UndefinedInstruction;
	result["SoftwareBreakpoint"] = (uint64_t)(void*)JIT_SoftwareBreakpoint;
	result["SystemCoprocRegisterTransfer"] = (uint64_t)(void*)JIT_SystemCoprocRegisterTransfer;
	result["NativeCoprocRegisterTransfer"] = (uint64_t)(void*)JIT_NativeCoprocRegisterTransfer;
	result["DataAbort"] = (uint64_t)(void*)JIT_DataAbort;
	result["SWI"] = (uint64_t)(void*)JIT_SWI;
	result["SignalInterrupt"] = (uint64_t)(void*)JIT_SignalInterrupt;
	result["ReadB"] = (uint64_t)(void*)JIT_ReadB;
	result["WriteB"] = (uint64_t)(void*)JIT_WriteB;
	result["Read"] = (uint64_t)(void*)JIT_Read;
	result["Write"] = (uint64_t)(void*)JIT_Write;
	result["SetPrivilege"] = (uint64_t)(void*)JIT_SetPrivilege;

	return result;
}

// -------------------------------------------------------------------------- //
//  * getSymbolAddress(const std::string& name)
// -------------------------------------------------------------------------- //
uint64_t
TJITLLVMGlueMemoryManager::getSymbolAddress(const std::string& name) {
	uint64_t result;
	ssize_t pos;
	if (name[0] == '_') {
		pos = 5;
	} else {
		pos = 4;
	}
	auto it = mGluesTable.find(name.substr(pos));
	if (it == mGluesTable.end()) {
		result = 0;
	} else {
		result = it->second;
	}
	return result;
}

// -------------------------------------------------------------------------- //
//  * allocateCodeSection(uintptr_t, unsigned, unsigned, llvm::StringRef)
// -------------------------------------------------------------------------- //
uint8_t*
TJITLLVMRecordingMemoryManager::allocateCodeSection(uintptr_t size, unsigned alignment, unsigned sectionID, llvm::StringRef sectionName) {
	if (sectionID != mLatestAllocatedSectionID) {
		mLatestAllocatedSectionID = sectionID;
		mAllocatedSectionsCount++;
	}
	uint8_t* result = TJITLLVMGlueMemoryManager::allocateCodeSection(size, alignment, sectionID, sectionName);
	mLatestAllocatedAddress = result;
	return result;
}

// -------------------------------------------------------------------------- //
//  * GetLatestSectionLoadAddress()
// -------------------------------------------------------------------------- //
uint8_t*
TJITLLVMRecordingMemoryManager::GetLatestSectionLoadAddress() {
	if (mAllocatedSectionsCount != 1) {
		fprintf(stderr, "Could not get section address as we have %i sections\n", mAllocatedSectionsCount);
		abort();
	}
	return mLatestAllocatedAddress;
}

// -------------------------------------------------------------------------- //
//  * Continue(TARMProcessor*, volatile KUInt32*)
// -------------------------------------------------------------------------- //
// This function continues execution by loading the next function and executing
// it. It is used by the functions here for lazy translation. Indeed,
// GetJITFuncForPC will actually translate emulated code to native, yet PC might
// change after these functions return.
//
JITFuncPtr
JIT_Continue(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	JITFuncPtr f = theMemIntf->GetJITObject()->GetJITFuncForPC(ioCPU, theMemIntf, ioCPU->mCurrentRegisters[TARMProcessor::kR15]);
	return f(ioCPU, inSignal);
}

// -------------------------------------------------------------------------- //
//  * DebuggerUND(TARMProcessor*, volatile bool*)
// -------------------------------------------------------------------------- //
JITFuncPtr
JIT_DebuggerUND(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15] - 8;
	KUInt32 thePAddress;
	theMemIntf->TranslateInstruction(pc, &thePAddress);
	ioCPU->GetEmulator()->DebuggerUND(thePAddress);
	ioCPU->DoUndefinedInstruction();
	return (JITFuncPtr) JIT_Continue;
}

// -------------------------------------------------------------------------- //
//  * Breakpoint(TARMProcessor*, volatile bool*)
// -------------------------------------------------------------------------- //
JITFuncPtr
JIT_Breakpoint(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15] - 8;
	KUInt32 theInstruction;
	theMemIntf->Read(pc, theInstruction);
	KUInt16 theBreakpointID = (KUInt16) ((theInstruction & 0xFFF00) >> 4 | (theInstruction & 0xF));
	ioCPU->GetEmulator()->Breakpoint(theBreakpointID);
	return (JITFuncPtr) JIT_Continue;
}

// -------------------------------------------------------------------------- //
//  * UndefinedInstruction(TARMProcessor*, volatile bool*)
// -------------------------------------------------------------------------- //
JITFuncPtr
JIT_UndefinedInstruction(TARMProcessor* ioCPU, volatile bool* inSignal) {
	ioCPU->DoUndefinedInstruction();
	return (JITFuncPtr) JIT_Continue;
}

// -------------------------------------------------------------------------- //
//  * SoftwareBreakpoint(TARMProcessor*, volatile bool*)
// -------------------------------------------------------------------------- //
JITFuncPtr
JIT_SoftwareBreakpoint(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15] - 8;
	KUInt32 theInstruction;
	theMemIntf->Read(pc, theInstruction);
	ioCPU->GetEmulator()->Breakpoint( theInstruction );
	return (JITFuncPtr) JIT_Continue;
}

// -------------------------------------------------------------------------- //
//  * SystemCoprocRegisterTransfer(TARMProcessor*, volatile bool*)
// -------------------------------------------------------------------------- //
JITFuncPtr
JIT_SystemCoprocRegisterTransfer(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15] - 8;
	KUInt32 theInstruction;
	theMemIntf->Read(pc, theInstruction);
	ioCPU->SystemCoprocRegisterTransfer(theInstruction);
	return (JITFuncPtr) JIT_Continue;
}

// -------------------------------------------------------------------------- //
//  * NativeCoprocRegisterTransfer(TARMProcessor*, volatile KUInt32*)
// -------------------------------------------------------------------------- //
JITFuncPtr
JIT_NativeCoprocRegisterTransfer(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15] - 8;
	KUInt32 theInstruction;
	theMemIntf->Read(pc, theInstruction);
	ioCPU->NativeCoprocRegisterTransfer(theInstruction);
	return (JITFuncPtr) JIT_Continue;
}

// -------------------------------------------------------------------------- //
//  * DataAbort(TARMProcessor*, volatile KUInt32*)
// -------------------------------------------------------------------------- //
JITFuncPtr
JIT_DataAbort(TARMProcessor* ioCPU, volatile bool* inSignal) {
	ioCPU->DataAbort();
	return (JITFuncPtr) JIT_Continue;
}

// -------------------------------------------------------------------------- //
//  * SWI(TARMProcessor*, volatile KUInt32*)
// -------------------------------------------------------------------------- //
JITFuncPtr
JIT_SWI(TARMProcessor* ioCPU, volatile bool* inSignal) {
	ioCPU->DoSWI();
	return (JITFuncPtr) JIT_Continue;
}

// -------------------------------------------------------------------------- //
//  * SignalInterrupt(TARMProcessor*, volatile KUInt32*)
// -------------------------------------------------------------------------- //
JITFuncPtr
JIT_SignalInterrupt(TARMProcessor* ioCPU, volatile bool* inSignal) {
	ioCPU->GetEmulator()->SignalInterrupt();
	return (JITFuncPtr) JIT_Continue;
}

// -------------------------------------------------------------------------- //
//  * ReadB(TARMProcessor*, KUInt32, KUInt8*)
// -------------------------------------------------------------------------- //
bool
JIT_ReadB(TARMProcessor* ioCPU, KUInt32 address, KUInt8* outByte) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	return theMemIntf->ReadB(address, *outByte);
}

// -------------------------------------------------------------------------- //
//  * WriteB(TARMProcessor*, KUInt32, KUInt8)
// -------------------------------------------------------------------------- //
bool
JIT_WriteB(TARMProcessor* ioCPU, KUInt32 address, KUInt8 inByte) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	return theMemIntf->WriteB(address, inByte);
}

// -------------------------------------------------------------------------- //
//  * Read(TARMProcessor*, KUInt32, KUInt32*)
// -------------------------------------------------------------------------- //
bool
JIT_Read(TARMProcessor* ioCPU, KUInt32 address, KUInt32* outWord) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	return theMemIntf->Read(address, *outWord);
}

// -------------------------------------------------------------------------- //
//  * Write(TARMProcessor*, KUInt32, KUInt32)
// -------------------------------------------------------------------------- //
bool
JIT_Write(TARMProcessor* ioCPU, KUInt32 address, KUInt32 inWord) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	return theMemIntf->Write(address, inWord);
}

// -------------------------------------------------------------------------- //
//  * SetPrivilege(TARMProcessor*, bool)
// -------------------------------------------------------------------------- //
void
JIT_SetPrivilege(TARMProcessor* ioCPU, bool privilege) {
	if (ioCPU->GetMode() != TARMProcessor::kUserMode) {
		TMemory* theMemIntf = ioCPU->GetMemory();
		theMemIntf->SetPrivilege( privilege );
	}
}

#endif

// ============================================================================ //
// FORTRAN, "the infantile disorder", by now nearly 20 years old, is hopelessly //
// inadequate for whatever computer application you have in mind today: it is   //
// too clumsy, too risky, and too expensive to use.                             //
//                 -- Edsger W. Dijkstra, SIGPLAN Notices, Volume 17, Number 5  //
// ============================================================================ //
