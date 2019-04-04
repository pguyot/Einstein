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

#endif /* T_PROTEUS_FIBER_H */
