// ==============================
// Fichier:			KDefinitions.cp
// Projet:			K
// 
// CrŽŽ le:			1/1/2004
// Tabulation:		4 espaces
// 
// ***** BEGIN LICENSE BLOCK *****
// Version: MPL 1.1
//
// The contents of this file are subject to the Mozilla Public License Version
// 1.1 (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
// http://www.mozilla.org/MPL/
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the
// License.
//
// The Original Code is KDefinitions.cp.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: KDefinitions.cp,v 1.3 2004/11/23 15:11:29 paul Exp $
// ===========

#include <K/Defines/KDefinitions.h>

#ifdef KDebugOn
#undef KDebugOn
#endif
#define KDebugOn 1

#include <K/Tests/KDebug.h>

#if __MWERKS__
	#pragma export off
#endif

// Tests ˆ la compilation pour KDefinitions
// Pour le moment, il s'agit surtout de KUIntPtr.

// Prototype.
void KDefinitions_compile_time_assertions( void );

void KDefinitions_compile_time_assertions( void )
{
	// If this fails to compile saying that you have a case constant more than once
	// (or if you get a similar warning), the size of KUIntPtr probably
	// doesn't match the size of void*. You may want to check the definition of KUIntPtr
	
	// Maybe I wasn't able to figure out how your compiler declares that it generates
	// 64 bits or 32 bits binaries.
    KCOMPILE_TIME_ASSERT_SIZE_EQUAL( void*, KUIntPtr );
}

// ====================== //
// Loose bits sink chips. //
// ====================== //
