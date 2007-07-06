// ==============================
// File:			TSDLScreenManager.h
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

#ifndef _TSDLSCREENMANAGER_H
#define _TSDLSCREENMANAGER_H

#include <K/Defines/KDefinitions.h>

///
/// Classe pour .
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TSDLScreenManager
{
public:
	/// \name Constantes publiques

	/// \name Constructeurs et destructeur

	///
	/// Constructeur par défaut.
	///
	TSDLScreenManager( void );

	///
	/// Destructeur.
	///
	virtual ~TSDLScreenManager( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TSDLScreenManager( const TSDLScreenManager& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TSDLScreenManager& operator = ( const TSDLScreenManager& inCopy );

	/// \name Variables

};

#endif
		// _TSDLSCREENMANAGER_H

// ======================================================================= //
//         It appears that after his death, Albert Einstein found himself  //
// working as the doorkeeper at the Pearly Gates.  One slow day, he        //
// found that he had time to chat with the new entrants.  To the first one //
// he asked, "What's your IQ?"  The new arrival replied, "190".  They      //
// discussed Einstein's theory of relativity for hours.  When the second   //
// new arrival came, Einstein once again inquired as to the newcomer's     //
// IQ.  The answer this time came "120".  To which Einstein replied, "Tell //
// me, how did the Cubs do this year?" and they proceeded to talk for half //
// an hour or so.  To the final arrival, Einstein once again posed the     //
// question, "What's your IQ?".  Upon receiving the answer "70",           //
// Einstein smiled and replied, "Got a minute to tell me about VMS 4.0?"   //
// ======================================================================= //
