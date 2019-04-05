//
//  KernelTask.hpp
//  Einstein
//
//  Created by Matthias Melcher on 4/5/19.
//

#ifndef PROTEUS_OS_KERNEL_TASK_H
#define PROTEUS_OS_KERNEL_TASK_H

#include "Proteus.h"

namespace NewtOS {

class TTask
{
public:
	T_GETSET_MEMBER_W(80, KUInt32, PSR);
};


} // namespace


#endif /* PROTEUS_OS_KERNEL_TASK_H */
