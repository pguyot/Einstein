// ==============================
// File:			TJITLLVMTranslator.cp
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

#include "TJITLLVMTranslator.h"
#include "Emulator/TARMProcessor.h"
#include "Emulator/JIT/LLVM/TJITLLVMPage.h"
#include "Emulator/JIT/LLVM/TJITLLVMOptimizeReadPass.h"

// llvm
#include <llvm/Analysis/Passes.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Verifier.h>

#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Pass.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/IR/Intrinsics.h>

// C++
#include <memory>
#include <sstream>

using namespace llvm;

#ifdef DEBUG
	#define STRINGIFY(x) #x
	#define TOSTRING(x) STRINGIFY(x)
//	#define ANONBLOCK "_:" TOSTRING(__LINE__) "#" TOSTRING(__COUNTER__)
//	#define BLOCKNAME(x) x ":" TOSTRING(__LINE__) "#" TOSTRING(__COUNTER__)
	#define ANONBLOCK "_#" TOSTRING(__COUNTER__)
	#define BLOCKNAME(x) x "#" TOSTRING(__COUNTER__)
#else
    #define ANONBLOCK ""
    #define BLOCKNAME(x) ""
#endif

// -------------------------------------------------------------------------- //
// Constants
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TranslateEntryPoint(KUInt32, KUInt32, std::string&)
// -------------------------------------------------------------------------- //
std::map<KUInt32, Function*>
TJITLLVMTranslator::TranslateEntryPoint(const TMemory& inMemoryIntf, KUInt32 offsetInPage, Module* inModule)
{
	FrameTranslator tr(inMemoryIntf, mPage, offsetInPage, inModule, false);
	tr.Translate(offsetInPage);
	return tr.Finish();
}

// -------------------------------------------------------------------------- //
//  * TranslateSingleInstruction(KUInt32, std::string&)
// -------------------------------------------------------------------------- //
Function*
TJITLLVMTranslator::TranslateSingleInstruction(const TMemory& inMemoryIntf, KUInt32 offsetInPage, Module* inModule)
{
	FrameTranslator tr(inMemoryIntf, mPage, offsetInPage, inModule, true);
	tr.Translate(offsetInPage);
	std::map<KUInt32, Function*> generated = tr.Finish();
	return generated[offsetInPage];
}

// -------------------------------------------------------------------------- //
//  * FrameTranslator(const TJITLLVMPage&, KUInt32, Module*, bool)
// -------------------------------------------------------------------------- //
TJITLLVMTranslator::FrameTranslator::FrameTranslator(
			const TMemory& inMemoryIntf,
			const TJITLLVMPage& inPage,
			KUInt32 offsetInPage,
			Module* inModule,
			bool stepFunction)
    :
		mStepFunction(stepFunction),
		mPage(inPage),
        mModule(inModule),
		mContext(inModule->getContext()),
		mBuilder(mContext),
		mFPM(inModule),
		mMemory(nullptr),
        mLabels(inPage.GetInstructionCount() + 1, nullptr),
		mCurrentVAddress(inPage.GetVAddr()),
		mCurrentPointer(inPage.GetPointer()),
		mInstructionCount(inPage.GetInstructionCount()),
		mOffsetInPage(offsetInPage),
    	mCPSR_N(nullptr),
	    mCPSR_Z(nullptr),
	    mCPSR_C(nullptr),
	    mCPSR_V(nullptr),
		mTMemoryPtrType(nullptr),
		mTARMProcessorPtrType(nullptr)
{
	for (int i = 0; i < 15; i++) {
		mRegisters[i] = nullptr;
	}

	// Setup optimizations.
	mFPM.add(new DataLayoutPass(inModule));
	// Provide basic AliasAnalysis support for GVN.
	mFPM.add(createBasicAliasAnalysisPass());
	
	mFPM.add(new TJITLLVMOptimizeReadPass(inMemoryIntf, inPage));
	
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	mFPM.add(createInstructionCombiningPass());
	// Reassociate expressions.
	mFPM.add(createReassociatePass());
	// Eliminate Common SubExpressions.
	mFPM.add(createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	mFPM.add(createCFGSimplificationPass());
	// Convert calls to tail calls (all JIT exit functions are designed for tail-calling).
	mFPM.add(createTailCallEliminationPass());
	// Use registers.
	mFPM.add(createPromoteMemoryToRegisterPass());
	mFPM.doInitialization();
	
	// Create inner function
	std::stringstream stream;
	stream << "i" << (mPage.GetVAddr() + (offsetInPage * 4));
	std::string functionName(stream.str());
	mFunction = Function::Create(GetInnerFuncType(), Function::PrivateLinkage, functionName, mModule);
	
	mPrologue = BasicBlock::Create(mContext, BLOCKNAME("prologue"), mFunction);
	
	auto argsIt = mFunction->arg_begin();
	mProcessor = argsIt++;
	mSignal = argsIt++;
	mOffsetArg = argsIt;
	mBuilder.SetInsertPoint(mPrologue);
	mPC = mBuilder.CreateAlloca(IntegerType::getInt32Ty(mContext));
	mMainExit = BasicBlock::Create(mContext, BLOCKNAME("mainexit"), mFunction);

	// Exit step : put it everywhere except at offset.
	if (mStepFunction) {
		for (int i = 0; i < mInstructionCount; i++) {
			if (i != offsetInPage) {
				BasicBlock* exitStepBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
				mLabels[i] = exitStepBlock;
				KUInt32 exitStepPC = mPage.GetVAddr() + (i + 1) * 4;
				mBuilder.SetInsertPoint(exitStepBlock);
				mBuilder.CreateStore(mBuilder.getInt32(exitStepPC), mPC);
				mBuilder.CreateBr(mMainExit);
			}
		}
	}
	// End of page.
	BasicBlock* endOfPageBlock = BasicBlock::Create(mContext, BLOCKNAME("endofpage"), mFunction);
	KUInt32 endOfPagePC = mPage.GetVAddr() + (mInstructionCount + 1) * 4;
	mLabels[mInstructionCount] = endOfPageBlock;
	mBuilder.SetInsertPoint(endOfPageBlock);
	mBuilder.CreateStore(mBuilder.getInt32(endOfPagePC), mPC);
	mBuilder.CreateBr(mMainExit);
	
	mBuilder.SetInsertPoint(mMainExit);
	Function* continueFunction = EnsureFunction(GetEntryPointFuncType(), "JIT_Continue");
	Instruction* retInst = mBuilder.CreateRet(mBuilder.CreateBitCast(continueFunction, PointerType::getInt8PtrTy(mContext)));
	mExitInstructions.push_back(retInst);
}

// -------------------------------------------------------------------------- //
//  * Finish()
// -------------------------------------------------------------------------- //
std::map<KUInt32, Function*>
TJITLLVMTranslator::FrameTranslator::Finish()
{
    // Generate register read operations in inner function's prologue.
	mBuilder.SetInsertPoint(mPrologue);
	std::vector<llvm::Value *> indexes;
	indexes.push_back(mBuilder.getInt32(0));
	indexes.push_back(mBuilder.getInt32(0));
	for (int i = 0; i < 15; i++) {
		Value* reg = mRegisters[i];
		if (reg != nullptr) {
			indexes[1] = mBuilder.getInt32(i_mCurrentRegisters_R0 + i);
			mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[i]);
		}
	}
	// We do not read PC.
	if (mCPSR_N != nullptr) {
		indexes[1] = mBuilder.getInt32(i_mCPSR_N);
		mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mCPSR_N);
	}
	if (mCPSR_Z != nullptr) {
		indexes[1] = mBuilder.getInt32(i_mCPSR_Z);
		mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mCPSR_Z);
	}
	if (mCPSR_C != nullptr) {
		indexes[1] = mBuilder.getInt32(i_mCPSR_C);
		mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mCPSR_C);
	}
	if (mCPSR_V != nullptr) {
		indexes[1] = mBuilder.getInt32(i_mCPSR_V);
		mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mCPSR_V);
	}
	
	// Generate switch unless there is a single entry.
	unsigned nbFunctions = (unsigned) mNewFunctionsBlocks.size();
	if (nbFunctions == 1) {
		mBuilder.CreateBr(mNewFunctionsBlocks.begin()->second);
	} else {
		SwitchInst* innerSwitch = mBuilder.CreateSwitch(mOffsetArg, mMainExit, nbFunctions);
		for (const auto& block : mNewFunctionsBlocks) {
			innerSwitch->addCase(mBuilder.getInt32(block.first), block.second);
		}
	}

	// Restore registers on each exit point.
	for (const auto exitInstructions : mExitInstructions) {
		mBuilder.SetInsertPoint(exitInstructions);
		PrepareExit();
	}
	
	// Inline inner function for each identified "function".
	InlineFunctionInfo ifi;
	for (CallInst* inlinableCall : mInlineCalls) {
		InlineFunction(inlinableCall, ifi);
	}
	
	// Optimize inner function.
#ifdef DEBUG
	// Verify the function.
	verifyFunction(*mFunction);
#endif

	// Print non optimized version.
	// mFunction->dump();
	
	mFPM.run(*mFunction);

	// Print optimized version.
	// mFunction->dump();
	
	// Optimize outer functions.
	for (auto newFuncPair : mNewFunctions) {
		mFPM.run(*newFuncPair.second);
	}
	
	return mNewFunctions;
}

// -------------------------------------------------------------------------- //
//  * PrepareExit()
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::PrepareExit()
{
	// Generate register write operations.
	std::vector<llvm::Value *> indexes;
	indexes.push_back(mBuilder.getInt32(0));
	indexes.push_back(mBuilder.getInt32(0));
	for (int i = 0; i < 15; i++) {
		Value* reg = mRegisters[i];
		if (reg != nullptr) {
			indexes[1] = mBuilder.getInt32(i_mCurrentRegisters_R0 + i);
			mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[i]), mBuilder.CreateGEP(mProcessor, indexes));
		}
	}
	indexes[1] = mBuilder.getInt32(i_mCurrentRegisters_R15);
	mBuilder.CreateStore(mBuilder.CreateLoad(mPC), mBuilder.CreateGEP(mProcessor, indexes));
	if (mCPSR_N != nullptr) {
		indexes[1] = mBuilder.getInt32(i_mCPSR_N);
		mBuilder.CreateStore(mBuilder.CreateLoad(mCPSR_N), mBuilder.CreateGEP(mProcessor, indexes));
	}
	if (mCPSR_Z != nullptr) {
		indexes[1] = mBuilder.getInt32(i_mCPSR_Z);
		mBuilder.CreateStore(mBuilder.CreateLoad(mCPSR_Z), mBuilder.CreateGEP(mProcessor, indexes));
	}
	if (mCPSR_C != nullptr) {
		indexes[1] = mBuilder.getInt32(i_mCPSR_C);
		mBuilder.CreateStore(mBuilder.CreateLoad(mCPSR_C), mBuilder.CreateGEP(mProcessor, indexes));
	}
	if (mCPSR_V != nullptr) {
		indexes[1] = mBuilder.getInt32(i_mCPSR_V);
		mBuilder.CreateStore(mBuilder.CreateLoad(mCPSR_V), mBuilder.CreateGEP(mProcessor, indexes));
	}
}

// -------------------------------------------------------------------------- //
//  * EnsureAllocated(Value**, int)
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::EnsureAllocated(Value** reg, int numBits) {
	if (*reg == nullptr) {
		auto ip = mBuilder.saveIP();
		mBuilder.SetInsertPoint(mPrologue);
		*reg = mBuilder.CreateAlloca(IntegerType::getIntNTy(mContext, numBits));
		mBuilder.restoreIP(ip);
	}
}

// -------------------------------------------------------------------------- //
//  * GetMemory()
// -------------------------------------------------------------------------- //
llvm::Value*
TJITLLVMTranslator::FrameTranslator::GetMemory() {
	if (mMemory == nullptr) {
		auto ip = mBuilder.saveIP();
		mBuilder.SetInsertPoint(mPrologue);
		std::vector<llvm::Value *> indexes;
		indexes.push_back(mBuilder.getInt32(0));
		indexes.push_back(mBuilder.getInt32(i_mMemory));
		mMemory = mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes));
		mBuilder.restoreIP(ip);
	}
	return mMemory;
}

// -------------------------------------------------------------------------- //
//  * BuildExitToFunction(const char*, KUInt32)
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::BuildExitToFunction(const char *funcName, KUInt32 exitPC) {
	BuildExitToFunction(funcName, mBuilder.getInt32(exitPC));
}

// -------------------------------------------------------------------------- //
//  * BuildExitToFunction(const char*, Value*)
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::BuildExitToFunction(const char *funcName, Value* exitPC) {
	if (exitPC != nullptr) {
		mBuilder.CreateStore(exitPC, mPC);
	}
	Function* function = EnsureFunction(GetEntryPointFuncType(), funcName);
	Instruction* callInst = mBuilder.CreateCall2(function, mProcessor, mSignal);
	mExitInstructions.push_back(callInst);
	mBuilder.CreateRet(callInst);
}

// -------------------------------------------------------------------------- //
//  * EnsureFunction(FunctipnType*, const char*)
// -------------------------------------------------------------------------- //
Function*
TJITLLVMTranslator::FrameTranslator::EnsureFunction(FunctionType* funcType, const char* funcName) {
	Function* function = mModule->getFunction(funcName);
	if (function == nullptr) {
		function = Function::Create(funcType, Function::ExternalLinkage, funcName, mModule);
		function->setCallingConv(CallingConv::C);
		function->setDoesNotThrow();
	}
	return function;
}

// -------------------------------------------------------------------------- //
//  * EnsureIntrinsic(FunctipnType*, const char*)
// -------------------------------------------------------------------------- //
Function*
TJITLLVMTranslator::FrameTranslator::EnsureIntrinsic(Intrinsic::ID intrinsicID) {
	std::vector<Type *> arg_type;
	arg_type.push_back(mBuilder.getInt32Ty());
	return Intrinsic::getDeclaration(mModule, intrinsicID, arg_type);
}

// -------------------------------------------------------------------------- //
//  * Translate(KUInt32)
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate(KUInt32 inOffsetInPage)
{
	// Push first block in the queue and create a function for the requested
	// entry point.
	AddFunctionIfRequired(inOffsetInPage, true);
	
	KUInt32 previousInstruction = 0;
	
	while (!mPending.empty()) {
		KUInt32 offsetInPage = mPending.front();
		mPending.pop_front();

		KUInt32 instruction = mCurrentPointer[offsetInPage];
		if (TranslateOptimizedPair(instruction, previousInstruction, offsetInPage)) {
			previousInstruction = 0;
			continue;
		}
		previousInstruction = instruction;
		
		BasicBlock* theBlock = GetBlock(offsetInPage);
		mBuilder.SetInsertPoint(theBlock);

		int theTestKind = instruction >> 28;
		if ((theTestKind != kTestAL) && (theTestKind != kTestNV))
		{
			// We have a real test.
			// Call this function recursively to produce next instruction basic block
			// This is required to skip the current instruction.
			// If we reached the end of page (or section to translate), we'll find
			// epilogue there anyway.
			BasicBlock* nextInstructionBlock = GetBlock(offsetInPage + 1);
			BasicBlock* thisInstructionBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
			DoTranslateTest(thisInstructionBlock, nextInstructionBlock, theTestKind);
			theBlock = thisInstructionBlock;
			// Continue from here.
			mBuilder.SetInsertPoint(theBlock);
		}
		
		if (theTestKind == kTestNV)
		{
			mBuilder.CreateBr(GetBlock(offsetInPage + 1));
		} else {
			switch ((instruction >> 26) & 0x3)				// 27 & 26
			{
				case 0x0:	// 00
					DoTranslate_00(offsetInPage, instruction);
					break;
					
				case 0x1:	// 01
					DoTranslate_01(offsetInPage, instruction);
					break;
					
				case 0x2:	// 10
					DoTranslate_10(offsetInPage, instruction);
					break;
					
				case 0x3:	// 11
					Translate_SWIAndCoproc(offsetInPage, instruction);
					break;
			} // switch 27 & 26
		}
	}
}

// -------------------------------------------------------------------------- //
//  * TranslateOptimizedPair(KUInt32, KUInt32, KUInt32)
// -------------------------------------------------------------------------- //
bool
TJITLLVMTranslator::FrameTranslator::TranslateOptimizedPair(KUInt32 instruction, KUInt32 previousInstruction, KUInt32 offsetInPage) {
	//     0xE35x00nn  cmp    rx, #n
	//     0x908FF10x  addls  pc, pc, rx, lsl #2
	if (
			((instruction & 0xFFFFFFF0) == 0x908FF100)
		&&  ((previousInstruction & 0xFFF0FF00) == 0xE3500000)
		&&  (((previousInstruction & 0x000F0000) >> 16) == (instruction & 0x0000000F))) {
		// Extract the number of cases from cmp instruction.
		KUInt32 nbCases = previousInstruction & 0x000000FF;
		// Make sure all cases are within current page.
		if ((offsetInPage + nbCases + 1) > mInstructionCount) {
			return false;
		}

		KUInt32 Rn = instruction & 0x0000000F;
		if (Rn == 15) {
			return false;
		}

		// Remove block from table, as a jump to addls instruction should yield different code.
		// Also make sure we have only one branch to this block.
		BasicBlock* theBlock = mLabels[offsetInPage];
		if (theBlock == nullptr) {
			return false;
		}
		if (theBlock->getSinglePredecessor() == nullptr) {
			return false;
		}
		mLabels[offsetInPage] = nullptr;

		// Build a switch.
		mBuilder.SetInsertPoint(theBlock);
		EnsureAllocated(&mRegisters[Rn], 32);
		Value* swValue = mBuilder.CreateLoad(mRegisters[Rn]);
		SwitchInst* sw = mBuilder.CreateSwitch(swValue, GetBlock(offsetInPage + 1), nbCases);
		for (unsigned caseIx = 0; caseIx <= nbCases; caseIx++) {
			sw->addCase(mBuilder.getInt32(caseIx), GetBlock(offsetInPage + caseIx + 2));
		}
		return true;
	}
	return false;
}

// -------------------------------------------------------------------------- //
//  * GetBlock(KUInt32)
// -------------------------------------------------------------------------- //
BasicBlock*
TJITLLVMTranslator::FrameTranslator::GetBlock(KUInt32 inOffsetInPage)
{
	// Check if this instruction is already translated.
	// Last element of the array is not null (it's epilogue BasicBlock)
	// and therefore we don't overrun the array with GetBlock(offset + 1).
	BasicBlock* theBlock = mLabels[inOffsetInPage];
	if (theBlock == nullptr) {
		mPending.push_back(inOffsetInPage);
		theBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
		mLabels[inOffsetInPage] = theBlock;
	}
	return theBlock;
}

// -------------------------------------------------------------------------- //
//  * AddFunctionIfRequired(KUInt32)
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::AddFunctionIfRequired(KUInt32 inOffsetInPage, bool inlining)
{
	if (inOffsetInPage < mInstructionCount) {
		BasicBlock* offsetBlock = GetBlock(inOffsetInPage);
		// Check if this function is already available.
		// If it isn't, add it to our new module functions (to finish it eventually)
		// and to the modules functions.
		if ((mStepFunction || !mPage.HasTranslatedFunction(inOffsetInPage))
			&& mNewFunctions.count(inOffsetInPage) == 0) {
			std::string functionName = mPage.FunctionName(inOffsetInPage);
			if (mStepFunction) {
				functionName = "s" + functionName;
			}
			Function* newFunction = Function::Create(GetEntryPointFuncType(), Function::ExternalLinkage, functionName, mModule);
			mNewFunctionsBlocks[inOffsetInPage] = offsetBlock;
			mNewFunctions[inOffsetInPage] = newFunction;
			
			auto argsIt = newFunction->arg_begin();
			Value* processor = argsIt++;
			Value* signal = argsIt;
			BasicBlock* functionBlock = BasicBlock::Create(mContext, ANONBLOCK, newFunction);
			auto ip = mBuilder.saveIP();
			mBuilder.SetInsertPoint(functionBlock);
			CallInst* returnValue = mBuilder.CreateCall3(mFunction, processor, signal, mBuilder.getInt32(inOffsetInPage));
			mBuilder.CreateRet(returnValue);
			mBuilder.restoreIP(ip);
			
			if (inlining) {
				mInlineCalls.push_back(returnValue);
			}
		}
	}
}

// -------------------------------------------------------------------------- //
//  * DoTranslateTest(BasicBlock*, BasicBlock*, int)
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::DoTranslateTest(
                BasicBlock* trueBlock,
                BasicBlock* falseBlock,
				int inTest)
{
	// Test the condition.
	switch (inTest)
	{
		// 0000 = EQ - Z set (equal)
		case kTestEQ:
			EnsureAllocated(&mCPSR_Z, 1);
			mBuilder.CreateCondBr(mBuilder.CreateLoad(mCPSR_Z), trueBlock, falseBlock);
			break;
			
			// 0001 = NE - Z clear (not equal)
		case kTestNE:
			EnsureAllocated(&mCPSR_Z, 1);
			mBuilder.CreateCondBr(mBuilder.CreateLoad(mCPSR_Z), falseBlock, trueBlock);
			break;
	
			// 0010 = CS - C set (unsigned higher or same)
		case kTestCS:
			EnsureAllocated(&mCPSR_C, 1);
			mBuilder.CreateCondBr(mBuilder.CreateLoad(mCPSR_C), trueBlock, falseBlock);
			break;
			
			// 0011 = CC - C clear (unsigned lower)
		case kTestCC:
			EnsureAllocated(&mCPSR_C, 1);
			mBuilder.CreateCondBr(mBuilder.CreateLoad(mCPSR_C), falseBlock, trueBlock);
			break;
			
			// 0100 = MI - N set (negative)
		case kTestMI:
			EnsureAllocated(&mCPSR_N, 1);
			mBuilder.CreateCondBr(mBuilder.CreateLoad(mCPSR_N), trueBlock, falseBlock);
			break;
			
			// 0101 = PL - N clear (positive or zero)
		case kTestPL:
			EnsureAllocated(&mCPSR_N, 1);
			mBuilder.CreateCondBr(mBuilder.CreateLoad(mCPSR_N), falseBlock, trueBlock);
			break;
			
			// 0110 = VS - V set (overflow)
		case kTestVS:
			EnsureAllocated(&mCPSR_V, 1);
			mBuilder.CreateCondBr(mBuilder.CreateLoad(mCPSR_V), trueBlock, falseBlock);
			break;
			
			// 0111 = VC - V clear (no overflow)
		case kTestVC:
			EnsureAllocated(&mCPSR_V, 1);
			mBuilder.CreateCondBr(mBuilder.CreateLoad(mCPSR_V), falseBlock, trueBlock);
			break;
			
			// 1000 = HI - C set and Z clear (unsigned higher)
		case kTestHI:
			EnsureAllocated(&mCPSR_C, 1);
			EnsureAllocated(&mCPSR_Z, 1);
			mBuilder.CreateCondBr(mBuilder.CreateAnd(mBuilder.CreateLoad(mCPSR_C), mBuilder.CreateNot(mBuilder.CreateLoad(mCPSR_Z))), trueBlock, falseBlock);
			break;
			
			// 1001 = LS - C clear or Z set (unsigned lower or same)
		case kTestLS:
			EnsureAllocated(&mCPSR_C, 1);
			EnsureAllocated(&mCPSR_Z, 1);
			mBuilder.CreateCondBr(mBuilder.CreateAnd(mBuilder.CreateLoad(mCPSR_C), mBuilder.CreateNot(mBuilder.CreateLoad(mCPSR_Z))), falseBlock, trueBlock);
			break;
			
			// 1010 = GE - N set and V set, or N clear and V clear (greater or equal)
		case kTestGE:
			EnsureAllocated(&mCPSR_N, 1);
			EnsureAllocated(&mCPSR_V, 1);
			mBuilder.CreateCondBr(mBuilder.CreateXor(mBuilder.CreateLoad(mCPSR_N), mBuilder.CreateLoad(mCPSR_V)), falseBlock, trueBlock);
			break;
			
			// 1011 = LT - N set and V clear, or N clear and V set (less than)
		case kTestLT:
			EnsureAllocated(&mCPSR_N, 1);
			EnsureAllocated(&mCPSR_V, 1);
			mBuilder.CreateCondBr(mBuilder.CreateXor(mBuilder.CreateLoad(mCPSR_N), mBuilder.CreateLoad(mCPSR_V)), trueBlock, falseBlock);
			break;
			
			// 1100 = GT - Z clear, and either N set and V set, or N clear and V clear (greater than)
		case kTestGT:
			EnsureAllocated(&mCPSR_Z, 1);
			EnsureAllocated(&mCPSR_N, 1);
			EnsureAllocated(&mCPSR_V, 1);
			mBuilder.CreateCondBr(mBuilder.CreateOr(
						mBuilder.CreateXor(mBuilder.CreateLoad(mCPSR_N), mBuilder.CreateLoad(mCPSR_V)),
						mBuilder.CreateLoad(mCPSR_Z)), falseBlock, trueBlock);
			break;
			
			// 1101 = LE - Z set, or N set and V clear, or N clear and V set (less than or equal)
		case kTestLE:
			EnsureAllocated(&mCPSR_Z, 1);
			EnsureAllocated(&mCPSR_N, 1);
			EnsureAllocated(&mCPSR_V, 1);
			mBuilder.CreateCondBr(mBuilder.CreateOr(
													mBuilder.CreateXor(mBuilder.CreateLoad(mCPSR_N), mBuilder.CreateLoad(mCPSR_V)),
													mBuilder.CreateLoad(mCPSR_Z)), trueBlock, falseBlock);
			break;
			
			// 1110 = AL - always
		case kTestAL:
			mBuilder.CreateBr(trueBlock);
			
			// 1111 = NV - never
		case kTestNV:
			mBuilder.CreateBr(falseBlock);
		default:
			break;
	}
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_00( KUInt32, KUInt32, KUInt32*, KUInt32, BasicBlock**, inInstruction )
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::DoTranslate_00(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  I  --Opcode--- S  --Rn--- --Rd--- ----------Operand 2-------- Data Processing
	// -Cond-- 0  0  0  1  0  R  0  0  --SBO-- --Rd--- -----------SBZ------------- MRS
	// -Cond-- 0  0  I  1  0  R  1  0  -field- --SBO-- ----------Operand---------- MSR
	// -Cond-- 0  0  0  0  0  0  A  S  --Rd--- --Rn--- --Rs--- 1  0  0  1  --Rm--- Multiply
	// -Cond-- 0  0  0  1  0  B  0  0  --Rn--- --Rd--- 0 0 0 0 1  0  0  1  --Rm--- Single Data Swap
	if ((inInstruction & 0x020000F0) == 0x90)	// I=0 & 0b1001----
	{
		if (inInstruction & 0x01000000)
		{
			// Single Data Swap
			Translate_SingleDataSwap(offsetInPage, inInstruction);
		} else {
			// Multiply
			if (inInstruction & 0x00200000)
			{
				Translate_MultiplyAndAccumulate(offsetInPage, inInstruction);
			} else {
				Translate_Multiply(offsetInPage, inInstruction);
			}
		}
	} else if ((inInstruction & 0x03BF0FFF) == 0x010F0000) {
	    Translate_MRS(offsetInPage, inInstruction);
	} else if ((inInstruction & 0x0FF000F0) == 0x01200070) {
		Translate_BKPT(offsetInPage, inInstruction);
	} else if ((inInstruction & 0x01B0F000) == 0x0120F000) {
	    Translate_MSR(offsetInPage, inInstruction);
	} else {
		Translate_DataProcessing(offsetInPage, inInstruction);
	}
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_01( JITUnit*, KUInt32, KUInt32, KUInt32, JITFuncPtr* )
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::DoTranslate_01(KUInt32 offsetInPage, KUInt32 inInstruction) {
	// Single Data Transfer & Undefined
	if ((inInstruction & 0x02000010) == 0x02000010)
	{
// -Cond-- 0  1  1  -XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX- 1  -XXXXX-
		// DebuggerUND doesn't read registers.
		// Yet, we'll save them to prepare exit in both cases.
		// PC is used to fetch the instruction and process it further.
		KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
		if (inInstruction == 0xE6000510) {
    		BuildExitToFunction("JIT_DebuggerUND", exitPC);
		} else {
    		BuildExitToFunction("JIT_UndefinedInstruction", exitPC);
		}
	} else {
// -Cond-- 0  1  I  P  U  B  W  L  --Rn--- --Rd--- -----------offset----------
		Translate_SingleDataTransfer(offsetInPage, inInstruction);
	}
}

// -------------------------------------------------------------------------- //
//  * DoTranslate_10( JITUnit*, KUInt32, KUInt32, JITFuncPtr* )
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::DoTranslate_10(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer
	// -Cond-- 1  0  1  L  ---------------------offset---------------------------- Branch
	if (inInstruction & 0x02000000)
	{
		Translate_Branch(offsetInPage, inInstruction);
	} else if ((inInstruction & 0x000F0000) == 0x000F0000) {
		// Use of R15 as Rn for LDM/STM is UNPREDICTABLE.
		KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
		BuildExitToFunction("JIT_UndefinedInstruction", exitPC);
	} else if ((inInstruction & 0x00500000) == 0x00100000) {
		Translate_LDM13(offsetInPage, inInstruction);
	} else if ((inInstruction & 0x00708000) == 0x00500000) {
		Translate_LDM2(offsetInPage, inInstruction);	// PC is not in register list
	} else if ((inInstruction & 0x00508000) == 0x00508000) {
		Translate_LDM13(offsetInPage, inInstruction);	// PC is in register list
	} else if ((inInstruction & 0x00500000) == 0x00000000) {
		Translate_STM1(offsetInPage, inInstruction);
	} else if ((inInstruction & 0x00700000) == 0x00400000) {
		Translate_STM2(offsetInPage, inInstruction);
	} else {
		KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
		BuildExitToFunction("JIT_UndefinedInstruction", exitPC);		
	}
}

// -------------------------------------------------------------------------- //
//  * Translate_SWIAndCoproc
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_SWIAndCoproc(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// -Cond-- 1  1  0  P  U  N  W  L  --Rn--- --CRd-- --CP#-- -----offset-------- Coproc Data Transfer
	// -Cond-- 1  1  1  0  --CP Opc--- --CRn-- --CRd-- --CP#-- ---CP--- 0  --CRm-- Coproc Data Operation
	// -Cond-- 1  1  1  0  -CPOpc-- L  --CRn-- --Rd--- --CP#-- ---CP--- 1  --CRm-- Coproc Register Transfer
	// -Cond-- 1  1  1  1  -----------------ignored by processor------------------ Software Interrupt
	// Extension for native calls:
	// -Cond-- 1  1  1  1  1  0  ---------------------index----------------------- Call Native Code at Index
	// -Cond-- 1  1  1  1  1  1  ---------------------index----------------------- Call Injection at Index
	
	// For some of these instructions, PC is used to fetch the instruction and process it further.
	KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
	if (inInstruction & 0x02000000)
	{
		if (inInstruction & 0x01000000)
		{
			// SWI.
        	BuildExitToFunction("JIT_SWI", exitPC);
		} else {
			if (inInstruction & 0x00000010)
			{
                KUInt32 CPNumber = (inInstruction & 0x00000F00) >> 8;
                if (CPNumber == 0xF) {
            		BuildExitToFunction("JIT_SystemCoprocRegisterTransfer", exitPC);
                } else if (CPNumber == 10) {
                    // Native primitives.
            		BuildExitToFunction("JIT_NativeCoprocRegisterTransfer", exitPC);
                } else {
            		BuildExitToFunction("JIT_UndefinedInstruction", exitPC);
                }
			} else {
                // All CP15 CDP operations are undefined.
                // And we don't have another coproc anyway.
            	BuildExitToFunction("JIT_UndefinedInstruction", exitPC);
			}
		}
	} else {
        // All CP15 LDC and STC operations are undefined.
        // And we don't have another coproc anyway.
        BuildExitToFunction("JIT_UndefinedInstruction", exitPC);
	}
	// We might return from there (typically for SWI, coproc or FP emulation).
	AddFunctionIfRequired(offsetInPage + 1, false);
}

// -------------------------------------------------------------------------- //
//  * Translate_SingleDataSwap
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_SingleDataSwap(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  0  1  0  B  0  0  --Rn--- --Rd--- 0 0 0 0 1  0  0  1  --Rm--- Single Data Swap
	const KUInt32 flag_b = (inInstruction & 0x00400000);
	const KUInt32 Rn = (inInstruction & 0x000F0000) >> 16;
	const KUInt32 Rd = (inInstruction & 0x0000F000) >> 12;
	const KUInt32 Rm = inInstruction & 0x0000000F;

	KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
	// Use of R15 If R15 is specified for <Rd>, <Rn>, or <Rm>, the result is UNPREDICTABLE.
	if ((Rd == 15) || (Rn == 15) || (Rm == 15))
	{
		BuildExitToFunction("JIT_UndefinedInstruction", exitPC);
	} else {
		EnsureAllocated(&mRegisters[Rd], 32);
		EnsureAllocated(&mRegisters[Rn], 32);
		EnsureAllocated(&mRegisters[Rm], 32);
		Value* address = mBuilder.CreateLoad(mRegisters[Rn]);
		BasicBlock* dataAbortExit = BasicBlock::Create(mContext, BLOCKNAME("dataabort"), mFunction);
		BasicBlock* writeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
		BasicBlock* storeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
		BasicBlock* signalExit = BasicBlock::Create(mContext, BLOCKNAME("signalexit"), mFunction);
		
		if (flag_b) {
			Function* readFunction = EnsureFunction(GetReadBFuncType(), "JIT_ReadB");
			Value* byte = mBuilder.CreateAlloca(Type::getInt8Ty(mContext));
			Value* result = mBuilder.CreateCall3(readFunction, GetMemory(), address, byte);
			mBuilder.CreateCondBr(result, dataAbortExit, writeBlock);
			
			mBuilder.SetInsertPoint(writeBlock);
			Function* writeFunction = EnsureFunction(GetWriteBFuncType(), "JIT_WriteB");
			result = mBuilder.CreateCall3(
								writeFunction,
								GetMemory(),
								address,
								mBuilder.CreateTrunc(mBuilder.CreateLoad(mRegisters[Rm]),
													   Type::getInt8Ty(mContext)));
			mBuilder.CreateCondBr(result, dataAbortExit, storeBlock);
			
			mBuilder.SetInsertPoint(storeBlock);
			mBuilder.CreateStore(mBuilder.CreateZExt(mBuilder.CreateLoad(byte), Type::getInt32Ty(mContext)), mRegisters[Rd]);
		} else {
			Function* readFunction = EnsureFunction(GetReadFuncType(), "JIT_Read");
			Value* word = mBuilder.CreateAlloca(Type::getInt32Ty(mContext));
			Value* result = mBuilder.CreateCall3(readFunction, GetMemory(), address, word);
			mBuilder.CreateCondBr(result, dataAbortExit, writeBlock);
			
			mBuilder.SetInsertPoint(writeBlock);
			Function* writeFunction = EnsureFunction(GetWriteFuncType(), "JIT_Write");
			result = mBuilder.CreateCall3(
								 writeFunction,
								 GetMemory(),
								 address,
								 mBuilder.CreateLoad(mRegisters[Rm]));
			mBuilder.CreateCondBr(result, dataAbortExit, storeBlock);
			
			mBuilder.SetInsertPoint(storeBlock);
			mBuilder.CreateStore(mBuilder.CreateLoad(word), mRegisters[Rd]);
		}
		// Next instruction
		BasicBlock* nextInstructionBlock = GetBlock(offsetInPage + 1);
		// Because reading or writing could cause an interrupt, we check it now.
		mBuilder.CreateCondBr(mBuilder.CreateLoad(mSignal, true), nextInstructionBlock, signalExit);

		mBuilder.SetInsertPoint(dataAbortExit);
		BuildExitToFunction("JIT_DataAbort", exitPC);

		mBuilder.SetInsertPoint(signalExit);
		mBuilder.CreateStore(mBuilder.getInt32(exitPC), mPC);
		mBuilder.CreateBr(mMainExit);

		// We might return from a DataAbort or a signal.
		AddFunctionIfRequired(offsetInPage + 1, false);
	}
}

// -------------------------------------------------------------------------- //
//  * Translate_MultiplyAndAccumulate
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_MultiplyAndAccumulate(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  0  0  0  0  A  S  --Rd--- --Rn--- --Rs--- 1  0  0  1  --Rm--- Multiply
	
	const KUInt32 flag_s = (inInstruction & 0x00100000);
	const KUInt32 Rd = (inInstruction & 0x000F0000) >> 16;
	const KUInt32 Rn = (inInstruction & 0x0000F000) >> 12;
	const KUInt32 Rs = (inInstruction & 0x00000F00) >> 8;
	const KUInt32 Rm = inInstruction & 0x0000000F;
	if ((Rd == 15) || (Rn == 15) || (Rs == 15) || (Rm == 15))
	{
		KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
		BuildExitToFunction("JIT_UndefinedInstruction", exitPC);
	} else {
		EnsureAllocated(&mRegisters[Rd], 32);
		EnsureAllocated(&mRegisters[Rn], 32);
		EnsureAllocated(&mRegisters[Rs], 32);
		EnsureAllocated(&mRegisters[Rm], 32);
		
		Value* result = mBuilder.CreateAdd(
							mBuilder.CreateMul(mBuilder.CreateLoad(mRegisters[Rm]),
											   mBuilder.CreateLoad(mRegisters[Rs])),
							mBuilder.CreateLoad(mRegisters[Rn]));
		mBuilder.CreateStore(result,mRegisters[Rd]);
		if (flag_s) {
			EnsureAllocated(&mCPSR_N, 1);
			EnsureAllocated(&mCPSR_Z, 1);
			mBuilder.CreateStore(mBuilder.CreateICmpSLT(result, mBuilder.getInt32(0)), mCPSR_N);
			mBuilder.CreateStore(mBuilder.CreateICmpEQ(result, mBuilder.getInt32(0)), mCPSR_Z);
		}
		
		// Next instruction
		mBuilder.CreateBr(GetBlock(offsetInPage + 1));
	}
}

// -------------------------------------------------------------------------- //
//  * Translate_Multiply
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_Multiply(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  0  0  0  0  A  S  --Rd--- --Rn--- --Rs--- 1  0  0  1  --Rm--- Multiply

	const KUInt32 flag_s = (inInstruction & 0x00100000);
	const KUInt32 Rd = (inInstruction & 0x000F0000) >> 16;
	const KUInt32 Rs = (inInstruction & 0x00000F00) >> 8;
	const KUInt32 Rm = inInstruction & 0x0000000F;
	if ((Rd == 15) || (Rs == 15) || (Rm == 15))
	{
		KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
		BuildExitToFunction("JIT_UndefinedInstruction", exitPC);
	} else {
		EnsureAllocated(&mRegisters[Rd], 32);
		EnsureAllocated(&mRegisters[Rs], 32);
		EnsureAllocated(&mRegisters[Rm], 32);

		Value* result = mBuilder.CreateMul(mBuilder.CreateLoad(mRegisters[Rm]), mBuilder.CreateLoad(mRegisters[Rs]));
		mBuilder.CreateStore(result,mRegisters[Rd]);
		if (flag_s) {
			EnsureAllocated(&mCPSR_N, 1);
			EnsureAllocated(&mCPSR_Z, 1);
			mBuilder.CreateStore(mBuilder.CreateICmpSLT(result, mBuilder.getInt32(0)), mCPSR_N);
			mBuilder.CreateStore(mBuilder.CreateICmpEQ(result, mBuilder.getInt32(0)), mCPSR_Z);
		}

		// Next instruction
		mBuilder.CreateBr(GetBlock(offsetInPage + 1));
	}
}

// -------------------------------------------------------------------------- //
//  * Translate_DataProcessing
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_DataProcessing(KUInt32 offsetInPage, KUInt32 inInstruction) {
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  I  --Opcode--- S  --Rn--- --Rd--- ----------Operand 2-------- Data Processing
	const KUInt32 flag_s = inInstruction & 0x00100000;
	const KUInt32 Rd = (inInstruction & 0x0000F000) >> 12;
	const KUInt32 Rn = (inInstruction & 0x000F0000) >> 16;

	// Data-processing operands
	Value* shifterOperand;
	Value* shifterCarryOut;
	
	Translate_DataProcessing_Shifter(offsetInPage, inInstruction, shifterOperand, shifterCarryOut);

	Value* operand;
	if (Rn == 15) {
		operand = mBuilder.getInt32((offsetInPage + 2) * 4 + mCurrentVAddress);
	} else {
		EnsureAllocated(&mRegisters[Rn], 32);
		operand = mBuilder.CreateLoad(mRegisters[Rn]);
	}
	
	if (Rd != 15) {
		EnsureAllocated(&mRegisters[Rd], 32);
	}

    Value* carry = nullptr;
    Value* overflow = nullptr;
	Value* result = nullptr;
	
	Translate_DataProcessing_Op(inInstruction, operand, shifterOperand, shifterCarryOut, carry, overflow, result);
	
	if (flag_s) {
	    if (Rd == 15) {
	        // Try CPSR = SPSR
	        // Result is UNPREDICTABLE if not in a mode with SPSR.
			Value* mode = BuildGetEmulationMode();
	        Value* cpsr = BuildGetSPSR(mode);
	        BuildSetCPSR(cpsr, mode, mBuilder.getInt1(true), mBuilder.getInt1(true), mBuilder.CreateAdd(result, mBuilder.getInt32(4)));
	    } else {
	        if (overflow != nullptr) {
	            EnsureAllocated(&mCPSR_V, 1);
	            mBuilder.CreateStore(overflow, mCPSR_V);
	        }
            EnsureAllocated(&mCPSR_C, 1);
            mBuilder.CreateStore(carry, mCPSR_C);
            EnsureAllocated(&mCPSR_N, 1);
            mBuilder.CreateStore(mBuilder.CreateICmpSLT(result, mBuilder.getInt32(0)), mCPSR_N);
            EnsureAllocated(&mCPSR_Z, 1);
            mBuilder.CreateStore(mBuilder.CreateICmpEQ(result, mBuilder.getInt32(0)), mCPSR_Z);
	    }
	}
	
	if (Rd == 15) {
		mBuilder.CreateStore(mBuilder.CreateAdd(result, mBuilder.getInt32(4)), mPC);
		mBuilder.CreateBr(mMainExit);
	} else {
		// Next instruction
        mBuilder.CreateBr(GetBlock(offsetInPage + 1));
	}
}

// -------------------------------------------------------------------------- //
//  * Translate_DataProcessing_AddressingMode
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_DataProcessing_Shifter(KUInt32 offsetInPage, KUInt32 inInstruction, Value*& shifterOperand, Value*& shifterCarryOut) {
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  I  --Opcode--- S  --Rn--- --Rd--- ----------Operand 2-------- Data Processing
	const KUInt32 flag_i = inInstruction & 0x02000000;
	
	if (flag_i) {
		// Immediate.
		KUInt32 theImmValue = inInstruction & 0xFF;
		KUInt32 theRotateAmount = ((inInstruction >> 8) & 0xF) * 2;
		if (theRotateAmount != 0)
		{
			theImmValue =
			(theImmValue >> theRotateAmount)
			| (theImmValue << (32 - theRotateAmount));
		}
		shifterOperand = mBuilder.getInt32(theImmValue);
		if (theRotateAmount == 0) {
			EnsureAllocated(&mCPSR_C, 1);
			shifterCarryOut = mBuilder.CreateLoad(mCPSR_C);
		} else {
			shifterCarryOut = mBuilder.getInt1((theImmValue & 0x80000000) != 0);
		}
	} else if ((inInstruction & 0x00000070) == 0) {
		// Register
		// & Logical shift left by immediate
		const KUInt32 shiftImm = (inInstruction & 0x00000F80) >> 7;
		const KUInt32 Rm = (inInstruction & 0x0000000F);
		if (Rm == 15) {
			shifterOperand = mBuilder.getInt32((offsetInPage + 2) * 4 + mCurrentVAddress);
		} else {
			EnsureAllocated(&mRegisters[Rm], 32);
			shifterOperand = mBuilder.CreateLoad(mRegisters[Rm]);
		}
		shifterOperand = mBuilder.CreateShl(shifterOperand, mBuilder.getInt32(shiftImm));
		if (shiftImm == 0) {
			EnsureAllocated(&mCPSR_C, 1);
			shifterCarryOut = mBuilder.CreateLoad(mCPSR_C);
		} else {
			shifterCarryOut = mBuilder.CreateTrunc(mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(32 - shiftImm)), IntegerType::getInt1Ty(mContext));
		}
	} else if ((inInstruction & 0x000000F0) == 0x00000010) {
		// Logical shift left by register.
		// Specifying R15 as register Rd, register Rm, register Rn, or register Rs has UNPREDICTABLE results.
		const KUInt32 Rs = (inInstruction & 0x00000F00) >> 8;
		const KUInt32 Rm = (inInstruction & 0x0000000F);
		Value* rsValue;
		if (Rs == 15) {
			rsValue = mBuilder.getInt32(0);
		} else {
			EnsureAllocated(&mRegisters[Rs], 32);
			rsValue = mBuilder.CreateAnd(mBuilder.CreateLoad(mRegisters[Rs]), mBuilder.getInt32(0x000000FF));
		}
		Value* rmValue;
		if (Rm == 15) {
			rmValue = mBuilder.getInt32(0);
		} else {
			EnsureAllocated(&mRegisters[Rm], 32);
			rmValue = mBuilder.CreateLoad(mRegisters[Rm]);
		}
		// if rs == 0 then shifter_operand = Rm
		shifterOperand = mBuilder.CreateSelect(
											   mBuilder.CreateICmpEQ(rsValue, mBuilder.getInt32(0)),
											   rmValue,
											   mBuilder.CreateSelect(
																	 mBuilder.CreateICmpUGE(rsValue, mBuilder.getInt32(32)),
																	 // if rs >= 32 then shifter_operand = 0
																	 mBuilder.getInt32(0),
																	 // else (rs < 32) shifter_operand = Rm LSL Rs
																	 mBuilder.CreateShl(rmValue, rsValue)));
		
		EnsureAllocated(&mCPSR_C, 1);
		// if rs == 0 then shifter_carry_out = C Flag
		shifterCarryOut = mBuilder.CreateSelect(
												mBuilder.CreateICmpEQ(rsValue, mBuilder.getInt32(0)),
												mBuilder.CreateLoad(mCPSR_C),
												// if rs <= 32 then shifter_carry_out = Rm[32 - Rs]
												mBuilder.CreateSelect(
																	  mBuilder.CreateICmpULE(rsValue, mBuilder.getInt32(32)),
																	  mBuilder.CreateTrunc(mBuilder.CreateLShr(shifterOperand, mBuilder.CreateSub(mBuilder.getInt32(32), rsValue)), IntegerType::getInt1Ty(mContext)),
																	  //else, shifter_carry_out = 0
																	  mBuilder.getInt1(0)));
	} else if ((inInstruction & 0x00000070) == 0x00000020) {
		// Logical shift right by immediate
		const KUInt32 shiftImm = (inInstruction & 0x00000F80) >> 7;
		const KUInt32 Rm = (inInstruction & 0x0000000F);
		if (Rm == 15) {
			shifterOperand = mBuilder.getInt32((offsetInPage + 2) * 4 + mCurrentVAddress);
		} else {
			EnsureAllocated(&mRegisters[Rm], 32);
			shifterOperand = mBuilder.CreateLoad(mRegisters[Rm]);
		}
		if (shiftImm == 0) {
			shifterCarryOut = mBuilder.CreateTrunc(mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(31)), IntegerType::getInt1Ty(mContext));
			shifterOperand = mBuilder.getInt32(0);
		} else {
			shifterCarryOut = mBuilder.CreateTrunc(mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(shiftImm - 1)), IntegerType::getInt1Ty(mContext));
			shifterOperand = mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(shiftImm));
		}
	} else if ((inInstruction & 0x000000F0) == 0x00000030) {
		// Logical shift right by register
		// Specifying R15 as register Rd, register Rm, register Rn, or register Rs has UNPREDICTABLE results.
		const KUInt32 Rs = (inInstruction & 0x00000F00) >> 8;
		const KUInt32 Rm = (inInstruction & 0x0000000F);
		Value* rsValue;
		if (Rs == 15) {
			rsValue = mBuilder.getInt32(0);
		} else {
			EnsureAllocated(&mRegisters[Rs], 32);
			rsValue = mBuilder.CreateAnd(mBuilder.CreateLoad(mRegisters[Rs]), mBuilder.getInt32(0x000000FF));
		}
		Value* rmValue;
		if (Rm == 15) {
			rmValue = mBuilder.getInt32(0);
		} else {
			EnsureAllocated(&mRegisters[Rm], 32);
			rmValue = mBuilder.CreateLoad(mRegisters[Rm]);
		}
		// if rs == 0 then shifter_operand = Rm
		shifterOperand = mBuilder.CreateSelect(
											   mBuilder.CreateICmpEQ(rsValue, mBuilder.getInt32(0)),
											   rmValue,
											   mBuilder.CreateSelect(
																	 mBuilder.CreateICmpUGE(rsValue, mBuilder.getInt32(32)),
																	 // if rs >= 32 then shifter_operand = 0
		                mBuilder.getInt32(0),
																	 // else (rs < 32) shifter_operand = Rm LSR Rs
																	 mBuilder.CreateLShr(rmValue, rsValue)));
		
		EnsureAllocated(&mCPSR_C, 1);
		// if rs == 0 then shifter_carry_out = C Flag
		shifterCarryOut = mBuilder.CreateSelect(
												mBuilder.CreateICmpEQ(rsValue, mBuilder.getInt32(0)),
												mBuilder.CreateLoad(mCPSR_C),
												// if rs <= 32 then shifter_carry_out = Rm[Rs - 1]
												mBuilder.CreateSelect(
																	  mBuilder.CreateICmpULE(rsValue, mBuilder.getInt32(32)),
																	  mBuilder.CreateTrunc(mBuilder.CreateLShr(shifterOperand, mBuilder.CreateSub(rsValue, mBuilder.getInt32(1))), IntegerType::getInt1Ty(mContext)),
																	  // else, shifter_carry_out = 0
																	  mBuilder.getInt1(0)));
	} else if ((inInstruction & 0x00000070) == 0x00000040) {
		// Arithmetic shift right by immediate
		const KUInt32 shiftImm = (inInstruction & 0x00000F80) >> 7;
		const KUInt32 Rm = (inInstruction & 0x0000000F);
		if (Rm == 15) {
			shifterOperand = mBuilder.getInt32((offsetInPage + 2) * 4 + mCurrentVAddress);
		} else {
			EnsureAllocated(&mRegisters[Rm], 32);
			shifterOperand = mBuilder.CreateLoad(mRegisters[Rm]);
		}
		if (shiftImm == 0) {
			shifterCarryOut = mBuilder.CreateTrunc(mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(31)), IntegerType::getInt1Ty(mContext));
			shifterOperand = mBuilder.CreateSelect(shifterCarryOut, mBuilder.getInt32(0xFFFFFFFF), mBuilder.getInt32(0));
		} else {
			shifterCarryOut = mBuilder.CreateTrunc(mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(shiftImm - 1)), IntegerType::getInt1Ty(mContext));
			shifterOperand = mBuilder.CreateAShr(shifterOperand, mBuilder.getInt32(shiftImm));
		}
	} else if ((inInstruction & 0x000000F0) == 0x00000050) {
		// Arithmetic shift right by register
		// Specifying R15 as register Rd, register Rm, register Rn, or register Rs has UNPREDICTABLE results.
		const KUInt32 Rs = (inInstruction & 0x00000F00) >> 8;
		const KUInt32 Rm = (inInstruction & 0x0000000F);
		Value* rsValue;
		if (Rs == 15) {
			rsValue = mBuilder.getInt32(0);
		} else {
			EnsureAllocated(&mRegisters[Rs], 32);
			rsValue = mBuilder.CreateAnd(mBuilder.CreateLoad(mRegisters[Rs]), mBuilder.getInt32(0x000000FF));
		}
		Value* rmValue;
		if (Rm == 15) {
			rmValue = mBuilder.getInt32(0);
		} else {
			EnsureAllocated(&mRegisters[Rm], 32);
			rmValue = mBuilder.CreateLoad(mRegisters[Rm]);
		}
		// if Rs[7:0] == 0 then
		//     shifter_operand = Rm
		//     shifter_carry_out = C Flag
		// else if Rs[7:0] < 32 then
		//      shifter_operand = Rm Arithmetic_Shift_Right Rs[7:0]
		//      shifter_carry_out = Rm[Rs[7:0] - 1]
		// else /* Rs[7:0] >= 32 */
		//      if Rm[31] == 0 then
		//          shifter_operand = 0
		//          shifter_carry_out = Rm[31]
		//      else /* Rm[31] == 1 */
		//          shifter_operand = 0xFFFFFFFF
		//          shifter_carry_out = Rm[31]
		
		// ARM ARM explains that if rs >= 32 then
		// shifter_operand = -1 or 0 depending on sign of Rm.
		// LLVM ASR is just doing this.
		shifterOperand =
		mBuilder.CreateAShr(rmValue,
							mBuilder.CreateSelect(
		            mBuilder.CreateICmpUGE(rsValue, mBuilder.getInt32(32)),
												  mBuilder.getInt32(31),
												  rsValue));
		
		EnsureAllocated(&mCPSR_C, 1);
		// if rs == 0 then shifter_carry_out = C Flag
		shifterCarryOut = mBuilder.CreateSelect(
												mBuilder.CreateICmpEQ(rsValue, mBuilder.getInt32(0)),
												mBuilder.CreateLoad(mCPSR_C),
												mBuilder.CreateTrunc(
																	 mBuilder.CreateLShr(rmValue,
																						 mBuilder.CreateSelect(
																											   mBuilder.CreateICmpULE(rsValue, mBuilder.getInt32(32)),
																											   mBuilder.CreateSub(rsValue, mBuilder.getInt32(1)),
																											   mBuilder.getInt32(31))),
																	 IntegerType::getInt1Ty(mContext)));
	} else if ((inInstruction & 0x00000070) == 0x00000060) {
		// Rotate right by immediate
		// Rotate right with extend
		const KUInt32 shiftImm = (inInstruction & 0x00000F80) >> 7;
		const KUInt32 Rm = (inInstruction & 0x0000000F);
		if (Rm == 15) {
			shifterOperand = mBuilder.getInt32((offsetInPage + 2) * 4 + mCurrentVAddress);
		} else {
			EnsureAllocated(&mRegisters[Rm], 32);
			shifterOperand = mBuilder.CreateLoad(mRegisters[Rm]);
		}
		if (shiftImm == 0) {
			// Rotate right with extend
			shifterCarryOut = mBuilder.CreateTrunc(shifterOperand, IntegerType::getInt1Ty(mContext));
			EnsureAllocated(&mCPSR_C, 1);
			shifterOperand = mBuilder.CreateOr(
											   mBuilder.CreateShl(mBuilder.CreateZExt(mBuilder.CreateLoad(mCPSR_C), IntegerType::getInt32Ty(mContext)), mBuilder.getInt32(31)),
											   mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(1)));
		} else {
			shifterCarryOut = mBuilder.CreateTrunc(mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(shiftImm - 1)), IntegerType::getInt1Ty(mContext));
			shifterOperand = mBuilder.CreateOr(
											   mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(shiftImm)),
											   mBuilder.CreateShl(shifterOperand, mBuilder.getInt32(32 - shiftImm)));
		}
	} else /* ((inInstruction & 0x000000F0) == 0x00000050) */ {
		// Rotate right by register
		// Specifying R15 as register Rd, register Rm, register Rn, or register Rs has UNPREDICTABLE results.
		const KUInt32 Rs = (inInstruction & 0x00000F00) >> 8;
		const KUInt32 Rm = (inInstruction & 0x0000000F);
		Value* rsValue;
		if (Rs == 15) {
			rsValue = mBuilder.getInt32(0);
		} else {
			EnsureAllocated(&mRegisters[Rs], 32);
			rsValue = mBuilder.CreateAnd(mBuilder.CreateLoad(mRegisters[Rs]), mBuilder.getInt32(0x000000FF));
		}
		Value* rmValue;
		if (Rm == 15) {
			rmValue = mBuilder.getInt32(0);
		} else {
			EnsureAllocated(&mRegisters[Rm], 32);
			rmValue = mBuilder.CreateLoad(mRegisters[Rm]);
		}
		Value* rsValue32 = mBuilder.CreateAnd(mBuilder.CreateLoad(mRegisters[Rs]), mBuilder.getInt32(0x0000001F));
		
		// if Rs[7:0] == 0 then
		//     shifter_operand = Rm
		//     shifter_carry_out = C Flag
		// else if Rs[4:0] == 32 then
		//      shifter_operand = Rm
		//      shifter_carry_out = Rm[31]
		// else /* Rs[4:0] > 32 */
		//      shifter_operand = Rm Rotate_Right Rs[4:0]
		//      shifter_carry_out = Rm[Rs[4:0] - 1]
		
		shifterOperand =
		mBuilder.CreateSelect(
							  mBuilder.CreateICmpEQ(rsValue32, mBuilder.getInt32(0)),
							  rmValue,
							  mBuilder.CreateOr(
												mBuilder.CreateLShr(rmValue, rsValue32),
												mBuilder.CreateShl(rmValue, mBuilder.CreateSub(mBuilder.getInt32(32), rsValue32))));
		EnsureAllocated(&mCPSR_C, 1);
		shifterCarryOut =
		mBuilder.CreateSelect(
							  mBuilder.CreateICmpEQ(rsValue, mBuilder.getInt32(0)),
							  mBuilder.CreateLoad(mCPSR_C),
							  mBuilder.CreateTrunc(mBuilder.CreateLShr(rmValue,
                    mBuilder.CreateSelect(
										  mBuilder.CreateICmpEQ(rsValue32, mBuilder.getInt32(0)),
										  mBuilder.getInt32(31),
										  mBuilder.CreateSub(rsValue32, mBuilder.getInt32(1)))), IntegerType::getInt1Ty(mContext)));
	}
}

// -------------------------------------------------------------------------- //
//  * Translate_DataProcessing
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_DataProcessing_Op(KUInt32 inInstruction, Value* operand, Value* shifterOperand, Value* shifterCarryOut, Value*& carry, Value*& overflow, Value*& result) {
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  I  --Opcode--- S  --Rn--- --Rd--- ----------Operand 2-------- Data Processing
	const KUInt32 Rd = (inInstruction & 0x0000F000) >> 12;
	Function* intrinsic;
	Value* compResult;
	
	switch ((inInstruction & 0x01E00000) >> 21)
	{
		case 0x0:	// 0b0000
			// AND
			result = mBuilder.CreateAnd(operand, shifterOperand);
			carry = shifterCarryOut;
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0x1:	// 0b0001
			// EOR
			result = mBuilder.CreateXor(operand, shifterOperand);
			carry = shifterCarryOut;
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0x2:	// 0b0010
			// SUB
			intrinsic = EnsureIntrinsic(Intrinsic::ssub_with_overflow);
			compResult = mBuilder.CreateCall2(intrinsic, operand, shifterOperand);
			result = mBuilder.CreateExtractValue(compResult, 0);
			overflow = mBuilder.CreateExtractValue(compResult, 1);
			carry = mBuilder.CreateICmpUGE(operand, shifterOperand);
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0x3:	// 0b0011
			// RSB
			intrinsic = EnsureIntrinsic(Intrinsic::ssub_with_overflow);
			compResult = mBuilder.CreateCall2(intrinsic, shifterOperand, operand);
			result = mBuilder.CreateExtractValue(compResult, 0);
			overflow = mBuilder.CreateExtractValue(compResult, 1);
			carry = mBuilder.CreateICmpUGE(shifterOperand, operand);
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0x4:	// 0b01000
			// ADD
			intrinsic = EnsureIntrinsic(Intrinsic::sadd_with_overflow);
			compResult = mBuilder.CreateCall2(intrinsic, operand, shifterOperand);
			result = mBuilder.CreateExtractValue(compResult, 0);
			overflow = mBuilder.CreateExtractValue(compResult, 1);
			carry = mBuilder.CreateOr(
									  mBuilder.CreateICmpULT(result, operand),
									  mBuilder.CreateICmpULT(result, shifterOperand));
			
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0x5:	// 0b01010
			// ADC
			EnsureAllocated(&mCPSR_C, 1);
			result = mBuilder.CreateAdd(mBuilder.CreateAdd(operand, shifterOperand), mBuilder.CreateZExt(mBuilder.CreateLoad(mCPSR_C), Type::getInt32Ty(mContext)));
			
			// OverFlow:
			// Addition generates an overflow if both operands have the same
			// sign (bit[31]), and the sign of the result is different to the
			// sign of both operands.
			overflow = mBuilder.CreateAnd(
										  mBuilder.CreateICmpEQ(
																mBuilder.CreateLShr(operand, mBuilder.getInt32(31)),
																mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(31))),
										  mBuilder.CreateICmpNE(
																mBuilder.CreateLShr(operand, mBuilder.getInt32(31)),
																mBuilder.CreateLShr(result, mBuilder.getInt32(31))));
			
			carry = mBuilder.CreateOr(
									  mBuilder.CreateICmpULT(result, operand),
									  mBuilder.CreateICmpULT(result, shifterOperand));
			
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0x6:	// 0b01100
			// SBC
			EnsureAllocated(&mCPSR_C, 1);
			result = mBuilder.CreateSub(mBuilder.CreateSub(operand, shifterOperand),
										mBuilder.CreateZExt(
															mBuilder.CreateXor(mBuilder.CreateLoad(mCPSR_C), mBuilder.getInt1(true)),
															Type::getInt32Ty(mContext)));
			
			// OverFlow:
			// Subtraction causes an overflow if the operands have different
			// signs, and the first operand and the result have different signs.
			overflow = mBuilder.CreateAnd(
										  mBuilder.CreateICmpNE(
																mBuilder.CreateLShr(operand, mBuilder.getInt32(31)),
																mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(31))),
										  mBuilder.CreateICmpNE(
																mBuilder.CreateLShr(operand, mBuilder.getInt32(31)),
																mBuilder.CreateLShr(result, mBuilder.getInt32(31))));
			
			carry = mBuilder.CreateICmpUGE(operand, shifterOperand);
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0x7:	// 0b0111
			// RSC
			EnsureAllocated(&mCPSR_C, 1);
			result = mBuilder.CreateSub(mBuilder.CreateSub(shifterOperand, operand),
										mBuilder.CreateZExt(
															mBuilder.CreateXor(mBuilder.CreateLoad(mCPSR_C), mBuilder.getInt1(true)),
															Type::getInt32Ty(mContext)));
			
			// OverFlow:
			// Subtraction causes an overflow if the operands have different
			// signs, and the first operand and the result have different signs.
			overflow = mBuilder.CreateAnd(
										  mBuilder.CreateICmpNE(
																mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(31)),
																mBuilder.CreateLShr(operand, mBuilder.getInt32(31))),
										  mBuilder.CreateICmpNE(
																mBuilder.CreateLShr(shifterOperand, mBuilder.getInt32(31)),
																mBuilder.CreateLShr(result, mBuilder.getInt32(31))));
			
			carry = mBuilder.CreateICmpUGE(shifterOperand, operand);
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0x8:	// 0b1000
			// TST
			result = mBuilder.CreateAnd(operand, shifterOperand);
			carry = shifterCarryOut;
			break;
			
		case 0x9:	// 0b1001
			// TEQ
			result = mBuilder.CreateXor(operand, shifterOperand);
			carry = shifterCarryOut;
			break;
			
		case 0xA:	// 0b1010
			// CMP
			intrinsic = EnsureIntrinsic(Intrinsic::ssub_with_overflow);
			compResult = mBuilder.CreateCall2(intrinsic, operand, shifterOperand);
			result = mBuilder.CreateExtractValue(compResult, 0);
			overflow = mBuilder.CreateExtractValue(compResult, 1);
			carry = mBuilder.CreateICmpUGE(operand, shifterOperand);
			break;
			
		case 0xB:	// 0b1011
			// CMN
			intrinsic = EnsureIntrinsic(Intrinsic::sadd_with_overflow);
			compResult = mBuilder.CreateCall2(intrinsic, operand, shifterOperand);
			result = mBuilder.CreateExtractValue(compResult, 0);
			overflow = mBuilder.CreateExtractValue(compResult, 1);
			carry = mBuilder.CreateOr(
									  mBuilder.CreateICmpULT(result, operand),
									  mBuilder.CreateICmpULT(result, shifterOperand));
			break;
			
		case 0xC:	// 0b1100
			// ORR
			result = mBuilder.CreateOr(operand, shifterOperand);
			carry = shifterCarryOut;
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0xD:	// 0b11010
			// MOV
			result = shifterOperand;
			carry = shifterCarryOut;
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0xE:	// 0b1110
			// BIC
			result = mBuilder.CreateAnd(operand, mBuilder.CreateXor(shifterOperand, mBuilder.getInt32(0xFFFFFFFF)));
			carry = shifterCarryOut;
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
			
		case 0xF:	// 0b11110
			// MVN
			result = mBuilder.CreateXor(shifterOperand, mBuilder.getInt32(0xFFFFFFFF));
			carry = shifterCarryOut;
			if (Rd != 15) {
				mBuilder.CreateStore(result, mRegisters[Rd]);
			}
			break;
	}
}



// -------------------------------------------------------------------------- //
//  * Translate_MRS
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_MRS(KUInt32 offsetInPage, KUInt32 inInstruction) {
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  0  1  0  R  0  0  --SBO-- --Rd--- -----------SBZ------------- MRS
	const KUInt32 flag_r = inInstruction & 0x00400000;
	const KUInt32 Rd = (inInstruction & 0x0000F000) >> 12;
	
	if (Rd == 15) {
		KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
		BuildExitToFunction("JIT_UndefinedInstruction", exitPC);
	} else {
		EnsureAllocated(&mRegisters[Rd], 32);
		Value* value;
		if (flag_r) {
			Value* mode = BuildGetEmulationMode();
			value = BuildGetSPSR(mode);
		} else{
			value = BuildGetCPSR();
		}
		mBuilder.CreateStore(value, mRegisters[Rd]);
		// Next instruction
		mBuilder.CreateBr(GetBlock(offsetInPage + 1));
	}
}

// -------------------------------------------------------------------------- //
//  * Translate_MSR
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_MSR(KUInt32 offsetInPage, KUInt32 inInstruction) {
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  0  1  1  0  R  1  0  -field- --SBO-- rotate   8_bit_immediate    MSR
	// -Cond-- 0  0  0  1  0  R  1  0  -field- --SBO-- --SBZ-- 0  0  0  0  --Rm--- MSR
	const KUInt32 flag_i = inInstruction & 0x02000000;
	const KUInt32 flag_r = inInstruction & 0x00400000;
	const KUInt32 mask_f = inInstruction & 0x00080000;
	const KUInt32 mask_c = inInstruction & 0x00010000;
	
	// This is the layout of status registers
	// 31 - 28 27 - 24 23 - 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -------c------- -------x------- -------s------- -------f-----------
	// N Z C V --res.- ----reserved--- ----reserved---  I  F  T --mode----
	
	// In ARMv4, only c can be set by user.
	// f is for both interruption bits and mode.

    Value* operand;
	if (flag_i) {
		// Immediate.
		KUInt32 theImmValue = inInstruction & 0xFF;
		KUInt32 theRotateAmount = ((inInstruction >> 8) & 0xF) * 2;
		if (theRotateAmount != 0)
		{
			theImmValue =
			(theImmValue >> theRotateAmount)
			| (theImmValue << (32 - theRotateAmount));
		}
		operand = mBuilder.getInt32(theImmValue);
	} else {
    	const KUInt32 Rm = (inInstruction & 0x0000000F);
	    if (Rm == 15) {
			operand = mBuilder.getInt32((offsetInPage + 2) * 4 + mCurrentVAddress);
		} else {
			EnsureAllocated(&mRegisters[Rm], 32);
			operand = mBuilder.CreateLoad(mRegisters[Rm]);
		}
	}

	std::vector<llvm::Value *> indexes;
	indexes.push_back(mBuilder.getInt32(0));
	indexes.push_back(mBuilder.getInt32(i_mMode));
	Value* mode = mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes));
	
	if (flag_r) {
		if ((mask_c == 0) || (mask_f == 0)) {
			Value* spsr = BuildGetSPSR(mode);
			KUInt32 mask = 0;
			if (mask_c) {
				mask |= 0x000000FF;
			}
			if (mask_f) {
				mask |= 0xFF000000;
			}
			operand =
				mBuilder.CreateOr(
				    mBuilder.CreateAnd(spsr, mBuilder.getInt32(~mask)),
					mBuilder.CreateAnd(operand, mBuilder.getInt32(mask)));
		}
		BuildSetSPSR(operand, mode);
	} else {
		KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
		Value* doFieldsBits = mBuilder.getInt1(mask_f != 0);
		Value* doControlBits;
		if (mask_c) {
			doControlBits = mBuilder.CreateICmpNE(mode, mBuilder.getInt32(TARMProcessor::kUserMode));
		} else {
			doControlBits = mBuilder.getInt1(false);
		}
		BuildSetCPSR(operand, mode, doControlBits, doFieldsBits, mBuilder.getInt32(exitPC));
	}
	
	// Next instruction
	mBuilder.CreateBr(GetBlock(offsetInPage + 1));
}

// -------------------------------------------------------------------------- //
//  * Translate_BKPT
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_BKPT(KUInt32 offsetInPage, KUInt32 inInstruction) {
	// Breakpoint handler will read the instruction to get the Breakpoint ID.
	KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
	BuildExitToFunction("JIT_Breakpoint", exitPC);
}

// -------------------------------------------------------------------------- //
//  * Translate_SingleDataTransfer
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_SingleDataTransfer(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 0  1  I  P  U  B  W  L  --Rn--- --Rd--- -----------offset----------

	
	// -Cond-- 0  1  0  P  U  B  W  L  --Rn--- --Rd--- -----------offset----------	// Immediate offset/index
	// -Cond-- 0  1  1  P  U  B  W  L  --Rn--- --Rd--- 0 0 0 0 0  0  0  0  --Rm---	// Register offset/index
	// -Cond-- 0  1  1  P  U  B  W  L  --Rn--- --Rd--- shift_imm  shift 0  --Rm---	// Scaled register offset/index
	
	// Load and Store Word or Unsigned Byte - Immediate offset
	// (P = 1, W = 0)
	// R15 : Rn : ok
	// -Cond-- 0  1  0  1  U  B  1  L  --Rn--- --Rd--- -----------offset----------

	// Load and Store Word or Unsigned Byte - Register offset
	// (P = 1, W = 0)
	// R15 : Rn, Rd : ok. Rm : unpredictable.
	// -Cond-- 0  1  1  1  U  B  0  L  --Rn--- --Rd--- 0 0 0 0 0  0  0  0  --Rm---
	
	// Load and Store Word or Unsigned Byte - Scaled register offset
	// (P = 1, W = 0)
	// R15 : Rn, Rd : ok. Rm : unpredictable.
	// -Cond-- 0  1  1  1  U  B  0  L  --Rn--- --Rd--- shift_imm  shift 0  --Rm---

	// Load and Store Word or Unsigned Byte - Immediate pre-indexed
	// (P = 1, W = 1)
	// R15 : Rn : unpredictable.
	// -Cond-- 0  1  0  1  U  B  1  L  --Rn--- --Rd--- -----------offset----------

	// Load and Store Word or Unsigned Byte - Register pre-indexed
	// (P = 1, W = 1)
	// R15 : Rd, ok. Rm or Rn : unpredictable.
	// -Cond-- 0  1  1  1  U  B  1  L  --Rn--- --Rd--- 0 0 0 0 0  0  0  0  --Rm---

	// Load and Store Word or Unsigned Byte - Scaled register pre-indexed
	// (P = 1, W = 1)
	// R15 : Rd, ok. Rm or Rn : unpredictable.
	// -Cond-- 0  1  1  1  U  B  1  L  --Rn--- --Rd--- shift_imm  shift 0  --Rm---

	// Load and Store Word or Unsigned Byte - Immediate post-indexed
	// (P = 0, W = 0 or 1 (unprivileged))
	// R15 : Rn : unpredictable.
	// -Cond-- 0  1  0  1  U  B  W  L  --Rn--- --Rd--- -----------offset----------
	
	// Load and Store Word or Unsigned Byte - Register post-indexed
	// (P = 0, W = 0 or 1 (unprivileged))
	// R15 : Rd, ok. Rm or Rn : unpredictable.
	// -Cond-- 0  1  1  1  U  B  W  L  --Rn--- --Rd--- 0 0 0 0 0  0  0  0  --Rm---
	
	// Load and Store Word or Unsigned Byte - Scaled register post-indexed
	// (P = 0, W = 0 or 1 (unprivileged))
	// R15 : Rd, ok. Rm or Rn : unpredictable.
	// -Cond-- 0  1  1  1  U  B  W  L  --Rn--- --Rd--- shift_imm  shift 0  --Rm---
	
	const KUInt32 flag_i = (inInstruction & 0x02000000);
	const KUInt32 flag_p = (inInstruction & 0x01000000);
	const KUInt32 flag_u = (inInstruction & 0x00800000);
	const KUInt32 flag_b = (inInstruction & 0x00400000);
	const KUInt32 flag_w = (inInstruction & 0x00200000);
	const KUInt32 flag_l = (inInstruction & 0x00100000);

	const KUInt32 Rn = (inInstruction & 0x000F0000) >> 16;
	const KUInt32 Rd = (inInstruction & 0x0000F000) >> 12;
	const KUInt32 Rm = (inInstruction & 0x0000000F);
	
	KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
	// Unpredicable uses of R15
	if ((flag_i && Rm == 15) || (Rn == 15 && !flag_p)) {
		BuildExitToFunction("JIT_UndefinedInstruction", exitPC);
	} else {
		Value* theOffset;
	
		if (flag_i) {
			if ((inInstruction & 0x00000FFF) >> 4) {
				// Shift.
				// PC is not Rm here (flag_i is set, see above).
				theOffset = GetShiftNoCarryNoR15(inInstruction);
			} else {
				// PC is not Rm here (flag_i is set, see above).
				EnsureAllocated(&mRegisters[Rm], 32);
				theOffset = mBuilder.CreateLoad(mRegisters[Rm]);
			}
		} else {
			// Immediate
			theOffset = mBuilder.getInt32(inInstruction & 0x00000FFF);
		}
	
		Value* theAddress;
		if (Rn == 15) {
			theAddress = mBuilder.getInt32((offsetInPage + 2) * 4 + mCurrentVAddress);
		} else {
			EnsureAllocated(&mRegisters[Rn], 32);
			theAddress = mBuilder.CreateLoad(mRegisters[Rn]);
		}
	
		if (flag_p) {
			if (flag_u) {
				theAddress = mBuilder.CreateAdd(theAddress, theOffset);
			} else {
				theAddress = mBuilder.CreateSub(theAddress, theOffset);
			}
		}
		
		Function* setPrivilegeFunc = nullptr;
		if (!flag_p && flag_w) {
			// Drop privilege for unprivileged access.
			setPrivilegeFunc = EnsureFunction(GetSetPrivilegeFuncType(), "JIT_SetPrivilege");
			mBuilder.CreateCall2(setPrivilegeFunc, mProcessor, mBuilder.getInt1(false));
		}
		
		BasicBlock* dataAbortExit = nullptr;
		BasicBlock* nextBlock = nullptr;
		
		if (flag_l) {
			// Load
			Value* wordResult = nullptr;
			bool optimized = false;
			if (ConstantInt::classof(theAddress) && !(!flag_p && flag_w)) {
				KUInt32 theAddressVal = (KUInt32) ((ConstantInt*)theAddress)->getZExtValue();
			    if (theAddressVal > mCurrentVAddress
			        && ((theAddressVal - mCurrentVAddress) / sizeof(KUInt32)) < mInstructionCount) {
			        // Within same page.
			        optimized = true;
			        if (flag_b) {
			            KUInt8* ptr = ((KUInt8*) mCurrentPointer) + (theAddressVal - mCurrentVAddress);
			            wordResult = mBuilder.getInt32((KUInt32) *ptr);
			        } else {
			            KUInt32 value = mCurrentPointer[(theAddressVal - mCurrentVAddress) / sizeof(KUInt32)];
			            wordResult = mBuilder.getInt32(value);
			        }
			    }
			}
			if (!optimized) {
				dataAbortExit = BasicBlock::Create(mContext, BLOCKNAME("dataabort"), mFunction);
				BasicBlock* proceedBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
                if (flag_b) {
                    // Byte.
                    Function* readFunction = EnsureFunction(GetReadBFuncType(), "JIT_ReadB");
                    Value* byte = mBuilder.CreateAlloca(Type::getInt8Ty(mContext));
                    Value* result = mBuilder.CreateCall3(readFunction, GetMemory(), theAddress, byte);
                    mBuilder.CreateCondBr(result, dataAbortExit, proceedBlock);
                    mBuilder.SetInsertPoint(proceedBlock);
                    wordResult = mBuilder.CreateZExt(mBuilder.CreateLoad(byte), Type::getInt32Ty(mContext));
                } else {
                    // Word.
                    Function* readFunction = EnsureFunction(GetReadFuncType(), "JIT_Read");
                    Value* word = mBuilder.CreateAlloca(Type::getInt32Ty(mContext));
                    Value* result = mBuilder.CreateCall3(readFunction, GetMemory(), theAddress, word);
                    mBuilder.CreateCondBr(result, dataAbortExit, proceedBlock);
                    mBuilder.SetInsertPoint(proceedBlock);
                    wordResult = mBuilder.CreateLoad(word);
                }
            }
			if (Rd == 15) {
				mBuilder.CreateStore(mBuilder.CreateAdd(wordResult, mBuilder.getInt32(4)), mPC);
				nextBlock = mMainExit;
			} else {
				EnsureAllocated(&mRegisters[Rd], 32);
				mBuilder.CreateStore(wordResult, mRegisters[Rd]);
			}
		} else {
			dataAbortExit = BasicBlock::Create(mContext, BLOCKNAME("dataabort"), mFunction);
			BasicBlock* proceedBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
			// Store
			Value* storedValue;
			if (Rd == 15) {
				// If PC is Rd, the stored value is +12 instead of +8
				storedValue = mBuilder.getInt32((offsetInPage + 3) * 4 + mCurrentVAddress);
			} else {
				EnsureAllocated(&mRegisters[Rd], 32);
				storedValue = mBuilder.CreateLoad(mRegisters[Rd]);
			}
			if (flag_b) {
				// Byte.
				Function* writeFunction = EnsureFunction(GetWriteBFuncType(), "JIT_WriteB");
				Value* result = mBuilder.CreateCall3(writeFunction, GetMemory(), theAddress, mBuilder.CreateTrunc(storedValue, Type::getInt8Ty(mContext)));
				mBuilder.CreateCondBr(result, dataAbortExit, proceedBlock);
			} else {
				// Word.
				Function* writeFunction = EnsureFunction(GetWriteFuncType(), "JIT_Write");
				Value* result = mBuilder.CreateCall3(writeFunction, GetMemory(), theAddress, storedValue);
				mBuilder.CreateCondBr(result, dataAbortExit, proceedBlock);
			}
			mBuilder.SetInsertPoint(proceedBlock);
		}
		
		if (!flag_p && flag_w) {
			// Restore privileged access.
			mBuilder.CreateCall2(setPrivilegeFunc, mProcessor, mBuilder.getInt1(true));
		}
		
		if ((Rn != 15) && (!flag_p || flag_w)) {
			// Write back.
			// PC is not Rn here (flag_p is clear or flag_w i set, see above).
			// If PC is Rn, result is UNPREDICTABLE.
			// We just don't write back PC, to be compatible with JITTARGET_GENERIC.
			if (!flag_p) {
				if (flag_u) {
					mBuilder.CreateStore(mBuilder.CreateAdd(theAddress, theOffset), mRegisters[Rn]);
				} else {
					mBuilder.CreateStore(mBuilder.CreateSub(theAddress, theOffset), mRegisters[Rn]);
				}
			} else {
				mBuilder.CreateStore(theAddress, mRegisters[Rn]);
			}
		}
		
		if (nextBlock == nullptr) {
			// Next instruction
			nextBlock = GetBlock(offsetInPage + 1);
			// We might return from a DataAbort or a signal, but only if PC wasn't written.
			AddFunctionIfRequired(offsetInPage + 1, false);
		}
		mBuilder.CreateBr(nextBlock);
		
		if (dataAbortExit) {
			mBuilder.SetInsertPoint(dataAbortExit);
			BuildExitToFunction("JIT_DataAbort", exitPC);
		}
	}
}

// -------------------------------------------------------------------------- //
//  * Translate_Branch
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_Branch(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// -Cond-- 1  0  1  L  ---------------------offset---------------------------- Branch
	
	// PC prefetch strategy:
	// ARM Architecture specifies that when read, PC is 8 bytes ahead executed instruction,
	// unless specified otherwise.
	// When written, it points to the next execute insruction.
	
	// The offset of the branch should be added to the read PC and then PC will point
	// exactly to the next instruction.
	// In other words, "B here" is coded EAFFFFFE, i.e. branch to offset -8.

	// LR value is the address of next instruction (PC+4).
	
	// On JIT exit, we set PC to address of instruction to execute + 4.

	// We do three kinds of branches here:
	// branches to another page. We just update PC and exit native code.
	// branches to the same page, forward. We simply proceed (and avoid updating PC)
	// branches to the same page, backward. We check signal. If false, we update PC and exit, if true we proceed.

	KUInt32 offset = (inInstruction & 0x007FFFFF) << 2;
	if (inInstruction & 0x00800000)
	{
		offset |= 0xFE000000;
	}
	KSInt32 delta = offsetInPage * 4 + (KSInt32) offset + 8;
	KSInt32 branchToInPage = (delta >> 2);
	
	if (inInstruction & 0x01000000)
	{
	    // This is branch with link.
	    KUInt32 lrValue = (offsetInPage + 1) * 4 + mCurrentVAddress;
		EnsureAllocated(&mRegisters[TARMProcessor::kR14], 32);
	    mBuilder.CreateStore(mBuilder.getInt32(lrValue), mRegisters[TARMProcessor::kR14]);
	    if (delta > 0 && branchToInPage < mInstructionCount) {
	        // Forward, in page.
            mBuilder.CreateBr(GetBlock(branchToInPage));
        } else if (branchToInPage >= 0 && branchToInPage < mInstructionCount) {
			BasicBlock* signalExit = BasicBlock::Create(mContext, BLOCKNAME("signalexit"), mFunction);
            // Backward, in page.
			mBuilder.CreateCondBr(mBuilder.CreateLoad(mSignal, true), GetBlock(branchToInPage), signalExit);
			mBuilder.SetInsertPoint(signalExit);
			mBuilder.CreateStore(mBuilder.getInt32(delta + mCurrentVAddress + 4), mPC);
			mBuilder.CreateBr(mMainExit);
        } else {
			mBuilder.CreateStore(mBuilder.getInt32(delta + mCurrentVAddress + 4), mPC);
			mBuilder.CreateBr(mMainExit);
	    }
		// We definitely will return.
		AddFunctionIfRequired(offsetInPage + 1, true);
	} else {
	    // This is regular branch.
		if (delta > 0 && branchToInPage < mInstructionCount) {
			// Forward, in page.
			mBuilder.CreateBr(GetBlock(branchToInPage));
		} else if (branchToInPage >= 0 && branchToInPage < mInstructionCount) {
			BasicBlock* signalExit = BasicBlock::Create(mContext, BLOCKNAME("signalexit"), mFunction);
			// Backward, in page.
			mBuilder.CreateCondBr(mBuilder.CreateLoad(mSignal, true), GetBlock(branchToInPage), signalExit);
			mBuilder.SetInsertPoint(signalExit);
			mBuilder.CreateStore(mBuilder.getInt32(delta + mCurrentVAddress + 4), mPC);
			mBuilder.CreateBr(mMainExit);
		} else {
			mBuilder.CreateStore(mBuilder.getInt32(delta + mCurrentVAddress + 4), mPC);
			mBuilder.CreateBr(mMainExit);
		}
	}
}

// -------------------------------------------------------------------------- //
//  * Translate_LDM13
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_LDM13(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer

	const KUInt32 flag_s = (inInstruction & 0x00400000);
	const KUInt32 flag_w = (inInstruction & 0x00200000);
	const KUInt32 Rn = (inInstruction & 0x000F0000) >> 16;
	const KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
	
	BasicBlock* dataAbortExit = BasicBlock::Create(mContext, BLOCKNAME("dataabort"), mFunction);
	
	Value* startAddress = BuildGetBlockDataTransferBaseAddress(inInstruction);
	KUInt32 registersMask = 0x0001;
	uint32_t numWords = 0;
	for (int i = 0; i <= 15; i++) {
		if (inInstruction & registersMask) {
			numWords++;
		}
		registersMask = registersMask << 1;
	}
	Value* countVal = mBuilder.getInt32(numWords);
	Value* words = mBuilder.CreateAlloca(Type::getInt32Ty(mContext), countVal);
	Function* readFunction = EnsureFunction(GetReadBlockFuncType(), "JIT_ReadBlock");
	Value* result = mBuilder.CreateCall4(readFunction, GetMemory(), startAddress, countVal, words);
	BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
	mBuilder.SetInsertPoint(continueBlock);
	int readWord = 0;
	registersMask = 0x0001;
	for (int i = 0; i < 15; i++) {
		if (inInstruction & registersMask) {
			EnsureAllocated(&mRegisters[i], 32);
			mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(words, mBuilder.getInt32(readWord))), mRegisters[i]);
			readWord++;
		}
		registersMask = registersMask << 1;
	}
	if (inInstruction & 0x8000) {
		// Store value + 4 for PREFETCH
		mBuilder.CreateStore(
			mBuilder.CreateAdd(
				mBuilder.CreateAnd(
					mBuilder.CreateLoad(mBuilder.CreateGEP(words, mBuilder.getInt32(readWord))),
					mBuilder.getInt32(0xFFFFFFFC)),
				mBuilder.getInt32(4)), mPC);
	}
	
	if (flag_w) {
		// Use of R15 as Rn is UNPREDICTABLE and generated an undefined instruction above.
		// We know Rn was allocated by BuildGetBlockDataTranferBaseAddress.
		Value* endAddress = mBuilder.CreateAdd(startAddress, mBuilder.getInt32(4 * (numWords - 1)));
		Value* storedAddress = BuildGetBlockDataTransferWriteback(inInstruction, startAddress, endAddress);
		mBuilder.CreateStore(storedAddress, mRegisters[Rn]);
	}

	// Next instruction
	if (inInstruction & 0x8000) {
		if (flag_s) {
			// This is LDM3
			// PC was written above (to mPC). So we pass nullptr to BuildSetCPSR to jump there
			// after any interrupt.
			Value* mode = BuildGetEmulationMode();
			Value* spsr = BuildGetSPSR(mode);
			BuildSetCPSR(spsr, mode, mBuilder.getInt1(true), mBuilder.getInt1(true), nullptr);
		}
		// Exit as PC was written to.
		mBuilder.CreateBr(mMainExit);
	} else {
		mBuilder.CreateBr(GetBlock(offsetInPage + 1));
		// We might return after a data abort.
		AddFunctionIfRequired(offsetInPage + 1, false);
	}

	mBuilder.SetInsertPoint(dataAbortExit);
	BuildExitToFunction("JIT_DataAbort", exitPC);
}

// -------------------------------------------------------------------------- //
//  * Translate_LDM2
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_LDM2(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// LDM(2) loads user mode registers when the processor is in a privileged mode.
	BasicBlock* dataAbortExit = BasicBlock::Create(mContext, BLOCKNAME("dataabort"), mFunction);
	
	Value* startAddress = BuildGetBlockDataTransferBaseAddress(inInstruction);
	Value* commonAddress = startAddress;
	KUInt32 commonRegistersMask = 0x0001;
	Function* readFunction = EnsureFunction(GetReadFuncType(), "JIT_Read");
	Value* word = mBuilder.CreateAlloca(Type::getInt32Ty(mContext));
	for (int i = 0; i < 8; i++) {
		if (inInstruction & commonRegistersMask) {
			EnsureAllocated(&mRegisters[i], 32);
			Value* result = mBuilder.CreateCall3(readFunction, GetMemory(), commonAddress, word);
			BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
			mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
			mBuilder.SetInsertPoint(continueBlock);
			mBuilder.CreateStore(mBuilder.CreateLoad(word), mRegisters[i]);
			commonAddress = mBuilder.CreateAdd(commonAddress, mBuilder.getInt32(4));
		}
		commonRegistersMask = commonRegistersMask << 1;
	}
	
	BasicBlock* fiqModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* otherModesBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* testSuperModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* endBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	
	std::vector<llvm::Value *> indexes;
	indexes.push_back(mBuilder.getInt32(0));
	indexes.push_back(mBuilder.getInt32(0));
	
	Value* mode = BuildGetEmulationMode();
	mBuilder.CreateCondBr(mBuilder.CreateICmpEQ(mode, mBuilder.getInt32(TARMProcessor::kFIQMode)), fiqModeBlock, testSuperModeBlock);
	
	mBuilder.SetInsertPoint(testSuperModeBlock);
	mBuilder.CreateCondBr(mBuilder.CreateICmpEQ(mode, mBuilder.getInt32(TARMProcessor::kUserMode)), endBlock, otherModesBlock);
	
	mBuilder.SetInsertPoint(fiqModeBlock);
	{
		Value* fiqAddress = commonAddress;
		KUInt32 fiqRegistersMask = commonRegistersMask;
		for (int i = 8; i < 13; i++) {
			if (inInstruction & fiqRegistersMask) {
				Value* result = mBuilder.CreateCall3(readFunction, GetMemory(), fiqAddress, word);
				BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
				mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
				mBuilder.SetInsertPoint(continueBlock);
				indexes[1] = mBuilder.getInt32(i_mR8_Bkup + i - 8);
				Value* backupRegister = mBuilder.CreateGEP(mProcessor, indexes);
				mBuilder.CreateStore(mBuilder.CreateLoad(word), backupRegister);
				fiqAddress = mBuilder.CreateAdd(fiqAddress, mBuilder.getInt32(4));
			}
			fiqRegistersMask = fiqRegistersMask << 1;
		}
		for (int i = 13; i < 15; i++) {
			if (inInstruction & fiqRegistersMask) {
				Value* result = mBuilder.CreateCall3(readFunction, GetMemory(), fiqAddress, word);
				BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
				mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
				mBuilder.SetInsertPoint(continueBlock);
				indexes[1] = mBuilder.getInt32(i_mR13_Bkup + i - 13);
				Value* backupRegister = mBuilder.CreateGEP(mProcessor, indexes);
				mBuilder.CreateStore(mBuilder.CreateLoad(word), backupRegister);
				fiqAddress = mBuilder.CreateAdd(fiqAddress, mBuilder.getInt32(4));
			}
			fiqRegistersMask = fiqRegistersMask << 1;
		}
		mBuilder.CreateBr(endBlock);
	}
	
	mBuilder.SetInsertPoint(otherModesBlock);
	{
		Value* otherAddress = commonAddress;
		KUInt32 otherRegistersMask = commonRegistersMask;
		for (int i = 8; i < 13; i++) {
			if (inInstruction & otherRegistersMask) {
				EnsureAllocated(&mRegisters[i], 32);
				Value* result = mBuilder.CreateCall3(readFunction, GetMemory(), otherAddress, word);
				BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
				mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
				mBuilder.SetInsertPoint(continueBlock);
				mBuilder.CreateStore(mBuilder.CreateLoad(word), mRegisters[i]);
				otherAddress = mBuilder.CreateAdd(otherAddress, mBuilder.getInt32(4));
			}
			otherRegistersMask = otherRegistersMask << 1;
		}
		for (int i = 13; i < 15; i++) {
			if (inInstruction & otherRegistersMask) {
				Value* result = mBuilder.CreateCall3(readFunction, GetMemory(), otherAddress, word);
				BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
				mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
				mBuilder.SetInsertPoint(continueBlock);
				indexes[1] = mBuilder.getInt32(i_mR13_Bkup + i - 13);
				Value* backupRegister = mBuilder.CreateGEP(mProcessor, indexes);
				mBuilder.CreateStore(mBuilder.CreateLoad(word), backupRegister);
				otherAddress = mBuilder.CreateAdd(otherAddress, mBuilder.getInt32(4));
			}
			otherRegistersMask = otherRegistersMask << 1;
		}
		mBuilder.CreateBr(endBlock);
	}

	mBuilder.SetInsertPoint(endBlock);
	mBuilder.CreateBr(GetBlock(offsetInPage + 1));

	mBuilder.SetInsertPoint(dataAbortExit);
	KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
	BuildExitToFunction("JIT_DataAbort", exitPC);

	// We might return after a data abort.
	AddFunctionIfRequired(offsetInPage + 1, false);
}

// -------------------------------------------------------------------------- //
//  * Translate_STM1
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_STM1(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer
	
	const KUInt32 flag_w = (inInstruction & 0x00200000);
	const KUInt32 Rn = (inInstruction & 0x000F0000) >> 16;
	
	BasicBlock* dataAbortExit = BasicBlock::Create(mContext, BLOCKNAME("dataabort"), mFunction);
	KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
	
	Value* startAddress = BuildGetBlockDataTransferBaseAddress(inInstruction);
	KUInt32 registersMask = 0x0001;
	uint32_t numWords = 0;
	for (int i = 0; i <= 15; i++) {
		if (inInstruction & registersMask) {
			numWords++;
		}
		registersMask = registersMask << 1;
	}
	Value* countVal = mBuilder.getInt32(numWords);
	Value* words = mBuilder.CreateAlloca(Type::getInt32Ty(mContext), countVal);
	int writtenWord = 0;
	registersMask = 0x0001;
	for (int i = 0; i < 15; i++) {
		if (inInstruction & registersMask) {
			EnsureAllocated(&mRegisters[i], 32);
			mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[i]), mBuilder.CreateGEP(words, mBuilder.getInt32(writtenWord)));
			writtenWord++;
		}
		registersMask = registersMask << 1;
	}
	if (inInstruction & 0x8000) {
		mBuilder.CreateStore(mBuilder.getInt32(exitPC), mBuilder.CreateGEP(words, mBuilder.getInt32(writtenWord)));
	}

	Function* writeFunction = EnsureFunction(GetWriteBlockFuncType(), "JIT_WriteBlock");
	Value* result = mBuilder.CreateCall4(writeFunction, GetMemory(), startAddress, countVal, words);
	BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
	mBuilder.SetInsertPoint(continueBlock);
	
	if (flag_w) {
		// Use of R15 as Rn is UNPREDICTABLE and generated an undefined instruction above.
		// We know Rn was allocated by BuildGetBlockDataTranferBaseAddress.
		Value* endAddress = mBuilder.CreateAdd(startAddress, mBuilder.getInt32(4 * (numWords - 1)));
		Value* storedAddress = BuildGetBlockDataTransferWriteback(inInstruction, startAddress, endAddress);
		mBuilder.CreateStore(storedAddress, mRegisters[Rn]);
	}
	
	// Next instruction
	mBuilder.CreateBr(GetBlock(offsetInPage + 1));

	mBuilder.SetInsertPoint(dataAbortExit);
	BuildExitToFunction("JIT_DataAbort", exitPC);

	// We might return after a data abort.
	AddFunctionIfRequired(offsetInPage + 1, false);
}

// -------------------------------------------------------------------------- //
//  * Translate_STM2
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::Translate_STM2(KUInt32 offsetInPage, KUInt32 inInstruction)
{
	// STM(2) stores user mode registers when the processor is in a privileged mode.
	BasicBlock* dataAbortExit = BasicBlock::Create(mContext, BLOCKNAME("dataabort"), mFunction);
	KUInt32 exitPC = (offsetInPage + 2) * 4 + mCurrentVAddress;
	
	Value* startAddress = BuildGetBlockDataTransferBaseAddress(inInstruction);
	Value* commonAddress = startAddress;
	KUInt32 commonRegistersMask = 0x0001;
	Function* writeFunction = EnsureFunction(GetWriteFuncType(), "JIT_Write");
	for (int i = 0; i < 8; i++) {
		if (inInstruction & commonRegistersMask) {
			EnsureAllocated(&mRegisters[i], 32);
			Value* result = mBuilder.CreateCall3(writeFunction, GetMemory(), commonAddress, mBuilder.CreateLoad(mRegisters[i]));
			BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
			mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
			mBuilder.SetInsertPoint(continueBlock);
			commonAddress = mBuilder.CreateAdd(commonAddress, mBuilder.getInt32(4));
		}
		commonRegistersMask = commonRegistersMask << 1;
	}
	
	BasicBlock* fiqModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* otherModesBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* testSuperModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* endBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	
	std::vector<llvm::Value *> indexes;
	indexes.push_back(mBuilder.getInt32(0));
	indexes.push_back(mBuilder.getInt32(0));
	
	Value* mode = BuildGetEmulationMode();
	mBuilder.CreateCondBr(mBuilder.CreateICmpEQ(mode, mBuilder.getInt32(TARMProcessor::kFIQMode)), fiqModeBlock, testSuperModeBlock);
	
	mBuilder.SetInsertPoint(testSuperModeBlock);
	mBuilder.CreateCondBr(mBuilder.CreateICmpEQ(mode, mBuilder.getInt32(TARMProcessor::kUserMode)), endBlock, otherModesBlock);
	
	mBuilder.SetInsertPoint(fiqModeBlock);
	{
		Value* fiqAddress = commonAddress;
		KUInt32 fiqRegistersMask = 0x0100;
		for (int i = 8; i < 13; i++) {
			if (inInstruction & fiqRegistersMask) {
				indexes[1] = mBuilder.getInt32(i_mR8_Bkup + i - 8);
				Value* backupRegister = mBuilder.CreateGEP(mProcessor, indexes);
				Value* result = mBuilder.CreateCall3(writeFunction, GetMemory(), fiqAddress, mBuilder.CreateLoad(backupRegister));
				BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
				mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
				mBuilder.SetInsertPoint(continueBlock);
				fiqAddress = mBuilder.CreateAdd(fiqAddress, mBuilder.getInt32(4));
			}
			fiqRegistersMask = fiqRegistersMask << 1;
		}
		for (int i = 13; i < 15; i++) {
			if (inInstruction & fiqRegistersMask) {
				indexes[1] = mBuilder.getInt32(i_mR13_Bkup + i - 13);
				Value* backupRegister = mBuilder.CreateGEP(mProcessor, indexes);
				Value* result = mBuilder.CreateCall3(writeFunction, GetMemory(), fiqAddress, mBuilder.CreateLoad(backupRegister));
				BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
				mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
				mBuilder.SetInsertPoint(continueBlock);
				fiqAddress = mBuilder.CreateAdd(fiqAddress, mBuilder.getInt32(4));
			}
			fiqRegistersMask = fiqRegistersMask << 1;
		}
		if (inInstruction & 0x8000) {
			Value* result = mBuilder.CreateCall3(writeFunction, GetMemory(), fiqAddress, mBuilder.getInt32(exitPC));
			mBuilder.CreateCondBr(result, dataAbortExit, endBlock);
		} else {
			mBuilder.CreateBr(endBlock);
		}
	}
	
	mBuilder.SetInsertPoint(otherModesBlock);
	{
		Value* otherAddress = commonAddress;
		KUInt32 otherRegistersMask = 0x0100;
		for (int i = 8; i < 13; i++) {
			if (inInstruction & otherRegistersMask) {
				EnsureAllocated(&mRegisters[i], 32);
				Value* result = mBuilder.CreateCall3(writeFunction, GetMemory(), otherAddress, mBuilder.CreateLoad(mRegisters[i]));
				BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
				mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
				mBuilder.SetInsertPoint(continueBlock);
				otherAddress = mBuilder.CreateAdd(otherAddress, mBuilder.getInt32(4));
			}
			otherRegistersMask = otherRegistersMask << 1;
		}
		for (int i = 13; i < 15; i++) {
			if (inInstruction & otherRegistersMask) {
				indexes[1] = mBuilder.getInt32(i_mR13_Bkup + i - 13);
				Value* backupRegister = mBuilder.CreateGEP(mProcessor, indexes);
				Value* result = mBuilder.CreateCall3(writeFunction, GetMemory(), otherAddress, mBuilder.CreateLoad(backupRegister));
				BasicBlock* continueBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
				mBuilder.CreateCondBr(result, dataAbortExit, continueBlock);
				mBuilder.SetInsertPoint(continueBlock);
				otherAddress = mBuilder.CreateAdd(otherAddress, mBuilder.getInt32(4));
			}
			otherRegistersMask = otherRegistersMask << 1;
		}
		if (inInstruction & 0x8000) {
			Value* result = mBuilder.CreateCall3(writeFunction, GetMemory(), otherAddress, mBuilder.getInt32(exitPC));
			mBuilder.CreateCondBr(result, dataAbortExit, endBlock);
		} else {
			mBuilder.CreateBr(endBlock);
		}
	}
	
	mBuilder.SetInsertPoint(endBlock);

	mBuilder.CreateBr(GetBlock(offsetInPage + 1));
	
	mBuilder.SetInsertPoint(dataAbortExit);
	BuildExitToFunction("JIT_DataAbort", exitPC);

	// We might return after a data abort.
	AddFunctionIfRequired(offsetInPage + 1, false);
}

// -------------------------------------------------------------------------- //
//  * BuildGetBlockDataTransferBaseAddress
// -------------------------------------------------------------------------- //
Value*
TJITLLVMTranslator::FrameTranslator::BuildGetBlockDataTransferBaseAddress(KUInt32 inInstruction) {
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer
	
	const KUInt32 flags_pu = (inInstruction & 0x01800000) >> 23;
	const KUInt32 Rn = (inInstruction & 0x000F0000) >> 16;
	const KUInt16 registers = (KUInt16) (inInstruction & 0x0000FFFF);
	EnsureAllocated(&mRegisters[Rn], 32);
	
	Value* result = mBuilder.CreateLoad(mRegisters[Rn]);

	switch (flags_pu) {
		case 1:
			// p = 0, u = 1 : increment after
			break;
		case 3:
			// p = 1, u = 1 : increment before
			result = mBuilder.CreateAdd(result, mBuilder.getInt32(4));
			break;
		case 0:
			// p = 0, u = 0 : decrement after
			result = mBuilder.CreateAdd(mBuilder.CreateSub(result, mBuilder.getInt32(CountBits(registers) * 4)), mBuilder.getInt32(4));
			break;
		case 2:
			// p = 1, u = 0 : decrement before
			result = mBuilder.CreateSub(result, mBuilder.getInt32(CountBits(registers) * 4));
			break;
	}
	
	return result;
}

// -------------------------------------------------------------------------- //
//  * BuildGetBlockDataTransferWriteback
// -------------------------------------------------------------------------- //
Value*
TJITLLVMTranslator::FrameTranslator::BuildGetBlockDataTransferWriteback(KUInt32 inInstruction, Value* startAddress, Value* endAddress) {
	// 31 - 28 27 26 25 24 23 22 21 20 19 - 16 15 - 12 11 - 08 07 06 05 04 03 - 00
	// -Cond-- 1  0  0  P  U  S  W  L  --Rn--- ------------register list---------- Block Data Transfer
	
	const KUInt32 flags_pu = (inInstruction & 0x01800000) >> 23;
	
	Value* result = nullptr;
	
	switch (flags_pu) {
		case 1:
			// p = 0, u = 1 : increment after
			result = mBuilder.CreateAdd(endAddress, mBuilder.getInt32(4));
			break;
		case 3:
			// p = 1, u = 1 : increment before
			result = endAddress;
			break;
		case 0:
			// p = 0, u = 0 : decrement after
			result = mBuilder.CreateSub(startAddress, mBuilder.getInt32(4));
			break;
		case 2:
			// p = 1, u = 0 : decrement before
			result = startAddress;
			break;
	}
	
	return result;
}

// -------------------------------------------------------------------------- //
//  * BuildGetEmulationMode
// -------------------------------------------------------------------------- //
Value*
TJITLLVMTranslator::FrameTranslator::BuildGetEmulationMode() {
	std::vector<llvm::Value *> modeIndexes;
	modeIndexes.push_back(mBuilder.getInt32(0));
	modeIndexes.push_back(mBuilder.getInt32(i_mMode));
	Value* mode = mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, modeIndexes));
	return mode;
}

// -------------------------------------------------------------------------- //
//  * BuildGetCPSR
// -------------------------------------------------------------------------- //
Value*
TJITLLVMTranslator::FrameTranslator::BuildGetCPSR() {
    EnsureAllocated(&mCPSR_N, 1);
    EnsureAllocated(&mCPSR_Z, 1);
    EnsureAllocated(&mCPSR_C, 1);
    EnsureAllocated(&mCPSR_V, 1);
    Value* result = mBuilder.CreateSelect(mBuilder.CreateLoad(mCPSR_N), mBuilder.getInt32(TARMProcessor::kPSR_NBit), mBuilder.getInt32(0));
    result = mBuilder.CreateOr(result,
        mBuilder.CreateSelect(mBuilder.CreateLoad(mCPSR_Z), mBuilder.getInt32(TARMProcessor::kPSR_ZBit), mBuilder.getInt32(0)));
    result = mBuilder.CreateOr(result,
        mBuilder.CreateSelect(mBuilder.CreateLoad(mCPSR_C), mBuilder.getInt32(TARMProcessor::kPSR_CBit), mBuilder.getInt32(0)));
    result = mBuilder.CreateOr(result,
        mBuilder.CreateSelect(mBuilder.CreateLoad(mCPSR_V), mBuilder.getInt32(TARMProcessor::kPSR_VBit), mBuilder.getInt32(0)));

	std::vector<llvm::Value *> indexes;
	indexes.push_back(mBuilder.getInt32(0));
	indexes.push_back(mBuilder.getInt32(i_mCPSR_I));
	result = mBuilder.CreateOr(result,
							   mBuilder.CreateSelect(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mBuilder.getInt32(TARMProcessor::kPSR_IBit), mBuilder.getInt32(0)));

	indexes[1] = mBuilder.getInt32(i_mCPSR_F);
	result = mBuilder.CreateOr(result,
							   mBuilder.CreateSelect(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mBuilder.getInt32(TARMProcessor::kPSR_FBit), mBuilder.getInt32(0)));

	indexes[1] = mBuilder.getInt32(i_mCPSR_T);
	result = mBuilder.CreateOr(result,
							   mBuilder.CreateSelect(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mBuilder.getInt32(TARMProcessor::kPSR_TBit), mBuilder.getInt32(0)));

	indexes[1] = mBuilder.getInt32(i_mMode);
    result = mBuilder.CreateOr(result,
	    mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)));
	    
	return result;
}

// -------------------------------------------------------------------------- //
//  * BuildGetSPSR
// -------------------------------------------------------------------------- //
Value*
TJITLLVMTranslator::FrameTranslator::BuildGetSPSR(Value* mode) {
	std::vector<llvm::Value *> spsrUndIndexes;
	spsrUndIndexes.push_back(mBuilder.getInt32(0));
	spsrUndIndexes.push_back(mBuilder.getInt32(i_mSPSRund));

	std::vector<llvm::Value *> spsrFiqIndexes;
	spsrFiqIndexes.push_back(mBuilder.getInt32(0));
	spsrFiqIndexes.push_back(mBuilder.getInt32(i_mSPSRfiq));

	std::vector<llvm::Value *> spsrIrqIndexes;
	spsrIrqIndexes.push_back(mBuilder.getInt32(0));
	spsrIrqIndexes.push_back(mBuilder.getInt32(i_mSPSRirq));

	std::vector<llvm::Value *> spsrSvcIndexes;
	spsrSvcIndexes.push_back(mBuilder.getInt32(0));
	spsrSvcIndexes.push_back(mBuilder.getInt32(i_mSPSRsvc));

	std::vector<llvm::Value *> spsrAbtIndexes;
	spsrAbtIndexes.push_back(mBuilder.getInt32(0));
	spsrAbtIndexes.push_back(mBuilder.getInt32(i_mSPSRabt));
	
	Value* result = mBuilder.CreateSelect(
        mBuilder.CreateICmpEQ(mode, mBuilder.getInt32(TARMProcessor::kUndefinedMode)),
        mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, spsrUndIndexes)),
        mBuilder.CreateSelect(
            mBuilder.CreateICmpEQ(mode, mBuilder.getInt32(TARMProcessor::kFIQMode)),
            mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, spsrFiqIndexes)),
            mBuilder.CreateSelect(
                mBuilder.CreateICmpEQ(mode, mBuilder.getInt32(TARMProcessor::kIRQMode)),
                mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, spsrIrqIndexes)),
                mBuilder.CreateSelect(
                    mBuilder.CreateICmpEQ(mode, mBuilder.getInt32(TARMProcessor::kSupervisorMode)),
                    mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, spsrSvcIndexes)),
                    mBuilder.CreateSelect(
                        mBuilder.CreateICmpEQ(mode, mBuilder.getInt32(TARMProcessor::kAbortMode)),
                        mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, spsrAbtIndexes)),
                        BuildGetCPSR())))));

	return result;
}

// -------------------------------------------------------------------------- //
//  * BuildBackupBankRegisters
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::BuildBackupBankRegisters(Value* currentMode) {
	for (int i = 8; i < 15; i++) {
		EnsureAllocated(&mRegisters[i], 32);
	}

	std::vector<llvm::Value *> indexes;
	indexes.push_back(mBuilder.getInt32(0));
	indexes.push_back(mBuilder.getInt32(0));

	BasicBlock* endBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* firstSetBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* userModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* supervisorModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* abortModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* undefinedModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* irqModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* fiqModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	SwitchInst* sw = mBuilder.CreateSwitch(currentMode, userModeBlock, 7);
	
	sw->addCase(mBuilder.getInt32(TARMProcessor::kUserMode), userModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kSystemMode), userModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kSupervisorMode), supervisorModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kAbortMode), abortModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kUndefinedMode), undefinedModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kIRQMode), irqModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kFIQMode), fiqModeBlock);
	
	mBuilder.SetInsertPoint(userModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[13]), mBuilder.CreateGEP(mProcessor, indexes));
	indexes[1] = mBuilder.getInt32(i_mR14_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[14]), mBuilder.CreateGEP(mProcessor, indexes));
	mBuilder.CreateBr(firstSetBlock);

	mBuilder.SetInsertPoint(supervisorModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13svc_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[13]), mBuilder.CreateGEP(mProcessor, indexes));
	indexes[1] = mBuilder.getInt32(i_mR14svc_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[14]), mBuilder.CreateGEP(mProcessor, indexes));
	mBuilder.CreateBr(firstSetBlock);

	mBuilder.SetInsertPoint(abortModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13abt_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[13]), mBuilder.CreateGEP(mProcessor, indexes));
	indexes[1] = mBuilder.getInt32(i_mR14abt_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[14]), mBuilder.CreateGEP(mProcessor, indexes));
	mBuilder.CreateBr(firstSetBlock);

	mBuilder.SetInsertPoint(undefinedModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13und_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[13]), mBuilder.CreateGEP(mProcessor, indexes));
	indexes[1] = mBuilder.getInt32(i_mR14und_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[14]), mBuilder.CreateGEP(mProcessor, indexes));
	mBuilder.CreateBr(firstSetBlock);

	mBuilder.SetInsertPoint(irqModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13irq_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[13]), mBuilder.CreateGEP(mProcessor, indexes));
	indexes[1] = mBuilder.getInt32(i_mR14irq_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[14]), mBuilder.CreateGEP(mProcessor, indexes));
	mBuilder.CreateBr(firstSetBlock);

	mBuilder.SetInsertPoint(firstSetBlock);
	for (int i = 8; i < 13; i++) {
		indexes[1] = mBuilder.getInt32(i_mR8_Bkup - 8 + i);
		mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[i]), mBuilder.CreateGEP(mProcessor, indexes));
	}
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(fiqModeBlock);
	for (int i = 8; i < 15; i++) {
		indexes[1] = mBuilder.getInt32(i_mR8fiq_Bkup - 8 + i);
		mBuilder.CreateStore(mBuilder.CreateLoad(mRegisters[i]), mBuilder.CreateGEP(mProcessor, indexes));
	}
	mBuilder.CreateBr(endBlock);
	
	mBuilder.SetInsertPoint(endBlock);
}

// -------------------------------------------------------------------------- //
//  * BuildRestoreBankRegisters
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::BuildRestoreBankRegisters(Value* currentMode, Value* newMode) {
	for (int i = 8; i < 15; i++) {
		EnsureAllocated(&mRegisters[i], 32);
	}

	std::vector<llvm::Value *> indexes;
	indexes.push_back(mBuilder.getInt32(0));
	indexes.push_back(mBuilder.getInt32(0));

	BasicBlock* endBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* restoreFIQBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* restoreSwitchBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* restoreFirstSetBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);

	BasicBlock* userModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* supervisorModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* abortModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* undefinedModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* irqModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* fiqModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);

    // Old mode was FIQ, new mode is not FIQ : restore R8-R12
    mBuilder.CreateCondBr(
        mBuilder.CreateAnd(
            mBuilder.CreateICmpNE(currentMode, newMode),
            mBuilder.CreateICmpEQ(currentMode, mBuilder.getInt32(TARMProcessor::kFIQMode))),
        restoreFirstSetBlock,
        restoreSwitchBlock);

    mBuilder.SetInsertPoint(restoreFirstSetBlock);
	for (int i = 8; i < 13; i++) {
		indexes[1] = mBuilder.getInt32(i_mR8_Bkup - 8 + i);
		mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[i]);
	}
	mBuilder.CreateBr(restoreSwitchBlock);
    
    mBuilder.SetInsertPoint(restoreSwitchBlock);
	SwitchInst* sw = mBuilder.CreateSwitch(newMode, userModeBlock, 7);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kUserMode), userModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kSystemMode), userModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kSupervisorMode), supervisorModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kAbortMode), abortModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kUndefinedMode), undefinedModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kIRQMode), irqModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kFIQMode), fiqModeBlock);
    
	mBuilder.SetInsertPoint(userModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[13]);
	indexes[1] = mBuilder.getInt32(i_mR14_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[14]);
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(supervisorModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13svc_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[13]);
	indexes[1] = mBuilder.getInt32(i_mR14svc_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[14]);
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(abortModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13abt_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[13]);
	indexes[1] = mBuilder.getInt32(i_mR14abt_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[14]);
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(undefinedModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13und_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[13]);
	indexes[1] = mBuilder.getInt32(i_mR14und_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[14]);
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(irqModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13irq_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[13]);
	indexes[1] = mBuilder.getInt32(i_mR14irq_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[14]);
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(fiqModeBlock);
	indexes[1] = mBuilder.getInt32(i_mR13fiq_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[13]);
	indexes[1] = mBuilder.getInt32(i_mR14fiq_Bkup);
	mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[14]);
	mBuilder.CreateCondBr(
	    mBuilder.CreateICmpNE(currentMode, newMode),
	    restoreFIQBlock,
	    endBlock);

	mBuilder.SetInsertPoint(restoreFIQBlock);
	// Restoring FIQ saved registers as we are entering FIQ mode
	for (int i = 8; i < 13; i++) {
		indexes[1] = mBuilder.getInt32(i_mR8fiq_Bkup - 8 + i);
		mBuilder.CreateStore(mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes)), mRegisters[i]);
	}
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(endBlock);
}

// -------------------------------------------------------------------------- //
//  * BuildSetCPSR
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::BuildSetCPSR(Value* operand, Value* currentMode, Value* doControlBits, Value* doFieldsBits, Value* exitPC) {
	std::vector<llvm::Value *> indexes;
	indexes.push_back(mBuilder.getInt32(0));
	indexes.push_back(mBuilder.getInt32(0));

	// First set field bits.
	BasicBlock* setFieldsBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* afterSetFieldsBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* setControlBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* setModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* setPrivilegeBitsBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* signalInterruptExit = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* endBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	mBuilder.CreateCondBr(doFieldsBits, setFieldsBlock, afterSetFieldsBlock);
	mBuilder.SetInsertPoint(setFieldsBlock);
	EnsureAllocated(&mCPSR_N, 1);
	EnsureAllocated(&mCPSR_Z, 1);
	EnsureAllocated(&mCPSR_C, 1);
	EnsureAllocated(&mCPSR_V, 1);
	mBuilder.CreateStore(mBuilder.CreateTrunc(
		mBuilder.CreateLShr(operand, mBuilder.getInt32(31)), IntegerType::getInt1Ty(mContext)), mCPSR_N);
	mBuilder.CreateStore(mBuilder.CreateTrunc(
		mBuilder.CreateLShr(operand, mBuilder.getInt32(30)), IntegerType::getInt1Ty(mContext)), mCPSR_Z);
	mBuilder.CreateStore(mBuilder.CreateTrunc(
		mBuilder.CreateLShr(operand, mBuilder.getInt32(29)), IntegerType::getInt1Ty(mContext)), mCPSR_C);
	mBuilder.CreateStore(mBuilder.CreateTrunc(
		mBuilder.CreateLShr(operand, mBuilder.getInt32(28)), IntegerType::getInt1Ty(mContext)), mCPSR_V);
	mBuilder.CreateBr(afterSetFieldsBlock);
	mBuilder.SetInsertPoint(afterSetFieldsBlock);
	mBuilder.CreateCondBr(doControlBits, setControlBlock, endBlock);

	mBuilder.SetInsertPoint(setControlBlock);
	Value* newMode = mBuilder.CreateAnd(operand, mBuilder.getInt32(TARMProcessor::kPSR_ModeMask));
	mBuilder.CreateCondBr(mBuilder.CreateICmpEQ(newMode, currentMode), setPrivilegeBitsBlock, setModeBlock);
	
	mBuilder.SetInsertPoint(setModeBlock);
	BuildBackupBankRegisters(currentMode);
	BuildRestoreBankRegisters(currentMode, newMode);
	indexes[1] = mBuilder.getInt32(i_mMode);
	mBuilder.CreateStore(newMode, mBuilder.CreateGEP(mProcessor, indexes));
	
	Function* setPrivilegeFunc = EnsureFunction(GetSetPrivilegeFuncType(), "JIT_SetPrivilege");
	mBuilder.CreateCall2(setPrivilegeFunc, mProcessor, mBuilder.CreateICmpNE(newMode, mBuilder.getInt32(TARMProcessor::kUserMode)));
	mBuilder.CreateBr(setPrivilegeBitsBlock);
	
	mBuilder.SetInsertPoint(setPrivilegeBitsBlock);
	indexes[1] = mBuilder.getInt32(i_mCPSR_I);
	Value* newCPSR_I = mBuilder.CreateTrunc(mBuilder.CreateLShr(operand, mBuilder.getInt32(7)), IntegerType::getInt1Ty(mContext));
	mBuilder.CreateStore(newCPSR_I, mBuilder.CreateGEP(mProcessor, indexes));
	indexes[1] = mBuilder.getInt32(i_mCPSR_F);
	Value* newCPSR_F = mBuilder.CreateTrunc(mBuilder.CreateLShr(operand, mBuilder.getInt32(6)), IntegerType::getInt1Ty(mContext));
	mBuilder.CreateStore(newCPSR_F, mBuilder.CreateGEP(mProcessor, indexes));
	indexes[1] = mBuilder.getInt32(i_mCPSR_T);
	mBuilder.CreateStore(
		mBuilder.CreateTrunc(mBuilder.CreateLShr(operand, mBuilder.getInt32(5)), IntegerType::getInt1Ty(mContext)),
		mBuilder.CreateGEP(mProcessor, indexes));

	indexes[1] = mBuilder.getInt32(i_mPendingInterrupts);
	Value* mPendingInterrupts = mBuilder.CreateLoad(mBuilder.CreateGEP(mProcessor, indexes));
	Value* mask;
	mask = mBuilder.CreateSelect(newCPSR_F, mBuilder.getInt32(0), mBuilder.getInt32(TARMProcessor::kFIQInterrupt));
	mask = mBuilder.CreateOr(mask, mBuilder.CreateSelect(newCPSR_I, mBuilder.getInt32(0), mBuilder.getInt32(TARMProcessor::kIRQInterrupt)));
	mBuilder.CreateCondBr(mBuilder.CreateICmpNE(mBuilder.getInt32(0), mBuilder.CreateAnd(mPendingInterrupts, mask)), signalInterruptExit, endBlock);
	
	mBuilder.SetInsertPoint(signalInterruptExit);
	BuildExitToFunction("JIT_SignalInterrupt", exitPC);
	
	mBuilder.SetInsertPoint(endBlock);
}

// -------------------------------------------------------------------------- //
//  * BuildSetSPSR
// -------------------------------------------------------------------------- //
void
TJITLLVMTranslator::FrameTranslator::BuildSetSPSR(Value* operand, Value* currentMode) {
	std::vector<llvm::Value *> indexes;
	indexes.push_back(mBuilder.getInt32(0));
	indexes.push_back(mBuilder.getInt32(0));

	BasicBlock* endBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* supervisorModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* abortModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* undefinedModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* irqModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	BasicBlock* fiqModeBlock = BasicBlock::Create(mContext, ANONBLOCK, mFunction);
	SwitchInst* sw = mBuilder.CreateSwitch(currentMode, endBlock, 5);
	
	sw->addCase(mBuilder.getInt32(TARMProcessor::kSupervisorMode), supervisorModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kAbortMode), abortModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kUndefinedMode), undefinedModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kIRQMode), irqModeBlock);
	sw->addCase(mBuilder.getInt32(TARMProcessor::kFIQMode), fiqModeBlock);
	
	mBuilder.SetInsertPoint(supervisorModeBlock);
	indexes[1] = mBuilder.getInt32(i_mSPSRsvc);
	mBuilder.CreateStore(operand, mBuilder.CreateGEP(mProcessor, indexes));
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(abortModeBlock);
	indexes[1] = mBuilder.getInt32(i_mSPSRabt);
	mBuilder.CreateStore(operand, mBuilder.CreateGEP(mProcessor, indexes));
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(undefinedModeBlock);
	indexes[1] = mBuilder.getInt32(i_mSPSRund);
	mBuilder.CreateStore(operand, mBuilder.CreateGEP(mProcessor, indexes));
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(irqModeBlock);
	indexes[1] = mBuilder.getInt32(i_mSPSRirq);
	mBuilder.CreateStore(operand, mBuilder.CreateGEP(mProcessor, indexes));
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(fiqModeBlock);
	indexes[1] = mBuilder.getInt32(i_mSPSRfiq);
	mBuilder.CreateStore(operand, mBuilder.CreateGEP(mProcessor, indexes));
	mBuilder.CreateBr(endBlock);

	mBuilder.SetInsertPoint(endBlock);
}

// -------------------------------------------------------------------------- //
//  * GetShiftNoCarryNoR15(KUInt32)
// -------------------------------------------------------------------------- //
Value*
TJITLLVMTranslator::FrameTranslator::GetShiftNoCarryNoR15(KUInt32 inInstruction)
{
	KUInt32 Shift = ((inInstruction & 0x00000FFF) >> 5);
	KUInt32 Rm = inInstruction & 0x0000000F;
	
	// Shift is not 0 here.
	assert(Shift > 0);
	// Rm is not 15 here.
	assert(Rm < 15);
	EnsureAllocated(&mRegisters[Rm], 32);
	Value* theResult = mBuilder.CreateLoad(mRegisters[Rm]);
	// Shift Amount
	KUInt32 amount = Shift >> 2;
	
	// Switch on the shift operation.
	switch (Shift & 0x3) // 0b0000011
	{
		case 0x0:
			// Logical Shift Left
			if (amount != 0)
			{
				if (amount < 32)
				{
					theResult = mBuilder.CreateShl(theResult, mBuilder.getInt32(amount));
				} else {
					theResult = mBuilder.getInt32(0);
				}
			}
			break;
			
		case 0x1:
			// Logical Shift Right
			if (amount != 0)
			{
				if (amount < 32)
				{
					theResult = mBuilder.CreateLShr(theResult, mBuilder.getInt32(amount));
				} else {
					theResult = mBuilder.getInt32(0);
				}
			} else {
				// LSR #0 is a special case meaning LSR #32
				// where the carry out is the bit 31 of Rm and
				// the result is 0.
				theResult = mBuilder.getInt32(0);
			}
			break;
			
		case 0x2:
			// Arithmetic Shift Right
			if (amount == 0)
			{
				// ASR #0 is a special case meaning ASR #32.
				theResult = mBuilder.CreateSelect(mBuilder.CreateICmpSLT(theResult, mBuilder.getInt32(0)),
												  mBuilder.getInt32(0xFFFFFFFF),
												  mBuilder.getInt32(0));
			} else {
				theResult = mBuilder.CreateAShr(theResult, mBuilder.getInt32(amount));
			}
			break;
			
		case 0x3:
			// Rotate Right
			if (amount != 0)
			{
				theResult = mBuilder.CreateOr(
					mBuilder.CreateLShr(theResult, mBuilder.getInt32(amount)),
					mBuilder.CreateShl(theResult, mBuilder.getInt32(32 - amount)));
			} else {
				// ROR #0 actually is RRX
				EnsureAllocated(&mCPSR_C, 1);
				theResult = mBuilder.CreateSelect(mBuilder.CreateLoad(mCPSR_C),
												  mBuilder.CreateOr(mBuilder.CreateLShr(theResult, mBuilder.getInt32(1)),
																	mBuilder.getInt32(0x80000000)),
												  mBuilder.CreateLShr(theResult, mBuilder.getInt32(1)));
			}
			break;
	}
	
	return theResult;
}

// -------------------------------------------------------------------------- //
//  * GetTMemoryPtrType()
// -------------------------------------------------------------------------- //
PointerType*
TJITLLVMTranslator::FrameTranslator::GetTMemoryPtrType() {
	if (mTMemoryPtrType == nullptr) {
		StructType* tMemoryType = StructType::create(mContext, "TMemory");
		mTMemoryPtrType = PointerType::get(tMemoryType, 0);
	}
	return mTMemoryPtrType;
}

// -------------------------------------------------------------------------- //
//  * GetTARMProcessorPtrType()
// -------------------------------------------------------------------------- //
PointerType*
TJITLLVMTranslator::FrameTranslator::GetTARMProcessorPtrType() {
	 // We consider functions return int8* and we will use bitcast.
	 // LLVM functions only know about the processor registers
	if (mTARMProcessorPtrType == nullptr) {
		StructType* tARMProcessorType = StructType::create(
			"TARMProcessor",
			IntegerType::get(mContext, 32),  // mCurrentRegisters[0]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[1]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[2]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[3]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[4]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[5]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[6]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[7]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[8]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[9]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[10]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[11]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[12]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[13]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[14]
			IntegerType::get(mContext, 32),  // mCurrentRegisters[15]
			IntegerType::get(mContext, 1),   // mCPSR_N
			IntegerType::get(mContext, 1),   // mCPSR_Z
			IntegerType::get(mContext, 1),   // mCPSR_C
			IntegerType::get(mContext, 1),   // mCPSR_V
			IntegerType::get(mContext, 1),   // mCPSR_I
			IntegerType::get(mContext, 1),   // mCPSR_F
			IntegerType::get(mContext, 1),   // mCPSR_T
			IntegerType::get(mContext, 32),  // mR8_Bkup
			IntegerType::get(mContext, 32),  // mR9_Bkup
			IntegerType::get(mContext, 32),  // mR10_Bkup
			IntegerType::get(mContext, 32),  // mR11_Bkup
			IntegerType::get(mContext, 32),  // mR12_Bkup
			IntegerType::get(mContext, 32),  // mR13_Bkup
			IntegerType::get(mContext, 32),  // mR14_Bkup
			IntegerType::get(mContext, 32),  // mR13svc_Bkup
			IntegerType::get(mContext, 32),  // mR14svc_Bkup
			IntegerType::get(mContext, 32),  // mR13abt_Bkup
			IntegerType::get(mContext, 32),  // mR14abt_Bkup
			IntegerType::get(mContext, 32),  // mR13und_Bkup
			IntegerType::get(mContext, 32),  // mR14und_Bkup
			IntegerType::get(mContext, 32),  // mR13irq_Bkup
			IntegerType::get(mContext, 32),  // mR14irq_Bkup
			IntegerType::get(mContext, 32),  // mR8fiq_Bkup
			IntegerType::get(mContext, 32),  // mR9fiq_Bkup
			IntegerType::get(mContext, 32),  // mR10fiq_Bkup
			IntegerType::get(mContext, 32),  // mR11fiq_Bkup
			IntegerType::get(mContext, 32),  // mR12fiq_Bkup
			IntegerType::get(mContext, 32),  // mR13fiq_Bkup
			IntegerType::get(mContext, 32),  // mR14fiq_Bkup
			IntegerType::get(mContext, 32),  // mSPSRsvc
			IntegerType::get(mContext, 32),  // mSPSRabt
			IntegerType::get(mContext, 32),  // mSPSRund
			IntegerType::get(mContext, 32),  // mSPSRirq
			IntegerType::get(mContext, 32),  // mSPSRfiq

			IntegerType::get(mContext, 32),  // mMode
			IntegerType::get(mContext, 32),  // mPendingInterrupts
			PointerType::get(StructType::create(mContext, "TLog"), 0),
			GetTMemoryPtrType(),
			NULL);

			mTARMProcessorPtrType = PointerType::get(tARMProcessorType, 0);
	}
	return mTARMProcessorPtrType;
}

// -------------------------------------------------------------------------- //
//  * GetEntryPointFuncType(void)
// -------------------------------------------------------------------------- //
FunctionType*
TJITLLVMTranslator::FrameTranslator::GetEntryPointFuncType(void) {
	// We consider functions return int8* and we will use bitcast.
	Type* argsList[2];
	argsList[0] = GetTARMProcessorPtrType();
	argsList[1] = PointerType::getInt1PtrTy(mContext);
	ArrayRef<Type*> funcArgs(argsList);
	return FunctionType::get(PointerType::getInt8PtrTy(mContext), funcArgs, false);
}

// -------------------------------------------------------------------------- //
//  * GetEntryPointFuncType(void)
// -------------------------------------------------------------------------- //
FunctionType*
TJITLLVMTranslator::FrameTranslator::GetInnerFuncType(void) {
	// We consider functions return int8* and we will use bitcast.
	Type* argsList[3];
	argsList[0] = GetTARMProcessorPtrType();
	argsList[1] = PointerType::getInt1PtrTy(mContext);
	argsList[2] = IntegerType::getInt32Ty(mContext);
	ArrayRef<Type*> funcArgs(argsList);
	return FunctionType::get(PointerType::getInt8PtrTy(mContext), funcArgs, false);
}

// -------------------------------------------------------------------------- //
//  * GetReadBFuncType(void)
// -------------------------------------------------------------------------- //
FunctionType*
TJITLLVMTranslator::FrameTranslator::GetReadBFuncType(void) {
	Type* argsList[3];
	argsList[0] = GetTMemoryPtrType();
	argsList[1] = Type::getInt32Ty(mContext);
	argsList[2] = PointerType::getInt8PtrTy(mContext);
	ArrayRef<Type*> funcArgs(argsList);
	return FunctionType::get(Type::getInt1Ty(mContext), funcArgs, false);
}

// -------------------------------------------------------------------------- //
//  * GetWriteBFuncType(void)
// -------------------------------------------------------------------------- //
FunctionType*
TJITLLVMTranslator::FrameTranslator::GetWriteBFuncType(void) {
	Type* argsList[3];
	argsList[0] = GetTMemoryPtrType();
	argsList[1] = Type::getInt32Ty(mContext);
	argsList[2] = Type::getInt8Ty(mContext);
	ArrayRef<Type*> funcArgs(argsList);
	return FunctionType::get(Type::getInt1Ty(mContext), funcArgs, false);
}

// -------------------------------------------------------------------------- //
//  * GetReadFuncType(void)
// -------------------------------------------------------------------------- //
FunctionType*
TJITLLVMTranslator::FrameTranslator::GetReadFuncType(void) {
	Type* argsList[3];
	argsList[0] = GetTMemoryPtrType();
	argsList[1] = Type::getInt32Ty(mContext);
	argsList[2] = PointerType::getInt32PtrTy(mContext);
	ArrayRef<Type*> funcArgs(argsList);
	return FunctionType::get(Type::getInt1Ty(mContext), funcArgs, false);
}

// -------------------------------------------------------------------------- //
//  * GetWriteFuncType(void)
// -------------------------------------------------------------------------- //
FunctionType*
TJITLLVMTranslator::FrameTranslator::GetWriteFuncType(void) {
	Type* argsList[3];
	argsList[0] = GetTMemoryPtrType();
	argsList[1] = Type::getInt32Ty(mContext);
	argsList[2] = Type::getInt32Ty(mContext);
	ArrayRef<Type*> funcArgs(argsList);
	return FunctionType::get(Type::getInt1Ty(mContext), funcArgs, false);
}

// -------------------------------------------------------------------------- //
//  * GetReadBlockFuncType(void)
// -------------------------------------------------------------------------- //
FunctionType*
TJITLLVMTranslator::FrameTranslator::GetReadBlockFuncType(void) {
	Type* argsList[4];
	argsList[0] = GetTMemoryPtrType();
	argsList[1] = Type::getInt32Ty(mContext);
	argsList[2] = Type::getInt32Ty(mContext);
	argsList[3] = PointerType::getInt32PtrTy(mContext);
	ArrayRef<Type*> funcArgs(argsList);
	return FunctionType::get(Type::getInt1Ty(mContext), funcArgs, false);
}

// -------------------------------------------------------------------------- //
//  * GetWriteBlockFuncType(void)
// -------------------------------------------------------------------------- //
FunctionType*
TJITLLVMTranslator::FrameTranslator::GetWriteBlockFuncType(void) {
	Type* argsList[4];
	argsList[0] = GetTMemoryPtrType();
	argsList[1] = Type::getInt32Ty(mContext);
	argsList[2] = Type::getInt32Ty(mContext);
	argsList[3] = PointerType::getInt32PtrTy(mContext);
	ArrayRef<Type*> funcArgs(argsList);
	return FunctionType::get(Type::getInt1Ty(mContext), funcArgs, false);
}

// -------------------------------------------------------------------------- //
//  * GetSetPrivilegeFuncType(void)
// -------------------------------------------------------------------------- //
FunctionType*
TJITLLVMTranslator::FrameTranslator::GetSetPrivilegeFuncType(void) {
	Type* argsList[2];
	argsList[0] = GetTARMProcessorPtrType();
	argsList[1] = Type::getInt1Ty(mContext);
	ArrayRef<Type*> funcArgs(argsList);
	return FunctionType::get(PointerType::getInt8PtrTy(mContext), funcArgs, false);
}

// -------------------------------------------------------------------------- //
//  * CountBits( KUInt16 )
// -------------------------------------------------------------------------- //
inline KUInt32
TJITLLVMTranslator::CountBits( KUInt16 inWord )
{
	// http://www.caam.rice.edu/~dougm/twiddle/BitCount.html
#define T unsigned
#define ONES ((T)(-1))
#define TWO(k) ((T)1 << (k))
#define CYCL(k) (ONES/(1 + (TWO(TWO(k)))))
#define BSUM(x,k) ((x)+=(x) >> TWO(k), (x) &= CYCL(k))
	inWord = (inWord & CYCL(0)) + ((inWord>>TWO(0)) & CYCL(0));
	inWord = (inWord & CYCL(1)) + ((inWord>>TWO(1)) & CYCL(1));
	BSUM(inWord,2);
	BSUM(inWord,3);
	
	return inWord;
}

#endif
