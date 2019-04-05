//
//  KernelTask.hpp
//  Einstein
//
//  Created by Matthias Melcher on 4/5/19.
//

#ifndef PROTEUS_OS_KERNEL_TASK_H
#define PROTEUS_OS_KERNEL_TASK_H

#include "Proteus.h"

#include "Environment.h"

namespace NewtOS {

class TTask
{
public:
	T_GETSET_MEMBER_W(0x10, KUInt32, R0);
	T_GETSET_MEMBER_W(0x14, KUInt32, R1);
	T_GETSET_MEMBER_W(0x18, KUInt32, R2);
	T_GETSET_MEMBER_W(0x1c, KUInt32, R3);
	T_GETSET_MEMBER_W(0x20, KUInt32, R4);
	T_GETSET_MEMBER_W(0x24, KUInt32, R5);
	T_GETSET_MEMBER_W(0x28, KUInt32, R6);
	T_GETSET_MEMBER_W(0x2c, KUInt32, R7);
	T_GETSET_MEMBER_W(0x30, KUInt32, R8);
	T_GETSET_MEMBER_W(0x34, KUInt32, R9);
	T_GETSET_MEMBER_W(0x38, KUInt32, R10);
	T_GETSET_MEMBER_W(0x3c, KUInt32, R11);
	T_GETSET_MEMBER_W(0x40, KUInt32, R12);
	T_GETSET_MEMBER_W(0x44, KUInt32, SP);
	T_GETSET_MEMBER_W(0x48, KUInt32, LR);
	T_GETSET_MEMBER_W(0x4c, KUInt32, PC);
	T_GETSET_MEMBER_W(0x50, KUInt32, PSR);

	T_GETSET_MEMBER_P(0x74, TEnvironment*, Environment);
	T_GETSET_MEMBER_P(0x78, TEnvironment*, SMemEnvironment);
	T_GETSET_MEMBER_P(0x7C, TTask*, MonitorTask);
};


} // namespace


#endif /* PROTEUS_OS_KERNEL_TASK_H */
