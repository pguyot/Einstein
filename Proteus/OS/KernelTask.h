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
	T_GETSET_MEMBER_W(0x10, KUInt32, 10_R0);
	T_GETSET_MEMBER_W(0x14, KUInt32, 14_R1);
	T_GETSET_MEMBER_W(0x18, KUInt32, 18_R2);
	T_GETSET_MEMBER_W(0x1c, KUInt32, 1C_R3);
	T_GETSET_MEMBER_W(0x20, KUInt32, 20_R4);
	T_GETSET_MEMBER_W(0x24, KUInt32, 24_R5);
	T_GETSET_MEMBER_W(0x28, KUInt32, 28_R6);
	T_GETSET_MEMBER_W(0x2c, KUInt32, 2C_R7);
	T_GETSET_MEMBER_W(0x30, KUInt32, 30_R8);
	T_GETSET_MEMBER_W(0x34, KUInt32, 34_R9);
	T_GETSET_MEMBER_W(0x38, KUInt32, 38_R10);
	T_GETSET_MEMBER_W(0x3c, KUInt32, 3C_R11);
	T_GETSET_MEMBER_W(0x40, KUInt32, 40_R12);
	T_GETSET_MEMBER_W(0x44, KUInt32, 44_SP);
	T_GETSET_MEMBER_W(0x48, KUInt32, 48_LR);
	T_GETSET_MEMBER_W(0x4c, KUInt32, 4C_PC);
	T_GETSET_MEMBER_W(0x50, KUInt32, 50_PSR);

	T_GETSET_MEMBER_P(0x74, TEnvironment*, 74_Environment);
	T_GETSET_MEMBER_P(0x78, TEnvironment*, 78_SMemEnvironment);
	T_GETSET_MEMBER_P(0x7C, TTask*, 7C_MonitorTask);
};


} // namespace


#endif /* PROTEUS_OS_KERNEL_TASK_H */
