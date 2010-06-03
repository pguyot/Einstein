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

/*
 Apple Newton and the PCMCIA Bus

 PCMCIA Memory Bus Pinout and Newton address mapping, Data and Address lines are
 directly connected to the CPU and not part of the diagram
 
 PIN Memory  I/O Dir Register Bitmask Description
  1  GND		 --
  7  !CE1        ->  1c00 0080	Card Enable
  9  !OE         ->				Output Enable
 15  !WE         ->				Write Enable
 16  READY !IREQ <-  1c00 0400	Ready, Busy / Interrupt Request (1=Card is reday to receive more data)
 17  Vcc         ->  2800 0020	Power
 18  Vpp1        ->  2c00 0004	Programming Power
 33  WP !IOIS16  <-  1c00 0200	Write Protect / IO is 16 bit wide
 36  !CD1        <-  1c00 0004	Card Detect
 42  !CE2        ->  1c00 0100	Card Enable
 43  !VS1        <>  1c00 0010  Refresh
 44  x !IORD     ->				IO Read Enable  (with Register Select)
 45  x !IOWR     ->				IO Write Enable (with Register Select)
 51  Vcc         ->				Power
 52  Vpp2        ->				Programming Power
 57  !VS2        <>  1c00 0020	Reserved
 58  RESET       ->  2400 1000	Card Reset
 59  !WAIT       <-				Extend Bus Cycle
 60  x !INPACK   <-				Input Port Acknowledge
 61  !REG        ??  1c00 0040	Register Select and IO Enable (1=Access Memory, 0=Access Registers)
 62  BVD2 !SPKR  <-  1c00 0002	Battery Voltage / Speaker Out
 63  BVD1 !STCH	 <-  1c00 0001	Battery Voltage / Card Status Changed
 67  !CD2        <-  1c00 0008  Card Detect
 
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
	enum {
		kSocketBusyIntVector			= 0x0400,	///< 0: Memory card busy interrupt
		kSocketStatusChangedIntVector	= 0x0001,	///< 1: IO card status changed interrupt#
		kSocketCardDetectedIntVector	= 0x000C,	///< 2: Card detected interrupt, called by TPCMCIAController::InsertCard
		kSocketCardLockIntVector		= 0x0000,	///< 3: Card unlock/remove interupt, In fact, we use GPIO
		kSocketCardIREQIntVector		= 0x0400,	///< 4: IO card IREQ interrupt
		kSocketCardInsertIntVector		= 0x0000,	///< 5: Card inserted interrupt#, Unused, Brick only
		kSocketSpeakerIntVector			= 0x0002,	///< 6: Speaker interrupt#
		kSocketDREQIntVector			= 0x0040,	///< 7: DMA REQ interrupt
		kSocketWPIntVector				= 0x0200,	///< 8: WP interrupt
		kSocketReadFailureIntVector		= 0x4000,	///< 9: Read failure interrupt
		kSocketWriteFailureIntVector	= 0x2000,   ///< 10: Write failure interrupt
		kSocketWrFIFOEmptyIntVector		= 0x1000,	///< 11: Write FIFO empty interrupt
		kSocketWrFIFOThreshldIntVector	= 0x0800	///< 12: Write FIFO threshold met (2 or 4 entries interrupt)
	};
	
	///
	/// Constructor from the log, the emulator and a socket number.
	///
	TPCMCIAController(
			TLog* inLog,
			TEmulator* inEmulator,
			int inSocketIx );

	///
	/// Destructor.
	///
	~TPCMCIAController( void );

	// Memory Interface.
	
	///
	/// Read a word.
	///
	KUInt32	Read( KUInt32 inOffset );

	///
	/// Write a word.
	///
	void	Write( KUInt32 inOffset, KUInt32 inValue );

	///
	/// Read a byte.
	///
	KUInt8	ReadB( KUInt32 inOffset );

	///
	/// Write a byte.
	///
	void	WriteB( KUInt32 inOffset, KUInt8 inValue );

	// Emulator Interface.

	///
	/// Insert a card.
	///
	void	InsertCard( TPCMCIACard* inCard );
	
	///
	/// Remove the card.
	///
	void	RemoveCard( void );

	// Card Interface.

	///
	/// Raise an interrupt.
	///
	void	RaiseInterrupt( int inVector = kSocketCardIREQIntVector );

	///
	/// Accessor to the log interface.
	///
	/// \return a pointer to the log object or nil.
	///
	inline TLog*				GetLog( void )
		{
			return mLog;
		}

	///
	/// Accessor to the emulator.
	///
	/// \return a pointer to the emulator object.
	///
	inline TEmulator*			GetEmulator( void )
		{
			return mEmulator;
		}

	///
	/// Log register data.
	///
	void LogRegister(KUInt32 reg, KUInt32 value);
	
	/// \name Constants
	enum {
		kAttrEndMask		= 0x3FFFFFF,
		kIOSpace			= 0x4000000,
		kIOEndMask			= 0x7FFFFFF,
		kMemSpace			= 0x8000000,
		kMemEndMask			= 0xBFFFFFF,
		kHdWr_Reg0000		= 0xC000000,	/// Int raised.
		kHdWr_IntCtrlReg	= 0xC000400,
		kHdWr_Reg0800		= 0xC000800,
		kHdWr_Reg0C00		= 0xC000C00,	/// Int raised (?).
		kHdWr_Reg1000		= 0xC001000,
		kHdWr_Reg1400		= 0xC001400,
		kHdWr_Reg1800		= 0xC001800,
		kHdWr_Reg1C00		= 0xC001C00,
		kHdWr_Reg2000		= 0xC002000,
		kHdWr_Reg2400		= 0xC002400,
		kHdWr_Reg2800		= 0xC002800,
		kHdWr_Reg2C00		= 0xC002C00,
		kHdWr_Reg3000		= 0xC003000,
		kHdWr_Reg3400		= 0xC003400,
		kHdWr_Reg3800		= 0xC003800,
		kHdWr_Reg3C00		= 0xC003C00,
		kHdWr_Reg4000		= 0xC004000,
		kHdWr_Reg4400		= 0xC004400
	};
	
	enum {
		// Register 1C00
		k1C00_CardIsPresent		= 0x000C,
		//       PC_CF
		k1C00_Pin63_46			= 0x0001,	///< BVD1, -STSCHG, -PDIAG	[I/O]
		k1C00_Pin62_45			= 0x0002,	///< BVD2, -SPKR, -DASP		[I/O]
		k1C00_Pin36_26			= 0x0004,	///< -CD1
		k1C00_Pin67_25			= 0x0008,	///< -CD2
		k1C00_Pin43_33			= 0x0010,	///< VS1
		k1C00_Pin57_40			= 0x0020,	///< VS2
		k1C00_Pin61_44			= 0x0040,	///< REG (kCardVDREQ)
		k1C00_Pin07_07			= 0x0080,	///< -CE1 (kCardVPCTL1)		[I]
		k1C00_Pin42_32			= 0x0100,	///< -CE2 (kCardVPCTL2)		[I]
		k1C00_Pin33_24			= 0x0200,	///< WP, -IOIS16
		k1C00_Pin16_37			= 0x0400,	///< RDY/-BSY, IREQ, INTRQ
		k1C00_VWrFIFOThreshld	= 0x0800,	///< kCardVWrFIFOThreshld
		k1C00_VWrFIFOEmpty		= 0x1000,	///< kCardVWrFIFOEmpty
		k1C00_VWriteFailure		= 0x2000,	///< kCardVWriteFailure
		k1C00_VReadFailure		= 0x4000,	///< kCardVReadFailure
		k1C00_VPCPins			= 0x7FFF,
		
		// Register 2000
		k2000_DynamicSwap		= 0x0800,	///< kCardDynamicSwap
		k2000_SwapSize			= 0x0400,	///< kCardSwapSize
		k2000_Assembly32		= 0x0100,	///< kCardAssembly32
		k2000_HandshakeReady	= 0x0080,	///< kCardHandshakeReady
		k2000_EndianConvert		= 0x0020,	///< !kCardNoEndianConvert
		k2000_RdWrQueueCtrl		= 0x001F,	///< SetRdWrQueueControl
		
		// Register 2400
		k2400_ResetPCMCIA		= 0x1000,	///< Asserted for 50 ticks.
		k2400_EnableResetOut	= 0x0800,	///< !kCardDisableResetOut
		k2400_EnableBus			= 0x0400,
		k2400_SelectIO			= 0x0100,	///< if set, this is I/O, if clear, this is memory
		k2400_SWWriteProtect	= 0x0200,	///< kSWWriteProtect
		k2400_NoByteAccess		= 0x0080,	///< !kCardByteAccess
		
		// Register 2800
		k2800_PullUpControl		= 0x7C00,	///< SetPullupControl
		k2800_VccOn				= 0x0020,	///< VccOn/VccOff
		
		// Register 2C00
		k2C00_VppOn				= 0x0004,	///< IsVppOn
		k2C00_VS1Out			= 0x1000,	///< kCardVS1Out
		k2C00_VS2Out			= 0x2000,	///< kCardVS2Out
		k2C00_VS1Dir			= 0x4000,	///< kCardVS1Dir (forces k2C00_VS1Out)
		k2C00_VS2Dir			= 0x8000	///< kCardVS2Dir (forces k2C00_VS2Out)
	};
	
private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TPCMCIAController( const TPCMCIAController& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TPCMCIAController& operator = ( const TPCMCIAController& inCopy );

	/// \name Variables
	TLog*				mLog;			///< Interface to the log.
	TInterruptManager*	mIntManager;	///< Interrupt manager
	TEmulator*			mEmulator;		///< Emulator (access to hardware).
	int					mSocketIx;		///< Socket index [0-3]
	TPCMCIACard*		mCard;			///< Card, if any.
	
	KUInt32				mReg_0000;		///< Value of xC000000 (r   - int, pending interrupts?)
	KUInt32				mIntCtrlReg;	///< Value of xC000400 (r/w - int, enables/1 and disables/0 interrupts)
	KUInt32				mReg_0800;		///< Value of xC000800 (w   - int)
	KUInt32				mReg_0C00;		///< Value of xC000C00 (r/w - int, promote interrupts to FIQ)
	KUInt32				mReg_1000;		///< Value of xC001000 (r/w - int, interrupt on rising edge)
	KUInt32				mReg_1400;		///< Value of xC001400 (w   - int, interrupt on falling edge)
	KUInt32				mReg_1800;		///< Value of xC001800 (r/w - int, wake up if interrupted)
	KUInt32				mReg_1C00;		///< Value of xC001C00 (r,  - pin acces, FIFO, see above)
	KUInt32				mReg_2000;		///< Value of xC002000 (r/w - FIFO)
	KUInt32				mReg_2400;		///< Value of xC002400 (r/w - pin access and direction)
	KUInt32				mReg_2800;		///< Value of xC002800 (r/w - pullups, power)
	KUInt32				mReg_2C00;		///< Value of xC002C00 (r/w - power, pin access)
	KUInt32				mReg_3000;		///< Value of xC003000 (r/w - memory speed )
										///< Bits 3F -> Attribute & Mem Speed
										///< R/W for GetChipInfo.
	KUInt32				mReg_3400;		///< Value of xC003400 (w   - ??
	KUInt32				mReg_3800;		///< Value of xC003800 (r/w - I/O Speed )
										///< Bits 3F -> I/O Speed.
										///< R/W for GetChipInfo.
	KUInt32				mReg_3C00;		///< Value of xC003C00 (w   - ?? )
	KUInt32				mReg_4000;		///< Value of xC004000 (w   - ?? )
	KUInt32				mReg_4400;		//                     (    - ?? )
};

#endif
		// _TPCMCIACONTROLLER_H

// ============================================================================ //
// Beware of the Turing Tar-pit in which everything is possible but nothing of  //
// interest is easy.                                                            //
// ============================================================================ //
