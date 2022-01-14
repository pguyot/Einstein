// ==============================
// File:			PlatformEvents.h
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

#ifndef _PLATFORMEVENTS_H
#define _PLATFORMEVENTS_H

#if TARGET_OS_NEWTON
#include <AEvents.h>
#include <Newton.h>
typedef ULong KUInt32;
typedef UChar KUInt8;
#else
#include <K/Defines/KDefinitions.h>
#define kMAXEVENTSIZE (256)
#endif

#define kDEFAULTEVENTQUEUESIZE 64
#define kEVENTQUEUESIZEINCREMENT 64

#define kDEFAULTBUFFERQUEUESIZE 16
#define kBUFFERQUEUESIZEINCREMENT 16

// Ports.
enum EPort {
	kPowerPort = 'powr',
	kNewtPort = 'newt'
};

// Other constants.
enum {
	kEinsteinNSEventClass = 'eins',
	kEventRuntimeWithSData = 'nsrt',
	kNewtonScriptEvalData = 'nsev',
	kPackageInstallData = 'pkgi'
};

// Keyboard event types
enum {
	kKeyDownEventType = 0x20,
	kKeyUpEventType = 0x1F,
	kKeyRepeatEventType = 0x23,
	kKeyConnectEventType = 0x21,
	kConnectedKeyCode = 1,
	kDisconnectedKeyCode = 0
};

// Keyboard event.
struct SKeyboardEvent {
	KUInt32 fEventClass; /// 'newt'
	KUInt32 fEventID; /// 'idle'
	KUInt32 fUnknown_08; /// 'keyb'
	KUInt32 fUnknown_0C;
	KUInt32 fUnknown_10;
	KUInt32 fUnknown_14;
	KUInt32 fUnknown_18;
	KUInt32 fUnknown_1C;
	KUInt32 fUnknown_20;
	KUInt32 fUnknown_24;
	KUInt32 fUnknown_28;
};

struct SEinsteinBufferEvent {
	KUInt32 fEventClass; /// 'eins'
	KUInt32 fEventID; /// 'nsrt'
	KUInt32 fDataClass; /// 'nsev' or 'pkgi'
	KUInt32 fSize; /// total size.
	KUInt32 fBufferID; /// id of buffer.
};

struct SAEventData {
	EPort fPort; ///< Port to send the event to.
	KUInt32 fSize; ///< Size of the event (in bytes).
	KUInt8 fData[kMAXEVENTSIZE]; ///< Data.
};

enum EEventType {
	kAEvent = 'aevt'
};

struct SEvent {
	EEventType fType;
	union {
		SAEventData aevent;
	} fData;
};

#endif
// _PLATFORMEVENTS_H

// =================================================================== //
// Never try to explain computers to a layman.  It's easier to explain //
// sex to a virgin.                                                    //
//         -- Robert Heinlein                                          //
//                                                                     //
// (Note, however, that virgins tend to know a lot about computers.)   //
// =================================================================== //
