// ==============================
// Fichier:			TEOFException.h
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
// The Original Code is TEOFException.h.
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
// $Id: TEOFException.h,v 1.1 2005/06/10 23:01:05 pguyot Exp $
// ===========

#ifndef _TEOFEXCEPTION_H
#define _TEOFEXCEPTION_H

#include <K/Defines/KDefinitions.h>
#include <K/Exceptions/IO/TIOException.h>

// K
#include <K/Tests/KDebug.h>

/// \name macro to throw the exception with the file and the line.
//@{
#if KDebugOn
	#define EOFException					\
		TEOFException( __FILE__, __LINE__ )
#else
	#define EOFException					\
		TEOFException()
#endif
//@}

///
/// Class for an exception representing an unexpected EOF.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.1 $
///
/// \test	aucun test défini.
///
class TEOFException : public TIOException
{
public:
	///
	/// Default constructor.
	///
	TEOFException( void );

	///
	/// Constructor from a file name and a line number.
	///
	/// \param inFileName	name of the file where the exception was thrown.
	/// \param inLine		line number where the exception was thrown.
	///
	TEOFException(
			const char* inFileName,
			KUInt32 inLine );

	///
	/// Destructor.
	///
	virtual ~TEOFException( void ) throw ();

private:
	/// \name Variables

};

#endif
		// _TEOFEXCEPTION_H

// ====================================== //
// Ignorance is bliss.                    //
//                 -- Thomas Gray         //
//                                        //
// Fortune updates the great quotes, #42: //
//         BLISS is ignorance.            //
// ====================================== //
