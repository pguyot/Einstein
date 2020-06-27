// ==============================
// File:			TInetService.h
// Project:			Einstein
//
// Copyright 2003-2013 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TINETSERVICE_H
#define _TINETSERVICE_H

#include <Protocols.h>
#include <CMService.h>
#include <CommServices.h>
#include <CommManagerInterface.h>
#include <OptionArray.h>

///
/// Class for the inet service (replacement of NIE for Einstein).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
PROTOCOL TInetService : public PMuxService
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(TInetService);

	CAPABILITIES((kCMS_CapabilityType_ServiceId 'inet'))

	///
	/// Protocol constructor.
	///
	TCMService* New( void );

	///
	/// Protocol destructor.
	///
	void 		Delete( void );

	///
	/// Start the service.
	///
	NewtonErr	Start(TOptionArray* options, ULong serviceId, TServiceInfo* serviceInfo );

	///
	/// Called back when done starting.
	///
	NewtonErr	DoneStarting(TAEvent* event, ULong size, TServiceInfo* serviceInfo);
};

#endif
		// _TINETSERVICE_H

// ========================================================================== //
// Real computer scientists don't write code.  They occasionally tinker with  //
// `programming systems', but those are so high level that they hardly count  //
// (and rarely count accurately; precision is for applications).              //
// ========================================================================== //
