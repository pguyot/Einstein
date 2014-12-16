// ==============================
// File:			TJITLLVMPage.cp
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
#include "TJITLLVMPage.h"
#include "TARMProcessor.h"
#include "TEmulator.h"

#include "UDisasm.h"

// LLVM
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>

// C++
#include <sstream>

using namespace llvm;

// -------------------------------------------------------------------------- //
//  * TJITLLVMPage( void )
// -------------------------------------------------------------------------- //
TJITLLVMPage::TJITLLVMPage( void )
    :
        mExecutionEngine(nullptr)
{
}

// -------------------------------------------------------------------------- //
//  * ~TJITLLVMPage( void )
// -------------------------------------------------------------------------- //
TJITLLVMPage::~TJITLLVMPage( void ) {
}

// -------------------------------------------------------------------------- //
//  * Init( KUInt32*, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TJITLLVMPage::Init(
			TMemory* inMemoryIntf,
			KUInt32 inVAddr,
			KUInt32 inPAddr )
{
	TJITPage<TJITLLVM, TJITLLVMPage>::Init( inMemoryIntf, inVAddr, inPAddr );
    mTranslator.Init(inVAddr, GetPointer());
	mExecutionEngine = nullptr;
#if !LLVM_USE_MCJIT
	mSingleModule = nullptr;
#endif
	mEntryPointFunctions.clear();
}


// -------------------------------------------------------------------------- //
//  * GetJITFuncForOffset( TMemory*, KUInt32 )
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::GetJITFuncForOffset(TMemory* inMemoryInterface, KUInt32 inOffset )
{
	auto it = mEntryPointFunctions.find(inOffset);
	if (it == mEntryPointFunctions.end()) {
		Module* funcModule = GetFunctionModule(inOffset);
		mTranslator.TranslateEntryPoint(inOffset, kInstructionCount, funcModule, mEntryPointFunctions);
#if LLVM_USE_MCJIT
		mExecutionEngine->finalizeObject();
#endif
		it = mEntryPointFunctions.find(inOffset);
	}
	auto funcPair = it->second;
	if (funcPair.second == nullptr) {
		funcPair.second = (JITFuncPtr) mExecutionEngine->getPointerToFunction(funcPair.first);
	}
	return funcPair.second;
}

// -------------------------------------------------------------------------- //
//  * GetJITFuncForSingleInstructionAtOffset( TMemory*, KUInt32 )
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::GetJITFuncForSingleInstructionAtOffset(TMemory* inMemoryInterface, KUInt32 inOffset )
{
	Function* function;
	auto it = mStepFunctions.find(inOffset);
	if (it == mStepFunctions.end()) {
		Module* funcModule = GetFunctionModule(inOffset);
		function = mTranslator.TranslateSingleInstruction(inOffset, funcModule);
#if LLVM_USE_MCJIT
		mExecutionEngine->finalizeObject();
#endif
		mStepFunctions[inOffset] = function;
	}
	return (JITFuncPtr) mExecutionEngine->getPointerToFunction(function);
}

// -------------------------------------------------------------------------- //
//  * GetFunctionModule( std::string& )
// -------------------------------------------------------------------------- //
Module*
TJITLLVMPage::GetFunctionModule(KUInt32 inOffset)
{
#if LLVM_USE_MCJIT
	std::stringstream stream;
	stream << "m" << (GetVAddr() + (inOffset * 4));
	std::string moduleName(stream.str());
	Module* funcModule = new Module(moduleName, getGlobalContext());
	if (mExecutionEngine == nullptr) {
		std::string engineBuilderError;
		mExecutionEngine = std::unique_ptr<ExecutionEngine>(EngineBuilder(funcModule)
													.setEngineKind(EngineKind::JIT)
													.setUseMCJIT(true)
													.setErrorStr(&engineBuilderError)
													.setMCJITMemoryManager(new MemoryManager())
													.create());
		if (mExecutionEngine.get() == nullptr) {
			fprintf(stderr, "Could not create MCJIT execution engine: %s\n", engineBuilderError.c_str());
			throw engineBuilderError;
		}
		assert(mExecutionEngine.get() != nullptr);
		mExecutionEngine->DisableSymbolSearching();
		mExecutionEngine->InstallLazyFunctionCreator(LazyFunctionCreator);
	} else {
		mExecutionEngine->addModule(funcModule);
	}
	funcModule->setDataLayout(mExecutionEngine->getDataLayout());
	return funcModule;
#else
	if (mSingleModule == nullptr) {
		mSingleModule = new Module("", getGlobalContext());
		std::string engineBuilderError;
		mExecutionEngine = std::unique_ptr<ExecutionEngine>(EngineBuilder(mSingleModule)
															.setErrorStr(&engineBuilderError)
															.create());
		if (mExecutionEngine.get() == nullptr) {
			fprintf(stderr, "Could not create execution engine: %s\n", engineBuilderError.c_str());
			throw engineBuilderError;
		}
		assert(mExecutionEngine.get() != nullptr);
		mExecutionEngine->DisableSymbolSearching();
		mExecutionEngine->InstallLazyFunctionCreator(LazyFunctionCreator);
	}
	return mSingleModule;
#endif
}

// -------------------------------------------------------------------------- //
//  * LazyFunctionCreator(const std::string&)
// -------------------------------------------------------------------------- //
// Translate glue functions.
//
void*
TJITLLVMPage::LazyFunctionCreator(const std::string& name) {
	if (name == "Continue") {
		return (void*)Continue;
	} else if (name == "DebuggerUND") {
		return (void*)DebuggerUND;
	} else if (name == "Breakpoint") {
		return (void*)Breakpoint;
	} else if (name == "UndefinedInstruction") {
		return (void*)UndefinedInstruction;
	} else if (name == "SoftwareBreakpoint") {
		return (void*)SoftwareBreakpoint;
	} else if (name == "SystemCoprocRegisterTransfer") {
		return (void*)SystemCoprocRegisterTransfer;
	} else if (name == "NativeCoprocRegisterTransfer") {
		return (void*)NativeCoprocRegisterTransfer;
	} else if (name == "DataAbort") {
		return (void*)DataAbort;
	} else if (name == "SWI") {
		return (void*)SWI;
	} else if (name == "SignalInterrupt") {
		return (void*)SignalInterrupt;
	} else if (name == "ReadB") {
		return (void*)ReadB;
	} else if (name == "WriteB") {
		return (void*)WriteB;
	} else if (name == "Read") {
		return (void*)Read;
	} else if (name == "Write") {
		return (void*)Write;
	} else if (name == "SetPrivilege") {
		return (void*)SetPrivilege;
	} else {
		return nullptr;
	}
}

// -------------------------------------------------------------------------- //
//  * getSymbolAddress(const std::string& name)
// -------------------------------------------------------------------------- //
#if LLVM_USE_MCJIT
uint64_t
TJITLLVMPage::MemoryManager::getSymbolAddress(const std::string& name) {
	void* funcPtr;
	if (name[0] == '_') {
		funcPtr = TJITLLVMPage::LazyFunctionCreator(name.substr(1));
	} else {
		funcPtr = TJITLLVMPage::LazyFunctionCreator(name);
	}
	return (uint64_t) funcPtr;
}
#endif

// -------------------------------------------------------------------------- //
//  * Continue(TARMProcessor*, volatile KUInt32*)
// -------------------------------------------------------------------------- //
// This function continues execution by loading the next function and executing
// it. It is used by the functions here for lazy translation. Indeed,
// GetJITFuncForPC will actually translate emulated code to native, yet PC might
// change after these functions return.
//
JITFuncPtr
TJITLLVMPage::Continue(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	JITFuncPtr f = theMemIntf->GetJITObject()->GetJITFuncForPC(ioCPU, theMemIntf, ioCPU->mCurrentRegisters[TARMProcessor::kR15]);
	return f(ioCPU, inSignal);
}

// -------------------------------------------------------------------------- //
//  * DebuggerUND(TARMProcessor*, volatile bool*)
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::DebuggerUND(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15] - 8;
	KUInt32 thePAddress;
	theMemIntf->TranslateInstruction(pc, &thePAddress);
	ioCPU->GetEmulator()->DebuggerUND(thePAddress);
	ioCPU->DoUndefinedInstruction();
	return (JITFuncPtr) Continue;
}

// -------------------------------------------------------------------------- //
//  * Breakpoint(TARMProcessor*, volatile bool*)
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::Breakpoint(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15] - 8;
	KUInt32 theInstruction;
	theMemIntf->Read(pc, theInstruction);
	KUInt16 theBreakpointID = (KUInt16) ((theInstruction & 0xFFF00) >> 4 | (theInstruction & 0xF));
	ioCPU->GetEmulator()->Breakpoint(theBreakpointID);
	return (JITFuncPtr) Continue;
}

// -------------------------------------------------------------------------- //
//  * UndefinedInstruction(TARMProcessor*, volatile bool*)
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::UndefinedInstruction(TARMProcessor* ioCPU, volatile bool* inSignal) {
	ioCPU->DoUndefinedInstruction();
	return (JITFuncPtr) Continue;
}

// -------------------------------------------------------------------------- //
//  * SoftwareBreakpoint(TARMProcessor*, volatile bool*)
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::SoftwareBreakpoint(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15] - 8;
	KUInt32 theInstruction;
	theMemIntf->Read(pc, theInstruction);
	ioCPU->GetEmulator()->Breakpoint( theInstruction );
	return (JITFuncPtr) Continue;
}

// -------------------------------------------------------------------------- //
//  * SystemCoprocRegisterTransfer(TARMProcessor*, volatile bool*)
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::SystemCoprocRegisterTransfer(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15] - 8;
	KUInt32 theInstruction;
	theMemIntf->Read(pc, theInstruction);
	ioCPU->SystemCoprocRegisterTransfer(theInstruction);
	return (JITFuncPtr) Continue;
}

// -------------------------------------------------------------------------- //
//  * NativeCoprocRegisterTransfer(TARMProcessor*, volatile KUInt32*)
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::NativeCoprocRegisterTransfer(TARMProcessor* ioCPU, volatile bool* inSignal) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	KUInt32 pc = ioCPU->mCurrentRegisters[TARMProcessor::kR15] - 8;
	KUInt32 theInstruction;
	theMemIntf->Read(pc, theInstruction);
	ioCPU->NativeCoprocRegisterTransfer(theInstruction);
	return (JITFuncPtr) Continue;
}

// -------------------------------------------------------------------------- //
//  * DataAbort(TARMProcessor*, volatile KUInt32*)
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::DataAbort(TARMProcessor* ioCPU, volatile bool* inSignal) {
	ioCPU->DataAbort();
	return (JITFuncPtr) Continue;
}

// -------------------------------------------------------------------------- //
//  * SWI(TARMProcessor*, volatile KUInt32*)
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::SWI(TARMProcessor* ioCPU, volatile bool* inSignal) {
	ioCPU->DoSWI();
	return (JITFuncPtr) Continue;
}

// -------------------------------------------------------------------------- //
//  * SignalInterrupt(TARMProcessor*, volatile KUInt32*)
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::SignalInterrupt(TARMProcessor* ioCPU, volatile bool* inSignal) {
	ioCPU->GetEmulator()->SignalInterrupt();
	return (JITFuncPtr) Continue;
}

// -------------------------------------------------------------------------- //
//  * ReadB(TARMProcessor*, KUInt32, KUInt8*)
// -------------------------------------------------------------------------- //
bool
TJITLLVMPage::ReadB(TARMProcessor* ioCPU, KUInt32 address, KUInt8* outByte) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	return theMemIntf->ReadB(address, *outByte);
}

// -------------------------------------------------------------------------- //
//  * WriteB(TARMProcessor*, KUInt32, KUInt8)
// -------------------------------------------------------------------------- //
bool
TJITLLVMPage::WriteB(TARMProcessor* ioCPU, KUInt32 address, KUInt8 inByte) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	return theMemIntf->WriteB(address, inByte);
}

// -------------------------------------------------------------------------- //
//  * Read(TARMProcessor*, KUInt32, KUInt32*)
// -------------------------------------------------------------------------- //
bool
TJITLLVMPage::Read(TARMProcessor* ioCPU, KUInt32 address, KUInt32* outWord) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	return theMemIntf->Read(address, *outWord);
}

// -------------------------------------------------------------------------- //
//  * Write(TARMProcessor*, KUInt32, KUInt32)
// -------------------------------------------------------------------------- //
bool
TJITLLVMPage::Write(TARMProcessor* ioCPU, KUInt32 address, KUInt32 inWord) {
	TMemory* theMemIntf = ioCPU->GetMemory();
	return theMemIntf->Write(address, inWord);
}

// -------------------------------------------------------------------------- //
//  * SetPrivilege(TARMProcessor*, bool)
// -------------------------------------------------------------------------- //
void
TJITLLVMPage::SetPrivilege(TARMProcessor* ioCPU, bool privilege) {
	if (ioCPU->GetMode() != TARMProcessor::kUserMode) {
		TMemory* theMemIntf = ioCPU->GetMemory();
		theMemIntf->SetPrivilege( privilege );
	}
}

#endif

// ============================================================================= //
// Ever wondered about the origins of the term "bugs" as applied to computer     //
// technology?  U.S. Navy Capt. Grace Murray Hopper has firsthand explanation.   //
// The 74-year-old captain, who is still on active duty, was a pioneer in        //
// computer technology during World War II.  At the C.W. Post Center of Long     //
// Island University, Hopper told a group of Long Island public school adminis-  //
// trators that the first computer "bug" was a real bug--a moth.  At Harvard     //
// one August night in 1945, Hopper and her associates were working on the       //
// "granddaddy" of modern computers, the Mark I.  "Things were going badly;      //
// there was something wrong in one of the circuits of the long glass-enclosed   //
// computer," she said.  "Finally, someone located the trouble spot and, using   //
// ordinary tweezers, removed the problem, a two-inch moth.  From then on, when  //
// anything went wrong with a computer, we said it had bugs in it."  Hopper      //
// said that when the veracity of her story was questioned recently, "I referred //
// them to my 1945 log book, now in the collection of the Naval Surface Weapons  //
// Center, and they found the remains of that moth taped to the page in          //
// question."                                                                    //
//                 [actually, the term "bug" had even earlier usage in           //
//                 regard to problems with radio hardware.  Ed.]                 //
// ============================================================================= //
