// ==============================
// Fichier:			TRandomAccessStream.h
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			11/6/2005
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
// The Original Code is TRandomAccessStream.h.
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
// $Id: TRandomAccessStream.h,v 1.1 2006/01/18 09:05:01 pguyot Exp $
// ===========

#ifndef _TRANDOMACCESSSTREAM_H
#define _TRANDOMACCESSSTREAM_H

#include <K/Defines/KDefinitions.h>
#include <K/Streams/TStream.h>

///
/// Class for a stream that can be rewound.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.1 $
///
/// \test	aucun test défini.
///
class TRandomAccessStream
	:
		public TStream
{
public:
	///
	/// Various modes for the cursor.
	/// To be used with SetCursor method.
	///
	enum ECursorMode {
		kFromStart,		///< from the start of the stream.
		kFromLEOF,		///< from the end of the stream.
		kFromCursor		///< from the current position of the cursor.
	};
	
	///
	/// Determine the position of the cursor in the stream.
	///
	/// \return the position from the start of the stream.
	/// \throws an exception if the operation isn't supported.
	///
	virtual KSInt64 GetCursor( void ) const = 0;

	///
	/// Move the cursor in the stream.
	/// Can throw an exception if the end of the stream was reached.
	///
	/// \param inPos	new position of the cursor.
	/// \param inMode	defines the origin of \c inPos
	/// \throws TPositionException if the end (or the beginning) of the stream
	///			was reached.
	/// \throws an exception if the operation isn't supported.
	///
	virtual void SetCursor( KSInt64 inPos, ECursorMode inMode ) = 0;
};

#endif
		// _TRANDOMACCESSSTREAM_H

// ========================================================================== //
// So you see Antonio, why worry about one little core dump, eh?  In reality  //
// all core dumps happen at the same instant, so the core dump you will have  //
// tomorrow, why, it already happened.  You see, it's just a little universal //
// recursive joke which threads our lives through the infinite potential of   //
// the instant.  So go to sleep, Antonio, your thread could break any moment  //
// and cast you out of the safe security of the instant into the dark void of //
// eternity, the anti-time.  So go to sleep...                                //
// ========================================================================== //
