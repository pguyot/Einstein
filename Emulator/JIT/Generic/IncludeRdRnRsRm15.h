// ==============================
// File:			IncludeRdRnRsRm.h
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

// Repeatly include a file, setting Rd, Rn, Rs and Rm from 0 to 14.
// Usage:
// #define INCLUDE_TEMPLATE	"MyFile.h"
// #include "IncludeRdRnRsRm.h"

#define Rd	0
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	1
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	2
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	3
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	4
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	5
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	6
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	7
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	8
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	9
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	10
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	11
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	12
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	13
#include "IncludeRnRsRm15.h"
#undef Rd
#define Rd	14
#include "IncludeRnRsRm15.h"
#undef Rd

// =================================================== //
// HELP!!!! I'm being held prisoner in /usr/games/lib! //
// =================================================== //
