// ==============================
// File:			TNullScreenManager.h
// Project:			Einstein
//
// Copyright 2013-2014 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TNULLSCREENMANAGER_H
#define _TNULLSCREENMANAGER_H

#include <K/Defines/KDefinitions.h>
#include "Emulator/Screen/TScreenManager.h"

///
/// Class for a null screen manager.
///
class TNullScreenManager
	:
		public TScreenManager
{
public:
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
	TNullScreenManager(
				TLog* inLog = nil,
				KUInt32 inPortraitWidth = kDefaultPortraitWidth,
				KUInt32 inPortraitHeight = kDefaultPortraitHeight,
				Boolean inFullScreen = false,
				Boolean inScreenIsLandscape = true)
		:
		    TScreenManager(inLog, inPortraitWidth, inPortraitHeight, inFullScreen, inScreenIsLandscape) {};


	///
	/// Destructeur.
	///
	virtual ~TNullScreenManager( void ) {};

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
};

#endif
		// _TNULLSCREENMANAGER_H
