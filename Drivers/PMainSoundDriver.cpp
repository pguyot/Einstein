// ==============================
// File:            PMainSoundDriver.cp
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

#include "PMainSoundDriver.impl.h"
#include "K/Misc/RelocHack.h"

#include "UGlue.h"

PROTOCOL_IMPL_SOURCE_MACRO(PMainSoundDriver)    // Magic stuff, do not touch

// DMA Manager stuff is not in Public JT. Cf the glue in Prims.a
class TDMAManager
{
public:
    void RequestAssignment( ULong inChannel, TDMAChannelDiscriptor* outDescriptor );
};

TDMAManager* GetDMAManagerObject( void );

typedef long (*InterruptHandlerProcPtr)(void*);

// These three functions are in Public JT.
InterruptObject* RegisterInterrupt(
            ULong inInterruptMask,
            void* inCookie,
            InterruptHandlerProcPtr inHandler,
            ULong inFlags);
void DeregisterInterrupt(InterruptObject*);
extern "C" long EnableInterrupt(InterruptObject*, ULong);

long OutputIntHandlerDispatcherGlue(void* context) {
    return ((PSoundDriver*) context)->OutputIntHandlerDispatcher();
}
long InputIntHandlerDispatcherGlue(void* context) {
    return ((PSoundDriver*) context)->InputIntHandlerDispatcher();
}

// ------------------------------------------------------------------------ //
// New( void )
// ------------------------------------------------------------------------ //
// Protocol constructor.
PSoundDriver*
PMainSoundDriver::New( void )
{
    // Register the interrupts.
    TDMAManager* theDMAManager = GetDMAManagerObject();
    theDMAManager->RequestAssignment( 10, &mOutputDMAChannel );
    // We don't need RelocFuncPtr with our Rex tool.
    mOutputInterrupt = RegisterInterrupt(
            mOutputDMAChannel.mInterruptMask,
            this,
#if __GNUC__
            OutputIntHandlerDispatcherGlue,
#else
            (InterruptHandlerProcPtr)
                RelocFuncPtr(OutputIntHandlerDispatcherGlue),
#endif
            1 );
    theDMAManager->RequestAssignment( 9, &mInputDMAChannel );
    mInputInterrupt = RegisterInterrupt(
            mInputDMAChannel.mInterruptMask,
            this,
#if __GNUC__
            (InterruptHandlerProcPtr) InputIntHandlerDispatcherGlue,
#else
            (InterruptHandlerProcPtr)
                RelocFuncPtr(InputIntHandlerDispatcherGlue),
#endif
            1 );

    (void) EnableInterrupt(mOutputInterrupt, 1);
    (void) EnableInterrupt(mInputInterrupt, 1);

    // Save the interrupt masks, so they can be raised later on by native code.
    NativeSetInterruptMask(
        mInputDMAChannel.mInterruptMask,
        mOutputDMAChannel.mInterruptMask);

    // Fill & register gestalt selectors.
    TUGestalt theGestalt;

    mVolumeInfo.fUnknown_1 = true;
    mVolumeInfo.fUnknown_2 = true;
    mVolumeInfo.fUnknown_3 = 0xC03FF9DB;
    mVolumeInfo.fUnknown_4 = 0x00000000;
    mVolumeInfo.fUnknown_5 = 0x0000000E;
    mVolumeInfo.fUnknown_6 = 0x0000001D;
    (void) theGestalt.RegisterGestalt(
                kGestalt_Ext_VolumeInfo,
                &mVolumeInfo,
                sizeof(mVolumeInfo));

    mSoundHWInfo.fUnknown_1 = true;
    mSoundHWInfo.fUnknown_2 = true;
    mSoundHWInfo.fUnknown_3 = 0x00790020;
    mSoundHWInfo.fUnknown_4 = 0x0000001D;
    mSoundHWInfo.fUnknown_5 = 0xFFE00625;
    mSoundHWInfo.fUnknown_6 = 0x00000000;
    mSoundHWInfo.fUnknown_7 = 0x00000000;
    mSoundHWInfo.fUnknown_8 = 0xFFDB1AC1;
    mSoundHWInfo.fUnknown_9 = 0x00000000;
    mSoundHWInfo.fUnknown_A = 0x00000000;

    (void) theGestalt.RegisterGestalt(
                kGestalt_Ext_SoundHWInfo,
                &mSoundHWInfo,
                sizeof(mSoundHWInfo));
    return this;
}

// ------------------------------------------------------------------------ //
// Delete( void )
// ------------------------------------------------------------------------ //
// Protocol destructor.
void
PMainSoundDriver::Delete( void )
{
    DeregisterInterrupt(mOutputInterrupt);
    DeregisterInterrupt(mInputInterrupt);
}

// ========================================================================== //
// AmigaDOS Beer: The company has gone out of business, but their recipe has  //
// been picked up by some weird German company, so now this beer will be an   //
// import.  This beer never really sold very well because the original        //
// manufacturer didn't understand marketing. Like Unix Beer, AmigaDOS Beer    //
// fans are an extremely loyal and loud group. It originally came in a        //
// 16-oz. can, but now comes in 32-oz.  cans too.  When this can was          //
// originally introduced, it appeared flashy and colorful, but the design     //
// hasn't changed much over the years, so it appears dated now.  Critics of   //
// this beer claim that it is only meant for watching TV anyway.              //
// ========================================================================== //
