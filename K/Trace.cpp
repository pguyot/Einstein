// ==============================
// File:			Trace.h
// Project:			Einstein
//
// Copyright 2021 by Matthias Melcher (einstein@matthiasm.com)
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

#include "Trace.h"

#ifdef _DEBUG


#if TARGET_OS_WIN32

#include <Windows.h>
#include <varargs.h>
#include <stdio.h>
#include <malloc.h>
#include <mutex>
void KTrace(char* format, ...)
{
    static char* textbuffer = nullptr;
    static int textbuffersize = 0;
    static std::mutex mutex;

    mutex.lock();

    if (textbuffersize == 0) {
        textbuffersize = 1023;
        textbuffer = (char*)::malloc(textbuffersize + 1U);
    }

    va_list args;
    va_start(args, format);
    int ret = ::_vsnprintf(textbuffer, textbuffersize, format, args);
    if (ret > textbuffersize) {
        textbuffersize = ret + 1023;
        ::free(textbuffer);
        textbuffer = (char*)::malloc(textbuffersize + 1U);
        ret = ::_vsnprintf(textbuffer, textbuffersize, format, args);
    }
    va_end(args);
    OutputDebugString(textbuffer);

    mutex.unlock();
}

#elif TARGET_OS_LINUX

#include <varargs.h>
void KTrace(char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(f, format, args);
    va_end(args);
}

#elif TARGET_OS_MAC

#include <varargs.h>
void KTrace(char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(f, format, args);
    va_end(args);
}

#else
#error Selected target OS not implemented, or no target OS selected
#endif


#endif

// =================================================== //
