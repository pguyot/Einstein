// ==============================
// File:			IncludeMask.h
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

// Repeatly include a file, setting FIELDS_MASK
// Usage:
// #define INCLUDE_TEMPLATE	"MyFile.h"
// #include "IncludeMask.h"
// #undef INCLUDE_TEMPLATE

#define FIELDS_MASK 0x00000000
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0x000000FF
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0x0000FF00
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0x0000FFFF
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0x00FF0000
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0x00FF00FF
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0x00FFFF00
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0x00FFFFFF
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0xFF000000
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0xFF0000FF
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0xFF00FF00
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0xFF00FFFF
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0xFFFF0000
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0xFFFF00FF
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0xFFFFFF00
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK
#define FIELDS_MASK 0xFFFFFFFF
#include INCLUDE_TEMPLATE
#undef FIELDS_MASK

// =================================================== //
// HELP!!!! I'm being held prisoner in /usr/games/lib! //
// =================================================== //
