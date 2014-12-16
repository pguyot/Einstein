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

// The three lines below are tricking the compiler into letting
// us handle a whole range of function calls. This may not work
// with all compilers out there!
typedef void (*AnyFunctionPtr)();
class TAnyClass { };
typedef void (TAnyClass::*AnyMethodPtr)();

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
class TJITGenericROMPatch {

protected:
	static TJITGenericROMPatch *first_;
	TJITGenericROMPatch *next_;
	KUInt32 address_;
	KUInt32 value_;
	KUInt32 originalInstruction_;
	JITFuncPtr stub_;
	AnyFunctionPtr function_;
	AnyMethodPtr method_;
	const char *name_;
	
	static TJITGenericROMPatch **patch_;
	static KUInt32 nPatch, NPatch;
	static KUInt32 addPatch(TJITGenericROMPatch *patch);
	
public:
	/// Create and add a new patch
	TJITGenericROMPatch(KUInt32 address, KUInt32 value);
	
	/// Create and add a new patch
	TJITGenericROMPatch(KUInt32 address, KUInt32 value, const char *name);
	
	/// Create and add a call to a JIT instruction
	TJITGenericROMPatch(KUInt32 address, JITFuncPtr stub, const char *name);
	
	/// Create and add a call to a Simulator function or static method
	TJITGenericROMPatch(KUInt32 address, JITFuncPtr stub, const char *name, AnyFunctionPtr function);
	
	/// Create and add a call to a Simulator class method
	TJITGenericROMPatch(KUInt32 address, JITFuncPtr stub, const char *name, AnyMethodPtr function);
	
	/// Create and add a call to a Simulator function or static method
	TJITGenericROMPatch(KUInt32 address, JITFuncPtr stub, const char *name, JITSimPtr function);
	
	/// Destructor
	virtual ~TJITGenericROMPatch();

	/// Return the first patch in the list.
	/// @return NULL if no patches
	static TJITGenericROMPatch *first() { return first_; }
	
	/// Return the next patch in the list
	/// @return NULL if no more patches in the database
	TJITGenericROMPatch *next() { return next_; }
	
	/// Return the address of this patch (must be dividable by four)
	KUInt32 address() { return address_; }
	
	/// Return the 32-bit patch value for this address
	KUInt32 value() { return value_; }
	
	/// Set the original code for this address
	void originalInstruction(KUInt32 instr) { originalInstruction_ = instr; }
	
	/// Return the Simulator stub by index
	static JITFuncPtr GetSimulatorStubAt(KUInt32 index);
	
	/// Return the Simulator function by index
	static AnyFunctionPtr GetSimulatorFunctionAt(KUInt32 index);
	
	/// Return the Simulator class method by index
	static AnyMethodPtr GetSimulatorMethodAt(KUInt32 index);
	
	/// Return the original ROM instruction
	static KUInt32 GetOriginalInstructionAt(KUInt32 command, KUInt32 address=0xFFFFFFFF);
	
	/// Return the name for this patch
	static const char* GetNameAt(KUInt32 index);
	
	/// Patch the ROM word
	virtual void apply(KUInt32 *ROM);
	
	// Get number of patches
	static KUInt32 GetNumPatches() { return nPatch; }
	
	///
	/// Loop through all patched and actually apply them.
	///
	static void DoPatchROM(KUInt32* inROMPtr, const std::string& inMachineName);
};


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


#define T_ROM_INJECTION(addr, name) \
extern JITInstructionProto(p##addr); \
TJITGenericROMInjection i##addr(addr, p##addr, name); \
JITInstructionProto(p##addr)

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
