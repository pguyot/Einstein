// ==============================
// File:			TSerialHostPortDirect.h
// Project:			Einstein
//
// Copyright 2020 Eckhart Koeppen
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

#ifndef _TSERIAL_HOST_PORT_DIRECT
#define _TSERIAL_HOST_PORT_DIRECT

#include "TSerialHostPort.h"
#include <K/Misc/TCircleBuffer.h>
#include <K/Threads/TThread.h>

///
/// Class to handle host emulated serial port
///
class TSerialHostPortDirect
		: public TSerialHostPort
{
public:
	TSerialHostPortDirect(
		TLog* inLog,
		KUInt32 inLocation,
		const std::string& inPath,
		TEmulator* inEmulator);

	virtual ~TSerialHostPortDirect();

	virtual void PutByte(KUInt8 nextChar) override;
	virtual KUInt8 GetByte() override;
	virtual void SetSerialOutputs(SerialOutputControl) override;
	virtual void ClearSerialOutputs(SerialOutputControl) override;

	virtual void SetInterruptEnable(bool enable) override;
	virtual void Reset() override;
	virtual void SetBreak(bool assert) override;

	virtual InterfaceSpeed SetSpeed(BitRate bitsPerSec) override;
	virtual void SetIOParms(TCMOSerialIOParms* opt) override;
	virtual void Reconfigure() override;

	virtual KSInt32 InitByOption(TOption* initOpt) override;
	virtual KSInt32 ProcessOption(TOption* opt) override;
	virtual KSInt32 SetSerialMode(SerialMode mode) override;

	virtual RxErrorStatus GetByteAndStatus(KUInt8* nextCharPtr) override;
	virtual bool AllSent() override;

	virtual KSInt32 WaitForAllSent() override;

	void Run();

protected:
	std::string mPath;
	int mFile;
	TThread* mIOThread;
	TMutex* mReadMutex;
	TCircleBuffer* mReadBuffer;
};

#endif
