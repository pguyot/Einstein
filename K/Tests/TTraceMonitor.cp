// ==============================
// Fichier:			TTraceMonitor.cp
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
// The Original Code is TTraceMonitor.cp.
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
#include <K/Tests/TTraceMonitor.h>

// ANSI C
#include <stdlib.h>
#include <stdio.h>

// ------------------------------------------------------------------------- //
//  * TTraceMonitor( unsigned int )
// ------------------------------------------------------------------------- //
TTraceMonitor::TTraceMonitor( unsigned int inMode )
	:
		mMode( inMode ),
		mStackDepth( 0 ),
		mStackCapacity( 0 ),
		mStack( NULL )
{
}

// ------------------------------------------------------------------------- //
//  * ~TTraceMonitor( void )
// ------------------------------------------------------------------------- //
TTraceMonitor::~TTraceMonitor( void )
{
	if (mStack)
	{
		::free( mStack );
	}
}

// ------------------------------------------------------------------------- //
//  * FunctionBegin( const char*, unsigned int )
// ------------------------------------------------------------------------- //
void
TTraceMonitor::FunctionBegin(
						const char* inFileName,
						unsigned int inLineNumber )
{
	// Affichage de la ligne de dŽbut de fonction.
	if (mMode & kDisplayBegin)
	{
		DoPrintBegin( mStackDepth, inFileName, inLineNumber );
	}

	// On pousse dans la pile.
	if (mMode & kKeepStack)
	{
		if (mStack == NULL)
		{
			// CrŽation de la pile
			mStack = (SStackEntry*)
					::malloc( kStackSizeIncrement * sizeof( SStackEntry ) );
			mStackCapacity = kStackSizeIncrement;
			mStackDepth = 0;
		} else if (mStackDepth == mStackCapacity) {
			// Augmentation de sa capacitŽ			
			mStackCapacity += kStackSizeIncrement;
			mStack = (SStackEntry*)
				::realloc( mStack, mStackCapacity * sizeof( SStackEntry ) );
		}
		
		mStack[mStackDepth].fFileName = inFileName;
		mStack[mStackDepth].fLineNumber = inLineNumber;
		mStackDepth++;
	}
}

// ------------------------------------------------------------------------- //
//  * FunctionEnd( const char*, unsigned int )
// ------------------------------------------------------------------------- //
void
TTraceMonitor::FunctionEnd(
						const char* inFileName,
						unsigned int inLineNumber )
{
	// Affichage de la ligne de fin de fonction.
	if (mMode & kDisplayEnd)
	{
		DoPrintEnd( mStackDepth, inFileName, inLineNumber );
	}

	// On retire de la pile le dernier ŽlŽment (il vaut mieux que ce soit
	// celui-ci!)
	if (mMode & kKeepStack)
	{
		if ((mStack != NULL) && (mStackDepth > 0))
		{
			mStackDepth--;
			if ((mStackDepth + (2 * kStackSizeIncrement)) < mStackCapacity)
			{
				// Diminution de sa capacitŽ			
				mStackCapacity -= (2 * kStackSizeIncrement);
				mStack = (SStackEntry*)
					::realloc(
						mStack, mStackCapacity * sizeof( SStackEntry ) );
			}
		}
	}
}

// ------------------------------------------------------------------------- //
//  * PrintStack( void )
// ------------------------------------------------------------------------- //
void
TTraceMonitor::PrintStack( void )
{
	unsigned int indexStack;
	for (indexStack = 0; indexStack < mStackDepth; indexStack++)
	{
		DoPrintStack(
						mStack[indexStack].fFileName,
						mStack[indexStack].fLineNumber );
	}
}

// ------------------------------------------------------------------------- //
//  * DoPrintBegin( unsigned int, const char*, unsigned int )
// ------------------------------------------------------------------------- //
void
TTraceMonitor::DoPrintBegin(
						unsigned int inStackDepth,
						const char* inFileName,
						unsigned int inLineNumber )
{
	// Identation pour inStackDepth, 2 espaces par profondeur
	unsigned int indexIndent;
	for (indexIndent = 0; indexIndent < inStackDepth; indexIndent++)
	{
		(void) ::printf( "  " );
	}

	// Affichage du nom de fichier et de la ligne
	(void) ::printf( "%s:%i\n", inFileName, inLineNumber );
}

// ------------------------------------------------------------------------- //
//  * DoPrintEnd( unsigned int, const char*, unsigned int )
// ------------------------------------------------------------------------- //
void
TTraceMonitor::DoPrintEnd(
						unsigned int inStackDepth,
						const char* inFileName,
						unsigned int inLineNumber )
{
	// Identation pour inStackDepth, 2 espaces par profondeur
	unsigned int indexIndent;
	for (indexIndent = 0; indexIndent < inStackDepth; indexIndent++)
	{
		(void) ::printf( "  " );
	}

	// Affichage du nom de fichier et de la ligne
	(void) ::printf( "...%s:%i\n", inFileName, inLineNumber );
}

// ------------------------------------------------------------------------- //
//  * DoPrintStack( const char*, unsigned int )
// ------------------------------------------------------------------------- //
void
TTraceMonitor::DoPrintStack(
						const char* inFileName,
						unsigned int inLineNumber )
{
	// Affichage du nom de fichier et de la ligne
	(void) ::printf( "> %s:%i\n", inFileName, inLineNumber );
}

// ====================================================================== //
// Nurse Donna:    Oh, Groucho, I'm afraid I'm gonna wind up an old maid. //
// Groucho:        Well, bring her in and we'll wind her up together.     //
// Nurse Donna:    Do you believe in computer dating?                     //
// Groucho:        Only if the computers really love each other.          //
// ====================================================================== //