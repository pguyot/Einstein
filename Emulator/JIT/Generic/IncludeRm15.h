// ==============================
// File:			IncludeRm15.h
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

// Repeatly include a file, setting Rm from 0 to 14.
// Usage:
// #define INCLUDE_TEMPLATE	"MyFile.h"
// #include "IncludeRm15.h"

#define Rm	0
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	1
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	2
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	3
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	4
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	5
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	6
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	7
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	8
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	9
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	10
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	11
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	12
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	13
#include INCLUDE_TEMPLATE
#undef Rm
#define Rm	14
#include INCLUDE_TEMPLATE
#undef Rm

// ==================================================================== //
// A computer scientist is someone who fixes things that aren't broken. //
// ==================================================================== //
