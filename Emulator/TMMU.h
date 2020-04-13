// ==============================
// File:			TMMU.h
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

#ifndef _TMMU_H
#define _TMMU_H

#include <K/Defines/KDefinitions.h>

// ANSI C & POSIX
#include <limits.h>
#include <stdio.h>

#if !TARGET_OS_WIN32
	#include <sys/time.h>
#endif

// Einstein
#include "TMemoryConsts.h"
#include "THashMapCache.h"

class TStream;
class TMemory;

///
/// Class to handle MMU operations.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 150 $
///
/// \test	aucun test défini.
///
class TMMU
{
public:
	///
	/// Constructor from the memory interface.
	///
	/// \param inMemoryIntf	memory interface.
	///
	TMMU( TMemory* inMemoryIntf );

	///
	/// Destructeur.
	///
	~TMMU( void );

	///
	/// Perform address translation for an instruction.
	/// Determine in which bank of executable code the instruction is.
	///
	/// \param inAddress			virtual address to read 32 bits from.
	/// \param outBankNumber		bank index (0 for ROM or 1 for RAM)
	/// \param outOffsetInBank		offset in the bank (address - base).
	/// \return true if the address couldn't be accessed for reading.
	///		
	bool		TranslateInstruction(
					KUInt32 inVAddress,
					KUInt32* outPAddress );

	///
	/// Translate an address using MMU tables for reading.
	///
	/// \param inVAddress   virtual address.
	/// \param outPAddress  physical address.
	/// \return true if the MMU tables couldn't be accessed for
	///			reading or if the target address is not accessible.
	///
	bool		TranslateR( KUInt32 inVAddress, KUInt32& outPAddress );

	///
	/// Translate an address using MMU tables for writing.
	///
	/// \param inVAddress   virtual address.
	/// \param outPAddress  physical address.
	/// \return true if the MMU tables couldn't be accessed for
	///			reading or if the target address is not accessible.
	///
	bool		TranslateW( KUInt32 inVAddress, KUInt32& outPAddress );

	///
	/// Enable or disable the MMU.
	///
	/// \param inEnableMMU  whether we want to enable the MMU.
	///
	void		SetMMUEnabled( bool inEnableMMU )
		{
			mMMUEnabled = inEnableMMU;
			// enable this to get a rough idea how the MMU table looks
			//if (mMMUEnabled) {
			//	FILE *f = fopen("/Users/matt/dev/Einstein/mmu.txt", "wb");
			//	FDump(f);
			//	fclose(f);
			//}
		}

	///
	/// Get the state of the MMU.
	///
	/// \return \c true if the MMU is enabled, \c false otherwise.
	///
	bool		IsMMUEnabled( void ) const
		{
			return mMMUEnabled;
		}

	///
	/// Set the memory model in privilege or unprivilege mode.
	///
	/// \param inPrivilege  whether we are in a privileged mode.
	///
	void		SetPrivilege( bool inPrivilege );

	///
	/// Get the current mode of the memory model (privileged or unprivileged)
	///
	/// \return true if we are in a privileged mode.
	///
	bool		GetPrivilege( void ) const
		{
			return mCurrentAPMode & kAPMagic_Privileged;
		}

	///
	/// Set the system protection.
	///
	/// \param inProtection the new protection.
	///
	void		SetSystemProtection( bool inProtection );

	///
	/// Determine if system protection is enabled.
	///
	/// \return \c true if the protection is enabled, \c false otherwise.
	///
	bool		GetSystemProtection( void ) const
		{
			return mCurrentAPMode & kAPMagic_System;
		}

	///
	/// Set the ROM protection.
	///
	/// \param inProtection the new protection.
	///
	void		SetROMProtection( bool inProtection );

	///
	/// Determine if ROM protection is enabled.
	///
	/// \return \c true if the protection is enabled, \c false otherwise.
	///
	bool		GetROMProtection( void ) const
		{
			return mCurrentAPMode & kAPMagic_ROM;
		}

	///
	/// Accessor on the translation table base.
	///
	/// \return the MMU translation table base.
	///
	KUInt32		GetTranslationTableBase( void ) const
		{
			return mTTBase;
		}

	///
	/// Selector on the translation table base.
	/// Bits 0-13 should be cleared.
	///
	/// \param inNewBase the new MMU translation table base.
	///
	void		SetTranslationTableBase( KUInt32 inNewBase )
		{
			InvalidateTLB();
			mTTBase = inNewBase;
		}

	///
	/// Accessor on the domain access control.
	///
	/// \return the domain access control.
	///
	KUInt32		GetDomainAccessControl( void ) const
		{
			return mDomainAC;
		}

	///
	/// Selector on the domain access control.
	///
	/// \param inNewDomainAC the new domain access control.
	///
	void		SetDomainAccessControl( KUInt32 inNewDomainAC )
		{
			InvalidateTLB();
			mDomainAC = inNewDomainAC;
		}

	///
	/// Get the fault status register.
	/// Lower 4 bits is the actual status while bits 4-7 is the domain.
	///
	/// \return the fault status register.
	///
	KUInt32		GetFaultStatusRegister( void ) const
		{
			return mFaultStatus;
		}

	///
	/// Set the fault status register.
	///
	/// \param inNewValue   new value for the fault status register.
	///
	void		SetFaultStatusRegister( KUInt32 inNewValue )
		{
			mFaultStatus = inNewValue;
		}

	///
	/// Get the fault address register.
	///
	/// \return the fault address register.
	///
	KUInt32		GetFaultAddressRegister( void ) const
		{
			return mFaultAddress;
		}

	///
	/// Set the fault address register.
	///
	/// \param inNewValue   new value for the fault address register.
	///
	void		SetFaultAddressRegister( KUInt32 inNewValue )
		{
			mFaultAddress = inNewValue;
		}

	///
	/// A hardware (post-translation) fault error occurred.
	/// The status is set to TMemoryConsts::kFSR_ExternalNLFPage.
	///
	/// \param inAddress	the translated address that failed.
	///
	void		SetHardwareFault( KUInt32 inAddress )
		{
			mFaultAddress = inAddress;
			mFaultStatus = TMemoryConsts::kFSR_ExternalNLFPage;
		}

	///
	/// Invalidate the TLB cache.
	///
	void		InvalidateTLB( void );

	///
	/// Save or restore the state of the MMU.
	///
	void		TransferState( TStream* inStream );
		
	///
	/// Dump the entire MMU Lookup Table to a File
	///
	void		FDump(FILE *f);
	
private:
	///
	/// New magic.
	/// SRP		read write
	/// 000		0011 0001
	/// 001		0111 0111
	/// 010		1011 0001
	/// 011		1111 0111
	/// 100		0011 0001
	/// 101		1111 0111
	/// 110		x011 x001
	/// 111		x111 x111
	enum {
		kAPMagic_Bits_Read		= 0xECFCFDEC,	///< 0x1110 1100 1111 1100 1111 1101 1110 1100
		kAPMagic_Bits_Write		= 0xE8E8E8E8,	///< 0x1110 1000 1110 1000 1110 1000 1110 1000
		kAPMagic_Privileged		= 0x01,
		kAPMagic_ROM			= 0x02,
		kAPMagic_System			= 0x04,
		kAPMagic_Bits_Manager	= 0x0F
	};
	
	struct SEntry {
		KUInt32			mPhysicalAddress;
		KUInt32			mDomainTimes2;
		KUInt32			mEntryPermIndex;
		KUInt32			key;
		SEntry*			prev;
		SEntry*			next;
	};

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMMU( const TMMU& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMMU& operator = ( const TMMU& inCopy );

	///
	/// Invalidate the perms cache.
	///
	void		InvalidatePerms( void );

	///
	/// Add a translation to the cache.
	///
	inline void	AddToCache(
				KUInt32 inVAddr,
				KUInt32 inPAddr,
				KUInt32 inDomainT2,
				KUInt32 inEntryPermIndex );

	/// \name Variables
	TMemory*			mMemoryIntf;		///< Interface to the memory.
	bool				mMMUEnabled;		///< If the MMU is currently
											///< enabled.
	KUInt8				mCurrentAPMode;		///< 0-7, SPR
	KUInt8				mCurrentAPRead;		///< Current AP bits for read acc.
	KUInt8				mCurrentAPWrite;	///< Current AP bits for write acc.
	KUInt32				mTTBase;			///< Translation Table Base.
	KUInt32				mDomainAC;			///< Domain Access Control.
	KUInt32				mFaultAddress;		///< Address of the last fault.
	KUInt32				mFaultStatus;		///< Status.
	THashMapCache<SEntry>	mCache;			///< TLB cache.
};

#endif
		// _TMMU_H

// ============================================== //
// The Macintosh is Xerox technology at its best. //
// ============================================== //
