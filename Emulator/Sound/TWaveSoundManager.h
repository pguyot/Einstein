// ==============================
// File:			TWaveSoundManager.h
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

#ifndef _TWAVESOUNDMANAGER_H
#define _TWAVESOUNDMANAGER_H

#include <K/Defines/KDefinitions.h>

// Windows Multimedia library waveOut interface
#include <Windows.h>
#include <mmeapi.h>

// Einstein
#include "TSoundManager.h"

// #define DEBUG_SOUND	0

class TCircleBuffer;
class TMutex;

///
/// Class to handle sound input/output with MS Multimedia wave library.
///
/// \author Paul Guyot <pguyot@kallisys.net>
///
class TWaveSoundManager
	:
		public TSoundManager
{
public:

	static const int NWaveBuffer = 4;

	///
	/// Constructor from a log.
	///
	/// \param inLog				log interface (can be null)
	///
	TWaveSoundManager( TLog* inLog = nil );

	///
	/// Destructor.
	///
	virtual ~TWaveSoundManager( void );

	///
	/// Schedule output of some buffer.
	///
	virtual void	ScheduleOutputBuffer( KUInt32 inBufferAddr, KUInt32 inSize );
	
	///
	/// Start output.
	///
	virtual void	StartOutput( void );

	///
	/// Stop output.
	///
	virtual void	StopOutput( void );

	///
	/// Is output running?
	///
	virtual Boolean	OutputIsRunning( void );

	///
	/// Method called to signal a change in the output volume.
	///
	virtual void	OutputVolumeChanged( void ) { updateVolume(); }

private:

	typedef enum { available = 0, pending, playing } State;

	typedef struct {
		WAVEHDR				waveHdr;
		KSInt16				*buffer;
		KUInt32				nBuffer;
		State				state;
	} WaveBuffer;

	void		initWaveBuffer(int ix);

	void		freeWaveBuffer(int ix);

	void		fillWaveBuffer(int ix, KUInt32 inBufferAddr, KUInt32 inSize);

	void		sendWaveBuffer(int ix);

	void		openWaveOut();

	void		updateVolume();

	static void CALLBACK waveOutProcCB(
						HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance,  
						DWORD_PTR dwParam1, DWORD_PTR dwParam2);

	void		waveOutProc(UINT uMsg, int ix);

	int			next(int ix);

	void		logError(const char *msg, MMRESULT err);

	WaveBuffer	wb[NWaveBuffer];

	HWAVEOUT	waveOut;

	bool		noWaveOut;

	int			playNext;

	int			nextAvailable;

	bool		isPlaying;

	DWORD		volume;

	TMutex		*mutex;
};

#endif
		// _TWAVESOUNDMANAGER_H

// ========================================= //
// TRANSACTION CANCELLED - FARECARD RETURNED //
// ========================================= //
