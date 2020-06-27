// ==============================
// File:			PEinsteinOutTranslator.impl.h
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

#ifndef _PEINSTEINOUTTRANSLATOR_IMPL_H
#define _PEINSTEINOUTTRANSLATOR_IMPL_H

#include "POutTranslator.h"

///
/// Class for the debugger NS out translator.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 117 $
///
/// \test	aucun test défini.
///
PROTOCOL PEinsteinOutTranslator : public POutTranslator
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(PEinsteinOutTranslator);

	CAPABILITIES( ("copyright" "Paul Guyot") ("version" "1.0") )

	POutTranslator*	New( void );
	void			Delete( void );

	ULong			Init( void* );
	ULong			Idle( void );
	void			ConsumeFrame( const RefVar&, int, int );
	void			Flush( int );
	void			Prompt( int );
	ULong			Print( const char*, ... );
	ULong			Putc( int );
	void			EnterBreakLoop(int);
	void			ExitBreakLoop( void );
	void			StackTrace( void* );
	void			ExceptionNotify( Exception* );

private:
	void			DoPrint( const char* inString );
};

#endif
		// _PEINSTEINOUTTRANSLATOR_IMPL_H

// ============================================================================== //
// I have sacrificed time, health, and fortune, in the desire to complete these   //
// Calculating Engines.  I have also declined several offers of great personal    //
// advantage to myself.  But, notwithstanding the sacrifice of these advantages   //
// for the purpose of maturing an engine of almost intellectual power, and        //
// after expending from my own private fortune a larger sum than the government   //
// of England has spent on that machine, the execution of which it only           //
// commenced, I have received neither an acknowledgement of my labors, not even   //
// the offer of those honors or rewards which are allowed to fall within the      //
// reach of men who devote themselves to purely scientific investigations...      //
//         If the work upon which I have bestowed so much time and thought were   //
// a mere triumph over mechanical difficulties, or simply curious, or if the      //
// execution of such engines were of doubtful practicability or utility, some     //
// justification might be found for the course which has been taken; but I        //
// venture to assert that no mathematician who has a reputation to lose will      //
// ever publicly express an opinion that such a machine would be useless if       //
// made, and that no man distinguished as a civil engineer will venture to        //
// declare the construction of such machinery impracticable...                    //
//         And at a period when the progress of physical science is obstructed    //
// by that exhausting intellectual and manual labor, indispensable for its        //
// advancement, which it is the object of the Analytical Engine to relieve, I     //
// think the application of machinery in aid of the most complicated and abtruse  //
// calculations can no longer be deemed unworthy of the attention of the country. //
// In fact, there is no reason why mental as well as bodily labor should not      //
// be economized by the aid of machinery.                                         //
//                 -- Charles Babbage, "The Life of a Philosopher"                //
// ============================================================================== //
