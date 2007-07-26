// ==============================
// File:			THostInfo.h
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

#ifndef _THOSTINFO_H
#define _THOSTINFO_H

#include <K/Defines/KDefinitions.h>

#if TARGET_OS_OPENSTEP
	#include <CoreFoundation/CoreFoundation.h>
#endif

// Einstein
#include "UserInfoDefinitions.h"

///
/// Class for information on the host system.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class THostInfo
{
public:
	///
	/// Accessor on the unique instance.
	///
	static const THostInfo*	GetHostInfo( void )
		{
			return kHostInfo;
		}
	
	///
	/// Accessor on information about the user (UTF-16).
	///
	/// \return some information about the the user.
	///
	const KUInt16* GetUserInfo( EUserInfoSel inSelector ) const;

	///
	/// Accessor on the timezone.
	///
	/// \return the delta with GMT in seconds.
	///
	int				GetHostTimeZone( void ) const;
	
private:
	///
	/// Instance unique.
	///
	static const THostInfo* kHostInfo;
	
	///
	/// Default constructor.
	///
	THostInfo( void );

	///
	/// Destructor.
	///
	~THostInfo( void );

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	THostInfo( const THostInfo& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	THostInfo& operator = ( const THostInfo& inCopy );

	///
	/// Retrieve information on the user.
	///
	void RetrieveUserInfo( void );

#if TARGET_OS_OPENSTEP
	///
	/// Convert a CF string to a UTF-16 buffer.
	///
	const KUInt16* ConvertCFString( CFStringRef inCFString );
#endif

	///
	/// Fill info that couldn't be retrieved with empty strings.
	///
	void FillNullInfoWithEmptyStrings( void );

	/// \name Variables
	const KUInt16*		mUserFirstName;			///< User first name.
	const KUInt16*		mUserLastName;			///< User last name.
	const KUInt16*		mUserCompany;			///< User company.
	const KUInt16*		mUserAddr;				///< User address.
	const KUInt16*		mUserAddr2;				///< User address (line 2).
	const KUInt16*		mUserPostalCode;		///< User postal code.
	const KUInt16*		mUserCity;				///< User city.
	const KUInt16*		mUserRegion;			///< User region.
	const KUInt16*		mUserCountry;			///< User country.
	const KUInt16*		mUserCountryISOCode;	///< User country ISO code.
	const KUInt16*		mUserHomePhone;			///< User home #.
	const KUInt16*		mUserHomeFaxPhone;		///< User home fax #.
	const KUInt16*		mUserWorkPhone;			///< User work #.
	const KUInt16*		mUserWorkFaxPhone;		///< User work fax #.
};

#endif
		// _THOSTINFO_H

// ====================================================================== //
// As the trials of life continue to take their toll, remember that there //
// is always a future in Computer Maintenance.                            //
//                 -- National Lampoon, "Deteriorata"                     //
// ====================================================================== //
