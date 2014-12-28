// ==============================
// File:			TJITLLVMOptimizeReadPass.cp
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
#include "Emulator/JIT/LLVM/TJITLLVMOptimizeReadPass.h"

#include <llvm/IR/Instruction.h>

using namespace llvm;

// -------------------------------------------------------------------------- //
// Constants
// -------------------------------------------------------------------------- //

char TJITLLVMOptimizeReadPass::ID = 0;

// -------------------------------------------------------------------------- //
//  * runOnBasicBlock(BasicBlock&)
// -------------------------------------------------------------------------- //
bool
TJITLLVMOptimizeReadPass::runOnBasicBlock(BasicBlock& block) {
	bool result = false;
	IRBuilder<> builder(block.getContext());
	SmallVector<CallInst*, 1> callInstructions;
	for (Instruction& inst : block) {
		if (isa<CallInst>(inst)) {
			CallInst* callInstruction = dyn_cast<CallInst>(&inst);
			Function* calledFunction = callInstruction->getCalledFunction();
			if (calledFunction) {
				StringRef name = calledFunction->getName();
				bool readByte = name.equals("JIT_ReadB");
				bool readWord = readByte ? false : name.equals("JIT_Read");
				bool readBlock = (readWord || readByte) ? false : name.equals("JIT_ReadBlock");
				if (readWord || readByte || readBlock) {
					Value* value = callInstruction->getOperand(1);
					if (isa<ConstantInt>(value)) {
						ConstantInt* constValue = dyn_cast<ConstantInt>(value);
						APInt intValue = constValue->getValue();
						KUInt32 startAddress = (KUInt32) intValue.getZExtValue();
						KUInt32 endAddress = (KUInt32) intValue.getZExtValue();
						KUInt32 numWords = 0;
						if (readBlock) {
							numWords = (KUInt32) dyn_cast<ConstantInt>(callInstruction->getOperand(2))->getValue().getZExtValue();
							endAddress = startAddress + (numWords - 1) * sizeof(KUInt32);
						}
						Value* readValue = nullptr;
						const KUInt32* blockPtr = nullptr;
						if (TMemory::IsPageInROM(startAddress) && TMemory::IsPageInROM(endAddress)) {
							const KUInt32* pointer = mMemoryIntf.GetDirectPointerToPage(startAddress);
							if (readByte) {
								readValue = builder.getInt8(*((const KUInt8*) pointer));
							} else if (readWord) {
								readValue = builder.getInt32(*pointer);
							} else {
								blockPtr = pointer;
							}
						} else if (startAddress >= mPage.GetVAddr() && endAddress < mPage.GetVAddr() + mPage.GetSize()) {
							const KUInt8* pointer = (const KUInt8*) mPage.GetPointer() + startAddress - mPage.GetVAddr();
							if (readByte) {
								readValue = builder.getInt8(*pointer);
							} else if (readWord) {
								readValue = builder.getInt32(*((const KUInt32*) pointer));
							} else {
								blockPtr = (const KUInt32*) pointer;
							}
						}
						
						if (readValue != nullptr) {
							builder.SetInsertPoint(callInstruction);
							builder.CreateStore(readValue, callInstruction->getOperand(2));
							callInstruction->replaceAllUsesWith(builder.getInt1(false));
							callInstructions.push_back(callInstruction);
							result = true;
						} else if (blockPtr != nullptr) {
							builder.SetInsertPoint(callInstruction);
							Value* buffer = callInstruction->getOperand(3);
							for (unsigned wordIx = 0; wordIx < numWords; wordIx++) {
								builder.CreateStore(builder.getInt32(blockPtr[wordIx]), builder.CreateGEP(buffer, builder.getInt32(wordIx)));
							}
							callInstruction->replaceAllUsesWith(builder.getInt1(false));
							callInstructions.push_back(callInstruction);
							result = true;
						}
					}
				}
			}
		}
	}
	
	if (result) {
		for (CallInst* inst : callInstructions) {
			inst->eraseFromParent();
		}
	}

	return result;
}

#endif
