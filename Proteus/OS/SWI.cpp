//
//  SWI.cpp
//  Einstein
//
//  Created by Matthias Melcher on 4/4/19.
//

#include "SWI.h"

#include "Globals.h"


T_JIT_TO_NATIVE(0x003AD750, "_SWI_Scheduler") {
	_SWI_Scheduler();
}

// _SWI_Scheduler starts here:
void _SWI_Scheduler()
{
// /* 0x003AD750-0x003AD754 */
	PUSH(SP, R1);
	PUSH(SP, R0);
// /* 0x003AD754-0x003AD758 */
	CPU->DisableFIQ();
	CPU->DisableIRQ();
// /* 0x003AD758-0x003AD794 */
	if (   GetGAtomicFIQNestCountFast()!=0
		|| GetGAtomicIRQNestCountFast()!=0
		|| GetGAtomicNestCount()!=0
		|| GetGAtomicFIQNestCount()!=0)
		goto L003ADA74;

// /* 0x003AD794-0x003AD798 */
	CPU->EnableFIQ();
	CPU->DisableIRQ();
// /* 0x003AD798-0x003AD7A0 */
	PC = 0x003AD7A0+4;
	return;

// /* 0x003AD7A0-0x003ADA70 */

// /* 0x003ADA70-0x003ADA78 */
L003ADA74:
	PC = 0x003ADA74+4;
	return;

// /* 0x003ADA74-0x003ADB14 */
// return from SWI

// /* 0x003ADB14-0x003ADBB4 */
// return from SWI
}

