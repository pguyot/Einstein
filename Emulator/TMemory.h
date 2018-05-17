// ==============================
// File:			TMemory.h
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

#ifndef _TMEMORY_H
#define _TMEMORY_H

#include <K/Defines/KDefinitions.h>

// ANSI C & POSIX
#include <limits.h>
#include <stdio.h>

#if !TARGET_OS_WIN32
	#include <sys/time.h>
#endif

// Einstein
#include "TFlash.h"
#include "JIT.h"
#include "TMemoryConsts.h"
#include "TMMU.h"

class TROMImage;
class TLog;
class TARMProcessor;
class TInterruptManager;
class TDMAManager;
class TSerialPortManager;
class TEmulator;
class TPCMCIAController;
class TStream;

// More than 2 sockets will yield to memory corruption because there aren't
// enough GPIO interrupts available.
// Other parts of the system seem to support up to 4 slots.
#define kNbSockets	2

///
/// Class to handle any access of the processor to the memory space.
/// This class also handles the MMU maps and so on.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TMemory
{
public:
	/// MMU needs to access some physical memory access routines.
	friend class TMMU;

	///
	/// Constructor from the ROM Image and the amount of RAM to use
	/// in the emulator.
	///
	/// \param inLog				interface for logging.
	/// \param inROMImageBuffer		pointer to the ROM Image (not copied).
	/// \param inRAMSize			size of the RAM installed (in bytes)
	/// \param inEmulator			interface to hardware.
	///
	TMemory(
			TLog* inLog,
			KUInt8* inROMImageBuffer,
			const char* inFlashPath,
			KUInt32 inRAMSize = 0x00400000 );

	///
	/// Constructor from the ROM Image and the amount of RAM to use
	/// in the emulator.
	///
	/// \param inLog				interface for logging.
	/// \param inROMImage			ROM Image.
	/// \param inRAMSize			size of the RAM installed (in bytes)
	/// \param inEmulator			interface to hardware.
	///
	TMemory(
			TLog* inLog,
			TROMImage* inROMImage,
			const char* inFlashPath,
			KUInt32 inRAMSize = 0x00400000 );

	///
	/// Destructor.
	/// Frees up all the structures.
	///
	~TMemory( void );

	///
	/// Set the emulator.
	///
	/// \param inEmulator emulator (access to hardware managers).
	///
	void		SetEmulator( TEmulator* inEmulator );

	///
	/// Set the interrupt manager.
	/// Used for tests.
	///
	/// \param inInterruptManager interrupt manager (access to hardware managers).
	///
	void		SetInterruptManager( TInterruptManager* inInterruptManager )
		{
			mInterruptManager = inInterruptManager;
		}

	///
	/// Accessor to the PCMCIA socket.
	///
	TPCMCIAController* GetPCMCIAController( int inIndex )
		{
			return mPCMCIACtrls[ inIndex ];
		}

	///
	/// Accessor on the JIT object.
	///
	JITClass*	GetJITObject( void )
		{
			return &mJIT;
		}
	
	typedef KUInt32 PAddr;	///< Physical address
	typedef KUInt32 VAddr;	///< Virtual address

	///
	/// Power flash off.
	///
	void		PowerOffFlash( void )
		{
			mFlash.PowerOff();
		}

	///
	/// Power flash on.
	///
	void		PowerOnFlash( void )
		{
			mFlash.PowerOn();
		}

	///
	/// Get a direct pointer to a buffer in RAM
	///
	/// \param inAddress the address the device seeks
	/// \param outPTR the actual address in the RAM buffer
	/// \return true if the address couldn't be accessed.
	///
	Boolean		GetDirectPointerToRAM( VAddr inAddress, KUInt8** outPtr );

	///
	/// Get a direct pointer to a buffer in RAM or ROM.
	///
	/// \param inAddress the address the device seeks
	/// \param outPTR the actual address in the RAM buffer or in the ROM buffer.
	/// \return true if the address couldn't be accessed.
	///
	Boolean		GetDirectPointerToROMRAM( VAddr inAddress, const KUInt8** outPtr );

	///
	/// Get a direct pointer to a page.
	///
	/// \param inPAddr			physical address.
	/// \return a pointer to instructions in this page.
	///
	KUInt32*	GetDirectPointerToPage( KUInt32 inPAddr );

	///
	/// Get a pointer into emulated RAM
	///
	KUIntPtr	GetRAMOffset() { return mRAMOffset; }

	///
	/// Fast read data.
	///
	/// \param inAddress		virtual address.
	/// \return true if reading failed.
	///
	Boolean		FastReadBuffer(
					VAddr inAddress,
					KUInt32 inAmount,
					KUInt8* outBuffer );
	
	///
	/// Fast read string, with allocation (with malloc).
	///
	/// \param inAddress		virtual address.
	/// \return true if reading failed.
	///
	Boolean		FastReadString(
					VAddr inAddress,
					char** outString );

	///
	/// Fast read string.
	///
	/// \param inAddress		virtual address.
	/// \return true if reading failed.
	///
	Boolean		FastReadString(
					VAddr inAddress,
					KUInt32* ioCount,
					char* outString );
	
	///
	/// Fast write data.
	///
	/// \param inAddress		virtual address.
	/// \return true if writing failed.
	///
	Boolean		FastWriteBuffer(
					VAddr inAddress,
					KUInt32 inAmount,
					const KUInt8* inBuffer );
	
	///
	/// Fast write string.
	///
	/// \param inAddress		virtual address.
	/// \return true if writing failed.
	///
	Boolean		FastWriteString(
					VAddr inAddress,
					KUInt32* ioCount,
					const char* inString );
	
	///
	/// Read an instruction from memory.
	/// This function must be used with results from the previous function.
	/// It returns the instruction at the given physical address in the
	/// proper bank.
	///
	/// \param inBankNumber			bank index (0 for ROM or 1 for RAM).
	/// \param inOffsetInBank		offset in the bank (address - base).
	/// \return the instruction
	///		
	KUInt32		ReadInstruction(
					KUInt32 inBankNumber,
					KUInt32 inOffsetInBank );
	
	///
	/// Read 32 bits from memory.
	/// Perform address translation if the MMU is enabled.
	///
	/// \param inAddress	virtual address to read 32 bits from.
	/// \param outWord		32 bits word that was read.
	/// \return true if the address couldn't be accessed for reading.
	///		
	Boolean		Read( VAddr inAddress, KUInt32& outWord );
	
	///
	/// Read 32 bits from memory, ignoring two last bits.
	/// Perform address translation if the MMU is enabled.
	///
	/// \param inAddress	virtual address to read 32 bits from.
	/// \param outWord		32 bits word that was read.
	/// \return true if the address couldn't be accessed for reading.
	///		
	Boolean		ReadAligned( VAddr inAddress, KUInt32& outWord );
	
	///
	/// Read 32 bits from memory, with a direct physical address.
	///
	/// \param inAddress	physical address to read 32 bits from.
	/// \param outFault		on return, \c true if there was a bus error
	///						(unchanged otherwise)
	/// \return the word that was read, 0 in case of error.
	///		
	KUInt32		ReadP( PAddr inAddress, Boolean& outFault );
	
	///
	/// Read 32 bits from memory, with a direct physical aligned address.
	///
	/// \param inAddress	physical address to read 32 bits from.
	/// \param outFault		on return, \c true if there was a bus error
	///						(unchanged otherwise)
	/// \return the word that was read, 0 in case of error.
	///		
	KUInt32		ReadPAligned( PAddr inAddress, Boolean& outFault );
	
	///
	/// Read 8 bits from memory.
	/// Perform address translation if the MMU is enabled.
	///
	/// \param inAddress	virtual address to read 8 bits from.
	/// \param outByte		byte that was read.
	/// \return true if the address couldn't be accessed for reading.
	///		
	Boolean		ReadB( VAddr inAddress, KUInt8& outByte );
	
	///
	/// Read 8 bits from memory, with a direct physical address.
	///
	/// \param inAddress	physical address to read 8 bits from.
	/// \param outByte		byte that was read.
	/// \return true if the address couldn't be accessed for reading.
	///		
	Boolean		ReadBP( PAddr inAddress, KUInt8& outByte );
	
	///
	/// Write 32 bits to memory.
	/// Perform address translation if the MMU is enabled.
	///
	/// \param inAddress	virtual address to write 32 bits to.
	/// \param inWord		32 bits word to write.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		Write( VAddr inAddress, KUInt32 inWord );

	///
	/// Write 32 bits to memory, ignoring two last bits.
	/// Perform address translation if the MMU is enabled.
	///
	/// \param inAddress	virtual address to write 32 bits to.
	/// \param inWord		32 bits word to write.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		WriteAligned( VAddr inAddress, KUInt32 inWord );

	///
	/// Write 32 bits to memory, with a direct physical address.
	///
	/// \param inAddress	virtual address to write 32 bits to.
	/// \param inWord		32 bits word to write.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		WriteP( PAddr inAddress, KUInt32 inWord );

	///
	/// Write 32 bits to memory, with a direct physical aligned address.
	///
	/// \param inAddress	virtual address to write 32 bits to.
	/// \param inWord		32 bits word to write.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		WritePAligned( PAddr inAddress, KUInt32 inWord );

	///
	/// Write 8 bits to memory.
	/// Perform address translation if the MMU is enabled.
	///
	/// \param inAddress	virtual address to write 8 bits to.
	/// \param inByte		byte to write.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		WriteB( VAddr inAddress, KUInt8 inByte );

	///
	/// Write 8 bits to memory, with a direct physical address.
	///
	/// \param inAddress	virtual address to write 8 bits to.
	/// \param inByte		byte to write.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		WriteBP( PAddr inAddress, KUInt8 inByte );

	///
	/// Translate a flash address and check its validity.
	///
	/// \param inAddress	virtual address.
	/// \param outAddress   physical (translated) address. Can be nil.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		TranslateAndCheckFlashAddress( KUInt32 inAddress, PAddr* outAddress );

	///
	/// Write to flash, 32 bits (address is unmodified).
	///
	/// \param inWord		32 bits word to write.
	/// \param inMask		data mask.
	/// \param inAddress	flash address to write to.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		WriteToFlash32Bits(
					KUInt32 inWord,
					KUInt32 inMask,
					KUInt32 inAddress );

	///
	/// Write to flash, 16 bits (address is multiplied by 2).
	///
	/// \param inWord		16 bits word to write.
	/// \param inMask		data mask.
	/// \param inAddress	flash address to write to.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		WriteToFlash16Bits(
					KUInt32 inWord,
					KUInt32 inMask,
					KUInt32 inAddress );

	///
	/// Erase flash (write FF)
	///
	/// \param inAddress	flash address of the block.
	/// \param inBlockSize  number of bytes to erase (block size).
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		EraseFlash(
					KUInt32 inAddress,
					KUInt32 inBlockSize );
	
	///
	/// Perform address translation for an instruction.
	///
	/// \param inVAddress			virtual address to read 32 bits from.
	/// \param outPAddress			physical address.
	/// \return true if the address couldn't be accessed for reading.
	///		
	Boolean		TranslateInstruction(
					VAddr inVAddress,
					PAddr* outPAddress )
		{
			return mMMU.TranslateInstruction(
					inVAddress, outPAddress );
		}

	///
	/// Translate an address using MMU tables for reading.
	///
	/// \param inVAddress   virtual address.
	/// \param outPAddress  physical address.
	/// \return true if the MMU tables couldn't be accessed for
	///			reading or if the target address is not accessible.
	///
	Boolean		TranslateR( VAddr inVAddress, PAddr& outPAddress )
		{
			return mMMU.TranslateR( inVAddress, outPAddress );
		}

	///
	/// Translate an address using MMU tables for writing.
	///
	/// \param inVAddress   virtual address.
	/// \param outPAddress  physical address.
	/// \return true if the MMU tables couldn't be accessed for
	///			reading or if the target address is not accessible.
	///
	Boolean		TranslateW( VAddr inVAddress, PAddr& outPAddress )
		{
			return mMMU.TranslateW( inVAddress, outPAddress );
		}

	///
	/// Enable or disable the MMU.
	///
	/// \param inEnableMMU  whether we want to enable the MMU.
	///
	void		SetMMUEnabled( Boolean inEnableMMU )
		{
			mMMU.SetMMUEnabled( inEnableMMU );
		}

	///
	/// Get the state of the MMU.
	///
	/// \return \c true if the MMU is enabled, \c false otherwise.
	///
	Boolean		IsMMUEnabled( void )
		{
			return mMMU.IsMMUEnabled();
		}

	///
	/// Set the memory model in privilege or unprivilege mode.
	///
	/// \param inPrivilege  whether we are in a privileged mode.
	///
	void		SetPrivilege( Boolean inPrivilege )
		{
			mMMU.SetPrivilege( inPrivilege );
		}

	///
	/// Get the current mode of the memory model (privileged or unprivileged)
	///
	/// \return true if we are in a privileged mode.
	///
	Boolean		GetPrivilege( void ) const
		{
			return mMMU.GetPrivilege();
		}

	///
	/// Set the system protection.
	///
	/// \param inProtection the new protection.
	///
	void		SetSystemProtection( Boolean inProtection )
		{
			mMMU.SetSystemProtection( inProtection );
		}

	///
	/// Determine if system protection is enabled.
	///
	/// \return \c true if the protection is enabled, \c false otherwise.
	///
	Boolean		GetSystemProtection( void ) const
		{
			return mMMU.GetSystemProtection();
		}

	///
	/// Set the ROM protection.
	///
	/// \param inProtection the new protection.
	///
	void		SetROMProtection( Boolean inProtection )
		{
			mMMU.SetROMProtection( inProtection );
		}

	///
	/// Determine if ROM protection is enabled.
	///
	/// \return \c true if the protection is enabled, \c false otherwise.
	///
	Boolean		GetROMProtection( void ) const
		{
			return mMMU.GetROMProtection();
		}

	///
	/// Accessor on the translation table base.
	///
	/// \return the MMU translation table base.
	///
	KUInt32		GetTranslationTableBase( void ) const
		{
			return mMMU.GetTranslationTableBase();
		}

	///
	/// Selector on the translation table base.
	/// Bits 0-13 should be cleared.
	///
	/// \param inNewBase the new MMU translation table base.
	///
	void		SetTranslationTableBase( KUInt32 inNewBase )
		{
			mMMU.SetTranslationTableBase( inNewBase );
		}

	///
	/// Accessor on the domain access control.
	///
	/// \return the domain access control.
	///
	KUInt32		GetDomainAccessControl( void ) const
		{
			return mMMU.GetDomainAccessControl();
		}

	///
	/// Selector on the domain access control.
	///
	/// \param inNewDomainAC the new domain access control.
	///
	void		SetDomainAccessControl( KUInt32 inNewDomainAC )
		{
			mMMU.SetDomainAccessControl( inNewDomainAC );
		}

	///
	/// Get the fault status register.
	/// Lower 4 bits is the actual status while bits 4-7 is the domain.
	///
	/// \return the fault status register.
	///
	KUInt32		GetFaultStatusRegister( void ) const
		{
			return mMMU.GetFaultStatusRegister();
		}

	///
	/// Set the fault status register.
	///
	/// \param inNewValue   new value for the fault status register.
	///
	void		SetFaultStatusRegister( KUInt32 inNewValue )
		{
			mMMU.SetDomainAccessControl( inNewValue );
		}

	///
	/// Get the fault address register.
	///
	/// \return the fault address register.
	///
	KUInt32		GetFaultAddressRegister( void ) const
		{
			return mMMU.GetFaultAddressRegister();
		}

	///
	/// Set the fault address register.
	///
	/// \param inNewValue   new value for the fault address register.
	///
	void		SetFaultAddressRegister( KUInt32 inNewValue )
		{
			mMMU.SetFaultAddressRegister( inNewValue );
		}

	///
	/// A hardware (post-translation) fault error occurred.
	/// The status is set to TMemoryConsts::kFSR_ExternalNLFPage.
	///
	/// \param inAddress	the translated address that failed.
	///
	void		SetHardwareFault( KUInt32 inAddress )
		{
			mMMU.SetHardwareFault( inAddress );
		}

	///
	/// Invalidate the TLB cache.
	///
	inline void		InvalidateTLB( void )
		{
			mMMU.InvalidateTLB();
		}

	///
	/// Set the processor.
	///
	/// \param inProcessor  processor using this interface.
	///
	void		SetProcessor( TARMProcessor* inProcessor )
		{
			mProcessor = inProcessor;
		}

	///
	/// Get the original word where a Breakpoint is set.
	///
	/// \param inAddress	virtual address to read 32 bits from.
	/// \param outWord		32 bits word that was read.
	/// \return true if the address couldn't be accessed for reading.
	///		
	Boolean		ReadBreakpoint( VAddr inAddress, KUInt32& outWord );

	///
	/// Set a Breakpoint at a given address.
	///
	/// \param inAddress	virtual address to set the Breakpoint to.
	/// \return true if the address couldn't be accessed for writing.
	///		
	Boolean		SetBreakpoint( VAddr inAddress, KUInt16 inID = 0 );

	///
	/// Clear the Breakpoint at a given address.
	///
	/// \param inAddress	virtual address to clear the Breakpoint of.
	/// \return true if the address couldn't be accessed for writing.
	///		
	Boolean		ClearBreakpoint( VAddr inAddress );

	///
	/// Disable the Breakpoint at a given address.
	///
	/// \param inAddress	virtual address to disable the Breakpoint of.
	/// \return true if the address couldn't be accessed for writing.
	///		
	Boolean		DisableBreakpoint( VAddr inAddress );

	///
	/// Enable the Breakpoint at a given address.
	///
	/// \param inAddress	virtual address to enable the Breakpoint of.
	/// \return true if the address couldn't be accessed for writing.
	///		
	Boolean		EnableBreakpoint( VAddr inAddress );

	static const int kMaxWatchpoints = 32;
	
	///
	/// Create a memory watchpoint for a specific address
	///
	Boolean		AddWatchpoint( VAddr inAddress, KUInt8 inMode);
	
	///
	/// Clear a memory watchpoint for a specific address
	///
	Boolean		ClearWatchpoint( VAddr inAddress);
	
	///
	/// Return teh memory watchpoint at index i
	///
	Boolean		GetWatchpoint( int inIndex, VAddr &outAddress, KUInt8 &outMode );
	
	///
	/// Accessor on the RAM size.
	///
	KUInt32		GetRAMSize( void ) const
		{
			return mRAMSize;
		}

	///
	/// Save or restore the state to or from a stream.
	///
	void		TransferState( TStream* inStream );
		
	///
	/// Check that two addresses are very probably on the same page.
	///
	static Boolean	SamePage( KUInt32 inAddr1, KUInt32 inAddr2 )
		{
			return (inAddr1 & TMemoryConsts::kMMUSmallestPageMask)
				== (inAddr2 & TMemoryConsts::kMMUSmallestPageMask);
		}
	
	static Boolean	IsPageInROM( KUInt32 inAddress ) 
		{
			return !((inAddress < 0x00002000) || (inAddress & TMemoryConsts::kROMEndMask));
		}

	///
	/// Compute the serial number from the NewtonID.
	///
	void	ComputeSerialNumber( const KUInt32 inNewtonID[2] );
	
	void FDump(FILE *f) { mMMU.FDump(f); }
	
private:
	struct SBreakpoint {
		PAddr   fAddress;		///< (physical) address of the Breakpoint.
		KUInt32 fOriginalValue; ///< Original value of the Breakpoint.
		KUInt32	fBPValue;		///< Value for the BP instruction.
	};
	
	struct SWatchpoint {
		VAddr   fAddress;		///< (physical) address of the Breakpoint.
		KUInt8	fMode;			///< mode bit: 1 for reading, 2 for writing
	};
	
	struct SDMAChannel {
		PAddr	fBaseRegister;
		PAddr	fPointerRegister;
		KUInt32	fWordRegister;
		KUInt32	fControlRegister;
		KUInt32	fCountSizeRegister_1_1000;
		KUInt32	fCountSizeRegister_1_1400;
		KUInt32	fUnknown_1_1800;
		KUInt32	fUnknown_2_0000;
		KUInt32	fUnknown_2_0400;
		KUInt32	fUnknown_2_0800;
	};
	
	///
	/// CRC for the serial number.
	///
	static const int kSerialNumberCRC[256];

	///
	/// Read 32 bits from memory.
	/// Perform address translation if the MMU is enabled.
	/// This function only applies to aligned ROM & RAM accesses.
	///
	/// \param inAddress	virtual address to read 32 bits from.
	/// \param outWord		32 bits word that was read.
	/// \return true if the address couldn't be accessed for reading.
	///		
	Boolean		ReadROMRAM( VAddr inAddress, KUInt32& outWord );

	///
	/// Read 32 bits from memory, with a direct physical address.
	/// This function only applies to aligned ROM & RAM accesses.
	///
	/// \param inAddress	physical address to read 32 bits from.
	/// \param outFault		on return, \c true if there was a bus error
	///						(unchanged otherwise)
	/// \return the word that was read, 0 in case of error.
	///		
	KUInt32		ReadROMRAMP( PAddr inAddress, Boolean& outFault );

	///
	/// Write 32 bits to memory.
	/// Perform address translation if the MMU is enabled.
	/// This function only applies to aligned RAM accesses.
	///
	/// \param inAddress	virtual address to write 32 bits to.
	/// \param inWord		32 bits word to write.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		WriteRAM( VAddr inAddress, KUInt32 inWord );

	///
	/// Write 32 bits to memory, with a direct physical address.
	/// This function only applies to aligned RAM accesses.
	///
	/// \param inAddress	virtual address to write 32 bits to.
	/// \param inWord		32 bits word to write.
	/// \return true if the address couldn't be accessed for writing.
	///
	Boolean		WriteRAMP( PAddr inAddress, KUInt32 inWord );

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMemory( const TMemory& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMemory& operator = ( const TMemory& inCopy );

	///
	/// Init the class (ram and such).
	///
	void				Init( void );

	/// \name Variables
	TARMProcessor*		mProcessor;			///< Reference to the CPU.
	TLog*				mLog;				///< Interface for logging.
	TFlash				mFlash;				///< Flash memory.
	KUInt8*				mROMImagePtr;		///< 16 MB
	KUInt8*				mRAM;				///< RAM
	KUInt32				mRAMSize;			///< Size of the RAM.
	KUInt32				mRAMEnd;			///< Address of the last RAM byte.
	KUIntPtr			mRAMOffset;			///< Offset mRAM - kRAMStart
	TMMU				mMMU;				///< MMU.
	KUInt32				mBankCtrlRegister;  ///< Bank control register.
	TInterruptManager*	mInterruptManager;	///< Interface to the interrupt mgr.
	TDMAManager*		mDMAManager;		///< Interface to the DMA mgr.
	TSerialPortManager*	mExternalPort;		///< External serial port.
	TSerialPortManager*	mInfraredPort;		///< Infrared serial port.
	TSerialPortManager*	mBuiltInExtraPort;	///< Built-in Extra serial port.
	TSerialPortManager*	mModemPort;			///< Modem serial port.
	TPCMCIAController*	mPCMCIACtrls[kNbSockets];
											///< PCMCIA controllers.
	KUInt32				mSerialNumberIx;	///< Index to serial number.
	KUInt32				mSerialNumber[2];	///< Serial number.
	TEmulator*			mEmulator;			///< Emulator (interface to hardware).
	KUInt32				mBPCount;			///< Number of Breakpoints.
	SBreakpoint*		mBreakpoints;		///< Breakpoints.
	KUInt32				mWPCount;			///< Number of Watchpoints.
	SWatchpoint*		mWatchpoints;		///< Watchpoints.
	JITClass			mJIT;				///< JIT.
};

#endif
		// _TMEMORY_H

// ======================================================================= //
//         THE LESSER-KNOWN PROGRAMMING LANGUAGES #10: SIMPLE              //
//                                                                         //
// SIMPLE is an acronym for Sheer Idiot's Monopurpose Programming Language //
// Environment.  This language, developed at the Hanover College for       //
// Technological Misfits, was designed to make it impossible to write code //
// with errors in it.  The statements are, therefore, confined to BEGIN,   //
// END and STOP.  No matter how you arrange the statements, you can't make //
// a syntax error.  Programs written in SIMPLE do nothing useful.  Thus    //
// they achieve the results of programs written in other languages without //
// the tedious, frustrating process of testing and debugging.              //
// ======================================================================= //
