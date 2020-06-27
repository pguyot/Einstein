// ==============================
// File:			PMainSoundDriver.impl.h
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

#ifndef _PMAINSOUNDDRIVER_IMPL_H
#define _PMAINSOUNDDRIVER_IMPL_H

#include "PSoundDriver.h"
#include "NewtonGestalt.h"

struct TDMAChannelDiscriptor
{
	ULong	mBaseRegisters_1;	// 0F080000 + id << 13
	ULong	mBaseRegisters_2;	// 0F090000 + id << 12
	ULong	mBitInDMACtrlRegs;	// 1 << id
	ULong	mInterruptMask;		// 1 << id+7
};

struct InterruptObject;

class TGestaltVolumeInfo
{
public:
	Boolean	fUnknown_1;	// true (output?)
	Boolean	fUnknown_2;	// true (input?)
	ULong	fUnknown_3;	// C03FF9DB
	ULong	fUnknown_4;	// 00000000
	ULong	fUnknown_5;	// 0000000E
	ULong	fUnknown_6;	// 0000001D
};

class TGestaltSoundHWInfo
{
public:
	Boolean	fUnknown_1;	// true (output?)
	Boolean	fUnknown_2;	// true (input?)
	ULong	fUnknown_3;	// 00790020
	ULong	fUnknown_4;	// 0000001D
	ULong	fUnknown_5;	// FFE00625
	ULong	fUnknown_6;	// 00000000
	ULong	fUnknown_7;	// 00000000
	ULong	fUnknown_8;	// FFDB1AC1
	ULong	fUnknown_9;	// 00000000
	ULong	fUnknown_A;	// 00000000
};

///
/// Class for our sound driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
PROTOCOL PMainSoundDriver : public PSoundDriver
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(PMainSoundDriver);

	CAPABILITIES(
		("copyright" "Paul Guyot")
		("version" "1.0")
		("SoundOutput" "")
		("SoundInput" "") )

	PSoundDriver*	New( void );
	void			Delete( void );
	long	SetSoundHardwareInfo(const TSoundDriverInfo *);
	long	GetSoundHardwareInfo(TSoundDriverInfo*);
	ULong	SetOutputBuffers(unsigned long, unsigned long, unsigned long, unsigned long);
	ULong	SetInputBuffers(unsigned long, unsigned long, unsigned long, unsigned long);
	ULong	ScheduleOutputBuffer(unsigned long, unsigned long);
	ULong	ScheduleInputBuffer(unsigned long, unsigned long);
	ULong	PowerOutputOn(long);
	ULong	PowerOutputOff();
	ULong	PowerInputOn(long);
	ULong	PowerInputOff();
	ULong	StartOutput();
	ULong	StartInput();
	ULong	StopOutput();
	ULong	StopInput();
	ULong	OutputIsEnabled();
	ULong	InputIsEnabled();
	ULong	OutputIsRunning();
	ULong	InputIsRunning();
	ULong	CurrentOutputPtr();
	ULong	CurrentInputPtr();
	ULong	OutputVolume(long);
	ULong	OutputVolume();
	ULong	InputVolume(long);
	ULong	InputVolume();
	ULong	EnableExtSoundSource(long);
	ULong	DisableExtSoundSource(long);
	ULong	OutputIntHandler();
	ULong	InputIntHandler();

private:
	///
	/// Native function to set the interrupt masks as allocated by the DMA
	/// Manager.
	///
	/// \param inInputSoundMask		input sound interrupt mask.
	/// \param inOutputSoundMask	output sound interrupt mask.
	///
	void	NativeSetInterruptMask(
				ULong inInputSoundMask,
				ULong inOutputSoundMask );

	TDMAChannelDiscriptor	mOutputDMAChannel;	///< DMA for output
	TDMAChannelDiscriptor	mInputDMAChannel;	///< DMA for input
	InterruptObject*		mOutputInterrupt;	///< Result of RegisterInterrupt
	InterruptObject*		mInputInterrupt;	///< Result of RegisterInterrupt

	TGestaltVolumeInfo		mVolumeInfo;
	TGestaltSoundHWInfo		mSoundHWInfo;
};

#endif
		// _PMAINSOUNDDRIVER_IMPL_H

// =========================================================================== //
// Many of the convicted thieves Parker has met began their                    //
// life of crime after taking college Computer Science courses.                //
//                 -- Roger Rapoport, "Programs for Plunder", Omni, March 1981 //
// =========================================================================== //
