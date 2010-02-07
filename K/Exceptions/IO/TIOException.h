// ==============================
// Fichier:			TIOException.h
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
// The Original Code is TIOException.h.
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
// $Id: TIOException.h,v 1.1 2005/06/10 23:01:05 pguyot Exp $
// ===========

#ifndef _TIOEXCEPTION_H
#define _TIOEXCEPTION_H

#include <K/Defines/KDefinitions.h>
#include <K/Exceptions/TException.h>

///
/// Class for I/O exceptions.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.1 $
///
/// \test	aucun test défini.
///
class TIOException : public TException
{
public:
	///
	/// Defautl constructor.
	///
	TIOException( void );

	///
	/// Constructor from a file name and a line number.
	///
	/// \param inFileName	name of the file where the exception was thrown.
	/// \param inLine		line number where the exception was thrown.
	///
	TIOException(
			const char* inFileName,
			KUInt32 inLine );

	///
	/// Destructor.
	///
	virtual ~TIOException( void ) throw ();

private:
	/// \name Variables

};

#endif
		// _TIOEXCEPTION_H

// ============================================================ //
// Do you guys know what you're doing, or are you just hacking? //
// ============================================================ //
