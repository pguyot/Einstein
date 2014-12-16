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



/**
 * This is a hack until we have task switching implemented
 *
 * Transcoded function _SemaphoreOpGlue
 * ROM: 0x003AE1FC - 0x003AE204
 */
void Func_0x003AE1FC(TARMProcessor* ioCPU, KUInt32 ret)
{
	KUInt32 prevLR = ioCPU->mCurrentRegisters[14];
L003AE1FC: // 0xEF00000B  swi	0x0000000b
	if (ioCPU->mCurrentRegisters[15]!=0x003AE1FC+4) __asm__("int $3\n" : : ); // be paranoid about a correct PC
	ioCPU->mCurrentRegisters[15] += 4; // update the PC
	{
		ioCPU->DoSWI();
		Func_0x00000008(ioCPU, 0x003AE204);
		if (ioCPU->mCurrentRegisters[15]!=0x003AE204) {
			RT_PANIC_UNEXPECTED_RETURN_ADDRESS
		}
	}
	if (prevLR != ioCPU->mCurrentRegisters[14]) UJITGenericRetargetSupport::ReturnToEmulator(ioCPU, 0x003AE200);
L003AE200: // 0xE1A0F00E  mov	pc, lr
	if (ioCPU->mCurrentRegisters[15]!=0x003AE200+4) __asm__("int $3\n" : : ); // be paranoid about a correct PC
	ioCPU->mCurrentRegisters[15] += 4; // update the PC
	{
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[14];
		const KUInt32 theResult = Opnd2;
		SETPC(theResult + 4);
		if (ret==0xFFFFFFFF)
			return; // Return to emulator
		if (ioCPU->mCurrentRegisters[15]!=ret)
			__asm__("int $3\n" : : ); // Unexpected return address
		return;
	}
	__asm__("int $3\n" : : ); // There was no return instruction found
}




