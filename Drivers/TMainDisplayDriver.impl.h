// ==============================
// File:			TMainDisplayDriver.impl.h
// Project:			Einstein
//
// Copyright 2003-2013 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TMAINSCREENDRIVER_IMPL_H
#define _TMAINSCREENDRIVER_IMPL_H

#include "TScreenDriver.h"

#include "NewtonGestalt.h"

///
/// Class for our screen driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
PROTOCOL TMainDisplayDriver : public TScreenDriver
	PROTOCOLVERSION(3.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(TMainDisplayDriver);

	CAPABILITIES( ("copyright" "Paul Guyot") ("version" "1.0") )

	void	Delete();
	ULong	ScreenSetup();
	ULong	GetScreenInfo(ScreenInfo*);
	ULong	PowerInit();
	ULong	PowerOn();
	ULong	PowerOff();
	ULong	Blit(PixelMap*, Rect*, Rect*, long);
	long	GetFeature(long);
	void	SetFeature(long, long);
	ULong	AutoAdjustFeatures();
	ULong	DoubleBlit(PixelMap*, PixelMap*, Rect*, Rect*, long);
	ULong	EnterIdleMode();
	ULong	ExitIdleMode();

private:
	TGestaltSoftContrast	mGestaltSoftContrast;
	TGestaltBacklight		mGestaltBacklight;
};

#endif
		// _TMAINSCREENDRIVER_IMPL_H

// ================================================ //
// This is an unauthorized cybernetic announcement. //
// ================================================ //
