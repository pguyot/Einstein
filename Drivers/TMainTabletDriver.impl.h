// ==============================
// File:			TMainTabletDriver.impl.h
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

#ifndef _TMAINTABLETDRIVER_IMPL_H
#define _TMAINTABLETDRIVER_IMPL_H

#include "TTabletDriver.h"

struct InterruptObject;

///
/// Class for our tablet driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
PROTOCOL TMainTabletDriver : public TTabletDriver
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(TMainTabletDriver);

	CAPABILITIES( ("copyright" "Paul Guyot") ("version" "1.0") )

	TTabletDriver*	New( void );				// PROTOCOL ctor
	void			Delete( void );				// PROTOCOL dtor
	ULong			Init(const Rect&);
	ULong			WakeUp();
	ULong			ShutDown();
	ULong			TabletIdle();
	ULong			GetSampleRate();
	void			SetSampleRate(unsigned long);
	void			GetTabletCalibration(Calibration*);
	void			SetTabletCalibration(const Calibration&);
	ULong			SetDoingCalibration(unsigned char, unsigned long*);
	void			GetTabletResolution(long*, long*);
	ULong			TabSetOrientation(long);
	ULong			GetTabletState();
	ULong			GetFingerInputState(unsigned char*);
	ULong			SetFingerInputState(unsigned char);
	ULong			RecalibrateTabletAfterRotate();
	ULong			TabletNeedsRecalibration();
	ULong			StartBypassTablet();
	ULong			StopBypassTablet();
	ULong			ReturnTabletToConsciousness(unsigned long, unsigned long, unsigned long);

private:
	///
	/// Interrupt handler.
	///
	static long		SInterruptHandler( void* inUserData );

	///
	/// Native function to insert tablet samples. Called by the interrupt
	/// handler.
	///
	/// \param outSampleRecord		coordinate & pressure of the sample.
	/// \param outSampleTime		time (in ticks) of the sample.
	/// \return \c true if there was a sample returned.
	///
	Boolean	NativeGetSample(
				ULong* outSampleRecord,
				ULong* outSampleTime );

	/// \name variables
	InterruptObject*		mInterrupt;	///< Result of RegisterInterrupt
};

#endif
		// _TMAINTABLETDRIVER_IMPL_H

// ======================================================================== //
// ... computer hardware progress is so fast.  No other technology since    //
// civilization began has seen six orders of magnitude in performance-price //
// gain in 30 years.                                                        //
//                 -- Fred Brooks                                           //
// ======================================================================== //
