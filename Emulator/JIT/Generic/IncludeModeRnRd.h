// ==============================
// File:			IncludeModeRnRd.h
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
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

// Repeatly include a file, setting FLAG_S to 0 and 1, MODE to (Imm, ImmC, NoShift, Regular) and Rn,Rd from 0 to 15.
// Usage:
// #define INCLUDE_TEMPLATE	"MyFile.h"
// #include "IncludeModeRnRd.h"
// #undef INCLUDE_TEMPLATE

#define FLAG_S	0
#define MODE	Imm
#include "IncludeRnRd.h"
#undef MODE
#define MODE	ImmC
#include "IncludeRnRd.h"
#undef MODE
#define MODE	NoShift
#include "IncludeRnRd.h"
#undef MODE
#define MODE	Regular
#include "IncludeRnRd.h"
#undef MODE
#undef FLAG_S
#define FLAG_S	1
#define MODE	Imm
#include "IncludeRnRd.h"
#undef MODE
#define MODE	ImmC
#include "IncludeRnRd.h"
#undef MODE
#define MODE	NoShift
#include "IncludeRnRd.h"
#undef MODE
#define MODE	Regular
#include "IncludeRnRd.h"
#undef MODE
#undef FLAG_S

// =================================================== //
// HELP!!!! I'm being held prisoner in /usr/games/lib! //
// =================================================== //
