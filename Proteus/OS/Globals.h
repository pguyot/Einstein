//
//  Globals.hpp
//  Einstein
//
//  Created by Matthias Melcher on 4/5/19.
//

#ifndef PROTEUS_OS_GLOBALS_H
#define PROTEUS_OS_GLOBALS_H

#include "Proteus.h"

namespace NewtOS {

	class TTask;

//-/* 0x0C008400-0x0C107E18 */ Range of global variables in memory

	extern TImageParamBlock *gParamBlockFromImage;

// /* 0x0C0084E4-0x0C100E58 */

GLOBAL_GETSET_W_DECL(0x0C100E58, KSInt32, AtomicFIQNestCountFast);

GLOBAL_GETSET_W_DECL(0x0C100E5C, KSInt32, AtomicIRQNestCountFast);

// /* 0x0C100E60-0x0C100FE4 */

GLOBAL_GETSET_W_DECL(0x0C100FE4, KUInt32, Schedule);

GLOBAL_GETSET_W_DECL(0x0C100FE8, KSInt32, AtomicNestCount);

// /* 0x0C100FEC-0x0C100FF0 */

GLOBAL_GETSET_W_DECL(0x0C100FF0, KSInt32, AtomicFIQNestCount);

// /* 0x0C100FF4-0x0C100FF8 */

GLOBAL_GETSET_P_DECL(0x0C100FF8, TTask*, CurrentTask);

// /* 0x0C100FFC-0x0C101028 */

GLOBAL_GETSET_W_DECL(0x0C101028, KUInt32, WantDeferred);

// /* 0x0C10102C-0x0C101040 */

GLOBAL_GETSET_W_DECL(0x0C101040, KUInt32, CopyDone);

// /* 0x0C101044-0x0C101A2C */

GLOBAL_GETSET_W_DECL(0x0C101A2C, KUInt32, WantSchedulerToRun);

// /* 0x0C101A30-0x0C107E18 */

} // namespace;

#endif /* PROTEUS_OS_GLOBALS_H */
