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
	TEnvironment *newEnvironment, *newSharedMemeoryEnvironment, *env;
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
	oldTask->Set50_PSR( R1 );
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
		oldTask->Set18_R2(R2);
		oldTask->Set1C_R3(R3);
		oldTask->Set20_R4(R4);
		oldTask->Set24_R5(R5);
		oldTask->Set28_R6(R6);
		oldTask->Set2C_R7(R7);
// /* 0x003AD880-0x003AD884 */
		oldTask->Set30_R8(R8);
		oldTask->Set34_R9(R9);
		oldTask->Set38_R10(R10);
		oldTask->Set3C_R11(R11);
		oldTask->Set40_R12(R12);
		oldTask->Set44_SP(CPU->mR13_Bkup);
		oldTask->Set48_LR(CPU->mR14_Bkup);
	}
// /* 0x003AD884-0x003AD88C */
	CPU->SetCPSR( R0 );
// /* 0x003AD88C-0x003AD898 */
	POP(SP, R0);
// /* 0x003AD898-0x003AD89C */
	if (!eqflag)
		goto L003AD8CC;
// /* 0x003AD89C-0x003AD8A0 */
	oldTask->Set18_R2(R2);
	oldTask->Set1C_R3(R3);
	oldTask->Set20_R4(R4);
	oldTask->Set24_R5(R5);
	oldTask->Set28_R6(R6);
	oldTask->Set2C_R7(R7);
	oldTask->Set30_R8(R8);
	oldTask->Set34_R9(R9);
	oldTask->Set38_R10(R10);
	oldTask->Set3C_R11(R11);
	oldTask->Set40_R12(R12);
// /* 0x003AD8A0-0x003AD8A8 */
	R4 = CPU->GetCPSR();
// /* 0x003AD8A8-0x003AD8AC */
	R5 = CPU->GetSPSR();
// /* 0x003AD8AC-0x003AD8B0 */
	CPU->SetCPSR(R5);
// /* 0x003AD8B0-0x003AD8BC */
	oldTask->Set44_SP(SP);
	oldTask->Set48_LR(LR);
// /* 0x003AD8BC-0x003AD8C4 */
	CPU->SetCPSR(R4);
// /* 0x003AD8C4-0x003AD8D0 */
L003AD8CC:
// /* 0x003AD8D0-0x003AD8D4 */
	oldTask->Set4C_PC(LR);
// /* 0x003AD8D4-0x003AD8D8 */
	POP(SP, R2);
	POP(SP, R3);
// /* 0x003AD8D8-0x003AD8DC */
	oldTask->Set10_R0(R2);
	oldTask->Set14_R1(R3);
// /* 0x003AD8DC-0x003AD8E0 */
	goto L003AD990;
// /* 0x003AD8E0-0x003AD8E4 */
L003AD8E0:
	R1 = R0;  // oldTask
// /* 0x003AD8E4-0x003AD8EC */
	LR = oldTask->Get4C_PC();
// /* 0x003AD8EC-0x003AD8F4 */
	R2 = oldTask->Get10_R0();
	R3 = oldTask->Get14_R1();
// /* 0x003AD8F4-0x003AD8F8 */
	PUSH(SP, R3);
	PUSH(SP, R2);
// /* 0x003AD8F8-0x003AD8FC */
	R0 = R1;
// /* 0x003AD8FC-0x003AD900 */
	R1 = CPU->GetSPSR();
// /* 0x003AD900-0x003AD904 */
	oldTask->Set50_PSR( R1 );
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
		oldTask->Set18_R2(R2);
		oldTask->Set1C_R3(R3);
		oldTask->Set20_R4(R4);
		oldTask->Set24_R5(R5);
		oldTask->Set28_R6(R6);
		oldTask->Set2C_R7(R7);
// /* 0x003AD928-0x003AD92C */
		oldTask->Set30_R8(R8);
		oldTask->Set34_R9(R9);
		oldTask->Set38_R10(R10);
		oldTask->Set3C_R11(R11);
		oldTask->Set40_R12(R12);
		oldTask->Set44_SP(CPU->mR13_Bkup);
		oldTask->Set48_LR(CPU->mR14_Bkup);
	}
// /* 0x003AD92C-0x003AD934 */
	CPU->SetCPSR( R0 );
// /* 0x003AD934-0x003AD940 */
	POP(SP, R0);
// /* 0x003AD940-0x003AD944 */
	if (!eqflag)
		goto L003AD974;
// /* 0x003AD944-0x003AD948 */
	oldTask->Set18_R2(R2);
	oldTask->Set1C_R3(R3);
	oldTask->Set20_R4(R4);
	oldTask->Set24_R5(R5);
	oldTask->Set28_R6(R6);
	oldTask->Set2C_R7(R7);
	oldTask->Set30_R8(R8);
	oldTask->Set34_R9(R9);
	oldTask->Set38_R10(R10);
	oldTask->Set3C_R11(R11);
	oldTask->Set40_R12(R12);
// /* 0x003AD948-0x003AD950 */
	R4 = CPU->GetCPSR();
// /* 0x003AD950-0x003AD954 */
	R5 = CPU->GetSPSR();
// /* 0x003AD954-0x003AD958 */
	CPU->SetCPSR( R5 );
// /* 0x003AD958-0x003AD964 */
	oldTask->Set44_SP(SP);
	oldTask->Set48_LR(LR);
// /* 0x003AD964-0x003AD96C */
	CPU->SetCPSR(R4);
// /* 0x003AD96C-0x003AD978 */
L003AD974:
// /* 0x003AD978-0x003AD97C */
	oldTask->Set4C_PC(LR);
// /* 0x003AD97C-0x003AD980 */
	POP(SP, R2);
	POP(SP, R3);
// /* 0x003AD980-0x003AD984 */
	oldTask->Set10_R0(R2);
	oldTask->Set14_R1(R3);
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
	LR = newTask->Get4C_PC();
// /* 0x003AD9AC-0x003AD9B0 */
	R1 = newTask->Get50_PSR();
// /* 0x003AD9B0-0x003AD9B4 */
	CPU->SetSPSR(R1);
// /* 0x003AD9B4-0x003AD9C0 */
	R1 = R1 & 0x1F;
// /* 0x003AD9C0-0x003AD9C4 */
	if ( R1!=0x1B ) {
// /* 0x003AD9C4-0x003AD9C8 */
		R0 = newTask->Get10_R0();
		R1 = newTask->Get14_R1();
		R2 = newTask->Get18_R2();
		R3 = newTask->Get1C_R3();
		R4 = newTask->Get20_R4();
		R5 = newTask->Get24_R5();
		R6 = newTask->Get28_R6();
		R7 = newTask->Get2C_R7();
		R8 = newTask->Get30_R8();
		R9 = newTask->Get34_R9();
		R10 = newTask->Get38_R10();
		R11 = newTask->Get3C_R11();
		R12 = newTask->Get40_R12();
		CPU->mR13_Bkup = newTask->Get44_SP();
		CPU->mR14_Bkup = newTask->Get48_LR();
// /* 0x003AD9C8-0x003AD9D0 */
	} else {
// /* 0x003AD9D0-0x003AD9D4 */
		R2 = newTask->Get50_PSR();
// /* 0x003AD9D4-0x003AD9D8 */
		R1 = CPU->GetCPSR();
// /* 0x003AD9D8-0x003AD9DC */
		CPU->SetCPSR(R2);
// /* 0x003AD9DC-0x003AD9E8 */
		SP = newTask->Get44_SP();
// /* 0x003AD9E8-0x003AD9EC */
		LR = newTask->Get48_LR();
// /* 0x003AD9EC-0x003AD9F0 */
		CPU->SetCPSR(R1);
// /* 0x003AD9F0-0x003AD9FC */
		R0 = newTask->Get10_R0();
		R1 = newTask->Get14_R1();
		R2 = newTask->Get18_R2();
		R3 = newTask->Get1C_R3();
		R4 = newTask->Get20_R4();
		R5 = newTask->Get24_R5();
		R6 = newTask->Get28_R6();
		R7 = newTask->Get2C_R7();
		R8 = newTask->Get30_R8();
		R9 = newTask->Get34_R9();
		R10 = newTask->Get38_R10();
		R11 = newTask->Get3C_R11();
		R12 = newTask->Get40_R12();
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
	newEnvironment = newTask->Get74_Environment();
	R2 = (KUInt32)(uintptr_t)newEnvironment;
// /* 0x003ADA10-0x003ADA14 */
	if (newEnvironment) {
// /* 0x003ADA14-0x003ADA18 */
		R2 = newEnvironment->Get10_DomainAccess();
// /* 0x003ADA18-0x003ADA1C */
		R0 = R0 | R2;
	}
// /* 0x003ADA1C-0x003ADA20 */
	newSharedMemeoryEnvironment = newTask->Get78_SMemEnvironment();
	R2 = (KUInt32)(uintptr_t)newSharedMemeoryEnvironment;
// /* 0x003ADA20-0x003ADA24 */
	if (newSharedMemeoryEnvironment) {
// /* 0x003ADA24-0x003ADA28 */
		R2 = newSharedMemeoryEnvironment->Get10_DomainAccess();
// /* 0x003ADA28-0x003ADA2C */
		R0 = R0 | R2;
	}
// /* 0x003ADA2C-0x003ADA30 */
	mTask = newTask;
// /* 0x003ADA30-0x003ADA34 */
	while ( (mTask = mTask->Get7C_MonitorTask()) ) {
// /* 0x003ADA34-0x003ADA3C */
		TEnvironment *env = mTask->Get74_Environment();
// /* 0x003ADA3C-0x003ADA44 */
		if (!env) break;
// /* 0x003ADA44-0x003ADA48 */
		R2 = env->Get10_DomainAccess();
// /* 0x003ADA48-0x003ADA4C */
		R0 = R0 | R2;
// /* 0x003ADA4C-0x003ADA50 */
	}
// /* 0x003ADA50-0x003ADA54 */
	R1 = (KUInt32)(uintptr_t)mTask;
	PUSH(SP, R1);
	PUSH(SP, R0);
// /* 0x003ADA54-0x003ADA60 */
	gParamBlockFromImage->SetB0_DomainAccess(R0);
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
	PUSH(SP, LR);
	PUSH(SP, R12);
	PUSH(SP, R3);
	PUSH(SP, R2);
// /* 0x003ADA78-0x003ADA84 */
	if (GetGWantSchedulerToRun())
// /* 0x003ADA84-0x003ADA88 */
		StartScheduler();
// /* 0x003ADA88-0x003ADA8C */
	POP(SP, R2);
	POP(SP, R3);
	POP(SP, R12);
	POP(SP, LR);
// /* 0x003ADA8C-0x003ADA9C */
	if (GetGCopyDone())
		goto L003ADB14;
// /* 0x003ADA9C-0x003ADAA0 */
	POP(SP, R0);
	POP(SP, R1);
// /* 0x003ADAA0-0x003ADAA4 */
	PUSH(SP, R2);
	PUSH(SP, R1);
	PUSH(SP, R0);
// /* 0x003ADAA4-0x003ADAAC */
	newTask = GetGCurrentTask();
	R1 = (KUInt32)(uintptr_t)newTask;
// /* 0x003ADAAC-0x003ADAB0 */
	R0 = 0;
// /* 0x003ADAB0-0x003ADAB4 */
	env = newTask->Get74_Environment();
	R2 = (KUInt32)(uintptr_t)env;
// /* 0x003ADAB4-0x003ADAB8 */
	if (env) {
// /* 0x003ADAB8-0x003ADABC */
		R2 = env->Get10_DomainAccess();
// /* 0x003ADABC-0x003ADAC0 */
		R0 |= R2;
	}
// /* 0x003ADAC0-0x003ADAC4 */
	env = newTask->Get78_SMemEnvironment();
	R2 = (KUInt32)(uintptr_t)env;
// /* 0x003ADAC4-0x003ADAC8 */
	if (env) {
// /* 0x003ADAC8-0x003ADACC */
		R2 = env->Get10_DomainAccess();
// /* 0x003ADACC-0x003ADAD0 */
		R0 |= R2;
	}
// /* 0x003ADAD0-0x003ADAD4 */
	mTask = newTask;
	for (;;) {
		mTask = mTask->Get7C_MonitorTask();
		R1 = (KUInt32)(uintptr_t)mTask;
// /* 0x003ADAD4-0x003ADADC */
		if (!mTask)
			break;
// /* 0x003ADADC-0x003ADAE0 */
		env = mTask->Get74_Environment();
		R2 = (KUInt32)(uintptr_t)env;
// /* 0x003ADAE0-0x003ADAE8 */
		if (!env)
			break;
// /* 0x003ADAE8-0x003ADAEC */
		R2 = env->Get10_DomainAccess();
// /* 0x003ADAEC-0x003ADAF0 */
		R0 |= R2;
// /* 0x003ADAF0-0x003ADAF4 */
	}
// /* 0x003ADAF4-0x003ADAF8 */
	PUSH(SP, R1);
	PUSH(SP, R0);
// /* 0x003ADAF8-0x003ADB04 */
	gParamBlockFromImage->SetB0_DomainAccess(R0);
// /* 0x003ADB04-0x003ADB08 */
	POP(SP, R0);
	POP(SP, R1);
// /* 0x003ADB08-0x003ADB0C */
	MEM->SetDomainAccessControl(R0);
// /* 0x003ADB0C-0x003ADB10 */
	POP(SP, R0);
	POP(SP, R1);
	POP(SP, R2);
// /* 0x003ADB10-0x003ADB14 */
	PC = LR+4;
	CPU->SetCPSR( CPU->GetSPSR() );
	return;
// return from SWI

// /* 0x003ADB14-0x003ADB18 */
L003ADB14:
// /* 0x003ADB18-0x003ADB20 */
	SetGCopyDone(0);
// /* 0x003ADB20-0x003ADB28 */
	newTask = GetGCurrentTask();
	R0 = (KUInt32)(uintptr_t)newTask;
// /* 0x003ADB28-0x003ADB2C */
	R1 = R0;
// /* 0x003ADB2C-0x003ADB34 */
	LR = newTask->Get4C_PC();
// /* 0x003ADB34-0x003ADB3C */
	R0 = newTask->Get10_R0();
	R1 = newTask->Get14_R1();
// /* 0x003ADB3C-0x003ADB40 */
	SP += 8;
// /* 0x003ADB40-0x003ADB44 */
	PUSH(SP, R2);
	PUSH(SP, R1);
	PUSH(SP, R0);
// /* 0x003ADB44-0x003ADB4C */
	newTask = GetGCurrentTask();
	R1 = (KUInt32)(uintptr_t)newTask;
// /* 0x003ADB4C-0x003ADB50 */
	R0 = 0;
// /* 0x003ADB50-0x003ADB54 */
	env = newTask->Get74_Environment();
	R2 = (KUInt32)(uintptr_t)env;
// /* 0x003ADB54-0x003ADB58 */
	if (env) {
// /* 0x003ADB58-0x003ADB5C */
		R2 = env->Get10_DomainAccess();
// /* 0x003ADB5C-0x003ADB60 */
		R0 |= R2;
	}
// /* 0x003ADB60-0x003ADB64 */
	env = newTask->Get78_SMemEnvironment();
// /* 0x003ADB64-0x003ADB68 */
	if (env) {
// /* 0x003ADB68-0x003ADB70 */
		R0 |= env->Get10_DomainAccess();
	}
// /* 0x003ADB70-0x003ADB74 */
	mTask = newTask;
	for (;;) {
		mTask = mTask->Get7C_MonitorTask();
// /* 0x003ADB74-0x003ADB7C */
		if (!mTask) break;
// /* 0x003ADB7C-0x003ADB80 */
		env = mTask->Get74_Environment();
// /* 0x003ADB80-0x003ADB88 */
		if (!env)
			break;
// /* 0x003ADB88-0x003ADB90 */
		R0 |= env->Get10_DomainAccess();
// /* 0x003ADB90-0x003ADB94 */
	}
// /* 0x003ADB94-0x003ADB98 */
	PUSH(SP, R1);
	PUSH(SP, R0);
// /* 0x003ADB98-0x003ADBA4 */
	gParamBlockFromImage->SetB0_DomainAccess(R0);
// /* 0x003ADBA4-0x003ADBA8 */
	POP(SP, R0);
	POP(SP, R1);
// /* 0x003ADBA8-0x003ADBAC */
	MEM->SetDomainAccessControl(R0);
// /* 0x003ADBAC-0x003ADBB0 */
	POP(SP, R0);
	POP(SP, R1);
	POP(SP, R2);
// /* 0x003ADBB0-0x003ADBB4 */
	PC = LR+4;
	CPU->SetCPSR( CPU->GetSPSR() );
	return;
// return from SWI
}


} // namespace


