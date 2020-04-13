// ==============================
// File:			TWaveSoundManager.cp
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

#undef DEBUG_LOG 

#include <K/Defines/KDefinitions.h>
#include "TWaveSoundManager.h"

// ANSI C & POSIX
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

// K
#include <K/Defines/UByteSex.h>
#include <K/Misc/TCircleBuffer.h>
#include <K/Threads/TMutex.h>

// Einstein.
#include "Emulator/Log/TLog.h"
#include "Emulator/TMemory.h"

#ifdef DEBUG_LOG
#	define LOG 
#else
#	define LOG if (0) 
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TWaveSoundManager( TLog* )
// -------------------------------------------------------------------------- //
TWaveSoundManager::TWaveSoundManager( TLog* inLog /* = nil */ )
	:
		TSoundManager( inLog ),
		waveOut(0L),
		noWaveOut(false),
		playNext(0),
		nextAvailable(0),
		isPlaying(false),
		volume(32768),
		mutex(0L)
{
	mutex = new TMutex();
	LOG LOG fprintf(stderr, "v WAVE constructor\n");
	mutex->Lock();
	for (int i=0; i<NWaveBuffer; i++) 
		initWaveBuffer(i);
	mutex->Unlock();
	LOG fprintf(stderr, "^ WAVE constructor\n");
}

// -------------------------------------------------------------------------- //
//  * ~TWaveSoundManager( void )
// -------------------------------------------------------------------------- //
TWaveSoundManager::~TWaveSoundManager( void )
{
	LOG fprintf(stderr, "v WAVE destructor\n");
	StopOutput();

	mutex->Lock();
	for (int i=0; i<NWaveBuffer; i++)
		freeWaveBuffer(i);
	mutex->Unlock();

	if (waveOut)
		waveOutClose(waveOut);

	delete mutex;

	LOG fprintf(stderr, "^ WAVE destructor\n");
}

// -------------------------------------------------------------------------- //
//  * ScheduleOutput( const KUInt8*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TWaveSoundManager::ScheduleOutputBuffer( KUInt32 inBufferAddr, KUInt32 inSize )
{
	// if we previously figured out that there is no device, leave.
	if (noWaveOut)
		return;

	// if there is no data to play, leave.
	if (inSize==0)
		return;

	LOG fprintf(stderr, "v WAVE schedule %d bytes\n", inSize);

	mutex->Lock();

	// find an available buffer
	if (wb[nextAvailable].state!=available) {
		LOG fprintf(stderr, "^ WAVE schedule (no available buffer)\n");
		mutex->Unlock();
		return;
	}

	if (!waveOut) {
		openWaveOut();
		if (!waveOut) {
			LOG fprintf(stderr, "^ WAVE schedule (no available audio device)\n");
			mutex->Unlock();
			return;
		}
	}

	// now fill the available buffer
	int thisBuffer = nextAvailable;
	nextAvailable = next(nextAvailable);

	fillWaveBuffer(thisBuffer, inBufferAddr, inSize);

	if (!OutputIsRunning()) {
		playNext = thisBuffer;
	} else {
		while (wb[playNext].state==pending) {
			sendWaveBuffer(playNext);
		}
	}

	if (wb[nextAvailable].state==available) {
		RaiseOutputInterrupt();
	}

	mutex->Unlock();

	LOG fprintf(stderr, "^ WAVE schedule %d bytes\n", inSize);
}

// -------------------------------------------------------------------------- //
//  * StartOutput( void )
// -------------------------------------------------------------------------- //
void
TWaveSoundManager::StartOutput( void )
{
	LOG fprintf(stderr, "v WAVE start\n");

	mutex->Lock();

	if (playNext==-1) {
		LOG fprintf(stderr, "^ WAVE start (nothing to play)\n");
		mutex->Unlock();
		return;
	}

	while (wb[playNext].state==pending) {
		sendWaveBuffer(playNext);
	}

	mutex->Unlock();

	LOG fprintf(stderr, "^ WAVE start\n");
}

// -------------------------------------------------------------------------- //
//  * StopOutput( void )
// -------------------------------------------------------------------------- //
void
TWaveSoundManager::StopOutput( void )
{
	LOG fprintf(stderr, "v WAVE stop\n");

	int i;
	waveOutReset(waveOut);

	mutex->Lock();

	for (i=0; i<NWaveBuffer; i++) {
		WaveBuffer &b = wb[i];
		b.state = available;
	}

	mutex->Unlock();

	LOG fprintf(stderr, "^ WAVE stop\n");
}

// -------------------------------------------------------------------------- //
//  * OutputIsRunning( void )
// -------------------------------------------------------------------------- //
Boolean
TWaveSoundManager::OutputIsRunning( void )
{
	LOG fprintf(stderr, "v WAVE isRunning\n");
	/*
#if 0
	printf( "OutputIsRunning: %i\n", !mOutputBuffer->IsEmpty() );
	return !mOutputBuffer->IsEmpty();
#endif
	return wb[0].playing || wb[1].playing;
	*/
	Boolean ret = isPlaying;
	LOG fprintf(stderr, "^ WAVE isRunning (%d)\n", ret);
	return ret;
}

void TWaveSoundManager::waveOutProcCB(
		HWAVEOUT, UINT uMsg, DWORD_PTR dwInstance, 
		DWORD dwParam1, DWORD dwParam2)
{
	TWaveSoundManager *me = (TWaveSoundManager*)dwInstance;
	WAVEHDR *wh = (WAVEHDR*)dwParam1;
	switch (uMsg) {
		case WOM_DONE:	me->waveOutProc(uMsg, wh->dwUser); break;
	}
}

void TWaveSoundManager::waveOutProc(UINT uMsg, int ix)
{
	if (uMsg!=WOM_DONE)
		return;

	LOG fprintf(stderr, "v WAVE callback (%d: %d)\n", uMsg, ix);

	mutex->Lock();

	WaveBuffer &b = wb[ix];

	// close out this buffer and make it available
	waveOutUnprepareHeader(waveOut, &b.waveHdr, sizeof(b.waveHdr));
	b.state = available;

	while (wb[playNext].state==pending) {
		sendWaveBuffer(playNext);
	}

	isPlaying = false;
	for (int i=0; i<NWaveBuffer; i++) {
		if (wb[i].state==playing) {
			isPlaying = true;
			break;
		}
	}

	if (ix==nextAvailable) {
		RaiseOutputInterrupt();
	}

	mutex->Unlock();

	LOG fprintf(stderr, "^ WAVE callback\n");
}


void TWaveSoundManager::initWaveBuffer(int ix)
{
	LOG fprintf(stderr, "  v init wave buffer %d\n", ix);

	WaveBuffer &b = wb[ix];
	memset(&b.waveHdr, 0, sizeof(b.waveHdr));
	b.buffer = 0L;
	b.nBuffer = 0L;
	b.state = available;

	LOG fprintf(stderr, "  ^ init wave buffer %d\n", ix);
}


void TWaveSoundManager::freeWaveBuffer(int ix)
{
	LOG fprintf(stderr, "  v free wave buffer %d\n", ix);

	WaveBuffer &b = wb[ix];
	if (b.state!=available) {
		LOG fprintf(stderr, "  ^ free wave buffer %d (buffer still in use!)\n", ix);
	}
	if (b.buffer) {
		free(b.buffer);
		b.buffer = 0;
	}

	LOG fprintf(stderr, "  ^ free wave buffer %d\n", ix);
}

void TWaveSoundManager::fillWaveBuffer(int ix, KUInt32 inBufferAddr, KUInt32 inSize)
{
	LOG fprintf(stderr, "  v fill wave buffer %d\n", ix);

	WaveBuffer &b = wb[ix];

	if (!b.buffer) {
		b.buffer = (KSInt16*)calloc(kNewtonBufferSize, 1);
	}

	if ( inSize > kNewtonBufferSize ) {
		LOG fprintf(stderr, "| truncating buffer!\n");
		inSize = kNewtonBufferSize;
	}

	GetMemory()->FastReadBuffer(inBufferAddr, inSize, (KUInt8*)b.buffer);
	b.nBuffer = inSize;

	KUInt32 frames = inSize / sizeof( KSInt16 );
	KSInt16 *p = b.buffer;
	for ( ;frames--; p++) {
		*p = UByteSex_FromBigEndian((KUInt16)(*p));
	}

	b.state = pending;

	LOG fprintf(stderr, "  ^ fill wave buffer %d\n", ix);
}

void TWaveSoundManager::sendWaveBuffer(int ix)
{
	LOG fprintf(stderr, "  v send wave buffer %d\n", ix);

	WaveBuffer &b = wb[ix];

	memset(&b.waveHdr, 0, sizeof(b.waveHdr));
	b.waveHdr.lpData = (LPSTR)b.buffer;
	b.waveHdr.dwBufferLength = b.nBuffer;
	b.waveHdr.dwFlags = 0;
	b.waveHdr.dwUser = ix;

	MMRESULT err = waveOutPrepareHeader(waveOut, &b.waveHdr, sizeof(b.waveHdr));
	if (err) {
		LOG fprintf(stderr, "ERROR: Prepare sound returned %d\n", err);
		logError("Error preparing waveOut sound buffer", err);
	}

	err = waveOutWrite(waveOut, &b.waveHdr, sizeof(b.waveHdr));
	if (err) {
		LOG fprintf(stderr, "ERROR: Write sound returned %d\n", err);
		logError("Error writing waveOut sound buffer", err);
	}

	isPlaying = true;
	b.state = playing;
	playNext = next(ix);

	LOG fprintf(stderr, "  ^ send wave buffer %d\n", ix);
}


int TWaveSoundManager::next(int ix)
{
	ix++;
	if (ix>=NWaveBuffer)
		ix = 0;
	return ix;
}

void TWaveSoundManager::openWaveOut() 
{
	LOG fprintf(stderr, "  v open wave out device\n");
	// we will play some sound, so open the sound device now
	if (!waveOut) {
		static WAVEFORMATEX waveFormat = {
			WAVE_FORMAT_PCM, 1, 
			22050, 44100, 1, 16, 0
		};
		MMRESULT err = waveOutOpen( 
			&waveOut, WAVE_MAPPER, &waveFormat,
			(DWORD_PTR)waveOutProcCB, (DWORD_PTR)this, 
			CALLBACK_FUNCTION
		);
		if (err) {
			logError("Error opening waveOut sound device", err);
			noWaveOut = true;
		}
	}
	LOG fprintf(stderr, "  ^ open wave out device\n");
}

void TWaveSoundManager::logError(const char *msg, MMRESULT err)
{
	TCHAR desc[MAXERRORLENGTH];
	MMRESULT errText = waveOutGetErrorText(err, (LPTSTR)desc, MAXERRORLENGTH);
	GetLog()->FLogLine("%s", msg);
	if (errText==MMSYSERR_NOERROR)
		GetLog()->FLogLine("  \"%s\"", desc);
	else 
		GetLog()->FLogLine("  Unknown error %d", err);
}

void TWaveSoundManager::updateVolume()
{
	KUInt32 vol = OutputVolume();

	const KUInt32 div = ((kOutputVolume_Max-kOutputVolume_Min)/0xffff)+1;

	if (vol==kOutputVolume_Max) {
		volume = 0xffff;
	} else if (vol<=kOutputVolume_Min || vol==kOutputVolume_Zero) {
		volume = 0;
	} else {
		volume = (vol - kOutputVolume_Min)/div;
	}
	LOG fprintf(stderr, "Newton volume = 0x%10x (%d) = PC volume 0x%04x (%d)\n", vol, vol, volume, volume);

	if (waveOut) {
		waveOutSetVolume(waveOut, volume);
	}
}


// ============================================================================= //
// As in Protestant Europe, by contrast, where sects divided endlessly into      //
// smaller competing sects and no church dominated any other, all is different   //
// in the fragmented world of IBM.  That realm is now a chaos of conflicting     //
// norms and standards that not even IBM can hope to control.  You can buy a     //
// computer that works like an IBM machine but contains nothing made or sold by  //
// IBM itself.  Renegades from IBM constantly set up rival firms and establish   //
// standards of their own.  When IBM recently abandoned some of its original     //
// standards and decreed new ones, many of its rivals declared a puritan         //
// allegiance to IBM's original faith, and denounced the company as a divisive   //
// innovator.  Still, the IBM world is united by its distrust of icons and       //
// imagery.  IBM's screens are designed for language, not pictures.  Graven      //
// images may be tolerated by the luxurious cults, but the true IBM faith relies //
// on the austerity of the word.                                                 //
//                 -- Edward Mendelson, "The New Republic", February 22, 1988    //
// ============================================================================= //
