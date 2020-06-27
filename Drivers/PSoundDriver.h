// ==============================
// File:			PSoundDriver.h
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

#ifndef _PSOUNDDRIVER_H
#define _PSOUNDDRIVER_H

#include <Newton.h>
#include <Protocols.h>

struct TSoundDriverInfo
{
	ULong fUnknown_00; //  = 00000001
	ULong fUnknown_04; //  = 00000001
	ULong fUnknown_08; //  = 00000001
	ULong fUnknown_0C; //  = 54600000
	ULong fUnknown_10; //  = 00000006
	ULong fUnknown_14; //  = 00000010
	ULong fUnknown_18; //  = 00000001
};

typedef long (*SoundCallbackProcPtr)(void*);

///
/// Protocol for the sound driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
PROTOCOL PSoundDriver : public TProtocol
{
public:
	static	PSoundDriver*		New( char* inImplementationName );	// PROTOCOL ctor
			void				Delete( void );						// PROTOCOL dtor

	// Cirrus driver returns -30009
	NewtonErr	SetSoundHardwareInfo(const TSoundDriverInfo*);
	NewtonErr	GetSoundHardwareInfo(TSoundDriverInfo*);
	ULong	SetOutputBuffers( ULong addr, ULong size, ULong addr2, ULong size2 );
	ULong	SetInputBuffers( ULong addr, ULong size, ULong addr2, ULong size2 );
	ULong	ScheduleOutputBuffer( ULong size1, ULong size2 );
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

	NONVIRTUAL long	    OutputIntHandlerDispatcher( void );
	NONVIRTUAL long	    InputIntHandlerDispatcher( void );
	NONVIRTUAL void		SetOutputCallbackProc( SoundCallbackProcPtr inProc, void* inCookie );
	NONVIRTUAL void		SetInputCallbackProc( SoundCallbackProcPtr inProc, void* inCookie );

	SoundCallbackProcPtr	mOutputCallbackProc;
	void*					mOutputCallbackCookie;
	SoundCallbackProcPtr	mInputCallbackProc;
	void*					mInputCallbackCookie;

	// Used by glue to find the proper handler.
	NONVIRTUAL long	    OutputIntHandlerDispatcher_717006( void );
	NONVIRTUAL long	    OutputIntHandlerDispatcher_737041( void );
	NONVIRTUAL long	    OutputIntHandlerDispatcher_747129( void );
	NONVIRTUAL long	    InputIntHandlerDispatcher_717006( void );
	NONVIRTUAL long	    InputIntHandlerDispatcher_737041( void );
	NONVIRTUAL long	    InputIntHandlerDispatcher_747129( void );
};

#endif
		// _PSOUNDDRIVER_H

// ============================================================ //
// If Machiavelli were a programmer, he'd have worked for AT&T. //
// ============================================================ //
