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
#include "TJITLLVMPage.h"
#include "llvm/IR/Module.h"

class TMemory;
class TARMProcessor;

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
	/// Constructor from interfaces to memory and MMU.
	///
	/// \param inMemoryIntf	interface to memory.
	/// \param inMMUIntf	interface to the MMU.
	///
	TJITLLVM(
		TMemory* inMemoryIntf,
		TMMU* inMMUIntf );
	
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
					TMemory* inMemoryInterface,
					KUInt32 inPC );

	///
	/// Get a JIT ptr for a single instruction at a given PC.
	///
	JITFuncPtr
	GetJITFuncForSingleInstructionAtPC(
				   TARMProcessor* ioCPU,
				   TMemory* inMemoryInterface,
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

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TJITLLVM( const TJITLLVM& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TJITLLVM& operator = ( const TJITLLVM& inCopy );

	enum {
		kPoolSize = 512,	///< 512 pages in pool.
	};
	
	/// \name Variables
	
	TJITLLVMPage*	        mPagesPool;	            ///< Array with all the pages.
	llvm::FunctionType*     mEntryPointFuncType;    ///< Type of entry points (JITFuncPtr)	
};

#endif
		// _TJITLLVM_H

// ====================================================================== //
// A complex system that works is invariably found to have evolved from a //
// simple system that works.                                              //
// ====================================================================== //
