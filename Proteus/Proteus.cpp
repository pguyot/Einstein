// ==============================
// File:			Proteus.cpp
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


#include "Proteus.h"

// --- general includes

#include "TARMProcessor.h"
#include "TMemory.h"
#include "TInterruptManager.h"
#include "TJITGeneric_Macros.h"
#include "TJITGenericROMPatch.h"

// --- proteus variables

TARMProcessor *CPU = nullptr;
TMemory *MEM = nullptr;
TInterruptManager *INT = nullptr;

// --- macros

/** Accessing the Registers */
#define R0 CPU->mCurrentRegisters[0]
#define R1 CPU->mCurrentRegisters[1]
#define R2 CPU->mCurrentRegisters[2]
#define R3 CPU->mCurrentRegisters[3]
#define R4 CPU->mCurrentRegisters[4]
#define R5 CPU->mCurrentRegisters[5]
#define R6 CPU->mCurrentRegisters[6]
#define R7 CPU->mCurrentRegisters[7]
#define R8 CPU->mCurrentRegisters[8]
#define R9 CPU->mCurrentRegisters[9]
#define R10 CPU->mCurrentRegisters[10]
#define R11 CPU->mCurrentRegisters[11]
#define R12 CPU->mCurrentRegisters[12]
#define R13 CPU->mCurrentRegisters[13]
#define SP CPU->mCurrentRegisters[13]
#define R14 CPU->mCurrentRegisters[14]
#define LR CPU->mCurrentRegisters[14]
#define R15 CPU->mCurrentRegisters[15]
#define PC CPU->mCurrentRegisters[15]

/** Exit the native code to this address in the interpreter */
#define EXIT(addr) { R15 = addr+4; return nullptr; }

/** Push a value onto the stack (stmdb) */
#define PUSH(sp, w) { sp-=4; MEM->Write(sp, w); } 

/** Pop a value from the stack (ldmia) */
#define POP(sp, w) { MEM->Read(sp, w); sp+=4; } 

/** Reverse pop, (ldmdb) */
#define RPOP(sp, w) { sp-=4; MEM->Read(sp, w); } 

/** Read a word from memory */
inline KUInt32 PEEK_W(KUInt32 addr) { KUInt32 w; MEM->Read(addr, w); return w; }

/** Write a word to memory */
#define POKE_W(addr, w) { MEM->Write(addr, w); }

/** Macro to define a getter for a global variable.
 \param addr address of globale variable in RAM
 \param type type of that variable
 \param name name of the variable without the leading 'g'
 */
#define GLOBAL_GETSET_W(addr, type, name) \
    const TMemory::VAddr g##name = addr; \
	type GetG##name() { KUInt32 w; MEM->Read(g##name, w); return (type)w; } \
	void SetG##name(type v) { KUInt32 w=(KUInt32)v; MEM->Write(g##name, w); }

/** Define a class memeber variable and its getter and setter */
#define	T_GETSET_MEMVAR_W(offset, type, name) \
	type Get##name() { KUInt32 w; MEM->Read(((KUInt32)(uintptr_t)this)+offset, w); return (type)w; } \
	void Set##name(type w) { KUInt32 v = (KUInt32)w; MEM->Write(((KUInt32)(uintptr_t)this)+offset, v); }
#define	T_GETSET_MEMVAR_P(offset, type, name) \
	type Get##name() { KUInt32 w; MEM->Read(((KUInt32)(uintptr_t)this)+offset, w); return (type)(uintptr_t)w; } \
	void Set##name(type w) { KUInt32 v = (KUInt32)(uintptr_t)w; MEM->Write(((KUInt32)(uintptr_t)this)+offset, v); }

// --- class predeclarations

class ImageParamBlock;
class TTaskQueue;
class TTask;
class TTaskContainer;

// --- typedefs

typedef KUInt32 KernelObjectState;

// --- function declaraitions

/** Call an SWI with a given ID */
JITUnit *SWI(KUInt32 id);

/** These are the innards of the SWI, don't call directly */
JITUnit *_SWI(KUInt32 id);

/** This is the Scheduler call within the SWI, don't call directly */
JITUnit *_SWIScheduler();

/** Make sure that the following commands are run uninterrupted. */
void _EnterAtomicFast();

/** End the atomic operation started by _EnterAtomicFast(). */
void _ExitAtomicFast();

/** Call the SWI that allows the OS to change to a different task. */
void DoSchedulerSWI();

/** TODO: write me */
void DoDeferrals();
// Dependency tree:
//	_EnterFIQAtomicFast (OK)
//	_ExitAtomicFast
//		DoSchedulerSWI (loops!)
//	PortDeferredSendNotify__Fv
//		Remove__17TDoubleQContainerFv (end)
//		QueueDelay__12TTimerEngineFP13TSharedMemMsg
//			CompCompare (end)
//			Queue__12TTimerEngineFP13TSharedMemMsg
//				Peek__17TDoubleQContainerFv (end)
//				SetAlarmAtomic__FR5TTime
//					SetAlarm
//						SetAlarm1 (end)
//						DisableAlarm1 (end)
//				Alarm__12TTimerEngineFv
//					Peek__17TDoubleQContainerFv (above)
//					ClearAlarmAtomic__Fv
//						_EnterFIQAtomicFast (end)
//						ClearAlarm
//							DisableAlarm1 (end)
//						_ExitFIQAtomicFast (DoSchedulerSWI)
//					GetClock (end)
//					CompCompare (end)
//					Remove__17TDoubleQContainerFv (end)
//					SetAlarmAtomic__FR5TTime (above)
//				AddToFront__17TDoubleQContainerFPv
//					CheckBeforeAdd__17TDoubleQContainerFPv (end)
//				AddBefore__17TDoubleQContainerFPvT1
//					CheckBeforeAdd__17TDoubleQContainerFPv (end)
//					AddToFront__17TDoubleQContainerFPv (above)
//				GetNext__17TDoubleQContainerFPv (end)
//				Add__17TDoubleQContainerFPv
//					CheckBeforeAdd__17TDoubleQContainerFPv (end)
//		NotifySend__FP13TSharedMemMsg
//			ConvertIdToObj__F11KernelTypesUlPv
//				LocalToGlobalId__FUl ?
//				Get__12TObjectTableFUl
//					Get__12TObjectTableFUl (end)
//			Send__5TPortFP13TSharedMemMsgUl
//				QueueTimeout__12TTimerEngineFP13TSharedMemMsg
//					CompAdd (end)
//					__nw__FUi
//						malloc
//							NewPtr 
//								ReportMemMgrTrap__Fl
//									sprintf
//										memset (end)
//										__vfprintf ?
//										putc
//											__flsbuf ?
//												...
//												_writebuf ?
//													_sys_flen ?
//													_sys_seek ?
//													_sys_write ?
//														Unnamed_00319030 ?
//														TapFileCntl (ARM: 0xE6000810)
//								HashCallChain (end)
//								GetHeap
//									GetCurrentHeap (end)
//								IsSafeHeap__FPv (end)
//								GetHeapSemaphore__FPv
//									GetSkiaHeapSemaphore
//										GetCurrentHeap (above)
//								Acquire__18TULockingSemaphoreF8SemFlags
//									SemOp__16TUSemaphoreGroupFP17TUSemaphoreOpList8SemFlags
//										swi(11) -> DoSemaphoreOp
//											SemOp__15TSemaphoreGroupFP16TSemaphoreOpList8SemFlagsP5TTask
//												... Add__10TTaskQueueFP5TTask17KernelObjectStateP14TTaskContainer (end)
//								CheckHeap
//									AddExceptionHandler ?
//									VetHeap ?
//									Subexception ?
//									NextHandler ?
//									ExitHandler ?
//								ReportSmashedHeap__FPclPv ?
//								NewDirectBlock
//									GetCurrentHeap (above)
//									NewBlockLow
//										NewBlock
//											SearchFreeList
//												FindSmallestSlide
//													LockedBlock (end)
//													SlideBlocksUp
//														MoveBytes -> BlockMove -> memmove (end)
//														SetFreeChain (end)
//													GetCurrentHeap <----------
//													SlideBlocksDown (above, SlideBlocksUp)
//											ExtendVMHeap
//												SetHeapLimits -> _MonitorDispatchSWI -> MonitorDispatchKernelGlue
//													-> Aquire__8TMonitorFv 
//												LockHeapRange -> _MonitorDispatchSWI
//											MoveFreeBlock -> SetFreeChain
//											RemoveFreeBlock (end)
//								IncrementBlockBusy (end)
//								Release__18TULockingSemaphoreFv
//									SemOp__16TUSemaphoreGroupFP17TUSemaphoreOpList8SemFlag (above)
//								SetPtrName (end)
//					_EnterAtomicFast (ok)
//					Queue__12TTimerEngineFP13TSharedMemMsg ?
//					_ExitAtomicFast (above)
//				Peek__17TDoubleQContainerFv (above)
//				GetNext__17TDoubleQContainerFPv (above)
//				UnScheduleTask__FP5TTask ?
//				WantSchedule__Fv ?
//				RemoveFromQueue__17TDoubleQContainerFPv ?
//				CompleteReceiver__13TSharedMemMsgFP13TSharedMemMsgl ?
//				AddToFront__17TDoubleQContainerFPv (above)
//				Add__17TDoubleQContainerFPv (above)
//			CompleteSender__13TSharedMemMsgFl ?
//	DeferredNotify__Fv
//		Remove__17TDoubleQContainerFv ?
//		NotifyTimeout__FP13TSharedMemMsg ?
//		NotifySend__FP13TSharedMemMsg (above)
//	DoDeferral__18TExtPageTrackerMgrFv
//		Peek__17TDoubleQContainerFv (above)
//		DoDeferral__15TExtPageTrackerFv ?
//		GetNext__17TDoubleQContainerFPv ?

// --- classes

class ImageParamBlock
{
public:
	T_GETSET_MEMVAR_W(176, KUInt32, CurrentDomainAccessControl);
};

class TTaskQueue
{
public:
	void CheckBeforeAdd(TTask*);
    void Add(TTask *aTask, KernelObjectState aState, TTaskContainer *aContainer);
public:
	T_GETSET_MEMVAR_P(0, TTask*, Task);
	T_GETSET_MEMVAR_P(4, KUInt32, Unknown4);
};

class TTask 
{
public:
	T_GETSET_MEMVAR_W(148, KUInt32, Unknown148);
};

class TTaskContainer
{
public:
};

// --- globals

//-/* 0x0C008400-0x0C107E18 */ Range of global variables in memory
// /* 0x0C008400-0x0C0084E4 */
ImageParamBlock *gParamBlockFromImage = (ImageParamBlock*)0x0C008400;
// /* 0x0C0084E4-0x0C100E54 */

/** TODO: write docs */
GLOBAL_GETSET_W(0x0C100E54, KUInt32, IntMaskShadowReg);

/** Return the number of nestings into the Fast Interrupt */
GLOBAL_GETSET_W(0x0C100E58, KSInt32, AtomicFIQNestCountFast);

/** Return the number of nestings into the Regular Interrupt */
GLOBAL_GETSET_W(0x0C100E5C, KSInt32, AtomicIRQNestCountFast);

// /* 0x0C100E60-0x0C100FE4 */

/** TODO: write docs (this is a booloean?!) */
GLOBAL_GETSET_W(0x0C100FE4, KUInt32, Schedule);

/** Return the number of nestings into the Atomic function */
GLOBAL_GETSET_W(0x0C100FE8, KSInt32, AtomicNestCount);

// /* 0x0C100FEC-0x0C100FF0 */

/** Return the number of nestings into the Fast Interrupt */
GLOBAL_GETSET_W(0x0C100FF0, KSInt32, AtomicFIQNestCount);

// /* 0x0C100FF4-0x0C101028 */

/** TODO: write docs (this is a booloean?!) */
GLOBAL_GETSET_W(0x0C101028, KUInt32, WantDeferred);

// /* 0x0C10102C-0x0C107E18 */

// --- ROM

//-/* 0x00000000-0x0071FC4C */ NewtonOS in ROM

/*
 * This injection initializes the Proteus system by setting up a few
 * variables for fast acces in later calls.
 */
T_ROM_INJECTION(0x00000000, "Initialize Proteus") {
	CPU = ioCPU;
	MEM = ioCPU->GetMemory();
	INT = MEM->GetInterruptManager();
	return ioUnit;
}

// /* 0x00000000-0x0014829C */
// TODO: implememt this to finish SWIs
// /* 0x00148298-0x00359AA8 */

void TTaskQueue::CheckBeforeAdd(TTask *)
{
	return;
}
T_ROM_INJECTION(0x00359AA8, "CheckBeforeAdd__10TTaskQueueFP5TTask")
{
	((TTaskQueue*)(uintptr_t)(R0))->CheckBeforeAdd(((TTask*)(uintptr_t)(R1)));
	EXIT(LR);
}


void TTaskQueue::Add(TTask *aTask, KernelObjectState aState, TTaskContainer *aContainer)
{
	R0 = (KUInt32)(uintptr_t)this;
	R1 = (KUInt32)(uintptr_t)aTask;
	R2 = (KUInt32)(uintptr_t)aState;
	R3 = (KUInt32)(uintptr_t)aContainer;
	R12 = SP;
	PUSH(SP, PC);
	PUSH(SP, LR);
	PUSH(SP, R12);
	PUSH(SP, R11);
	PUSH(SP, R7);
	PUSH(SP, R6);
	PUSH(SP, R5);
	PUSH(SP, R4);
	R11 = R12 - 4;
	R5 = R0;
	R4 = R1;
	R7 = R2;
	R6 = R3;
	aTask->SetUnknown148(0);
	CheckBeforeAdd(aTask);
	if (GetTask()==nullptr) {
		SetTask(aTask);
		POKE_W(R4+152, R5);
	} else {
		R0 = PEEK_W(R5+4);
		R0 += 148; POKE_W(R0, R4);
		R0 = PEEK_W(R5+4);
		POKE_W(R4+152, R0);
	}
	SetUnknown4(R4);
	R0 = PEEK_W(R4+108);
	R0 = R0 | R7;
	R4 += 108; POKE_W(R4, R0);
	R4 += 36; POKE_W(R4, R6);
	KUInt32 sp = R11;
	RPOP(sp, PC);
	RPOP(sp, SP);
	RPOP(sp, R11);
	RPOP(sp, R7);
	RPOP(sp, R6);
	RPOP(sp, R5);
	RPOP(sp, R4);
}
T_ROM_INJECTION(0x00359AAC, "Add__10TTaskQueueFP5TTask17KernelObjectStateP14TTaskContainer")
{
    ((TTaskQueue*)(uintptr_t)(R0))->Add(((TTask*)(uintptr_t)(R1)), R2, ((TTaskContainer*)(uintptr_t)(R3)));
    EXIT(PC);
}


// /* 0x00359B14-0x00392AC0 */


/**
 * Start an atomic operation, asequence of commands that will not be iterrupted.
 * End the atomic operation by calling _ExitAtomicFast().
 */
void _EnterAtomicFast() 
{
	if (GetGAtomicFIQNestCountFast()!=0) {
		SetGAtomicIRQNestCountFast( GetGAtomicIRQNestCountFast()+1 );
	} else {
		INT->SetIntCtrlReg( 0x0C400000 );
		KUInt32 interruptCtrl = GetGIntMaskShadowReg() & INT->GetFIQMask();
		SetGAtomicIRQNestCountFast( GetGAtomicIRQNestCountFast()+1 );
		INT->SetIntCtrlReg( interruptCtrl | 0x0C400000 );
	}
}

T_ROM_INJECTION(0x00392AC0, "_EnterAtomicFast") {
	_EnterAtomicFast();
	EXIT(LR);
}



void _ExitAtomicFast() 
{
// /* 0x00392B1C-0x00392B90 */
}


// /* 0x00392B90-0x003AD658 */


// as long as the SWI returns directly to the emulator, we can't activate this call
//T_ROM_INJECTION(0x003AD658, "DoSchedulerSWI") {
//	DoSchedulerSWI();
//	EXIT(LR);
//}

void DoSchedulerSWI()
{
	SWI(34);
}


// /* 0x003AD660-0x003AD698 */

/**
 * This is the native entry point for the swi call.
 *
 * At this point, the Emulator has set the CPU into svc mode and has
 * changed register banks to svc as well. The code below implements
 * some tests and the goes on to call the native implementation
 * of the actual swi.
 */
T_ROM_INJECTION(0x003AD698, "SWIBoot")
{
	// Get the binary version of the original swi command that called us.
	KUInt32 swi = PEEK_W(LR-4);

	// Make sure that this is really an swi call, and nut just aregular branch command.
    if ( (swi&0x0F000000) != 0x0F000000 ) {
		LR -= 4;
		PC = LR+4;
		CPU->SetCPSR( CPU->GetSPSR() );
		return nullptr;
	}

	// Get the condition code from the original swi command.
    KUInt32 cc = swi & 0xF0000000;
    if ( cc != 0xE0000000 ) {
		bool cond = false;
		KUInt32 flags = CPU->GetSPSR();
		switch ( cc>>24 ) {
			case 0x00: cond = (flags!=0x40000000); break; // EQ
			case 0x01: cond = (flags==0x40000000); break; // NE
			case 0x02: cond = (flags!=0x20000000); break; // CS
			case 0x03: cond = (flags==0x20000000); break; // CC
			case 0x04: cond = (flags!=0x80000000); break; // MI
			case 0x05: cond = (flags==0x80000000); break; // PL
			case 0x06: cond = (flags!=0x10000000); break; // VS
			case 0x07: cond = (flags==0x10000000); break; // VC
			case 0x08: cond = ((flags&~(flags>>1))!=0x20000000); break; // HI
			case 0x09: cond = ((flags&~(flags>>1))==0x20000000); break; // LS
			case 0x0A: cond = ((flags^(flags<<3))!=0x80000000); break; // GE
			case 0x0B: cond = ((flags^(flags<<3))==0x80000000); break; // LT
			case 0x0C: cond = (((flags^(flags<<3))|(flags<<1))!=0x80000000); break; // GT
			case 0x0D: cond = (((flags^(flags<<3))|(flags<<1))==0x80000000); break; // LE
			case 0x0E: cond = true; break; // AL
			case 0x0F: cond = false; break; // NV
		}
		if (cond==false) {
			LR -= 4;
			PC = LR+4;
			CPU->SetCPSR(CPU->GetSPSR());
			return nullptr;
		}
	} 
	return _SWI( swi & 0x00FFFFFF );
}


/**
 * Handle all operations that need to be executet in supervisor mode.
 *
 * This method is called from any code, priviledged or not, via the \c swi call
 * to run low level OS functions that need run in supervisor mode.
 *
 * After running the operation, every SWI call ends in the scheduler, allowing 
 * NewtonOS to switch to another task if needed. With Proteus running NewtonOS
 * in native mode inside the Einstein emulator, it is essential that task switching
 * is reimplemented so that we can switch between native and emulated tasks.
 *
 * \return FIXME: unitl fully implemented, we always return null for simplicity.
 */
JITUnit *SWI(KUInt32 cmd)
{
	CPU->DoSWI();	// Change the CPU into svc mode.
	return _SWI(cmd);;
}


/** 
 * SWI native implementation.
 *
 * This is only used when teh CPU is already in svc mode. Use SWI(KUInt32) instead.
 *
 * \return FIXME: unitl fully implemented, we always return null for simplicity.
 */
JITUnit *_SWI(KUInt32 cmd)
{
	// TODO: until final, the individual commands expect r0 and r1 on the stack
    PUSH(SP, R1);
    PUSH(SP, R0);

	KUInt32 prevCPUMode = CPU->GetSPSR() & 0x0000001F;
    if (prevCPUMode!=TARMProcessor::kUserMode && prevCPUMode!=0)
		throw "SWI from non-user mode (rebooting)";

    KUInt32 dac = 0x55555555;
	gParamBlockFromImage->SetCurrentDomainAccessControl( dac );
    MEM->SetDomainAccessControl( dac );

    if (   GetGAtomicFIQNestCountFast()==0
        && GetGAtomicIRQNestCountFast()==0
        && GetGAtomicNestCount()==0
        && GetGAtomicFIQNestCount()==0 )
	{
		CPU->SetCPSR( (CPU->GetCPSR()&~0xff) | TARMProcessor::kSupervisorMode );
	}

	R1 = cmd;
    if (R1>=35)
		throw "Undefined SWI";
    R0 = 0x003AD56C; // address of jump table

	switch (R1) {
		case 34: // L003AE14C
			POP(SP, R0);
			POP(SP, R1);
			return _SWIScheduler();
		default:
			EXIT(0x003AD74C);
    }
// /* 0x003AD74C-0x003AD750 */
}


T_ROM_INJECTION(0x003AD750, "_SWIScheduler") { return _SWIScheduler(); }
/**
 * This function is called at the end of SWIs.
 *
 * All functions called in the dispatch above will eventually end here, in the scheduler.
 * The scheduler will check if it is time to run another thread in the multithreaded 
 * system. Thread changes are usually requested by external events or by the timer.
 * This is called preemptive multitasking. If a task does not call any SWIs for a long 
 * time, all other tasks in teh system are blocked and won;t run.
 *
 * NewtonOS has one very special event that can (and must) trigger a task change: if a
 * user task reads or writes an address that was marked by the Memory Managemnet Unit (MMU),
 * an interrupt is called that may allocate more memory to the location that was read.
 * Since allocating memory may trigger major reorganizetions, Newt puts the current
 * task to sleep and switches to teh memory management task. Only if that task is succesful,
 * the original task will be rescheduled to try the same memory access again.
 *
 * Until memory management and task switching is fully implemented, there is no use in
 * implementing any user code. Otherwies, user code could trigger a memory exeception
 * at any time and crash teh system if not handled correctly.
 */
JITUnit *_SWIScheduler()
{
    PUSH(SP, R1);
    PUSH(SP, R0);
    // bit7=IRQ, bit6=FIQ, bit0-4=mode (10=usr, 11=FIQ, 12=IRQ, 13=abt, 17=svc, 1b=und, 1f=system)
    CPU->SetCPSR( (CPU->GetCPSR()&~0xff) | 0xD3 ); // Disable interrupts, abt mode
    if (   GetGAtomicFIQNestCountFast()!=0
        || GetGAtomicIRQNestCountFast()!=0
        || GetGAtomicNestCount()!=0
        || GetGAtomicFIQNestCount()!=0)
        goto L003ADA74;
    
    // this SWI is not a nested call
    // bit7=IRQ, bit6=FIQ, bit0-4=mode (10=usr, 11=FIQ, 12=IRQ, 13=abt, 17=svc, 1b=und, 1f=system)
    CPU->SetCPSR( (CPU->GetCPSR()&~0xff) | 0x93 ); // Enable FIQ, keep IRQ disabled
    if (GetGWantDeferred()!=0)
        goto L003AD7C8;

    if (GetGSchedule()==0)
        goto L003ADA74;

L003AD7C0:
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
    // bit7=IRQ, bit6=FIQ, bit0-4=mode (10=usr, 11=FIQ, 12=IRQ, 13=abt, 17=svc, 1b=und, 1f=system)
    CPU->SetCPSR( (CPU->GetCPSR()&~0xff) | 0x13 ); // Enable all interrupts

// /* 0x003AD7D8-0x003AD7DC */
    // HERE: DoDeferrals();

    // bail out at this point
    R15 = 0x003AD7D8+4;
    return nullptr;

L003AD7DC:
// /* 0x003AD7DC-0x003AD7F8 */

L003AD7F8:
// /* 0x003AD7F8-0x003AD7FC */
    // bail out at this point
    R15 = 0x003AD7F8+4;
    return nullptr;

// /* 0x003AD7FC-0x003ADA74 */

L003ADA74:
    // this SWI is somehow nested into an atomic call or an interrupt
    // bail out at this point
    R15 = 0x003ADA74+4;
    return nullptr;
}

// /* 0x003ADA74-0x003ADBB4 */
    // End of SWI scheduler??

// /* 0x003ADBB4-0x0071FC4C */

// --- RExBlock

//-/* 0x0071FC4C-0x007EE048 */ NewtonOS ROM Extensions
// /* 0x0071FC4C-0x007EE048 */

// --- Jump Table (not used here)

//-/* 0x01a00000-0x01c1085C */

// ============================================================================== //
// I've ended up calling my cat Brexit. It wakes me up meowing like crazy every
// morning because it wants to go out, but as soon as I open the door, it just
// sits there undecided and then looks angry when I put it outside.
//   -- French Europe Minister Nathalie Loiseau
// ============================================================================== //
