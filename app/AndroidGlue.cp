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
#include <Emulator/Sound/TAndroidSoundManager.h>
#include <Platform/TPlatformManager.h>
#include <Log/TFileLog.h>
#include <Log/TStdOutLog.h>
#include <string.h>
#include <dlfcn.h>

#include <android/bitmap.h>


const jint kRequiredActionsRefreshScreen	= 0x01;
const jint kRequiredActionsStartSound		= 0x02;
const jint kRequiredActionsStopSound		= 0x04;
const jint kRequiredActionsSetVolume		= 0x08;


TAndroidApp *theApp = 0;
TLog *theLog = 0;


class TAndroidLog : public TLog
{
public:
	TAndroidLog() { }
protected:
	virtual void DoLogLine(const char* inLine) {
		__android_log_print(ANDROID_LOG_WARN, "NewtonEmulator", "%s", inLine);
	}
};


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
JNIEXPORT jstring JNICALL Java_com_newtonforever_einstein_jni_Native_stringFromJNI( JNIEnv* env, jobject thiz )
{
    if (theLog) theLog->FLogLine("Testing Android %s. Seems fine so far!", "NDK");
    return env->NewStringUTF("This is the Einstein native interface (5)!");
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_initEmulator( JNIEnv* env, jobject thiz, jstring logPath )
{
	jboolean isCopy;
	
	const char *cLogPath = env->GetStringUTFChars(logPath, &isCopy);
	if (cLogPath && *cLogPath) {
		if (strcmp(cLogPath, "STDOUT")==0) {
			theLog = new TStdOutLog();
		} else if (strcmp(cLogPath, "CONSOLE")==0) {
			theLog = new TAndroidLog();
		} else if (strcmp(cLogPath, "NULL")==0) {
			theLog = 0L;
		} else {
			theLog = new TFileLog(cLogPath);
		}
	} else {
		theLog = 0L;
	}
	
	if (theLog) theLog->LogLine("initEmulator: start");
	theApp = new TAndroidApp();
	if (theLog) theLog->LogLine("initEmulator: done");
	
	env->ReleaseStringUTFChars(logPath, cLogPath);
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_runEmulator( JNIEnv* env, jobject thiz, jstring dataPath, jint screenWidth, jint screenHeight )
{
	jboolean isCopy;
	const char *cDataPath = env->GetStringUTFChars(dataPath, &isCopy);
	if (theLog) theLog->FLogLine("runEmulator: start (dataPath=%s)", cDataPath);
	theApp->Run(cDataPath, screenWidth, screenHeight, theLog);
	env->ReleaseStringUTFChars(dataPath, cDataPath);
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_stopEmulator( JNIEnv* env, jobject thiz )
{
	if (theLog) theLog->LogLine("stopEmulator");
	if (theApp) theApp->Stop();
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_powerOnEmulator( JNIEnv* env, jobject thiz )
{
	if (theLog) theLog->LogLine("powerOnEmulator");
	if (theApp) theApp->PowerOn();
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_powerOffEmulator( JNIEnv* env, jobject thiz )
{
	if (theLog) theLog->LogLine("powerOffEmulator");
	if (theApp) theApp->PowerOff();
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_sendPowerSwitchEvent( JNIEnv* env, jobject thiz )
{
	if (theLog) theLog->LogLine("powerEvent");
	if (theApp) {
		theApp->getPlatformManager()->SendPowerSwitchEvent();
	}
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_rebootEmulator( JNIEnv* env, jobject thiz )
{
	if (theLog) theLog->LogLine("reboot");
	if (theApp) theApp->reboot();
}

JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_changeScreenSize( JNIEnv* env, jobject thiz, jint w, jint h )
{
	if (theLog) theLog->LogLine("changeScreenSize");
	if (theApp) theApp->ChangeScreenSize(w, h);
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_penDown( JNIEnv* env, jobject thiz, jint x, jint y )
{
	if (theApp) {
		TScreenManager *tsm = theApp->getScreenManager();
		if (tsm) {
			if (!theApp->getPlatformManager()->IsPowerOn()) {
				// After five minutes, the MP switches itself off. On Android,
				// the host OS should decide when to put the device to sleep.
				// Newton OS nevertheless switches itself off. To continue work,
				// we'd have to pull the power switch. Since we have no power switch 
				// on Einstein/IOS, any screen touch will power the Newton back on.
				theApp->PowerOn();
				//theApp->getPlatformManager()->SendPowerSwitchEvent();
			}
			//LOGI("Sending pen down at %d, %d (%d)", x, y, theApp->getScreenManager()->GetActualScreenHeight());
			tsm->PenDown(theApp->getScreenManager()->GetActualScreenHeight()-y, x);
		}
	}
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_penUp( JNIEnv* env, jobject thiz)
{
	if (theApp) {
		TScreenManager *tsm = theApp->getScreenManager();
		if (tsm) {
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

JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_renderEinsteinView(JNIEnv * env, jobject obj, jobject bitmap)
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
        if (theLog) theLog->FLogLine("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return ret;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        if (theLog) theLog->FLogLine("Bitmap format is not RGB_565 !");
        return ret;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        if (theLog) theLog->FLogLine("AndroidBitmap_lockPixels() failed ! error=%d", ret);
		return ret;
    }

    unsigned short *p = (unsigned short*)pixels;
	if (theApp)
	{
		ret = theApp->updateScreen(p);
	}
	
    AndroidBitmap_unlockPixels(env, bitmap);
	
	return ret;
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_toggleNetworkCard( JNIEnv* env, jobject thiz )
{
	if (theLog) theLog->FLogLine("Toggle Network Card");
	if (theApp) {
		if (theApp->getPlatformManager()) {
			theApp->getPlatformManager()->SendNetworkCardEvent();
		}
	}
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_setNewtonID( JNIEnv* env, jobject thiz, jstring jID )
{
	if (theApp) {
		jboolean isCopy;
		KUInt32 id0, id1;
		const char *cID = env->GetStringUTFChars(jID, &isCopy);
		sscanf(cID, "%08x%08x", &id0, &id1);
		theApp->SetNewtonID(id0, id1);
		env->ReleaseStringUTFChars(jID, cID);
	}
}


//JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_screenIsDirty( JNIEnv* env, jobject thiz )
//{
//	int ret = 0;
//	if (theApp)
//	{
//		ret = theApp->screenIsDirty();
//	}	
//	return ret;
//}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_setBacklight( JNIEnv* env, jobject thiz, jint v )
{
	if (theApp) {
		TScreenManager *tsm = theApp->getScreenManager();
		if (tsm) {
			tsm->SetBacklight(v);
		}
	}
}


JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_backlightIsOn( JNIEnv* env, jobject thiz )
{
	if (theApp) {
		TScreenManager *tsm = theApp->getScreenManager();
		if (tsm) {
			return tsm->GetBacklight() ? 1 : 0;
		}
	}
	return -1;
}

JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_installNewPackages( JNIEnv* env, jobject thiz )
{
	if (theApp) {
		theApp->getPlatformManager()->InstallNewPackages();
	}
}

JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_installPackage( JNIEnv* env, jobject thiz, jstring filename )
{
	if (theApp) {
		jboolean isCopy;
		const char *cFilename = env->GetStringUTFChars(filename, &isCopy);
		if (theLog)
			theLog->FLogLine("Request to install package %s\n", cFilename);
		theApp->getPlatformManager()->InstallPackage(cFilename);
		env->ReleaseStringUTFChars(filename, cFilename);
	}
}

JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_isPowerOn( JNIEnv* env, jobject thiz )
{
	if (theApp) {
		return theApp->IsPowerOn();
	}
}


JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_getRequiredActions( JNIEnv* env, jobject thiz )
{
	jint requiredActions = 0;
	if (theApp)
	{
		if (theApp->screenIsDirty()) {
			requiredActions |= kRequiredActionsRefreshScreen;
		}
		if (TAndroidSoundManager::soundOutputDataAvailable()) {
			requiredActions |= kRequiredActionsStartSound;
		}
		if (TAndroidSoundManager::soundVolumeChanged()) {
			requiredActions |= kRequiredActionsSetVolume;
		}
	}
	return requiredActions;
}


JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_getSoundBufferSize( JNIEnv* env, jobject thiz )
{
	return TAndroidSoundManager::soundOutputBytesAvailable();
}


JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_fillSoundBuffer( JNIEnv* env, jobject thiz, jshortArray soundBuffer)
{
	jsize len = env->GetArrayLength(soundBuffer);
    jshort *values = env->GetShortArrayElements(soundBuffer, NULL);
	return TAndroidSoundManager::soundOutputBytesCopy(values, 2*len);
}


JNIEXPORT jint JNICALL Java_com_newtonforever_einstein_jni_Native_getSoundVolume( JNIEnv* env, jobject thiz )
{
	return TAndroidSoundManager::getGlobalVolume();
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_soundBufferAvailable( JNIEnv* env, jobject thiz )
{
}


JNIEXPORT void JNICALL Java_com_newtonforever_einstein_jni_Native_soundBufferFinishedOrCanceled( JNIEnv* env, jobject thiz )
{
}

// ============================================================================ //
// Beware of the Turing Tar-pit in which everything is possible but nothing of  //
// interest is easy.                                                            //
// ============================================================================ //

