// ==============================
// File:			TFLScreenManager.cp
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
#include "TFLScreenManager.h"

// FLTK interface
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/fl_draw.H>

#include <stdio.h>
#include <stdlib.h>
#include <cctype>

// K
#include <K/Defines/UByteSex.h>
#include <K/Threads/TThread.h>

// Einstein
#include "Emulator/Log/TLog.h"
#include "Emulator/Platform/TPlatformManager.h"
#include "app/TFLApp.h"


#if TARGET_OS_WIN32 || TARGET_OS_MAC
#define SRC_BITMAP_FORMAT_RGBA 1
#else
#define SRC_BITMAP_FORMAT_RGBA 0
#endif

#if TARGET_OS_MAC
#include <CoreGraphics/CoreGraphics.h>
#endif

#if TARGET_OS_LINUX
#include <X11/extensions/Xrender.h>
#endif


// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
// FLTK Class
// -------------------------------------------------------------------------- //

// FLTK had (has) a bug where the keycode for single quote, back quote, and backslash were (are) mixed up
#if TARGET_OS_WIN32_NO_MORE
#define vkTick 0x2A
#define vkBackTick 0x27
#define vkBackSlash 0x32
#else 
#define vkTick 0x27
#define vkBackTick 0x32
#define vkBackSlash 0x2A
#endif

static const struct {unsigned short vk, fltk;} vktab[] = {
    { 49, ' ' }, { vkTick, '\"' }, { vkTick, '\'' }, { 24, '+'}, { 43, ',' }, { 27, '-' }, { 47, '.' }, { 44, '/' },
  { 29, '0' }, { 18, '1'  }, { 19, '2'  }, { 20, '3'  }, 
  { 21, '4' }, { 23, '5'  }, { 22, '6'  }, { 26, '7'  }, 
  { 28, '8' }, { 25, '9'  }, { 41, ':'  }, { 41, ';'  }, { 43, '<' }, { 24, '='  }, { 47, '>' }, { 44, '?' },
  {  0, 'A' }, { 11, 'B'  }, {  8, 'C'  }, {  2, 'D'  }, 
  { 14, 'E' }, {  3, 'F'  }, {  5, 'G'  }, {  4, 'H'  }, 
  { 34, 'I' }, { 38, 'J'  }, { 40, 'K'  }, { 37, 'L'  }, 
  { 46, 'M' }, { 45, 'N'  }, { 31, 'O'  }, { 35, 'P'  }, 
  { 12, 'Q' }, { 15, 'R'  }, {  1, 'S'  }, { 17, 'T'  }, 
  { 32, 'U' }, {  9, 'V'  }, { 13, 'W'  }, {  7, 'X'  }, 
  { 16, 'Y' }, {  6, 'Z'  }, 
    { 33, '[' }, { vkBackSlash, '\\' }, { 30, ']' }, { 27, '_' },
    { vkBackTick, '`' }, { 33, '{' }, { vkBackSlash, '|' }, { 30, '}' }, { vkBackTick, '~' },
  { 51, FL_BackSpace }, { 48, FL_Tab }, { 36, FL_Enter }, { 127, FL_Pause },
  { 107, FL_Scroll_Lock }, { 53, FL_Escape }, { 0x73, FL_Home }, { 123, FL_Left },
  { 126, FL_Up }, { 124, FL_Right }, { 125, FL_Down }, { 0x74, FL_Page_Up },
  { 0x79, FL_Page_Down },  { 119, FL_End }, { 0x71, FL_Print }, { 127, FL_Insert },
  { 0x6e, FL_Menu }, { 114, FL_Help }, { 0x47, FL_Num_Lock }, 
  { 76, FL_KP_Enter }, { 67, FL_KP+'*' }, { 69, FL_KP+'+'}, { 78, FL_KP+'-' }, { 65, FL_KP+'.' }, { 75, FL_KP+'/' }, 
  { 82, FL_KP+'0' }, { 83, FL_KP+'1' }, { 84, FL_KP+'2' }, { 85, FL_KP+'3' }, 
  { 86, FL_KP+'4' }, { 87, FL_KP+'5' }, { 88, FL_KP+'6' }, { 89, FL_KP+'7' }, 
  { 91, FL_KP+'8' }, { 92, FL_KP+'9' }, { 81, FL_KP+'=' }, 
  { 0x7a, FL_F+1 }, { 0x78, FL_F+2  }, { 0x63, FL_F+3  }, { 0x76, FL_F+4  }, 
  { 0x60, FL_F+5 }, { 0x61, FL_F+6  }, { 0x62, FL_F+7  }, { 0x64, FL_F+8  }, 
  { 0x65, FL_F+9 }, { 0x6D, FL_F+10 }, { 0x67, FL_F+11 }, { 0x6f, FL_F+12 }, 
  { 56, FL_Shift_L }, { 56, FL_Shift_R }, { 59, FL_Control_L }, { 59, FL_Control_R }, 
  { 57, FL_Caps_Lock }, { 55, FL_Meta_L }, { 55, FL_Meta_R },
  { 58, FL_Alt_L }, { 58, FL_Alt_R }, { 0x75, FL_Delete },
};


///
/// This class generates an FLTK widget which is then automatically added to 
/// the active group widget.
///
class Fl_Newton_Screen_Widget : public Fl_Box
{
    typedef Fl_Box      super;

	unsigned char		*rgbData_;
    TFLScreenManager    *screenManager_;
    TFLApp              *mApp = nullptr;
	int					rgbWidth_, rgbHeight_;
	int					penX, penY, penIsDown;
    bool                mPowerState = false;

public:
	Fl_Newton_Screen_Widget(int x, int y, int w, int h, const char *l, TFLScreenManager *s, TFLApp *inApp)
		: Fl_Box(x, y, w, h, l),
		rgbData_(0L),
		screenManager_(s),
        mApp(inApp),
		penX(0), penY(0), penIsDown(0)
	{
        box(FL_FLAT_BOX);
        color(FL_BLACK);
		rgbWidth_ = w;
		rgbHeight_ = h;
#if SRC_BITMAP_FORMAT_RGBA
		rgbData_ = (unsigned char*)calloc(w*h, 4);
#else
		rgbData_ = (unsigned char*)calloc(w * h, 3);
#endif
	}

	~Fl_Newton_Screen_Widget()
	{
		screenManager_->unlinkWidget();
	}

	int getRGBWidth()
	{
		return rgbWidth_;
	}

	int getRGBHeight()
	{
		return rgbHeight_;
	}

	void draw() override
	{
		// TODO: if we are in fullscreen mode with a portrait Newton on a landscape host screen,
		//       we should keep the aspect ration and draw black borders (and vice versa).
		//       Currently we stretch the image very badly. Maybe we should always keep the aspect ration 
		//		 when going fullscreen? 
		// FIXME draw borders if the widget is larger than our bitmap
		// FIXME enable clipping if the widget is smaller
		// FIXME center the bitmap if it is smaller
        if (mPowerState) {
#if TARGET_OS_WIN32
			// Windows GDI provides a function that scales the bitmap while the hardware blits it to the screen
			static int useGDI = -1; // initially, we don't know
			if (useGDI == -1) {
				useGDI = ((GetDeviceCaps(fl_gc, RASTERCAPS) & RC_STRETCHDIB) != 0);
				OutputDebugString(useGDI ? "Using accelerated image scaling" : "Using FLTK image scaling\n");
			}
			if (useGDI == 1) {
				float s = fl_graphics_driver->scale();
				static BITMAPINFO info = { { sizeof(BITMAPINFOHEADER) } };
				info.bmiHeader.biBitCount = 32;
				info.bmiHeader.biWidth = rgbWidth_;
				info.bmiHeader.biHeight = rgbHeight_;
				info.bmiHeader.biPlanes = 1;
				info.bmiHeader.biSizeImage = rgbWidth_ * rgbHeight_ * 4;
				info.bmiHeader.biCompression = BI_RGB;
				StretchDIBits(fl_gc, int(x()*s), int(y()*s), int(w()*s), int(h()*s),
					0, rgbHeight_, rgbWidth_, -rgbHeight_, rgbData_, &info, DIB_RGB_COLORS, SRCCOPY);
			} else {
				fl_draw_image(rgbData_, x(), y(), rgbWidth_, rgbHeight_, 4); // 32 bits: RGBx
			}
#elif TARGET_OS_MAC
			// TODO: this code is faster than the FLTK version, but slower than expected. We should invest
			//       some time to find the perfect setup for maximum speed and full GPU support.
            CGColorSpaceRef lut = 0;
            lut = CGColorSpaceCreateDeviceRGB();

            // we need to set up a data provider that grabs our raw RGBs
            CGDataProviderRef src = CGDataProviderCreateWithData( nullptr, rgbData_, rgbWidth_*rgbHeight_*4, nullptr);

            // now wrap that all insode an image format
            CGImageRef img = CGImageCreate(rgbWidth_, rgbHeight_, 8, 32, rgbWidth_*4,
                                           lut, kCGImageAlphaNoneSkipLast, src, nullptr,
                                           false, kCGRenderingIntentDefault);
            CGColorSpaceRelease(lut);
            CGDataProviderRelease(src);

            // draw the image into the destination context
            if (img) {
                CGContextSaveGState(fl_gc);
                
                CGContextSetInterpolationQuality(fl_gc, kCGInterpolationNone);
                CGContextScaleCTM(fl_gc, 1, -1);
                CGContextDrawImage(fl_gc, CGRectMake(0, -h() - y(), w(), h()), img);
                
                CGImageRelease(img);
                CGContextRestoreGState(fl_gc);
            } else {
                fl_draw_image(rgbData_, x(), y(), rgbWidth_, rgbHeight_, 4); // 32 bit: RGB
            }
//#elif TARGET_OS_LINUX
#if 0
    // TODO: Nothing implemented yet for X11/Xlib. XRender extension seems to be the way to go.
            double xscale= 1.0;
            double yscale= 1.0;
            XTransform xform = {{
                                        { XDoubleToFixed( xscale ), XDoubleToFixed( 0 ), XDoubleToFixed( 0 ) },
                                        { XDoubleToFixed( 0 ), XDoubleToFixed( yscale ), XDoubleToFixed( 0 ) },
                                        { XDoubleToFixed( 0 ), XDoubleToFixed( 0 ), XDoubleToFixed(1.0) }
                                }};

            XRenderPictFormat *picFormat = XRenderFindVisualFormat(fl_display, (const Visual*)fl_visual);
            XRenderPictureAttributes pa = { };
            //pa.subwindow_mode = IncludeInferiors;
            Pixmap pixmap = XCreatePixmapFromBitmapData(
                    fl_display		/* display */,
                    fl_window		/* d */,
                    reinterpret_cast<char *>(rgbData_)        /* data */,
                    rgbWidth_	/* width */,
                    rgbHeight_	/* height */,
                    0	/* fg */,
                    0	/* bg */,
                    3	/* depth */
            );
            Picture srcPic = XRenderCreatePicture(fl_display, pixmap, picFormat, CPSubwindowMode, &pa);
            Picture dstPic = XRenderCreatePicture(fl_display, fl_window, picFormat, CPSubwindowMode, &pa);

            XRenderSetPictureTransform(fl_display, srcPic, &xform);
            XRenderComposite(fl_display, PictOpOver,
                             srcPic, 0, dstPic, // src, mask, dest
                             0, 0, // src xy (in destination space!)
                             0, 0, // mask xy
                             x(), y(), rgbWidth_, rgbHeight_);

            XRenderFreePicture(fl_display, srcPic);
            XRenderFreePicture(fl_display, dstPic);
#endif
#else
            if ( (rgbWidth_==w()) && (rgbHeight_==h())) {
                fl_draw_image(rgbData_, x(), y(), rgbWidth_, rgbHeight_, 3);
            } else {
                // This code very inefficiently copies the RGB data,
                // scales it using the CPU, and then reders it to
                // the screen, which in turn may scale it onece
                // more in FLTK. It would be preferable to find an
                // accelerated GPU version or disable scaling.
                Fl_RGB_Image img(rgbData_, rgbWidth_, rgbHeight_);
                img.scale(w(), h(), 0, 1);
                img.draw(x(), y(), w(), h());
            }
#endif
        } else {
			super::draw();
        }
    }

	unsigned char *getRGBData() 
	{
		return rgbData_;
	}

	void redraw()
	{
		// thread safe version of redraw
		Fl::lock();
		Fl_Box::redraw();
		Fl::unlock();
		Fl::awake();
	}

	void unlinkSelf()
	{
		screenManager_ = 0L;
		Fl::lock();
		if (parent()) {
			parent()->remove(this);
			parent()->redraw();
		}
		Fl::delete_widget(this);
		Fl::unlock();
		Fl::awake();
	}

	int penXPos()
	{
		int mx = (Fl::event_x() - x()) * rgbWidth_ / w();
		if (mx<0) mx = 0;
		if (mx>=rgbWidth_) mx = rgbWidth_-1;
		penX = mx;
		return mx;
	}

	int penYPos()
	{
		int my = (Fl::event_y() - y()) * rgbHeight_ / h();
		if (my<0) my = 0;
		if (my>=rgbHeight_) my = rgbHeight_-1;
		penY = my;
		return my;
	}

	unsigned int eventKeyToMac() {
		unsigned int fltk = Fl::event_key();
//		::KTrace("Event key %d %c\n", fltk, fltk);
		if (fltk<256 && isalpha(fltk))
			fltk = toupper(fltk);
		int a = 0;
		int b = sizeof(vktab)/sizeof(*vktab);
		while (a < b) {
			int c = (a+b)/2;
			if (vktab[c].fltk == fltk) {
//				::KTrace("  returns %d 0x%02x\n", vktab[c].vk, vktab[c].vk);
				return vktab[c].vk;
			}
			if (vktab[c].fltk < fltk) a = c+1; else b = c;
		}
		return 127;
	}

	void penDownTimer()
	{
		if (penIsDown) {
			screenManager_->PenDown(penXPos(), penYPos());
			Fl::repeat_timeout(screenManager_->GetTabletSampleRate()/4000000.0, penDownTimerCB, this);
		} else {
			Fl::remove_timeout(penDownTimerCB, this);
		}
	}

	static void penDownTimerCB(void *me) {
		((Fl_Newton_Screen_Widget*)me)->penDownTimer();
	}

	int handle(int event) override
	{
		switch (event) {
			case FL_PUSH:
				screenManager_->PenDown(penXPos(), penYPos());
                //::KTrace("%d %d\n", penXPos(), penYPos());
				penIsDown = true;
				Fl::add_timeout(screenManager_->GetTabletSampleRate()/4000000.0, penDownTimerCB, this);
				return 1;
			case FL_DRAG:
				screenManager_->PenDown(penXPos(), penYPos());
				return 1;
			case FL_RELEASE:
                Fl::remove_timeout(penDownTimerCB, this);
				screenManager_->PenUp();
				penIsDown = false;
				return 1;
			case FL_KEYDOWN:
                // send Shift-Command key combinations back to FLTK, so the UI can handle them
				if ((Fl::event_state() & (FL_SHIFT | FL_CTRL | FL_ALT | FL_META)) == (FL_COMMAND | FL_SHIFT)) {
					// send key-up events for shift and ctrl modifiers to NewtonOS, so that the UI shortcut
					// becomes transparent and the following keystrokes have no modifiers for NewtonOS
					screenManager_->KeyUp(56); // shift
					if (FL_CTRL==FL_COMMAND)
						screenManager_->KeyUp(59); // ctrl on Windows and Linux
					else
						screenManager_->KeyUp(55); // meta on macOS
					return 0;
				}
                // allow for Command-Q to quit the app which is not used in NewtonOS AFAIK.
                if ( ((Fl::event_state()&(FL_SHIFT|FL_CTRL|FL_ALT|FL_META))==FL_COMMAND) && (Fl::event_key()=='q') )
                    return 0;
#if TARGET_OS_MAC
                // FLTK on MacOS sends only one keystroke down when the capslock is typed
                // and a key_up event when it is typed again
                if (Fl::event_key()==65509) {
                    screenManager_->KeyDown(eventKeyToMac());
                    screenManager_->KeyUp(eventKeyToMac());
                    return 1;
                }
#endif
                screenManager_->KeyDown(eventKeyToMac());
				return 1;
			case FL_KEYUP:
                // let Shift-Command key combinations through
                if ( (Fl::event_state()&(FL_SHIFT|FL_CTRL|FL_ALT|FL_META))==(FL_COMMAND|FL_SHIFT))
                    return 0;
                // allow for Command-Q to quit the app which is not used in NewtonOS AFAIK.
                if ( ((Fl::event_state()&(FL_SHIFT|FL_CTRL|FL_ALT|FL_META))==FL_COMMAND) && (Fl::event_key()=='q') )
                    return 0;
#if TARGET_OS_MAC
                if (Fl::event_key()==65509) {
                    screenManager_->KeyDown(eventKeyToMac());
                    screenManager_->KeyUp(eventKeyToMac());
                    return 1;
                }
#endif
				screenManager_->KeyUp(eventKeyToMac());
				return 1;
			case FL_FOCUS:
				return 1;
            case FL_DND_ENTER: // receive all DND events and ask for more
//                mApp->GetPlatformManager()->EvalNewtonScript
//                (
//                 "SetClipboard( { label: \"Test\", types: [ ['text] ], data: [ [ "
//                 "    { viewFont: 18435, textFlags: 4, offset: -6, text: \"Hallo, Welt!\", "
//                 "        viewBounds: { left: 109, top: 183, right: 145, bottom: 205 }"
//                 "    } "
//                 "] ], bounds: {left:109, right:145, top:183, bottom:205} } "
//                 ");"
//                 );
//                usleep(100000);
//                screenManager_->PenDown(2, 80);
//                penIsDown = true;
//                Fl::add_timeout(screenManager_->GetTabletSampleRate()/4000000.0, penDownTimerCB, this);
                return 1;
            case FL_DND_DRAG:
                //screenManager_->PenDown(penXPos(), penYPos());
                return 1;
            case FL_DND_LEAVE:
                // cancel the dnd command somehow
            case FL_DND_RELEASE:
//                Fl::remove_timeout(penDownTimerCB, this);
//                screenManager_->PenUp();
//                penIsDown = false;
//                return 1;
                // on MacOS, Fl::event_text() is empty
                // TODO: on other platforms, we may want to peek at the file extension and only allow valid types
                return 1;
            case FL_PASTE: // user dropped something onto the widget
                mApp->InstallPackagesFromURI(Fl::event_text());
                return 1;
		}
		return Fl_Box::handle(event);
	}

    /**
     Someone has resized this widget.

     This is a rather uncommon event because NewtonOS does not support resizing.
     One reason could be rotating the screen, so we let the system rotate and be fine.
     If there is an actual change in the number of pixels, we must either resize the screen buffer to
     hold more or less pixels, or we must scale the screen buffer to the widget, or we must crop
     or frame the original image.

     Also, things are different in fullscreen mode!
     */
    void resize(int x, int y, int w, int h) override
    {
		// As I am adding accelerated rendering on all platforms, I decided that the output simply scales with the render area,
		// so there is no need to change the rgb data parameters here. ::draw() will take care of the scaling.
		// On devices that don;t have hardware accelerated scaling, this may be a bit more complicated.
        super::resize(x, y, w, h);
    }

	void newRGBSize(int w, int h) {
		if (w * h != rgbWidth_ * rgbHeight_) {
			if (rgbData_) 
				::free(rgbData_);
			rgbData_ = (KUInt8*)calloc(4, w * h);
		} else {
			memset(rgbData_, 0, w * h);
		}
		rgbWidth_ = w;
		rgbHeight_ = h;
	}

    void PowerOn()
    {
        mPowerState = true;
        redraw();
    }

    void PowerOff()
    {
        mPowerState = false;
        redraw();
    }
};


// -------------------------------------------------------------------------- //
//  * GetDisplaySize( void )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::GetScreenSize(
			KUInt32* outWidth,
			KUInt32* outHeight)
{
	*outWidth = Fl::w()-Fl::x();
	*outHeight = Fl::h()-Fl::y();
}


// -------------------------------------------------------------------------- //
//  * TFLScreenManager( TLog* )
// -------------------------------------------------------------------------- //
TFLScreenManager::TFLScreenManager(
                                   TFLApp *inApp,
			TLog* inLog /* = nil */,
			KUInt32 inPortraitWidth /* = kDefaultPortraitWidth */,
			KUInt32 inPortraitHeight /* = kDefaultPortraitHeight */,
			bool inFullScreen /* = false */,
			bool inScreenIsLandscape /* = true */)
:	TScreenManager(
		inLog,
		inPortraitWidth,
		inPortraitHeight,
		inFullScreen,
		inScreenIsLandscape )
,	mApp(inApp)
,	mWidget(0L)
,	mScreenWasLandscape(inScreenIsLandscape)
{
	bool createWindow = (Fl_Group::current()==0L);

	if (createWindow) {
		new Fl_Window(inPortraitWidth, inPortraitHeight, "Einstein");
	}
	Fl_Group *parent = Fl_Group::current();

	int xo = 0, yo = 0;
	if (parent->type()<FL_WINDOW) {
		xo = parent->x();
		yo = parent->y();
	}

	mWidget = new Fl_Newton_Screen_Widget(
		xo, yo, GetScreenWidth(), GetScreenHeight(),
		0L, this, mApp);
	
	mWidget->label(
		"booting...\n"
		"\n"
		"right-click or\n"
		"ctrl-left-click\n"
		"to open the\n"
		"Einstein menu");
	mWidget->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
	mWidget->labelfont(FL_HELVETICA_BOLD);
	mWidget->labelsize(11);
	mWidget->labelcolor(FL_WHITE);

	if (createWindow) {
		Fl::get_system_colors();
		((Fl_Window*)parent)->show();
	}
}

// -------------------------------------------------------------------------- //
//  * ~TX11ScreenManager( void )
// -------------------------------------------------------------------------- //
TFLScreenManager::~TFLScreenManager( )
{
	//if (mWidget)
	//	mWidget->unlinkSelf();
}

// -------------------------------------------------------------------------- //
//  * PowerOn( void )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::PowerOn( void )
{
}

// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::PowerOff( void )
{
}

// -------------------------------------------------------------------------- //
//  * PowerOnScreen( void )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::PowerOnScreen( void )
{
    mWidget->label("");
    mWidget->PowerOn();
    gApp->PowerChangedEvent(true);
}


// -------------------------------------------------------------------------- //
//  * PowerOffScreen( void )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::PowerOffScreen( void )
{
    mWidget->label("Newton is sleeping...");
    mWidget->PowerOff();
    gApp->PowerChangedEvent(false);
}

// -------------------------------------------------------------------------- //
//  * BacklightChanged( bool )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::BacklightChanged( bool inState)
{
    gApp->BacklightChangedEvent(inState);
	UpdateScreenRect(0L);
}

// -------------------------------------------------------------------------- //
//  * ContrastChanged( KUInt32 )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::ContrastChanged( KUInt32 )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * ScreenOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::ScreenOrientationChanged( EOrientation inNewOrientation )
{
	static int newWidth, newHeight;

	bool screenWillBeLandscape = ((inNewOrientation & kOrientation_LandscapeBit) != 0);
	if (screenWillBeLandscape != mScreenWasLandscape) {
		newWidth = mWidget->h();
		newHeight = mWidget->w();
	} else {
		newWidth = mWidget->w();
		newHeight = mWidget->h();
	}
    Fl::awake(
              [](void *inScreenManager)->void
    {
        TFLScreenManager *mgr = (TFLScreenManager*)inScreenManager;
        mgr->mApp->ResizeFromNewton(newWidth, newHeight);
    },
              this
              );
	mWidget->newRGBSize(GetScreenWidth(), GetScreenHeight());
	mScreenWasLandscape = screenWillBeLandscape;
}

// -------------------------------------------------------------------------- //
//  * TabletOrientationChanged( EOrientation )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::TabletOrientationChanged( EOrientation )
{
	// Just ignore it.
}

// -------------------------------------------------------------------------- //
//  * UpdateScreenRect( SRect* )
// -------------------------------------------------------------------------- //
void
TFLScreenManager::UpdateScreenRect( SRect* inUpdateRect )
{
#if SRC_BITMAP_FORMAT_RGBA
	const int mBitsPerPixel = 32;
#else
	const int mBitsPerPixel = 24;
#endif

	KUInt16 top, left, bottom, right, height, width;
	if (inUpdateRect) {
		top = inUpdateRect->fTop;
		// clip the vertical bounds
		if (top < 0) top = 0;
		bottom = inUpdateRect->fBottom;
		if (bottom >= mWidget->getRGBHeight()) bottom = mWidget->getRGBHeight() - 1;
		if (top > bottom) return;
		height = bottom - top;
		// clip the horizontal bounds
		left = inUpdateRect->fLeft;
		if (left < 0) left = 0;
		right = inUpdateRect->fRight;
		if (right >= mWidget->getRGBWidth()) right = mWidget->getRGBWidth() - 1;
		if (left > right) return;
		width = right - left;
	} else {
		top = 0; 
		left = 0;
		height = mWidget->getRGBHeight();
		width = mWidget->getRGBWidth();
	}

	KUInt8 rs, gs, bs; 
	if (GetBacklight()) {
		rs = 1; gs = 0; bs = 1;
	} else {
		rs = 0; gs = 0; bs = 0;
	}

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
		((KUInt8*)mWidget->getRGBData())
		+ (top * dstRowBytes)
		+ (left * mBitsPerPixel / 8);

	int indexRows;

	for (indexRows = height; indexRows != 0; indexRows--)
	{
		KUInt8* srcCursor = srcRowPtr;
		KUInt8* srcEnd = srcRowPtr + srcWidthInBytes;
		KUInt8* dstCursor = dstRowPtr;
		do {
			KUInt8 theByte = *srcCursor++;
			// First pixel
			KUInt8 thePixel = (theByte & 0xF0) | (theByte>>4);
			*dstCursor++ = thePixel>>rs;
			*dstCursor++ = thePixel>>gs;
			*dstCursor++ = thePixel>>bs;
			if (mBitsPerPixel == 32) *dstCursor++ = 0;
			// Second pixel
			thePixel = (theByte<<4) | (theByte & 0x0f);
			*dstCursor++ = thePixel>>rs;
			*dstCursor++ = thePixel>>gs;
			*dstCursor++ = thePixel>>bs;
			if (mBitsPerPixel == 32) *dstCursor++ = 0;
		} while (srcCursor < srcEnd);
		srcRowPtr += srcRowBytes;
		dstRowPtr += dstRowBytes;
	}

	mWidget->redraw();
}

Fl_Widget *TFLScreenManager::GetWidget()
{
    return mWidget;
}

// ========================================================================= //
// The most likely way for the world to be destroyed, most experts agree, is 
// by accident. That's where we come in; we're computer professionals. We 
// cause accidents.
// ========================================================================= //
