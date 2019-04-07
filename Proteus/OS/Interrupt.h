// ==============================
// File:			Interrupt.h
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


#ifndef NEWT_OS_INTERRUPT_H
#define NEWT_OS_INTERRUPT_H

#include "Proteus.h"


namespace NewtOS {

	struct InterruptObject
	{
		T_GETSET_MEMBER_W(0x00, KUInt32, 00);
		T_GETSET_MEMBER_W(0x04, KSInt32, 04);
		T_GETSET_MEMBER_W(0x08, KUInt32, 08);
		T_GETSET_MEMBER_P(0x0C, InterruptObject*, 0C_Next);
		T_GETSET_MEMBER_P(0x10, void*, 10_Handler);
		T_GETSET_MEMBER_P(0x14, InterruptObject*, 14_Queue);
		T_GETSET_MEMBER_W(0x18, KSInt32, 18);
		T_GETSET_MEMBER_W(0x1C, KUInt16, 1C); // or two shorts?
	};


} // namespace

#endif /* NEWT_OS_INTERRUPT_H */

// ======================================================================= //
//                       - insert insane quote here -                      //
// ======================================================================= //
