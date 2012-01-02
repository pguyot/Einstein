// ==============================
// File:			AndroidGlue.h
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

#ifndef EINSTEIN_ANDROID_GLUE_H
#define EINSTEIN_ANDROID_GLUE_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include <errno.h>
#include <jni.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

  extern JNIEXPORT jstring JNICALL Java_com_example_einstein_einstein_stringFromJNI( JNIEnv* env, jobject thiz );
  
  extern JNIEXPORT void JNICALL Java_com_example_einstein_einstein_initEmulator( JNIEnv* env, jobject thiz, jstring logPath );
  extern JNIEXPORT void JNICALL Java_com_example_einstein_einstein_runEmulator( JNIEnv* env, jobject thiz, jstring dataPath, jint screenWidth, jint screenHeight );
  extern JNIEXPORT void JNICALL Java_com_example_einstein_einstein_stopEmulator( JNIEnv* env, jobject thiz );
  extern JNIEXPORT void JNICALL Java_com_example_einstein_einstein_powerOnEmulator( JNIEnv* env, jobject thiz );
  extern JNIEXPORT void JNICALL Java_com_example_einstein_einstein_powerOffEmulator( JNIEnv* env, jobject thiz );

  extern JNIEXPORT void JNICALL Java_com_example_einstein_einstein_toggleNetworkCard( JNIEnv* env, jobject thiz );

  extern JNIEXPORT jint JNICALL Java_com_example_einstein_einstein_screenIsDirty( JNIEnv* env, jobject thiz );
  extern JNIEXPORT void JNICALL Java_com_example_einstein_einstein_setBacklight( JNIEnv* env, jobject thiz, jint v );
  extern JNIEXPORT jint JNICALL Java_com_example_einstein_einstein_backlightIsOn( JNIEnv* env, jobject thiz );
    
  extern JNIEXPORT void JNICALL Java_com_example_einstein_EinsteinView_penDown( JNIEnv* env, jobject thiz, jint x, jint y );
  extern JNIEXPORT void JNICALL Java_com_example_einstein_EinsteinView_penUp( JNIEnv* env, jobject thiz);
  extern JNIEXPORT void JNICALL Java_com_example_einstein_einstein_installNewPackages( JNIEnv* env, jobject thiz );
  
  extern JNIEXPORT jint JNICALL Java_com_example_einstein_EinsteinView_renderEinsteinView(JNIEnv * env, jobject obj, jobject bitmap);
  
#ifdef __cplusplus
}
#endif
    
#endif

// ============================================================================ //
// Beware of the Turing Tar-pit in which everything is possible but nothing of  //
// interest is easy.                                                            //
// ============================================================================ //

