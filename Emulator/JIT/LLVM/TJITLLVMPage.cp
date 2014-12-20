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

#include "Emulator/JIT/LLVM/TJITLLVMObjectCache.h"
#include "Emulator/JIT/LLVM/TJITLLVM.h"

// LLVM
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/DataStream.h>

// C++
#include <sstream>

using namespace llvm;

// -------------------------------------------------------------------------- //
//  * TJITLLVMPage( void )
// -------------------------------------------------------------------------- //
TJITLLVMPage::TJITLLVMPage( void )
    :
        mExecutionEngine(nullptr),
		mTranslator(*this)
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
	mEntryPointFunctions.clear();
	mStepFunctions.clear();
	mExecutionEngine = nullptr;
	mLoadedObjects.clear();
	
	TJITLLVM* jit = inMemoryIntf->GetJITObject();
	// We only cache object code for ROM.
	if (inPAddr < TMemoryConsts::kHighROMEnd) {
		TJITLLVMObjectCache* objectCache = jit->GetObjectCache();
		if (objectCache) {
			// Load saved pages directly
			mEntryPointFunctions = objectCache->LoadPageFunctions(mLoadedObjects, *this);
		}
	}
}


// -------------------------------------------------------------------------- //
//  * GetJITFuncForOffset( TMemory*, KUInt32 )
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::GetJITFuncForOffset(TMemory* inMemoryInterface, KUInt32 inOffset )
{
	auto it = mEntryPointFunctions.find(inOffset);
	if (it == mEntryPointFunctions.end()) {
		// Not found.
		// We need to perform translation of the page.
		if (mExecutionEngine == nullptr) {
			TJITLLVM* jit = inMemoryInterface->GetJITObject();
			mExecutionEngine = std::unique_ptr<ExecutionEngine>(jit->CreateExecutionEngine());
			TJITLLVMObjectCache* objectCache = jit->GetObjectCache();
			if (objectCache) {
				// Set an object cache so we are notified of newly compiled pages and we
				// can save them to disk.
				mExecutionEngine->setObjectCache(objectCache);
			}
		}
		Module* funcModule = new Module(ModuleName(inOffset), getGlobalContext());
		funcModule->setDataLayout(mExecutionEngine->getDataLayout());
		std::map<KUInt32, Function*> generated = mTranslator.TranslateEntryPoint(inOffset, funcModule);
		mExecutionEngine->addModule(funcModule);
		mExecutionEngine->finalizeObject();
		// Plug all generated functions.
		for (auto pair : generated) {
			Function* function = pair.second;
			std::string functionName = function->getName();
			JITFuncPtr ptr = (JITFuncPtr) mExecutionEngine->getFunctionAddress(functionName);
			KUInt32 offset = OffsetFromFunctionName(functionName);
			mEntryPointFunctions[offset] = ptr;
		}
		it = mEntryPointFunctions.find(inOffset);
		assert(it != mEntryPointFunctions.end());
	}
	return it->second;
}

// -------------------------------------------------------------------------- //
//  * GetJITFuncForSingleInstructionAtOffset( TMemory*, KUInt32 )
// -------------------------------------------------------------------------- //
JITFuncPtr
TJITLLVMPage::GetJITFuncForSingleInstructionAtOffset(TMemory* inMemoryInterface, KUInt32 inOffset )
{
	JITFuncPtr ptr;
	auto it = mStepFunctions.find(inOffset);
	if (it == mStepFunctions.end()) {
		if (mExecutionEngine == nullptr) {
			TJITLLVM* jit = inMemoryInterface->GetJITObject();
			mExecutionEngine = std::unique_ptr<ExecutionEngine>(jit->CreateExecutionEngine());
			TJITLLVMObjectCache* objectCache = jit->GetObjectCache();
			if (objectCache) {
				// Set an object cache so we are notified of newly compiled pages and we
				// can save them to disk.
				mExecutionEngine->setObjectCache(objectCache);
			}
		}
		Module* funcModule = new Module("i" + ModuleName(inOffset), getGlobalContext());
		funcModule->setDataLayout(mExecutionEngine->getDataLayout());
		const Function* function = mTranslator.TranslateSingleInstruction(inOffset, funcModule);
		mExecutionEngine->addModule(funcModule);
		mExecutionEngine->finalizeObject();
		ptr = (JITFuncPtr) mExecutionEngine->getFunctionAddress(function->getName());
		mStepFunctions[inOffset] = ptr;
	} else {
		ptr = it->second;
	}
	return ptr;
}

// -------------------------------------------------------------------------- //
//  * PagePrefix(TJITLLVMPage&)
// -------------------------------------------------------------------------- //
std::string
TJITLLVMPage::PagePrefix() const {
	char pagePrefix[18];
	snprintf(pagePrefix, sizeof(pagePrefix), "%.8X@%.8X", GetPAddr(), GetVAddr());
	return std::string(pagePrefix);
}

// -------------------------------------------------------------------------- //
//  * ModuleName(TJITLLVMPage&, KUInt32)
// -------------------------------------------------------------------------- //
std::string
TJITLLVMPage::ModuleName(KUInt32 entryPointOffset) const {
	char moduleName[27];
	snprintf(moduleName, sizeof(moduleName), "%.8X@%.8X_%.8X", GetPAddr(), GetVAddr(), GetVAddr() + (entryPointOffset * 4));
	return std::string(moduleName);
}

// -------------------------------------------------------------------------- //
//  * FunctionName(TJITLLVMPage&, KUInt32)
// -------------------------------------------------------------------------- //
std::string
TJITLLVMPage::FunctionName(KUInt32 offset) const {
	return ModuleName(offset);
}

// -------------------------------------------------------------------------- //
//  * OffsetFromFunctionName(TJITLLVMPage&, KUInt32)
// -------------------------------------------------------------------------- //
KUInt32
TJITLLVMPage::OffsetFromFunctionName(const std::string& inFunctionName) const {
	const char* str = inFunctionName.c_str();
	const char* functionVAddrStr = str + 18;
	KUInt32 functionVAddr;
	int r = sscanf(functionVAddrStr, "%X", &functionVAddr);
	if (r != 1) {
		fprintf(stderr, "Could not parse function name %s\n", str);
		abort();
	}
	return (functionVAddr - GetVAddr()) / 4;
}

// -------------------------------------------------------------------------- //
//  * OffsetFromFunctionName(TJITLLVMPage&, KUInt32)
// -------------------------------------------------------------------------- //
std::string
TJITLLVMPage::PagePrefixFromModuleName(const std::string& inModuleName) {
	return inModuleName.substr(0, 17);
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
