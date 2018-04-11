// ==============================
// File:			TJITGenericROMPatch.h
// Project:			Einstein
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

#ifndef _TJITGENERICROMPATCH_H
#define _TJITGENERICROMPATCH_H

#include <K/Defines/KDefinitions.h>
#include "JIT.h"

typedef void (*AnyFunctionPtr)();

class TJITGenericROMPatch;


/**
 \briefManage all code patches.

 This class is completely static and does not need to be instatiated anywhere.
 It keeps track of all patches as they are created, and applies them after the
 ROM is initialized.
 */
class TJITGenericPatchManager
{
	friend TJITGenericROMPatch;
	
private:
	static TJITGenericROMPatch **mPatchList;
	static KUInt32 mPatchListTop, mPatchListSize;

	/// Add a new patch to the patch list.
	static KUInt32 add(TJITGenericROMPatch *patch);

public:
	/// Get number of patches
	static KUInt32 GetNumPatches() { return mPatchListTop; }

	/// Get patch at a given index.
	static TJITGenericROMPatch *GetPatchAt(KUInt32 ix);

	/// Loop through all patched and actually apply them.
	static void DoPatchROM(KUInt32* inROMPtr, const std::string& inMachineName);
};


///
/// This class defines a patch location.
/// Patches can be created in any module by static declaration of TJITGenericROMPatch's
/// which will then be linked into the patch database and applied to a
/// freshly loaded ROM.
/// Usage is currently limited to ROM v717006
///
/// These ROM patches provide the original instruction and are used when
/// translating code.
///
class TJITGenericROMPatch
{
	friend TJITGenericPatchManager;

private:
	KUInt32 mIndex;
	KUInt32 mAddress;
	KUInt32 mOriginalInstruction;
	KUInt32 mNewInstruction;
	const char *mName;
	
	JITFuncPtr mStub;
	AnyFunctionPtr mFunction;

	/// Add this patch to the patch manager and return the new index.
	KUInt32 addToManager() { return TJITGenericPatchManager::add(this); }

protected:

	/// Remember the priginal instruction at this address before the patch was applied.
	void SetOrigialInstruction(KUInt32 instr) { mOriginalInstruction = instr; }

	/// Return the offset of the instruction into the ROM word array.
	KUInt32 GetOffsetInROM() { return mAddress>>2; }

	/// Return the new instruction that replaces the original one.
	KUInt32 GetNewInstruction() { return mNewInstruction; }

public:
	/// Create and add a new patch
	TJITGenericROMPatch(KUInt32 address, KUInt32 value);
	
	/// Create and add a new patch
	TJITGenericROMPatch(KUInt32 address, KUInt32 value, const char *name);
	
	/// Create and add a call to a JIT instruction
	TJITGenericROMPatch(KUInt32 address, JITFuncPtr stub, const char *name);
	
	/// Create and add a call to a Simulator function or static method
	TJITGenericROMPatch(KUInt32 address, JITFuncPtr stub, const char *name, AnyFunctionPtr function);
	
	/// Create and add a call to a Simulator function or static method
	TJITGenericROMPatch(KUInt32 address, JITFuncPtr stub, const char *name, JITSimPtr function);
	
	/// Destructor
	virtual ~TJITGenericROMPatch();

	/// Return the address of this patch (must be dividable by four)
	//KUInt32 address() { return address_; }
	
	/// Patch the ROM word
	virtual void apply(KUInt32 *ROM);

	/// Return the name of this patch.
	/// Names can be given at creation time and can be choosen artbitrarily.
	/// They are used to help in the debugging process.
	const char *GatName() { return mName; }

	/// Return the data word at the patch address before the patch was applied.
	KUInt32 GetOriginalInstruction() { return mOriginalInstruction; }

	/// Return the JIT Stub address for this patch.
	JITFuncPtr GetStub() { return mStub; }

	/// Return the function address for this patch.
	AnyFunctionPtr GetFunction() { return mFunction; }

};


/**
 This Macro makes it easy to replace code anywhere in ROM. The original ARM
 command at the given address is no longer executed.
 The C code can immediatly follow the Macro. The function must end in
 <tt>SETPC(jumpDestAddress+4); MMUCALLNEXT_AFTERSETPC;</tt>. 
 The available variables are <tt>ioCPU</tt> and <tt>ioUnit</tt>.
 */
#define T_ROM_PATCH(addr, name) \
extern JITInstructionProto(p##addr); \
TJITGenericROMPatch i##addr(addr, p##addr, name); \
JITInstructionProto(p##addr)


///
/// An Injection is different to a Patch. It will call native code, but then
/// return and execute the original code.
///
class TJITGenericROMInjection : public TJITGenericROMPatch {
	
public:
	/// Create and add a call to a JIT instruction as an injection
	TJITGenericROMInjection(KUInt32 address, JITFuncPtr stub, const char *name)
	: TJITGenericROMPatch(address, stub, name) { }
	
	TJITGenericROMInjection(KUInt32 address, JITFuncPtr stub, const char *name, JITSimPtr nativeStub)
	: TJITGenericROMPatch(address, stub, name, nativeStub) { }
	
	/// Patch the ROM word
	virtual void apply(KUInt32 *ROM);
};


/**
 This Macro makes it easy to insert code anywhere in ROM. The original ARM 
 command at the given address is executed after the injected code is run.
 The C code can immediatly follow the Macro. The function must end in
 <tt>return ioUnit;</tt>. The other available variable is <tt>ioCPU</tt>.
 */
#define T_ROM_INJECTION(addr, name) \
extern JITInstructionProto(p##addr); \
TJITGenericROMInjection i##addr(addr, p##addr, name); \
JITInstructionProto(p##addr)


/**
 (Matt: not sure anymore)
 */
#define T_ROM_INJECTION3(addr, name, nativeCall) \
extern JITInstructionProto(p##addr); \
TJITGenericROMInjection i##addr(addr, p##addr, name, nativeCall); \
JITInstructionProto(p##addr)


///
/// Native Injections are functions that can be executed in native mode (using
/// the Simulator fiber system) or in JIT mode if needed.
///
class TJITGenericROMSimulatorInjection : public TJITGenericROMPatch {
	
public:
	/// Create and add a call to a JIT instruction as an injection
	TJITGenericROMSimulatorInjection(KUInt32 address, KSInt32 (*stub)(), const char *name)
	: TJITGenericROMPatch(address, (JITFuncPtr)stub, name) { }
	
	/// Patch the ROM word
	virtual void apply(KUInt32 *ROM);
};

#endif
