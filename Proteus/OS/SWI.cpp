//
//  SWI.cpp
//  Einstein
//
//  Created by Matthias Melcher on 4/4/19.
//

#include "SWI.h"

#include "Globals.h"
#include "KernelTask.h"
#include "KernelObject.h"
#include "Interrupt.h"

/*
 SWI5_68_PowerOffSystem:
 @; switch at 0x000D8AAC: case 68 (0x44)
 @ label = 'SWI5_68_PowerOffSystem'
 bl      VEC_PowerOffSystemKernelGlue__Fv  @ 0x000D919C 0xEB6CD571 - .l.q
 b       L000D91E4                   @ 0x000D91A0 0xEA00000F - ....

 SaveCPUStateAndStopSystem:
 @ label = 'SaveCPUStateAndStopSystem'
 msr     cpsr_ctl, #211              @ [ 0x000000D3 ] 0x00018F48 0xE321F0D3 - .!..

 SWI5_69_PauseSystem:
 @; switch at 0x000D8AAC: case 69 (0x45)
 @ label = 'SWI5_69_PauseSystem'
 bl      VEC_PauseSystemKernelGlue__Fv   @ 0x000D91A4 0xEB6CD571 - .l.q
 b       L000D91E4                   @ 0x000D91A8 0xEA00000D - ....
*/


namespace NewtOS {


	void JIT_DoDeferrals()
	{
		FindFiber()->SwitchToJIT(0x00148298);
	}


	TTask *JIT_Scheduler()
	{
		FindFiber()->SwitchToJIT(0x001CC1EC);
		return (TTask*)(uintptr_t)R0;
	}


	void JIT_DisableInterrupt(InterruptObject *intr)
	{
		R0 = (KUInt32)(uintptr_t)intr;
		FindFiber()->SwitchToJIT(0x000E5890);
		return;
	}

	// 0x00392B90
	void _EnterFIQAtomicFast()
	{
		INT->SetIntCtrlReg(0x0C400000);
		SetGAtomicFIQNestCountFast( GetGAtomicFIQNestCountFast()+1 );
	}

// /* 0x003AD658-0x003AD660 */

	void JIT_ExitFIQAtomicFast()
	{
		FindFiber()->SwitchToJIT(0x00392BB0);
		return;
// /* 0x00392BB0-0x00392C30 */
// ExitAtomic underflow
// /* 0x00392C48-0x00392C4C */
	}


	void JIT_SetAndClearBitsAtomic(KUInt32 addr, KUInt32 inSetMask, KUInt32 inClearMask)
	{
		int anyInterruptsEnabled = CPU->IsIRQEnabled() || CPU->IsFIQEnabled();
		if (anyInterruptsEnabled)
			_EnterFIQAtomicFast();
		KUInt32 w = PEEK_W(addr);
		w |= inSetMask;
		w &= ~inClearMask;
		POKE_W(addr, w);
		if (anyInterruptsEnabled)
			JIT_ExitFIQAtomicFast();
	}

	int SetAndClearBitsAtomicBegin()
	{
		int anyInterruptsEnabled = CPU->IsIRQEnabled() || CPU->IsFIQEnabled();
		if (anyInterruptsEnabled)
			_EnterFIQAtomicFast();
		return anyInterruptsEnabled;
	}

	void SetAndClearBitsAtomicEnd(int anyInterruptsEnabled)
	{
		if (anyInterruptsEnabled)
			JIT_ExitFIQAtomicFast();
	}

	// 0x000E57BC
	void JIT_QuickEnableInterrupt(InterruptObject *intr)
	{
		_EnterFIQAtomicFast();
		KUInt32 mask = intr->Get08() | 0x00000080;
		intr->Set08( mask );
		KUInt32 bits = intr->Get00();
		if (bits==0x00400000 || bits==0x08000000 || bits==0x04000000) {
			if (mask&1) {
				INT->SetIntEDReg1( INT->GetIntEDReg1() | bits );
			}
			if (mask&2) {
				INT->SetIntEDReg2( INT->GetIntEDReg2() | bits );
			}
			if (mask&0x0400) {
				INT->SetIntEDReg3( INT->GetIntEDReg3() | bits );
			}
			SetGIntMaskShadowReg( GetGIntMaskShadowReg() | bits );
		}
		JIT_ExitFIQAtomicFast();
	}


	// 0x001CC4A8
	void JIT_StartScheduler() // TODO: implemet natively, so we don't have to save registers
	{
		PUSH(SP, R5);
		PUSH(SP, R4);
		if ( GetGSchedulerRunning() ) {
			InterruptObject *intr = GetGSchedulerIntObj();
			JIT_DisableInterrupt( intr );
			KUInt32 nextIntr = INT->GetTimer() + 73720; // add 20ms
			INT->SetTimerMatchRegister( 3, nextIntr );
			SetGSchedulerRunning( 1 );
			JIT_QuickEnableInterrupt( intr );
		}
		SetGWantSchedulerToRun( 0 );
		POP(SP, R4);
		POP(SP, R5);
	}
	

	// 0x0025215C
	/**
	 * Replace some global variablea with values from the current task.
	 *
	 * \note fully native
	 */
	void SwapInGlobals(TTask *inTask)
	{
		ObjectId taskID = inTask->Get00_Id();
		SetGCurrentTaskId( taskID );
		void *globals = inTask->GetA0_Globals();
		SetGCurrentGlobals( globals );
		ObjectId monitor = inTask->GetD8_MonitorId();
		SetGgCurrentMonitorId( monitor );
	}
//	T_JIT_TO_NATIVE(0x0025215C, "SwapInGlobals") {
//		TTask *inTask = (TTask*)(uintptr_t)R0;
//		SwapInGlobals(inTask);
//	}



	T_JIT_TO_NATIVE(0x003AD750, "_SWI_Scheduler") {
		_SWI_Scheduler();
	}

	// _SWI_Scheduler starts here:
	void _SWI_Scheduler()
	{
		TTask *oldTask, *newTask, *mTask;
		TEnvironment *newEnvironment, *newSharedMemeoryEnvironment, *env;
		KUInt32 domainAccessControl;

		PUSH(SP, R1);
		PUSH(SP, R0);
		CPU->DisableFIQ();
		CPU->DisableIRQ();
		if (   GetGAtomicFIQNestCountFast()!=0
			|| GetGAtomicIRQNestCountFast()!=0
			|| GetGAtomicNestCount()!=0
			|| GetGAtomicFIQNestCount()!=0)
			goto L003ADA74;
		CPU->EnableFIQ();
		CPU->DisableIRQ();
		if (GetGWantDeferred()!=0)
			goto L003AD7C8;
		if (GetGSchedule()==0)
			goto L003ADA74;
		PUSH(SP, LR);
		PUSH(SP, R12);
		PUSH(SP, R11);
		PUSH(SP, R10);
		PUSH(SP, R3);
		PUSH(SP, R2);
		goto L003AD7F8;

	L003AD7C8:
		PUSH(SP, LR);
		PUSH(SP, R12);
		PUSH(SP, R11);
		PUSH(SP, R10);
		PUSH(SP, R3);
		PUSH(SP, R2);
		CPU->EnableFIQ();
		CPU->EnableIRQ();
		JIT_DoDeferrals();
		CPU->EnableFIQ();
		CPU->DisableIRQ();
		if (GetGSchedule()==0)
			goto L003ADA70;
	L003AD7F8:
		newTask = JIT_Scheduler();
		if (GetGWantSchedulerToRun()) {
			JIT_StartScheduler();
		}
		oldTask = GetGCurrentTask();
		if (oldTask!=newTask) {
			SetGCurrentTask( newTask );
			if (oldTask) {
				POP(SP, R2);
				POP(SP, R3);
				POP(SP, R10);
				POP(SP, R11);
				POP(SP, R12);
				POP(SP, LR);
				if (GetGCopyDone()==0) {
					R1 = CPU->GetSPSR();
					oldTask->Set50_PSR( R1 );
					R1 = R1 & 0x1F;
					if (R1!=0x1B) {
						KUInt32 saveCPSR = CPU->GetCPSR();
						CPU->SetCPSR(  (R0&0xFFFFFF00) | 0x000000D3); // 0x13 = kSupervisorMode and disabled interrupts
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
						oldTask->Set44_SP(CPU->mR13_Bkup);
						oldTask->Set48_LR(CPU->mR14_Bkup);
						CPU->SetCPSR( saveCPSR );
					} else {
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
						R4 = CPU->GetCPSR();
						R5 = CPU->GetSPSR();
						CPU->SetCPSR(R5);
						oldTask->Set44_SP(SP);
						oldTask->Set48_LR(LR);
						CPU->SetCPSR(R4);
					}
					oldTask->Set4C_PC(LR);
					POP(SP, R2);
					POP(SP, R3);
					oldTask->Set10_R0(R2);
					oldTask->Set14_R1(R3);
				} else {
					LR = oldTask->Get4C_PC();
					R2 = oldTask->Get10_R0();
					R3 = oldTask->Get14_R1();
					PUSH(SP, R3);
					PUSH(SP, R2);
					R1 = CPU->GetSPSR();
					oldTask->Set50_PSR( R1 );
					R1 = R1 & 0x1F;
					if (R1!=0x1B) {
						KUInt32 saveCPSR = CPU->GetCPSR();
						CPU->SetCPSR( (CPU->GetCPSR()&0xFFFFFF00) | 0xD3 );
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
						oldTask->Set44_SP(CPU->mR13_Bkup);
						oldTask->Set48_LR(CPU->mR14_Bkup);
						CPU->SetCPSR( saveCPSR );
					} else {
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
						R4 = CPU->GetCPSR();
						R5 = CPU->GetSPSR();
						CPU->SetCPSR( R5 );
						oldTask->Set44_SP(SP);
						oldTask->Set48_LR(LR);
						CPU->SetCPSR(R4);
					}
					oldTask->Set4C_PC(LR);
					POP(SP, R2);
					POP(SP, R3);
					oldTask->Set10_R0(R2);
					oldTask->Set14_R1(R3);
					SetGCopyDone( 0 );
				}
			} else {
				SP += 32; // pop 8 values form the stack
			}

			newTask = GetGCurrentTask();
			SwapInGlobals( newTask );
			LR = newTask->Get4C_PC();
			R1 = newTask->Get50_PSR();
			CPU->SetSPSR(R1);
			R1 = R1 & 0x1F;
			if ( R1!=0x1B ) {
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
			} else {
				R2 = newTask->Get50_PSR();
				R1 = CPU->GetCPSR();
				CPU->SetCPSR(R2);
				SP = newTask->Get44_SP();
				LR = newTask->Get48_LR();
				CPU->SetCPSR(R1);
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
			newTask = GetGCurrentTask();
			domainAccessControl = 0;
			newEnvironment = newTask->Get74_Environment();
			if (newEnvironment) {
				domainAccessControl |= newEnvironment->Get10_DomainAccess();
			}
			newSharedMemeoryEnvironment = newTask->Get78_SMemEnvironment();
			if (newSharedMemeoryEnvironment) {
				domainAccessControl |= newSharedMemeoryEnvironment->Get10_DomainAccess();
			}
			mTask = newTask;
			while ( (mTask = mTask->Get7C_MonitorTask()) ) {
				TEnvironment *env = mTask->Get74_Environment();
				if (!env) break;
				domainAccessControl |= env->Get10_DomainAccess();
			}
			gParamBlockFromImage->SetB0_DomainAccess( domainAccessControl );
			MEM->SetDomainAccessControl( domainAccessControl );
			PC = LR+4;
			CPU->SetCPSR( CPU->GetSPSR() );
			return;
		}

	L003ADA70:
		POP(SP, R2);
		POP(SP, R3);
		POP(SP, R10);
		POP(SP, R11);
		POP(SP, R12);
		POP(SP, LR);

	L003ADA74:
		PUSH(SP, LR);
		PUSH(SP, R12);
		PUSH(SP, R3);
		PUSH(SP, R2);
		if (GetGWantSchedulerToRun())
			JIT_StartScheduler();
		POP(SP, R2);
		POP(SP, R3);
		POP(SP, R12);
		POP(SP, LR);
		if (!GetGCopyDone()) {
			POP(SP, R0);
			POP(SP, R1);
			newTask = GetGCurrentTask();
			domainAccessControl = 0;
			env = newTask->Get74_Environment();
			if (env) {
				domainAccessControl |= env->Get10_DomainAccess();
			}
			env = newTask->Get78_SMemEnvironment();
			if (env) {
				domainAccessControl |= env->Get10_DomainAccess();
			}
			mTask = newTask;
			while ( (mTask = mTask->Get7C_MonitorTask()) ) {
				env = mTask->Get74_Environment();
				if (!env)
					break;
				domainAccessControl |= env->Get10_DomainAccess();
			}
			gParamBlockFromImage->SetB0_DomainAccess( domainAccessControl );
			MEM->SetDomainAccessControl( domainAccessControl );
		} else {
			SetGCopyDone(0);
			newTask = GetGCurrentTask();
			LR = newTask->Get4C_PC();
			R0 = newTask->Get10_R0();
			R1 = newTask->Get14_R1();
			SP += 8;
			newTask = GetGCurrentTask();
			domainAccessControl = 0;
			env = newTask->Get74_Environment();
			if (env) {
				domainAccessControl |= env->Get10_DomainAccess();
			}
			env = newTask->Get78_SMemEnvironment();
			if (env) {
				domainAccessControl |= env->Get10_DomainAccess();
			}
			mTask = newTask;
			while ( (mTask = mTask->Get7C_MonitorTask()) ) {
				env = mTask->Get74_Environment();
				if (!env)
					break;
				domainAccessControl |= env->Get10_DomainAccess();
			}
			gParamBlockFromImage->SetB0_DomainAccess( domainAccessControl );
			MEM->SetDomainAccessControl( domainAccessControl );
		}
		PC = LR+4;
		CPU->SetCPSR( CPU->GetSPSR() );
		return;
	}


} // namespace


