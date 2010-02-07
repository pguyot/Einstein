// ==============================
// Fichier:			TError.cp
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// CrŽŽ le:			10/6/2005
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
// The Original Code is TError.cp.
// 
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2005 the
// Initial Developer. All Rights Reserved.
// 
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
// 
// ***** END LICENSE BLOCK *****
// ===========
// $Id: TError.cp,v 1.1 2005/06/10 23:01:05 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include "TError.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TError( void )
// -------------------------------------------------------------------------- //
TError::TError( void )
{
}

// -------------------------------------------------------------------------- //
//  * TError( const char*, KUInt32 )
// -------------------------------------------------------------------------- //
TError::TError( const char* inFileName, KUInt32 inLine )
	:
		TException( inFileName, inLine )
{
}

// -------------------------------------------------------------------------- //
//  * ~TError( void ) throw ()
// -------------------------------------------------------------------------- //
TError::~TError( void ) throw ()
{
}


// ============================================================================== //
// "We invented a new protocol and called it Kermit, after Kermit the Frog,       //
// star of "The Muppet Show." [3]                                                 //
//                                                                                //
// [3]  Why?  Mostly because there was a Muppets calendar on the wall when we     //
// were trying to think of a name, and Kermit is a pleasant, unassuming sort of   //
// character.  But since we weren't sure whether it was OK to name our protocol   //
// after this popular television and movie star, we pretended that KERMIT was an  //
// acronym; unfortunately, we could never find a good set of words to go with the //
// letters, as readers of some of our early source code can attest.  Later, while //
// looking through a name book for his forthcoming baby, Bill Catchings noticed   //
// that "Kermit" was a Celtic word for "free", which is what all Kermit programs  //
// should be, and words to this effect replaced the strained acronyms in our      //
// source code (Bill's baby turned out to be a girl, so he had to name her Becky  //
// instead).  When BYTE Magazine was preparing our 1984 Kermit article for        //
// publication, they suggested we contact Henson Associates Inc. for permission   //
// to say that we did indeed name the protocol after Kermit the Frog.  Permission //
// was kindly granted, and now the real story can be told.  I resisted the        //
// temptation, however, to call the present work "Kermit the Book."               //
//                 -- Frank da Cruz, "Kermit - A File Transfer Protocol"          //
// ============================================================================== //
