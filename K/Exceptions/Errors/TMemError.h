// ==============================
// Fichier:			TMemError.h
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			10/6/2005
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
// The Original Code is TMemError.h.
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
// $Id: TMemError.h,v 1.1 2005/06/10 23:01:05 pguyot Exp $
// ===========

#ifndef _TMEMERROR_H
#define _TMEMERROR_H

#include <K/Defines/KDefinitions.h>
#include <K/Exceptions/Errors/TError.h>

// K
#include <K/Tests/KDebug.h>

/// \name macro to throw the exception with the file and the line.
//@{
#if KDebugOn
	#define MemError					\
		TMemError( __FILE__, __LINE__ )
#else
	#define MemError					\
		TMemError()
#endif
//@}

///
/// Classe pour .
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.1 $
///
/// \test	aucun test défini.
///
class TMemError : public TError
{
public:
	///
	/// Default constructor.
	///
	TMemError( void );

	///
	/// Constructor from a file name and a line number.
	///
	/// \param inFileName	name of the file where the exception was thrown.
	/// \param inLine		line number where the exception was thrown.
	///
	TMemError(
			const char* inFileName,
			KUInt32 inLine );

	///
	/// Destructor.
	///
	virtual ~TMemError( void ) throw ();

private:
	/// \name Variables

};

#endif
		// _TMEMERROR_H

// ============================================= //
// This file will self-destruct in five minutes. //
// ============================================= //
