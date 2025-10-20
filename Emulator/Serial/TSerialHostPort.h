// ==============================
// File:			TSerialHostPort.h
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

#ifndef _TSERIAL_HOST_PORT
#define _TSERIAL_HOST_PORT

#include <K/Defines/KDefinitions.h>
#include "Emulator/TInterruptManager.h"
#include "Emulator/Platform/TNewt.h"

class TLog;

typedef KUInt32 RxErrorStatus; /* mirrors SCC read register 1 bits			*/
typedef KUInt32 InterfaceSpeed;
typedef KUInt32 SerialMode;
typedef KUInt32 BitRate;

typedef KSInt32 StopBits;
typedef KSInt32 ParityType;
typedef KSInt32 DataBits;

class TOption
{
public:
	KUInt32 fLabel; // option label
	KUInt32 fLength; // length of data, excluding header (i.e., fLabel, fLength, fFlags)
	KUInt32 fFlags; // flag bits

	// variable amount of data follows, derived classes specify
};

class TCMOSerialIOParms : public TOption
{
public:
	TCMOSerialIOParms();

	KSInt32 fStopBits; // default 1 stop bit
	KSInt32 fParity; // default no parity
	KSInt32 fDataBits; // default 8 data bits
	BitRate fSpeed; // default 9600 bps
};

typedef KUInt32 SerialOutputControl;
#define kSerialOutputDTR (0x00000001)
#define kSerialOutputRTS (0x00000002)

typedef KUInt32 SerialStatus; /* mirrors SCC read register 0 bits			*/
#define kSerialRxCharAvailable (0x00000001)
#define kSerialDSRAsserted (0x00000002) /* 2.0 CHANGE!!					*/
#define kSerialTxBufferEmpty (0x00000004)
#define kSerialDCDAsserted (0x00000008) /* note positive logic here					*/
#define kSerialRIAsserted (0x00000010) /* 2.0 CHANGE!!					*/
#define kSerialCTSAsserted (0x00000020) /* 2.0 CHANGE!!					*/
#define kSerialTxUnderrun (0x00000040)
#define kSerialBreak (0x00000080)
#define kSerialAbort (0x00000080)
#define kSerialChipGone (0x00000100) /* card pulled out!							*/

typedef KUInt32 SerialFeatures; /* features supported by this chip (v2.0)	*/
#define kSerFeatureNone (0x00000000)
#define kSerFeatureDefaults (0x00000001) /* default capabilities (used for v1.0 chips) */
#define kSerFeatureVersion2 (0x00000002) /* supports version 2.0 interface calls		*/
#define kSerFeatureTriStateTxD (0x00000004) /* can tristate TxD							*/
#define kSerFeatureHiSpeedClk (0x00000008) /* can switch to high-speed clock...(?)		*/
#define kSerFeatureCTSClock (0x00000010) /* CTS may be used for clocking \
											 (enables auto-clock detect feature)		*/
#define kSerFeatureAllSent (0x00000020) /* All sent call is supported				*/
#define kSerFeatureTxConfigNeeded (0x00000040) /* output config needed: half duplex...		*/
#define kSerFeatureTVRemote (0x00000080) /* supports TV remote mode					*/
#define kSerFeatureGetErrByte (0x00000100) /* supports GetErrorByte call				*/
#define kSerFeatureSDLCMode (0x00000200) /* supports sdlc sync calls					*/
#define kSerFeatureLocalTalk (0x00000400) /* supports localtalk calls					*/
#define kSerFeatureAsyncRxDMA (0x00001000) /* supports input dma for async				*/
#define kSerFeatureAsyncTxDMA (0x00002000) /* supports output dma for async			*/
#define kSerFeatureSyncRxDMA (0x00004000) /* supports input dma for sync				*/
#define kSerFeatureSyncTxDMA (0x00008000) /* supports output dma for sync				*/
#define kSerFeatureWireAsyncRxDMABuf (0x00010000) /* async input dma requires wired buffer	*/
#define kSerFeatureWireAsyncTxDMABuf (0x00020000) /* async output dma requires wired buffer	*/
#define kSerFeatureWaitForAllSent (0x00100000) /* WaitForAllSent call available?	*/
#define kSerFeatureEinstein (0x80000000) /* Einstein emulated port */

///
/// Base class for host emulated ports
///
/// Implements parts of the TSerialChipV2 protocol
///
class TSerialHostPort
{
public:
	TSerialHostPort(
		TLog* inLog,
		KUInt32 inLocation,
		TEmulator* inEmulator) :
			mLog(inLog),
			mEmulator(inEmulator), mLocation(inLocation), mInterruptID(0), mSerialStatus(kSerialChipGone), mSerialOutputs(0)
	{
	}

	virtual ~TSerialHostPort() { }

	virtual void PutByte(KUInt8 nextChar) = 0;
	void
	ResetTxBEmpty()
	{
		mSerialStatus |= kSerialTxBufferEmpty;
	}
	virtual KUInt8 GetByte() = 0;
	bool
	TxBufEmpty()
	{
		return mSerialStatus & kSerialTxBufferEmpty;
	}
	bool
	RxBufFull()
	{
		return mSerialStatus & kSerialRxCharAvailable;
	}
	SerialStatus
	GetSerialStatus()
	{
		return mSerialStatus;
	}
	void
	ResetSerialStatus()
	{
		mSerialStatus = 0;
	}
	virtual void SetSerialOutputs(SerialOutputControl) = 0;
	virtual void ClearSerialOutputs(SerialOutputControl) = 0;
	SerialOutputControl
	GetSerialOutputs()
	{
		return mSerialOutputs;
	}

	virtual void SetInterruptEnable(bool enable) = 0;
	virtual void Reset() = 0;
	virtual void SetBreak(bool assert) = 0;

	virtual InterfaceSpeed SetSpeed(BitRate bitsPerSec) = 0;
	virtual void SetIOParms(TCMOSerialIOParms* opt) = 0;
	virtual void Reconfigure() = 0;

	SerialFeatures
	GetFeatures()
	{
		return kSerFeatureDefaults | kSerFeatureVersion2 | kSerFeatureEinstein;
	}
	virtual KSInt32 InitByOption(TOption* initOpt) = 0;
	virtual KSInt32 ProcessOption(TOption* opt) = 0;
	virtual KSInt32 SetSerialMode(SerialMode mode) = 0;

	virtual RxErrorStatus GetByteAndStatus(KUInt8* nextCharPtr) = 0;
	virtual bool AllSent() = 0;

	virtual KSInt32 WaitForAllSent() = 0;

	KUInt32
	GetInterruptID()
	{
		return mInterruptID;
	}

	void
	AssignInterruptID()
	{
		mInterruptID = mCurrentInterruptID;
		mCurrentInterruptID <<= 1;
	}

protected:
	TLog* mLog;
	TEmulator* mEmulator;
	KUInt32 mLocation;
	KUInt32 mInterruptID;
	SerialStatus mSerialStatus;
	SerialOutputControl mSerialOutputs;

	static inline KUInt32 mCurrentInterruptID = TInterruptManager::kPCMCIA0IntMask << 1;
};

#endif
