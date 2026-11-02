// ==============================
// File:			TSerialHostPortPTYBasilisk.h
// Project:			Einstein
//
// Copyright 2026 Eckhart Koeppen
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

#ifndef _TSERIAL_HOST_PORT_PTY_BASILISK
#define _TSERIAL_HOST_PORT_PTY_BASILISK

#include "TSerialHostPortPTY.h"

///
/// PTY host serial port with Basilisk II / Classic Mac OS teardown workaround.
///
/// Enables TIOCPKT packet mode on the PTY master. Detects Basilisk II's
/// KillIO / close sequence (NOSTOP followed by two combined read/write flushes)
/// and forcefully recreates the PTY to unblock blocking reads in Basilisk.
///
class TSerialHostPortPTYBasilisk
		: public TSerialHostPortPTY
{
public:
	TSerialHostPortPTYBasilisk(
		TLog* inLog,
		KUInt32 inLocation,
		const std::string& inPath,
		TEmulator* inEmulator);

	virtual ~TSerialHostPortPTYBasilisk() override = default;

protected:
	virtual void ConfigureMaster() override;
	virtual bool ProcessIncomingBytes(KUInt8* inData, int inCount, bool& outResetRequested) override;

private:
	/// Whether reads from the PTY master have a TIOCPKT header.
	std::atomic<bool> mPacketMode { false };

	/// Basilisk close detection.
	/// NOSTOP arms detection; the first combined read/write flush is setup.
	/// The second combined flush resets the PTY, as in the legacy driver.
	bool mFlushDetectArmed { false };
	bool mSawSetupFlush { false };
};

#endif
