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
#include "Emulator/JIT/JIT.h"

typedef void (*AnyFunctionPtr)();

class TJITGenericPatchObject;


/**
 \brief Manage all code patches.

 This class is completely static and does not need to be instatiated anywhere.
 It keeps track of all patches as they are created, and applies them after the
 ROM is initialized.
 */
class TJITGenericPatchManager
{

private:
	static TJITGenericPatchObject **mPatchList;
	static KUInt32 mPatchListTop, mPatchListSize;

public:
	/// Get number of patches
	static KUInt32 GetNumPatches() { return mPatchListTop; }

	/// Add a new patch to the patch list.
	static KUInt32 Add(TJITGenericPatchObject *patch);

	/// Loop through all patched and actually apply them.
	static void DoPatchROM(KUInt32* inROMPtr, const std::string& inMachineName);

	/// Get patch at a given index.
	static TJITGenericPatchObject *GetPatchAt(KUInt32 ix);
};


/**
 \brief This abstract class is the base implementation for all types of patches.

 New types of patches can be based on this class or any of the derived classes.
 The only method that must be overridden is `Apply(KUInt32 *ROM)`, which
 replaces onw instruction word in ROM with another instruction.
 */
class TJITGenericPatchObject
{
private:
	KUInt32 mIndex;
	KUInt32 mAddress;
	KUInt32 mOriginalInstruction;
	const char *mName;

	/// Add this patch to the patch manager and return the new index.
	KUInt32 AddToManager() { return TJITGenericPatchManager::Add(this); }

protected:
	// Instruction format for SWI. NewtonOS uses SWIs 0 to 34.
	// The patch manager uses SWIs with bit 23 (P) set.
	// Bit 22 indicates if the patch is applied before (native code injection,
	// I=1) or instead (native code call, I=0) of an instruction.
	// -Cond-- 1  1  1  1   P  N -----------ignored by processor------------------ Software Interrupt
	static const KUInt32 kPatchMask 			= 0xFF800000;
	static const KUInt32 kNativeMask 			= 0xFFC00000;
	static const KUInt32 kSWIIndexMask 			= 0x003FFFFF;
	static const KUInt32 kSWI			 		= 0xEF000000;
	static const KUInt32 kSWIPatch				= 0xEF800000;
	static const KUInt32 kSWINativeCall 		= 0xEF800000;
	static const KUInt32 kSWINativeInjection 	= 0xEFC00000;

	/// Remember the priginal instruction at this address before the patch was applied.
	void SetOrigialInstruction(KUInt32 instr) { mOriginalInstruction = instr; }

	/// Return the offset of the instruction into the ROM word array.
	KUInt32 GetOffsetInROM();

	/// Return the index into the manager
	KUInt32 GetIndex() { return mIndex; }

public:
	/// Create and add a new patch
	TJITGenericPatchObject(KUInt32 address, const char *name=0L);

	/// Destructor
	virtual ~TJITGenericPatchObject();

	/// Patch the ROM word
	virtual void Apply(KUInt32 *ROM) = 0;

	/// Call the patch code
	virtual JITUnit *Call(JITUnit *ioUnit, TARMProcessor *ioCPU) { return 0; }

	/// Return the name of this patch.
	const char *GetName() { return mName; }

	/// Return the data word at the patch address before the patch was applied.
	KUInt32 GetOriginalInstruction() { return mOriginalInstruction; }

	// return 1 if the instruction is a patch
	static inline KUInt8 IsPatch(KUInt32 instr) { return (instr&kPatchMask)==kSWIPatch; }

	// return 1 if the instruction is a native injection
	static inline KUInt8 IsNativeInjection(KUInt32 inInstruction) { return (inInstruction&kNativeMask)==kSWINativeInjection; }

	// return 1 if the instruction is a native call
	static inline KUInt8 IsNativeCall(KUInt32 inInstruction) { return (inInstruction&kNativeMask)==kSWINativeCall; }

	/// Return the index into the manager
	static KUInt32 GetIndex(KUInt32 inInstruction) { return (inInstruction&kSWIIndexMask); }
};


/**
 \brief This patch replaces one instruction in ROM with another instruction.

 This is the simpelest form of a patch. It replaces one word in ROM with a
 new word. The JIT system will interprete the new instruction instead of
 the original. No further action is taken.
 */
class TJITGenericPatch : public TJITGenericPatchObject
{
private:
	KUInt32 mNewInstruction;

protected:
	/// Return the new instruction that replaces the original one.
	KUInt32 GetNewInstruction() { return mNewInstruction; }

public:
	/// Create and add a new patch
	TJITGenericPatch(KUInt32 address, KUInt32 value, const char *name=0L);

	/// Patch the ROM word
	virtual void Apply(KUInt32 *ROM);
};


/**
 \brief Find and replace words in an area of the ROM.
 */
class TJITGenericPatchFindAndReplace : public TJITGenericPatch
{
private:
	KUInt32 mStart;
	KUInt32 mEnd;
	KUInt32 *mKey;
	KUInt32 *mReplacement;

public:
	/// Create and add a new patch
	TJITGenericPatchFindAndReplace(KUInt32 startAddress, KUInt32 endAddress,
								   KUInt32 *key, KUInt32 *replacement,
								   const char *name=0L);

	/// Patch the ROM
	virtual void Apply(KUInt32 *ROM);
};


/**
 \brief This patch type is used to call a JIT stub \b instead of an instruction.

 Use this patch type to replace the ARM instruction at the give address in ROM
 with a call to native code. The native code has full access to the entire
 emulator including CPU and Memory.

 - `Apply()` patches the current instruction with an SWI instruction, indexing
   this patch in the manager. Bit 23 is set to indicate a patch, bit 22 is
   clear to indicate a native call, not an injection (kSWINativeCall)
 - in the JIT translation phase, the instruction is transcoded into a JITUnit
   calling `CallPatchNative`, followed by a JITUnit containing the index
 - `Call()` is called when running JITUnits later in the emulation

 \see JITInstructionProto(CallPatchNative)
 \see Translate_SWIAndCoproc
 \see Translate_PatchNativeCall
 */
class TJITGenericPatchNativeCall : public TJITGenericPatchObject
{
private:
	JITFuncPtr mStub;

protected:
	/// Return the JIT Stub address for this patch.
	JITFuncPtr GetStub() { return mStub; }

public:
	/// Create and add a call to a JIT instruction
	TJITGenericPatchNativeCall(KUInt32 address, JITFuncPtr stub, const char *name)
	: TJITGenericPatchObject(address, name), mStub(stub)  { }

	/// Patch the ROM word
	virtual void Apply(KUInt32 *ROM);

	/// Call the patch code
	virtual JITUnit *Call(JITUnit *ioUnit, TARMProcessor *ioCPU);
};


/**
 \brief This Macro makes it easy to replace code anywhere in ROM.

 The original ARM command at the given address is replaced with a call into
 native code. The 'C' code can immediatly follow the Macro. The function must
 end in `return ioUnit;` if the next instruction is to be executed, or in
 `return 0L;` if the PC changed and execution shall continue elsewhere.
 Two prameters are available to the function: 'JITUnit* ioUnit' and
 'TARMProcessor* ioCPU'.

 The sample code replaces this sequence that returns
 error code -36018, "Unsupported primitive":

 \code
 // mvn     r0, #177                        @ 0x00025158
 // sub     r0, r0, #35840
 // mov     pc, lr

 T_ROM_PATCH(0x00000010, "TADSPEndpoint::RemoveFromAppWorld(void)") {
     ioCPU->SetRegister(0, -36018);
     ioCPU->SetRegister(15, ioCPU->GetRegister(14));
     return 0L;
 }
 \endcode

 \param addr this is the address in ROM that we want to patch, must
 		be word-aligned
 \param name naming the patch makes debugging easier

 */
#define T_ROM_PATCH(addr, name) \
extern JITInstructionProto(p##addr); \
TJITGenericPatchNativeCall i##addr(addr, p##addr, name); \
JITInstructionProto(p##addr)


/**
 \brief This patch type is used to call a JIT stub \b before another instruction.

 An Injection is different to a Patch. It will call native code, but then
 return and execute the original code.

 Use the patch type to place a native call just before the ARM instruction at
 the give address in ROM. The native code has full access to the entire
 emulator including CPU and Memory.

 - `Apply()` patches the current instruction with an SWI instruction, indexing
   this patch in the manager. Bit 23 is set to indicate a patch, bit 22 is
   also set to indicate an injection (kSWINativeInjection)
 - before any JIT translation, the instruction is transcoded into a JITUnit
   calling `CallPatchNative`, followed by a JITUnit containing the index. The
   translation then continuse with the original instruction at this address.
 - `Call()` is called when running JITUnits later in the emulation

 \see JITInstructionProto(CallPatchNative)
 \see Translate_SWIAndCoproc
 \see Translate_PatchNativeCall
 */
class TJITGenericPatchNativeInjection : public TJITGenericPatchNativeCall {
	
public:
	/// Create and add a call to a JIT instruction as an injection
	TJITGenericPatchNativeInjection(KUInt32 address, JITFuncPtr stub, const char *name)
	: TJITGenericPatchNativeCall(address, stub, name) { }
	
	/// Patch the ROM word
	virtual void Apply(KUInt32 *ROM);
};


/**
 \brief This Macro makes it easy to insert native code anywhere in ROM.

 The original ARM instruction at the given address is executed after the injected
 code returns. The 'C' code can immediatly follow the Macro. The function must end
 in `return ioUnit;`. Two prameters are available to the function:
 'JITUnit* ioUnit' and 'TARMProcessor* ioCPU'.

 The sample code outputs a message for each Data Abort interrupt and then
 continues execution of the ROM code.

 \code
 T_ROM_INJECTION(0x00000010, "Data Abort") {
     fprintf(stderr, "DATA ABORT at 0x%08X\n", ioCPU->mR14abt_Bkup-8);
     return ioUnit;
 }
 \endcode

 \param addr this is the address in ROM that we want to patch, must
 		be word-aligned
 \param name naming the patch makes debugging easier
 */
#define T_ROM_INJECTION(addr, name) \
extern JITInstructionProto(p##addr); \
TJITGenericPatchNativeInjection i##addr(addr, p##addr, name); \
JITInstructionProto(p##addr)


#endif
