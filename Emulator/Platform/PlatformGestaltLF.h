// ==============================
// File:			PlatformGestalt.h
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

#ifndef _PLATFORMGESTALT_H
#define _PLATFORMGESTALT_H

#if TARGET_OS_NEWTON
	#include <Newton.h>
	#include <AEvents.h>
	typedef ULong	KUInt32;
	typedef UChar	KUInt8;
#else
	#include <K/Defines/KDefinitions.h>
#endif


#define kGestalt_Einstein_Base				0x03000001
#define kGestalt_Einstein_EmulatorInfo		(kGestalt_Einstein_Base + 1)

// Version
#define	kDR1Version	0x00010000
#define	kDR2Version	0x00010001
#define	kUP1Version	0x00010002
#define	kUP2Version	0x00010003

// Host CPU.

struct SGestaltEinsteinEmulatorInfo
{
	KUInt32	fVersion;					///< Current version.
};

// constant kGestaltArg_EinsteinEmulatorInfo := '[0x03000002, [struct,long], 1];
// constant kDR1Version := 0x00010000;

#endif
		// _PLATFORMGESTALT_H

// =============================================================== //
// Science is to computer science as hydrodynamics is to plumbing. //
// =============================================================== //
