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
#include "TMemory.h"
#include "Log/TLog.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
#define kTMMUStats	0
#define MMUDebug	0


#if kTMMUStats
static KUInt32 gNbHits = 0;				///< Nombre de hits.
static KUInt32 gNbPerm = 0;				///< Nombre d'erreurs de permission.
static KUInt32 gNbInvalidate = 0;		///< Nombre d'invalidations.
static KUInt32 gNbMiss = 0;				///< Nombre de ratŽs.
#endif

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
//  * TranslateR( KUInt32, KUInt32& )
// -------------------------------------------------------------------------- //
Boolean
TMMU::TranslateR( KUInt32 inVAddress, KUInt32& outPAddress )
{
#if MMUDebug > 1
	(void) ::fprintf( stderr, "TranslateR %.8X\n", inVAddress );
#endif

	KUInt32 theDomain_times2;
	KUInt32 theAccessPermissionMask;
	KUInt32 entryPermIndex = 0;

#if kTMMUStats
	if ((gNbHits & 0xFFF) == 0) {
		fprintf(
			stderr,
			"MMU: Hits: %i, Miss: %i, Perm: %i, Inv: %i\n",
			gNbHits,
			gNbMiss,
			gNbPerm,
			gNbInvalidate);
	}
	gNbHits++;
#endif
	
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
				theAccessPermissionMask = mCurrentAPRead;
			}
			
			if (theAccessPermissionMask & (1 << theEntry->mEntryPermIndex))
			{
				outPAddress = theEntry->mPhysicalAddress
					| (inVAddress & TMemoryConsts::kMMUSmallestPageMaskNeg);
				return false;
			}
#if kTMMUStats
			else
				gNbPerm++;
#endif
		}
#if kTMMUStats
		else
			gNbPerm++;
#endif
	} // if (theEntry)
	
#if kTMMUStats
	else
		gNbMiss++;
#endif

	// Bits 31-20 of target address are catenated with bits 31-14 of the
	// TTB.
	Boolean fault = false;
	KUInt32 tableEntry =
		mMemoryIntf->ReadROMRAMP( mTTBase | ((inVAddress >> 18) & 0xFFFFFFFC), fault );
	if (fault)
	{
		mFaultAddress = inVAddress;
		mFaultStatus = TMemoryConsts::kFSR_ExternalTrLvl1;
		return true;
	}

#if MMUDebug > 1
	(void) ::fprintf( stderr, "Table entry at %.8X is %.8X\n", mTTBase | ((inVAddress >> 18) & 0xFFFFFFFC), tableEntry );
#endif
	
	theDomain_times2 = (tableEntry & 0x000003E0) >> 4;
	theAccessPermissionMask = kAPMagic_Bits_Manager;
	
#if MMUDebug > 1
	(void) ::fprintf( stderr, "Domain is %i, access is %s\n", theDomain_times2 / 2, (mDomainAC & (1 << theDomain_times2)) ? "true" : "false" );
#endif
	
	if (mDomainAC & (1 << theDomain_times2))
	{
		if (!(mDomainAC & (1 << (theDomain_times2 + 1))))
		{
#if MMUDebug > 1
			(void) ::fprintf( stderr, "non manager access\n" );
#endif
			theAccessPermissionMask = mCurrentAPRead;

#if MMUDebug > 1
		} else {
			(void) ::fprintf( stderr, "manager access\n" );
#endif
		}

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
#if MMUDebug > 1
				(void) ::fprintf( stderr, "Coarse second level table, second entry at %.8X\n",
					(tableEntry & 0xFFFFFC00)
					| ((inVAddress & 0x000FF000) >> 10) );
#endif
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
#if MMUDebug > 1
				(void) ::fprintf( stderr, "Coarse second level table, second entry is %.8X\n",
					tableEntry );
#endif
				switch (tableEntry & 0x03)
				{
					case 0x0:
						// Fault.
#if MMUDebug
						if (mMemoryIntf->mLog) {
							mMemoryIntf->mLog->FLogLine(
								"FSR=7, Coarse entry = %.8X for %.8X",
								tableEntry,
								inVAddress );
						}
#endif
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
#if MMUDebug > 1
							(void) ::fprintf( stderr, "large page access permission error\n" );
#endif
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
#if MMUDebug > 1
							(void) ::fprintf(
										stderr,
										"small page access permission error, AP = %i, mask=%.1X\n",
										(int) entryPermIndex,
										(unsigned int) theAccessPermissionMask );
#endif
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
#if MMUDebug
						if (mMemoryIntf->mLog) {
							mMemoryIntf->mLog->FLogLine(
								"FSR=7, Fine entry = %.8X for %.8X",
								tableEntry,
								inVAddress );
						}
#endif
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

#if MMUDebug > 1
	(void) ::fprintf( stderr, "outPAddress = %.8X\n", outPAddress );
#endif
			
	return false;
}

// -------------------------------------------------------------------------- //
//  * TranslateW( KUInt32, KUInt32& )
// -------------------------------------------------------------------------- //
Boolean
TMMU::TranslateW( KUInt32 inVAddress, KUInt32& outPAddress )
{
#if MMUDebug > 1
	(void) ::fprintf( stderr, "TranslateW %.8X\n", inVAddress );
#endif

	KUInt32 theDomain_times2;
	KUInt32 theAccessPermissionMask;
	KUInt32 entryPermIndex = 0;

#if kTMMUStats
	if ((gNbHits & 0xFFF) == 0) {
		fprintf(
			stderr,
			"MMU: Hits: %i, Miss: %i, Perm: %i, Inv: %i\n",
			gNbHits,
			gNbMiss,
			gNbPerm,
			gNbInvalidate);
	}
	gNbHits++;
#endif

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
				theAccessPermissionMask = mCurrentAPWrite;
			}
			
			if (theAccessPermissionMask & (1 << theEntry->mEntryPermIndex))
			{
				outPAddress = theEntry->mPhysicalAddress
					| (inVAddress & TMemoryConsts::kMMUSmallestPageMaskNeg);
				return false;
			}
#if kTMMUStats
			else
				gNbPerm++;
#endif
		}
#if kTMMUStats
		else
			gNbPerm++;
#endif
	} // if (theEntry)

#if kTMMUStats
	else
		gNbMiss++;
#endif

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

#if MMUDebug > 1
	(void) ::fprintf( stderr, "Table entry at %.8X is %.8X\n", mTTBase | ((inVAddress >> 18) & 0xFFFFFFFC), tableEntry );
#endif
	
	theDomain_times2 = (tableEntry & 0x000003E0) >> 4;
	theAccessPermissionMask = kAPMagic_Bits_Manager;
	
#if MMUDebug > 1
	(void) ::fprintf( stderr, "Domain is %i, access is %s\n", theDomain_times2 / 2, (mDomainAC & (1 << theDomain_times2)) ? "true" : "false" );
#endif
	
	if (mDomainAC & (1 << theDomain_times2))
	{
		if (!(mDomainAC & (1 << (theDomain_times2 + 1))))
		{
#if MMUDebug > 1
			(void) ::fprintf( stderr, "non manager access\n" );
#endif
			theAccessPermissionMask = mCurrentAPWrite;

#if MMUDebug > 1
		} else {
			(void) ::fprintf( stderr, "manager access\n" );
#endif
		}

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
#if MMUDebug > 1
				(void) ::fprintf( stderr, "Coarse second level table, second entry at %.8X\n",
					(tableEntry & 0xFFFFFC00)
					| ((inVAddress & 0x000FF000) >> 10) );
#endif
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
#if MMUDebug > 1
				(void) ::fprintf( stderr, "Coarse second level table, second entry is %.8X\n",
					tableEntry );
#endif
				switch (tableEntry & 0x03)
				{
					case 0x0:
						// Fault.
#if MMUDebug
						if (mMemoryIntf->mLog) {
							mMemoryIntf->mLog->FLogLine(
								"FSR=7, Coarse entry = %.8X for %.8X",
								tableEntry,
								inVAddress );
						}
#endif
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
#if MMUDebug > 1
							(void) ::fprintf( stderr, "large page access permission error\n" );
#endif
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
#if MMUDebug > 1
							(void) ::fprintf(
										stderr,
										"small page access permission error, AP = %i, mask=%.1X\n",
										(int) entryPermIndex,
										(unsigned int) theAccessPermissionMask );
#endif
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
#if MMUDebug
						if (mMemoryIntf->mLog) {
							mMemoryIntf->mLog->FLogLine(
								"FSR=7, Fine entry = %.8X for %.8X",
								tableEntry,
								inVAddress );
						}
#endif
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

#if MMUDebug > 1
	(void) ::fprintf( stderr, "outPAddress = %.8X\n", outPAddress );
#endif
			
	return false;
}

// -------------------------------------------------------------------------- //
//  * TranslateInstruction( VAddr, KUInt32* )
// -------------------------------------------------------------------------- //
Boolean
TMMU::TranslateInstruction(
			KUInt32 inVAddress,
			KUInt32* outPAddress )
{
	register KUInt32 theAddress = inVAddress;

	// Optimization: avoid translation when reading unprotected ROM
	if (mMMUEnabled && ((theAddress < 0x00002000) || (theAddress & TMemoryConsts::kROMEndMask)))
	{
		KUInt32 theTranslatedAddress;
		if (TranslateR( theAddress, theTranslatedAddress ))
		{
			return true;
		}
		theAddress = theTranslatedAddress;
	}
	
	// Optimization: we can only execute stuff from RAM or ROM.	
	if (!(theAddress & TMemoryConsts::kROMEndMask))
	{
		*outPAddress = theAddress;
	} else if (theAddress < TMemoryConsts::kRAMStart) {
#if MMUDebug
		if (mMemoryIntf->mLog)
		{
			mMemoryIntf->mLog->FLogLine(
				"Trying to execute something not in RAM or ROM at V0x%.8X (P0x%.8X)",
				(unsigned int) inVAddress,
				(unsigned int) theAddress );
		}
#endif
		SetHardwareFault( inVAddress );
		return true;
	} else if (theAddress < mMemoryIntf->mRAMEnd) {
		*outPAddress = theAddress;
	} else {
#if MMUDebug
		if (mMemoryIntf->mLog)
		{
			mMemoryIntf->mLog->FLogLine(
				"Trying to execute something not in RAM or ROM at V0x%.8X (P0x%.8X)",
				(unsigned int) inVAddress,
				(unsigned int) theAddress );
		}
#endif
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
#if kTMMUStats
	gNbInvalidate++;
#endif

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
//  * SaveState( TStream* ) const
// -------------------------------------------------------------------------- //
void
TMMU::SaveState( TStream* inStream ) const
{
	// The various registers.
	inStream->PutInt32BE( mMMUEnabled );
	inStream->PutInt32BE( mCurrentAPMode );
	inStream->PutInt32BE( mTTBase );
	inStream->PutInt32BE( mDomainAC );
	inStream->PutInt32BE( mFaultAddress );
	inStream->PutInt32BE( mFaultStatus );
}

// -------------------------------------------------------------------------- //
//  * LoadState( TStream* )
// -------------------------------------------------------------------------- //
void
TMMU::LoadState( TStream* inStream )
{
	mMMUEnabled = inStream->GetInt32BE();
	mCurrentAPMode = inStream->GetInt32BE();
	mTTBase = inStream->GetInt32BE();
	mDomainAC = inStream->GetInt32BE();
	mFaultAddress = inStream->GetInt32BE();
	mFaultStatus = inStream->GetInt32BE();
	
	// Reset the cache.
	InvalidateTLB();
}

// ==================== //
// You might have mail. //
// ==================== //
