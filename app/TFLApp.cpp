// ==============================
// File:			TFLApp.cp
// Project:			Einstein
//
// Copyright 2003-2020 by Paul Guyot and Matthias Melcher.
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

// ----- ASAPs
// TODO: compile for Android

// ----- User Interface improvements
// TODO: store Monitor location and state in settings
// TODO: launch Monitor stopped at boot point
// TODO: cleanup settings dialog
// TODO: make menubar and toolbar optional
// TODO: make FKey Bar for eMate emulation with volume slider (screenshot)
// TODO: is the drop-down menu still on par?

// ----- Minor Improvemnts in Usability
// TODO: menu and action to reboot Newton
// TODO: install essentials
// TODO: drag'n'drop of multiple files and archives
// TODO: drag'n'drop from network locations
// TODO: drag'n'drop for the Unna Archive
// TODO: automated Internet access (install and setup)
// TODO: add preferences to point to a UNAA archive image, so we can browse that and install quickly

// ----- Major new Features
// TODO: patch ROMs for Y10k bug
// TODO: integrate Inspector
// TODO: integrate newt/64
// TODO: printer support
// TODO: wake-up/launch on appointment in the future

// ----- Imporvemnets to the inner workings
// TODO: Full Android support as a address book and calender app
// TODO: Fix lockas and race conditions
// TODO: cleanup all compile warnings on all platforms
// TODO: Windows: static linking without installer/VC Libs (should work now?!)
// TODO: Linux: App Icon, Flatpak
// TODO: Usermode network gobbles up first byte: https://github.com/pguyot/Einstein/issues/58

// ----- Documentations
// TODO: refine and test the BUILD.md build instructions
// TODO: about panel must have all authors and references to the linked libraries (FLTK, ...)
// TODO: release notes
// TODO: help pages for use of Einstein, Monitor, etc.
// TODO: help for getting started with NewtonOS, links


/*
 Einstein threads:

 - Mainthread: GUI
    The main thread at startup runs all code that creates windows, destroys windows,
    or handles user interaction. In FLTK, callbacks are always running in the main
    thread. If other threads need to call FLTK, the you should do it by guarding
    the call with `Fl::lock(); ... ; Fl::unlocka();` and possibly `myWidget->redraw();`

- Emulator/Monitor thread:
    The Emulator runs inside its own thread. If the Monitor is enabled, the Emulator
    runs within the Monitor thread.

 - Interrupt Manager:
    The interrupt manager runs within its own thread.

 - UsermodeNetwork thread:
    Parts of the usermode network driver run within a thread.

 - TCP Serial Port:
    All TCP communications runs within a thread

 */

#include <K/Defines/KDefinitions.h>
#include "TFLApp.h"
#include "TFLAppUI.h"
#include "TFLAppWindow.h"
#include "TFLSettingsUI.h"

// ANSI C & POSIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

// C++14
#include <thread>

// FLTK user interface
#include <FL/x.H>
#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Tooltip.H>

// Einstein
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/Sound/TNullSoundManager.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Network/TUsermodeNetwork.h"
#include "Emulator/Screen/TFLScreenManager.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/Log/TFileLog.h"
#include "Emulator/Log/TBufferLog.h"
#include "Emulator/Serial/TSerialPorts.h"
#include "Emulator/Serial/TSerialPortManager.h"
#include "Emulator/Serial/TTcpClientSerialPortManager.h"

// Http Client GET
#undef min
#include "HTTPRequest.hpp"

// Additional managers for every supported platform
#if TARGET_OS_WIN32
#include "Emulator/Sound/TWaveSoundManager.h"
#elif TARGET_OS_LINUX
#include "Emulator/Sound/TPulseAudioSoundManager.h"
#elif TARGET_OS_MAC
#include "Emulator/Sound/TCoreAudioSoundManager.h"
#else
#error Selected target OS not implemented, or no target OS selected
#endif

// Monitor system for debugging ARM code
#include "Monitor/TFLMonitor.h"
#include "Monitor/TSymbolList.h"


// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// Local Classes
// -------------------------------------------------------------------------- //

TFLApp *gApp = nullptr;


// MARK: - Public: -
// --- Constructor and destructor


/**
 Constructor for the app.
 */
TFLApp::TFLApp( void ) = default;


/**
 Clean up time.
 */
TFLApp::~TFLApp( void )
{
    delete mEmulator;
    delete mScreenManager;
    delete mSoundManager;
    delete mLog;
    delete mROMImage;
    delete mMonitor;
    delete mSymbolList;
}


// MARK: -
// --- Startup and run the emulator.


/**
 Run EInstein.

 \todo Must urgently refactor this so it becomes readable again.
 */
void
TFLApp::Run( int argc, char* argv[] )
{
    mProgramName = argv[0];

    InitFLTK(argc, argv);

    InitSettings();

#if 0
    mFLSettings->useMonitor = 1;
    mLog = new TBufferLog();
#else
    mFLSettings->useMonitor = 0;
    mLog = new TFileLog("/tmp/Einstein_log.txt");
#endif

    int ramSize = mFLSettings->RAMSize;
    bool hidemouse = (bool)mFLSettings->hideMouse;

    (void) ::printf( "Welcome to Einstein console.\n" );
    (void) ::printf( "This is %s.\n", VERSION_STRING );

    static char theROMImagePath[FL_PATH_MAX];

    for (bool firstAttempt=true;;firstAttempt=false) {
        if (!firstAttempt || !mFLSettings->dontShow)
            mFLSettings->ShowSettingsPanelModal();
        strncpy(theROMImagePath, mFLSettings->ROMPath, FL_PATH_MAX);
        mROMImage = TROMImage::LoadROMAndREX(theROMImagePath, mFLSettings->useMonitor, mFLSettings->mUseBuiltinRex);
        if (!mROMImage) {
            fl_alert("Can't load ROM image.\nFile format not supported.");
            continue;
        }
        if (mROMImage->GetErrorCode()==TROMImage::kNoError)
            break;
        switch (mROMImage->GetErrorCode()) {
            case TROMImage::kErrorLoadingROMFile:
                fl_alert("Can't load ROM file\n%s\n%s", theROMImagePath, strerror(errno));
                break;
            case TROMImage::kErrorLoadingNewtonREXFile:
                fl_alert("Can't load Newton REX file\n%s\n%s", theROMImagePath, strerror(errno));
                break;
            case TROMImage::kErrorLoadingEinsteinREXFile:
                fl_alert("Can't load Einstein REX file\n%s\n%s", theROMImagePath, strerror(errno));
                break;
            case TROMImage::kErrorWrongSize:
                fl_alert("Can't load ROM file\n%s\nUnexpected file size.", theROMImagePath);
                break;
        }
        delete mROMImage;
        // go back to showing the settings panel
    }

    const char *theFlashPath = strdup(mFLSettings->FlashPath);

    InitScreen();

    InitSound();
    
    InitNetwork();

    mEmulator = new TEmulator(mLog, mROMImage, theFlashPath,
                              mSoundManager, mScreenManager, mNetworkManager, ramSize << 16 );
    mPlatformManager = mEmulator->GetPlatformManager();

    // yes, this is valid C++ code; it tells the emulator to call us so we can FLTK to
    // call us again later from the main thread which then closes all windows, terminating
    // the main application loop which then terminates the thread that called us to begin with.
    // Or as Mony says: "Would That It Were So Simple"
    //mEmulator->CallOnQuit([](){Fl::awake([](void*){gApp->UserActionQuit();});});

    InitSerialPorts(); // do this after creating the emulator

    InitMonitor(theROMImagePath);
    if (mMonitor)
        mMonitor->RunOnStartup(true);

    Fl::lock();
    wAppWindow->show(1, argv);
    StoreAppWindowSize();
    if (hidemouse) {
        wAppWindow->HideMousePointer();
    }

    // launch the actual emulation in the background
    auto emulatorThread = new std::thread(&TFLApp::EmulatorThreadEntry, this);

    // run the user interface until all windows are close
    Fl::run();

    // if the emulator does not know yet, tell it to wrap things up and quit
    mEmulator->Quit();

    // also, let the Monitor know that we are leaving
    if (mMonitor)
        mMonitor->Stop();

    // This is a good time to save preferences that might have changed while running
    if (wAppWindow->fullscreen_active()) {
        mFLSettings->mAppWindowPosX = mWindowedX;
        mFLSettings->mAppWindowPosY = mWindowedY;
    } else {
        mFLSettings->mAppWindowPosX = wAppWindow->x();
        mFLSettings->mAppWindowPosY = wAppWindow->y();
    }
    mFLSettings->savePreferences();

    // wait for the emulator to finish before we leave the house, too and lock the doors
    emulatorThread->join();
}


// MARK: -
// --- User Actions


/**
 User wants to quit the emulator and leave the app.
 */
void TFLApp::UserActionQuit()
{
    // tell the emulator to shut everything down
    if ( mEmulator ) {
        mEmulator->Quit();
    }
    // closing all windows will end Fl::run();
    Fl_Window *w;
    while ( (w=Fl::first_window()) ) {
        w->hide();
    }
    // afte Fl::run() if finished, TFLApp waits for the emulator
    // process to finish as well.
}


/**
 User wants us to toggle the power switch.
 \fixme This is currently the same as Menu Quit
 */
void TFLApp::UserActionTogglePower()
{
    mPlatformManager->SendPowerSwitchEvent();
}


/**
 User wants us to toggle the backlight.
 */
void TFLApp::UserActionToggleBacklight()
{
    mPlatformManager->SendBacklightEvent();
}


/**
 User wants us to slide the network card in or out
 */
void TFLApp::UserActionToggleNetworkCard()
{
    mPlatformManager->SendNetworkCardEvent();
}


/**
 Install a package from a file.

 \param filenames This is a list of filenames, separated by \n.

 \todo Support packages that are compressed in the common old compression formats .hqx, .sit, .zip, .sit.hqx, .sae(?)
 \todo grab apckages from the net using https://github.com/elnormous/HTTPRequest
 */
void TFLApp::InstallPackagesFromURI(const char *filenames)
{
    // bail early if the filenames are emoty
    if (!filenames || !*filenames) return;

    // TODO: do we have to handle backslashes on MSWindows?

    // Check the filename extension! Do that in the event handler too.
    char *fName = strdup(filenames);

    // grab the start of the first filename
    char *fn = fName;
    for (;;) {
        // find the start of the next filename
        char *nl = strchr(fn, '\n');
        if (nl) *nl = 0;

        // remove all the %nn encoding and insert the corresponding characters
        fl_decode_uri(fn);

        // On macOS, dropped files are just absolute BSD paths, starting with a '/'
        // On MSWindows, dropped files are absolute file paths, starting with 'C:\' or anothor drive name
        // URLs start with http://, for example "http://www.unna.org/unna/games/Pyramid/Pyramid.pkg"
        if (strncmp(fn, "http://", 7)==0 || strncmp(fn, "https://", 8) == 0) {
            if (strcasecmp(fl_filename_ext(fn), ".pkg")==0) {
                try {
                    http::Request request(fn);
                    const http::Response response = request.send("GET");
                    const KUInt8 *package = reinterpret_cast<const KUInt8*>(response.body.data());
                    KUInt32 packageSize = static_cast<KUInt32>(response.body.size());
                    if (memcmp(package, "package", 7)!=0) {
                        fl_message("Can't install\n%s\nThis is not a Newton package.", fn);
                    } else {
                        mPlatformManager->InstallPackage(package, packageSize);
                    }
                }
                catch (const std::exception& e)
                {
                    fl_message("Can't install\n%s\n%s", fn, e.what());
                }
            } else {
                fl_message("Can't install\n%s\nNetwork download supports only .pkg files.", fn);
            }
        } else {
            // install the package
            mPlatformManager->InstallPackage(fn);
        }

        // if there is another filename, loop around
        if (nl)
            fn = nl+1;
        else
            break;
    }
    free(fName);
}


/**
 User asks Einstein to install a package.

 We open a file chooser dialog and then take the package selected and push it to the emulator.

 \todo Use the system native file chooser!
 */
void TFLApp::UserActionInstallPackage()
{
    static char *filename = 0L;

    const char *newname = nullptr;
    Fl_Native_File_Chooser fnfc;
    fnfc.title("Install Package...");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_MULTI_FILE);
    fnfc.filter("Package\t*.pkg"); // "Compressed Package\t*.{sit,sae,hqx,zip,sit.hqx,hqx.sit}");
    //fnfc.directory("/var/tmp");   // FIXME: save this in the preferences
    switch ( fnfc.show() ) {
        case -1: return; // Error text is in fnfc.errmsg()
        case  1: return; // user canceled
    }
    for (int i=0; i<fnfc.count(); i++) {
        newname = fnfc.filename(i);
        if (newname && *newname) {
            if (!filename)
                filename = (char*)calloc(FL_PATH_MAX, 1);
            strncpy(filename, newname, FL_PATH_MAX);
            filename[FL_PATH_MAX] = 0;
            mPlatformManager->InstallPackage(filename);
        }
    }
}


/**
 User wants to see the About window.

 \todo The About WIndow is not very beautilf. We should add credits and clickable links
 to give the user complete information on teh project. We should also provide version
 information for teh REx and maybe otehr interfaces.
 */
void TFLApp::UserActionShowAboutPanel()
{
    mFLSettings->ShowAboutDialog();
}


/**
 User wants to see the setting window.
 */
void TFLApp::UserActionShowSettingsPanel()
{
    mFLSettings->ShowSettingsPanel();
}


/**
 User wants the screen to be the original scale, so resize it back to 1:1; mapping.

 Depending on the preferences, we would leave fullscreen mode.
 */
void TFLApp::UserActionOriginalScreenSize()
{
    if (wAppWindow->fullscreen_active()) {
        UserActionToggleFullscreen();
    }
    if (!wAppWindow->fullscreen_active()) {
        wAppWindow->resizable(mNewtonScreen);
        wAppWindow->size( mScreenManager->GetScreenWidth(), wMenubar->h() + wToolbar->h() + mScreenManager->GetScreenHeight() );
        if (!mFLSettings->mAllowScreenResize)
            wAppWindow->resizable(nullptr);
        StoreAppWindowSize();
    }
}


/**
 User wants the app into or out of fullscreen mode.

 Remove the decoration from the main window and set it into fullscreen mode.
 Switch the menubar and the toolbar off.
 Scale the pixel output to the Newton screen.
 Rotating the screen should actually rotate it upside down and left-sdie right in fullscreen mode.
 */
void TFLApp::UserActionToggleFullscreen()
{
    if (wAppWindow->fullscreen_active()) {
        wAppWindow->resizable(mNewtonScreen);
        wAppWindow->fullscreen_off(mWindowedX, mWindowedY, mWindowedWidth, mWindowedHeight);
        if (!mFLSettings->mAllowScreenResize)
            wAppWindow->resizable(nullptr);
    } else {
        StoreAppWindowSize();
        wAppWindow->resizable(mNewtonScreen);
        wAppWindow->fullscreen();
        if (!mFLSettings->mAllowScreenResize)
            wAppWindow->resizable(nullptr);
    }
}


/**
 Called by the user interface if the user chooses to reice the ROM via TCP.

 This is a lot of effort for something that can be done better using 'netcat' on Unixes.
 I am sure another tool exists on MSWindows. I am also sure that nobody ever uses
 this because I never got any feedback that it is not working.

 \todo we should remove ROM download support.
 */
void TFLApp::UserActionFetchROM()
{
    // not yet implemented
#if 0
    static Fl_Window *downloadDialog = 0L;
    if (!downloadDialog) {
        downloadDialog = createROMDownloadDialog();
        wDownloadIP3->value("192");
        wDownloadIP2->value("168");
        wDownloadIP1->value("0");
        wDownloadIP0->value("24");
        wDownloadPort->value("10080");
        char path[FL_PATH_MAX]; path[0] = 0;
        fl_filename_absolute(path, ".");
        strcat(path, "myROM");
        wDownloadPath->copy_label(path);
    }
    downloadDialog->show();
#endif
}


void TFLApp::UserActionPopupMenu()
{
    mFLSettings->HandlePopupMenu();
}


void TFLApp::UserActionToggleMonitor()
{
    if (mMonitor)
        mMonitor->Show();
}


// MARK: -
// ---  Events from within the meulator


/**
 This is called by the screen manager when the state of the backlight changed.
 */
void TFLApp::PowerChangedEvent(bool inState)
{
    // we have a hidden button in the FLuid file that does nothing but keep
    // track of the "on" image.
    static Fl_Image *onImage = nullptr;
    static Fl_Image *offImage = nullptr;
    Fl::lock();
    if (!onImage) {
        onImage = wPowerOnTool->image();
        offImage = wPowerTool->image();
    }
    if (inState) {
        wPowerTool->image(onImage);
    } else {
        wPowerTool->image(offImage);
    }
    wPowerTool->redraw();
    Fl::awake();
    Fl::unlock();
}


/**
 This is called by the screen manager when the state of the backlight changed.
 */
void TFLApp::BacklightChangedEvent(bool inState)
{
    // we have a hidden button in the FLuid file that does nothing but keep
    // track of the "on" image.
    static Fl_Image *onImage = nullptr;
    static Fl_Image *offImage = nullptr;
    Fl::lock();
    if (!onImage) {
        onImage = wBacklightOnTool->image();
        offImage = wBacklightTool->image();
    }
    if (inState) {
        wBacklightTool->image(onImage);
    } else {
        wBacklightTool->image(offImage);
    }
    wBacklightTool->redraw();
    Fl::awake();
    Fl::unlock();
}


void TFLApp::ResizeFromNewton(int w, int h)
{
    if (mNewtonScreen->w()==w && mNewtonScreen->h()==h)
        return;
    Fl::lock();
    wAppWindow->resizable(mNewtonScreen);
    int dw = w - mNewtonScreen->w();
    int dh = h - mNewtonScreen->h();
    wAppWindow->size( wAppWindow->w() + dw, wAppWindow->h() + dh );
    if (!mFLSettings->mAllowScreenResize)
        wAppWindow->resizable(nullptr);
    Fl::unlock();
}


// MARK: - Private: -


void TFLApp::InitFLTK(int argc, char **argv) {
    Fl::scheme("gtk+");
    Fl::args(1, argv);
    Fl::get_system_colors();
    Fl::use_high_res_GL(1);
    Fl::visual(FL_RGB);
    Fl_Tooltip::size(12);
}


void TFLApp::InitSettings() {
    mFLSettings = new TFLSettingsUI();
#if TARGET_OS_WIN32
    mFLSettings->mSettingsPanel->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#endif
    mFLSettings->setApp(this, mProgramName);
    mFLSettings->loadPreferences();
    mFLSettings->revertDialog();
}


void TFLApp::InitSound()
{
#if TARGET_OS_WIN32
    mSoundManager = new TWaveSoundManager( mLog );
#elif TARGET_OS_LINUX
    mSoundManager = new TPulseAudioSoundManager( mLog );
#elif TARGET_OS_MAC
    mSoundManager = new TCoreAudioSoundManager( mLog );
#else
#   error Selected target OS support not implemented, or no target OS selected
#endif
}


void TFLApp::InitNetwork() {
#if TARGET_OS_MAC || TARGET_OS_LINUX || TARGET_OS_WIN32
    mNetworkManager = new TUsermodeNetwork(mLog);
#else
#   warn Please configure a network driver
    mNetworkManager = new TNullNetworkManager(mLog);
#endif
}


/**
 Initialize the application window and the screen driver.
 */
void TFLApp::InitScreen()
{
    int portraitWidth = mFLSettings->screenWidth;
    int portraitHeight = mFLSettings->screenHeight;
    if (portraitHeight < portraitWidth)
    {
        (void) ::fprintf(
                         stderr,
                         "Warning, (portrait) height (%i) is smaller than width (%i). Boot screen won't be displayed properly\n",
                         portraitHeight,
                         portraitWidth );
    }

    Fl_Group::current(nullptr);
    wAppWindow = CreateApplicationWindow(
                                         mFLSettings->mAppWindowPosX,
                                         mFLSettings->mAppWindowPosY);
    wAppWindow->SetApp( this );
    wAppWindow->size(portraitWidth, portraitHeight + wToolbar->y() + wToolbar->h());
    wAppWindow->resizable(nullptr);
#if TARGET_OS_WIN32
    wAppWindow->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#endif
    wAppWindow->callback(quit_cb, this);
    if (mFLSettings->mAllowFullscreen)
        wMenuItemFullscreen->activate();
    else
        wMenuItemFullscreen->deactivate();
    wAppWindow->begin();
    TFLScreenManager *flScreenManager = new TFLScreenManager(this, mLog, portraitWidth, portraitHeight, false, false);
    mNewtonScreen = flScreenManager->GetWidget();
    mScreenManager = flScreenManager;
    flScreenManager->GetWidget()->position(wToolbar->x(), wToolbar->y()+wToolbar->h());
    wAppWindow->end();
    if (mFLSettings->mAllowScreenResize)
        wAppWindow->resizable(flScreenManager->GetWidget());
    else
        wAppWindow->resizable(nullptr);
    StoreAppWindowSize();
}


void TFLApp::InitSerialPorts()
{
    // TODO: add preferences for the current driver, port and server address
    // Basic initialization of all serial ports

    mEmulator->SerialPorts.Initialize(TSerialPorts::kTcpClientDriver,
                                      TSerialPorts::kNullDriver,
                                      TSerialPorts::kNullDriver,
                                      TSerialPorts::kNullDriver );
#if 0
    // TODO: save the serial port setting in a save place
    TSerialPortManager *extr = mEmulator->SerialPorts.GetDriverFor(TSerialPorts::kExtr);
    if (extr && extr->GetID()==TSerialPorts::kTcpClientDriver)
    {
        TTcpClientSerialPortManager *tcp = (TTcpClientSerialPortManager*)extr;
        tcp->SetServerAddress([[defaults stringForKey: kExtrTCPServerAddress] UTF8String]);
        tcp->SetServerPort((int)[defaults integerForKey: kExtrTCPServerPort]);
    }
    mEmulator->SerialPorts.PortChangedCallback(
                                               // THIS IS A LAMBDA FUNCTION. This function is called when an application
                                               // on the emulated Newton changes the serial port settings
                                               [self](int serPort)->void
                                               {
        if (serPort==TSerialPorts::kExtr) {
            TSerialPortManager *extr = mEmulator->SerialPorts.GetDriverFor(TSerialPorts::kExtr);
            if (extr && extr->GetID()==TSerialPorts::kTcpClientDriver) {
                TTcpClientSerialPortManager *tcp = (TTcpClientSerialPortManager*)extr;

                char *tcpServer = tcp->GetServerAddressDup();
                NSString *nsTcpServer = [NSString stringWithUTF8String:tcpServer];
                [[mUserDefaultsController defaults] setValue:nsTcpServer forKey:kExtrTCPServerAddress];
                ::free(tcpServer);

                int tcpPort = tcp->GetServerPort();
                [[mUserDefaultsController defaults] setInteger:tcpPort forKey:kExtrTCPServerPort];
            }
        }
    }
                                               );
#endif
}


void TFLApp::InitMonitor(const char *theROMImagePath)
{
    bool useMonitor = (bool)mFLSettings->useMonitor;
    if (useMonitor)
    {
        char theSymbolListPath[FL_PATH_MAX];
        strncpy(theSymbolListPath, theROMImagePath, FL_PATH_MAX);
        fl_filename_setext(theSymbolListPath, FL_PATH_MAX, ".symbols");
        mSymbolList = new TSymbolList( theSymbolListPath );
        mMonitor = new TFLMonitor( (TBufferLog*) mLog, mEmulator, mSymbolList, theROMImagePath);
    } else {
        (void) ::printf( "Booting...\n" );
    }
}


/**
 Launch the emulator or monitor thread.
 */
void
TFLApp::EmulatorThreadEntry()
{
    if (mMonitor) {
        mMonitor->Run();
    } else {
        mEmulator->Run();
    }
    // wake up the FLTK mainloop and have it call GUI Quit.
    Fl::awake([](void*){gApp->UserActionQuit();});
}


/**
 Create a file for logging all important events at runtime.
 */
void
TFLApp::CreateLog( const char* inFilePath )
{
    if (mLog)
    {
        (void) ::printf( "A log already exists (--monitor & --log are exclusive)\n" );
        ::exit(1);
    }
    mLog = new TFileLog( inFilePath );
}


/**
 Create the appropriate screen manager for this platform.

 In FLTK world, that would always be the FLTK Screen driver.

 \todo do we have to do any fullscreen management here? RaspberryPI? Linux tablets? Pen PCs?
 */
void TFLApp::CreateScreenManager(
                                 const char* inClass,
                                 int inPortraitWidth,
                                 int inPortraitHeight,
                                 bool inFullScreen)
{	
    if (::strcmp( inClass, "FL" ) == 0)
    {
        bool screenIsLandscape = true;

        KUInt32 theWidth;
        KUInt32 theHeight;

#if 0
        if (inFullScreen)
        {
            KUInt32 theScreenWidth;
            KUInt32 theScreenHeight;
            TFLScreenManager::GetScreenSize(&theScreenWidth, &theScreenHeight);
            if (theScreenWidth >= theScreenHeight)
            {
                screenIsLandscape = true;
                theWidth = inPortraitHeight;
                theHeight = inPortraitWidth;
            } else {
                screenIsLandscape = false;
                theWidth = inPortraitWidth;
                theHeight = inPortraitHeight;
            }
        } else {
            theWidth = inPortraitWidth;
            theHeight = inPortraitHeight;
        }
#else
        theWidth = inPortraitWidth;
        theHeight = inPortraitHeight;
#endif

        mScreenManager = new TFLScreenManager(this,
                                              mLog,
                                              theWidth,
                                              theHeight,
                                              inFullScreen,
                                              screenIsLandscape);
    } else {
        (void) ::fprintf( stderr, "Unknown screen manager class %s\n", inClass );
        ::exit( 1 );
    }
}


/**
 User wants us to quit.

 This may be a menu item or the Cllose button on the window decoration.
 */
void TFLApp::quit_cb(Fl_Widget *, void *p) 
{
    gApp->UserActionQuit();
}


/**
 Store the current size of the app window in mWindowed... .
 */
void TFLApp::StoreAppWindowSize()
{
    mWindowedX = wAppWindow->x();
    mWindowedY = wAppWindow->y();
    mWindowedWidth = wAppWindow->w();
    mWindowedHeight = wAppWindow->h();
}


#include "Emulator/JIT/Generic/TJITGenericROMPatch.h"
#include "Emulator/JIT/Generic/TJITGeneric_Macros.h"

/* FIXME: this nice idea works well on MP2100US, but not at all on eMates. How come? MP2100D not yet tested. */
T_ROM_INJECTION(0x001B37FC, kROMPatchVoid, 0x001A1650, "AddClipboard__9TRootViewFRC6RefVarT1")
{
//    fprintf(stderr, "AddClipboard__9TRootViewFRC6RefVarT1\n");
    // r0 is a pointer to TRootView
    TNewt::RefArg a = TNewt::RefVar::FromPtr(ioCPU->GetRegister(1));
    //TNewt::RefArg b = TNewt::RefVar::FromPtr(ioCPU->GetRegister(2));

    NewtRef data = TNewt::GetFrameSlot(a, TNewt::MakeSymbol("data"));
//    TNewt::PrintRef(data, 8);
    if (!TNewt::RefIsArray(data)) return ioUnit; // expected an array
    int nData = (int)TNewt::RefArrayGetNumSlots(data);

    std::string clipboardText = "";
    bool firstText = true;
    for (int i=0; i<nData; i++) {
        NewtRef dataSet = TNewt::RefArrayGetSlot(data, i);
//        TNewt::PrintRef(dataSet, 8);
        if (!TNewt::RefIsArray(dataSet)) continue;
        int nDataSet = (int)TNewt::RefArrayGetNumSlots(dataSet);
        for (int j=0; j<nDataSet; j++) {
            NewtRef textRec = TNewt::RefArrayGetSlot(dataSet, j);
//            TNewt::PrintRef(textRec, 8);
            if (!TNewt::RefIsFrame(textRec)) continue;
            NewtRef textRef = TNewt::GetFrameSlot(textRec, TNewt::MakeSymbol("text"));
//            TNewt::PrintRef(textRef, 8);
            if (TNewt::RefIsString(textRef)) {
                int textLen = TNewt::RefStringLength(textRef);
                char *text = (char*)malloc(textLen+1);
                TNewt::RefToString(textRef, text, textLen);
                if (firstText)
                    firstText = false;
                else
                    clipboardText.append("\n");
                clipboardText.append(text);
                free(text);
                break;
            }
        }
    }
    const char *cstring = clipboardText.c_str();
    Fl::copy(cstring, strlen(cstring), 1);
    return ioUnit;
}

/**
 This is the first function that is called on all platforms.

 We use some static initialisation throughout the code that will be called before this function
 is ever reached. Also, different platforms have different entry points (MS Windows for example calls
 WinMain() first). FLTK makes sure that main() is called soon after.
 */
int main(int argc, char** argv )
{
    TFLApp theApp;
    gApp = &theApp;
    theApp.Run( argc, argv );
    return 0;
}


// ======================================================================= //
// We build our computer (systems) the way we build our cities: over time, 
// without a plan, on top of ruins.
//   -- Ellen Ullman
// ======================================================================= //
