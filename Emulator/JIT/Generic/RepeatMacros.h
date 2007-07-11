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

// Replaced variable argument defines with several fixed arg defines
// to avoid differences in macro expansions with different preprocessors.
//   - Matthias

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
#define repeat_16p2(macro, a)	\
	macro(a, 0)	\
	macro(a, 1)	\
	macro(a, 2)	\
	macro(a, 3)	\
	macro(a, 4)	\
	macro(a, 5)	\
	macro(a, 6)	\
	macro(a, 7)	\
	macro(a, 8)	\
	macro(a, 9)	\
	macro(a, 10)	\
	macro(a, 11)	\
	macro(a, 12)	\
	macro(a, 13)	\
	macro(a, 14)	\
	macro(a, 15)
#define repeat_16p3(macro, a, b)	\
	macro(a, b, 0)	\
	macro(a, b, 1)	\
	macro(a, b, 2)	\
	macro(a, b, 3)	\
	macro(a, b, 4)	\
	macro(a, b, 5)	\
	macro(a, b, 6)	\
	macro(a, b, 7)	\
	macro(a, b, 8)	\
	macro(a, b, 9)	\
	macro(a, b, 10)	\
	macro(a, b, 11)	\
	macro(a, b, 12)	\
	macro(a, b, 13)	\
	macro(a, b, 14)	\
	macro(a, b, 15)
#define repeat_16p4(macro, a, b, c)	\
	macro(a, b, c, 0)	\
	macro(a, b, c, 1)	\
	macro(a, b, c, 2)	\
	macro(a, b, c, 3)	\
	macro(a, b, c, 4)	\
	macro(a, b, c, 5)	\
	macro(a, b, c, 6)	\
	macro(a, b, c, 7)	\
	macro(a, b, c, 8)	\
	macro(a, b, c, 9)	\
	macro(a, b, c, 10)	\
	macro(a, b, c, 11)	\
	macro(a, b, c, 12)	\
	macro(a, b, c, 13)	\
	macro(a, b, c, 14)	\
	macro(a, b, c, 15)
#define repeat_16p5(macro, a, b, c, d)	\
	macro(a, b, c, d, 0)	\
	macro(a, b, c, d, 1)	\
	macro(a, b, c, d, 2)	\
	macro(a, b, c, d, 3)	\
	macro(a, b, c, d, 4)	\
	macro(a, b, c, d, 5)	\
	macro(a, b, c, d, 6)	\
	macro(a, b, c, d, 7)	\
	macro(a, b, c, d, 8)	\
	macro(a, b, c, d, 9)	\
	macro(a, b, c, d, 10)	\
	macro(a, b, c, d, 11)	\
	macro(a, b, c, d, 12)	\
	macro(a, b, c, d, 13)	\
	macro(a, b, c, d, 14)	\
	macro(a, b, c, d, 15)

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
/*
#define repeat_256p(macro, ...)	\
	xrepeat_16p(repeat_16p, macro, __VA_ARGS__)
*/
#define repeat_256p4(macro, a, b, c)	\
	repeat_16p5(macro, a, b, c, 0)	\
	repeat_16p5(macro, a, b, c, 1)	\
	repeat_16p5(macro, a, b, c, 2)	\
	repeat_16p5(macro, a, b, c, 3)	\
	repeat_16p5(macro, a, b, c, 4)	\
	repeat_16p5(macro, a, b, c, 5)	\
	repeat_16p5(macro, a, b, c, 6)	\
	repeat_16p5(macro, a, b, c, 7)	\
	repeat_16p5(macro, a, b, c, 8)	\
	repeat_16p5(macro, a, b, c, 9)	\
	repeat_16p5(macro, a, b, c, 10)	\
	repeat_16p5(macro, a, b, c, 11)	\
	repeat_16p5(macro, a, b, c, 12)	\
	repeat_16p5(macro, a, b, c, 13)	\
	repeat_16p5(macro, a, b, c, 14)	\
	repeat_16p5(macro, a, b, c, 15)

#endif
		// _REPEATMACROS_H

// =================================================== //
// HELP!!!! I'm being held prisoner in /usr/games/lib! //
// =================================================== //
