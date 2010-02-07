// ==============================
// Fichier:			TError.h
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
// The Original Code is TError.h.
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
// $Id: TError.h,v 1.1 2005/06/10 23:01:05 pguyot Exp $
// ===========

#ifndef _TERROR_H
#define _TERROR_H

#include <K/Defines/KDefinitions.h>
#include <K/Exceptions/TException.h>

///
/// Class for errors.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.1 $
///
/// \test	aucun test défini.
///
class TError : public TException
{
public:
	///
	/// Default constructor.
	///
	TError( void );

	///
	/// Constructor from a file name and a line number.
	///
	/// \param inFileName	name of the file where the exception was thrown.
	/// \param inLine		line number where the exception was thrown.
	///
	TError(
			const char* inFileName,
			KUInt32 inLine );

	///
	/// Destructeur.
	///
	virtual ~TError( void ) throw ();

private:
	/// \name Variables

};

#endif
		// _TERROR_H

// =========================================================================== //
// Some programming languages manage to absorb change, but withstand progress. //
//                 -- Epigrams in Programming, ACM SIGPLAN Sept. 1982          //
// =========================================================================== //
