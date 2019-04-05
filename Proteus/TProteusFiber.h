//
//  TProteusFiber.hpp
//  Einstein
//
//  Created by Matthias Melcher on 4/4/19.
//

#ifndef T_PROTEUS_FIBER_H
#define T_PROTEUS_FIBER_H

#include "TProteusMacros.h"
#include "K/Threads/TFiber.h"


namespace NewtOS {


typedef void (*FPtr)();

enum {
	kFiberAbort = 1,
	kFiberIdle,
	kFiberCallNative,
	kFiberCallJIT,
	kFiberReturn,
	kFiberUnknown
};

class TProteusFiber : public TFiber
{
public:
	TProteusFiber() { }
	KSInt32 Task(KSInt32 inReason=0, void* inUserData=0L)
	{
		SwitchToJIT();
		return 0;
	}
	void SwitchToJIT(KUInt32 pc=0xFFFFFFFF) {
		LR = 0x007FFFF0;
		if (pc!=0xFFFFFFFF) PC = pc+4;
		for (;;) {
			FPtr fn;
			int reason = Suspend(kFiberCallJIT);
			switch (reason) {
				case kFiberReturn:
					return;
				case kFiberCallNative:
					fn = (FPtr)GetUserData();
					fn();
					break;
				default:
					fprintf(stderr, "ERROR: TSVCFiber::SwitchToJIT, unexpected Resume(%d)\n", reason);
			}
		}
	}
};

extern TProteusFiber *FindFiber();

extern TProteusFiber *svcFiber;

#define T_JIT_TO_NATIVE(addr, name) \
extern JITInstructionProto(p##addr); \
extern void P##addr(); \
TJITGenericPatchNativeInjection i##addr(addr, p##addr, name); \
JITInstructionProto(p##addr) { \
if (TProteusFiber *fiber = FindFiber()) { \
fiber->Resume(kFiberCallNative, (void*)P##addr); \
return nullptr; \
} else return ioUnit; \
} \
void P##addr()

} // namespace;

#endif /* T_PROTEUS_FIBER_H */
