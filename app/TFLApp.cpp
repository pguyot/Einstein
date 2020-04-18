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

/*

 Additions:
 File -> New, Open, Save, Save As..., Recent List could provide access to many
            Falsh files, and switching between Flash files and rebooting
 Platform -> should have buttons for hard and soft reset
 Help -> should link to some pages, even if it is just GitHub
 Window -> Dump Rom shou;d probably be called Fetch ROM
            I would like to support a Toolbox window here as well
 Clipboard icon into the toolbar? Drag'n'drop of text in general?

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

// Einstein
#include "Emulator/ROM/TROMImage.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/Sound/TNullSoundManager.h"
#include "Emulator/Network/TNetworkManager.h"
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

// Monito system for debugging ARM code
#include "Monitor/TMonitor.h"
#include "Monitor/TSymbolList.h"


// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
// Local Classes
// -------------------------------------------------------------------------- //


TFLApp *gApp = nullptr;


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


static void draw_ramp(int x, int y, int w, int h, Fl_Color c) {
    for (int i=y; i<y+h; i++) {
        fl_color(fl_color_average(FL_BACKGROUND_COLOR, c, i/100.0));
        //fl_rectf(x, y, w, h, Fl::box_color(c));
        fl_xyline(x, i, x+w);
    }
}

/**
 Run EInstein.

 \todo Must urgently refactor this so it becomes readable again.
 */
void
TFLApp::Run( int argc, char* argv[] )
{
    mProgramName = argv[0];

    Fl::scheme("gtk+");
    Fl::args(1, argv);
    Fl::get_system_colors();
    Fl::use_high_res_GL(1);
    Fl::set_boxtype(FL_FREE_BOXTYPE, draw_ramp, 0, 0, 0, 0);
    Fl::set_boxtype((Fl_Boxtype)(FL_FREE_BOXTYPE+1), draw_ramp, 0, 0, 0, 0);

    mFLSettings = new TFLSettingsUI();
#if TARGET_OS_WIN32
    mFLSettingsDialog->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#endif
    mFLSettings->setApp(this, mProgramName);
    mFLSettings->loadPreferences();
    mFLSettings->revertDialog();

    //flSettings->dontShow = false;
    if (!mFLSettings->dontShow) {
        mFLSettings->ShowSettingsPanelModal();
//        mFLSettingsDialog->show(1, argv);
//        while (mFLSettingsDialog->visible())
//            Fl::wait();
    }

    const char* defaultMachineString = "717006";
    int theMachineID = mFLSettings->machine;
    const char* theMachineString = strdup(mFLSettings->MachineID);
    const char* theScreenManagerClass = nil;
    const char *theROMImagePath = strdup(mFLSettings->ROMPath);
    const char *theFlashPath = strdup(mFLSettings->FlashPath);
    int portraitWidth = mFLSettings->screenWidth;
    int portraitHeight = mFLSettings->screenHeight;
    int ramSize = mFLSettings->RAMSize;
    bool fullscreen = (bool)mFLSettings->fullScreen;
    bool hidemouse = (bool)mFLSettings->hideMouse;
    bool useMonitor = (bool)mFLSettings->useMonitor;
    int useAIFROMFile = 0;	// 0 uses flat rom, 1 uses .aif/.rex naming, 2 uses Cirrus naming scheme

    int xx, yy, ww, hh;
    if (fullscreen) {
        Fl::screen_xywh(xx, yy, ww, hh, 0);
        portraitWidth = ww;
        portraitHeight = hh;
    }

    if (portraitHeight < portraitWidth)
    {
        (void) ::fprintf(
                         stderr,
                         "Warning, (portrait) height (%i) is smaller than width (%i). Boot screen won't be displayed properly\n",
                         portraitHeight,
                         portraitWidth );
    }

    (void) ::printf( "Welcome to Einstein console.\n" );
    (void) ::printf( "This is %s.\n", VERSION_STRING );

    Fl_Group::current(nullptr);
    TFLAppWindow *win = CreateApplicationWindow();
    win->SetApp( this );
    win->size(portraitWidth, portraitHeight + wToolbox->y() + wToolbox->h());
    win->resizable(nullptr);
#if TARGET_OS_WIN32
    win->icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
#endif
    win->callback(quit_cb, this);
    win->begin();
    TFLScreenManager *flScreenManager = new TFLScreenManager(this, mLog, portraitWidth, portraitHeight, false, false);
    mScreenManager = flScreenManager;
    flScreenManager->GetWidget()->position(wToolbox->x(), wToolbox->y()+wToolbox->h());
    win->end();

#if TARGET_OS_WIN32
    mSoundManager = new TWaveSoundManager( mLog );
#elif TARGET_OS_LINUX
    mSoundManager = new TPulseAudioSoundManager( mLog );
#elif TARGET_OS_MAC
    mSoundManager = new TCoreAudioSoundManager( mLog );
#else
#   error Selected target OS support not implemented, or no target OS selected
#endif

    mNetworkManager = new TNullNetworkManager(mLog);
    if (theMachineString == nil)
    {
        theMachineString = defaultMachineString;
    }

    // will we use an AIF image?
    {
        const char *ext = fl_filename_ext(theROMImagePath);
#if TARGET_OS_WIN32
        if ( ext && stricmp(ext, ".aif")==0 )
        {
            useAIFROMFile = 1;
        }
#else
        if ( ext && strcasecmp(ext, ".aif")==0 )
        {
            useAIFROMFile = 1;
        }
#endif
        const char *name = fl_filename_name(theROMImagePath);
        if (	name
            && strncmp(name, "Senior Cirrus", 13)==0
            && strstr(name, "image"))
        {
            useAIFROMFile = 2;
        }
    }
    char theREX1Path[FL_PATH_MAX];
    strcpy(theREX1Path, theROMImagePath);
    char *rexName = (char*)fl_filename_name(theREX1Path);
    if (rexName) {
        strcpy(rexName, "Einstein.rex");
    }

    switch (useAIFROMFile) {
        case 0:
            mROMImage = new TFlatROMImageWithREX(theROMImagePath, theREX1Path, theMachineString, useMonitor);
            break;
        case 1:
        {
            char theREX0Path[FL_PATH_MAX];
            strcpy(theREX0Path, theROMImagePath);
            fl_filename_setext(theREX0Path, FL_PATH_MAX, ".rex");
            mROMImage = new TAIFROMImageWithREXes(theROMImagePath, theREX0Path, theREX1Path, theMachineString, useMonitor );
        }
            break;
        case 2:
        {
            char theREX0Path[FL_PATH_MAX];
            strcpy(theREX0Path, theROMImagePath);
            char *image = strstr(theREX0Path, "image");
            strcpy(image, "high");
            mROMImage = new TAIFROMImageWithREXes(theROMImagePath, theREX0Path, theREX1Path, theMachineString, useMonitor );
        }
            break;
    }

    mEmulator = new TEmulator(
                              mLog, mROMImage, theFlashPath,
                              mSoundManager, mScreenManager, mNetworkManager, ramSize << 16 );
    mPlatformManager = mEmulator->GetPlatformManager();

    // TODO: add preferences for the current driver, port and server address
    // Basic initialization of all serial ports
    mEmulator->SerialPorts.Initialize(TSerialPorts::kTcpClientDriver,
                                      TSerialPorts::kNullDriver,
                                      TSerialPorts::kNullDriver,
                                      TSerialPorts::kNullDriver );
#if 0
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


    if (useMonitor)
    {
        char theSymbolListPath[512];
        (void) ::snprintf( theSymbolListPath, 512, "%s/%s.symbols",
                          theROMImagePath, theMachineString );
        mSymbolList = new TSymbolList( theSymbolListPath );
        mMonitor = new TMonitor( (TBufferLog*) mLog, mEmulator, mSymbolList, theROMImagePath);
    } else {
        (void) ::printf( "Booting...\n" );
    }

    Fl::lock();
    win->show(1, argv);
    if (hidemouse) {
        win->HideMousePointer();
    }

    // launch the actual emulation in the background
    auto emulatorThread = new std::thread(&TFLApp::EmulatorThreadEntry, this);

    // run the user interface untill all windows are close
    Fl::run();

    // if the emulator does not know yet, tell it to wrap things up and quit
    mEmulator->Quit();

    // TODO: this would be a great time to save preferences that might have changed while running

    // wait for the emulator to finish before we leave the house, too and lock the doors
    // FIXME: we must have a timeout on this!
    emulatorThread->join();
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
 Creta ethe appropriate screen manager for this platform.

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
 User wants us to slide the network card in or out
 */
void TFLApp::UserActionToggleNetworkCard()
{
    mPlatformManager->SendNetworkCardEvent();
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
 User wants us to toggle the backlight.
 */
void TFLApp::UserActionToggleBacklight()
{
    mPlatformManager->SendBacklightEvent();
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


/**
 Install a package from a file.

 \param filenames This is a list of filenames, separated by \n.

 \todo Support packages that are compressed in the common old compression formats .hqx, .sit, .zip, .sit.hqx, .sae(?)
 */
void TFLApp::InstallPackagesFromURI(const char *filenames)
{
    // bail early if the filenames are emoty
    if (!filenames || !*filenames) return;

    // TODO: do we have to handle backslashes on MSWindows?

    // Check the filename extension! Do that in the event handler too.
    char *fName = strdup(filenames);

    // remove all the %nn encoding and insert the corresponding characters
    fl_decode_uri(fName);

    // grab the start of the first filename
    char *fn = fName;
    for (;;) {
        // find the start of the next filename
        char *nl = strchr(fn, '\n');
        if (nl) *nl = 0;

        // on some platforms, the filename starts with "file://" or other prefixes, so remove them
        char *prefix = fn;
        for (;;) {
            KUInt8 p = *prefix;
            if (p==0 || p>=0x80 || (!isalnum(p) && p!='_')) {
                if (strncmp(prefix, "://", 3)==0)
                    fn = prefix + 3;
                break;
            }
            prefix++;
        }

        // install the package
        mPlatformManager->InstallPackage(fn);

        // if there is another filename, loop around
        if (nl)
            fn = nl+1;
        else
            break;
    }
    free(fName);
}


/**
 User asks EIntein to install a package.

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
