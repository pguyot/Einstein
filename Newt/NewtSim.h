//
//  NewtSim.h
//  Einstein
//
//  Created by Matthias Melcher on 9/3/14.
//
//

#ifndef __Einstein__NewtSim__
#define __Einstein__NewtSim__


#include <stdint.h>
#include <MacTypes.h>


typedef uint32_t KUInt32;
typedef uint8_t KUInt8;



class TARMProcessor
{
public:
	Boolean TestEQ() { return mCPSR_Z; }
	Boolean TestNE() { return !mCPSR_Z; }
	Boolean TestCS() { return mCPSR_C; }
	Boolean TestCC() { return !mCPSR_C; }
	Boolean TestMI() { return mCPSR_N; }
	Boolean TestPL() { return !mCPSR_N; }
	Boolean TestVS() { return mCPSR_V; }
	Boolean TestVC() { return !mCPSR_V; }
	Boolean TestHI() { return ((mCPSR_C) && !(mCPSR_Z)); }
	Boolean TestLS() { return (!(mCPSR_C) || (mCPSR_Z)); }
	Boolean TestGE() { return (mCPSR_N == mCPSR_V); }
	Boolean TestLT() { return (mCPSR_N != mCPSR_V); }
	Boolean TestGT() { return ((!mCPSR_Z) && (mCPSR_N == mCPSR_V)); }
	Boolean TestLE() { return ((mCPSR_Z) || (mCPSR_N != mCPSR_V)); }

	KUInt32		ManagedMemoryReadAligned(KUInt32 inAddress, KUInt32 inPC) {
		return *((KUInt32*)(inAddress));
	}
	KUInt32		ManagedMemoryRead(KUInt32 inAddress, KUInt32 inPC) {
		return *((KUInt32*)(inAddress));
	}
	KUInt8		ManagedMemoryReadB(KUInt32 inAddress, KUInt32 inPC) {
		return *((KUInt8*)(inAddress));
	}
	void		ManagedMemoryWriteAligned(KUInt32 inAddress, KUInt32 inData, KUInt32 inPC) {
		*((KUInt32*)(inAddress)) = inData;
	}
	void		ManagedMemoryWrite(KUInt32 inAddress, KUInt32 inData, KUInt32 inPC) {
		*((KUInt32*)(inAddress)) = inData;
	}
	void		ManagedMemoryWriteB(KUInt32 inAddress, KUInt8 inData, KUInt32 inPC) {
		*((KUInt8*)(inAddress)) = inData;
	}
	void		UnexpectedPC();
	void		ReturnToEmulator(KUInt32 inPC) { throw 0L; } // should never be called

	KUInt32		mCurrentRegisters[16];
	Boolean		mCPSR_N;					///< N Flag of CPSR.
	Boolean		mCPSR_Z;					///< Z Flag of CPSR.
	Boolean		mCPSR_C;					///< C Flag of CPSR.
	Boolean		mCPSR_V;					///< V Flag of CPSR.
	Boolean		mCPSR_I;					///< I Bit of CPSR.
	Boolean		mCPSR_F;					///< F Bit of CPSR.
	Boolean		mCPSR_T;					///< T Bit of CPSR.
	KUInt32		mR8_Bkup;					///< A copy of R8
	KUInt32		mR9_Bkup;					///< A copy of R9
	KUInt32		mR10_Bkup;					///< A copy of R10
	KUInt32		mR11_Bkup;					///< A copy of R11
	KUInt32		mR12_Bkup;					///< A copy of R12
	KUInt32		mR13_Bkup;					///< A copy of R13
	KUInt32		mR14_Bkup;					///< A copy of R14
	KUInt32		mR13svc_Bkup;				///< A copy of R13svc
	KUInt32		mR14svc_Bkup;				///< A copy of R14svc
	KUInt32		mR13abt_Bkup;				///< A copy of R13abt
	KUInt32		mR14abt_Bkup;				///< A copy of R14abt
	KUInt32		mR13und_Bkup;				///< A copy of R13und
	KUInt32		mR14und_Bkup;				///< A copy of R14und
	KUInt32		mR13irq_Bkup;				///< A copy of R13irq
	KUInt32		mR14irq_Bkup;				///< A copy of R14irq
	KUInt32		mR8fiq_Bkup;				///< A copy of R8fiq
	KUInt32		mR9fiq_Bkup;				///< A copy of R9fiq
	KUInt32		mR10fiq_Bkup;				///< A copy of R10fiq
	KUInt32		mR11fiq_Bkup;				///< A copy of R11fiq
	KUInt32		mR12fiq_Bkup;				///< A copy of R12fiq
	KUInt32		mR13fiq_Bkup;				///< A copy of R13fiq
	KUInt32		mR14fiq_Bkup;				///< A copy of R14fiq
	KUInt32		mSPSRsvc;					///< Saved PSR for svc mode
	KUInt32		mSPSRabt;					///< Saved PSR for abt mode
	KUInt32		mSPSRund;					///< Saved PSR for und mode
	KUInt32		mSPSRirq;					///< Saved PSR for irq mode
	KUInt32		mSPSRfiq;					///< Saved PSR for fiq mode
};


void SetCPSRBitsForLogicalOpLeaveCarry( TARMProcessor* ioCPU, KUInt32 inResult );


#define SETPC(v) ioCPU->mCurrentRegisters[15] = (KUInt32)(v);
#define T_ROM_SIMULATION(a, b) //

#endif /* defined(__Einstein__NewtSim__) */
