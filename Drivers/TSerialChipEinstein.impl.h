// ==============================
// File:			TSerialChipEinstein.impl.h
// Project:			Einstein
//
// Copyright 2020 by Eckhart Koeppen (eck@40hz.org)
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

#ifndef _TSERIALCHIPEINSTEIN_IMPL_H
#define _TSERIALCHIPEINSTEIN_IMPL_H

#include "SerialChipV2.h"

struct InterruptObject;

class THMOSerialEinsteinHardware : public TOption
{
public:
	THMOSerialEinsteinHardware();

	ULong	fLocationID;	// synthetic hardware location
	ULong	fType;			// host side driver type
    ULong   fDataSize;      // size of configuration data
    UChar   fData[60];      // configuration data
};

///
/// Class for the emulated TSerialChipEinstein chipset.
///
/// \author Eckhart Koeppen <eck@40hz.org>
///
PROTOCOL TSerialChipEinstein : public TSerialChip
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(TSerialChipEinstein);

	CAPABILITIES( ("copyright" "Eckhart Koeppen") ("version" "1.0") )

	TSerialChip*		New( void );				// PROTOCOL ctor
	void				Delete( void );				// PROTOCOL dtor

	NewtonErr 			InstallChipHandler(void* serialTool, SCCChannelInts* intHandlers);
	NewtonErr 			RemoveChipHandler(void* serialTool);

	void				PutByte(UByte nextChar);	// ** only called if TxBufEmpty() or TxBE int
	void				ResetTxBEmpty();			// ** called if TxBE int and no more to write
	UByte				GetByte();					// ** only called if RxBufFull() or RxBF int
	Boolean				TxBufEmpty();				// ** is transmitter ready for another byte?
	Boolean				RxBufFull();				// ** does receiver have a byte available?
	RxErrorStatus		GetRxErrorStatus();			// ** return receive error status indications for current
													//  byte available - this resets the status
	SerialStatus		GetSerialStatus();			// get status bits
	void				ResetSerialStatus();		// reset latched status bits
	void				SetSerialOutputs(SerialOutputControl); 		// **
	void				ClearSerialOutputs(SerialOutputControl);	// **
	SerialOutputControl	GetSerialOutputs();

	void				PowerOff();					// turn power off (may not be called)
	void				PowerOn();					// turn power back on and reconfigure
	Boolean				PowerIsOn();				// is serial chip powered?
	void				SetInterruptEnable(Boolean enable); // ** enable/disable interrupts

	void				Reset();					// **
	void				SetBreak(Boolean assert);	// **

	InterfaceSpeed		SetSpeed(BitRate bitsPerSec);
	void				SetIOParms(TCMOSerialIOParms* opt);	// set bit rate, parity, data bits, stop bits
	void				Reconfigure();				// ** reconfigure based on speed, IOParms

	//---------------------------------------------
	// Special methods for PCMCIA card serial chips

	NewtonErr			Init(TCardSocket* theCardSocket, TCardHandler* theCardHandler, UByte* baseRegAddr);
	void				CardRemoved(void);			// async "card gone" call - call with IRQs disabled!

	//	----------------------------------------------------------------
	//
	//	2.0 calls
	//
	SerialFeatures		GetFeatures();				// return features vector of this chip
	NewtonErr			InitByOption(TOption* initOpt); // used instead of Init for 2.0 interfaces.
	NewtonErr 			ProcessOption(TOption* opt); // process option from client
	NewtonErr			SetSerialMode(SerialMode mode); // call after binding

	void				SysEventNotify(ULong event); // called when serial tool gets a power on/off event
													// event is generally kSysEvent_PowerOn or kSysEvent_PowerOff
													// called after comm tool for power-off, and before for power-on

	void				SetTxDTransceiverEnable(Boolean enable); // **
	RxErrorStatus		GetByteAndStatus(UByte* nextCharPtr); // ** NOTE: GetRxErrorStatus not called if this is available

	NewtonErr			SetIntSourceEnable(SerialIntSource src, Boolean enable);
													// enable/disable specific ints. Returns error
													//  if that particular source cannot be controlled?
													//  Only used currently to shut off CTS ints on auto-clock detect.
	Boolean				AllSent();					// ** have all transmit characters left chip?
	void 				ConfigureForOutput(Boolean start);	// only called if kSerFeatureTxConfigNeeded = 1

	//  ------------------------------------------
	//  Methods for DMA control ...

	NewtonErr			InitTxDMA(TCircleBuf* buf, SCCIntHandler txDMAIntHandler);
	NewtonErr			InitRxDMA(TCircleBuf* buf, ULong notifyLevel, RxDMAIntHandler intHandler);	// int handler takes flags param!
	NewtonErr			TxDMAControl(DMAControl ctl); 	// start, stop, reset, etc.
	NewtonErr			RxDMAControl(DMAControl ctl); 	// start, stop, reset, suspend, etc.

	//  ------------------------------------------
	//  Special methods for Synchronous serial...

	void 				SetSDLCAddress(UByte nodeID); // **
	void				ReEnableReceiver(Boolean reset); // ** reenable receiver for another packet

	//---------------------------------
	// Special methods for LocalTalk...

	Boolean 			LinkIsFree(Boolean resetClks);
	Boolean 			SendControlPacket(UByte pType, UByte dest, Boolean syncPulse); // sync send control packet
	void				WaitForPacket(TTimeout delay); 	// sync wait for a control packet in...
														//  call RxDMAIntHandler if one comes in.
														//  NOTE: may turn async if data packet comes in...

	//	----------------------------------------------------------------
	//
	//	3.0 calls
	//
	NewtonErr			WaitForAllSent(); 			// delay until all bytes have been sent

private:
	NewtonErr			HandleInterrupt();

	ULong				fLocationID;
	ULong				fInterruptID;
	InterruptObject*	fInterruptObject;
	void*				fSerialTool;
	SCCChannelInts 		fIntHandlers;
};

#endif
		// _TSERIALCHIPEINSTEIN_IMPL_H

// ========================================================================= //
// For every problem there is one solution which is simple, neat, and wrong. //
// -- H. L. Mencken                                                          //
// ========================================================================= //
