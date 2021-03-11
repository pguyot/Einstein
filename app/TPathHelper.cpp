//
//  TPathHelper.cpp
//  Einstein
//
//  Created by Steven Frank on 2/20/17.
//
//

#include "TPathHelper.h"

#include <K/Defines/KDefinitions.h>

#if TARGET_OS_MAC
#include "TCocoaPathHelper.h"
#endif

std::string TPathHelper::GetSerialPipeBasePath()
{
#if TARGET_OS_MAC
	return TCocoaPathHelper::GetSerialPipeBasePath();
#else
	KPrintf("TPathHelper::GetSerialPipeBasePath() not implemented on this platform!\n");
	return ".";
#endif
}
