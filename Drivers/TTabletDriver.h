// ==============================
// File:            TTabletDriver.h
// Project:         Einstein
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

#ifndef _TTABLETDRIVER_H
#define _TTABLETDRIVER_H

#include <Newton.h>
#include <Protocols.h>

struct Calibration {
    ULong   fUnknown_00;
    ULong   fUnknown_04;
    ULong   fUnknown_08;
    ULong   fUnknown_0C;
    ULong   fUnknown_10;
};

///
/// Protocol for the tablet driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test   aucun test défini.
///
PROTOCOL TTabletDriver : public TProtocol
{
public:
    static  TTabletDriver*      New( char* inImplementationName );  // PROTOCOL ctor
            void                Delete( void );                     // PROTOCOL dtor
            ULong               Init(const Rect&);
            ULong               WakeUp();
            ULong               ShutDown();
            ULong               TabletIdle();
            ULong               GetSampleRate();
            void                SetSampleRate(unsigned long);
            void                GetTabletCalibration(Calibration*);
            void                SetTabletCalibration(const Calibration&);
            ULong               SetDoingCalibration(unsigned char, unsigned long*);
            void                GetTabletResolution(long*, long*);
            ULong               TabSetOrientation(long);
            ULong               GetTabletState();
            ULong               GetFingerInputState(unsigned char*);
            ULong               SetFingerInputState(unsigned char);
            ULong               RecalibrateTabletAfterRotate();
            ULong               TabletNeedsRecalibration();
            ULong               StartBypassTablet();
            ULong               StopBypassTablet();
            ULong               ReturnTabletToConsciousness(unsigned long, unsigned long, unsigned long);
};

#endif
        // _TTABLETDRIVER_H

// =============================================== //
// Mommy, what happens to your files when you die? //
// =============================================== //
