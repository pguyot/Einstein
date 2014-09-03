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

/*
class TInterpreter {
public:
 StackTrace__12TInterpreterFv:			@ 0x002D3CC4: TInterpreter::StackTrace(void)
 GetLocalFromStack__12TInterpreterFRC6RefVarT1:	@ 0x002D3CD8: TInterpreter::GetLocalFromStack(RefVar const &, RefVar const &)
 SetLocalOnStack__12TInterpreterFRC6RefVarN21:	@ 0x002D3D8C: TInterpreter::SetLocalOnStack(RefVar const &, RefVar const &, RefVar const &)
 GetSelfFromStack__12TInterpreterFRC6RefVar:	@ 0x002D3E44: TInterpreter::GetSelfFromStack(RefVar const &)
 HandleBreakPoints__12TInterpreterFv:		@ 0x002D3F6C: TInterpreter::HandleBreakPoints(void)
 SetBreakPoints__12TInterpreterFRC6RefVar:	@ 0x002D41AC: TInterpreter::SetBreakPoints(RefVar const &)
 EnableBreakPoints__12TInterpreterFUc:		@ 0x002D41C8: TInterpreter::EnableBreakPoints(unsigned char)
 FastRun__12TInterpreterFl:			@ 0x002EE0A8: TInterpreter::FastRun(long)
 FastRun1__12TInterpreterFlR12FastRunState:	@ 0x002EE138: TInterpreter::FastRun1(long, FastRunState &)
 SetFastLoopFlag__12TInterpreterFv:		@ 0x002F1CEC: TInterpreter::SetFastLoopFlag(void)
 AlternatingLoops__12TInterpreterFl:		@ 0x002F1D68: TInterpreter::AlternatingLoops(long)
 Run__12TInterpreterFv:				@ 0x002F1DB4: TInterpreter::Run(void)
 SlowRun__12TInterpreterFl:			@ 0x002F1EE0: TInterpreter::SlowRun(long)
 __ct__12TInterpreterFv:				@ 0x002F40E0: TInterpreter::TInterpreter(void)
 __dt__12TInterpreterFv:				@ 0x002F41CC: TInterpreter::~TInterpreter(void)
 PushValue__12TInterpreterFRC6RefVar:		@ 0x002F4274: TInterpreter::PushValue(RefVar const &)
 PopValue__12TInterpreterFv:			@ 0x002F428C: TInterpreter::PopValue(void)
 PeekValue__12TInterpreterFl:			@ 0x002F42A0: TInterpreter::PeekValue(long)
 SetValue__12TInterpreterFlT1:			@ 0x002F42B4: TInterpreter::SetValue(long, long)
 ValuePosition__12TInterpreterFv:		@ 0x002F42C8: TInterpreter::ValuePosition(void)
 PeekControl__12TInterpreterFl:			@ 0x002F42E4: TInterpreter::PeekControl(long)
 SetControl__12TInterpreterFlT1:			@ 0x002F42F8: TInterpreter::SetControl(long, long)
 ControlPosition__12TInterpreterFv:		@ 0x002F430C: TInterpreter::ControlPosition(void)
 TopLevelCall__12TInterpreterFRC6RefVarT1:	@ 0x002F4328: TInterpreter::TopLevelCall(RefVar const &, RefVar const &)
 Call__12TInterpreterFRC6RefVarl:		@ 0x002F443C: TInterpreter::Call(RefVar const &, long)
 Send__12TInterpreterFRC6RefVarN21l:		@ 0x002F477C: TInterpreter::Send(RefVar const &, RefVar const &, RefVar const &, long)
 CallCodeBlock__12TInterpreterFRC6RefVarlT2:	@ 0x002F4A54: TInterpreter::CallCodeBlock(RefVar const &, long, long)
 CallPlainCodeBlock__12TInterpreterFRC6RefVarlT2:	@ 0x002F4C04: TInterpreter::CallPlainCodeBlock(RefVar const &, long, long)
 CallCFunction__12TInterpreterFRC6RefVarli:	@ 0x002F4DA8: TInterpreter::CallCFunction(RefVar const &, long, int)
 CallPlainCFunction__12TInterpreterFRC6RefVarl:	@ 0x002F4F8C: TInterpreter::CallPlainCFunction(RefVar const &, long)
 CallCFuncPtr__12TInterpreterFPFRC6RefVare_ll:	@ 0x002F5124: TInterpreter::CallCFuncPtr(long (*)(RefVar const &, ...), long)
 Return__12TInterpreterF19FramesProfilingKind:	@ 0x002F52C8: TInterpreter::Return(FramesProfilingKind)
 SetFlags__12TInterpreterFv:			@ 0x002F543C: TInterpreter::SetFlags(void)
 PopHandlers__12TInterpreterFv:			@ 0x002F5674: TInterpreter::PopHandlers(void)
 TranslateException__12TInterpreterFP9Exception:	@ 0x002F5890: TInterpreter::TranslateException(Exception *)
 ExceptionBeingHandled__12TInterpreterFv:	@ 0x002F59AC: TInterpreter::ExceptionBeingHandled(void)
 HandleException__12TInterpreterFP9ExceptionlR10StackState:	@ 0x002F5DE8: TInterpreter::HandleException(Exception *, long, StackState &)
 GetReceiver__12TInterpreterFv:			@ 0x002F63B4: TInterpreter::GetReceiver(void)
 GetImplementor__12TInterpreterFv:		@ 0x002F63C4: TInterpreter::GetImplementor(void)
 IsSend__12TInterpreterFv:			@ 0x002F63D4: TInterpreter::IsSend(void)
 SetCallEnv__12TInterpreterFv:			@ 0x002F63DC: TInterpreter::SetCallEnv(void)
 SetSendEnv__12TInterpreterFRC6RefVarT1:		@ 0x002F63E8: TInterpreter::SetSendEnv(RefVar const &, RefVar const &)
 TaciturnPrintObject__12TInterpreterFRC6RefVarl:	@ 0x0035E4F4: TInterpreter::TaciturnPrintObject(RefVar const &, long)
 TraceSend__12TInterpreterFRC6RefVarT1lT3:	@ 0x0035E600: TInterpreter::TraceSend(RefVar const &, RefVar const &, long, long)
 TraceFreqCall__12TInterpreterFl:		@ 0x0035E658: TInterpreter::TraceFreqCall(long)
 TraceApply__12TInterpreterFRC6RefVarl:		@ 0x0035E6D0: TInterpreter::TraceApply(RefVar const &, long)
 TraceCall__12TInterpreterFRC6RefVarl:		@ 0x0035E740: TInterpreter::TraceCall(RefVar const &, long)
 TraceSetOptions__12TInterpreterFv:		@ 0x0035E79C: TInterpreter::TraceSetOptions(void)
 TraceGet__12TInterpreterFRC6RefVarN21:		@ 0x0035EA38: TInterpreter::TraceGet(RefVar const &, RefVar const &, RefVar const &)
 TraceSet__12TInterpreterFRC6RefVarN31:		@ 0x0035EC00: TInterpreter::TraceSet(RefVar const &, RefVar const &, RefVar const &, RefVar const &)
 TraceReturn__12TInterpreterFUc:			@ 0x0035ED6C: TInterpreter::TraceReturn(unsigned char)
 TraceReturn__12TInterpreterFv:			@ 0x0035EE30: TInterpreter::TraceReturn(void)
 TraceArgs__12TInterpreterFlN21:			@ 0x0035EE38: TInterpreter::TraceArgs(long, long, long)
 TraceMethod__12TInterpreterFRC6RefVarT1PclT4:	@ 0x0035EEC8: TInterpreter::TraceMethod(RefVar const &, RefVar const &, char *, long, long)
};
 */

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
	void		ReturnToEmualtor(KUInt32 inPC) { throw 0L; } // should never be called

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
