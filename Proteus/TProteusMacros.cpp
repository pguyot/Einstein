// ==============================
// File:			TProteusMacros.cpp
// Project:			Einstein
//
// Copyright 2019 by Matthais Melcher (proteus@matthiasm.com).
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


#include "TProteusMacros.h"
#include "TProteusFiber.h"


namespace NewtOS {


TARMProcessor *CPU = nullptr;
TMemory *MEM = nullptr;
TInterruptManager *INT = nullptr;

/**
 * This injection initializes the Proteus system by setting up a few
 * variables for fast acces in later calls.
 */
T_ROM_INJECTION(0x00000000, "Initialize Proteus") {
	CPU = ioCPU;
	MEM = ioCPU->GetMemory();
	INT = MEM->GetInterruptManager();
	if (!svcFiber) {
		svcFiber = new TProteusFiber();
		svcFiber->Run(kFiberCallJIT);
	}
	return ioUnit;
}


} // namespace

// ======================================================================= //
//                       - insert insane quote here -                      //
// ======================================================================= //
