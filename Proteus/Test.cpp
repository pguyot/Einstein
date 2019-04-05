


#include "Test.h"
#include "Proteus.h"

/*

 Back Story
 ----------

 On April 1st 2019 it hit me. I was watching Noah's "Love Notes to Newton" yet
 again to show my kids the only movie credit that I ever got, and that finally
 got me an entry into the IMDB despite working 10 years in Hollywood as a
 software engineer in the movies.

 I realized that I had been tinkering with the Newton for 14 years by now. I
 ported Einstein to MSWindows and Android with the help of other Newton fans.
 I wrote a disassembler for the ROM, and I wrote serial port drivers among other
 code that requiered disassembling and understanding the ROM. At least five
 developers come to mind who have a fantastic understanding on what is going on
 inside NewtonOS.

 So I said another thing in "Love Notes", along the lines of "Newton is samll
 enough for a single person to comprehend, it's my 2-million-fields Sudoku".
 Let's do the math: NewtonOS is smaller than 8MB. Half of that is ARM machine
 code. One machine code command is 4 bytes, leaving us with less than 1 million
 ARM instructions. If in the last 14 years, instead of wasting my time on real
 Sudoku, I would have needed to decode 1'000'000 / 14 / 365 = 200 instructions
 a day to completely translate the ROM. That is an absolutely realistic number.
 ARM assembly is very easy to understand and translate into C++.

 Just for fun. let's divide that by the number of people listed above, and
 assume we have 5 years to do it. Every person would have to deliver less than
 100 lines a day, which compresses down to about 25 lines of C++ code.

 Whoa! All of a sudden, the complete reimplementation of the Newton ROM in
 C++ is actually within somewhat reasonable reach! If we would have started
 this in 2005, we would have had the new Newton OS completed in 2010, just by
 writing a few lines of code instead of sitting in the bathroom, solving Sudoku.

 Now imagine just for giggles if any of the original developers would join in
 to this effort... .


 Main Goal
 ---------

  * full source code of ROM in C++ and NewtonScript
     * able to run code native inside Einstein
     * recompile parts and improvements back into ARM code for a new ROM
     * compile an entirely native NewtonOS


 Milestones
 ----------


 Resources
 ---------


 So Apple has abandoned us, but they left us some great resources - and
 challenges. (...)


 Proteus
 -------

 So here I want to try a concept I have that will make it possible to jump
 between JIT emulated ARM code and native code in Einstein. I have shown that
 this works before, but this will hopefully a much cleaner approach.

 JIT interpretation runs in a loop across arrays of ioUnits. The PC register
 determines which ioUnit to execute next. The JIT emulator has no concept
 of stacks and heaps, subroutines or interrupt. It just executes ioUnits one
 by one.

 Native code has a tremendous speed factor over JIT in Einstein. Not only do we
 save the processor the work of interpreting commands and the executing units
 with a facto of 10 to 50 over the original command, we can also take shortcut
 when accessing hardware registers, the coprocessor, or doing FP math.

 To make native code fast, it should also execute in the native order of things
 by storing temporary variables and by calling other subroutines using the
 native stack. The JIT Interpreter can call native code easily at any time.
 This is great if we can start implementing native code at the leaves of the
 call tree (leaves would be functions that do not call other functions), working
 our way up to the brancehs and ultimately the root.

 But real life is different. First of all, some leaves are rarely if ever
 called, but would be implemented first by this logic, creating a lot of work
 and very little benefit. Also, code is recursive. When implementing a major
 function, like the thread dispatch, we will eventually find out that after
 a slew of function calls, it may call itself again later, locking the native
 code development in the same loop. Lastly, code is not linear. Interrupts
 can occur at any time, and many function calls in NewtonOS can call the
 scheduler, which may switch from one task to another.

 The solution is to use a rather uncommon API that emulates the Newton hardware
 well: Fibers. Fibers are a kind of thread, but no two fibers can run at the
 same time. Netwons have a single core CPU and preemptive multitasking, which
 is very much like fibres. The only exception are the two hardware interrupts
 which work more like threads, but can be emulated with fibers with a trick.

 Everything that can be interrupted needs to run in a fiber, and everything
 that interrupts other code also needs to run in fibers. So we will evetually
 create an initial fiber, one fiber for every asynchronous CPU mode, and one
 fiber for every system task that the OS creates. For simplicity, I will put the
 SWI in its own fiber for now. So here are a list of fibers:

 Fiber 0: JIT Emulator  ---------------------------------
 Fiber 1: IRQ Interrupt ---------------------------------
 Fiber 2: FIQ Interrupt ---------------------------------
 Fiber 3: SWI Interrupt ---------------------------------
 Fiber n: TTask n       ---------------------------------

 I mentioned that only one fiber can run at a time. So lets say we run a task
 int native mode which then calls an SWI that is available native, execpt for a
 subroutine that has not been translated yet. The SWI returns to the native
 caller, which then also calls a JIT function. This would be the call graph:

 Fiber 0: JIT Emulator  -------#===#-------#===#---------
 Fiber 1: IRQ Interrupt -------|---|-------|---|---------
 Fiber 2: FIQ Interrupt -------|---|-------|---|---------
 Fiber 3: SWI Interrupt ---#===#---#===#---|---|---------
 Fiber n: TTask n      >===#-----------#===#---#=====-----

 We see that the JIT section is completely agnostic to the type of code it runs
 wheras the fibers are strictly allocated to a task.

 So what we need is an implementation of fibers (see K/Threads/TFiber), a way
 to call native subroutines from JIT, using the correct fiber, and a way to call
 JIT subroutines from native, and eventually return to the correct fiber. To
 not make this any more complicated as needed, I limit those mode changes to
 function calls (ARM 'bl' commenads).

 We need one stub that takes "C++" style parameters, converts them into ARM
 notation, sets a return address, and jumps to teh JIT fiber. We need a second
 kind of stub that chooses the right fiber and converts ARM arguments to "C++"
 calls. We also need to modify the Scheduler to switch from fiber to fiber or
 fiber to JIT or JIT to fiber.


 Memory Emulation
 ----------------

 Einstein is running in emulated memory. Let's say we have a class:

 class TQueue {
 public:
     TQueue *pPrev;
     TQueue *pNext;
 };

 The correct way to read the value of a
 member variable of a class would be:

 TQueue *q = x;
 KUInt32 tQueuePtr = (KUInt32)(uintptr_t)q;
 KUInt32 tNext = tMemoryManager->Read(tQueuePtr+4);
 TQueue *next = (TQueue*)(uintptr_t)tNext;

 But we would much rather write this:

 TQueue *q = x;
 TQueue *next = q->GetNext();

 This is moch more readable and has the huge advantage that this would compile
 and run even outside of Einstein, assuming that the Get method is implemented
 accordingly. I have decided to create macros that build getters and setters
 for both cases, inside Einstein or as standalones:

 class TQueue {
 #if EINSTEIN
 GetNext() { return (TQueue*)tMemoryManager->Read(((KUInt32)this)+4); }
 #else
 TQueue *pNext;
 GetNext() { return pNext; }
 #endif
 }

 The additional conversions should be optimized away by the compiler, and not
 being able to access pNext should make up for the improved readability. This
 even works in 64-bit mode, emulating a 32-bit machine.


 How to Use
 ----------

 Replacing any ARM code with native code requires a patch in ROM and an adapter
 that converts ARM calling convention into C++. Let's write a native version
 of 'TDoubleQContainer::GetNext(void *)' at 0x0009C89C. This is a C++ method,
 so R0 holds the 'this' pointer, R1 holds the item pointer, and the next item
 in the queue is returned in R0.

 void TDoubleQContainer::GetNext(void *item)
 {
 	// ... some native code...
 	return nextItem;
 }
 // Stub that patches the ROM to make JIT call the native function instead:
 T_JIT_TO_NATIVE(0x0009C89C, "TDoubleQContainer::GetNext(void *)") {
 	TDoubleQContainer *This = (TDoubleQContainer*)(uintptr_t)R0;
 	void *item = (TDoubleQContainer*)(uintptr_t)R1;
 	void *ret = This->GetNext(item);
 	R0 = (KUInt32)(uintptr_t)ret;
 	EXIT(LR);
 }

 To call JIT code from mative code, we need similar code to convert native
 parameters into ARM calling convention. Here we replace a call to
 TSingleQContainer::Peek() at 0x001E2C18:

 void someNativeFunction() {
 	// ...
 	// Replace 'void *p = myQueue->Peek()' which was originally
 	// 'bl 0x001E2C18 ; TSingleQContainer::Peek()'
 	R0 = (KUInt32)(uintptr_t)myQueue;
 	FindFiber()->SwitchToJIT(0x001E2C18);
 	void *p = (void*)(uintptr_t)R0;
 	// ...
 }

 */

// --- proteus variables


#if 0


/*
 Let's implement this part of the SWI:
 stmdb   sp!, {r0}                       @ 0x003AD80C 0xE92D0001 - .-..
 bl      VEC_StartScheduler              @ 0x003AD810 0xEB5D66F1 - .]f.
 ldmia   sp!, {r0}                       @ 0x003AD814 0xE8BD0001 - ....
*/

void swi_native_test()
{
	PUSH(SP, R0);

	FindFiber()->SwitchToJIT(0x001CC4A8); // StartScheduler

	POP(SP, R0);
	PC = 0x003AD818+4;
}

T_JIT_TO_NATIVE(0x003AD80C, "SWI Test") {
	swi_native_test();
}


/*
mov     r4, r0                          @ 0x000E589C 0xE1A04000 - ..@.
*/
void swi_native_test_2()
{
	R4 = R0;
	PC = 0x000E58A0 + 4;
}


T_JIT_TO_NATIVE(0x000E589C, "SWI Test 2") {
	swi_native_test_2();
}

#endif








