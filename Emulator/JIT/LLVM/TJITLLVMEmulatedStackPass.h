// ==============================
// File:			TJITLLVMEmulatedStackPass.h
// Project:			Einstein
//
// Copyright 2014 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TJITLLVMEMULATEDSTACKPASS_H
#define _TJITLLVMEMULATEDSTACKPASS_H

// llvm
#include <llvm/Pass.h>
#include <llvm/IR/Dominators.h>

class TMemory;

///
/// Pass to optimize write and read operations on emulated stack.
/// This pass tries to eliminate saving and restoring of registers.
///
class TJITLLVMEmulatedStackPass : public llvm::FunctionPass {
public:
	///
	/// Constructor from the TARMProcessor pointer type.
	///
	TJITLLVMEmulatedStackPass(llvm::PointerType* inProcessorPtrType) :
	llvm::FunctionPass(ID),
	mProcessorPointerType(inProcessorPtrType) {};
	
	virtual ~TJITLLVMEmulatedStackPass() {};
	
	///
	/// Dependency handling.
	///
	void getAnalysisUsage(llvm::AnalysisUsage &) const override;
		
	///
	/// Perform optimization.
	/// Returns true if the function was modified.
	///
	bool runOnFunction(llvm::Function&) override;

private:
	enum MemoryOpType {
		Read,
		Write,
		Invalidating	// Other operations that may invalidate anything
	};
	struct MemoryOp {
		MemoryOpType		type;
		bool				isBlock;
		llvm::Value*		startAddress;
		uint32_t			size;
		llvm::CallInst*		inst;
	};
	
	///
	/// Find memory operations.
	///
	std::deque<MemoryOp> FindMemoryOperations(llvm::Function&) const;
	
	///
	/// Determine whether two addresses are identical.
	///
	bool IsSameAddress(llvm::Value* addr1, llvm::Value* addr2, int64_t offset1 = 0, int64_t offset2 = 0) const;
	
	///
	/// Determine whether a write operation may invalidate another operation.
	///
	bool MayInvalidate(MemoryOp& invalidatingOp, MemoryOp& writeOp) const;

	///
	/// Determine whether a value is stack relative.
	///
	bool IsStackRelative(llvm::Value* value, int32_t& ioOffset) const;

	///
	/// Rewrite a read operation by using written buffer.
	///
	void RewriteRead(llvm::IRBuilder<>& builder, MemoryOp& readOp, MemoryOp& writeOp) const;

	llvm::PointerType*	mProcessorPointerType;
	static char			ID;
};

#endif
