//
//  NewtSim.cp
//  Einstein
//
//  Created by Matthias Melcher on 9/3/14.
//
//

#include "NewtSim.h"

#include <stdio.h>

#include "SingleQContainer.h"


TARMProcessor *cpu;
KUInt32 stack[16384];


class SingleQContainer
{
public:
	SingleQContainer() {
		cpu->mCurrentRegisters[0] = (KUInt32)this;
		Func_0x001E2BBC(cpu);
	}
	void Init(unsigned long offset) {
		cpu->mCurrentRegisters[0] = (KUInt32)this;
		cpu->mCurrentRegisters[1] = (KUInt32)offset;
		Func_0x001E2BCC(cpu);
	}
	void Add(void* data) {
		cpu->mCurrentRegisters[0] = (KUInt32)this;
		cpu->mCurrentRegisters[1] = (KUInt32)data;
		Func_0x001E2BDC(cpu);
	}
	void *Remove() {
		cpu->mCurrentRegisters[0] = (KUInt32)this;
		Func_0x001E2BF4(cpu);
		return (void*)cpu->mCurrentRegisters[0];
	}
	void *Peek() {
		cpu->mCurrentRegisters[0] = (KUInt32)this;
		Func_0x001E2C18(cpu);
		return (void*)cpu->mCurrentRegisters[0];
	}
	void *GetNext(void* data) {
		cpu->mCurrentRegisters[0] = (KUInt32)this;
		cpu->mCurrentRegisters[1] = (KUInt32)data;
		Func_0x001E2C30(cpu);
		return (void*)cpu->mCurrentRegisters[0];
	}
private:
	void*		pFirst;
	KUInt32		pOffsetToNext;
};


void InitSim()
{
	cpu = new TARMProcessor();
	cpu->mCurrentRegisters[14] = (KUInt32)(stack+16383);
}


/*
Check the Simulation as a stand-alone tool.
 */
int main(int, char**)
{
	typedef struct { void* next; const char* t; } Data;
	Data d0 = { 0, "The quick" };
	Data d1 = { 0, "brown fox" };
	Data d2 = { 0, "jumps over" };
	Data d3 = { 0, "the lazy dog." };
	InitSim();
	SingleQContainer q;
	q.Init(0);
	q.Add(&d0);
	q.Add(&d1);
	q.Add(&d2);
	q.Add(&d3);

	Data *di = (Data*)q.Peek();
	while (di) {
		printf("Data: '%s'\n", di->t);
		di = (Data*)q.GetNext(di);
	}
	
	for (int i=0; i<4; i++) {
		Data *d = (Data*)q.Remove();
		printf("Data: '%s'\n", d->t);
	}
	
	return 0;
}


void
SetCPSRBitsForLogicalOpLeaveCarry( TARMProcessor* ioCPU, KUInt32 inResult )
{
	if (inResult == 0)
	{
		ioCPU->mCPSR_Z = true;
		ioCPU->mCPSR_N = false;
	} else {
		ioCPU->mCPSR_Z = false;
		if (inResult & 0x80000000)
		{
			ioCPU->mCPSR_N = true;
		} else {
			ioCPU->mCPSR_N = false;
		}
	}
}



