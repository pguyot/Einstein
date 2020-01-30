// ==============================
// File:			TScreenManager.h
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

#ifndef _TSCREENMANAGER_H
#define _TSCREENMANAGER_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TInterruptManager;
class TMemory;
class TPlatformManager;
class TStream;

#define SIXTEEN_GREYS					1
#define	TWOHUNDREDANDFIFTYSIX_GREYS		0
#define MILLIONS_OF_COLORS				0

///
/// Class to handle the display and the tablet.
/// The tablet needs two event handlers:
/// -> pen down at x,y with pressure p
/// -> pen up
///
/// By default, pressure is 4. Time is also 0 by default (the screen manager
/// will ask the current time to the interrupt manager).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test d�fini.
///
class TScreenManager
{
public:
	///
	/// Rect structure (as defined in QD)
	///
	struct SRect {
		KUInt16	fTop;
		KUInt16	fLeft;
		KUInt16	fBottom;
		KUInt16	fRight;
	};
	
	///
	/// Screen & tablet orientations.
	///
	enum EOrientation {
		kOrientation_AppleTop		= 0x00000000,
		kOrientation_AppleRight		= 0x00000001,
		kOrientation_AppleBottom	= 0x00000002,
		kOrientation_AppleLeft		= 0x00000003,
		kOrientation_LandscapeBit	= 0x00000001,
		kOrientation_Default		= kOrientation_AppleBottom
	};
	
	///
	/// Tablet states.
	/// I simplified states 1-7 since the system doesn't seem to care
	/// about them.
	///
	enum ETabletState {
		kPenIsUp						= 0,
		kPenIsDown						= 1,
		kTabletIsBypassed				= 8,
		kTabletIsOff					= 9
	};

	///
	/// Other constants relative to the screen & the tablet.
	///
	enum {
		kDefaultContrast				= 0xFFFFFFF8,
		kDefaultBacklight				= false	/* off */,
		kDefaultPortraitWidth			= 320,
		kDefaultPortraitHeight			= 480,
#if SIXTEEN_GREYS
		kBitsPerPixel					= 4,
#endif
#if TWOHUNDREDANDFIFTYSIX_GREYS
		kBitsPerPixel					= 8,
#endif
#if MILLIONS_OF_COLORS
		kBitsPerPixel					= 32,
#endif
		kDefaultSampleRate				= 0x0000B400
	};
	
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
	TScreenManager(
				TLog* inLog = nil,
				KUInt32 inPortraitWidth = kDefaultPortraitWidth,
				KUInt32 inPortraitHeight = kDefaultPortraitHeight,
				Boolean inFullScreen = false,
				Boolean inScreenIsLandscape = true );

	///
	/// Destructor.
	///
	virtual ~TScreenManager( void );

	///
	/// Set the interrupt manager.
	/// This method is called once the interrupt manager is created.
	///
	/// \param inManager	reference to the interrupt manager
	///
	void	SetInterruptManager( TInterruptManager* inManager )
		{
			mInterruptManager = inManager;
		}

	///
	/// Set the memory interface.
	/// This method is called once the memory interface is created.
	///
	/// \param inManager	reference to the memory interface
	///
	void	SetMemory( TMemory* inMemory )
		{
			mMemory = inMemory;
		}

	///
	/// Set the platform manager.
	/// This method is called once the platform manager is created.
	///
	/// \param inManager	reference to the platform manager.
	///
	void	SetPlatformManager( TPlatformManager* inPlatformManager )
		{
			mPlatformManager = inPlatformManager;
		}

	///
	/// Wake up tablet.
	/// This method is called by the tablet driver.
	///
	void	WakeUpTablet( void );

	///
	/// Shut down tablet.
	/// This method is called by the tablet driver.
	///
	void	ShutDownTablet( void );

	///
	/// Start bypassing the tablet. Events are ignored.
	/// This method is called by the tablet driver.
	///
	void	StartBypassTablet( void );

	///
	/// Stop bypassing the tablet. Events are no longer ignored.
	/// This method is called by the tablet driver.
	///
	void	StopBypassTablet( void );

	///
	/// Get the current tablet sample rate.
	/// This method is called by the tablet driver.
	///
	KUInt32	GetTabletSampleRate( void ) const
		{
			return mTabletSampleRate;
		}

	///
	/// Set the tablet sample rate.
	/// This method is called by the tablet driver.
	///
	/// \param inSampleRate	the new sample rate.
	///
	void	SetTabletSampleRate( KUInt32 inSampleRate )
		{
			mTabletSampleRate = inSampleRate;
		}

	///
	/// Get the tablet state.
	/// This method is called by the tablet driver.
	///
	ETabletState	GetTabletState( void ) const;

	///
	/// Access a sample.
	/// This method is called by the tablet driver.
	///
	/// \param outPackedSample	packed sample for the tablet journal.
	/// \param outTimeInTicks	time in ticks.
	/// \return \c true if there was a sample left, \c false otherwise.
	///
	Boolean	GetSample( KUInt32* outPackedSample, KUInt32* outTimeInTicks );

	///
	/// Notify that the tablet orientation changed.
	/// This method is called when the tablet driver calls SetTabletOrientation.
	///
	/// \param inNewOrientation	the new orientation of the screen.
	///
	virtual void	TabletOrientationChanged(
						EOrientation inNewOrientation ) = 0;

	///
	/// Get the screen orientation.
	/// This method is called by the tablet driver.
	///
	EOrientation	GetTabletOrientation( void ) const
		{
			return mTabletOrientation;
		}

	///
	/// Set the tablet orientation.
	/// This method is called by the tablet driver.
	///
	void			SetTabletOrientation( EOrientation inOrientation )
		{
			if (mTabletOrientation != inOrientation)
			{
				mTabletOrientation = inOrientation;
				TabletOrientationChanged( inOrientation );
			}
		}

	///
	/// This method is called by the platform manager when the emulator is
	/// turned on.
	///
	virtual void	PowerOn( void ) = 0;

	///
	/// This method is called by the platform manager when the emulator is
	/// turned off.
	///
	virtual void	PowerOff( void ) = 0;

	///
	/// Power on the screen (open the window?)
	/// This method is called by the display driver.
	///
	virtual void	PowerOnScreen( void ) = 0;

	///
	/// Power off the screen (close the window?)
	/// This method is called by the display driver.
	///
	virtual void	PowerOffScreen( void ) = 0;

	///
	/// Notify that the screen orientation changed.
	/// This method is called when the display driver calls SetScreenOrientation.
	///
	/// \param inNewOrientation	the new orientation of the screen.
	///
	virtual void	ScreenOrientationChanged(
						EOrientation inNewOrientation ) = 0;

	///
	/// Notify that the contrast changed.
	/// This method is called when the display driver calls SetScreenContrast.
	///
	/// \param inNewContrast the new contrast of the screen.
	///
	virtual void	ContrastChanged( KUInt32 inNewContrast ) = 0;

	///
	/// Notify that the backlight changed.
	/// This method is called when the display driver calls SetScreenContrast.
	///
	/// \param inNewBacklight the new state of the backlight.
	///
	virtual void	BacklightChanged( Boolean inNewBacklight ) = 0;

	///
	/// Get the screen orientation.
	/// This method is called by the display driver.
	///
	EOrientation	GetScreenOrientation( void ) const
		{
			return mScreenOrientation;
		}

	///
	/// Set the screen orientation.
	/// This method is called by the display driver.
	///
	void			SetScreenOrientation(
						EOrientation inOrientation )
		{
			if (mScreenOrientation != inOrientation)
			{
				mScreenOrientation = inOrientation;
				ScreenOrientationChanged( inOrientation );
			}
		}

	///
	/// Get the contrast of the screen.
	/// This method is called by the display driver.
	///
	/// \return the contrast of the screen.
	///
	KUInt32	GetContrast( void ) const
		{
			return mContrast;
		}

	///
	/// Set the contrast of the screen.
	/// This method is called by the display driver.
	///
	/// \param inContrast	new contrast of the screen.
	///
	void	SetContrast( KUInt32 inContrast )
		{
			if (inContrast != mContrast)
			{
				mContrast = inContrast;
				ContrastChanged( inContrast );
			}
		}

	///
	/// Get the state of the backlight.
	/// This method is called by the display driver.
	///
	Boolean	GetBacklight( void ) const
		{
			return mBacklight;
		}

	///
	/// Set the state of the backlight.
	/// This method is called by the display driver.
	///
	void	SetBacklight( Boolean inBacklight )
		{
			if (inBacklight != mBacklight)
			{
				mBacklight = inBacklight;
				BacklightChanged( inBacklight );
			}
		}

	///
	/// Notify that some screen bits changed.
	///
	/// \param inUpdateRect	rectangle of the bits that changed.
	///
	virtual void	UpdateScreenRect( SRect* inUpdatedRect ) = 0;

	///
	/// Get the screen width (from the orientation)
	///
	/// \return the screen width (from the current orientation)
	///
	KUInt32	GetScreenWidth( void ) const
		{
			if (mScreenOrientation & kOrientation_LandscapeBit)
			{
				return mPortraitHeight;
			} else {
				return mPortraitWidth;
			}
		}

	///
	/// Get the screen height (from the orientation)
	///
	/// \return the screen height (from the current orientation)
	///
	KUInt32	GetScreenHeight( void ) const
		{
			if (mScreenOrientation & kOrientation_LandscapeBit)
			{
				return mPortraitWidth;
			} else {
				return mPortraitHeight;
			}
		}

	///
	/// Get the actual screen width (physical or virtual)
	///
	/// \return the actual screen width
	///
	KUInt32	GetActualScreenWidth( void ) const
		{
			if (mFullScreen)
			{
				return mPhysicalWidth;
			} else {
				return GetScreenWidth();
			}
		}

	///
	/// Get the actual screen height (physical or virtual)
	///
	/// \return the actual screen height
	///
	KUInt32	GetActualScreenHeight( void ) const
		{
			if (mFullScreen)
			{
				return mPhysicalHeight;
			} else {
				return GetScreenHeight();
			}
		}

	///
	/// Determine if the screen is full screen.
	///
	/// \return true if the display is full screen.
	///
	Boolean	IsFullScreen( void ) const
		{
			return mFullScreen;
		}

	///
	/// Update some screen bits.
	/// This method is called by the display driver.
	///
	void		Blit(
						KUInt32 inPixelMapAddr,
						SRect* inSrcRect,
						SRect* inDstRect,
						KUInt32 inMode );

	///
	/// Send a key down event to the Newton.
	///
	void		KeyDown( KUInt8 inKeyCode );

	///
	/// Send a key up event to the Newton.
	///
	void		KeyUp( KUInt8 inKeyCode );

	///
	/// Send a key repeat event to the Newton.
	///
	void		KeyRepeat( KUInt8 inKeyCode );
  
    ///
    /// Full screen emulators can use this to offer interactive host functionality.
    ///
    virtual void OpenEinsteinMenu() { }
	
	///
	/// Save or restore the state to and from a stream.
	///
	void		TransferState( TStream* inStream );
	
	///
	/// Change Screen Size.
	///
	void		ChangeScreenSize(int inWidth, int inHeight);
	
	///
	/// Switch the status overlay on
	///
	void		OverlayOn();
	
	///
	/// Switch the status overlay off
	///
	void		OverlayOff();
	
	///
	/// Switch the status overlay on
	///
	Boolean		OverlayIsOn() { return mOverlayIsOn; }
	
	///
	/// Push the overlay changes to the screen
	///
	void		OverlayFlush();
	
	///
	/// Clear the contents of the overlayplane
	///
	void		OverlayClear(KSInt32 line=-1);
	
	///
	/// print some text into the status overlay
	///
	void		OverlayPrintAt(KSInt32 x, KSInt32 y, const char *text, Boolean centered=0);
	
	///
	/// print a progress bar
	///
	void		OverlayPrintProgress(KSInt32 y, KUInt32 percent);
	

protected:
	///
	/// Accessor on the interface to the log (may be null).
	///
	TLog*	GetLog( void ) const
		{
			return mLog;
		}

	///
	/// Accessor on the interface to the interrupt manager.
	///
	TInterruptManager*	GetInterruptManager( void ) const
		{
			return mInterruptManager;
		}

	///
	/// Accessor on the interface to the memory.
	///
	TMemory*	GetMemory( void ) const
		{
			return mMemory;
		}
  
public:
	///
	///	Insert a sample with pen down.
	/// The coordinates are rotated if required.
	///
	/// \param inXCoord			x coordinate of the pen (11 bits)
	/// \param inYCoord			y coordinate of the pen (11 bits)
	/// \param inPressure		pressure of the event (0-7)
	/// \param inTimeInTicks	time of the event (in ticks, 0 ask IntMgr)
	///
	void	PenDown(
				KUInt16 inXCoord,
				KUInt16 inYCoord,
				KUInt8 inPressure = 4,
				KUInt32 inTimeInTicks = 0 );

	///
	///	The pen was lifted.
	///
	/// \param inTimeInTicks	time of the event (in ticks, 0 ask IntMgr)
	///
	void	PenUp( KUInt32 inTimeInTicks = 0 );

	///
	/// Accessor on the screen buffer.
	///
	KUInt8*	GetScreenBuffer( void ) const
		{
			return mScreenBuffer;
		}

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TScreenManager( const TScreenManager& inCopy );

	///
	/// Op�rateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet � copier
	///
	TScreenManager& operator = ( const TScreenManager& inCopy );

	///
	/// Insert sample to the buffer.
	///
	/// \param inPackedSample	packed sample for the tablet journal.
	/// \param inTimeInTicks	time in ticks (0 ask intMgr)
	///
	void	InsertSample( KUInt32 inPackedSample, KUInt32 inTimeInTicks = 0 );

	///
	/// Raise input to wake the tablet driver.
	///
	void	RaiseTabletInterrupt( void ) const;

	///
	/// Update some screen bits, with no rotation.
	///
	inline void	Blit_0(
						KUInt32 inBaseAddy,
						KUInt32 inSrcRowBytes,
						KUInt16 inPixmapTop,
						KUInt16 inPixmapLeft,
						SRect* inSrcRect,
						SRect* inDstRect,
						KUInt32 inMode );

	///
	/// Update some screen bits, with a 90 clockwise rotation.
	///
	inline void	Blit_90(
						KUInt32 inBaseAddy,
						KUInt32 inSrcRowBytes,
						KUInt16 inPixmapTop,
						KUInt16 inPixmapLeft,
						SRect* inSrcRect,
						SRect* inDstRect,
						KUInt32 inMode );

	///
	/// Update some screen bits, with a 180 clockwise rotation.
	///
	inline void	Blit_180(
						KUInt32 inBaseAddy,
						KUInt32 inSrcRowBytes,
						KUInt16 inPixmapTop,
						KUInt16 inPixmapLeft,
						SRect* inSrcRect,
						SRect* inDstRect,
						KUInt32 inMode );

	///
	/// Update some screen bits, with a 90 counter-clockwise rotation.
	///
	inline void	Blit_270(
						KUInt32 inBaseAddy,
						KUInt32 inSrcRowBytes,
						KUInt16 inPixmapTop,
						KUInt16 inPixmapLeft,
						SRect* inSrcRect,
						SRect* inDstRect,
						KUInt32 inMode );

	/// \name Variables
	TLog*				mLog;				///< Reference to the log.
	TInterruptManager*	mInterruptManager;	///< Reference to the interrupt mgr.
	TMemory*			mMemory;			///< Interface to the memory.
	TPlatformManager*	mPlatformManager;	///< Reference to the platform mgr.
	KUInt32				mPortraitWidth;		///< Width in portrait mode.
	KUInt32				mPortraitHeight;	///< Height in portrait mode.
	KUInt32				mPhysicalWidth;		///< Actual screen width (full screen).
	KUInt32				mPhysicalHeight;	///< Actual screen height (full screen).
	Boolean				mFullScreen;		///< Full screen mode.
	Boolean				mScreenIsLandscape;	///< Whether screen is landscape.
	Boolean				mBypassTablet;		///< Whether we bypass the tablet.
	Boolean				mTabletIsDown;		///< Whether the tablet is down.
	Boolean				mPenIsDown;			///< If pen is down.
	KUInt32				mTabletSampleRate;	///< Sample rate (in ticks) of the
											///< tablet.
	KUInt32*			mTabletBuffer;		///< (Circular) buffer for the
											///< tablet samples.
	KUInt32				mTabletBufCCrsr;	///< Consumer buffer cursor (Newton)
	KUInt32				mTabletBufPCrsr;	///< Producer buffer cursor (Host)
	EOrientation		mTabletOrientation;	///< Current tablet orientation.
	EOrientation		mScreenOrientation;	///< Current screen orientation.
	KUInt32				mContrast;			///< Current screen contrast.
	Boolean				mBacklight;			///< Current screen backlight.
	Boolean				mKbdIsConnected;	///< If keyboard is connected.
	KUInt8*				mScreenBuffer;		///< Buffer of the screen.
	
protected:
	Boolean				mOverlayIsOn;		///< Show overlay on screen
	Boolean				mOverlayIsDirty[4];	///< There is something visible in the overlay buffer line
	char				mOverlay[4][40];	///< Overlay text buffer
	SRect				mOverlayRect;		///< Position and size of overlay on screen
	static KUInt8		mFontData[128][13]; ///< Pixeldata for overlay font
};

#endif
		// _TSCREENMANAGER_H

// =================================================== //
// Can't open /usr/fortunes.  Lid stuck on cookie jar. //
// =================================================== //
