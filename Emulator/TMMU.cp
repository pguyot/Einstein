// ==============================
// File:			TMMU.cp
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

#include <K/Defines/KDefinitions.h>
#include "TMMU.h"

// K
#include <K/Streams/TStream.h>

// Einstein
#include "Emulator/TMemory.h"
#include "Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

constexpr KUInt32 TMemoryConsts::MMUPageSizes[4];
constexpr KUInt32 TMemoryConsts::MMUPageMasks[4];
constexpr KUInt32 TMemoryConsts::MMUPageMasksNeg[4];

// -------------------------------------------------------------------------- //
//  * TMMU( TMemory* )
// -------------------------------------------------------------------------- //
TMMU::TMMU( TMemory* inMemoryIntf )
	:
		mMemoryIntf( inMemoryIntf ),
		mMMUEnabled( false ),
		mCurrentAPMode( kAPMagic_Privileged ),
		mTTBase( 0 ),
		mDomainAC( 0xFFFFFFFF )
{
	// Init the cache entries with unprobable values.
	SEntry* theEntries = mCache.GetValues();
	KUInt32 indexEntry;
	for (indexEntry = 0; indexEntry < THashMapCache<SEntry>::kCacheSize; indexEntry++) {
		SEntry* theEntry = &theEntries[indexEntry];
		theEntry->key = 1;
		theEntry->mPhysicalAddress = 1;
	}
}

// -------------------------------------------------------------------------- //
//  * ~TMMU( void )
// -------------------------------------------------------------------------- //
TMMU::~TMMU( void )
{
}

// -------------------------------------------------------------------------- //
//  * SetPrivilege( Boolean )
// -------------------------------------------------------------------------- //
void
TMMU::SetPrivilege( Boolean inPrivilege )
{
	if (inPrivilege)
	{
		mCurrentAPMode |= kAPMagic_Privileged;
	} else {
		mCurrentAPMode &= ~kAPMagic_Privileged;
	}
	mCurrentAPRead = (kAPMagic_Bits_Read >> (4 * mCurrentAPMode)) & 0xF;
	mCurrentAPWrite = (kAPMagic_Bits_Write >> (4 * mCurrentAPMode)) & 0xF;
	InvalidatePerms();
}

// -------------------------------------------------------------------------- //
//  * SetSystemProtection( Boolean )
// -------------------------------------------------------------------------- //
void
TMMU::SetSystemProtection( Boolean inProtection )
{
	if (inProtection)
	{
		mCurrentAPMode |= kAPMagic_System;
	} else {
		mCurrentAPMode &= ~kAPMagic_System;
	}
	mCurrentAPRead = (kAPMagic_Bits_Read >> (4 * mCurrentAPMode)) & 0xF;
	mCurrentAPWrite = (kAPMagic_Bits_Write >> (4 * mCurrentAPMode)) & 0xF;
	InvalidatePerms();
}

// -------------------------------------------------------------------------- //
//  * SetROMProtection( Boolean )
// -------------------------------------------------------------------------- //
void
TMMU::SetROMProtection( Boolean inProtection )
{
	if (inProtection)
	{
		mCurrentAPMode |= kAPMagic_ROM;
	} else {
		mCurrentAPMode &= ~kAPMagic_ROM;
	}
	mCurrentAPRead = (kAPMagic_Bits_Read >> (4 * mCurrentAPMode)) & 0xF;
	mCurrentAPWrite = (kAPMagic_Bits_Write >> (4 * mCurrentAPMode)) & 0xF;
	InvalidatePerms();
}

// -------------------------------------------------------------------------- //
//  * TranslateR(KUInt32, KUInt32&)
// -------------------------------------------------------------------------- //
bool
TMMU::TranslateR(KUInt32 inVAddress, KUInt32& outPAddress)
{
	// Optimization: avoid translation when reading unprotected ROM
	if (inVAddress < TMemoryConsts::kROMEnd
		&& (!mMMUEnabled || inVAddress >= TMemoryConsts::kProtectedROMEnd)) {
		outPAddress = inVAddress;
		return false;
	}

	return Translate(inVAddress, outPAddress, mCurrentAPRead);
}

// -------------------------------------------------------------------------- //
//  * TranslateW(KUInt32, KUInt32&)
// -------------------------------------------------------------------------- //
bool
TMMU::TranslateW(KUInt32 inVAddress, KUInt32& outPAddress)
{
	return Translate(inVAddress, outPAddress, mCurrentAPWrite);
}

// -------------------------------------------------------------------------- //
//  * Translate(KUInt32, KUInt32&)
// -------------------------------------------------------------------------- //
bool
TMMU::Translate(KUInt32 inVAddress, KUInt32& outPAddress, KUInt8 inCurrentAP)
{
	KUInt32 theDomain_times2;
	KUInt32 theAccessPermissionMask;
	KUInt32 entryPermIndex = 0;

	// Lookup in the cache.
	KUInt32 pageAddress = inVAddress & TMemoryConsts::kMMUSmallestPageMask;
	SEntry* theEntry = mCache.Lookup(pageAddress);
	if (theEntry)
	{
		// Check the permissions.
		// If they are incorrect, fall thru to properly handle the error.
		theAccessPermissionMask = kAPMagic_Bits_Manager;
		theDomain_times2 = theEntry->mDomainTimes2;
		
		if (mDomainAC & (1 << theDomain_times2))
		{
			if (!(mDomainAC & (1 << (theDomain_times2 + 1))))
			{
				theAccessPermissionMask = inCurrentAP;
			}
			
			if (theAccessPermissionMask & (1 << theEntry->mEntryPermIndex))
			{
				outPAddress = theEntry->mPhysicalAddress
					| (inVAddress & TMemoryConsts::kMMUSmallestPageMaskNeg);
				return false;
			}
		}
	} // if (theEntry)

	// Bits 31-20 of target address are catenated with bits 31-14 of the
	// TTB.
	Boolean fault = false;
	KUInt32 tableEntry = mMemoryIntf->ReadROMRAMP( mTTBase | ((inVAddress >> 18) & 0xFFFFFFFC), fault );
	if (fault)
	{
		mFaultAddress = inVAddress;
		mFaultStatus = TMemoryConsts::kFSR_ExternalTrLvl1;
		return true;
	}

	theDomain_times2 = (tableEntry & 0x000003E0) >> 4;
	theAccessPermissionMask = kAPMagic_Bits_Manager;
	
	if (mDomainAC & (1 << theDomain_times2))
	{
		if (!(mDomainAC & (1 << (theDomain_times2 + 1))))
		{
			// non manager access
			theAccessPermissionMask = inCurrentAP;
		} // else manager access

		int subpageIndexShift;
		
		// What kind of lookup is it?
		switch (tableEntry & 0x3)
		{
			case 0x0:
				// Unmapped.
				mFaultAddress = inVAddress;
				mFaultStatus =
					TMemoryConsts::kFSR_TranslationSection
					| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
				return true;
			
			case 0x1:
				// 01
				// Coarse second level table.
				tableEntry = mMemoryIntf->ReadROMRAMP(
					(tableEntry & 0xFFFFFC00)
					| ((inVAddress & 0x000FF000) >> 10), fault );
				if (fault)
				{
					mFaultAddress = inVAddress;
					mFaultStatus =
						TMemoryConsts::kFSR_ExternalTrLvl2
						| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
					return true;
				}
				switch (tableEntry & 0x03)
				{
					case 0x0:
						// Fault.
						mFaultAddress = inVAddress;
						mFaultStatus =
							TMemoryConsts::kFSR_TranslationPage
							| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
						return true;
					
					case 0x01:
						// Large page.
						// shift: index * 2 + 4
						// (always > kAPMagic_APShift)
						subpageIndexShift = ((inVAddress & 0x0000C000) >> 13) + 4;
						entryPermIndex = 
							(tableEntry & (0x00000003 << subpageIndexShift))
								>> subpageIndexShift;
						if (!(theAccessPermissionMask & (1 << entryPermIndex)))
						{
							mFaultAddress = inVAddress;
							mFaultStatus =
								TMemoryConsts::kFSR_PermissionPage
								| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
							return true;
						}
						outPAddress = (tableEntry & TMemoryConsts::kMMULargePageMask)
									| (inVAddress & TMemoryConsts::kMMULargePageMaskNeg);
						break;
					
					case 0x02:
						// Small page.
						// shift: index * 2 + 4
						// (always > kAPMagic_APShift)
						subpageIndexShift = ((inVAddress & 0x00000C00) >> 9) + 4;
						entryPermIndex = 
							(tableEntry & (0x00000003 << subpageIndexShift)) >> subpageIndexShift;
						if (!(theAccessPermissionMask & (1 << entryPermIndex)))
						{
							mFaultAddress = inVAddress;
							mFaultStatus =
								TMemoryConsts::kFSR_PermissionPage
								| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
							return true;
						}
						outPAddress = (tableEntry & TMemoryConsts::kMMUSmallPageMask)
									| (inVAddress & TMemoryConsts::kMMUSmallPageMaskNeg);
						break;
					
					case 0x03:
						// Tiny page.
						// They must not appear in coarse table.
						// If they do, the result is unpredictable.
						return true;
				}
				break;
			
			case 0x2:
				// 10
				// Section descriptor.
				// Check permissions.
				entryPermIndex = (tableEntry & 0x00000C00) >> 10;

				if (!(theAccessPermissionMask & (1 << entryPermIndex)))
				{
					mFaultAddress = inVAddress;
					mFaultStatus =
						TMemoryConsts::kFSR_PermissionSection
						| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
					return true;
				}
				outPAddress = (tableEntry & TMemoryConsts::kMMUSectionMask)
					| (inVAddress & TMemoryConsts::kMMUSectionMaskNeg);
				break;
			
			case 0x3:
				// 11
				// Fine second level table.
				tableEntry = mMemoryIntf->ReadROMRAMP(
					(tableEntry & 0xFFFFF000)
					| ((inVAddress & 0x000FFC00) >> 10), fault );
				if (fault)
				{
					mFaultAddress = inVAddress;
					mFaultStatus =
						TMemoryConsts::kFSR_ExternalTrLvl2
						| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
					return true;
				}
				switch (tableEntry & 0x03)
				{
					case 0x0:
						// Fault.
						mFaultAddress = inVAddress;
						mFaultStatus =
							TMemoryConsts::kFSR_TranslationPage
							| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
						return true;
					
					case 0x01:
						// Large page.
						// shift: index * 2 + 4
						// (always > kAPMagic_APShift)
						subpageIndexShift = ((inVAddress & 0x0000C000) >> 11) + 4;
						entryPermIndex = 
							(tableEntry & (0x00000003 << subpageIndexShift))
								>> subpageIndexShift;

						if (!(theAccessPermissionMask & (1 << entryPermIndex)))
						{
							mFaultAddress = inVAddress;
							mFaultStatus =
								TMemoryConsts::kFSR_PermissionPage
								| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
							return true;
						}
						outPAddress = (tableEntry & TMemoryConsts::kMMULargePageMask)
									| (inVAddress & TMemoryConsts::kMMULargePageMaskNeg);
						break;
					
					case 0x02:
						// Small page.
						// shift: index * 2 + 4
						// (always > kAPMagic_APShift)
						subpageIndexShift = ((inVAddress & 0x00000C00) >> 9) + 4;
						entryPermIndex = 
							(tableEntry & (0x00000003 << subpageIndexShift))
								>> subpageIndexShift;
						if (!(theAccessPermissionMask & (1 << entryPermIndex)))
						{
							mFaultAddress = inVAddress;
							mFaultStatus =
								TMemoryConsts::kFSR_PermissionPage
								| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
							return true;
						}
						outPAddress = (tableEntry & TMemoryConsts::kMMUSmallPageMask)
									| (inVAddress & TMemoryConsts::kMMUSmallPageMaskNeg);
						break;
					
					case 0x03:
						// Tiny page.
						entryPermIndex = 
							(tableEntry & 0x00000030) >> 4;
						if (!(theAccessPermissionMask & (1 << entryPermIndex)))
						{
							mFaultAddress = inVAddress;
							mFaultStatus =
								TMemoryConsts::kFSR_PermissionPage
								| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
							return true;
						}
						outPAddress = (tableEntry & TMemoryConsts::kMMUTinyPageMask)
									| (inVAddress & TMemoryConsts::kMMUTinyPageMaskNeg);
						break;
				}
				break;
		}
	} else {
		switch (tableEntry & 0x3)
		{
			case 0x0:
				mFaultAddress = inVAddress;
				mFaultStatus =
					TMemoryConsts::kFSR_TranslationSection
					| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
				return true;

			case 0x2:
				mFaultAddress = inVAddress;
				mFaultStatus =
					TMemoryConsts::kFSR_DomainSection
					| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
				return true;

			case 0x1:
			case 0x3:
				mFaultAddress = inVAddress;
				mFaultStatus =
					TMemoryConsts::kFSR_DomainPage
					| (theDomain_times2 << TMemoryConsts::kFSR_DomainShiftMin1);
				return true;
		}
	}

	// Add the value to the cache.
	AddToCache(
		pageAddress,
		outPAddress & TMemoryConsts::kMMUSmallestPageMask,
		theDomain_times2,
		entryPermIndex );

	return false;
}

// -------------------------------------------------------------------------- //
//  * TranslateInstruction( VAddr, KUInt32* )
// -------------------------------------------------------------------------- //
bool
TMMU::TranslateInstruction(
			KUInt32 inVAddress,
			KUInt32* outPAddress )
{
	KUInt32 theAddress = inVAddress;

	// Optimization: avoid translation when reading unprotected ROM
	if (theAddress < TMemoryConsts::kROMEnd
		&& (!mMMUEnabled || theAddress >= TMemoryConsts::kProtectedROMEnd)) {
		*outPAddress = theAddress;
		return false;
	}

	// We can only execute stuff from RAM or ROM.
	if (mMMUEnabled)
	{
		KUInt32 theTranslatedAddress = 0;
		if (TranslateR( theAddress, theTranslatedAddress ))
		{
			return true;
		}
		theAddress = theTranslatedAddress;
	}
	
	if (!(theAddress & TMemoryConsts::kROMEndMask))
	{
		*outPAddress = theAddress;
	} else if (theAddress < TMemoryConsts::kRAMStart) {
		SetHardwareFault( inVAddress );
		return true;
	} else if (theAddress < mMemoryIntf->mRAMEnd) {
		*outPAddress = theAddress;
	} else {
		SetHardwareFault( inVAddress );
		return true;
	}
	
	return false;
}

// -------------------------------------------------------------------------- //
//  * AddToCache( KUInt32, KUInt32, KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TMMU::AddToCache(
			KUInt32 inVAddr,
			KUInt32 inPAddr,
			KUInt32 inDomainT2,
			KUInt32 inEntryPermIndex )
{
	// Take last page.
	SEntry* theEntry = mCache.GetLastValue();
	
	// Remove it from the table.
	mCache.Erase( theEntry->key );
	
	// Modify the entry.
	theEntry->key = inVAddr;
	theEntry->mPhysicalAddress = inPAddr;
	theEntry->mDomainTimes2 = inDomainT2;
	theEntry->mEntryPermIndex = inEntryPermIndex;
	
	// Add it into the table.
	mCache.Insert( inVAddr, theEntry );
	
	// Finally touch the entry.
	mCache.MakeFirst( theEntry );
}

// -------------------------------------------------------------------------- //
//  * InvalidateTLB( void )
// -------------------------------------------------------------------------- //
void
TMMU::InvalidateTLB( void )
{
	mCache.Clear();
	mMemoryIntf->GetJITObject()->InvalidateTLB();
}

// -------------------------------------------------------------------------- //
//  * InvalidatePerms( void )
// -------------------------------------------------------------------------- //
void
TMMU::InvalidatePerms( void )
{
	mMemoryIntf->GetJITObject()->InvalidateTLB();
}


// -------------------------------------------------------------------------- //
//  * TransferState( TStream* )
// -------------------------------------------------------------------------- //
void
TMMU::TransferState( TStream* inStream )
{
	// Do not load or save any cached data.
	InvalidateTLB();
	InvalidatePerms();

	// The various registers.
	inStream->TransferBoolean( mMMUEnabled );
	inStream->TransferByte( mCurrentAPMode );
	inStream->TransferByte( mCurrentAPRead );
	inStream->TransferByte( mCurrentAPWrite );
	inStream->TransferInt32BE( mTTBase );
	inStream->TransferInt32BE( mDomainAC );
	inStream->TransferInt32BE( mFaultAddress );
	inStream->TransferInt32BE( mFaultStatus );
}

// ==================== //
// You might have mail. //
// ==================== //
