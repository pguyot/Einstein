//
//  TProteusMacros.cpp
//  Einstein
//
//  Created by Matthias Melcher on 4/4/19.
//

#include "TProteusMacros.h"
#include "TProteusFiber.h"

TARMProcessor *CPU = nullptr;
TMemory *MEM = nullptr;
TInterruptManager *INT = nullptr;

/**
 * This injection initializes the Proteus system by setting up a few
 * variables for fast acces in later calls.
 */
T_ROM_INJECTION(0x00000000, "Initialize Proteus") {
	CPU = ioCPU;
	MEM = ioCPU->GetMemory();
	INT = MEM->GetInterruptManager();
	if (!svcFiber) {
		svcFiber = new TProteusFiber();
		svcFiber->Run(kFiberCallJIT);
	}
	return ioUnit;
}


