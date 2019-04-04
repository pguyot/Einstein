//
//  Globals.cpp
//  Einstein
//
//  Created by Matthias Melcher on 4/5/19.
//

#include "Globals.h"

//-/* 0x0C008400-0x0C107E18 */ Range of global variables in memory
// /* 0x0C008400-0x0C100E58 */

GLOBAL_GETSET_W(0x0C100E58, KSInt32, AtomicFIQNestCountFast);

GLOBAL_GETSET_W(0x0C100E5C, KSInt32, AtomicIRQNestCountFast);

// /* 0x0C100E60-0x0C100FEC */

GLOBAL_GETSET_W(0x0C100FE8, KSInt32, AtomicNestCount);

// /* 0x0C100FE8-0x0C100FF0 */

GLOBAL_GETSET_W(0x0C100FF0, KSInt32, AtomicFIQNestCount);

// /* 0x0C100FF4-0x0C107E18 */

