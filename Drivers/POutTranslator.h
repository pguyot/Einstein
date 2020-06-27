// ==============================
// File:            POutTranslator.h
// Project:         Einstein
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

#ifndef _POUTTRANSLATOR_H
#define _POUTTRANSLATOR_H

#include <Newton.h>
#include <Protocols.h>
#include <Objects.h>

///
/// Classe pour .
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 117 $
///
/// \test   aucun test défini.
///
PROTOCOL POutTranslator : public TProtocol
{
public:
    static  POutTranslator* New( char* inImplementationName );  // PROTOCOL ctor
            void            Delete( void );                     // PROTOCOL dtor

    ULong                   Init( void* );
    ULong                   Idle( void );
    void                    ConsumeFrame( const RefVar&, int, int );
    void                    Flush( int );
    void                    Prompt( int );
    ULong                   Print( const char*, ... );
    ULong                   Putc( int );
    void                    EnterBreakLoop(int);
    void                    ExitBreakLoop( void );
    void                    StackTrace( void* );
    void                    ExceptionNotify( Exception* );
};

#endif
        // _POUTTRANSLATOR_H

// =========================================================================== //
// Although it is still a truism in industry that "no one was ever fired for   //
// buying IBM," Bill O'Neil, the chief technology officer at Drexel Burnham    //
// Lambert, says he knows for a fact that someone has been fired for just that //
// reason.  He knows it because he fired the guy.                              //
//         "He made a bad decision, and what it came down to was, 'Well, I     //
// bought it because I figured it was safe to buy IBM,'"  Mr. O'Neil says.     //
// "I said, 'No.  Wrong.  Game over.  Next contestant, please.'"               //
//                 -- The Wall Street Journal, December 6, 1989                //
// =========================================================================== //
