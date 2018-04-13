// ==============================
// File:			CompatibilityWin32.h
// Project:			Einstein
//
// Copyright 2007 by Matthias Melcher (einstein@robowerk.com).
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

#ifndef _COMPATIBILITY_WIN32_H
#define _COMPATIBILITY_WIN32_H

#include <K/Defines/KDefinitions.h>

#define PATH_MAX 1024

extern void bzero(void *dst, size_t n);
extern int snprintf(char *buffer, size_t count, const char *format, ...);
extern KUInt16 *strdupToUTF16(const char *str);

#endif
