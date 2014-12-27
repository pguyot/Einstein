// ==============================
// File:			TJITLLVMOptimizeReadPass.h
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

#ifndef _TJITLLVMOPTIMIZEREADPASS_H
#define _TJITLLVMOPTIMIZEREADPASS_H

// llvm
#include <llvm/Pass.h>

class TMemory;

///
/// Pass to optimize read operations.
/// Combined with constant propagation, this pass will optimize the following:
/// add r0, pc, #n
/// ldm r0, {r1,r2}
///
/// This pass generates more constants and make functions redundant
/// (typically returning after a data abort that will not happen).
///
class TJITLLVMOptimizeReadPass : public llvm::BasicBlockPass {
public:
	///
	/// Constructor from a memory interface.
	///
	TJITLLVMOptimizeReadPass(const TMemory& inMemoryIntf, const TJITLLVMPage& inPage) :
	llvm::BasicBlockPass(ID),
	mMemoryIntf(inMemoryIntf),
	mPage(inPage) {};
	
	virtual ~TJITLLVMOptimizeReadPass() {};
	
	///
	/// Perform optimization.
	/// Returns true if the block was modified.
	///
	bool runOnBasicBlock(llvm::BasicBlock&) override;

private:
	const TMemory&		mMemoryIntf;
	const TJITLLVMPage&	mPage;
	static char			ID;
};

#endif
