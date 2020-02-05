// ==============================
// File:			TCocoaScreenManager.h
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

#ifndef _TCOCOASCREENMANAGER_H
#define _TCOCOASCREENMANAGER_H

#include <K/Defines/KDefinitions.h>
#include "TScreenManager.h"
#include "CocoaScreenProxy.h"

// CoreGraphics via ApplicationServices
#include <ApplicationServices/ApplicationServices.h>

// ObjC
#include <objc/objc.h>

class TPlatformManager;

///
/// Class for a screen manager for Cocoa.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 121 $
///
/// \test	aucun test défini.
///
class TCocoaScreenManager
	:
		public TScreenManager
{
public:
	///
	/// Constructor from a proxy, the app, a log and dimensions.
	///
	/// \param inProxy				Cocoa thread-safe proxy.
	/// \param inEmulatorApp		Cocoa application controller.
	/// \param inLog				log interface (can be null)
	/// \param inPortraitWidth		width (in portrait mode).
	/// \param inPortraitHeight		height (in portrait mode).
	///
	TCocoaScreenManager(
				CocoaScreenProxy* inProxy,
				id inEmulatorApp,
				TLog* inLog = nil,
				KUInt32 inPortraitWidth = kDefaultPortraitWidth,
				KUInt32 inPortraitHeight = kDefaultPortraitHeight,
				Boolean inFullScreen = false,
				Boolean inScreenIsLandscape = true);

	///
	/// Destructeur.
	///
	virtual ~TCocoaScreenManager( void );

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
	/// Selector to the platform manager.
	///
	/// \param inPlatformManager a reference to the platform manager.
	///
	void			SetPlatformManager( TPlatformManager* inPlatformManager )
		{
			mPlatformManager = inPlatformManager;
		}

	///
	/// Accessor to the data provider.
	///
	/// \return a reference to the data provider.
	///
	CGDataProviderRef		GetDataProvider( void )
		{
			return mDataProviderRef;
		}

	///
	///	Insert a sample with pen down.
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
				KUInt32 inTimeInTicks = 0 )
		{
			TScreenManager::PenDown( inXCoord, inYCoord, inPressure, inTimeInTicks );
		}

	///
	///	The pen was lifted.
	///
	/// \param inTimeInTicks	time of the event (in ticks, 0 ask IntMgr)
	///
	void	PenUp( KUInt32 inTimeInTicks = 0 )
		{
			TScreenManager::PenUp( inTimeInTicks );
		}

	///
	///	The power switch was handled.
	///
	void	PowerSwitch( void );

	///
	///	A file was dragged on the window.
	///
	/// \param inPath	path of the file.
	///
	void	DraggedFile( const char* inPath );

	///
	///	Direct access to teh RGB buffer.
	///
	KUInt32* GetImageBuffer() { return mImageBuffer; }


private:
	/// \name Variables
	id						mProxy;					///< Cocoa proxy.
	id						mEmulatorApp;			///< Cocoa application controller.
	id						mEmulatorWindow;		///< Window of the emulator.
	id						mEmulatorScreenView;	///< View of the emulator.
	id						mEmulatorScreenText;	///< "Screen if off" text.
	TPlatformManager*		mPlatformManager;		///< Reference to the platform manager.
	Boolean					mPowerIsOn;				///< Whether the window is opened.
	KUInt32*				mImageBuffer;			///< Image buffer.
	CGDataProviderRef		mDataProviderRef;		///< Direct access data provider.
};

#endif
		// _TCOCOASCREENMANAGER_H

// =========================================================================== //
// COMPASS [for the CDC-6000 series] is the sort of assembler one expects from //
// a corporation whose president codes in octal.                               //
//                 -- J.N. Gray                                                //
// =========================================================================== //
