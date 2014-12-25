// ==============================
// File:			TJITLLVM.h
// Project:			Einstein
//
// Copyright 2013 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TJITLLVM_H
#define _TJITLLVM_H

#include <K/Defines/KDefinitions.h>
#include "TJIT.h"

// Einstein
#include "Emulator/JIT/LLVM/TJITLLVMPage.h"

#include <llvm/IR/Module.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Object/ObjectFile.h>

#include "K/Threads/TThread.h"
#include "K/Threads/TMutex.h"
#include "K/Threads/TCondVar.h"

// C++
#include <set>
#include <deque>


class TMemory;
class TARMProcessor;
class TJITLLVMObjectCache;
class TROMImage;

///
/// Class for LLVM-based JIT interface.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 150 $
///
class TJITLLVM
	:
		public TJIT< TJITLLVM, TJITLLVMPage >
{
public:
	
	///
	/// Constructor.
	///
	/// \param inMemoryIntf	interface to memory.
	/// \param inMMUIntf	interface to the MMU.
	/// \param inROMImage	ROM image or nullptr if there is no image.
	///
	TJITLLVM(
		TMemory* inMemoryIntf,
		TMMU* inMMUIntf,
		const TROMImage* inROMImage);
	
	///
	/// Destructor.
	///
	virtual ~TJITLLVM( void );
	
	///
	/// One or more steps with JIT.
	///
	/// \param ioObject			ARM CPU.
	/// \param inCount			number of instructions to execute.
	///
	virtual void	Step(
						TARMProcessor* ioObject,
						KUInt32 inCount );

	///
	/// Run with JIT.
	///
	/// \param ioObject			ARM CPU.
	/// \param inSignal			pointer to a signal set to false to stop
	///							execution.
	///
	virtual void	Run(
						TARMProcessor* ioObject,
						volatile bool* inSignal );

	///
	/// Get a JIT ptr for a given PC.
	///
	JITFuncPtr GetJITFuncForPC(
					TARMProcessor* ioCPU,
					TMemory& inMemoryInterface,
					KUInt32 inPC );

	///
	/// Get a JIT ptr for a single instruction at a given PC.
	///
	JITFuncPtr
	GetJITFuncForSingleInstructionAtPC(
				   TARMProcessor* ioCPU,
				   TMemory& inMemoryInterface,
				   KUInt32 inPC );

	///
	/// ID and version for patches.
	///
	static const KUInt32 kID = 0x4C4C564D;	// 'LLVM'
	static const KUInt32 kVersion = 0;
	
	///
	/// Patch the ROM.
	/// This function is called to modify the ROM before it is saved on disk.
	/// It is only called when the image is created.
	///
	static void DoPatchROM(KUInt32* romPtr, const std::string& inMachineName);
	
	///
	/// Get the object cache, or nullptr if it is not enabled (typically we're
	/// not starting from a ROM Image).
	///
	TJITLLVMObjectCache* GetObjectCache() {
		return mObjectCache;
	}
	
	///
	/// Create a new execution engine.
	///
	llvm::ExecutionEngine* CreateExecutionEngine();
	
private:
	TJITLLVM( const TJITLLVM& inCopy ) = delete;
	TJITLLVM& operator = ( const TJITLLVM& inCopy ) = delete;
	
	///
	/// Constructor helper to create the glues table.
	///
	static std::map<std::string, uint64_t> CreateGluesTable();
	
	TJITLLVMObjectCache*	mObjectCache;	///< Directory with cached native code.
	const llvm::Target*		mTarget;		///< Lookup only once.
	const std::map<std::string, uint64_t> mGluesTable;	///< Address of glue functions.
	TJITLLVMPage			mMainROMPage;
};

///
/// Memory manager that solves glue symbols.
///
class TJITLLVMGlueMemoryManager : public llvm::SectionMemoryManager {
public:
	TJITLLVMGlueMemoryManager(const std::map<std::string, uint64_t>& inGlues) :
	mGluesTable(inGlues) {};

	/// This method returns the address of the specified function or variable.
	/// It is used to resolve symbols during module linking.
	uint64_t getSymbolAddress(const std::string &Name) override;

private:
	const std::map<std::string, uint64_t>&	mGluesTable;
};

///
/// Memory manager that records allocation so we know about load addresses
/// of functions and avoid symbol lookups.
///
/// (What a hack !)
///
class TJITLLVMRecordingMemoryManager : public TJITLLVMGlueMemoryManager {
public:
	TJITLLVMRecordingMemoryManager(const std::map<std::string, uint64_t>& inGlues) :
	TJITLLVMGlueMemoryManager(inGlues) {};
	
	///
	/// Patch of allocateCodeSection callback to note where
	/// sections are loaded
	///
	uint8_t* allocateCodeSection(uintptr_t size, unsigned alignment, unsigned sectionID, llvm::StringRef sectionName) override;
	
	///
	/// Start recording allocations.
	///
	void FlushRecordedAllocations() {
		mAllocatedSectionsCount = 0;
		mLatestAllocatedSectionID = (unsigned) -1;
		mLatestAllocatedAddress = nullptr;
	}
	
	///
	/// Get section load address.
	/// Aborts if more than one section was loaded.
	///
	uint8_t* GetLatestSectionLoadAddress();
	
private:
	unsigned							mAllocatedSectionsCount;
	unsigned							mLatestAllocatedSectionID;
	uint8_t*							mLatestAllocatedAddress;
};

#endif
		// _TJITLLVM_H

// ====================================================================== //
// A complex system that works is invariably found to have evolved from a //
// simple system that works.                                              //
// ====================================================================== //
