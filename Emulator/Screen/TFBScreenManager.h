// ==============================
// File:			TFBScreenManager.h
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

#ifndef _TFBSCREENMANAGER_H
#define _TFBSCREENMANAGER_H

#include <K/Defines/KDefinitions.h>
#include "TScreenManager.h"

///
/// Class for a fbdev screen manager.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 150 $
///
/// \test	aucun test défini.
///
class TFBScreenManager
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
	///
	TFBScreenManager(
				TLog* inLog = nil,
				KUInt32 inPortraitWidth = kDefaultPortraitWidth,
				KUInt32 inPortraitHeight = kDefaultPortraitHeight );


	///
	/// Destructor.
	///
	virtual ~TFBScreenManager( void );

	///
	/// Notify that the tablet orientation changed.
	/// This method is called when the tablet driver calls SetTabletOrientation.
	///
	/// \param inNewOrientation	the new orientation of the screen.
	///
	virtual void	TabletOrientationChanged(
						EOrientation inNewOrientation );

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

private:
	///
	/// Open the frame buffer.
	///
	void			Open( void );
	
	///
	/// Close the frame buffer.
	///
	void			Close( void );
	
	/// \name Variables
	int				mFrameBufferFD;	///< fd for the fb
};

#endif
		// _TFBSCREENMANAGER_H

// ====================================================================== //
// Has everyone noticed that all the letters of the word "database" are   //
// typed with the left hand?  Now the layout of the QWERTYUIOP typewriter //
// keyboard was designed, among other things, to facilitate the even use  //
// of both hands.  It follows, therefore, that writing about databases is //
// not only unnatural, but a lot harder than it appears.                  //
// ====================================================================== //
