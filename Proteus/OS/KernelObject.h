// ==============================
// File:			KernelObject.h
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


#ifndef NEWT_OS_KERNEL_OBJECT_H
#define NEWT_OS_KERNEL_OBJECT_H

#include "Proteus.h"


typedef KUInt32 ObjectId;

namespace NewtOS {

	class TObject // : public SingleObject
{
public:
	T_GETSET_MEMBER_W(0x00, ObjectId, 00_Id);
	T_GETSET_MEMBER_P(0x04, TObject*, 04_Next);
	T_GETSET_MEMBER_P(0x08, ObjectId, 08_OwnerId);
	T_GETSET_MEMBER_P(0x0C, ObjectId, 0C_AssignedOwnerId);
};


} // namespace

#endif /* NEWT_OS_KERNEL_OBJECT_H */

// ======================================================================= //
//                       - insert insane quote here -                      //
// ======================================================================= //
