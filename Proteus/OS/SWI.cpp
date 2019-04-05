//
//  SWI.cpp
//  Einstein
//
//  Created by Matthias Melcher on 4/4/19.
//

#include "SWI.h"

#include "Globals.h"


void DoDeferrals()
{
	FindFiber()->SwitchToJIT(0x00148298);
}

TTask *Scheduler()
{
	FindFiber()->SwitchToJIT(0x001CC1EC);
	return (TTask*)(uintptr_t)R0;
}

void StartScheduler()
{
	FindFiber()->SwitchToJIT(0x001CC4A8);
}




T_JIT_TO_NATIVE(0x003AD750, "_SWI_Scheduler") {
	_SWI_Scheduler();
}

// _SWI_Scheduler starts here:
void _SWI_Scheduler()
{
	TTask *task;
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
// /* 0x003AD798-0x003AD7B0 */
	if (GetGWantDeferred()!=0)
		goto L003AD7C8;
// /* 0x003AD7B0-0x003AD7C0 */
	if (GetGSchedule()==0)
		goto L003ADA74;
// /* 0x003AD7C0-0x003AD7C4 */
	PUSH(SP, LR);
	PUSH(SP, R12);
	PUSH(SP, R11);
	PUSH(SP, R10);
	PUSH(SP, R3);
	PUSH(SP, R2);
// /* 0x003AD7C4-0x003AD7C8 */
	goto L003AD7F8;
// /* 0x003AD7C8-0x003AD7CC */
L003AD7C8:
	PUSH(SP, LR);
	PUSH(SP, R12);
	PUSH(SP, R11);
	PUSH(SP, R10);
	PUSH(SP, R3);
	PUSH(SP, R2);
// /* 0x003AD7CC-0x003AD7D0 */
	CPU->EnableFIQ();
	CPU->EnableIRQ();
// /* 0x003AD7D0-0x003AD7DC */
	DoDeferrals();
// /* 0x003AD7DC-0x003AD7E0 */
	CPU->EnableFIQ();
	CPU->DisableIRQ();
// /* 0x003AD7E0-0x003AD7F8 */
	if (GetGSchedule()==0)
		goto L003ADA70;
// /* 0x003AD7F8-0x003AD7FC */
L003AD7F8:
	task = Scheduler();
	R0 = (KUInt32)(uintptr_t)task;
// /* 0x003AD7FC-0x003AD80C */
	if (GetGWantSchedulerToRun()==0)
		goto L003AD818;
// /* 0x003AD80C-0x003AD810 */
	PUSH(SP, R0);
// /* 0x003AD810-0x003AD814 */
	StartScheduler();
// /* 0x003AD814-0x003AD818 */
	POP(SP, R0);
// /* 0x003AD818-0x003AD81C */
L003AD818:
// /* 0x003AD81C-0x003AD820 */
	R1 = (KUInt32)(uintptr_t)GetGCurrentTask();
// /* 0x003AD820-0x003AD828 */
	if (R0==R1)
		goto L003ADA70;
// /* 0x003AD828-0x003AD82C */
	R2 = 0x0C100FF8;
// /* 0x003AD82C-0x003AD830 */
	SetGCurrentTask((TTask*)(uintptr_t)R0);
// /* 0x003AD830-0x003AD834 */
	if (R1==0) {
// /* 0x003AD834-0x003AD838 */
		SP += 32;
// /* 0x003AD838-0x003AD83C */
		goto L003AD990;
	}
// /* 0x003AD83C-0x003AD840 */
	POP(SP, R2);
	POP(SP, R3);
	POP(SP, R10);
	POP(SP, R11);
	POP(SP, R12);
	POP(SP, LR);
// /* 0x003AD840-0x003AD844 */
	R0 = R1;
// /* 0x003AD844-0x003AD84C */
	R1 = GetGCopyDone();
// /* 0x003AD84C-0x003AD854 */
	if (GetGCopyDone()!=0)
		goto L003AD8E0;
// /* 0x003AD854-0x003AD858 */
	R1 = CPU->GetSPSR();
// /* 0x003AD858-0x003AD85C */
	POKE_W(R0+80, R1); // oldTask->SetPSR(R1);
	PC = 0x003AD854+4;
	return;
// /* 0x003AD85C-0x003AD8E0 */
	
// /* 0x003AD8E0-0x003AD8E4 */
L003AD8E0:
	PC = 0x003AD8E0+4;
	return;
// /* 0x003AD8E4-0x003AD990 */

// /* 0x003AD990-0x003AD994 */
L003AD990:
	PC = 0x003AD990+4;
	return;

// /* 0x003AD994-0x003ADA70 */

// /* 0x003ADA70-0x003ADA74 */
L003ADA70:
	POP(SP, R2);
	POP(SP, R3);
	POP(SP, R10);
	POP(SP, R11);
	POP(SP, R12);
	POP(SP, LR);

// /* 0x003ADA74-0x003ADA78 */
L003ADA74:
	PC = 0x003ADA74+4;
	return;
// /* 0x003ADA78-0x003ADB14 */
// return from SWI

// /* 0x003ADB14-0x003ADBB4 */
// return from SWI
}

