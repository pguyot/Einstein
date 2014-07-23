//
//  TTask.h
//  Einstein
//
//  Created by Matthias Melcher on 7/11/14.
//
//

#ifndef EINSTEIN_NEWT_SIMULATOR_GLUE_H
#define EINSTEIN_NEWT_SIMULATOR_GLUE_H

#include <K/Defines/KDefinitions.h>
#include "TROMImage.h"

// Einstein
#include "TMemory.h"
#include "TARMProcessor.h"
#include "TJITGeneric_Macros.h"


#define T_ROM_SIMULATION(addr, name) \
	T_ROM_INJECTION(addr, name) {\
		if (ioCPU->GetMode()==TARMProcessor::kUserMode) {\
			return ioUnit;\
		} else {\
			Func_##addr(ioCPU);\
			return 0;\
		}\
	}

#endif /* EINSTEIN_NEWT_SIMULATOR_GLUE_H */
