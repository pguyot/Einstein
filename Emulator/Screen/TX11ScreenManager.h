// ==============================
// File:			TX11ScreenManager.h
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

#ifndef _TX11SCREENMANAGER_H
#define _TX11SCREENMANAGER_H

#include <K/Defines/KDefinitions.h>
#include "TScreenManager.h"

// X11 - conflict with Quickdraw.
#ifdef __QUICKDRAW__
namespace X11NameSpace {
#endif
#include <X11/Xlib.h>
#ifdef __QUICKDRAW__
};
#endif

#ifdef __QUICKDRAW__
#define X11Prefix X11NameSpace::
#else
#define X11Prefix
#endif

class TThread;

///
/// Class for a screen manager on X11.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TX11ScreenManager
	:
		public TScreenManager
{
public:
	///
	/// Get the dimension of the screen.
	///
	static void	GetScreenSize(
			KUInt32* outWidth,
			KUInt32* outHeight );

	///
	/// Constructor from a log and dimensions.
	///
	/// \param inLog				log interface (can be null)
	/// \param inPortraitWidth		width (in portrait mode).
	/// \param inPortraitHeight		height (in portrait mode).
	/// \param inFullScreen			whether we're full screen (and we do
	///								emulate the rotation).
	/// \param inScreenIsLandscape	whether the physical screen is in landscape.
	///
	TX11ScreenManager(
				TLog* inLog = nil,
				KUInt32 inPortraitWidth = kDefaultPortraitWidth,
				KUInt32 inPortraitHeight = kDefaultPortraitHeight,
				Boolean inFullScreen = false,
				Boolean inScreenIsLandscape = true);


	///
	/// Destructeur.
	///
	virtual ~TX11ScreenManager( void );

	///
	/// Notify that the tablet orientation changed.
	/// This method is called when the tablet driver calls SetTabletOrientation.
	///
	/// \param inNewOrientation	the new orientation of the screen.
	///
	virtual void	TabletOrientationChanged(
						EOrientation inNewOrientation );

	///
	/// This method is called by the platform manager when the emulator is
	/// turned on.
	///
	virtual void	PowerOn( void );

	///
	/// This method is called by the platform manager when the emulator is
	/// turned off.
	///
	virtual void	PowerOff( void );

	///
	/// Power on the screen (open the window?)
	/// This method is called by the display driver.
	/// It doesn't do anything. The work is done in ScreenSetup.
	///
	virtual void	PowerOnScreen( void );

	///
	/// Power off the screen (close the window?)
	/// This method is called by the display driver.
	///
	virtual void	PowerOffScreen( void );

	///
	/// Notify that the screen orientation changed.
	/// This method is called when the display driver calls SetScreenOrientation.
	///
	/// \param inNewOrientation	the new orientation of the screen.
	///
	virtual void	ScreenOrientationChanged(
						EOrientation inNewOrientation );

	///
	/// Notify that the contrast changed.
	/// This method is called when the display driver calls SetScreenContrast.
	///
	/// \param inNewContrast the new contrast of the screen.
	///
	virtual void	ContrastChanged( KUInt32 inNewContrast );

	///
	/// Notify that the backlight changed.
	/// This method is called when the display driver calls SetScreenContrast.
	///
	/// \param inNewBacklight the new state of the backlight.
	///
	virtual void	BacklightChanged( Boolean inNewBacklight );

	///
	/// Notify that some screen bits changed.
	///
	/// \param inUpdateRect	rectangle of the bits that changed.
	///
	virtual void	UpdateScreenRect( SRect* inUpdatedRect );

	///
	/// Thread loop entry point.
	///
	void	Run( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TX11ScreenManager( const TX11ScreenManager& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TX11ScreenManager& operator = ( const TX11ScreenManager& inCopy );

	///
	/// Create the event thread.
	///
	void	CreateEventThread( void );

	///
	/// Join and destroy the event thread.
	///
	void	JoinAndDestroyEventThread( void );

	///
	/// Connect to the display server.
	///
	void	OpenDisplay( void );

	///
	/// Disconnect from the display server.
	///
	void	CloseDisplay( void );

	///
	/// Do connect to the display server.
	///
	static X11Prefix Display*	DoOpenDisplay( void );

	///
	/// Do disconnect from the display server.
	///
	static void	DoCloseDisplay( X11Prefix Display* inDisplay );

	///
	/// Set up the window.
	///
	void	SetupWindow( void );

	///
	/// Make window unresizable by changing the hints.
	///
	void	MakeWindowUnresizable( void );

	///
	/// Set up the window in the current rotation.
	///
	void	SetupRotateWindow( void );

	///
	/// Find the best visual for our needs.
	///
	void	FindVisual( void );

	///
	/// Create the palette (and allocate the colors if we do DirectColor).
	///
	void	CreatePalette( void );

	///
	/// Dispose the palette (especially if we do DirectColor).
	///
	void	FreePalette( void );

	///
	/// Translate key codes (from X11 to Newton).
	///
	KUInt8	TranslateKeyCode( XEvent inEvent );

	///
	/// Load the key codes translation (from a file or whatever).
	///
	void	LoadKeyCodesTranslation( void );

	/// \name Variables
	X11Prefix Display*		mDisplay;			///< X11 display
	X11Prefix Window		mWindow;			///< The X11 window
	X11Prefix Window		mRootWindow;		///< The X11 root window
	TThread*				mThread;			///< The X11 event thread.
	Boolean					mPowerIsOn;			///< Whether the window is opened.
	X11Prefix Atom			mWMDeleteWindow;	///< Window manager delete protocol.
	X11Prefix Atom			mWMState;			///< Window manager state message.
	X11Prefix Atom			mWMFullScreen;		///< Window manager full screen state.
	X11Prefix XImage*		mImage;				///< Image to draw to.
	X11Prefix GC			mGC;				///< Graph context.
	X11Prefix Visual*		mVisual;			///< Visual for the image.
	KUInt32					mBitsPerPixel;		///< Bits per pixel for the image.
	KUInt32					mDepth;				///< Depth of the image.
	KUInt32					mRedMask;			///< Red mask for the visual.
	KUInt32					mBlueMask;			///< Blue mask for the visual.
	KUInt32					mGreenMask;			///< Green mask for the visual.
	Boolean					mAllocColor;		///< If we use a palette.
	char*					mImageBuffer;		///< Image buffer.
	X11Prefix XColor		mColors[32];		///< Grays & greens.
	KUInt32					mPalette[32];		///< Grays & greens, as stored.
	static const KUInt8		mKeycodes[128];		///< Keycodes.
};

#endif
		// _TX11SCREENMANAGER_H

// ============================================================= //
// To err is human -- to blame it on a computer is even more so. //
// ============================================================= //
