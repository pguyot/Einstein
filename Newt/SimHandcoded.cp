//
//  SimHandcoded.cpp
//  Einstein
//
//  Created by Matthias Melcher on 7/30/14.
//
//

#include "SimHandcoded.h"

#ifdef IS_NEWT_SIM
#include <stdlib.h>
#endif


// keep this empty if possible


/**
 This is a little trick to make SWI functions return to the SWI dispatch.
 See 003AD698-003AE158 SWIBoot, 003ADD3C-003ADD50 SWI0_GetPort, etc.
 */
void Func_0x003AD750(TARMProcessor* ioCPU, KUInt32 ret)
{
	return;
}







