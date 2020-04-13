// ==============================
// File:			TAndroidNativeScreenManager.h
// Project:			Einstein
//
// Copyright 2011 by Matthias Melcher (einstein@matthiasm.com).
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

#ifndef _TANDROID_NATIVE_SCREENMANAGER_H
#define _TANDROID_NATIVE_SCREENMANAGER_H

#include <K/Defines/KDefinitions.h>
#include <android/rect.h>
#include "TScreenManager.h"

class TThread;

///
/// Class for a screen manager using FLTK.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \author Matthias Melcher <einstein@matthism.com>
///
class TAndroidNativeScreenManager : public TScreenManager
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
    TAndroidNativeScreenManager(
            TLog* inLog = nil,
            KUInt32 inPortraitWidth = kDefaultPortraitWidth,
            KUInt32 inPortraitHeight = kDefaultPortraitHeight,
            bool inFullScreen = true,
            bool inScreenIsLandscape = true);


    ///
    /// Destructeur.
    ///
    virtual ~TAndroidNativeScreenManager( void );

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
    virtual void	ScreenOrientationChanged( EOrientation inNewOrientation );

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
    virtual void	BacklightChanged( bool inNewBacklight );

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

    int update(unsigned short *buffer, const ARect &r);

    int updateOverlay(unsigned short *buffer);

private:
    ///
    /// Constructeur par copie volontairement indisponible.
    ///
    /// \param inCopy		objet � copier
    ///
    TAndroidNativeScreenManager( const TAndroidNativeScreenManager& inCopy );

    ///
    /// Op�rateur d'assignation volontairement indisponible.
    ///
    /// \param inCopy		objet � copier
    ///
    TAndroidNativeScreenManager& operator = ( const TAndroidNativeScreenManager& inCopy );
};


#endif
// _TANDROID_NATIVE_SCREENMANAGER_H

// ============================================================= //
// To err is human -- to blame it on a computer is even more so. //
// ============================================================= //
//
