// ==============================
// File:			RepeatMacros.h
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

#ifndef _REPEATMACROS_H
#define _REPEATMACROS_H

// Define macros to repeat another macros with variable arguments.
// This is meant to multiply the instruction emulation functions.

// Usage:
// #define foo(x) ...
// ...
// repeat_16(foo)
// -> foo(0) foo(1) foo(2) ...
#define repeat_16(macro)	\
	macro(0)	\
	macro(1)	\
	macro(2)	\
	macro(3)	\
	macro(4)	\
	macro(5)	\
	macro(6)	\
	macro(7)	\
	macro(8)	\
	macro(9)	\
	macro(10)	\
	macro(11)	\
	macro(12)	\
	macro(13)	\
	macro(14)	\
	macro(15)

// Usage:
// #define foo(x, y) ...
// ...
// repeat_16p(foo, "42", 43.0)
// -> foo("42", 43.0, 0) foo("42", 43.0, 1) foo("42", 43.0, 2) ...
#define repeat_16p(macro, ...)	\
	macro(__VA_ARGS__, 0)	\
	macro(__VA_ARGS__, 1)	\
	macro(__VA_ARGS__, 2)	\
	macro(__VA_ARGS__, 3)	\
	macro(__VA_ARGS__, 4)	\
	macro(__VA_ARGS__, 5)	\
	macro(__VA_ARGS__, 6)	\
	macro(__VA_ARGS__, 7)	\
	macro(__VA_ARGS__, 8)	\
	macro(__VA_ARGS__, 9)	\
	macro(__VA_ARGS__, 10)	\
	macro(__VA_ARGS__, 11)	\
	macro(__VA_ARGS__, 12)	\
	macro(__VA_ARGS__, 13)	\
	macro(__VA_ARGS__, 14)	\
	macro(__VA_ARGS__, 15)
#define xrepeat_16p(macro, ...)	\
	macro(__VA_ARGS__, 0)	\
	macro(__VA_ARGS__, 1)	\
	macro(__VA_ARGS__, 2)	\
	macro(__VA_ARGS__, 3)	\
	macro(__VA_ARGS__, 4)	\
	macro(__VA_ARGS__, 5)	\
	macro(__VA_ARGS__, 6)	\
	macro(__VA_ARGS__, 7)	\
	macro(__VA_ARGS__, 8)	\
	macro(__VA_ARGS__, 9)	\
	macro(__VA_ARGS__, 10)	\
	macro(__VA_ARGS__, 11)	\
	macro(__VA_ARGS__, 12)	\
	macro(__VA_ARGS__, 13)	\
	macro(__VA_ARGS__, 14)	\
	macro(__VA_ARGS__, 15)

// Usage:
// #define foo(x, y)
// ...
// repeat_256(foo)
// -> foo(0, 0) foo(0, 1) foo(0, 2) ... foo(1, 0) ... foo(15, 15)
#define repeat_256(macro)	\
	xrepeat_16p(repeat_16p, macro)

// Usage:
// #define foo(x, y, z) ...
// ...
// repeat_256p(foo, "42", 43.0)
// -> foo("42", 43.0, 0, 0) foo("42", 43.0, 0, 1) ...
#define repeat_256p(macro, ...)	\
	xrepeat_16p(repeat_16p, macro, __VA_ARGS__)

#endif
		// _REPEATMACROS_H

// =================================================== //
// HELP!!!! I'm being held prisoner in /usr/games/lib! //
// =================================================== //
