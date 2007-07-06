// ==============================
// File:			IncludeRnRdRm.h
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

// Repeatly include a file, setting Rn, Rd and Rm from 0 to 15.
// Usage:
// #define INCLUDE_TEMPLATE	"MyFile.h"
// #include "IncludeRnRdRm.h"

#define Rn	0
#include "IncludeRdRm.h"
#undef Rn
#define Rn	1
#include "IncludeRdRm.h"
#undef Rn
#define Rn	2
#include "IncludeRdRm.h"
#undef Rn
#define Rn	3
#include "IncludeRdRm.h"
#undef Rn
#define Rn	4
#include "IncludeRdRm.h"
#undef Rn
#define Rn	5
#include "IncludeRdRm.h"
#undef Rn
#define Rn	6
#include "IncludeRdRm.h"
#undef Rn
#define Rn	7
#include "IncludeRdRm.h"
#undef Rn
#define Rn	8
#include "IncludeRdRm.h"
#undef Rn
#define Rn	9
#include "IncludeRdRm.h"
#undef Rn
#define Rn	10
#include "IncludeRdRm.h"
#undef Rn
#define Rn	11
#include "IncludeRdRm.h"
#undef Rn
#define Rn	12
#include "IncludeRdRm.h"
#undef Rn
#define Rn	13
#include "IncludeRdRm.h"
#undef Rn
#define Rn	14
#include "IncludeRdRm.h"
#undef Rn
#define Rn	15
#include "IncludeRdRm.h"
#undef Rn

// =================================================== //
// HELP!!!! I'm being held prisoner in /usr/games/lib! //
// =================================================== //
