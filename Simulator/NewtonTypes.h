// ==============================
// File:			NewtonTypes.h
// Project:			Einstein
//
// Copyright 1999-2012 by Newton Research Group and others
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


#ifndef NEWTON_TYPES_H
#define NEWTON_TYPES_H


#include <K/Defines/KDefinitions.h>


typedef KUInt32 ArrayIndex;
typedef KUInt8 Boolean;
typedef KSInt32 Ref;


class SingleObject {
};


#define ISINT(a) (((a)&3)==0)


#endif	// FAST_RUN_STATE_H

