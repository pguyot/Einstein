// ==============================
// File:			TPCMCIAController.h
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

#ifndef _TPCMCIACONTROLLER_H
#define _TPCMCIACONTROLLER_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TEmulator;
class TInterruptManager;
class TPCMCIACard;
class TStream;

/*
 Apple Newton and the PCMCIA Bus

 The MessagePad 2x00/eMate PCMCIA controller is called "Voyager" in the DDK
 (CardSocket.h), the ROM symbols call it "Bradly" (gBradlyChipInfo) and its
 interrupt dispatcher "Tric" (DispatchTricInterrupt).

 Most of the information below was verified against the TCardSocket
 implementation in the MP2x00 ROM (ROM addresses 0x00054D9C..0x0005612C) and
 the DDK header PCMCIA/CardSocket.h. Bits marked "unknown" are set by the ROM
 but never interpreted by it.

 PCMCIA Memory Bus Pinout and Newton address mapping, Data and Address lines are
 directly connected to the CPU and not part of the diagram

 PIN Memory  I/O Dir Register Bitmask Description
  1  GND		 --
  7  !CE1        ->  1c00 0080	Card Enable (DDK: kCardVPCTL1 "reserved", pin unverified)
  9  !OE         ->				Output Enable
 15  !WE         ->				Write Enable
 16  READY !IREQ <-  1c00 0400	Ready, Busy / Interrupt Request (1=Card is ready to receive more data)
 17  Vcc         ->  2800 0020	Power on/off (voltage selected in 2c00 0003)
 18  Vpp1        ->  2c00 0004	Programming Power: 1=12V, 0=same as Vcc
 33  WP !IOIS16  <-  1c00 0200	Write Protect / IO is 16 bit wide
 36  !CD1        <-  1c00 0004	Card Detect (active low)
 42  !CE2        ->  1c00 0100	Card Enable (DDK: kCardVPCTL2 "reserved", pin unverified)
 43  !VS1        <>  1c00 0010  Voltage Sense 1 (input), output via 2c00 1000/4000
 44  x !IORD     ->				IO Read Enable  (with Register Select)
 45  x !IOWR     ->				IO Write Enable (with Register Select)
 51  Vcc         ->				Power
 52  Vpp2        ->				Programming Power
 57  !VS2        <>  1c00 0020	Voltage Sense 2 (input), output via 2c00 2000/8000
 58  RESET       ->  2400 1000	Card Reset
 59  !WAIT       <-				Extend Bus Cycle
 60  x !INPACK   <-				Input Port Acknowledge
 61  !REG        ??  1c00 0040	DDK: kCardVDREQ, "DMA REQ" (pin unverified)
 62  BVD2 !SPKR  <-  1c00 0002	Battery Voltage / Speaker Out
 63  BVD1 !STCH	 <-  1c00 0001	Battery Voltage / Card Status Changed
 67  !CD2        <-  1c00 0008  Card Detect (active low)

 Socket address space (offsets from the socket base, see gCardSocketVAddr):

 0x0000000-0x3FFFFFF  Attribute memory
 0x4000000-0x7FFFFFF  I/O space
 0x8000000-0xBFFFFFF  Common memory
 0xC000000-0xC0044FF  Controller registers, one 32 bit register every 0x400
					  bytes. Only the lower 16 bits are significant.

 Interrupt registers (0x0000..0x1800) all use the same bit layout, which is
 the layout of the pin register 0x1C00 (see kSocket...IntVector below):

 0x0000  r   Pending interrupts (raw status). The dispatcher uses
			 (0x0000 & 0x0400) to find active interrupts.
 0x0400  r/w Interrupt enable mask. EnableSocketInterrupt() sets bits,
			 DisableSocketInterrupt() clears them.
 0x0800  w   Interrupt clear (write 1 to acknowledge). Written by
			 ClearSocketInterrupt(), by EnableSocketInterrupt() before enabling,
			 and by the dispatcher before calling the handler.
 0x0C00  r/w Promote to FIQ (kSocketIntPromoteFIQ). The ROM registers the
			 dispatcher on two CPU interrupt lines that serve *all* sockets:
			 0x00010000 (gPCMCIA0IntObj) only handles interrupts whose bit is
			 set in 0x0C00, 0x02000000 (gPCMCIA1IntObj) handles all others.
 0x1000  r/w Trigger on rising edge (kSocketIntEnableRising)
 0x1400  r/w Trigger on falling edge (kSocketIntEnableFalling)
 0x1800  r/w Wake up the Newton on this interrupt (kSocketIntWakeup)

 ResetInterrupts() initializes these to:
	0x0400 &= 0x800C  (only card detect stays enabled)
	0x0C00  = 0x0400  (Ready/IREQ goes to FIQ)
	0x1400  = 0x040D  (falling: Ready/IREQ, CD1, CD2, BVD1/STSCHG)
	0x1000  = 0x7A00  (rising: R/W failure, FIFO empty/threshold, WP)
	0x0800  = 0x7FF3  (clear everything but card detect)

 The card server uses falling edge for card detect while waiting for a card,
 and rising edge plus wakeup (0x0009) once a card is inserted.

 The dispatcher services pending interrupts in this order (intPriorityVectors):
 0x0400, 0x000C, 0x0200, 0x0040, 0x1000, 0x0800, 0x0002, 0x0001, 0x2000, 0x4000

 Card lock/eject switch interrupts (kSocketCardLockInt) do not use the
 controller, but GPIO lines 2, 3, 13, 14 for sockets 0..3
 (gpioIntVectorNumbers).
 */

///
/// Class for the PCMCIA controllers.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TPCMCIAController
{
public:
	/// Interrupt bits, indexed by TSocketInt in the DDK. The values match the
	/// ROM table intVectorNumbers (ROM 0x0071AC24) exactly.
	enum {
		kSocketBusyIntVector = 0x0400, ///< 0: Memory card busy interrupt
		kSocketStatusChangedIntVector = 0x0001, ///< 1: IO card status changed interrupt#
		kSocketCardDetectedIntVector = 0x000C, ///< 2: Card detected interrupt, raised by InsertCard/RemoveCard if the edge is enabled
		kSocketCardLockIntVector = 0x0000, ///< 3: Card unlock/remove interupt, In fact, we use GPIO
		kSocketCardIREQIntVector = 0x0400, ///< 4: IO card IREQ interrupt (same pin as Busy)
		kSocketCardInsertIntVector = 0x0000, ///< 5: Card inserted interrupt#, Unused, Brick only
		kSocketSpeakerIntVector = 0x0002, ///< 6: Speaker interrupt#
		kSocketDREQIntVector = 0x0040, ///< 7: DMA REQ interrupt
		kSocketWPIntVector = 0x0200, ///< 8: WP interrupt
		kSocketReadFailureIntVector = 0x4000, ///< 9: Read failure interrupt
		kSocketWriteFailureIntVector = 0x2000, ///< 10: Write failure interrupt
		kSocketWrFIFOEmptyIntVector = 0x1000, ///< 11: Write FIFO empty interrupt
		kSocketWrFIFOThreshldIntVector = 0x0800 ///< 12: Write FIFO threshold met (2 or 4 entries interrupt)
	};

	///
	/// Constructor from the log, the emulator and a socket number.
	///
	TPCMCIAController(
		TLog* inLog,
		TEmulator* inEmulator,
		int inSocketIx);

	///
	/// Destructor.
	///
	~TPCMCIAController(void);

	// Memory Interface.

	///
	/// Read a word.
	///
	KUInt32 Read(KUInt32 inOffset);

	///
	/// Write a word.
	///
	void Write(KUInt32 inOffset, KUInt32 inValue);

	///
	/// Read a byte.
	///
	KUInt8 ReadB(KUInt32 inOffset);

	///
	/// Write a byte.
	///
	void WriteB(KUInt32 inOffset, KUInt8 inValue);

	// Emulator Interface.

	///
	/// Insert a card.
	///
	void InsertCard(TPCMCIACard* inCard);

	///
	/// Remove the card.
	///
	void RemoveCard(void);

	///
	/// Save or load the controller registers and the state of the card.
	///
	void TransferState(TStream* inStream);

	///
	/// Return the card in the slot or nullptr if empty.
	///
	TPCMCIACard*
	CurrentCard()
	{
		return mCard;
	}

	// Card Interface.

	///
	/// Raise an interrupt.
	///
	void RaiseInterrupt(int inVector = kSocketCardIREQIntVector);

	///
	/// Accessor to the log interface.
	///
	///  \return a pointer to the log object or nil.
	///
	inline TLog*
	GetLog(void)
	{
		return mLog;
	}

	///
	/// Accessor to the emulator.
	///
	///  \return a pointer to the emulator object.
	///
	inline TEmulator*
	GetEmulator(void)
	{
		return mEmulator;
	}

	///
	/// Log register data.
	///
	void LogRegister(KUInt32 reg, KUInt32 value);

	///
	/// Get memory mapping register access
	///
	KUInt32
	GetReg2000()
	{
		return mReg_2000;
	}

	/// \name Constants
	enum {
		kAttrEndMask = 0x3FFFFFF,
		kIOSpace = 0x4000000,
		kIOEndMask = 0x7FFFFFF,
		kMemSpace = 0x8000000,
		kMemEndMask = 0xBFFFFFF,
		kHdWr_Reg0000 = 0xC000000, ///< r: pending interrupts
		kHdWr_IntCtrlReg = 0xC000400, ///< r/w: interrupt enable
		kHdWr_Reg0800 = 0xC000800, ///< w: interrupt clear (write 1 to clear)
		kHdWr_Reg0C00 = 0xC000C00, ///< r/w: promote interrupt to FIQ
		kHdWr_Reg1000 = 0xC001000, ///< r/w: interrupt on rising edge
		kHdWr_Reg1400 = 0xC001400, ///< r/w: interrupt on falling edge
		kHdWr_Reg1800 = 0xC001800, ///< r/w: interrupt wakes up the Newton
		kHdWr_Reg1C00 = 0xC001C00, ///< r: pin and FIFO status
		kHdWr_Reg2000 = 0xC002000, ///< r/w: data path and FIFO control
		kHdWr_Reg2400 = 0xC002400, ///< r/w: bus, interface and reset control
		kHdWr_Reg2800 = 0xC002800, ///< r/w: Vcc and pullups
		kHdWr_Reg2C00 = 0xC002C00, ///< r/w: voltage select and VS pins
		kHdWr_Reg3000 = 0xC003000, ///< r/w: memory wait states
		kHdWr_Reg3400 = 0xC003400, ///< w: unknown, set to 0 at init
		kHdWr_Reg3800 = 0xC003800, ///< r/w: I/O wait states
		kHdWr_Reg3C00 = 0xC003C00, ///< w: unknown, set to 0 at init
		kHdWr_Reg4000 = 0xC004000, ///< w: unknown, set to 0 at init
		kHdWr_Reg4400 = 0xC004400 ///< r: chip ID
	};

	enum {
		// Register 1C00, pin and FIFO status, read only (TCardSocket::GetVPCPins
		// returns it masked with 0x7FFF). The ROM never writes this register.
		// The same bit layout is used by all interrupt registers 0000..1800.
		// TCardSocket::GetPCPins converts it into the older kCard... layout.
		k1C00_CardIsPresent = 0x000C, ///< -CD1|-CD2, both clear when a card is fully inserted (IsCardDetected)
		//       PC_CF
		k1C00_Pin63_46 = 0x0001, ///< BVD1, -STSCHG, -PDIAG	[I/O] (kCardVBVD1Stschg)
		k1C00_Pin62_45 = 0x0002, ///< BVD2, -SPKR, -DASP		[I/O] (kCareVBVD2Spkr)
		k1C00_Pin36_26 = 0x0004, ///< -CD1, active low (kCardVCD1)
		k1C00_Pin67_25 = 0x0008, ///< -CD2, active low (kCardVCD2)
		k1C00_Pin43_33 = 0x0010, ///< VS1 (kCardVVS1)
		k1C00_Pin57_40 = 0x0020, ///< VS2 (kCardVVS2), VS1|VS2: 5V card, VS2 only or none: 3.3V card, VS1 only: X.X V card (rejected)
		k1C00_Pin61_44 = 0x0040, ///< DMA request (kCardVDREQ), pin assignment unverified
		k1C00_Pin07_07 = 0x0080, ///< PCTL1 (kCardVPCTL1, "reserved"), pin assignment unverified	[I]
		k1C00_Pin42_32 = 0x0100, ///< PCTL2 (kCardVPCTL2, "reserved"), pin assignment unverified	[I]
		k1C00_Pin33_24 = 0x0200, ///< WP, -IOIS16 (kCardVWPIOIs16)
		k1C00_Pin16_37 = 0x0400, ///< RDY/-BSY, IREQ, INTRQ (kCardVReadyIREQ)
		k1C00_VWrFIFOThreshld = 0x0800, ///< kCardVWrFIFOThreshld, write FIFO holds 2 or 4 entries (see k2000_WrFIFOIntCntl)
		k1C00_VWrFIFOEmpty = 0x1000, ///< kCardVWrFIFOEmpty
		k1C00_VWriteFailure = 0x2000, ///< kCardVWriteFailure
		k1C00_VReadFailure = 0x4000, ///< kCardVReadFailure
		k1C00_VPCPins = 0x7FFF,

		// Register 2000, data path and FIFO control. Init value is 0x0368.
		// Bits 0x0200 and 0x0040 are set at init, but never used by the ROM.
		k2000_DynamicSwap = 0x0800, ///< kCardDynamicSwap, also set temporarily by Do16BitRead/Write for unaligned 16 bit access
		k2000_SwapSize = 0x0400, ///< kCardSwapSize
		k2000_Assembly32 = 0x0100, ///< kCardAssembly32, combine two 16 bit card accesses into one 32 bit access
		k2000_HandshakeReady = 0x0080, ///< kCardHandshakeReady, handshake with the ready pin
		k2000_EndianConvert = 0x0020, ///< !kCardNoEndianConvert
		k2000_RdWrQueueCtrl = 0x001F, ///< SetRdWrQueueControl, see below
		k2000_WrFIFOEnable = 0x0010, ///< kCardWrFIFOEnable
		k2000_WrFIFOIntCntl = 0x0008, ///< kCardWrFIFOIntCntl, FIFO threshold interrupt at 4 (1) or 2 (0) entries
		k2000_WrFIFOFlush = 0x0004, ///< kCardWrFIFOFlush
		k2000_RdPrefetch = 0x0002, ///< kCardRdPrefectch, memory read prefetch
		k2000_RdIOPrefetch = 0x0001, ///< kCardRdIOPrefectch, I/O read prefetch

		// Register 2400, bus, interface and reset control. Init value is 0x08D2.
		// Bits 0x0040, 0x0010, and 0x0002 are set at init, but never used by the ROM.
		k2400_ResetPCMCIA = 0x1000, ///< Card RESET, asserted for 80 timer ticks (~22us) by PCMCIAReset
		k2400_EnableResetOut = 0x0800, ///< !kCardDisableResetOut
		k2400_EnableBus = 0x0400, ///< EnableBus/DisableBus, IsPCMCIABusEnable
		k2400_SWWriteProtect = 0x0200, ///< kCardSWWriteProtect, software write protect
		k2400_SelectIO = 0x0100, ///< if set, this is I/O, if clear, this is memory (SelectIOInterface, IsIOInteface)
		k2400_NoByteAccess = 0x0080, ///< !kCardByteAccess

		// Register 2800, Vcc and pullups. Init value is 0x1C45.
		// Bits 0x0040, 0x0004, and 0x0001 are set at init, but never used by the ROM.
		k2800_PullUpControl = 0x7C00, ///< SetPullupControl, see below
		k2800_RdyWaitPullupEn = 0x4000, ///< kCardRdyWaitPullupEn, strong pullups for READY and WAIT
		k2800_DisProtBVD1 = 0x2000, ///< kCardDisProtBVD1, disable BVD1 input protection while card is off
		k2800_InputPullupEn = 0x1000, ///< kCardInputPullupEn
		k2800_CDPullupControl = 0x0C00, ///< kCardCDPullupControl1/0, 11: weak CD pullup always on, 10: except in sleep
		k2800_VccOn = 0x0020, ///< VccOn/VccOff, IsVccOn

		// Register 2C00, voltage select and VS pins. Init value is 0x31C1.
		// Bits 0x0100, 0x0080, and 0x0040 are set at init, but never used by the ROM.
		k2C00_VccSelect = 0x0003, ///< SelectVoltageLevel: 01 = 5V Vcc, 10 = 3.3V Vcc
		k2C00_Vcc5V = 0x0001, ///< kSocketPowerLevelVcc5V
		k2C00_Vcc3p3V = 0x0002, ///< kSocketPowerLevelVcc3p3V
		k2C00_VppOn = 0x0004, ///< Vpp is 12V (kSocketPowerLevelVpp12V), else Vpp follows Vcc. IsVppOn tests this and k2800_VccOn.
		k2C00_VS1Out = 0x1000, ///< kCardVS1Out
		k2C00_VS2Out = 0x2000, ///< kCardVS2Out
		k2C00_VS1Dir = 0x4000, ///< kCardVS1Dir, 1: output. SetControl sets k2C00_VS1Out if this bit is clear.
		k2C00_VS2Dir = 0x8000, ///< kCardVS2Dir, 1: output. SetControl sets k2C00_VS2Out if this bit is clear.

		// Register 3000 and 3800, wait states. Init value is 0x7F00, high bits are
		// never touched again. Card access time is (n+2) cycles of the high
		// speed clock (GetHighSpeedClock), default is 300ns, 600ns for
		// attribute memory on 3.3V cards. GetChipInfo also verifies that both
		// registers hold 16 bit values, or the socket is not used.
		k3000_WaitCount = 0x003F, ///< SetAttributeMemSpeed, SetCommonMemSpeed (both use this register)
		k3800_WaitCount = 0x003F, ///< SetIOSpeed

		// Register 4400, read only
		k4400_ChipID = 0x00FC ///< GetChipInfo, stored in gBradlyChipInfo for socket 0, must not be 0
	};

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TPCMCIAController(const TPCMCIAController& inCopy) = delete;

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TPCMCIAController& operator=(const TPCMCIAController& inCopy) = delete;

	///
	/// Raise the CPU interrupt lines for pending and enabled interrupts.
	///
	void UpdateInterruptLines(void);

	///
	/// Read the pins from the card into mReg_1C00 and return them.
	///
	KUInt32 UpdatePins(void);

	///
	/// Turn pin changes into pending interrupts, as set in 0x1000 and 0x1400.
	///
	void LatchPinChanges(KUInt32 inOldPins, KUInt32 inNewPins);

	// \name Variables

	/// Interface to the log.
	TLog* mLog { nullptr };

	/// Interrupt manager
	TInterruptManager* mIntManager { nullptr };

	/// Emulator (access to hardware).
	TEmulator* mEmulator { nullptr };

	/// Socket index [0-3]
	int mSocketIx { 0 };

	/// Card, if any.
	TPCMCIACard* mCard { nullptr };

	/// Value of xC000000 (r   - int, pending interrupts, ANDed with mIntCtrlReg by the dispatcher)
	KUInt32 mReg_0000 { 0 };

	/// Value of xC000400 (r/w - int, enables/1 and disables/0 interrupts)
	KUInt32 mIntCtrlReg { 0 };

	/// Value of xC000800 (w   - int, writing 1 clears the pending interrupt in xC000000)
	KUInt32 mReg_0800 { 0 };

	/// Value of xC000C00 (r/w - int, promote interrupts to FIQ, i.e. CPU interrupt 0x00010000 instead of 0x02000000)
	KUInt32 mReg_0C00 { 0 };

	/// Value of xC001000 (r/w - int, interrupt on rising edge)
	KUInt32 mReg_1000 { 0 };

	/// Value of xC001400 (r/w - int, interrupt on falling edge)
	KUInt32 mReg_1400 { 0 };

	/// Value of xC001800 (r/w - int, wake up if interrupted)
	KUInt32 mReg_1800 { 0 };

	/// Value of xC001C00 (r   - pin access, FIFO status, see k1C00_...)
	KUInt32 mReg_1C00 { 0 };

	/// Value of xC002000 (r/w - data path and FIFO control, see k2000_...)
	KUInt32 mReg_2000 { 0 };

	/// Value of xC002400 (r/w - bus enable, I/O or memory interface, reset, see k2400_...)
	KUInt32 mReg_2400 { 0 };

	/// Value of xC002800 (r/w - pullups, Vcc on/off, see k2800_...)
	KUInt32 mReg_2800 { 0 };

	/// Value of xC002C00 (r/w - Vcc and Vpp voltage select, VS pins, see k2C00_...)
	KUInt32 mReg_2C00 { 0 };

	/// Value of xC003000 (r/w - memory speed )
	/// Bits 3F -> Attribute & Mem Speed
	/// R/W for GetChipInfo.
	KUInt32 mReg_3000 { 0 };

	/// Value of xC003400 (w   - ??, only ever set to 0 by TCardSocket::Init)
	KUInt32 mReg_3400 { 0 };

	/// Value of xC003800 (r/w - I/O Speed )
	/// Bits 3F -> I/O Speed.
	/// R/W for GetChipInfo.
	KUInt32 mReg_3800 { 0 };

	/// Value of xC003C00 (w   - ??, only ever set to 0 by TCardSocket::Init)
	KUInt32 mReg_3C00 { 0 };

	/// Value of xC004000 (w   - ??, only ever set to 0 by TCardSocket::Init)
	KUInt32 mReg_4000 { 0 };

	// the register 0x4400 is only read and always reads 0xFC. GetChipInfo
	// returns bits 0xFC as a chip ID and stores them in gBradlyChipInfo.
	// TCardSocket::Init disables the socket (error -10008) if the ID is 0,
	// otherwise the value is not interpreted.
};

#endif
// _TPCMCIACONTROLLER_H

// ============================================================================ //
// Beware of the Turing Tar-pit in which everything is possible but nothing of  //
// interest is easy.                                                            //
// ============================================================================ //
