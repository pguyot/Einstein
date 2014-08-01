//
//  SimHandcoded.cpp
//  Einstein
//
//  Created by Matthias Melcher on 7/30/14.
//
//

#include "SimHandcoded.h"


/**
 * Transcoded function ThrowSystemPainc_ExitAtomic_Underflow
 * ROM: 0x00392C2C - 0x00392C4C
 *
 * This function is NOT hooked into the ROM, however it must be available
 * for _ExitAtomicFast() and others.
 */
void Func_0x00392C2C(TARMProcessor* ioCPU)
{
	ioCPU->ReturnToEmualtor(0x00392C2C);
}
//T_ROM_SIMULATION(0x00392C2C, "ThrowSystemPainc_ExitAtomic_Underflow")

/**
 * Transcoded function DoSchedulerSWI
 * ROM: 0x003AD658 - 0x003AD660
 *
 * This function is part of a cyclic dependency and can't be implemented yet.
 *
 * This function is NOT hooked into the ROM, however it must be available
 * for _ExitAtomicFast() and others.
 */
void Func_0x003AD658(TARMProcessor* ioCPU)
{
	ioCPU->ReturnToEmualtor(0x003AD658);
}
//T_ROM_SIMULATION(0x003AD658, "DoSchedulerSWI")

// sprintf
void Func_0x0033E620(TARMProcessor* ioCPU)
{
	ioCPU->ReturnToEmualtor(0x0033DC28);
}
