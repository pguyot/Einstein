// ==============================
// File:			TAndroidNativeApp.cp
// Project:			Einstein
//
// Copyright 2020 by Matthias Melcher (einstein@matthiasm.com).
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

#include <K/Defines/KDefinitions.h>
#include "TAndroidNativeApp.h"

// ANSI C & POSIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/system_properties.h>

// Java Native Interface
#include <jni.h>
#include <android/log.h>

// Einstein
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Network/TUsermodeNetwork.h"
#include "Emulator/Sound/TAndroidNativeSoundManager.h"
#include "Emulator/Screen/TAndroidNativeScreenManager.h"
#include "Emulator/Serial/TTcpClientSerialPortManager.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Log/TLog.h"
#include "app/json11.hpp"


// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

static const char *LOG_TAG = "Einstein";

// -------------------------------------------------------------------------- //
// Variable intialisation
// -------------------------------------------------------------------------- //

KUInt32 TAndroidNativeCore::pScreenTopPadding = 25;
KUInt32 TAndroidNativeCore::pScreenBottomPadding = 35;
KUInt32 TAndroidNativeCore::pScreenWidth = 320;
KUInt32 TAndroidNativeCore::pScreenHeight = 480;

ARect TAndroidNativeCore::pDirtyRect[] = { 0 };
bool TAndroidNativeCore::pScreenIsDirty = false;
TMutex TAndroidNativeCore::pScreenMutex;

// The ANativeActivity object instance that this app is running in.
ANativeActivity *TAndroidNativeCore::pActivity = 0L;

// The current configuration the app is running in.
AConfiguration* TAndroidNativeCore::pConfig = 0L;

// This is the last instance's saved state, as provided at creation time.
// It is NULL if there was no state.  You can use this as you need; the
// memory will remain around until you call android_app_exec_cmd() for
// APP_CMD_RESUME, at which point it will be freed and savedState set to NULL.
// These variables should only be changed when processing a APP_CMD_SAVE_STATE,
// at which point they will be initialized to NULL and you can malloc your
// state and place the information here.  In that case the memory will be
// freed for you later.
void* TAndroidNativeCore::pSavedState = 0;
size_t TAndroidNativeCore::pSavedStateSize = 0;

// The ALooper associated with the app's thread.
ALooper* TAndroidNativeCore::pAppLooper = 0;

// When non-NULL, this is the input queue from which the app will
// receive user input events.
AInputQueue* TAndroidNativeCore::pInputQueue = 0;

// When non-NULL, this is the window surface that the app can draw in.
ANativeWindow* TAndroidNativeCore::pNativeWindow = 0;

// Use this buffer for direct drawing access
ANativeWindow_Buffer TAndroidNativeCore::pNativeWindowBuffer = { 0 };
ANativeWindow_Buffer TAndroidNativeCore::pApplicationWindowBuffer = { 0 };

// Current state of the app's activity.  May be either APP_CMD_START,
// APP_CMD_RESUME, APP_CMD_PAUSE, or APP_CMD_STOP; see below.
int TAndroidNativeCore::pActivityState = 0;

// This is non-zero when the application's NativeActivity is being
// destroyed and waiting for the app thread to complete.
int TAndroidNativeCore::pDestroyRequested = 0;

pthread_mutex_t TAndroidNativeCore::pMutex = { 0 };
pthread_cond_t TAndroidNativeCore::pCond = { 0 };
int TAndroidNativeCore::pMsgReadPipe = 0;
int TAndroidNativeCore::pMsgWritePipe = 0;
pthread_t TAndroidNativeCore::pThread = 0;
int TAndroidNativeCore::pRunning = 0;
int TAndroidNativeCore::pStateSaved = 0;
int TAndroidNativeCore::pDestroyed = 0;
//int TAndroidNativeCore::pRedrawNeeded = 0;
AInputQueue *TAndroidNativeCore::pPendingInputQueue = 0;
ANativeWindow *TAndroidNativeCore::pPendingWindow = 0;
//ARect TAndroidNativeCore::pPendingContentRect = { 0 };

int TAndroidNativeCore::pTimerReadPipe = -1;
int TAndroidNativeCore::pTimerWritePipe = -1;

int TAndroidNativeCore::pHostID = HOST_ID_UNKNOWN;

TAndroidNativeApp *TAndroidNativeActivity::einstein = nullptr;
TLog *TAndroidNativeActivity::theLog = nullptr;

// -------------------------------------------------------------------------- //
//  * TAndroidLog
// -------------------------------------------------------------------------- //

class TAndroidLog : public TLog
{
public:
    TAndroidLog() { }
protected:
    virtual void DoLogLine(const char* inLine) {
        __android_log_print(ANDROID_LOG_WARN, "NewtonEmulator", "%s", inLine);
    }
};

// -------------------------------------------------------------------------- //
//  * TAndroidNativeApp( void )
// -------------------------------------------------------------------------- //
TAndroidNativeApp::TAndroidNativeApp( void )
        :
        mProgramName( nil ),
        mROMImage( nil ),
        mEmulator( nil ),
        mSoundManager( nil ),
        mScreenManager( nil ),
        mPlatformManager( nil ),
        mLog( nil ),
        mNetworkManager( nil ),
        mQuit(false),
        mNewtonID0(0x00004E65),
        mNewtonID1(0x77746F6E)
{
}

// -------------------------------------------------------------------------- //
//  * ~TAndroidNativeApp( void )
// -------------------------------------------------------------------------- //
TAndroidNativeApp::~TAndroidNativeApp( void )
{
    if (mEmulator)
    {
        delete mEmulator;
    }
    if (mScreenManager)
    {
        delete mScreenManager;
    }
    if (mSoundManager)
    {
        delete mSoundManager;
    }
    if (mLog)
    {
        delete mLog;
    }
    if (mROMImage)
    {
        delete mROMImage;
    }
    if (mNetworkManager)
    {
        delete mNetworkManager;
    }
}


// -------------------------------------------------------------------------- //
// Run( int, char** )
// -------------------------------------------------------------------------- //
void
TAndroidNativeApp::Run(const char *dataPath, int newtonScreenWidth, int newtonScreenHeight, TLog *inLog)
{
    mProgramName = "Einstein";
    mROMImage = NULL;
    mEmulator = NULL;
    mSoundManager = NULL;
    mScreenManager = NULL;
    mPlatformManager = NULL;
    mLog = nullptr; // this is a quite detailed log, so keep it NULL unless you are debugging

    if (inLog) inLog->LogLine("Loading assets...");

    if (inLog) inLog->LogLine("  mLog:");
    // The log slows down the emulator and may cause a deadlock when running
    // the Network card emulation. Only activate if you really need it!
    // CAUTION: the destructor will delete our mLog. That is not good! Avoid!
    //if (inLog) mLog = inLog;
    if (inLog) inLog->FLogLine("    OK: 0x%08x", (intptr_t)mLog);

    char theROMPath[1024];
    snprintf(theROMPath, 1024, "%s/717006.rom", dataPath);
    if (mLog) mLog->FLogLine("  ROM exists at %s?", theROMPath);
    if (access(theROMPath, R_OK)==-1) {
        char buf[2048];
        if (errno==EACCES) {
            snprintf(buf, 2048,
                    "Einstein needs permission to read and write external storage.\n\n"
                    "Please activate \"Storage\" in the Einstein app permissions panel "
                    "of the settings dialog.");
            // FIXME: find the code that allows permission requests on the fly
            TAndroidNativeActivity::alert(buf);
        } else {
            snprintf(buf, 2048,
                    "Can't find ROM file at \"%s\". The ROM files is not part "
                    "of EInstein and needs to be installed separately.",
                    theROMPath);
            TAndroidNativeActivity::alert(buf);
        }
        if (mLog) mLog->FLogLine("Can't read ROM file %s", theROMPath);
        return;
    }
    if (mLog) mLog->FLogLine("    OK");

    char theREXPath[1024];
    snprintf(theREXPath, 1024, "%s/Einstein.rex", dataPath);
    if (mLog) mLog->FLogLine("  REX exists at %s?", theREXPath);
    if (access(theREXPath, R_OK)==-1) {
        char buf[2048];
        snprintf(buf, 2048,
                 "Can't find REX file at \"%s\". The ROM files is not part "
                 "of EInstein and needs to be installed separately.",
                 theROMPath);
        TAndroidNativeActivity::alert(buf);
        if (mLog) mLog->FLogLine("Can't read ROM file %s", theROMPath);
        return;
        if (mLog) mLog->FLogLine("Can't read REX file %s", theREXPath);
        return;
    }
    if (mLog) mLog->FLogLine("    OK");

    char theImagePath[1024];
    snprintf(theImagePath, 1024, "%s/717006.img", dataPath);

    char theFlashPath[1024];
    snprintf(theFlashPath, 1024, "%s/flash", dataPath);

    if (mLog) mLog->FLogLine("  mROMImage:");
    mROMImage = new TFlatROMImageWithREX(theROMPath, theREXPath, theImagePath);
    if (mLog) mLog->FLogLine("    OK: 0x%08x", (intptr_t)mROMImage);

    if (mLog) mLog->FLogLine("  mSoundManager:");
    mSoundManager = new TAndroidNativeSoundManager(mLog);
    if (mLog) mLog->FLogLine("    OK: 0x%08x", (intptr_t)mSoundManager);

    bool isLandscape = false;
    if (mLog) mLog->FLogLine("  mScreenManager");
    mScreenManager = new TAndroidNativeScreenManager(mLog,
                                                     (KUInt32)newtonScreenWidth, (KUInt32)newtonScreenHeight,
                                                     true, // fullscreen
                                                     isLandscape);
    if (mLog) mLog->FLogLine("    OK: 0x%08x", (intptr_t)mScreenManager);

    if (mLog) mLog->FLogLine("  mNetworkManager:");
    mNetworkManager = new TUsermodeNetwork(mLog);
    if (mLog) mLog->FLogLine("    OK: 0x%08x", (intptr_t)mNetworkManager);

    if (mLog) mLog->FLogLine("  mEmulator:");
    mEmulator = new TEmulator(
            mLog,
            mROMImage,
            theFlashPath,
            mSoundManager,
            mScreenManager,
            mNetworkManager,
            0x40 << 16
            );

    if (mLog) mLog->FLogLine("    OK: 0x%08x", (intptr_t)mEmulator);
    mEmulator->SetNewtonID(mNewtonID0, mNewtonID1);

    mPlatformManager = mEmulator->GetPlatformManager();
    mPlatformManager->SetDocDir(dataPath);

        mEmulator->SerialPorts.Initialize(TSerialPorts::kTcpClientDriver,
                                      TSerialPorts::kNullDriver,
                                      TSerialPorts::kNullDriver,
                                      TSerialPorts::kNullDriver );

    // Create the Overlay text window
    mScreenManager->OverlayClear();
    mScreenManager->OverlayOn();
    mScreenManager->OverlayPrintAt(0, 0, "Booting...", true);
    mScreenManager->OverlayPrintProgress(1, 0);
    mScreenManager->OverlayFlush();

    if (mLog) mLog->FLogLine("Creating helper thread.");
    pthread_t theThread;
    int theErr = ::pthread_create( &theThread, NULL, SThreadEntry, this );
    if (theErr) {
        if (mLog) mLog->FLogLine( "Error with pthread_create (%i)\n", theErr );
        ::exit(2);
    }
    if (mLog) mLog->FLogLine("Booting NewtonOS...");
}


// -------------------------------------------------------------------------- //
// Quit the Main tread
// -------------------------------------------------------------------------- //
void
TAndroidNativeApp::Stop( void )
{
    mEmulator->Stop();
}


// -------------------------------------------------------------------------- //
// Wake up Emulator
// -------------------------------------------------------------------------- //
void
TAndroidNativeApp::PowerOn( void )
{
#if 0
    mPlatformManager->PowerOn();
#else
    if (!mPlatformManager->IsPowerOn())
        mPlatformManager->SendPowerSwitchEvent();
#endif
}


// -------------------------------------------------------------------------- //
// Send Emulator to Sleep
// -------------------------------------------------------------------------- //
void
TAndroidNativeApp::PowerOff( void )
{
#if 0
    mPlatformManager->PowerOff();
#else
    if (mPlatformManager->IsPowerOn())
        mPlatformManager->SendPowerSwitchEvent();
#endif
}


void TAndroidNativeApp::reboot()
{
    TARMProcessor *cpu = mEmulator->GetProcessor();
    cpu->ResetInterrupt();
}

int TAndroidNativeApp::IsPowerOn()
{
    return mPlatformManager->IsPowerOn();
}

void TAndroidNativeApp::ChangeScreenSize(int w, int h)
{
    mScreenManager->ChangeScreenSize(w, h);
}

// -------------------------------------------------------------------------- //
// ThreadEntry( void )
// -------------------------------------------------------------------------- //
void
TAndroidNativeApp::ThreadEntry( void )
{
    mEmulator->Run();
    mQuit = true;
}


int TAndroidNativeApp::updateScreen(unsigned short *buffer, const ARect &r)
{
    int ret = 0;
    if (mScreenManager) {
        TAndroidNativeScreenManager *tasm = (TAndroidNativeScreenManager*)mScreenManager;
        ret = tasm->update(buffer, r);
    }
    return ret;
}

// ---- TAndroidNativeCore -------------------------------------------------------------------------


void TAndroidNativeCore::log_e(const char *text, ...)
{
    va_list args;
    va_start (args, text);
    __android_log_vprint(ANDROID_LOG_ERROR, LOG_TAG, text, args);
    va_end (args);
}


void TAndroidNativeCore::log_w(const char *text, ...)
{
    va_list args;
    va_start (args, text);
    __android_log_vprint(ANDROID_LOG_WARN, LOG_TAG, text, args);
    va_end (args);
}


void TAndroidNativeCore::log_i(const char *text, ...)
{
    va_list args;
    va_start (args, text);
    __android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, text, args);
    va_end (args);
}


void TAndroidNativeCore::log_v(const char *text, ...)
{
#ifdef _DEBUG
    va_list args;
    va_start (args, text);
    __android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, text, args);
    va_end (args);
#else
    text = 0;
#endif
}


void TAndroidNativeCore::free_saved_state()
{
    pthread_mutex_lock(&pMutex);
    if (pSavedState != NULL) {
        free(pSavedState);
        pSavedState = NULL;
        pSavedStateSize = 0;
    }
    pthread_mutex_unlock(&pMutex);
}

/**
 Call when ALooper_pollAll() returns LOOPER_ID_MAIN, reading the next
 app command message.
 */
int8_t TAndroidNativeCore::read_cmd()
{
    int8_t cmd;
    if (read(pMsgReadPipe, &cmd, sizeof(cmd)) == sizeof(cmd)) {
        switch (cmd) {
            case APP_CMD_SAVE_STATE:
                free_saved_state();
                break;
            default:
                break;
        }
        return cmd;
    } else {
        log_e("No data on command pipe!");
    }
    return -1;
}


void TAndroidNativeCore::print_cur_config()
{
    char lang[2], country[2];
    AConfiguration_getLanguage(pConfig, lang);
    AConfiguration_getCountry(pConfig, country);

    log_v("Config: mcc=%d mnc=%d lang=%c%c cnt=%c%c orien=%d touch=%d dens=%d "
          "keys=%d nav=%d keysHid=%d navHid=%d sdk=%d size=%d long=%d "
          "modetype=%d modenight=%d",
          AConfiguration_getMcc(pConfig),
          AConfiguration_getMnc(pConfig),
          lang[0], lang[1], country[0], country[1],
          AConfiguration_getOrientation(pConfig),
          AConfiguration_getTouchscreen(pConfig),
          AConfiguration_getDensity(pConfig),
          AConfiguration_getKeyboard(pConfig),
          AConfiguration_getNavigation(pConfig),
          AConfiguration_getKeysHidden(pConfig),
          AConfiguration_getNavHidden(pConfig),
          AConfiguration_getSdkVersion(pConfig),
          AConfiguration_getScreenSize(pConfig),
          AConfiguration_getScreenLong(pConfig),
          AConfiguration_getUiModeType(pConfig),
          AConfiguration_getUiModeNight(pConfig));
}

/**
 Call with the command returned by android_app_read_cmd() to do the
 initial pre-processing of the given command.  You can perform your own
 actions for the command after calling this function.
 */
void TAndroidNativeCore::pre_exec_cmd(int8_t cmd)
{
    switch (cmd) {
        case APP_CMD_INPUT_CHANGED:
            log_v("APP_CMD_INPUT_CHANGED\n");
            pthread_mutex_lock(&pMutex);
            if (pInputQueue != NULL) {
                AInputQueue_detachLooper(pInputQueue);
            }
            pInputQueue = pPendingInputQueue;
            if (pInputQueue != NULL) {
                log_v("Attaching input queue to looper");
                AInputQueue_attachLooper(pInputQueue, pAppLooper, LOOPER_ID_INPUT, NULL, NULL);
            }
            pthread_cond_broadcast(&pCond);
            pthread_mutex_unlock(&pMutex);
            break;

        case APP_CMD_INIT_WINDOW:
            log_v("APP_CMD_INIT_WINDOW\n");
            // tell the main thread that we received the window handle
            pthread_mutex_lock(&pMutex);
            pNativeWindow = pPendingWindow;
            pthread_cond_broadcast(&pCond);
            pthread_mutex_unlock(&pMutex);

            // The following line makes sure that the toolbars are drawn and receive events, however
            // Einstein will appear "under" those toolbars. We need to find the height of the top
            // and bottom toolbar and avoid drawing into them (or keeping them at a matching
            // background color)
            //ANativeActivity_setWindowFlags(get_activity(), AWINDOW_FLAG_FORCE_NOT_FULLSCREEN, 0);
            //ANativeActivity_setWindowFlags(get_activity(), AWINDOW_FLAG_FULLSCREEN, 0);
            ANativeWindow_setBuffersGeometry(pNativeWindow,
                                             (int32_t)pScreenWidth,
                                             (int32_t)pScreenHeight+pScreenTopPadding+pScreenBottomPadding,
                                             WINDOW_FORMAT_RGB_565);
            break;

        case APP_CMD_TERM_WINDOW:
            log_v("APP_CMD_TERM_WINDOW\n");
            pthread_cond_broadcast(&pCond);
            break;

        case APP_CMD_RESUME:
        case APP_CMD_START:
        case APP_CMD_PAUSE:
        case APP_CMD_STOP:
            log_v("activityState=%d\n", cmd);
            pthread_mutex_lock(&pMutex);
            pActivityState = cmd;
            pthread_cond_broadcast(&pCond);
            pthread_mutex_unlock(&pMutex);
            break;

        case APP_CMD_CONFIG_CHANGED:
            log_v("APP_CMD_CONFIG_CHANGED\n");
            AConfiguration_fromAssetManager(pConfig, pActivity->assetManager);
            print_cur_config();
            break;

        case APP_CMD_DESTROY:
            log_v("APP_CMD_DESTROY\n");
            pDestroyRequested = 1;
            // TODO: Einstein should quit now
            break;

        default:
            break;
    }
}

/**
 Call with the command returned by read_cmd() to do the
 final post-processing of the given command.  You must have done your own
 actions for the command before calling this function.
 */
void TAndroidNativeCore::post_exec_cmd(int8_t cmd)
{
    switch (cmd) {
        case APP_CMD_TERM_WINDOW:
            log_v("APP_CMD_TERM_WINDOW\n");
            pthread_mutex_lock(&pMutex);
            pNativeWindow = NULL;
            pthread_cond_broadcast(&pCond);
            pthread_mutex_unlock(&pMutex);
            break;

        case APP_CMD_SAVE_STATE:
            log_v("APP_CMD_SAVE_STATE\n");
            pthread_mutex_lock(&pMutex);
            pStateSaved = 1;
            pthread_cond_broadcast(&pCond);
            pthread_mutex_unlock(&pMutex);
            break;

        case APP_CMD_RESUME:
            free_saved_state();
            break;
    }
}


void TAndroidNativeCore::create_timer_handler()
{
    int msgpipe[2];
    if (::pipe(msgpipe)) {
        log_e("could not create timer pipe: %s", strerror(errno));
        return;
    }
    pTimerReadPipe = msgpipe[0];
    pTimerWritePipe = msgpipe[1];
    ALooper_addFd(pAppLooper, pTimerReadPipe, LOOPER_ID_TIMER, ALOOPER_EVENT_INPUT, NULL, NULL);
}


void TAndroidNativeCore::destroy_timer_handler()
{
    ALooper_removeFd(pAppLooper, pTimerReadPipe);
    ::close(pTimerWritePipe);
    ::close(pTimerReadPipe);
}


void TAndroidNativeCore::send_timer_index(uint8_t ix)
{
    if (pTimerWritePipe!=-1)
        ::write(pTimerWritePipe, &ix, 1);
}


uint8_t TAndroidNativeCore::receive_timer_index()
{
    uint8_t ix = 0;
    if (pTimerReadPipe!=-1)
        ::read(pTimerReadPipe, &ix, 1);
    return ix;
}


void TAndroidNativeCore::destroy()
{
    log_v("TAndroidNativeCore::destroy()!");
    free_saved_state();
    pthread_mutex_lock(&pMutex);
    if (pInputQueue != NULL) {
        AInputQueue_detachLooper(pInputQueue);
    }
    destroy_timer_handler();
    AConfiguration_delete(pConfig);
    pDestroyed = 1;
    pthread_cond_broadcast(&pCond);
    pthread_mutex_unlock(&pMutex);
    // Can't touch android_app object after this.
}


void *TAndroidNativeCore::thread_entry(void* param)
{
    pConfig = AConfiguration_new();
    AConfiguration_fromAssetManager(pConfig, pActivity->assetManager);

    print_cur_config();

    pAppLooper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    ALooper_addFd(pAppLooper, pMsgReadPipe, LOOPER_ID_MAIN, ALOOPER_EVENT_INPUT, NULL, NULL);

    create_timer_handler();

    pthread_mutex_lock(&pMutex);
    pRunning = 1;
    pthread_cond_broadcast(&pCond);
    pthread_mutex_unlock(&pMutex);

    //char *argv[] = { strdup(pActivity->obbPath), 0 };
    TAndroidNativeActivity::createEmulator();
    TAndroidNativeActivity::initEmulator();
    TAndroidNativeActivity::runEmulator();

    destroy();
    return NULL;
}

/**
 Allocate memory for our internal screen buffer.
 TODO: react to screen changes
 TODO: user should be able to change screen margins on the fly
 */
void TAndroidNativeCore::allocate_screen()
{
    pApplicationWindowBuffer.bits = calloc(pScreenWidth*pScreenHeight, 2); // one uint16_t per pixel
    pApplicationWindowBuffer.width =  (int32_t)pScreenWidth;
    pApplicationWindowBuffer.height = (int32_t)pScreenHeight;
    pApplicationWindowBuffer.stride = (int32_t)pScreenWidth;
    pApplicationWindowBuffer.format = WINDOW_FORMAT_RGB_565;
}

bool TAndroidNativeCore::copy_screen()
{
    bool ret = false;
    if (lock_screen()) {
        ARect r;
        popDirtyRect(r);

        /* Now follows a horrible hack.
         *
         * OK, so here is the long story for this optimisations. We used to spend a lot of time
         * copying screen content from the Activity RGB buffer into the hardware buffer when we
         * really only needed to copy the bits that were changed since the last call.
         *
         * Well, it turns out that Android keeps up to three hardware buffers around, and does not
         * accumulate the changes. So in the worst case, the third buffer is three changes behind.
         * We fixed that by accumulating screen changes for the first, second, and third buffer.
         *
         * But Android doe not guarantee that we will always get the same three buffers. So in the
         * hack below, we store the most recent buffers as 'known buffers'. If any buffer in that
         * list changes, we forget all buffers and start anew. So far, this seems to work ok,
         * although it does not take into account that the order of buffers could change as well.
         *
         * If it works, it reduces CPU load by up to 60% in some special cases. Writing on the
         * screen is one of those cases, and should now be much faster, even on slow hosts.
         *
         * FIXME: Android of course optimizes buffer use, and we need to take into account that
         * the user might focus on another app. When the focus goes back to Einstein, we may get
         * the same buffers, but their content may be completely different.
         *
         * TODO: make sure that the entire screen is redrawn after focus events and similar
         * TODO: hide a buffer serial identifier in the blue channel of the screen buffer to verify
         *       that buffer content hasn't changed completely
         */
        static void *knownScreenBuffer[pNScreenBuffer] = { NULL };
        void *bits = pNativeWindowBuffer.bits;
        int i;
        for (i=0; i<pNScreenBuffer; i++) {
            if (knownScreenBuffer[i]==bits) break;
        }
        if (i==pNScreenBuffer) {
            // - this is a previously unknown screen buffer, which messes up our entire assumption
            for (i = 0; i < pNScreenBuffer; i++) {
                if (knownScreenBuffer[i] == 0L) {
                    knownScreenBuffer[i] = bits;
                    break;
                }
            }
            if (i == pNScreenBuffer) {
                knownScreenBuffer[0] = bits;
                for (i = 1; i < pNScreenBuffer; i++) {
                    knownScreenBuffer[i] = NULL;
                }
            }
            r.left = 0; r.right = (int32_t)pScreenWidth; r.top = 0; r.bottom = (int32_t)pScreenHeight;
        }

        //log_i("Redrawing %d %d %d %d", r.left, r.right, r.top, r.bottom);

        // TODO: there are endless possibilities to optimize the following code
        // We are wasting time by copying the entire screen contents at every dirty frame
        // We can identify previously written buffers and copy only those pixels
        // that actually changed.
        int srcStride = pApplicationWindowBuffer.stride;
        int ww = pApplicationWindowBuffer.width;
        int hh = pApplicationWindowBuffer.height;

        int dstStride = pNativeWindowBuffer.stride;

        // FIXME: bad clipping
        if (pNativeWindowBuffer.width<ww) ww = pNativeWindowBuffer.width;
        if (pNativeWindowBuffer.height<hh) hh = pNativeWindowBuffer.height;

        int32_t top = r.top; if (top<0) top = 0;
        int32_t bottom = r.bottom; if (bottom>hh) bottom = hh;
        int32_t left = r.left; if (left<0) left = 0;
        int32_t right = r.right; if (right>ww) right = ww;

        const uint16_t *src = (uint16_t*)pApplicationWindowBuffer.bits
                              + top*srcStride + left ;
        uint16_t *dst = (uint16_t*)pNativeWindowBuffer.bits
                        + pScreenTopPadding*dstStride
                        + top*dstStride + left;
        ww = right - left;

        for (int32_t y=top; y<bottom; ++y) {
            memcpy(dst, src, size_t(ww * 2));
            src += srcStride;
            dst += dstStride;
        }

        unlock_and_post_screen();
        ret = true;
    } else {
        // wait for screen buffer to be created
    }
    return ret;
}

/**
 Take ownership of screen memory for gaining write access.

 If the screen is already locked, it will not be locked again
 and a value of true will be returned.

 \return true if we gaines access, false if no access was granted and screen memory must not be writte to
 */
bool TAndroidNativeCore::lock_screen()
{
    if (screen_is_locked())
        return true;

    if (!pNativeWindow) {
        log_w("Unable to lock window buffer: no native window found.");
        return false;
    }

    if (ANativeWindow_lock(pNativeWindow, &pNativeWindowBuffer, 0L) < 0) {
        log_w("Unable to lock window buffer: Android won't lock.");
        return false;
    }
    return true;
}

/**
 Release screen memory ownership and give it back to the system.

 The memory content will be copied to the physical screen next.
 If the screen is not locked, this call will have no effect.
 */
void TAndroidNativeCore::unlock_and_post_screen()
{
    if (!screen_is_locked())
        return;

    ANativeWindow_unlockAndPost(pNativeWindow);
    pNativeWindowBuffer.bits = 0L; // avoid any misunderstandings...
}

/**
 Is the screen currently locked?
 \return true if it is locked and the app has write access.
 */
bool TAndroidNativeCore::screen_is_locked()
{
    return (pNativeWindowBuffer.bits!=0L);
}

static void rectUnion(ARect &d, const ARect &s) {
    if (d.right==0) {
        d = s;
    } else {
        if (s.top<d.top) d.top = s.top;
        if (s.left<d.left) d.left = s.left;
        if (s.bottom>d.bottom) d.bottom = s.bottom;
        if (s.right>d.right) d.right = s.right;
    }
}

/**
 * Mark an area of the screen dirty, so that it will be redrawn.
 * Dirty rectangles are merged into one bounding box rectangle.
 * @param r
 */
void TAndroidNativeCore::addDirtyRect(const ARect &r)
{
    pScreenMutex.Lock();
    for (int i=0; i<pNScreenBuffer; ++i) {
        rectUnion(pDirtyRect[i], r);
    }
    pScreenIsDirty = true;
    pScreenMutex.Unlock();
}

/**
 * Tell Android to redraw the entire screen.
 * @param r
 */
void TAndroidNativeCore::addDirtyScreen()
{
    ARect r = { 0, 0, (int32_t)pScreenWidth, (int32_t)pScreenHeight };
    addDirtyRect(r);
}

/**
 * Pop the topmost rectangle from the stack and add an empty rect at the bottom.
 * @param r
 */
void TAndroidNativeCore::popDirtyRect(ARect &r)
{
    pScreenMutex.Lock();
    r = pDirtyRect[0];
    //log_i("Dirty: %d %d %d %d", r.left, r.top, r.right, r.bottom);
    memmove(pDirtyRect+0, pDirtyRect+1, pNScreenBuffer*sizeof(ARect));
    pScreenIsDirty = false;
    pScreenMutex.Unlock();
}



// ---- TAndroidNativeActivity ---------------------------------------------------------------------


void TAndroidNativeActivity::write_cmd(int8_t cmd)
{
    if (write(pMsgWritePipe, &cmd, sizeof(cmd)) != sizeof(cmd)) {
        log_e("Failure writing android_app cmd: %s\n", strerror(errno));
    }
}


void TAndroidNativeActivity::set_input(AInputQueue* inputQueue)
{
    pthread_mutex_lock(&pMutex);
    pPendingInputQueue = inputQueue;
    write_cmd(APP_CMD_INPUT_CHANGED);
    while (pInputQueue != pPendingInputQueue) {
        pthread_cond_wait(&pCond, &pMutex);
    }
    pthread_mutex_unlock(&pMutex);
}


void TAndroidNativeActivity::set_window(ANativeWindow* window)
{
    pthread_mutex_lock(&pMutex);
    if (pPendingWindow != NULL) {
        write_cmd(APP_CMD_TERM_WINDOW);
    }
    pPendingWindow = window;
    if (window != NULL) {
        write_cmd(APP_CMD_INIT_WINDOW);
    }
    while (pNativeWindow != pPendingWindow) {
        pthread_cond_wait(&pCond, &pMutex);
    }
    pthread_mutex_unlock(&pMutex);
}


void TAndroidNativeActivity::set_activity_state(int8_t cmd)
{
    pthread_mutex_lock(&pMutex);
    write_cmd(cmd);
    while (pActivityState != cmd) {
        pthread_cond_wait(&pCond, &pMutex);
    }
    pthread_mutex_unlock(&pMutex);
}


void TAndroidNativeActivity::close_activity()
{
    pthread_mutex_lock(&pMutex);
    write_cmd(APP_CMD_DESTROY);
    while (!pDestroyed) {
        pthread_cond_wait(&pCond, &pMutex);
    }
    pthread_mutex_unlock(&pMutex);

    close(pMsgReadPipe);
    close(pMsgWritePipe);
    pthread_cond_destroy(&pCond);
    pthread_mutex_destroy(&pMutex);
}


// ---- Android Native Activity callbacks ----

/**
 The rectangle in the window in which content should be placed has changed.
 */
void TAndroidNativeActivity::onContentRectChanged(ANativeActivity *activity, const ARect *rect)
{
    // TODO: implement me
    //log_w("unhandled onContentRectChanged");
    if (rect) {
        addDirtyRect(*rect);
    } else {
        addDirtyScreen();
    }
}

/**
 The drawing window for this native activity needs to be redrawn. To avoid transient artifacts during screen changes (such resizing after rotation), applications should not return from this function until they have finished drawing their window in its current state.
 */
void TAndroidNativeActivity::onNativeWindowRedrawNeeded(ANativeActivity *activity, ANativeWindow *window)
{
    // TODO: implement me
    //log_w("unhandled onNativeWindowRedrawNeeded");
    addDirtyScreen();
}

/**
 The drawing window for this native activity has been resized. You should retrieve the new size from the window and ensure that your rendering in it now matches.
 */
void TAndroidNativeActivity::onNativeWindowResized(ANativeActivity *activity, ANativeWindow *window)
{
    // TODO: implement me
    log_w("unhandled onNativeWindowResized");
    addDirtyScreen();
}

/**
 NativeActivity is being destroyed. See Java documentation for Activity.onDestroy() for more information.
 */
void TAndroidNativeActivity::onDestroy(ANativeActivity* activity)
{
    log_v("Destroy: %p\n", activity);
    // FIXME: use the correct free()
    close_activity();
}

/**
 NativeActivity has started. See Java documentation for Activity.onStart() for more information.
 */
void TAndroidNativeActivity::onStart(ANativeActivity* activity)
{
    log_v("Start: %p\n", activity);
    set_activity_state(APP_CMD_START);
}

/**
 NativeActivity has resumed. See Java documentation for Activity.onResume() for more information.
 */
void TAndroidNativeActivity::onResume(ANativeActivity* activity)
{
    log_v("Resume: %p\n", activity);
    set_activity_state(APP_CMD_RESUME);
    addDirtyScreen();
}

/**
 Framework is asking NativeActivity to save its current instance state. See Java documentation for Activity.onSaveInstanceState() for more information. The returned pointer needs to be created with malloc(); the framework will call free() on it for you. You also must fill in outSize with the number of bytes in the allocation. Note that the saved state will be persisted, so it can not contain any active entities (pointers to memory, file descriptors, etc).
 */
void *TAndroidNativeActivity::onSaveInstanceState(ANativeActivity* activity, size_t* outLen)
{
    struct android_app* android_app = (struct android_app*)activity->instance;
    void* savedState = NULL;

    log_v("SaveInstanceState: %p\n", activity);
    pthread_mutex_lock(&pMutex);
    pStateSaved = 0;
    write_cmd(APP_CMD_SAVE_STATE);
    while (!pStateSaved) {
        pthread_cond_wait(&pCond, &pMutex);
    }

    if (pSavedState != NULL) {
        savedState = pSavedState;
        *outLen = pSavedStateSize;
        pSavedState = NULL;
        pSavedStateSize = 0;
    }

    pthread_mutex_unlock(&pMutex);

    return savedState;
}

/**
 NativeActivity has paused. See Java documentation for Activity.onPause() for more information.
 */
void TAndroidNativeActivity::onPause(ANativeActivity* activity)
{
    log_v("Pause: %p\n", activity);
    set_activity_state(APP_CMD_PAUSE);
}

/**
 NativeActivity has stopped. See Java documentation for Activity.onStop() for more information.
 */
void TAndroidNativeActivity::onStop(ANativeActivity* activity)
{
    log_v("Stop: %p\n", activity);
    set_activity_state(APP_CMD_STOP);
}

/**
 The current device AConfiguration has changed. The new configuration can be retrieved from assetManager.
 */
void TAndroidNativeActivity::onConfigurationChanged(ANativeActivity* activity)
{
    struct android_app* android_app = (struct android_app*)activity->instance;
    log_v("ConfigurationChanged: %p\n", activity);
    write_cmd(APP_CMD_CONFIG_CHANGED);
}

/**
 The system is running low on memory. Use this callback to release resources you do not need, to help the system avoid killing more important processes.
 */
void TAndroidNativeActivity::onLowMemory(ANativeActivity* activity)
{
    struct android_app* android_app = (struct android_app*)activity->instance;
    log_v("LowMemory: %p\n", activity);
    write_cmd(APP_CMD_LOW_MEMORY);
}

/**
 Focus has changed in this NativeActivity's window. This is often used, for example, to pause a game when it loses input focus.
 */
void TAndroidNativeActivity::onWindowFocusChanged(ANativeActivity* activity, int focused)
{
    log_v("WindowFocusChanged: %p -- %d\n", activity, focused);
    write_cmd(focused ? APP_CMD_GAINED_FOCUS : APP_CMD_LOST_FOCUS);
    addDirtyScreen();
}

/**
 The drawing window for this native activity has been created. You can use the given native window object to start drawing.
 */
void TAndroidNativeActivity::onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window)
{
    log_v("NativeWindowCreated: %p -- %p\n", activity, window);
    set_window(window);
    addDirtyScreen();
}

/**
 The drawing window for this native activity is going to be destroyed. You MUST ensure that you do not touch the window object after returning from this function: in the common case of drawing to the window from another thread, that means the implementation of this callback must properly synchronize with the other thread to stop its drawing before returning from here.
 */
void TAndroidNativeActivity::onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window)
{
    log_v("NativeWindowDestroyed: %p -- %p\n", activity, window);
    set_window(NULL);
}

/**
 The input queue for this native activity's window has been created. You can use the given input queue to start retrieving input events.
 */
void TAndroidNativeActivity::onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue)
{
    log_v("InputQueueCreated: %p -- %p\n", activity, queue);
    set_input(queue);
}

/**
 The input queue for this native activity's window is being destroyed. You should no longer try to reference this object upon returning from this function.
 */
void TAndroidNativeActivity::onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue)
{
    log_v("InputQueueDestroyed: %p -- %p\n", activity, queue);
    set_input(NULL);
}

/**
 Main Entry Point 2.
 Create a thread that will launch Einstein and handle communication between Java and Einstein and back.
 \param activity the interface to the Java end of Android
 \param savedState if this app is relaunched, this is a memory block with the state of the app when it was interrupted
 \param savedStateSize size of that block
 */
void TAndroidNativeActivity::create(ANativeActivity* activity, void* savedState,
                                 size_t savedStateSize)
{
    static const char *sEinstein = "Einstein";
    activity->instance = (void*)sEinstein;
    set_activity(activity);
    set_callbacks();

    // We use the top padding because NativeActivity will always be under a translucent status bar.
    // By offsetting the content, we don;t disturb that bar
    char host[1024] = { 0 };
    int statusBarHeight = -1;
    int navigationBarHeight = -1;
    int screenHeight = -1;
    {   // This is already the main Java thread. No need to attach.
        JNIEnv *env = get_activity()->env;
        jclass clazz = env->GetObjectClass(pActivity->clazz);
        jmethodID methodID = env->GetMethodID(clazz, "getStatusBarHeight", "()I");
        statusBarHeight = env->CallIntMethod(pActivity->clazz, methodID);
        methodID = env->GetMethodID(clazz, "getNavigationBarHeight", "()I");
        navigationBarHeight = env->CallIntMethod(pActivity->clazz, methodID);
        methodID = env->GetMethodID(clazz, "getScreenHeight", "()I");
        screenHeight = env->CallIntMethod(pActivity->clazz, methodID);
        jmethodID midToString = env->GetMethodID(clazz, "getDeviceName", "()Ljava/lang/String;");
        jstring jstr = (jstring) env->CallObjectMethod(pActivity->clazz, midToString);
        if (jstr != 0) {
            const char* cstr = env->GetStringUTFChars(jstr, 0);
            strncpy(host, cstr, sizeof(host));
            env->ReleaseStringUTFChars(jstr, cstr);
            env->DeleteLocalRef(jstr);
        }
    }

    if (strcmp(host, "Freescale EVK_MX6SL")==0) {
        // MobiScribe API Level: 19
        // Scale from 480x320 at 2.8x3.3, so we use factor 2.5 (542x432) or factor 2 (678x540)
        pHostID = HOST_ID_EVK_MX6SL;
        pScreenHeight = 542;
        pScreenWidth = 432;
    } else if (strcmp(host, "samsung SM-N975F")==0) {
        // 2280 x 1080
        pScreenHeight = (int)((2280-4*24)/3);
        pScreenWidth = (int)(1080/3);
    } else if (strcmp(host, "unknown Android SDK built for x86_64")==0) {
        // Emulator
    }
    log_i("HostID is \"%s\"", host);

    // 63, 126, 1794 (1920) on our emulator
    //pad/hgt = stat/shgt
    pScreenTopPadding = (pScreenHeight * statusBarHeight / screenHeight) +2;
    //pScreenBottomPadding = (pScreenHeight * 32 / screenHeight) +2;

    allocate_screen();

    pthread_mutex_init(&pMutex, NULL);
    pthread_cond_init(&pCond, NULL);

    if (savedState != NULL) {
        pSavedState = malloc(savedStateSize);
        pSavedStateSize = savedStateSize;
        memcpy(pSavedState, savedState, savedStateSize);
    }

    int msgpipe[2];
    if (pipe(msgpipe)) {
        log_e("could not create pipe: %s", strerror(errno));
        return;
    }
    pMsgReadPipe = msgpipe[0];
    pMsgWritePipe = msgpipe[1];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&pThread, &attr, thread_entry, 0L);

    // Wait for thread to start.
    pthread_mutex_lock(&pMutex);
    while (!pRunning) {
        pthread_cond_wait(&pCond, &pMutex);
    }
    pthread_mutex_unlock(&pMutex);
}

/**
 Set all callbacks from the Native Activity.
 */
void TAndroidNativeActivity::set_callbacks()
{
    ANativeActivityCallbacks *cb = pActivity->callbacks;
    cb->onContentRectChanged = onContentRectChanged;
    cb->onNativeWindowRedrawNeeded = onNativeWindowRedrawNeeded;
    cb->onNativeWindowResized = onNativeWindowResized;
    cb->onDestroy = onDestroy;
    cb->onStart = onStart;
    cb->onResume = onResume;
    cb->onSaveInstanceState = onSaveInstanceState;
    cb->onPause = onPause;
    cb->onStop = onStop;
    cb->onConfigurationChanged = onConfigurationChanged;
    cb->onLowMemory = onLowMemory;
    cb->onWindowFocusChanged = onWindowFocusChanged;
    cb->onNativeWindowCreated = onNativeWindowCreated;
    cb->onNativeWindowDestroyed = onNativeWindowDestroyed;
    cb->onInputQueueCreated = onInputQueueCreated;
    cb->onInputQueueDestroyed = onInputQueueDestroyed;
}

/**
 This is the main entry point from the Android JavaVM into the native world.
 */
JNIEXPORT void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize)
{
    // TODO: can we return an error message here if creation of the app failed?
    TAndroidNativeActivity::create(activity, savedState, savedStateSize);
}


void TAndroidNativeActivity::createEmulator()
{
    log_i("Entering createEmulator().");

    // Create view
    //einsteinView = new EinsteinView(this);

    // Show or hide Android status bar. Note that this must take place before we call setContentView
    //requestWindowFeature(Window.FEATURE_NO_TITLE);
    //final bool statusBarVisible = sharedPrefs.getBoolean("androidstatusbar", true);
    //updateFullscreenStatus(statusBarVisible);
    //setContentView(einsteinView);

    //int permission =
    //        ContextCompat.checkSelfPermission(this,
    //                                          Manifest.permission.WRITE_EXTERNAL_STORAGE);
    //if (permission != PackageManager.PERMISSION_GRANTED) {
    //    ActivityCompat.requestPermissions(this,
    //                                      new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
    //                                      REQUEST_WRITE);
    //    return;
    //}

    einstein = new TAndroidNativeApp();
    theLog = new TAndroidLog();

    log_i("Leaving createEmulator().");
}


void TAndroidNativeActivity::initEmulator()
{
    log_i("Entering initEmulator(): Initializing Einstein application.");

//    einstein->Run( "/storage/emulated/0/Download/Einstein", pScreenWidth, pScreenHeight, theLog );
    einstein->Run( "/sdcard/Download/Einstein", (int)pScreenWidth, (int)pScreenHeight, theLog );
    //MiscUtils.sleepForMillis(2000);

    log_i("Leaving initEmulator().");
}


int TAndroidNativeActivity::handleLooperCommands()
{
    int8_t cmd = TAndroidNativeCore::read_cmd();
    TAndroidNativeCore::pre_exec_cmd(cmd);

    // handle additional commands here that are not handled by pre_ or Post_exec_cmd().
    // See: APP_CMD_... for a list of available commands send by Android

    // fixup and finalize application wide command handling
    TAndroidNativeCore::post_exec_cmd(cmd);
    return 1;
}


int TAndroidNativeActivity::handleLooperInputEvents()
{
    AInputQueue *queue = TAndroidNativeCore::input_event_queue();
    AInputEvent *event = nullptr;

    if (AInputQueue_getEvent(queue, &event) >= 0) {
        if (AInputQueue_preDispatchEvent(queue, event)==0) {
            int consumed = 0;
            switch (AInputEvent_getType(event)) {
                case  AINPUT_EVENT_TYPE_KEY:
                    // TODO: we still need to write the keyboard handler
                    //consumed = handle_keyboard_event(queue, event);
                    break;
                case AINPUT_EVENT_TYPE_MOTION:
                    consumed = handleLooperMouseEvent(queue, event);
                    break;
                default:
                    // don't do anything. There may be additional event types in the future
                    break;
            }
            AInputQueue_finishEvent(queue, event, consumed);
        }
    }
    return 0;
}


int TAndroidNativeActivity::handleLooperMouseEvent(AInputQueue *queue, AInputEvent *event)
{
    int ex = (int)(AMotionEvent_getX(event, 0) * pScreenWidth /
                   ANativeWindow_getWidth(TAndroidNativeCore::native_window()));
    int ey = (int)(AMotionEvent_getY(event, 0) * (pScreenHeight+pScreenTopPadding+pScreenBottomPadding) /
                   ANativeWindow_getHeight(TAndroidNativeCore::native_window()) - pScreenTopPadding + pScreenBottomPadding)
                           - pScreenBottomPadding;

    int consumed = 0;
    if (einstein) {
        TScreenManager *screen = einstein->getScreenManager();
        if (screen) {
            switch (AMotionEvent_getAction(event)) {
                case AMOTION_EVENT_ACTION_DOWN:
                    // every tap resets the sleep timer
                    if (!einstein->getPlatformManager()->IsPowerOn()) {
                        // After five minutes, the MP switches itself off. On Android,
                        // the host OS should decide when to put the device to sleep.
                        // Newton OS nevertheless switches itself off. To continue work,
                        // we'd have to pull the power switch. Since we have no power switch
                        // on Einstein/IOS, any screen touch will power the Newton back on.
                        einstein->PowerOn();
                    }
                    // fall through
                case AMOTION_EVENT_ACTION_MOVE:
                    screen->PenDown((KUInt16)(screen->GetActualScreenHeight()-ey), (KUInt16)ex);
                    consumed = 1;
                    break;
                case AMOTION_EVENT_ACTION_UP:
                    screen->PenUp();
                    consumed = 1;
                    break;
                default:
                    break;
            }
        }
    }
    return consumed;
}


void TAndroidNativeActivity::runEmulator()
{
    addDirtyScreen();
    while (pDestroyRequested==0) {
        int done = false;
        int ident = 0;
        int events;
        int ret = 0;
        // currently empty
        int delay = 1000/10; // millis. This value is also the maximum frame rate
        while (!done) {
            ident = ALooper_pollOnce(delay, nullptr, &events, nullptr);
            switch (ident) {
                case TAndroidNativeCore::LOOPER_ID_MAIN:
                    ret = handleLooperCommands();
                    break;
                case TAndroidNativeCore::LOOPER_ID_INPUT:
                    ret = handleLooperInputEvents();
                    break;
                case TAndroidNativeCore::LOOPER_ID_TIMER:
                    //timer_do_callback(TAndroidNativeCore::receive_timer_index());
                    break;
                case ALOOPER_POLL_WAKE:
                    TAndroidNativeCore::log_e("Someone woke up ALooper_pollOnce.");
                    done = true;
                    break;
                case ALOOPER_POLL_CALLBACK:
                    TAndroidNativeCore::log_e(
                            "Someone added a callback to ALooper_pollOnce.");
                    done = true;
                    break;
                case ALOOPER_POLL_TIMEOUT:
                    done = true; // timer expired, interrupt the loop
                    break;
                case ALOOPER_POLL_ERROR:
                    TAndroidNativeCore::log_e(
                            "Something caused an ERROR in ALooper_pollOnce.");
                    done = true; // return to the app to find the error
                    break;
                default:
                    TAndroidNativeCore::log_e(
                            "Unknown return value from ALooper_pollOnce.");
                    done = true; // return to the app, just in case
                    break;
            }
            // we need to repeat this as long as there are messages in the queue, or any
            // change in the graphical interface will trigger a redraw immediately. To
            // save time and energy, we want to collect graphics changes and execute
            // them as soon as no more events are pending.
            // Setting delay to zero on the second round makes sure that all events
            // are handled first, and the call returns only when no more
            // events are pending.
            delay = 0;
        }
        if (pScreenIsDirty) {
            // - copy Einstein screen memory into a local RGB buffer
            // This is a direct buffer, so we only need to copy the current dirty region
            einstein->updateScreen((KUInt16*)pApplicationWindowBuffer.bits, pDirtyRect[pNScreenBuffer-1]);
            // - copy from local RGB buffer into Android hardware buffer
            // Android triple-buffers in hardware, so copy_screen must copy all current changes
            // plus the changes from the two previous dirty regions
            copy_screen();
        }
        //log_i("PC=%08x", einstein->GetEmulator()->GetProcessor()->GetRegister(15));
    }

}


void TAndroidNativeActivity::alert(const char* text)
{
    TAndroidNativeToJava java;
    if (java.is_attached()) {
        jclass clazz = java.env()->GetObjectClass(pActivity->clazz);
        jstring jmessage = java.env()->NewStringUTF(text);
        jmethodID methodID = java.env()->GetMethodID(clazz, "showAlert", "(Ljava/lang/String;)V");
        java.env()->CallVoidMethod(pActivity->clazz, methodID, jmessage);
        java.env()->DeleteLocalRef(jmessage);
    }
}


// ---- Java Stuff ---------------------------------------------------------------------------------

/*
 * In AndroidNative, we try to avoid having Java source files. The class below still allow us
 * to call Java methods from within native code.
 *
 * @code
 *   Fl_Android_Java java;
 *   if (java.is_attached()) {
 *     jclass class_tone_generator = java.env()->FindClass("android/media/ToneGenerator");
 *     jmethodID toneGeneratorConstructor = java.env()->GetMethodID(
 *          class_tone_generator, "<init>",
 *          "(II)V");
 *     jobject toneGeneratorObj = java.env()->NewObject(
 *          class_tone_generator, toneGeneratorConstructor,
 *          4,  // STREAM_ALARM
 *          100); // volume
 *    jmethodID method_start_tone = java.env()->GetMethodID(
 *          class_tone_generator,
 *          "startTone",
 *          "(II)Z");
 *     java.env()->CallBooleanMethod(
 *          toneGeneratorObj, method_start_tone,
 *          androidSoundID,
 *          1000);
 *    java.env()->DeleteLocalRef(class_tone_generator);
 *    java.env()->DeleteLocalRef(toneGeneratorObj);
 *  }
 * @endcode
 */

TAndroidNativeToJava::TAndroidNativeToJava()
{
    jint lResult;
    jint lFlags = 0;

    pJavaVM = TAndroidNativeCore::get_activity()->vm;
    pJNIEnv = TAndroidNativeCore::get_activity()->env;

    JavaVMAttachArgs lJavaVMAttachArgs = {
            .version = JNI_VERSION_1_6,
            .name = "NativeThread",
            .group = nullptr
    };

    lResult = pJavaVM->AttachCurrentThread(&pJNIEnv, NULL); //&lJavaVMAttachArgs);
    if (lResult == JNI_ERR) return;

    pNativeActivity = TAndroidNativeCore::get_activity()->clazz;

    pNativeActivityClass = env()->GetObjectClass(pNativeActivity);

    pAttached = true;
}


TAndroidNativeToJava::~TAndroidNativeToJava()
{
    if (is_attached()) {
        pJavaVM->DetachCurrentThread();
    }
}

// ======================================================================= //
// We build our computer (systems) the way we build our cities: over time,
// without a plan, on top of ruins.
//   -- Ellen Ullman
// ======================================================================= //

