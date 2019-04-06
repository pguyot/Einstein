// ==============================
// File:			SWI.cpp
// Project:			Einstein
//
// Copyright 2019 by Matthais Melcher (proteus@matthiasm.com).
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================


#include "SWI.h"

#include "Globals.h"
#include "KernelTask.h"
#include "KernelObject.h"

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



//DisableInterrupt:
//	@ label = 'DisableInterrupt'
//	mov     r12, sp                         @ 0x000E5890 0xE1A0C00D - ....


//QuickEnableInterrupt:
//	@ label = 'QuickEnableInterrupt'
//	mov     r12, sp                         @ 0x000E57BC 0xE1A0C00D - ....

//	InterruptObject *	RegisterInterrupt(ULong inArg1, void * inArg2, NewtonInterruptHandler inHandler, ULong inArg4);
//	void					QuickEnableInterrupt(InterruptObject * interrupt);
//	NewtonErr			EnableInterrupt(InterruptObject * interrupt, ULong inBits);
//	NewtonErr			DisableInterrupt(InterruptObject * interrupt);
//	NewtonErr			ClearInterrupt(InterruptObject * interrupt);

//	struct InterruptObject
//	{
//		ULong					x00;
//		int32_t				x04;
//		ULong					x08;			// +08	0x80 -> enabled
//		InterruptObject *	next;			// +0C	next interrupt in queue
//		NewtonInterruptHandler	handler;		// +10
//		InterruptObject *	queue;		// +14	queue this interrupt belongs to
//		int32_t				x18;
//		short					x1C;			// priority?
//		short					x1E;			// |
//	};

	void JIT_StartScheduler() // TODO: implemet natively, so we don;t have to save registers
	{


//		if (!gSchedulerRunning)
//		{
//			DisableInterrupt(gSchedulerIntObj);
//			g0F182C00 = g0F181800 + 20 * kMilliseconds;
//			gSchedulerRunning = YES;
//			QuickEnableInterrupt(gSchedulerIntObj);
//		}
//		gWantSchedulerToRun = NO;

		FindFiber()->SwitchToJIT(0x001CC4A8);
		return;
// /* 0x001CC4A8-0x001CC4AC */
		R12 = SP;
// /* 0x001CC4AC-0x001CC4B0 */
		PUSH(SP, PC);
		PUSH(SP, LR);
		PUSH(SP, R12);
		PUSH(SP, R11);
		PUSH(SP, R5);
		PUSH(SP, R4);
// /* 0x001CC4B0-0x001CC4B4 */
		R11 = R12 - 4;
		R0 = GetGSchedule
		if (!GetGSchedulerRunning()) {
// /* 0x001CC4C4-0x001CC4C8 */
			InterrupObject*

// /* 0x001CC4C8-0x001CC4F8 */
		}
		SetGWantSchedulerToRun( 0 );
// /* 0x001CC500-0x001CC504 */
		R11 += 4*4;
		//RPOP(R11, PC);
		RPOP(R11, SP);
		RPOP(R11, R11);
		RPOP(R11, R5);
		RPOP(R11, R4);
}


// /* 0x001CC4A8-0x001CC518 */



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
	T_JIT_TO_NATIVE(0x0025215C, "SwapInGlobals") {
		TTask *inTask = (TTask*)(uintptr_t)R0;
		SwapInGlobals(inTask);
	}


	/**
	 * This is the part that switches tasks at the end of every SWI.
	 *
	 * This function handles deferred calls and invokes the task scheduler.
	 * If the scheduler retrurns a new task, this function will save all
	 * registers and task states, restore the state of the new task, and
	 * continue execution in the new task when returning from the SWI.
	 *
	 * \note Thia must be called from the main SWI dispatch.
	 * \note fully native, depends on JIT calls
	 * \todo romove goto's
	 * \todo what is the idea behing gCopyDone?
	 */
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
	T_JIT_TO_NATIVE(0x003AD750, "_SWI_Scheduler") {
		_SWI_Scheduler();
	}


} // namespace


// ======================================================================= //
//                       - insert insane quote here -                      //
// ======================================================================= //
