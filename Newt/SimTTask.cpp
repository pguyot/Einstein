//
//  SimTTask.cpp
//  Einstein
//
//  Created by Matthias Melcher on 7/12/14.
//
//

#include "SimTTask.h"

/*

 Sorry, this is no implementation of TTask yet.

 Our current goal is to understand, when and how the stack for a task is
 allocated.
 
 We know already that no memory is actually allocated, but instead the stack 
 area is marked. Writing to the marked area will trigger a memory access 
 violation, which in turn will map physical RAM here.
 
 There is also a "band" below the lowest stack position that will trigger
 an overflow when the stack grows larger than some maximum.
 
 
TTask::FreeStack(...)
   TTask + 140, 08c, fStackBase: regaular memory, freed with 'free()'
   TTask + 108: 06c, fState: if kMemIsVirtual call FreePagedMem
   TTask + 136: 088, fStackTop: PagedMem

TTask::Init(TaskProcPtr inProc, size_t inStackSize, ObjectId inTaskId, ObjectId inDataId, ULong inPriority, ULong inName, CEnvironment * inEnvironment);
	Calls NewStack()
 
 -> TStackManger::FMNewStack
 
 
 When a memory fault is triggered, we fiddle around a lot with the CPU states and 
 domain, but essentially we call FaultMonitorEntry(). If FaultMonitorEntry() return 1,
 we end up in a exception (or reboot). If it returns 0, we simply schedule the next task.
 
 FaultMonitorEntry(): 
 
*/

