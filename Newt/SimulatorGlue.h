//
//  TTask.h
//  Einstein
//
//  Created by Matthias Melcher on 7/11/14.
//
//

#ifndef EINSTEIN_NEWT_SIMULATOR_GLUE_H
#define EINSTEIN_NEWT_SIMULATOR_GLUE_H


#ifdef IS_NEWT_SIM

#include "NewtSim.h"
#include <MacTypes.h>


/**
 * This macro declares global variables for access through the simulator.
 * \param type is the C++ type of the variable, KUInt32 if unknown.
 * \param name is the name of the variable.
 *   A constant numeric value, prefixed with ptr_ will be generated, pointing
 *   to the memory address of teh variable (real or simulated).
 * \param addr is the address in emulated memory.
 *   This parameter is ignored when compiled in native code.
 */
#define NEWT_DEF_GLOBAL(type, name, addr) \
extern type name; \
extern const KUInt32 ptr_##name;

#define NEWT_IMP_GLOBAL(type, name, addr) \
type name = 0; \
KUInt32 const ptr_##name = (KUInt32)(&name);

#else

#include <K/Defines/KDefinitions.h>
#include "TMemory.h"
#include "TARMProcessor.h"
#include "TJITGeneric_Macros.h"
#include "Emulator/JIT/Generic/TJITGenericROMPatch.h"
#include "Emulator/JIT/Generic/UJITGenericRetargetSupport.h"

#if 0
#define T_ROM_SIMULATION(addr, name) \
	T_ROM_INJECTION(addr, name) {\
		if (ioCPU->GetMode()==TARMProcessor::kUserMode) {\
			return ioUnit;\
		} else {\
			Func_##addr(ioCPU);\
			return 0;\
		}\
	}
#else
#define T_ROM_SIMULATION(addr, name) \
	T_ROM_INJECTION(addr, name) {\
		Func_##addr(ioCPU, 0xFFFFFFFF);\
		return 0;\
	}
#define T_ROM_SIMULATION3(addr, name, nativeCall) \
	T_ROM_INJECTION3(addr, name, nativeCall) {\
		Func_##addr(ioCPU, 0xFFFFFFFF);\
		return 0;\
	}
#endif

#define RT_PANIC_UNEXPECTED_RETURN_ADDRESS \
	UJITGenericRetargetSupport::UnexpectedPC(ioCPU);

#endif


#define NEWT_DEF_GLOBAL(type, name, addr) \
extern type name; \
extern const KUInt32 ptr_##name;

#define NEWT_IMP_GLOBAL(type, name, addr) \
KUInt32 const ptr_##name = (addr);

#define R0 ioCPU->mCurrentRegisters[0]
#define R1 ioCPU->mCurrentRegisters[1]
#define R2 ioCPU->mCurrentRegisters[2]
#define R3 ioCPU->mCurrentRegisters[3]
#define R4 ioCPU->mCurrentRegisters[4]
#define R5 ioCPU->mCurrentRegisters[5]
#define R6 ioCPU->mCurrentRegisters[6]
#define R7 ioCPU->mCurrentRegisters[7]
#define R8 ioCPU->mCurrentRegisters[8]
#define R9 ioCPU->mCurrentRegisters[9]
#define R10 ioCPU->mCurrentRegisters[10]
#define R11 ioCPU->mCurrentRegisters[11]
#define R12 ioCPU->mCurrentRegisters[12]
#define SP ioCPU->mCurrentRegisters[13]
#define LR ioCPU->mCurrentRegisters[14]
#define PC ioCPU->mCurrentRegisters[15]


#include "unsorted/unsorted_001.h"
#include "unsorted/unsorted_002.h"
#include "unsorted/unsorted_003.h"
#include "unsorted/unsorted_004.h"
#include "unsorted/unsorted_005.h"
#include "unsorted/unsorted_006.h"
#include "unsorted/unsorted_007.h"
#include "unsorted/unsorted_008.h"
#include "unsorted/unsorted_009.h"
#include "unsorted/unsorted_010.h"
#include "unsorted/unsorted_011.h"
#include "unsorted/unsorted_012.h"
#include "unsorted/unsorted_013.h"
#include "unsorted/unsorted_014.h"
#include "unsorted/unsorted_015.h"
#include "unsorted/unsorted_016.h"
#include "unsorted/unsorted_017.h"
#include "unsorted/unsorted_018.h"
#include "unsorted/unsorted_019.h"
#include "unsorted/unsorted_020.h"
#include "unsorted/unsorted_021.h"
#include "unsorted/unsorted_022.h"
#include "unsorted/unsorted_023.h"
#include "unsorted/unsorted_024.h"
#include "unsorted/unsorted_025.h"

#include "core/swi.h"

#include "classes/TSingleQContainer.h"
#include "classes/TInterpreter.h"
#include "classes/TSerialChipVoyager.h"

#include "SimHandcoded.h"

#endif /* EINSTEIN_NEWT_SIMULATOR_GLUE_H */
