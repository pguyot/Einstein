//
//  SWI.cpp
//  Einstein
//
//  Created by Matthias Melcher on 4/4/19.
//

#include "SWI.h"

#include "Globals.h"
#include "KernelTask.h"

namespace NewtOS {

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

	void SwapInGlobals(TTask *inTask)
	{
		R0 = (KUInt32)(uintptr_t)inTask;
		FindFiber()->SwitchToJIT(0x0025215C);
	}



T_JIT_TO_NATIVE(0x003AD750, "_SWI_Scheduler") {
	_SWI_Scheduler();
}

// _SWI_Scheduler starts here:
void _SWI_Scheduler()
{
	TTask *oldTask, *newTask, *mTask;
	KUInt32 oldCPUMode;
	TEnvironment *newEnvironment, *newSharedMemeoryEnvironment;
	bool eqflag;
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
	newTask = Scheduler();
	R0 = (KUInt32)(uintptr_t)newTask;
// /* 0x003AD7FC-0x003AD80C */
	if (GetGWantSchedulerToRun()==0)
		goto L003AD818;
// /* 0x003AD80C-0x003AD810 */
	PUSH(SP, R0); // R0=newTask;
// /* 0x003AD810-0x003AD814 */
	StartScheduler();
// /* 0x003AD814-0x003AD818 */
	POP(SP, R0); // R0=newTask;
// /* 0x003AD818-0x003AD81C */
L003AD818:
// /* 0x003AD81C-0x003AD820 */
	oldTask = GetGCurrentTask();
	R1 = (KUInt32)(uintptr_t)oldTask;
// /* 0x003AD820-0x003AD828 */
	if ( oldTask==newTask )
		goto L003ADA70;
// /* 0x003AD828-0x003AD82C */
	R2 = 0x0C100FF8;
// /* 0x003AD82C-0x003AD830 */
	SetGCurrentTask( newTask ); // R0=newTask;
// /* 0x003AD830-0x003AD834 */
	if ( oldTask==nullptr ) {
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
	R0 = R1; // R0 = oldTask;
// /* 0x003AD844-0x003AD854 */
	if (GetGCopyDone()!=0)
		goto L003AD8E0;
// /* 0x003AD854-0x003AD858 */
	R1 = CPU->GetSPSR();
// /* 0x003AD858-0x003AD85C */
	oldTask->SetPSR( R1 );
// /* 0x003AD85C-0x003AD860 */
	R1 = R1 & 0x1F;
// /* 0x003AD860-0x003AD864 */
	CPU->mCPSR_Z = (R1==0x1B); // 0x1B = kUndefinedMode
	eqflag = (R1==0x1B);
// /* 0x003AD864-0x003AD868 */
	R1 = R0 + 24;
// /* 0x003AD868-0x003AD86C */
	PUSH(SP, R0);
// /* 0x003AD86C-0x003AD870 */
	R0 = CPU->GetCPSR();
// /* 0x003AD870-0x003AD874 */
	CPU->SetCPSR(  (R0&0xFFFFFF00) | 0x000000D3); // 0x13 = kSupervisorMode and disabled interrupts
// /* 0x003AD874-0x003AD880 */
	if (!eqflag) {
		oldTask->SetR2(R2);
		oldTask->SetR3(R3);
		oldTask->SetR4(R4);
		oldTask->SetR5(R5);
		oldTask->SetR6(R6);
		oldTask->SetR7(R7);
// /* 0x003AD880-0x003AD884 */
		oldTask->SetR8(R8);
		oldTask->SetR9(R9);
		oldTask->SetR10(R10);
		oldTask->SetR11(R11);
		oldTask->SetR12(R12);
		oldTask->SetSP(CPU->mR13_Bkup);
		oldTask->SetLR(CPU->mR14_Bkup);
	}
// /* 0x003AD884-0x003AD88C */
	CPU->SetCPSR( R0 );
// /* 0x003AD88C-0x003AD898 */
	POP(SP, R0);
// /* 0x003AD898-0x003AD89C */
	if (!eqflag)
		goto L003AD8CC;
// /* 0x003AD89C-0x003AD8A0 */
	oldTask->SetR2(R2);
	oldTask->SetR3(R3);
	oldTask->SetR4(R4);
	oldTask->SetR5(R5);
	oldTask->SetR6(R6);
	oldTask->SetR7(R7);
	oldTask->SetR8(R8);
	oldTask->SetR9(R9);
	oldTask->SetR10(R10);
	oldTask->SetR11(R11);
	oldTask->SetR12(R12);
// /* 0x003AD8A0-0x003AD8A8 */
	R4 = CPU->GetCPSR();
// /* 0x003AD8A8-0x003AD8AC */
	R5 = CPU->GetSPSR();
// /* 0x003AD8AC-0x003AD8B0 */
	CPU->SetCPSR(R5);
// /* 0x003AD8B0-0x003AD8BC */
	oldTask->SetSP(SP);
	oldTask->SetLR(LR);
// /* 0x003AD8BC-0x003AD8C4 */
	CPU->SetCPSR(R4);
// /* 0x003AD8C4-0x003AD8D0 */
L003AD8CC:
// /* 0x003AD8D0-0x003AD8D4 */
	oldTask->SetPC(LR);
// /* 0x003AD8D4-0x003AD8D8 */
	POP(SP, R2);
	POP(SP, R3);
// /* 0x003AD8D8-0x003AD8DC */
	oldTask->SetR0(R2);
	oldTask->SetR1(R3);
// /* 0x003AD8DC-0x003AD8E0 */
	goto L003AD990;
// /* 0x003AD8E0-0x003AD8E4 */
L003AD8E0:
	R1 = R0;  // oldTask
// /* 0x003AD8E4-0x003AD8EC */
	LR = oldTask->GetPC();
// /* 0x003AD8EC-0x003AD8F4 */
	R2 = oldTask->GetR0();
	R3 = oldTask->GetR1();
// /* 0x003AD8F4-0x003AD8F8 */
	PUSH(SP, R3);
	PUSH(SP, R2);
// /* 0x003AD8F8-0x003AD8FC */
	R0 = R1;
// /* 0x003AD8FC-0x003AD900 */
	R1 = CPU->GetSPSR();
// /* 0x003AD900-0x003AD904 */
	oldTask->SetPSR( R1 );
// /* 0x003AD904-0x003AD908 */
	R1 = R1 & 0x1F;
// /* 0x003AD908-0x003AD90C */
	eqflag = (R1==0x1B);
// /* 0x003AD90C-0x003AD914 */
	PUSH(SP, R0);
// /* 0x003AD914-0x003AD918 */
	R0 = CPU->GetCPSR();
// /* 0x003AD918-0x003AD91C */
	CPU->SetCPSR( (CPU->GetCPSR()&0xFFFFFF00) | 0xD3 );
// /* 0x003AD91C-0x003AD928 */
	if (!eqflag) {
		oldTask->SetR2(R2);
		oldTask->SetR3(R3);
		oldTask->SetR4(R4);
		oldTask->SetR5(R5);
		oldTask->SetR6(R6);
		oldTask->SetR7(R7);
// /* 0x003AD928-0x003AD92C */
		oldTask->SetR8(R8);
		oldTask->SetR9(R9);
		oldTask->SetR10(R10);
		oldTask->SetR11(R11);
		oldTask->SetR12(R12);
		oldTask->SetSP(CPU->mR13_Bkup);
		oldTask->SetLR(CPU->mR14_Bkup);
	}
// /* 0x003AD92C-0x003AD934 */
	CPU->SetCPSR( R0 );
// /* 0x003AD934-0x003AD940 */
	POP(SP, R0);
// /* 0x003AD940-0x003AD944 */
	if (!eqflag)
		goto L003AD974;
// /* 0x003AD944-0x003AD948 */
	oldTask->SetR2(R2);
	oldTask->SetR3(R3);
	oldTask->SetR4(R4);
	oldTask->SetR5(R5);
	oldTask->SetR6(R6);
	oldTask->SetR7(R7);
	oldTask->SetR8(R8);
	oldTask->SetR9(R9);
	oldTask->SetR10(R10);
	oldTask->SetR11(R11);
	oldTask->SetR12(R12);
// /* 0x003AD948-0x003AD950 */
	R4 = CPU->GetCPSR();
// /* 0x003AD950-0x003AD954 */
	R5 = CPU->GetSPSR();
// /* 0x003AD954-0x003AD958 */
	CPU->SetCPSR( R5 );
// /* 0x003AD958-0x003AD964 */
	oldTask->SetSP(SP);
	oldTask->SetLR(LR);
// /* 0x003AD964-0x003AD96C */
	CPU->SetCPSR(R4);
// /* 0x003AD96C-0x003AD978 */
L003AD974:
// /* 0x003AD978-0x003AD97C */
	oldTask->SetPC(LR);
// /* 0x003AD97C-0x003AD980 */
	POP(SP, R2);
	POP(SP, R3);
// /* 0x003AD980-0x003AD984 */
	oldTask->SetR0(R2);
	oldTask->SetR1(R3);
// /* 0x003AD984-0x003AD990 */
	SetGCopyDone( 0 );
// /* 0x003AD990-0x003AD994 */
L003AD990:
// /* 0x003AD994-0x003AD998 */
	newTask = GetGCurrentTask();
	R0 = (KUInt32)(uintptr_t)newTask;
// /* 0x003AD998-0x003AD99C */
	R4 = R0;
// /* 0x003AD99C-0x003AD9A0 */
	SwapInGlobals( newTask );
// /* 0x003AD9A0-0x003AD9A4 */
	R0 = R4;
// /* 0x003AD9A4-0x003AD9AC */
	LR = newTask->GetPC();
// /* 0x003AD9AC-0x003AD9B0 */
	R1 = newTask->GetPSR();
// /* 0x003AD9B0-0x003AD9B4 */
	CPU->SetSPSR(R1);
// /* 0x003AD9B4-0x003AD9C0 */
	R1 = R1 & 0x1F;
// /* 0x003AD9C0-0x003AD9C4 */
	if ( R1!=0x1B ) {
// /* 0x003AD9C4-0x003AD9C8 */
		R0 = newTask->GetR0();
		R1 = newTask->GetR1();
		R2 = newTask->GetR2();
		R3 = newTask->GetR3();
		R4 = newTask->GetR4();
		R5 = newTask->GetR5();
		R6 = newTask->GetR6();
		R7 = newTask->GetR7();
		R8 = newTask->GetR8();
		R9 = newTask->GetR9();
		R10 = newTask->GetR10();
		R11 = newTask->GetR11();
		R12 = newTask->GetR12();
		CPU->mR13_Bkup = newTask->GetSP();
		CPU->mR14_Bkup = newTask->GetLR();
// /* 0x003AD9C8-0x003AD9D0 */
	} else {
// /* 0x003AD9D0-0x003AD9D4 */
		R2 = newTask->GetPSR();
// /* 0x003AD9D4-0x003AD9D8 */
		R1 = CPU->GetCPSR();
// /* 0x003AD9D8-0x003AD9DC */
		CPU->SetCPSR(R2);
// /* 0x003AD9DC-0x003AD9E8 */
		SP = newTask->GetSP();
// /* 0x003AD9E8-0x003AD9EC */
		LR = newTask->GetLR();
// /* 0x003AD9EC-0x003AD9F0 */
		CPU->SetCPSR(R1);
// /* 0x003AD9F0-0x003AD9FC */
		R0 = newTask->GetR0();
		R1 = newTask->GetR1();
		R2 = newTask->GetR2();
		R3 = newTask->GetR3();
		R4 = newTask->GetR4();
		R5 = newTask->GetR5();
		R6 = newTask->GetR6();
		R7 = newTask->GetR7();
		R8 = newTask->GetR8();
		R9 = newTask->GetR9();
		R10 = newTask->GetR10();
		R11 = newTask->GetR11();
		R12 = newTask->GetR12();
	}
// /* 0x003AD9FC-0x003ADA00 */
	PUSH(SP, R2);
	PUSH(SP, R1);
	PUSH(SP, R0);
// /* 0x003ADA00-0x003ADA08 */
	newTask = GetGCurrentTask();
	R1 = (KUInt32)(uintptr_t)newTask;
// /* 0x003ADA08-0x003ADA0C */
	R0 = 0;
// /* 0x003ADA0C-0x003ADA10 */
	newEnvironment = newTask->GetEnvironment();
	R2 = (KUInt32)(uintptr_t)newEnvironment;
// /* 0x003ADA10-0x003ADA14 */
	if (newEnvironment) {
// /* 0x003ADA14-0x003ADA18 */
		R2 = newEnvironment->GetDomainAccess();
// /* 0x003ADA18-0x003ADA1C */
		R0 = R0 | R2;
	}
// /* 0x003ADA1C-0x003ADA20 */
	newSharedMemeoryEnvironment = newTask->GetSMemEnvironment();
	R2 = (KUInt32)(uintptr_t)newSharedMemeoryEnvironment;
// /* 0x003ADA20-0x003ADA24 */
	if (newSharedMemeoryEnvironment) {
// /* 0x003ADA24-0x003ADA28 */
		R2 = newSharedMemeoryEnvironment->GetDomainAccess();
// /* 0x003ADA28-0x003ADA2C */
		R0 = R0 | R2;
	}
// /* 0x003ADA2C-0x003ADA30 */
	mTask = newTask;
// /* 0x003ADA30-0x003ADA34 */
	while ( (mTask = mTask->GetMonitorTask()) ) {
// /* 0x003ADA34-0x003ADA3C */
		TEnvironment *env = mTask->GetEnvironment();
// /* 0x003ADA3C-0x003ADA44 */
		if (!env) break;
// /* 0x003ADA44-0x003ADA48 */
		R2 = env->GetDomainAccess();
// /* 0x003ADA48-0x003ADA4C */
		R0 = R0 | R2;
// /* 0x003ADA4C-0x003ADA50 */
	}
// /* 0x003ADA50-0x003ADA54 */
	R1 = (KUInt32)(uintptr_t)mTask;
	PUSH(SP, R1);
	PUSH(SP, R0);
// /* 0x003ADA54-0x003ADA60 */
	gParamBlockFromImage->Set176xB0(R0);
// /* 0x003ADA60-0x003ADA64 */
	POP(SP, R0);
	POP(SP, R1);
// /* 0x003ADA64-0x003ADA68 */
	MEM->SetDomainAccessControl(R0);
// /* 0x003ADA68-0x003ADA6C */
	POP(SP, R0);
	POP(SP, R1);
	POP(SP, R2);
// /* 0x003ADA6C-0x003ADA70 */
	PC = LR+4;
	CPU->SetCPSR( CPU->GetSPSR() );
	return;


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


} // namespace


