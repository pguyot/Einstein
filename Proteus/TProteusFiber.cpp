//
//  TProteusFiber.cpp
//  Einstein
//
//  Created by Matthias Melcher on 4/4/19.
//

#include "TProteusFiber.h"


TProteusFiber *FindFiber()
{
	if (CPU->GetMode()==CPU->kSupervisorMode)
		return svcFiber;
	return nullptr;
}

TProteusFiber *svcFiber = nullptr;


