// ==============================
// File:			TFLScreenManager.h
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

#ifndef _TFLSCREENMANAGER_H
#define _TFLSCREENMANAGER_H

#include <K/Defines/KDefinitions.h>
#include "TScreenManager.h"

class Fl_Newton_Screen_Widget;
class TFLApp;
class Fl_Widget;

///
/// Class for a screen manager using FLTK.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \author Matthias Melcher <einstein@robowerk.com>
///
class TFLScreenManager
	:
		public TScreenManager
{
	friend Fl_Newton_Screen_Widget;
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
	TFLScreenManager(
                     TFLApp *inApp,
				TLog* inLog = nil,
				KUInt32 inPortraitWidth = kDefaultPortraitWidth,
				KUInt32 inPortraitHeight = kDefaultPortraitHeight,
				Boolean inFullScreen = false,
				Boolean inScreenIsLandscape = true);


	///
	/// Destructeur.
	///
	virtual ~TFLScreenManager( void );

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

    Fl_Widget *GetWidget();

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TFLScreenManager( const TFLScreenManager& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TFLScreenManager& operator = ( const TFLScreenManager& inCopy );

	void unlinkWidget() { mWidget = 0L; }
	
    TFLApp *mApp = nullptr;
    
	///
	/// FLTK Widget class that will hold the Newton screen image
	///
	Fl_Newton_Screen_Widget	*mWidget;

	// Store the value of the last time that the orientation was set.
	Boolean mScreenWasLandscape = false;

};

#endif
		// _TFLSCREENMANAGER_H

// ============================================================= //
// To err is human -- to blame it on a computer is even more so. //
// ============================================================= //
