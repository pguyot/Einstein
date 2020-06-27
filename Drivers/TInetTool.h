// ==============================
// File:			TInetTool.h
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

#ifndef _TINETTOOL_H
#define _TINETTOOL_H

#include "TCommTool.h"

///
/// Class for the Internet tool (replacement of NIE for Einstein).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 147 $
///
/// \test	aucun test défini.
///
class TInetTool : public TCommTool
{
public:
	/// \name Constantes publiques

	/// \name Constructeurs et destructeur

	///
	/// Constructeur par défaut.
	///
	TInetTool( void );

	///
	/// Destructeur.
	///
	virtual ~TInetTool( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TInetTool( const TInetTool& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TInetTool& operator = ( const TInetTool& inCopy );

	/// \name Variables

};

#endif
		// _TINETTOOL_H

// ====================== //
// Send some filthy mail. //
// ====================== //
