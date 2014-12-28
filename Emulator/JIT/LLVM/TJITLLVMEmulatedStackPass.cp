// ==============================
// File:			TJITLLVMEmulatedStackPass.cp
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

#include <K/Defines/KDefinitions.h>
#include "JIT.h"

#ifdef JITTARGET_LLVM

#include "Emulator/TMemory.h"
#include "Emulator/JIT/LLVM/TJITLLVMEmulatedStackPass.h"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/Analysis/CFG.h>
#include <llvm/Analysis/LoopInfo.h>

using namespace llvm;

// -------------------------------------------------------------------------- //
// Constants
// -------------------------------------------------------------------------- //

char TJITLLVMEmulatedStackPass::ID = 0;

// -------------------------------------------------------------------------- //
//  * runOnFunction(Function&)
// -------------------------------------------------------------------------- //
void
TJITLLVMEmulatedStackPass::getAnalysisUsage(llvm::AnalysisUsage & AU) const {
	AU.setPreservesCFG();
	AU.addRequired<DominatorTreeWrapperPass>();
}

// -------------------------------------------------------------------------- //
//  * runOnFunction(Function&)
// -------------------------------------------------------------------------- //
bool
TJITLLVMEmulatedStackPass::runOnFunction(Function& function) {
	bool result = false;
	IRBuilder<> builder(function.getContext());

    DominatorTree& DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    LoopInfo* LI = getAnalysisIfAvailable<LoopInfo>();
    
    // Collect every writes and every reads.
    std::deque<MemoryOp> memoryOperations = FindMemoryOperations(function);
	
	// This is a N^3 algorithm that could be optimized if profiling reveals it is worth it.
	// Consider every read operation.
	for (MemoryOp readOp : memoryOperations) {
		if (readOp.type == Read) {
			// Look for dominating writes with the same address and same size.
			for (MemoryOp writeOp : memoryOperations) {
				if (writeOp.type == Write
					&& writeOp.size == readOp.size
					&& IsSameAddress(writeOp.startAddress, readOp.startAddress)
					&& DT.dominates(writeOp.inst, readOp.inst)) {
					// Found a candidate operation.
					// We try to find out whether any invalidation could happen between both operations.
					bool invalidated = false;
					for (MemoryOp invalidatingOp : memoryOperations) {
						if (invalidatingOp.type == Read) continue;
						if (invalidatingOp.inst == writeOp.inst) continue;
						// We know write >> read (write dominates)
						// If invalidating -?> read, check if we have invalidating >> write >> read,
						// i.e. if we cannot have write -?> invalidating.
						if (isPotentiallyReachable(invalidatingOp.inst, readOp.inst, &DT, LI)
							&& !DT.dominates(invalidatingOp.inst, writeOp.inst)) {
							if (invalidatingOp.type == Invalidating) {
								invalidated = true;
								break;
							} else {
								invalidated = MayInvalidate(invalidatingOp, writeOp);
								if (invalidated) break;
							}
						}
					}
					if (!invalidated) {
						RewriteRead(builder, readOp, writeOp);
						result = true;
						break;	// Only erase it once.
					}
				}
			}
		}
	}
	return result;
}

// -------------------------------------------------------------------------- //
//  * FindMemoryOperations(Function&) const
// -------------------------------------------------------------------------- //
std::deque<TJITLLVMEmulatedStackPass::MemoryOp>
TJITLLVMEmulatedStackPass::FindMemoryOperations(Function& function) const {
	std::deque<MemoryOp> result;
    for (inst_iterator I = inst_begin(&function), E = inst_end(&function); I != E; ++I) {
        Instruction& inst = *I;
		if (isa<CallInst>(inst)) {
            CallInst* callInst = dyn_cast<CallInst>(&inst);
            Function* calledFunction = callInst->getCalledFunction();
            if (calledFunction) {
                StringRef name = calledFunction->getName();
                bool isMemoryOp = true;
                MemoryOp op;
				op.startAddress = callInst->getArgOperand(1);
				op.inst = callInst;
                if (calledFunction->isIntrinsic()) {
                    isMemoryOp = false;
                } else if (name.equals("JIT_Read")) {
                    op.type = Read;
                    op.isBlock = false;
                    op.size = 4;
                } else if (name.equals("JIT_ReadB")) {
                    op.type = Read;
                    op.isBlock = false;
                    op.size = 1;
                } else if (name.equals("JIT_ReadBlock")) {
                    op.type = Read;
                    op.isBlock = true;
                    ConstantInt* numWords = dyn_cast<ConstantInt>(callInst->getArgOperand(2));
                    if (numWords == nullptr) {
                        result.clear();
                        return result;
                    }
                    op.size = 4 * (uint32_t) numWords->getZExtValue();
                } else if (name.equals("JIT_Write")) {
                    op.type = Write;
                    op.isBlock = false;
                    op.size = 4;
                } else if (name.equals("JIT_WriteB")) {
                    op.type = Write;
                    op.isBlock = false;
                    op.size = 1;
                } else if (name.equals("JIT_WriteBlock")) {
                    op.type = Write;
                    op.isBlock = true;
                    ConstantInt* numWords = dyn_cast<ConstantInt>(callInst->getArgOperand(2));
                    if (numWords == nullptr) {
                        result.clear();
                        return result;
                    }
                    op.size = 4 * (uint32_t) numWords->getZExtValue();
                } else {
                    op.type = Invalidating;
					op.startAddress = nullptr;
                }
                if (isMemoryOp) {
                    result.push_back(op);
                }
            }
        }
	}
	return result;
}

// -------------------------------------------------------------------------- //
//  * IsSameAddress(Value*, Value*, int32_t, int32_t) const
// -------------------------------------------------------------------------- //
bool
TJITLLVMEmulatedStackPass::IsSameAddress(llvm::Value* addr1, llvm::Value* addr2, int64_t offset1, int64_t offset2) const {
	if (addr1 == addr2 && offset1 == offset2) {
		return true;
	}
	ConstantInt* addr1Con = dyn_cast<ConstantInt>(addr1);
	ConstantInt* addr2Con = dyn_cast<ConstantInt>(addr2);
	if (addr1Con && addr2Con) {
		return (addr1Con->getSExtValue() + offset1) == (addr2Con->getSExtValue() + offset2);
	}
	Value* newAddr1 = addr1;
	int64_t newOffset1 = offset1;
	Value* newAddr2 = addr2;
	int64_t newOffset2 = offset2;
	if (AddOperator* addOp = dyn_cast<AddOperator>(addr1)) {
		if (addOp->getNumOperands() == 2) {
			if (ConstantInt* offset = dyn_cast<ConstantInt>(addOp->getOperand(1))) {
				newOffset1 += offset->getValue().getSExtValue();
				newAddr1 = addOp->getOperand(0);
			}
		}
	}
	if (AddOperator* addOp = dyn_cast<AddOperator>(addr2)) {
		if (addOp->getNumOperands() == 2) {
			if (ConstantInt* offset = dyn_cast<ConstantInt>(addOp->getOperand(1))) {
				newOffset2 += offset->getValue().getSExtValue();
				newAddr2 = addOp->getOperand(0);
			}
		}
	}
	Instruction* inst1 = dyn_cast<Instruction>(newAddr1);
	Instruction* inst2 = dyn_cast<Instruction>(newAddr2);
	if (inst1 && inst2 && inst1->isIdenticalTo(inst2)) {
		newAddr2 = newAddr1;
	}
	if (addr1 != newAddr1 || addr2 != newAddr2) {
		return IsSameAddress(newAddr1, newAddr2, newOffset1, newOffset2);
	}
	return false;
}

// -------------------------------------------------------------------------- //
//  * RewriteRead(MemoryOp&, MemoryOp&) const
// -------------------------------------------------------------------------- //
void
TJITLLVMEmulatedStackPass::RewriteRead(llvm::IRBuilder<>& builder, MemoryOp& readOp, MemoryOp& writeOp) const {
	builder.SetInsertPoint(readOp.inst);
	if (readOp.isBlock && writeOp.isBlock) {
		Value* readBuffer = readOp.inst->getOperand(3);
		Value* writeBuffer = writeOp.inst->getOperand(3);
		// Currently copy buffers.
		// We could swap buffers if we proved read buffer was not written to/read before write.
		unsigned numWords = readOp.size / 4;
		for (unsigned wordIx = 0; wordIx < numWords; wordIx++) {
			builder.CreateStore(builder.CreateLoad(builder.CreateGEP(writeBuffer, builder.getInt32(wordIx))), builder.CreateGEP(readBuffer, builder.getInt32(wordIx)));
		}
		readOp.inst->replaceAllUsesWith(builder.getInt1(false));
		readOp.inst->eraseFromParent();
	} else if (!readOp.isBlock && !writeOp.isBlock && readOp.size == 4 && writeOp.size == 4) {
		Value* readBuffer = readOp.inst->getOperand(2);
		Value* writeValue = writeOp.inst->getOperand(2);
		builder.CreateStore(writeValue, readBuffer);
		readOp.inst->replaceAllUsesWith(builder.getInt1(false));
		readOp.inst->eraseFromParent();
	}
}

// -------------------------------------------------------------------------- //
//  * MayInvalidate(Value*, int32_t&) const
// -------------------------------------------------------------------------- //
bool
TJITLLVMEmulatedStackPass::MayInvalidate(MemoryOp& invalidatingOp, MemoryOp& writeOp) const {
	// Cowardly considering that write op could be invalidated
	// unless write op is on the stack and invalidating op is not or at a different offset.
	int32_t writeOpStackOffset = 0;
	if (IsStackRelative(writeOp.startAddress, writeOpStackOffset)) {
		if (writeOpStackOffset + writeOp.size > 0) return true;
		int32_t invalidatingOpStackOffset = 0;
		if (IsStackRelative(invalidatingOp.startAddress, invalidatingOpStackOffset)) {
			if (invalidatingOpStackOffset == writeOpStackOffset) return true;
			if (invalidatingOpStackOffset > writeOpStackOffset
				&& writeOpStackOffset + writeOp.size > invalidatingOpStackOffset) return true;
			if (invalidatingOpStackOffset < writeOpStackOffset
				&& invalidatingOpStackOffset + invalidatingOp.size > writeOpStackOffset) return true;
		}
		return false;
	}
	return true;
}

// -------------------------------------------------------------------------- //
//  * IsStackRelative(Value*, int32_t&) const
// -------------------------------------------------------------------------- //
bool
TJITLLVMEmulatedStackPass::IsStackRelative(llvm::Value* value, int32_t& ioOffset) const {
	if (isa<Constant>(value)) {
		return false;
	}
	if (isa<PHINode>(value)) {
		return false;
	}
	if (AddOperator* addOp = dyn_cast<AddOperator>(value)) {
		if (addOp->getNumOperands() == 2) {
			if (ConstantInt* offset = dyn_cast<ConstantInt>(addOp->getOperand(1))) {
				ioOffset += offset->getValue().getSExtValue();
				return IsStackRelative(addOp->getOperand(0), ioOffset);
			}
		}
	}
	if (LoadInst* loadInst = dyn_cast<LoadInst>(value)) {
		Value* loadPtr = loadInst->getOperand(0);
		if (GEPOperator* gepOp = dyn_cast<GEPOperator>(loadPtr)) {
			if (gepOp->getNumIndices() == 2
				&& gepOp->getPointerOperandType() == mProcessorPointerType) {
				GEPOperator::const_op_iterator iter = gepOp->idx_begin();
				ConstantInt* firstIndex = dyn_cast<ConstantInt>(iter);
				ConstantInt* secondIndex = dyn_cast<ConstantInt>(iter[1]);
				if ((firstIndex != nullptr) &&
					(firstIndex->getValue().getSExtValue() == 0) &&
					(secondIndex != nullptr) &&
					(secondIndex->getValue().getSExtValue() == 13)) {
					return true;
				}
			}
		}
	}
	return false;
}

#endif
