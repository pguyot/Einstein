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
#include <llvm/ExecutionEngine/ObjectCache.h>

#include "JITLLVM.h"
#include "TJITLLVMTranslator.h"

class TJITLLVM;

///
/// Class for a JIT page for LLVM-based emulation.
///
class TJITLLVMPage
	:
		public TJITPage< TJITLLVM, TJITLLVMPage >
{

public:
	///
	/// Access from TJITLLVM & TJITLLVMTranslator.
	///
	friend class TJITLLVM;
	
	///
	/// Default constructor used by cache.
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
			KUInt32 inPAddr,
			KUInt32 inSize );

	///
	/// Get the function for a given offset.
	///
	JITFuncPtr GetJITFuncForOffset(TMemory& inMemoryIntf, KUInt32 inPC);

	///
	/// Get the function for a single instruction at given offset.
	/// This is used with Step.
	///
	JITFuncPtr GetJITFuncForSingleInstructionAtOffset(TMemory& inMemoryIntf, KUInt32 inPC);

	///
	/// Get the prefix for this page.
	/// This prefix is applied to both modules and function names.
	///
	std::string PagePrefix() const;
	
	///
	/// Get a module name for a given entry point.
	///
	std::string ModuleName(KUInt32 entryPointOffset) const;
	
	///
	/// Get a function name for a given offset.
	///
	std::string FunctionName(KUInt32 offset) const;
	
	///
	/// Extract the offset from a function name.
	///
	KUInt32 OffsetFromFunctionName(const std::string& inFunctionName) const;

	///
	/// Get the page prefix for a given module.
	/// Used by the cache to gather modules belonging to a given page.
	///
	static std::string PagePrefixFromModuleName(const std::string& inModuleName);

	///
	/// Determine if we already have a given function to avoid translating it again.
	///
	bool HasTranslatedFunction(KUInt32 inOffset) const {
		return mEntryPointFunctions.count(inOffset) > 0;
	}

private:
	///
	/// Prevent copies.
	///
	TJITLLVMPage( const TJITLLVMPage& ) = delete;
	
	/// \name Variables
	std::unique_ptr<llvm::ExecutionEngine>		mExecutionEngine;		///< Execution engine for the page.
	llvm::SmallVector<llvm::ObjectImage*, 2>	mLoadedObjects;			///< Loaded, cached objects.
	std::map<KUInt32, JITFuncPtr>				mEntryPointFunctions;	///< Available functions by offset.
	std::map<KUInt32, JITFuncPtr>				mStepFunctions;			///< Step functions.
	TJITLLVMTranslator							mTranslator;
};

#endif
		// _TJITLLVMPAGE_H

#endif

// ======================================== //
// Ask not for whom the <CONTROL-G> tolls. //
// ======================================== //
