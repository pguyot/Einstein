// ==============================
// File:                        Newt/Common/Glue.h
// Project:                     Simulator
//
// Copyright 1999-2012 by Newton Research Group and others
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

#ifndef NEWT_COMMON_GLUE_H
#define NEWT_COMMON_GLUE_H

#include <K/Defines/KDefinitions.h>
#include <K/Threads/TFibre.h>

#include "Newt/Common/Types.h"

// Einstein
#include "JIT.h"
#include "TARMProcessor.h"
#include "TJITGeneric_Macros.h"
#include "TROMImage.h"


extern class NewtGlueFibre* gCurrentFibre;
extern class TARMProcessor* gCurrentCPU;
	
KUInt32 NewtReadWord(KUInt32);
inline KUInt32 NewtReadWord(void* p) { return NewtReadWord((KUInt32)p); }
void NewtWriteWord(KUInt32, KUInt32);
inline void NewtWriteWord(void* p, KUInt32 v) { NewtWriteWord((KUInt32)p, v); }
	
KUInt8 NewtReadByte(KUInt32);
inline KUInt8 NewtReadByte(void* p) { return NewtReadByte((KUInt32)p); }
void NewtWriteByte(KUInt32, KUInt8);
inline void NewtWriteByte(void* p, KUInt8 v) { NewtWriteByte((KUInt32)p, v); }
	
void NewtCallJIT(KUInt32 addr);
	
class NewtGlueDispatch
{
public:
	static JITUnit* Dispatch(JITUnit* ioUnit, TARMProcessor* ioCPU, KUInt32 callIndex);
	NewtGlueDispatch();
};
	
class NewtGlueStackManager
{
public:
	KUInt32 pSize;
	NewtGlueStackManager(KUInt32 n) {
		pSize = n;
		gCurrentCPU->SetRegister(13, gCurrentCPU->GetRegister(13)-pSize);
	}
	~NewtGlueStackManager() {
		gCurrentCPU->SetRegister(13, gCurrentCPU->GetRegister(13)+pSize);
	}
};



#define NEWT_INJECTION(addr, name) \
	extern KSInt32 p##addr(); \
	TROMSimulatorInjection i##addr(addr, p##addr, name); \
	KSInt32 p##addr()

#define NEWT_GET_SET_W(type, name) \
	private: type p##name; \
	public:  type Get##name() { return (type)NewtReadWord( (KUInt32)&p##name ); } \
			 void Set##name(type v) { NewtWriteWord( (KUInt32)&p##name, (KUInt32)v ); } \
			 type* PtrTo##name() { return &p##name; }

#define NEWT_IS_SET_W(type, name) \
	private: type p##name; \
	public:  type Is##name() { return (type)NewtReadWord( (KUInt32)&p##name ); } \
			 void Set##name(type v) { NewtWriteWord( (KUInt32)&p##name, (KUInt32)v ); }

#define NEWT_GET_SET_W_ARRAY(type, name, size) \
	private: type p##name[size]; \
	public:  type Get##name(KUInt32 ix) { return (type)NewtReadWord( ((KUInt32)&p##name)+4*ix ); } \
	         void Set##name(KUInt32 ix, type v) { NewtWriteWord( ((KUInt32)&p##name)+4*ix, (KUInt32)v ); } \
			 type* PtrTo##name##s() { return p##name; } \
			 type* PtrTo##name(KUInt32 ix) { return p##name + ix; }

#define NEWT_GET_SET_B(type, name) \
	private: type p##name; \
	public:  type Get##name() { return (type)NewtReadByte( (KUInt32)&p##name ); } \
	         void Set##name(type v) { NewtWriteByte( (KUInt32)&p##name, (KUInt8)v ); }

#define NEWT_IS_SET_B(type, name) \
	private: type p##name; \
	public:  type Is##name() { return (type)NewtReadByte( (KUInt32)&p##name ); } \
			 void Set##name(type v) { NewtWriteByte( (KUInt32)&p##name, (KUInt8)v ); }

#define NEWT_RETURN \
	gCurrentCPU->SetRegister(15, gCurrentCPU->GetRegister(14)+4); \
	return 0

#define NEWT_RETVAL \
	gCurrentCPU->mCurrentRegisters[0] = (KUInt32)

#define NEWT_CLASS(type) \
	((type*)gCurrentCPU->GetRegister(0))

#define NEWT_ARG0(type) \
	((type)gCurrentCPU->GetRegister(0))

#define NEWT_ARG1(type) \
	((type)gCurrentCPU->GetRegister(1))

#define NEWT_ARG2(type) \
	((type)gCurrentCPU->GetRegister(2))

#define NEWT_ARG3(type) \
	((type)gCurrentCPU->GetRegister(3))

#define NEWT_PUSH_REGISTERS \
	{ \
		KUInt32 _r0 = gCurrentCPU->GetRegister(0); \
		KUInt32 _r1 = gCurrentCPU->GetRegister(1); \
		KUInt32 _r2 = gCurrentCPU->GetRegister(2); \
		KUInt32 _r3 = gCurrentCPU->GetRegister(3); \
		KUInt32 _r4 = gCurrentCPU->GetRegister(4); \
		KUInt32 _r5 = gCurrentCPU->GetRegister(5); \
		KUInt32 _r6 = gCurrentCPU->GetRegister(6); \
		KUInt32 _r7 = gCurrentCPU->GetRegister(7); \
		KUInt32 _r8 = gCurrentCPU->GetRegister(8); \
		KUInt32 _r9 = gCurrentCPU->GetRegister(9); \
		KUInt32 _r10 = gCurrentCPU->GetRegister(10); \
		KUInt32 _r11 = gCurrentCPU->GetRegister(11); \
		KUInt32 _r12 = gCurrentCPU->GetRegister(12); \
		KUInt32 _r13 = gCurrentCPU->GetRegister(13); \
		KUInt32 _r14 = gCurrentCPU->GetRegister(14); \
		KUInt32 _r15 = gCurrentCPU->GetRegister(15);

#define NEWT_POP_REGISTERS \
		gCurrentCPU->SetRegister(15, _r15); \
		gCurrentCPU->SetRegister(14, _r14); \
		gCurrentCPU->SetRegister(13, _r13); \
		gCurrentCPU->SetRegister(12, _r12); \
		gCurrentCPU->SetRegister(11, _r11); \
		gCurrentCPU->SetRegister(10, _r10); \
		gCurrentCPU->SetRegister(9, _r9); \
		gCurrentCPU->SetRegister(8, _r8); \
		gCurrentCPU->SetRegister(7, _r7); \
		gCurrentCPU->SetRegister(6, _r6); \
		gCurrentCPU->SetRegister(5, _r5); \
		gCurrentCPU->SetRegister(4, _r4); \
		gCurrentCPU->SetRegister(3, _r3); \
		gCurrentCPU->SetRegister(2, _r2); \
		gCurrentCPU->SetRegister(1, _r1); \
		gCurrentCPU->SetRegister(0, _r0); \
	}

#define NEWT_GLOBAL_W_DECL(type, name, addr) \
	extern type g##name; \
	void GlobalSet##name(type); \
	type GlobalGet##name();

#define NEWT_GLOBAL_W_IMPL(type, name, addr) \
	type g##name; \
	void GlobalSet##name(type v) { NewtWriteWord(addr, (KUInt32)v); } \
	type GlobalGet##name() { return (type)NewtReadWord(addr); }


#define NEWT_LOCAL(type, name) \
	NewtGlueStackManager ngManage##name(sizeof(type)); \
	type* ngPtr##name = (type*)(gCurrentCPU->GetRegister(13));

#define NEWT_LOACL_REF(name) \
	(*(ngPtr##Name))

#define NEWT_LOCAL_PTR(name) \
	(ngPtr##name)

#define NEWT_LOCAL_GET_W(type, name) \
	(type)NewtReadWord(ngPtr##name)

typedef KSInt32 (*NewtGlueTask)();

class NewtGlueFibre : public TFibre {
public:
	NewtGlueFibre();
	KSInt32 Task(KSInt32 inReason=0, void* inUserData=0L);
	KUInt32 pRecursions;
	KUInt32 pSP;
	KUInt32 pTaskIndex;
};



#endif
// NEWT_COMMON_GLUE_H

