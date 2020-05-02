// File:			TAndroidNativeApp.h
// ==============================
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

#ifndef _TANDROID_NATIVE_APP_H
#define _TANDROID_NATIVE_APP_H

#include <poll.h>
#include <pthread.h>
#include <sched.h>

#include <android/configuration.h>
#include <android/looper.h>
#include <android/native_activity.h>
#include <android/window.h>

#include <K/Defines/KDefinitions.h>
#include <K/Threads/TMutex.h>
#include <Emulator/Serial/TSerialPortManager.h>

#include "AndroidGlue.h"
#include "Version.h"


class TROMImage;
class TEmulator;
class TSoundManager;
class TScreenManager;
class TLog;
class TPlatformManager;
class TNetworkManager;
class TSerialPortManager;

///
/// Classe pour le programme einstein en ligne de commande.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \author Matthias Melcher <einstein@matthiasm.com>
/// \version $Revision: 113 $
///
/// \test	aucun test d�fini.
///
class TAndroidNativeApp
{
public:
    ///
    /// Constructeur par d�faut.
    ///
    TAndroidNativeApp( void );

    ///
    /// Destructeur.
    ///
    ~TAndroidNativeApp( void );

    ///
    /// Point d'entr�e.
    ///
    void Run(const char *dataPath, int screenWidth, int screenHeight, TLog *inLog);

    ///
    /// Stop the app thread
    ///
    void Stop();

    ///
    /// awake emulator from sleep
    ///
    void PowerOn();

    ///
    /// send emulator to sleep
    ///
    void PowerOff();

    int IsPowerOn();

    ///
    /// user requested simulating the power button
    ///
    void menuPower();

    ///
    /// user requested simulating holding the power button longer
    ///
    void menuBacklight();

    ///
    /// user requested to install a package
    ///
    void menuInstallPackage();

    ///
    /// show the "About..." screen
    ///
    void menuAbout();

    ///
    /// user wants to see the Settings dialog box
    ///
    void menuShowSettings();

    ///
    /// Open the dialog to download the ROM via TCP/IP
    ///
    void menuDownloadROM();

    ///
    /// Reboot NewtonOS
    ///
    void reboot();

    void ChangeScreenSize(int w, int h);

    TPlatformManager *getPlatformManager() { return mPlatformManager; }

    TScreenManager *getScreenManager() { return mScreenManager; }

    int updateScreen(unsigned short *buffer, const ARect &r);

    void SetNewtonID(KUInt32 id0, KUInt32 id1) {
        mNewtonID0 = id0;
        mNewtonID1 = id1;
    }

    TEmulator *GetEmulator() { return mEmulator; }

private:
    ///
    /// Constructeur par copie volontairement indisponible.
    ///
    /// \param inCopy		objet � copier
    ///
    TAndroidNativeApp( const TAndroidNativeApp& inCopy );

    ///
    /// Op�rateur d'assignation volontairement indisponible.
    ///
    /// \param inCopy		objet � copier
    ///
    TAndroidNativeApp& operator = ( const TAndroidNativeApp& inCopy );

    ///
    /// Affiche un message d'erreur sur la syntaxe et sort.
    ///
    void SyntaxError( void );

    ///
    /// Affiche un message d'erreur sur la syntaxe (une option en particulier)
    /// et sort.
    ///
    /// \param inBadOption	bad option
    ///
    void SyntaxError( const char* inBadOption );

    ///
    /// Affiche l'aide et sort.
    ///
    void Help( void );

    ///
    /// Affiche la version et sort.
    ///
    void Version( void );

    ///
    /// Cr�e le gestionnaire de son.
    ///
    void CreateSoundManager( const char* inClass );

    ///
    /// Cr�e le gestionnaire d'�cran.
    ///
    void CreateScreenManager(
            const char* inClass,
            int inPortraitWidth,
            int inPortraitHeight,
            bool inFullScreen);

    ///
    /// Cr�e le log.
    ///
    void CreateLog( const char* inPath );

    ///
    /// Point d'entr�e du processus l�ger.
    ///
    static void* SThreadEntry( void* inUserData )
    {
        ((TAndroidNativeApp*) inUserData)->ThreadEntry();
        return NULL;
    }

    ///
    /// Point d'entr�e du processus l�ger.
    ///
    void ThreadEntry( void );

    ///
    /// Boucle du menu.
    ///
    void MenuLoop( void );

    ///
    /// Boucle du menu (moniteur)
    ///
    void MonitorMenuLoop( void );

    ///
    /// Boucle du menu (app)
    ///
    void AppMenuLoop( void );

    ///
    /// Execute a command.
    ///
    /// \return true if the command was known.
    ///
    bool ExecuteCommand( const char* inCommand );

    ///
    /// Affiche l'aide (du menu)
    ///
    void PrintHelp( void );

    ///
    /// Affiche une ligne (dans stdout ou via le moniteur)
    ///
    void PrintLine( const char* inLine );

    /// \name Variables
    const char*			mProgramName;		///< Nom du programme.
    TROMImage*			mROMImage;			///< Image ROM.
    TEmulator*			mEmulator;			///< Emulateur.
    TSoundManager*		mSoundManager;		///< Gestionnaire de son.
    TScreenManager*		mScreenManager;		///< Gestionnaire d'�cran.
    TPlatformManager*	mPlatformManager;	///< Reference to the platform manager.
    TLog*				mLog;				///< Log.
    TNetworkManager*	mNetworkManager;	///< network connection
    bool				mQuit;				///< If we should quit.

    KUInt32				mNewtonID0, mNewtonID1;
};


/**
 Static class that manages all interaction between the Android Native Activity
 and Einstein. It also keeps often used data for global access.

 On launch, it creates a main thread and communication pipe to
 the Activity. All Einstein code will run in that thread. Activity
 events will be polled in runEmulator() using the provided
 Android Looper and then be routed to the emulator.

 This code is based on the native activity interface
 provided by <android/native_activity.h>. It is based on a set
 of application-provided callbacks that will be called
 by the Activity's main thread when certain events occur.

 The "TAndroidNativeCore" class holds an ALooper instance that already
 listens to two important things:

    - activity lifecycle events (e.g. "pause", "resume"). See APP_CMD_XXX
      declarations below.

    - input events coming from the AInputQueue attached to the activity.

    Each of these correspond to an ALooper identifier returned by
    ALooper_pollOnce with values of LOOPER_ID_MAIN and LOOPER_ID_INPUT,
    respectively.
 */
class TAndroidNativeCore
{
public:
    enum {
        HOST_ID_UNKNOWN = 0,
        HOST_ID_EVK_MX6SL         // MobiScribe w/ 6.8" e-ink display
    };
    static int getHostID() { return pHostID; }
    static int pHostID;

    static KUInt32 pScreenTopPadding;
    static KUInt32 pScreenBottomPadding;
    static KUInt32 pScreenWidth;
    static KUInt32 pScreenHeight;
    static const int pNScreenBuffer = 3;

    static void addDirtyRect(const ARect &r);
    static void addDirtyScreen();
    static void popDirtyRect(ARect &r);
    static ARect pDirtyRect[pNScreenBuffer+1];
    static bool pScreenIsDirty;
    static TMutex pScreenMutex;

    enum {
        /**
         * Looper data ID of commands coming from the app's main thread, which
         * is returned as an identifier from ALooper_pollOnce().  The data for this
         * identifier is a pointer to an android_poll_source structure.
         * These can be retrieved and processed with android_app_read_cmd()
         * and android_app_exec_cmd().
         */
                LOOPER_ID_MAIN = 1,

        /**
         * Looper data ID of events coming from the AInputQueue of the
         * application's window, which is returned as an identifier from
         * ALooper_pollOnce().  The data for this identifier is a pointer to an
         * android_poll_source structure.  These can be read via the inputQueue
         * object of android_app.
         */
                LOOPER_ID_INPUT,

        /**
         * Timer data ID of all timer events coming from the Unix timer_create()
         * and friends, used in fl_add_timeout() and colleagues.
         */
                LOOPER_ID_TIMER,

        /**
         * Start of user-defined ALooper identifiers.
         */
                LOOPER_ID_USER,
    };

    /**
     * @see android.H Fl_Android_Platform_Event
     */
    enum {
        APP_CMD_INPUT_CHANGED,
        APP_CMD_INIT_WINDOW,
        APP_CMD_TERM_WINDOW,
        APP_CMD_WINDOW_RESIZED,
        APP_CMD_WINDOW_REDRAW_NEEDED,
        APP_CMD_CONTENT_RECT_CHANGED,
        APP_CMD_GAINED_FOCUS,
        APP_CMD_LOST_FOCUS,
        APP_CMD_CONFIG_CHANGED,
        APP_CMD_LOW_MEMORY,
        APP_CMD_START,
        APP_CMD_RESUME,
        APP_CMD_SAVE_STATE,
        APP_CMD_PAUSE,
        APP_CMD_STOP,
        APP_CMD_DESTROY,
    };

public:
    // --- logging
    static void log_e(const char *text, ...);
    static void log_w(const char *text, ...);
    static void log_i(const char *text, ...);
    static void log_v(const char *text, ...);

    // --- application state stuff
    static int8_t read_cmd();
    static void pre_exec_cmd(int8_t cmd);
    static void post_exec_cmd(int8_t cmd);
    static int destroy_requested() { return pDestroyRequested; }
    static const char *get_internal_data_path() { return pActivity->internalDataPath; }
    static const char *get_external_data_path() { return pActivity->externalDataPath; }
    static AAssetManager *get_asset_manager() { return pActivity->assetManager; }
    static ANativeActivity *get_activity() { return pActivity; }

    // --- event handling
    static AInputQueue *input_event_queue() { return pInputQueue; }

    // --- screen stuff
    static bool copy_screen();
    static inline ANativeWindow *native_window() { return pNativeWindow; }
    static inline ANativeWindow_Buffer &graphics_buffer() { return pApplicationWindowBuffer; }

    // --- timer stuff
    static void send_timer_index(uint8_t ix);
    static uint8_t receive_timer_index();


protected:
    static void free_saved_state();
    static void print_cur_config();
    static void destroy();
    static void* thread_entry(void* param);

    // --- screen handling stuff
    static void allocate_screen();
    static bool lock_screen();
    static void unlock_and_post_screen();
    static bool screen_is_locked();

    // --- timer stuff
    static void create_timer_handler();
    static void destroy_timer_handler();

    static ANativeActivity *pActivity;
    static AConfiguration *pConfig;
    static void *pSavedState;
    static size_t pSavedStateSize;
    static ALooper *pAppLooper;
    static AInputQueue *pInputQueue;
    static ANativeWindow *pNativeWindow;
    static ANativeWindow_Buffer pNativeWindowBuffer;
    static ANativeWindow_Buffer pApplicationWindowBuffer;
    static int pActivityState;
    static int pDestroyRequested;

    // ---- no need to make these visible to the outside ----
    static pthread_mutex_t pMutex;
    static pthread_cond_t pCond;
    static int pMsgReadPipe;
    static int pMsgWritePipe;
    static pthread_t pThread;
    static int pRunning;
    static int pStateSaved;
    static int pDestroyed;
    static AInputQueue* pPendingInputQueue;
    static ANativeWindow* pPendingWindow;

    // --- timer variables
    static int pTimerReadPipe;
    static int pTimerWritePipe;

};


/**
 * All methods in this class run in a different thread, started by TAndroidNativeCore.
 */
class TAndroidNativeActivity : public TAndroidNativeCore
{
public:
    static void createEmulator();
    static void initEmulator();
    static void runEmulator();

    static void alert(const char *text);

    static TAndroidNativeApp *einstein;
    static TLog *theLog;

private:
    static int handleLooperCommands();
    static int handleLooperInputEvents();
    static int handleLooperMouseEvent(AInputQueue *queue, AInputEvent *event);

public:
    static void create(ANativeActivity* activity, void* savedState, size_t savedStateSize);

private:
    static void set_activity(ANativeActivity *a) { pActivity = a; }
    static void set_callbacks();

    // ---- Android Native Activity interface
    static void write_cmd(int8_t cmd);
    static void set_input(AInputQueue* inputQueue);
    static void set_window(ANativeWindow* window);
    static void set_activity_state(int8_t cmd);
    static void close_activity();

    // ---- Android Native Activity callbacks ----
    static void onContentRectChanged(ANativeActivity *activity, const ARect *rect);
    static void onNativeWindowRedrawNeeded(ANativeActivity *activity, ANativeWindow *window);
    static void onNativeWindowResized(ANativeActivity *activity, ANativeWindow *window);
    static void onDestroy(ANativeActivity* activity);
    static void onStart(ANativeActivity* activity);
    static void onResume(ANativeActivity* activity);
    static void* onSaveInstanceState(ANativeActivity* activity, size_t* outLen);
    static void onPause(ANativeActivity* activity);
    static void onStop(ANativeActivity* activity);
    static void onConfigurationChanged(ANativeActivity* activity);
    static void onLowMemory(ANativeActivity* activity);
    static void onWindowFocusChanged(ANativeActivity* activity, int focused);
    static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window);
    static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window);
    static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue);
    static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue);
};


/**
 A class to make Java calls from C++ easier.
 */
class TAndroidNativeToJava
{
    JavaVM *pJavaVM = nullptr;
    JNIEnv *pJNIEnv = nullptr;
    jobject pNativeActivity;
    jclass pNativeActivityClass;
    bool pAttached = false;

public:
    TAndroidNativeToJava();
    ~TAndroidNativeToJava();
    bool is_attached() { return pAttached; }
    JavaVM *VM() { return pJavaVM; }
    JNIEnv *env() { return pJNIEnv; }
    jobject native_ativity() { return pNativeActivity; }
    jclass native_activity_class() { return pNativeActivityClass; }
};


#endif
// _TANDROID_NATIVE_APP_H

// ============================================================================ //
// Mac Airways:                                                                 //
// The cashiers, flight attendants and pilots all look the same, feel the same  //
// and act the same. When asked questions about the flight, they reply that you //
// don't want to know, don't need to know and would you please return to your   //
// seat and watch the movie.                                                    //
// ============================================================================ //
