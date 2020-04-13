// ==============================
// File:			TX11ScreenManager.cp
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
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
#include "TX11ScreenManager.h"
#include <Emulator/TMemory.h>
#include <TEmulator.h>

// X11 interface
#if __QUICKDRAW__
namespace X11NameSpace {
#endif
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
// I'll see later for SHM
// #include <X11/extensions/XShm.h>
#if __QUICKDRAW__
};

using namespace X11NameSpace;
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

// I'll see later for SHM
// #include <sys/ipc.h>
// #include <sys/shm.h>

// K
#include <K/Defines/UByteSex.h>
#include <K/Threads/TThread.h>

// Einstein
#include "Emulator/Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

#define kBorderWidth		0

// -------------------------------------------------------------------------- //
//  * GetDisplaySize( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::GetScreenSize(
			KUInt32* outWidth,
			KUInt32* outHeight)
{
	X11Prefix Display* theDisplay;
	theDisplay = DoOpenDisplay();
	int theScreenNumber = DefaultScreen(theDisplay);
	*outWidth = (KUInt32) DisplayWidth(theDisplay, theScreenNumber);
	*outHeight = (KUInt32) DisplayHeight(theDisplay, theScreenNumber) - 50 /* debug */;
	DoCloseDisplay(theDisplay);
}

// -------------------------------------------------------------------------- //
//  * TX11ScreenManager( TLog* )
// -------------------------------------------------------------------------- //
TX11ScreenManager::TX11ScreenManager(
			TLog* inLog /* = nil */,
			KUInt32 inPortraitWidth /* = kDefaultPortraitWidth */,
			KUInt32 inPortraitHeight /* = kDefaultPortraitHeight */,
			bool inFullScreen /* = false */,
			bool inScreenIsLandscape /* = true */)
	:
		TScreenManager(
			inLog,
			inPortraitWidth,
			inPortraitHeight,
			inFullScreen,
			inScreenIsLandscape ),
		mWindow( 0 ),
		mThread( nil ),
		mPowerIsOn( false ),
		mImage( NULL ),
		mBitsPerPixel( 0 ),
		mDepth( 0 ),
		mAllocColor( false ),
		mImageBuffer( NULL )
{
	// I'm doing things in a multi-threaded way.
	(void) XInitThreads();
	
	// Connect to the display.
	OpenDisplay();
	
	// Create some atoms.
	mWMDeleteWindow = XInternAtom( mDisplay, "WM_DELETE_WINDOW", false );
	mWMState = XInternAtom(mDisplay, "_NET_WM_STATE", false);
	mWMFullScreen = XInternAtom( mDisplay, "_NET_WM_STATE_FULLSCREEN", false);	

	// Setup the translation map for key codes.
	LoadKeyCodesTranslation();
	
	// Find the visual.
	FindVisual();
	
	// Create and setup the window.
	SetupWindow();
	
	// Create the palette.
	CreatePalette();
}

// -------------------------------------------------------------------------- //
//  * ~TX11ScreenManager( void )
// -------------------------------------------------------------------------- //
TX11ScreenManager::~TX11ScreenManager( void )
{
	if (mPowerIsOn)
	{
		PowerOffScreen();
	}
	
	// Destroy the image.
	if (mImage)
	{
		mImage->data = NULL;
		XDestroyImage( mImage );
	}
	
	// Free the image buffer.
	if (mImageBuffer)
	{
		::free(mImageBuffer);
	}

	// Destroy the window.
	if (mWindow > 0)
	{
		XDestroyWindow( mDisplay, mWindow );
	}

	// Free the palette.
	FreePalette();

	// Close the connection to the display.
	CloseDisplay();

	if (mThread)
	{
		delete mThread;
	}
}

// -------------------------------------------------------------------------- //
//  * Run( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::Run( void )
{
	int displayFd = ConnectionNumber(mDisplay);
	struct timeval waitTime{};
	bool gotAnEvent = true;
	bool penIsDown = false;
	KUInt16 xcoord = 0;
	KUInt16 ycoord = 0;
	fd_set myFdSet;
	bool loop = true;
	
	while (loop) {
        XEvent theEvent;
        // If the pen is down, wait for the sample rate, otherwise,
        // wait forever.
        if (penIsDown) {
            // Select.
            FD_ZERO(&myFdSet);
            FD_SET(displayFd, &myFdSet);
            // 250 ns
            KUInt32 theSampleRateInTicks = GetTabletSampleRate();
            waitTime.tv_sec = theSampleRateInTicks / 4000000;
            waitTime.tv_usec = (theSampleRateInTicks / 4) % 1000000;
            if (::select(displayFd + 1, &myFdSet, NULL, NULL, &waitTime) > 0) {
                if (XPending(mDisplay)) {
                    XNextEvent(mDisplay, &theEvent);
                    gotAnEvent = true;
                }
            } else {
                // No event.
                gotAnEvent = false;
            }
        } else {
            XNextEvent(mDisplay, &theEvent);
            gotAnEvent = true;
        }

        if (gotAnEvent) {
            switch (theEvent.type) {
                case MotionNotify:
                    // Motion event (while button 1 is down)
                    // (maybe check coordinates)
                    xcoord = theEvent.xmotion.x;
                    ycoord = theEvent.xmotion.y;
                    PenDown(xcoord, ycoord);
                    break;
                case ButtonPress:
                    // (maybe check coordinates & button that was pressed)
                    xcoord = theEvent.xbutton.x;
                    ycoord = theEvent.xbutton.y;
                    PenDown(xcoord, ycoord);
                    penIsDown = true;
                    break;
                case ButtonRelease:
                    // (maybe check button that was released)
                    PenUp();
                    penIsDown = false;
                    break;

                case UnmapNotify:
                    loop = false;
                    break;

                case Expose:
                    // Redraw the bits that were lost.
                {
                    int theErr = XPutImage(
                            mDisplay,
                            mWindow,
                            mGC,
                            mImage,
                            theEvent.xexpose.x,
                            theEvent.xexpose.y,
                            theEvent.xexpose.x,
                            theEvent.xexpose.y,
                            theEvent.xexpose.width,
                            theEvent.xexpose.height
                    );
                    if (theErr != 0) {
                        (void) ::fprintf(
                                stderr,
                                "XPutImage returned an error (%i)\n",
                                theErr);
                        ::abort();
                    }
                }
                    break;

                case KeyPress: {
                    KeyDown(TranslateKeyCode(theEvent));
                }
                    break;

                case KeyRelease: {
                    KeyUp(TranslateKeyCode(theEvent));
                }
                    break;

                case ClientMessage:
                    // FIXME: this does not quit the app!
                    // We need to call X11ScreenManager::PowerOffScreen()
                    // Then mPlatform::Quit();
                    // and then we are still stuck in TCLIApp::AppMenuLoop oder MonitorMenuLoop
                    if (theEvent.xclient.data.l[0] == mWMDeleteWindow) {
                        GetMemory()->GetEmulator()->Quit();
                    }
                    break;
                default:
//					(void) ::printf( "Other event: type=%i\n", theEvent.type );
                    // Do nothing.
                    break;
            } // switch
        } else {
            // The pen is still down, generate a new message.
            PenDown(xcoord, ycoord);
        }
    }
}

// -------------------------------------------------------------------------- //
//  * OpenDisplay( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::OpenDisplay( void )
{
	mDisplay = DoOpenDisplay();
}

// -------------------------------------------------------------------------- //
//  * CloseDisplay( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::CloseDisplay( void )
{
	DoCloseDisplay(mDisplay);
}

// -------------------------------------------------------------------------- //
//  * DoOpenDisplay( void )
// -------------------------------------------------------------------------- //
X11Prefix Display*
TX11ScreenManager::DoOpenDisplay( void )
{
	X11Prefix Display* theDisplay = XOpenDisplay(NULL);
	if (theDisplay == NULL)
	{
		// Try again with :0.0
		theDisplay = XOpenDisplay(":0.0");
		if (theDisplay == NULL)
		{
			// Couldn't connect.
			// Abort.
			(void) ::fprintf( stderr, "Couldn't connect to X11 display.\n" );
			::abort();
		}
	}
	
	return theDisplay;
}

// -------------------------------------------------------------------------- //
//  * DoCloseDisplay( X11Prefix Display* )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::DoCloseDisplay( X11Prefix Display* inDisplay )
{
	XCloseDisplay(inDisplay);
}

// -------------------------------------------------------------------------- //
//  * SetupWindow( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::SetupWindow( void )
{
	// Grab the screen number.
	int theScreenNumber = DefaultScreen(mDisplay);
	
	// Create a simple window at 0,0

	// We won't inherit the colormap from the parent window.
	mRootWindow = RootWindow(mDisplay, theScreenNumber);
	Colormap cmap =
		XCreateColormap(mDisplay, mRootWindow, mVisual, AllocNone);
	XInstallColormap(mDisplay, cmap);

	XSetWindowAttributes theAttributes;
	memset(&theAttributes, 0, sizeof(theAttributes));
	theAttributes.colormap = cmap;
	theAttributes.background_pixel = 0; /* FIXME: this should be white */
	theAttributes.border_pixel     = 0;
	unsigned long mask = CWColormap | CWBorderPixel | CWBackPixel;
	
	int theWidth = GetActualScreenWidth();
	int theHeight = GetActualScreenHeight();
	
	mWindow = XCreateWindow(
				mDisplay,
				mRootWindow,
				0,
				0,
				theWidth,
				theHeight,
				kBorderWidth,
				mDepth, /* depth */
				InputOutput, /* class */
				mVisual, /* visual */
				mask, /* attribute mask */
				&theAttributes /* attributes */);

	if (mWindow < LastExtensionError)
	{
		(void) ::fprintf( stderr, "XCreateSimpleWindow returned an error: %i\n",
									(int) mWindow );
		::abort();
	}

	// Set its name (I won't set its icon yet)
	if (XStoreName( mDisplay, mWindow, "Einstein Platform" ) < 0)
	{
		(void) ::fprintf( stderr, "XStoreName returned an error\n" );
		::abort();
	}
	
	// Chose events we want.
	if (XSelectInput(
		mDisplay,
		mWindow,
		ExposureMask | ButtonPressMask | ButtonReleaseMask |
		StructureNotifyMask | Button1MotionMask | KeyPressMask | KeyReleaseMask ) < 0)
	{
		(void) ::fprintf( stderr, "XSelectInput returned an error\n" );
		::abort();
	}
	
	// Prevent window from being closable.
	XSetWMProtocols( mDisplay, mWindow, &mWMDeleteWindow, 1 );

	mGC = XCreateGC( mDisplay, mWindow, 0, 0 );

	// Set up the window in the current orientation.
	SetupRotateWindow();
}

// -------------------------------------------------------------------------- //
//  * SetupRotateWindow( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::SetupRotateWindow( void )
{
	// Make window unresizable
	MakeWindowUnresizable();
	
	// Create a new image.
	KUInt32 theWidth = GetActualScreenWidth();
	KUInt32 theHeight = GetActualScreenHeight();
	mImage = XCreateImage(
				mDisplay,
				mVisual,
				mDepth /* depth */,
				ZPixmap /* format */,
				0 /* offset */,
				mImageBuffer /* data */,
				theWidth /* width */,
				theHeight /* height */,
				8 /* bitmap_pad */,
				0 /* bytes per line -- let Xlib compute it */);
	mBitsPerPixel = mImage->bits_per_pixel;
	if (mImageBuffer == NULL)
	{
		mImageBuffer = (char*)
			::malloc( (theWidth * theHeight * mBitsPerPixel) / 8);
		mImage->data = mImageBuffer;
	}
}

// -------------------------------------------------------------------------- //
//  * FindVisual( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::FindVisual( void )
{
	XVisualInfo theVisualInfoTemplate;
	theVisualInfoTemplate.c_class = TrueColor;
	theVisualInfoTemplate.depth = 24;
	theVisualInfoTemplate.bits_per_rgb = 8;
	do {
		int nbItems;
		theVisualInfoTemplate.c_class = TrueColor;
		XVisualInfo* theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask | VisualBitsPerRGBMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = 24;
			mVisual = theVisualInfos->visual;
			mRedMask = theVisualInfos->red_mask;
			mBlueMask = theVisualInfos->blue_mask;
			mGreenMask = theVisualInfos->green_mask;
			break;
		}

		// On essaye avec 15 bits.
		theVisualInfoTemplate.c_class = TrueColor;
		theVisualInfoTemplate.depth = 15;
		theVisualInfoTemplate.bits_per_rgb = 5;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask | VisualBitsPerRGBMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = 15;
			mVisual = theVisualInfos->visual;
			mRedMask = theVisualInfos->red_mask;
			mBlueMask = theVisualInfos->blue_mask;
			mGreenMask = theVisualInfos->green_mask;
			break;
		}
		
		// On essaye avec 16 bits en rrrrrggggggbbbbb
		theVisualInfoTemplate.c_class = TrueColor;
		theVisualInfoTemplate.depth = 16;
		theVisualInfoTemplate.red_mask = 0xF800;
		theVisualInfoTemplate.green_mask = 0x07E0;
		theVisualInfoTemplate.blue_mask = 0x001F;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask
					| VisualRedMaskMask
					| VisualGreenMaskMask
					| VisualBlueMaskMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = 16;
			mVisual = theVisualInfos->visual;
			mRedMask = theVisualInfos->red_mask;
			mBlueMask = theVisualInfos->blue_mask;
			mGreenMask = theVisualInfos->green_mask;
			break;
		}
		
		// Nothing allowed me to avoid allocation of colors.
		// Try higher depths first.
		
		// TrueColor, 15.
		theVisualInfoTemplate.c_class = TrueColor;
		theVisualInfoTemplate.depth = 15;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = theVisualInfos->depth;
			mVisual = theVisualInfos->visual;
			mAllocColor = true;
			break;
		}

		// TrueColor, 16.
		theVisualInfoTemplate.c_class = TrueColor;
		theVisualInfoTemplate.depth = 16;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = theVisualInfos->depth;
			mVisual = theVisualInfos->visual;
			mAllocColor = true;
			break;
		}

		// TrueColor, 24.
		theVisualInfoTemplate.c_class = TrueColor;
		theVisualInfoTemplate.depth = 24;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = theVisualInfos->depth;
			mVisual = theVisualInfos->visual;
			mAllocColor = true;
			break;
		}

		// TrueColor, 32.
		theVisualInfoTemplate.c_class = TrueColor;
		theVisualInfoTemplate.depth = 32;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = theVisualInfos->depth;
			mVisual = theVisualInfos->visual;
			mAllocColor = true;
			break;
		}

		// DirectColor, 15.
		theVisualInfoTemplate.c_class = DirectColor;
		theVisualInfoTemplate.depth = 15;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = theVisualInfos->depth;
			mVisual = theVisualInfos->visual;
			mAllocColor = true;
			break;
		}

		// DirectColor, 16.
		theVisualInfoTemplate.c_class = DirectColor;
		theVisualInfoTemplate.depth = 16;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = theVisualInfos->depth;
			mVisual = theVisualInfos->visual;
			mAllocColor = true;
			break;
		}

		// DirectColor, 24.
		theVisualInfoTemplate.c_class = DirectColor;
		theVisualInfoTemplate.depth = 24;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = theVisualInfos->depth;
			mVisual = theVisualInfos->visual;
			mAllocColor = true;
			break;
		}

		// DirectColor, 32.
		theVisualInfoTemplate.c_class = DirectColor;
		theVisualInfoTemplate.depth = 32;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask | VisualDepthMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = theVisualInfos->depth;
			mVisual = theVisualInfos->visual;
			mAllocColor = true;
			break;
		}

		// Direct Color, any.
		theVisualInfoTemplate.c_class = DirectColor;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = theVisualInfos->depth;
			mVisual = theVisualInfos->visual;
			mAllocColor = true;
			break;
		}

		// True Color, any.
		theVisualInfoTemplate.c_class = TrueColor;
		theVisualInfos =
			XGetVisualInfo(
				mDisplay,
				VisualClassMask,
				&theVisualInfoTemplate,
				&nbItems );
		if (nbItems > 0)
		{
			mDepth = theVisualInfos->depth;
			mVisual = theVisualInfos->visual;
			mAllocColor = true;
			break;
		}

		// Default visual.
		mDepth = DefaultDepth(mDisplay, DefaultScreen(mDisplay));
		mVisual = DefaultVisual(mDisplay, DefaultScreen(mDisplay));
		mAllocColor = true;
	} while (0);
}

// -------------------------------------------------------------------------- //
//  * CreatePalette( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::CreatePalette( void )
{
	int index;
	if (mAllocColor)
	{
		// Get the color map
		Colormap theColormap =
			DefaultColormap(mDisplay, DefaultScreen(mDisplay));
		for (index = 0; index < 16; index++)
		{
			mColors[index].flags = DoRed | DoGreen | DoBlue;
			unsigned short theComp =
				(index << 12) | (index << 8) | (index << 4) | index;
			mColors[index].red = theComp;
			mColors[index].green = theComp;
			mColors[index].blue = theComp;
			mColors[index].pixel = 42;
			(void) XAllocColor( mDisplay, theColormap, &mColors[index] );
			mPalette[index] = mColors[index].pixel;
			
			mColors[16 + index].flags = DoRed | DoGreen | DoBlue;
			mColors[16 + index].red = 0;
			mColors[16 + index].green = theComp;
			mColors[16 + index].blue = 0;
			(void) XAllocColor( mDisplay, theColormap, &mColors[index + 16] );
			mPalette[index + 16] = mColors[index + 16].pixel;
		}
	} else {
		bool swapBytes;
#if TARGET_RT_LITTLE_ENDIAN
		swapBytes = mImage->byte_order == MSBFirst;
#else
		swapBytes = mImage->byte_order == LSBFirst;
#endif

		KUInt32 theFullMask = mRedMask | mBlueMask | mGreenMask;
		if (mDepth == 24)
		{
			// 3*8
			for (index = 0; index < 16; index++)
			{
				KUInt32 theFullPixel = index | (index << 4);
				theFullPixel |= (theFullPixel << 8) | (theFullPixel << 16);
				if (swapBytes)
				{
					mPalette[index] = UByteSex::Swap(theFullPixel & theFullMask);
					mPalette[index + 16] = UByteSex::Swap(theFullPixel & mGreenMask);
				} else {
					mPalette[index] = theFullPixel & theFullMask;
					mPalette[index + 16] = theFullPixel & mGreenMask;
				}
			}
		} else if (mDepth == 15) {
			for (index = 0; index < 16; index++)
			{
				KUInt16 theFullPixel = index << 1;
				theFullPixel |= (theFullPixel << 5) | (theFullPixel << 10);
				if (swapBytes)
				{
					mPalette[index] = UByteSex::Swap(theFullPixel & theFullMask);
					mPalette[index + 16] = UByteSex::Swap(theFullPixel & mGreenMask);
				} else {
					mPalette[index] = theFullPixel & theFullMask;
					mPalette[index + 16] = theFullPixel & mGreenMask;
				}
			}
		} else if (mDepth == 16) {
			for (index = 0; index < 16; index++)
			{
				KUInt16 theFullPixel = index << 1;
				theFullPixel |= (theFullPixel << 6) | (theFullPixel << 11);
				if (swapBytes)
				{
					mPalette[index] = UByteSex::Swap(theFullPixel & theFullMask);
					mPalette[index + 16] = UByteSex::Swap(theFullPixel & mGreenMask);
				} else {
					mPalette[index] = theFullPixel & theFullMask;
					mPalette[index + 16] = theFullPixel & mGreenMask;
				}
			}
		}
	}
}

// -------------------------------------------------------------------------- //
//  * FreePalette( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::FreePalette( void )
{
	if (mAllocColor)
	{
		Colormap theColormap =
			DefaultColormap(mDisplay, DefaultScreen(mDisplay));
		(void) XFreeColors(
					mDisplay, theColormap, (long unsigned int*)mPalette, sizeof(mPalette), 0);
	}
}

// -------------------------------------------------------------------------- //
//  * MakeWindowUnresizable( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::MakeWindowUnresizable( void )
{
	// Make window unresizable
	XSizeHints* theHints = XAllocSizeHints();
	if (theHints == NULL)
	{
		(void) ::fprintf( stderr, "Couldn't allocate hints\n" );
		::abort();
	}
	
	KUInt32 theWidth = GetActualScreenWidth();
	KUInt32 theHeight = GetActualScreenHeight();
	theHints->min_width = theWidth;
	theHints->max_width = theWidth;
	theHints->min_height = theHeight;
	theHints->max_height = theHeight;
	theHints->flags = PMinSize | PMaxSize;
	XSetWMNormalHints( mDisplay, mWindow, theHints );
	XFree( theHints );
}

// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::PowerOn( void )
{
	// This space for rent.
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::PowerOff( void )
{
	// This space for rent.
}

// -------------------------------------------------------------------------- //
//  * PowerOnScreen( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::PowerOnScreen( void )
{
	// Display our window
	(void) XMapWindow( mDisplay, mWindow );

	// Make the window full screen.
	if (IsFullScreen())
	{
        XEvent theEvent;
        theEvent.type = ClientMessage;
        theEvent.xclient.type = ClientMessage;
        theEvent.xclient.serial = 0;
        theEvent.xclient.send_event = true;
        theEvent.xclient.display = mDisplay;
        theEvent.xclient.window = mWindow;
        theEvent.xclient.message_type = mWMState;
        theEvent.xclient.format = 32;
        theEvent.xclient.data.l[0] = 1 /* ADD */;
        theEvent.xclient.data.l[1] = mWMFullScreen;
        theEvent.xclient.data.l[2] = None;

        XSendEvent(
        		mDisplay, mRootWindow,
				false,
				SubstructureRedirectMask | SubstructureNotifyMask,
				&theEvent);
	}

	// Start the event thread.
	if (mThread)
	{
		delete mThread;
	}
	mThread = new TThread( this );

	mPowerIsOn = true;
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::PowerOffScreen( void )
{
	// Hide our window
	XUnmapWindow( mDisplay, mWindow );

	// Join and destroy the thread.
	if (mThread)
	{
		delete mThread;
		mThread = nil;
	}
	
	mPowerIsOn = false;
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( bool )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::BacklightChanged( bool )
{
	// Redraw the screen.
	SRect wholeScreen;
	wholeScreen.fTop = 0;
	wholeScreen.fLeft = 0;
	wholeScreen.fBottom = GetScreenHeight();
	wholeScreen.fRight = GetScreenWidth();
	UpdateScreenRect( &wholeScreen );
}

// -------------------------------------------------------------------------- //
//  * ContrastChanged( KUInt32 )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::ContrastChanged( KUInt32 )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * ScreenOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::ScreenOrientationChanged( EOrientation /* inNewOrientation */ )
{
	if (!IsFullScreen())
	{
		MakeWindowUnresizable();
		
		// Resize the window (ignore any error).
		(void) XResizeWindow(mDisplay, mWindow, GetScreenWidth(), GetScreenHeight() );
		
		// Destroy the image, but not the buffer.
		mImage->data = NULL;
		XDestroyImage(mImage);
		
		// Then rotate the window.
		SetupRotateWindow();
	}
}

// -------------------------------------------------------------------------- //
//  * TabletOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::TabletOrientationChanged( EOrientation )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * UpdateScreenRect( SRect* )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::UpdateScreenRect( SRect* inUpdateRect )
{
	KUInt16 top = inUpdateRect->fTop;
	KUInt16 left = inUpdateRect->fLeft;
	KUInt16 height = inUpdateRect->fBottom - top;
	KUInt16 width = inUpdateRect->fRight - left;
	
	// Update the buffer.
	// We copy more pixels than what we should.
	if (left & 0x1)
	{
		left -= 1;
		width += 1;
	}
	if (width & 0x1)
	{
		width += 1;
	}
	
	KUInt8* theScreenBuffer = GetScreenBuffer();
	KUInt32 theScreenWidth = GetScreenWidth();
	KUInt32 dstRowBytes = theScreenWidth * mBitsPerPixel / 8;
	KUInt32 srcRowBytes = theScreenWidth * kBitsPerPixel / 8;
	KUInt32 srcWidthInBytes = width * kBitsPerPixel / 8;

	KUInt8* srcRowPtr =
		theScreenBuffer
		+ (top * srcRowBytes)
		+ (left * kBitsPerPixel / 8);
	KUInt8* dstRowPtr =
		((KUInt8*) mImageBuffer)
		+ (top * dstRowBytes)
		+ (left * mBitsPerPixel / 8);

	int indexRows;

/*
	// Rotate the source instead.
	switch (mBlitRotation)
	{
		case kBlitRotationNone:
			// this space for rent.
			break;
		
		case kBlitRotation90:
			// 90° clockwise.
			{
				// Iterate on pixels.
				int indexX;
				for (indexRows = 0; indexRows < height; indexRows++)
				{
					KUInt8* srcCursor = srcRowPtr;
					for (indexX = 0; indexX < width; indexX += 2)
					{
						KUInt8 theByte = *srcCursor++;
						// First pixel.
						KUInt32 offset = (indexX * srcRowBytes) + (indexRows/2);
						KUInt8 theDstByte = mRotationBuffer[offset];
						if (indexRows & 0x1)
						{
							theDstByte = (theDstByte & 0x0F) | (theByte & 0xF0);
						} else {
							theDstByte = (theDstByte & 0xF0) | (theByte >> 4);
						}
						mRotationBuffer[offset] = theDstByte;

						// Second pixel.
						offset += srcRowBytes;
						theDstByte = mRotationBuffer[offset];
						if (indexRows & 0x1)
						{
							theDstByte = (theDstByte & 0x0F) | (theByte << 4);
						} else {
							theDstByte = (theDstByte & 0xF0) | (theByte & 0xF);
						}
						mRotationBuffer[offset] = theDstByte;
					}
					srcRowPtr += srcRowBytes;
				}
				
				srcRowPtr = (KUInt8*) mRotationBuffer;
				dstRowPtr =
					((KUInt8*) mImageBuffer)
					+ (left * dstRowBytes)
					+ (top * mBitsPerPixel / 8);
			}
			break;
		
		case kBlitRotation180:
			{
				// Iterate on rows.
				KUInt8* rotateRowPtr =
					((KUInt8*) mRotationBuffer)
					+ (height * srcRowBytes);
				for (indexRows = height; indexRows != 0; indexRows--)
				{
					KUInt8* srcCursor = srcRowPtr;
					KUInt8* srcEnd = srcRowPtr + srcWidthInBytes;
					KUInt8* dstCursor = rotateRowPtr + width - 1;
					do {
						KUInt8 theByte = *srcCursor++;
						// Swap pixels
						theByte = (theByte >> 4) | (theByte << 4);
						*dstCursor-- = theByte;
					} while (srcCursor < srcEnd);

					srcRowPtr += srcRowBytes;
					rotateRowPtr -= srcRowBytes;
				}

				srcRowPtr = (KUInt8*) mRotationBuffer;
				dstRowPtr =
					((KUInt8*) mImageBuffer)
					+ ((mX11ScreenHeight - top - height) * dstRowBytes)
					+ ((mX11ScreenWidth - left - width) * mBitsPerPixel / 8);
			}
			break;

		case kBlitRotation270:
			// 90° counter-clockwise.
			{
				// Iterate on pixels.
				int indexX;
				for (indexRows = 0; indexRows < height; indexRows++)
				{
					KUInt8* srcCursor = srcRowPtr;
					for (indexX = 0; indexX < width; indexX += 2)
					{
						KUInt8 theByte = *srcCursor++;
						// First pixel.
						KUInt32 offset = (indexX * srcRowBytes) + (indexRows/2);
						KUInt8 theDstByte = mRotationBuffer[offset];
						if (indexRows & 0x1)
						{
							theDstByte = (theDstByte & 0x0F) | (theByte & 0xF0);
						} else {
							theDstByte = (theDstByte & 0xF0) | (theByte >> 4);
						}
						mRotationBuffer[offset] = theDstByte;

						// Second pixel.
						offset += srcRowBytes;
						theDstByte = mRotationBuffer[offset];
						if (indexRows & 0x1)
						{
							theDstByte = (theDstByte & 0x0F) | (theByte << 4);
						} else {
							theDstByte = (theDstByte & 0xF0) | (theByte & 0xF);
						}
						mRotationBuffer[offset] = theDstByte;
					}
					srcRowPtr += srcRowBytes;
				}
				
				srcRowPtr = (KUInt8*) mRotationBuffer;
				dstRowPtr =
					((KUInt8*) mImageBuffer)
					+ (left * dstRowBytes)
					+ (top * mBitsPerPixel / 8);
			}
			break;
		
	}
*/
#if SIXTEEN_GREYS
	KUInt32 paletteBase;
	if (GetBacklight())
	{
		paletteBase = 16;
	} else {
		paletteBase = 0;
	}

	KUInt32* thePalette = mPalette;
	if (mBitsPerPixel == 32)
	{
		for (indexRows = height; indexRows != 0; indexRows--)
		{
			KUInt8* srcCursor = srcRowPtr;
			KUInt8* srcEnd = srcRowPtr + srcWidthInBytes;
			KUInt32* dstCursor = (KUInt32*) dstRowPtr;
			do {
				KUInt8 theByte = *srcCursor++;
				// First pixel
				KUInt32 thePixel = (theByte & 0xF0) >> 4;
				*dstCursor++ = thePalette[paletteBase + thePixel];
				// Second pixel
				thePixel = (theByte & 0x0F);
				*dstCursor++ = thePalette[paletteBase + thePixel];
			} while (srcCursor < srcEnd);
			srcRowPtr += srcRowBytes;
			dstRowPtr += dstRowBytes;
		}
	} else if (mBitsPerPixel == 16) {
		for (indexRows = height; indexRows != 0; indexRows--)
		{
			KUInt8* srcCursor = srcRowPtr;
			KUInt8* srcEnd = srcRowPtr + srcWidthInBytes;
			KUInt16* dstCursor = (KUInt16*) dstRowPtr;
			do {
				KUInt8 theByte = *srcCursor++;
				// First pixel
				KUInt32 thePixel = (theByte & 0xF0) >> 4;
				*dstCursor++ = thePalette[paletteBase + thePixel];
				// Second pixel
				thePixel = (theByte & 0x0F);
				*dstCursor++ = thePalette[paletteBase + thePixel];
			} while (srcCursor < srcEnd);
			srcRowPtr += srcRowBytes;
			dstRowPtr += dstRowBytes;
		}
	} else if (mBitsPerPixel == 8) {
		for (indexRows = height; indexRows != 0; indexRows--)
		{
			KUInt8* srcCursor = srcRowPtr;
			KUInt8* srcEnd = srcRowPtr + srcWidthInBytes;
			KUInt8* dstCursor = (KUInt8*) dstRowPtr;
			do {
				KUInt8 theByte = *srcCursor++;
				// First pixel
				KUInt32 thePixel = (theByte & 0xF0) >> 4;
				*dstCursor++ = thePalette[paletteBase + thePixel];
				// Second pixel
				thePixel = (theByte & 0x0F);
				*dstCursor++ = thePalette[paletteBase + thePixel];
			} while (srcCursor < srcEnd);
			srcRowPtr += srcRowBytes;
			dstRowPtr += dstRowBytes;
		}
	} else {
		(void) ::fprintf( stderr, "Unsupported host screen depth\n" );
		::abort();
	}
#elif TWOHUNDREDANDFIFTYSIX_GREYS
	KUInt32 theMask;
	if (GetBacklight())
	{
		theMask = mGreenMask;
	} else {
		theMask = mRedMask | mBlueMask | mGreenMask;
	}

	if (mBitsPerPixel == 32)
	{
		for (indexRows = height; indexRows != 0; indexRows--)
		{
			KUInt8* srcCursor = srcRowPtr;
			KUInt8* srcEnd = srcRowPtr + srcWidthInBytes;
			KUInt32* dstCursor = (KUInt32*) dstRowPtr;
			do {
				KUInt8 theByte = *srcCursor++;
				KUInt32 thePixel =
					theByte | (theByte << 8)
					| (theByte << 16) | (theByte << 24);
				*dstCursor++ = thePixel & theMask;
			} while (srcCursor < srcEnd);
			srcRowPtr += srcRowBytes;
			dstRowPtr += dstRowBytes;
		}
	} else {
		(void) ::fprintf( stderr, "Unsupported host screen depth\n" );
		::abort();
	}
#elif MILLIONS_OF_COLORS
	if (mBitsPerPixel == 32)
	{
		for (indexRows = height; indexRows != 0; indexRows--)
		{
			KUInt32* srcCursor = (KUInt32*) srcRowPtr;
			KUInt32* srcEnd = (KUInt32*) (srcRowPtr + srcWidthInBytes);
			KUInt32* dstCursor = (KUInt32*) dstRowPtr;
			do {
				*dstCursor++ = *srcCursor++;
			} while (srcCursor < srcEnd);
			srcRowPtr += srcRowBytes;
			dstRowPtr += dstRowBytes;
		}
	} else {
		(void) ::fprintf( stderr, "Unsupported host screen depth\n" );
		::abort();
	}
#else
	#error "Unsupported emulated screen depth"
#endif

	// Put the image.
	int theErr = XPutImage(
						mDisplay,
						mWindow,
						mGC,
						mImage,
						left,
						top,
						left,
						top,
						width,
						height
						);
	if (theErr != 0)
	{
		(void) ::fprintf( stderr, "XPutImage returned an error (%i)\n", theErr );
		::abort();
	}
	// Make sure that every rectangle of data is actually sent immediatly,
	// (this can be optimized by merging multiple requests into a singel one that is
	// sent no more than 50 times per second, for example)
	XFlush(mDisplay);
}

static const KUInt8 none = 0x2f; // send a '.' for unexpected keycodes

// -------------------------------------------------------------------------- //
//  * TranslateKeyCode( int )
// -------------------------------------------------------------------------- //
KUInt8
TX11ScreenManager::TranslateKeyCode( XEvent inEvent )
{
    static int macKeycode = 0;
    auto X11KeyCode = inEvent.xkey.keycode;
    //printf("X11 Keycode 0x%08x %d\n", X11KeyCode, X11KeyCode);
    if (X11KeyCode < sizeof(mKeycodes))
    	return mKeycodes[ X11KeyCode ];
    else
        return none;
}

// -------------------------------------------------------------------------- //
//  * LoadKeyCodesTranslation( void )
// -------------------------------------------------------------------------- //
void
TX11ScreenManager::LoadKeyCodesTranslation( void )
{
	// Open file ~/.einstein.keycodes
	
	// If it cannot be found, fill with X == X (what we have on Apple X11).
	//int index;
	//for (index = 0; index < 255; index++)
	//{
	//if (mKeycodes[index]==0xff)
	//	    mKeycodes[index] = index;
	//}
}

const KUInt8 TX11ScreenManager::mKeycodes[128] = {
        none, none, none, none, none, none, none, none, none,
        53,	// 9: Esc
        18,	// 10: 1
        19,	// 11: 2
        20,	// 12: 3
        21,	// 13: 4
        23,	// 14: 5
        22,	// 15: 6
        26,	// 16: 7
        28,	// 17: 8
        25,	// 18: 9
        29,	// 19: 0
        27,	// 20: -
        24,	// 21: =
        51,	// 22: Backspace
        48,	// 23: Tab
        12,	// 24: Q
        13,	// 25: W
        14,	// 26: E
        15,	// 27: R
        17,	// 28: T
        16,	// 29: Y
        32,	// 30: U
        34,	// 31: I
        31,	// 32: O
        35,	// 33: P
        33,	// 34: [
        30,	// 35: ]
        36,	// 36: Return
        0x37,	// 37: Ctrl Left
        0,	// 38: A
        1,	// 39: S
        2,	// 40: D
        3,	// 41: F
        5,	// 42: G
        4,	// 43: H
        38,	// 44: J
        40,	// 45: K
        37,	// 46: L
        41,	// 47: ;
        39,	// 48: '
        0x32,	// 49: `
        56,	// 50: Shift Left
        42,	// 51: '\'
        6,	// 52: Z
        7,	// 53: X
        8,	// 54: C
        9,	// 55: V
        11,	// 56: B
        45,	// 57: N
        46,	// 58: M
        43,	// 59: ,
        47,	// 60: .
        44,	// 61: /
        56,	// 62: Shift Right
        67,	// 63: KP *
        0x3a,	// 64: Alt Left
        49,	// 65: Space
        57,	// 66: Caps Lock
        122,	// 67: F1
        120,	// 68: F2
        99,	// 69: F3
        118,	// 70: F4
        96,	// 71: F5
        97,	// 72: F6
        98,	// 73: F7
        100,	// 74: F8
        101,	// 75: F9
        109,	// 76: F10
        71,	// 77: Num Lock
        107,	// 78: Scroll Lock
        89,	// 79: KP 7
        91,	// 80: KP 8
        92,	// 81: KP 9
        78,	// 82: KP -
        86,	// 83: KP 4
        87,	// 84: KP 5
        88,	// 85: KP 6
        69,	// 86: KP +
        83,	// 87: KP 1
        84,	// 88: KP 2
        85,	// 89: KP 3
        82,	// 90: KP 0
        65,	// 91: KP .
        none, none, // 92, 93
        50,	// 94: International
        103,	// 95: F11
        111,	// 96: F12
        115,	// 97: Home
        62,	// 98: Cursor Up
        116,	// 99: Page Up
        59,	// 100: Cursor Left
        none, // 101
        60,	// 102: Cursor Right
        119,	// 103: End
        61,	// 104: Cursor Down
        121,	// 105: Page Down
        114,	// 106: Insert
        117,	// 107: Delete
        0x3a,	// 108: KP Enter
        54,	// 109: Ctrl Right
        113,	// 110: Pause
        0x7e,	// 111: up
        75,	// 112: KP /
        0x7b, // 113: arrow left
        0x7c, // 114: arrow right
        58,	// 115: Logo Left (-> Option)
        0x7d,	// 116: down
        50,	// 117: Menu (-> International)
        none, none, none, none, none, none, none, none, none, none // 118..127
};

// ========================================================================= //
// ... Any resemblance between the above views and those of my employer,     //
// my terminal, or the view out my window are purely coincidental.  Any      //
// resemblance between the above and my own views is non-deterministic.  The //
// question of the existence of views in the absence of anyone to hold them  //
// is left as an exercise for the reader.  The question of the existence of  //
// the reader is left as an exercise for the second god coefficient.  (A     //
// discussion of non-orthogonal, non-integral polytheism is beyond the scope //
// of this article.)                                                         //
// ========================================================================= //
