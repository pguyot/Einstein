// ==============================
// Fichier:			TMonitorMethodClient.cp
// Projet:			K
//
// CrŽŽ le:			05/09/2002
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
// The Original Code is TMonitorMethodClient.cp.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2002-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Tests/TMonitorMethodClient.h>

// K Libs
#include <K/Tests/TFunctionMonitor.h>

// ------------------------------------------------------------------------- //
//  * TMonitorMethodClient( void )
// ------------------------------------------------------------------------- //
TMonitorMethodClient::TMonitorMethodClient(
							TFunctionMonitor* inMonitor,
							const char* inFileName,
							unsigned int inLineNumber )
	:
		mMonitor( inMonitor ),
		mFileName( inFileName ),
		mLineNumber( inLineNumber )
{
	inMonitor->FunctionBegin( inFileName, inLineNumber );
}

// ------------------------------------------------------------------------- //
//  * ~TMonitorMethodClient( void )
// ------------------------------------------------------------------------- //
TMonitorMethodClient::~TMonitorMethodClient( void )
{
	mMonitor->FunctionEnd( mFileName, mLineNumber );
}

// =============================================================================== //
// ===  ALL USERS PLEASE NOTE  ========================                            //
//                                                                                 //
// The garbage collector now works.  In addition a new, experimental garbage       //
// collection algorithm has been installed.  With SI:%DSK-GC-QLX-BITS set to 17,   //
// (NOT the default) the old garbage collection algorithm remains in force; when   //
// virtual storage is filled, the machine cold boots itself.  With SI:%DSK-GC-     //
// QLX-BITS set to 23, the new garbage collector is enabled.  Unlike most garbage  //
// collectors, the new gc starts its mark phase from the mind of the user, rather  //
// than from the obarray.  This allows the garbage collection of significantly     //
// more Qs.  As the garbage collector runs, it may ask you something like "Do you  //
// remember what SI:RDTBL-TRANS does?", and if you can't give a reasonable answer  //
// in thirty seconds, the symbol becomes a candidate for GCing.  The variable      //
// SI:%GC-QLX-LUSER-TM governs how long the GC waits before timing out the user.   //
// =============================================================================== //