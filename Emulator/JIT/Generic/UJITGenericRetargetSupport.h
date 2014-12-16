// ==============================
// File:			UJITGenericRetargetSupport.h
// Project:			Einstein
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

#ifndef _UJITGENERICRETARGETSUPPORT_H
#define _UJITGENERICRETARGETSUPPORT_H

#include <K/Defines/KDefinitions.h>
#include "JIT.h"

class TARMProcessor;

class UJITGenericRetargetSupport {
public:
	//
	// Read a word in memory and handle access violations if needed.
	//
	static KUInt32		ManagedMemoryReadAligned(TARMProcessor* ioCPU, KUInt32 inAddress);
	static KUInt32		ManagedMemoryRead(TARMProcessor* ioCPU, KUInt32 inAddress);
	static KUInt8		ManagedMemoryReadB(TARMProcessor* ioCPU, KUInt32 inAddress);
	static void		ManagedMemoryWriteAligned(TARMProcessor* ioCPU, KUInt32 inAddress, KUInt32 inData);
	static void		ManagedMemoryWrite(TARMProcessor* ioCPU, KUInt32 inAddress, KUInt32 inData);
	static void		ManagedMemoryWriteB(TARMProcessor* ioCPU, KUInt32 inAddress, KUInt8 inData);
	static void		UnexpectedPC(TARMProcessor* ioCPU);
	static void		ReturnToEmulator(TARMProcessor* ioCPU, KUInt32 inPC);
	static void		JumpToCalculatedAddress(TARMProcessor* ioCPU, KUInt32 inPC, KUInt32 ret=0xFFFFFFFF);	
};


#endif
