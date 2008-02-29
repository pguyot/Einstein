// ==============================
// File:                        math/fixed.h
// Project:                     Albert
//
// Copyright 2003-2008 by Matthias Melcher (albert@matthiasm.com).
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

#ifndef ALBERT_MATH_FIXED_H
#define ALBERT_MATH_FIXED_H

#include <K/Defines/KDefinitions.h>
#include <Albert/sys/types.h>


namespace Albert {


/**
 * Multiply two fixed-point numbers and return the result.
 */
extern Fixed FixedMultiply(Fixed a, Fixed b);


} // namespace


#endif
// ALBERT_MATH_FIXED_H

