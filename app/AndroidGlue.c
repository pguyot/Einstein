// ==============================
// File:			AndroidGlue.c
// Project:			Einstein
//
// Copyright 2011 by Matthias Melcher (einstein@matthiasm.com)
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

#include <app/AndroidGlue.h>
#include <string.h>

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. 
 */
jstring
Java_com_example_einstein_einstein_stringFromJNI( JNIEnv* env, jobject thiz )
{
    LOGI("Testing Android %s. Seems fine so far!", "NDK");
    return (*env)->NewStringUTF(env, "This is the Einstein native interface (4)!");
}

// ============================================================================ //
// Beware of the Turing Tar-pit in which everything is possible but nothing of  //
// interest is easy.                                                            //
// ============================================================================ //

