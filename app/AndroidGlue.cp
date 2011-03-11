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
#include <app/TAndroidApp.h>
#include <string.h>

#include <android/bitmap.h>


TAndroidApp *theApp = 0;


/* This is a trivial JNI example where we use a native method
 * to return a new VM String. 
 */
JNIEXPORT jstring JNICALL Java_com_example_einstein_einstein_stringFromJNI( JNIEnv* env, jobject thiz )
{
    LOGI("Testing Android %s. Seems fine so far!", "NDK");
    return env->NewStringUTF("This is the Einstein native interface (5)!");
}


JNIEXPORT void JNICALL Java_com_example_einstein_einstein_initEmulator( JNIEnv* env, jobject thiz )
{
	LOGI("initEmulator: start");
	theApp = new TAndroidApp();
	LOGI("initEmulator: done");
}


JNIEXPORT void JNICALL Java_com_example_einstein_einstein_runEmulator( JNIEnv* env, jobject thiz, jstring dataPath )
{
	jboolean isCopy;
	const char *cDataPath = env->GetStringUTFChars(dataPath, &isCopy);
	LOGI("runEmulator: start (dataPath=%s)", cDataPath);
	theApp->Run(cDataPath);
	LOGI("runEmulator: done");
	env->ReleaseStringUTFChars(dataPath, cDataPath);
}


JNIEXPORT void JNICALL Java_com_example_einstein_EinsteinView_renderEinsteinView(JNIEnv * env, jobject obj, jobject bitmap)
{
    AndroidBitmapInfo  info;
    void              *pixels;
    int                ret;
    static int c = 0;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGB_565 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    unsigned short *p = (unsigned short*)pixels;
    p[c]   = 0xf00f;
    p[c+1] = 0x0ff0;
    c++;
    if (c>4000) c = 0;

    AndroidBitmap_unlockPixels(env, bitmap);
}


// ============================================================================ //
// Beware of the Turing Tar-pit in which everything is possible but nothing of  //
// interest is easy.                                                            //
// ============================================================================ //

