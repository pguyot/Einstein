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
#include <Emulator/Screen/TScreenManager.h>
#include <string.h>
#include <dlfcn.h>

#include <android/bitmap.h>


TAndroidApp *theApp = 0;

/*
 We need to find a way to stop all threads when the Java part of our app 
 vanishes by either getting closed, killed, or uninstalled. Maybe implementing
 these is helpful? It seems not, so we may have to implement a heartbeat :-(
 
 jint JNI_OnLoad(JavaVM* vm, void* reserved);
 void JNI_OnUnload(JavaVM* vm, void* reserved);
 
 also, what do these do?
 
 struct _JavaVM {
	jint AttachCurrentThread(JNIEnv** p_env, void* thr_args)
	...
 }
*/ 

#if 0
/* This function is called very early, as expected */
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LOGE("***** JNI_OnLoad called! ******");
	return JNI_VERSION_1_4;
}

/* this function is not called when deactivating or killing the app! */
void JNI_OnUnload(JavaVM*, void*)
{
	LOGE("***** JNI_OnUnload called! ******");
}
#endif


/*
 We need to call Java functions from C++. This should be the way:
 
 jclass GetObjectClass(jobject obj);
 jmethodID GetMethodID(jclass clazz, const char* name, const char* sig);
	sig is ()V for void fn(vioid) or (II)V for void fn(int, int), etc.
 void CallVoidMethod(jobject obj, jmethodID methodID, ...);
 void CallNonvirtualVoidMethod(jobject obj, jclass clazz, jmethodID methodID, ...)

 */

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
	
	void *mLibHandle = dlopen("libaudio.so", 0);
	if (mLibHandle) {
		// http://source.android.com/porting/audio.html
		LOGE("YAY, libaudio is here!\n");
		void *fn = dlsym(mLibHandle, "createAudioHardware");
		if (fn) {
			LOGE("YAY again: we have the right funciton ************");
		} else {
			LOGE("BOOHOO");
		}
	} else {
		LOGE("dlopen failed on libaudio.so %s\n", dlerror());
	}
	
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


JNIEXPORT void JNICALL Java_com_example_einstein_EinsteinView_penDown( JNIEnv* env, jobject thiz, jint x, jint y )
{
	if (theApp) {
		TScreenManager *tsm = theApp->getScreenManager();
		if (tsm) {
			LOGI("Sending pen down at %d, %d", x, y);
			tsm->PenDown(480-y, x);
		}
	}
}


JNIEXPORT void JNICALL Java_com_example_einstein_EinsteinView_penUp( JNIEnv* env, jobject thiz)
{
	if (theApp) {
		TScreenManager *tsm = theApp->getScreenManager();
		if (tsm) {
			LOGI("Sending pen up");
			tsm->PenUp();
		}
	}
}


/* -- this will be called by a thread in C++, which is probably not a good idea at all.
static jobject		tasmObject = 0;
static jclass		tasmClass = 0;
static jmethodID	tasmMethodInvalidate = 0;

void callScreenInvalidate() 
{
	if (tasmClass) {
		CallVoidMethod(tasmObject, tasmMethodInvalidate);
	}
}
*/

JNIEXPORT jint JNICALL Java_com_example_einstein_EinsteinView_renderEinsteinView(JNIEnv * env, jobject obj, jobject bitmap)
{
/*	
	if (!tasmClass) {
		tasmObject = obj;
		tasmClass = GetObjectClass(obj);
		tasmMethodInvalidate = GetMethodID(tasmClass, "invalidate", "()V");
	}
*/
	
    AndroidBitmapInfo	info;
    void				*pixels;
    int					ret = 0;
    static int			c = 0;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return ret;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGB_565 !");
        return ret;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
		return ret;
    }

    unsigned short *p = (unsigned short*)pixels;
	if (theApp)
	{
		ret = theApp->updateScreen(p);
	}
	
    p[c] = 0xf00f;
    p[(c+160)%320] = 0x0ff0;
    c++;
    if (c>320) c = 0;

    AndroidBitmap_unlockPixels(env, bitmap);
	
	return ret;
}

JNIEXPORT jint JNICALL Java_com_example_einstein_einstein_screenIsDirty( JNIEnv* env, jobject thiz )
{
	int ret = 0;
	if (theApp)
	{
		ret = theApp->screenIsDirty();
	}	
	return ret;
}


// ============================================================================ //
// Beware of the Turing Tar-pit in which everything is possible but nothing of  //
// interest is easy.                                                            //
// ============================================================================ //

