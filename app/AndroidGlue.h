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
	
	
	/** Initializes the emulator.
	 * @param logPath can be any path name in the Android file sytem. Special values for logPath are "NULL"
     * or "" for no log, "STDOUT" for logging to some stdout device, and "CONSOLE" for logging to the
     * Android debugging log (LogCat on Eclipse).
	 */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_initEmulator( JNIEnv* env, jobject thiz, jstring logPath );
	
	/** Launches the emulator. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_runEmulator( JNIEnv* env, jobject thiz, jstring dataPath, jint screenWidth, jint screenHeight );
	
	/** Stops the emulator. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_stopEmulator( JNIEnv* env, jobject thiz );
	
	/** Wakes the emulator from sleep. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_powerOnEmulator( JNIEnv* env, jobject thiz );
	
	/** Sends emulator to sleep. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_powerOffEmulator( JNIEnv* env, jobject thiz );
	
	/** Checks if the power is currently on. */
	extern JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_isPowerOn( JNIEnv* env, jobject thiz );
	
	/** Sends a power switch event. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_sendPowerSwitchEvent( JNIEnv* env, jobject thiz );
	
	/** Reboots the emulator. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_rebootEmulator( JNIEnv* env, jobject thiz );
	
	/** Changes the screen size (requires reboot). */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_changeScreenSize( JNIEnv* env, jobject thiz, jint w, jint h );
	
	/** Toggles the network card in and out of the PCMCIA slot. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_toggleNetworkCard( JNIEnv* env, jobject thiz );
	
	/** Checks if the screen contents has changed since the last call
	 * @return 0 if the screen hasn't changed since the last call */
	extern JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_screenIsDirty( JNIEnv* env, jobject thiz );
	
	/** Switches the backlight on or off.
	 * @param v 0 to switch backlight off */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_setBacklight( JNIEnv* env, jobject thiz, jint v );
	
	/** Checks if backlight is on.
	 * @return 0 if the backlight is currently off */
	extern JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_backlightIsOn( JNIEnv* env, jobject thiz );
	
	/** Installs packages that are new in the Einstein directory.
	 * @TODO find a better way to do this */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_installNewPackages( JNIEnv* env, jobject thiz );
	
	/** Installs a package from a file. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_installPackage( JNIEnv* env, jobject thiz, jstring filename );
	
	/** Sets the Newton ID. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_setNewtonID( JNIEnv* env, jobject thiz, jstring );
	
	/** Sends refresh requests */
	extern JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_renderEinsteinView(JNIEnv * env, jobject obj, jobject bitmap);
	
	/** Sends a pen down event. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_penDown( JNIEnv* env, jobject thiz, jint x, jint y );
	
	/** Sends a pen down event. */
	extern JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_penUp( JNIEnv* env, jobject thiz);
	
	
#ifdef __cplusplus
}
#endif

#endif

// ============================================================================ //
// Beware of the Turing Tar-pit in which everything is possible but nothing of  //
// interest is easy.                                                            //
// ============================================================================ //

