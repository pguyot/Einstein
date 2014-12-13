// ==============================
// File:			THostInfo.cp
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
#include "THostInfo.h"

// MacOS X
#if TARGET_OS_OPENSTEP
#ifdef TARGET_IOS
#else
	#include <CoreFoundation/CoreFoundation.h>
	#include <CoreServices/CoreServices.h>
	#include <AddressBook/AddressBook.h>
#endif
#endif

// ANSI C & POSIX
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#if TARGET_OS_WIN32
	#include <windows.h>
	#include "CompatibilityWin32.h"
	#include <time.h>
#else
	#include <unistd.h>
	#include <sys/time.h>
	#include <pwd.h>
#endif

// K
#include <K/Defines/UByteSex.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

const THostInfo* THostInfo::kHostInfo = new THostInfo();

// -------------------------------------------------------------------------- //
//  * THostInfo( void )
// -------------------------------------------------------------------------- //
THostInfo::THostInfo( void )
	:
		mUserFirstName( nil ),
		mUserLastName( nil ),
		mUserCompany( nil ),
		mUserAddr( nil ),
		mUserAddr2( nil ),
		mUserPostalCode( nil ),
		mUserCity( nil ),
		mUserRegion( nil ),
		mUserCountry( nil ),
		mUserCountryISOCode( nil ),
		mUserHomePhone( nil ),
		mUserHomeFaxPhone( nil ),
		mUserWorkPhone( nil ),
		mUserWorkFaxPhone( nil )
{
	// Fill the first and the last name of the user.
	RetrieveUserInfo();
	
	// Fill everything that couldn't be retrieved.
	FillNullInfoWithEmptyStrings();
}

// -------------------------------------------------------------------------- //
//  * ~THostInfo( void )
// -------------------------------------------------------------------------- //
THostInfo::~THostInfo( void )
{
	if (mUserFirstName)
	{
		::free( (KUInt16*) mUserFirstName );
	}
	
	if (mUserLastName)
	{
		::free( (KUInt16*) mUserLastName );
	}

	if (mUserCompany)
	{
		::free( (KUInt16*) mUserCompany );
	}

	if (mUserAddr)
	{
		::free( (KUInt16*) mUserAddr );
	}

	if (mUserAddr2)
	{
		::free( (KUInt16*) mUserAddr2 );
	}

	if (mUserPostalCode)
	{
		::free( (KUInt16*) mUserPostalCode );
	}

	if (mUserCity)
	{
		::free( (KUInt16*) mUserCity );
	}

	if (mUserRegion)
	{
		::free( (KUInt16*) mUserRegion );
	}

	if (mUserCountry)
	{
		::free( (KUInt16*) mUserCountry );
	}

	if (mUserCountryISOCode)
	{
		::free( (KUInt16*) mUserCountryISOCode );
	}

	if (mUserHomePhone)
	{
		::free( (KUInt16*) mUserHomePhone );
	}

	if (mUserHomeFaxPhone)
	{
		::free( (KUInt16*) mUserHomeFaxPhone );
	}

	if (mUserWorkPhone)
	{
		::free( (KUInt16*) mUserWorkPhone );
	}

	if (mUserWorkFaxPhone)
	{
		::free( (KUInt16*) mUserWorkFaxPhone );
	}
}

// -------------------------------------------------------------------------- //
//  * GetUserInfo( void )
// -------------------------------------------------------------------------- //
const KUInt16*
THostInfo::GetUserInfo( EUserInfoSel inSelector ) const
{
	switch (inSelector)
	{
		case kUserInfo_FirstName:
			return mUserFirstName;
			
		case kUserInfo_LastName:
			return mUserLastName;

		case kUserInfo_Company:
			return mUserCompany;

		case kUserInfo_Addr:
			return mUserAddr;

		case kUserInfo_Addr2:
			return mUserAddr2;

		case kUserInfo_PostalCode:
			return mUserPostalCode;

		case kUserInfo_City:
			return mUserCity;

		case kUserInfo_Region:
			return mUserRegion;

		case kUserInfo_Country:
			return mUserCountry;

		case kUserInfo_CountryISOCode:
			return mUserCountryISOCode;

		case kUserInfo_HomePhone:
			return mUserHomePhone;

		case kUserInfo_HomeFaxPhone:
			return mUserHomeFaxPhone;

		case kUserInfo_WorkPhone:
			return mUserWorkPhone;

		case kUserInfo_WorkFaxPhone:
			return mUserWorkFaxPhone;
	}
	
	return nil;
}

// -------------------------------------------------------------------------- //
//  * GetHostTimeZone( void )
// -------------------------------------------------------------------------- //
int
THostInfo::GetHostTimeZone( void ) const
{
#if TARGET_OS_OPENSTEP
	// MacOS X:
	// gettimeofday won't work because the information is kept outside
	// the kernel.
	// So I'll use CFTimeZone.
	CFTimeZoneRef theTZ = CFTimeZoneCopyDefault();
	CFTimeInterval theDelta =  CFTimeZoneGetSecondsFromGMT(
		theTZ, CFAbsoluteTimeGetCurrent());
	CFRelease(theTZ);
	return (int) theDelta;
#elif TARGET_OS_WIN32
	// Timezone is returned in hours.
	long seconds = 0;
	_get_timezone(&seconds);
	return seconds*3600;
#else
	// Use gettimeofday.
	// Cygwin crashes if we give NULL for first parameter.
	struct timeval theTime;
	struct timezone theTZ;
	if (::gettimeofday(&theTime, &theTZ) == 0)
	{
		return theTZ.tz_minuteswest * 60;
	} else {
		return 0;
	}
#endif
}

// -------------------------------------------------------------------------- //
//  * RetrieveUserInfo( void )
// -------------------------------------------------------------------------- //
void
THostInfo::RetrieveUserInfo( void )
{
	do {
#if TARGET_OS_WIN32
          // do nothing
#elif TARGET_OS_ANDROID
          // do nothing
#else
#if TARGET_OS_OPENSTEP
#ifdef TARGET_IOS
#else
		// On MacOS X, I try:
		// - The AddressBook
		// - Carbon access to the full name

		ABAddressBookRef theABRef = ABGetSharedAddressBook();
		ABPersonRef theUserRef = ABGetMe( theABRef );
		if (theUserRef)
		{
			// Good. User has a card.
			// It's what we'll use for all info.
			CFStringRef theInfo =
				(CFStringRef) ABRecordCopyValue(
					theUserRef, kABFirstNameProperty );
			if (theInfo)
			{
				mUserFirstName = ConvertCFString( theInfo );
			}
			
			// Try the last name now.
			theInfo =
				(CFStringRef) ABRecordCopyValue(
					theUserRef, kABLastNameProperty );
			if (theInfo)
			{
				mUserLastName = ConvertCFString( theInfo );
			}

			theInfo =
				(CFStringRef) ABRecordCopyValue(
					theUserRef, kABOrganizationProperty );
			if (theInfo)
			{
				mUserCompany = ConvertCFString( theInfo );
			}

			ABMultiValueRef theAddresses =
				(ABMultiValueRef) ABRecordCopyValue(
					theUserRef, kABAddressProperty );
			if (theAddresses && (ABMultiValueCount(theAddresses) > 0))
			{
				// Pick the primary one, work or home, we don't care.
				CFStringRef primaryId = ABMultiValueCopyPrimaryIdentifier(
						theAddresses );
				CFIndex index = ABMultiValueIndexForIdentifier(
						theAddresses, primaryId );

				CFDictionaryRef theAddress = (CFDictionaryRef)
					ABMultiValueCopyValueAtIndex(theAddresses, index);

				theInfo = (CFStringRef) CFDictionaryGetValue(
					theAddress, kABAddressStreetKey);
				
				if (theInfo)
				{
					// If there is a new line, split the string.
					CFRange theRange = CFStringFind(
											theInfo,
											CFSTR("\n"),
											0 );
					if (theRange.length > 0)
					{
						CFIndex theLength = CFStringGetLength( theInfo );
						CFIndex theLocation = theRange.location;
						theRange.length = theLocation;
						theRange.location = 0;
						mUserAddr = ConvertCFString(
							CFStringCreateWithSubstring(
								kCFAllocatorDefault,
								theInfo,
								theRange ) );
						theRange.length = theLength - theLocation - 1;
						theRange.location = theLocation + 1;
						mUserAddr2 = ConvertCFString(
							CFStringCreateWithSubstring(
								kCFAllocatorDefault,
								theInfo,
								theRange ) );
					} else {
						mUserAddr = ConvertCFString( theInfo );
					}
				}

				theInfo = (CFStringRef) CFDictionaryGetValue(
					theAddress, kABAddressCityKey);
				
				if (theInfo)
				{
					mUserCity = ConvertCFString( theInfo );
				}

				theInfo = (CFStringRef) CFDictionaryGetValue(
					theAddress, kABAddressZIPKey);
				
				if (theInfo)
				{
					mUserPostalCode = ConvertCFString( theInfo );
				}

				theInfo = (CFStringRef) CFDictionaryGetValue(
					theAddress, kABAddressStateKey);
				
				if (theInfo)
				{
					mUserRegion = ConvertCFString( theInfo );
				}

				theInfo = (CFStringRef) CFDictionaryGetValue(
					theAddress, kABAddressCountryKey);
				
				if (theInfo)
				{
					mUserCountry = ConvertCFString( theInfo );
				}
				
				theInfo = (CFStringRef) CFDictionaryGetValue(
					theAddress, kABAddressCountryCodeKey );
				
				if (theInfo)
				{
					mUserCountryISOCode = ConvertCFString( theInfo );
				}
			}
			
			ABMultiValueRef thePhones =
				(ABMultiValueRef) ABRecordCopyValue(
					theUserRef, kABPhoneProperty );
			if (thePhones)
			{
				CFIndex nbPhones = ABMultiValueCount( thePhones );
				CFIndex indexPhones;
				for (indexPhones = 0; indexPhones < nbPhones; indexPhones++)
				{
					CFStringRef theLabel = ABMultiValueCopyLabelAtIndex(
						thePhones, indexPhones );
					CFStringRef thePhone;

					if ((mUserHomePhone == nil)
						&& (CFStringCompare(theLabel, kABPhoneHomeLabel, 0)
							== kCFCompareEqualTo))
					{
						thePhone = (CFStringRef) ABMultiValueCopyValueAtIndex(
									thePhones, indexPhones);
						if (thePhone)
						{
							mUserHomePhone = ConvertCFString( thePhone );
						}
					} else if ((mUserHomeFaxPhone == nil)
						&& (CFStringCompare(theLabel, kABPhoneHomeFAXLabel, 0)
							== kCFCompareEqualTo)) {
						thePhone = (CFStringRef) ABMultiValueCopyValueAtIndex(
									thePhones, indexPhones);
						if (thePhone)
						{
							mUserHomeFaxPhone = ConvertCFString( thePhone );
						}
					} else if ((mUserWorkPhone == nil)
						&& (CFStringCompare(theLabel, kABPhoneWorkLabel, 0)
							== kCFCompareEqualTo)) {
						thePhone = (CFStringRef) ABMultiValueCopyValueAtIndex(
									thePhones, indexPhones);
						if (thePhone)
						{
							mUserWorkPhone = ConvertCFString( thePhone );
						}
					} else if ((mUserWorkFaxPhone == nil)
						&& (CFStringCompare(theLabel, kABPhoneWorkFAXLabel, 0)
							== kCFCompareEqualTo)) {
						thePhone = (CFStringRef) ABMultiValueCopyValueAtIndex(
									thePhones, indexPhones);
						if (thePhone)
						{
							mUserWorkFaxPhone = ConvertCFString( thePhone );
						}
					}
					
					CFRelease(theLabel);
				}
				
				CFRelease(thePhones);
			}
			
			break;
		}

#endif
#endif

		char* theLogin = getlogin();
		struct passwd* theUserPasswdStruct;
		if (theLogin == NULL)
		{
			theUserPasswdStruct = getpwuid(getuid());
		} else {
			theUserPasswdStruct = getpwnam(getlogin());
		}
		// Try pwent.
		endpwent();
		const char* theName = 0;
		if (theUserPasswdStruct) {
			theName = theUserPasswdStruct->pw_gecos;
		} else {
			theName = "Isaac Newton";
		}
		int nameIndex = 0;
		size_t theLength = ::strlen( theName );
		mUserFirstName = (KUInt16*)
			::malloc( (theLength + 1) * sizeof( KUInt16 ) );
		while (true)
		{
			KUInt16 theChar = theName[nameIndex];
			((KUInt16*) mUserFirstName)[nameIndex] =
				UByteSex_ToBigEndian(theChar);

			if (theChar == '\0')
			{
				break;
			}
			
			if (theChar == ' ')
			{
				// End of first name.
				((KUInt16*) mUserFirstName)[nameIndex] = 0x0000;
				
				mUserFirstName =
					(KUInt16*) ::realloc(
									(KUInt16*) mUserFirstName,
									(nameIndex + 1) * sizeof(KUInt16) );

				// We process the last name.				
				theLength -= nameIndex;	// incl. null term.
				mUserLastName =
					(KUInt16*) ::malloc(
						theLength * sizeof( KUInt16 ) );
				
				KUInt16* lastNameCrsr = (KUInt16*) mUserLastName;
				do
				{
					theChar = theName[++nameIndex];
					*lastNameCrsr++ = UByteSex_ToBigEndian(theChar);
				} while (theChar != 0x00);
				
				break;
			}
			
			nameIndex++;
		}
#endif
	} while (false);
}

#if TARGET_OS_OPENSTEP
// -------------------------------------------------------------------------- //
//  * ConvertCFString( CFStringRef )
// -------------------------------------------------------------------------- //
const KUInt16*
THostInfo::ConvertCFString( CFStringRef inCFString )
{
	// Allocate the string.
	CFIndex theLength = CFStringGetLength( inCFString );
	KUInt16* theResult =
		(KUInt16*) ::malloc( (theLength + 1) * sizeof( KUInt16 ) );

	// Copy the characters.
	CFRange theRange;
	theRange.location = 0;
	theRange.length = theLength;

	CFStringGetCharacters( inCFString, theRange, (UniChar*) theResult );

	// Add the null terminator.
	((KUInt16*) theResult)[theLength] = 0x0000;

#if TARGET_RT_LITTLE_ENDIAN
	// On little endian, swap the characters.
	KUInt16* theCursor = theResult;
	while (theLength-- > 0)	// No need to swap the null terminator.
	{
		KUInt16 theChar = UByteSex_ToBigEndian(*theCursor);
		*theCursor = theChar;
		theCursor++;
	}
#endif

	return theResult;
}
#endif

// -------------------------------------------------------------------------- //
//  * FillNullInfoWithEmptyStrings( void )
// -------------------------------------------------------------------------- //
void
THostInfo::FillNullInfoWithEmptyStrings( void )
{
	if (!mUserFirstName)
	{
		mUserFirstName
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserFirstName)[0] = 0x0000;
	}
	
	if (!mUserLastName)
	{
		mUserLastName
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserLastName)[0] = 0x0000;
	}

	if (!mUserCompany)
	{
		mUserCompany
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserCompany)[0] = 0x0000;
	}

	if (!mUserAddr)
	{
		mUserAddr
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserAddr)[0] = 0x0000;
	}

	if (!mUserAddr2)
	{
		mUserAddr2
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserAddr2)[0] = 0x0000;
	}

	if (!mUserPostalCode)
	{
		mUserPostalCode
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserPostalCode)[0] = 0x0000;
	}

	if (!mUserCity)
	{
		mUserCity
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserCity)[0] = 0x0000;
	}

	if (!mUserRegion)
	{
		mUserRegion
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserRegion)[0] = 0x0000;
	}

	if (!mUserCountry)
	{
		mUserCountry
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserCountry)[0] = 0x0000;
	}

	if (!mUserCountryISOCode)
	{
		mUserCountryISOCode
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserCountryISOCode)[0] = 0x0000;
	}

	if (!mUserHomePhone)
	{
		mUserHomePhone
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserHomePhone)[0] = 0x0000;
	}

	if (!mUserHomeFaxPhone)
	{
		mUserHomeFaxPhone
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserHomeFaxPhone)[0] = 0x0000;
	}

	if (!mUserWorkPhone)
	{
		mUserWorkPhone
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserWorkPhone)[0] = 0x0000;
	}

	if (!mUserWorkFaxPhone)
	{
		mUserWorkFaxPhone
			= (KUInt16*) ::malloc( sizeof( KUInt16 ) );
		((KUInt16*) mUserWorkFaxPhone)[0] = 0x0000;
	}
}

// =============================================================================== //
// An interpretation _I satisfies a sentence in the table language if and only if //
// each entry in the table designates the value of the function designated by the  //
// function constant in the upper-left corner applied to the objects designated    //
// by the corresponding row and column labels.                                     //
//                 -- Genesereth & Nilsson, "Logical foundations of Artificial     //
//                    Intelligence"                                                //
// =============================================================================== //
