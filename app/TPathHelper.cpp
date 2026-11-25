//
//  TPathHelper.cpp
//  Einstein
//
//  Created by Steven Frank on 2/20/17.
//
//

#include "TPathHelper.h"
#include "Emulator/Log/KPrintf.h"

#include <K/Defines/KDefinitions.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_OSX
#include "TCocoaPathHelper.h"
#endif
#endif

std::string
TPathHelper::GetSerialPipeBasePath()
{
#if defined(__APPLE__) && TARGET_OS_OSX
	return TCocoaPathHelper::GetSerialPipeBasePath();
#else
	KPrintf("TPathHelper::GetSerialPipeBasePath() not implemented on this platform!\n");
	return ".";
#endif
}
