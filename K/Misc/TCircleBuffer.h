// ==============================
// Fichier:			TCircleBuffer.h
// Projet:			K
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			26/5/2005
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
// The Original Code is TCircleBuffer.h.
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
// $Id: TCircleBuffer.h,v 1.2 2005/05/27 01:52:08 pguyot Exp $
// ===========

#ifndef _TCIRCLEBUFFER_H
#define _TCIRCLEBUFFER_H

#include <K/Defines/KDefinitions.h>

///
/// Class for a ring buffer that can be growed if required.
/// This class is not thread safe.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.2 $
///
/// \test	aucun test défini.
///
class TCircleBuffer
{
public:
	///
	/// Constructor from an initial capacity.
	///
	TCircleBuffer( KUIntPtr inInitialCapacity = 0 );

	///
	/// Destructor.
	///
	~TCircleBuffer( void );

	///
	/// Copy data for the producer.
	///
	/// \param inBuffer		where to read data.
	/// \param inAmount		number of bytes to produce.
	///
	void		Produce( const void* inBuffer, KUIntPtr inAmount );

	///
	/// Copy data for the consumer.
	///
	/// \param outBuffer	where to write data.
	/// \param inAmount		number of bytes to consume.
	/// \return the number of bytes actually consumed.
	///
	KUIntPtr	Consume( void* outBuffer, KUIntPtr inAmount );

	///
	/// Determine if the buffer is empty.
	///
	/// \return \c true if the buffer is empty, false otherwise.
	///
	inline Boolean		IsEmpty( void ) const
		{
			return mProducerCrsr == mConsumerCrsr;
		}
	
private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TCircleBuffer( const TCircleBuffer& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TCircleBuffer& operator = ( const TCircleBuffer& inCopy );

	/// \name Variables
	KUInt8*					mBuffer;		///< Where data is hold.
	KUIntPtr				mProducerCrsr;	///< Producer cursor.
	KUIntPtr				mConsumerCrsr;	///< Consumer cursor.
	KUIntPtr				mBufferSize;	///< Size of the buffer.
};

#endif
		// _TCIRCLEBUFFER_H

// ============================================================================== //
//         One of the questions that comes up all the time is: How enthusiastic   //
// is our support for UNIX?                                                       //
//         Unix was written on our machines and for our machines many years ago.  //
// Today, much of UNIX being done is done on our machines. Ten percent of our     //
// VAXs are going for UNIX use.  UNIX is a simple language, easy to understand,   //
// easy to get started with. It's great for students, great for somewhat casual   //
// users, and it's great for interchanging programs between different machines.   //
// And so, because of its popularity in these markets, we support it.  We have    //
// good UNIX on VAX and good UNIX on PDP-11s.                                     //
//         It is our belief, however, that serious professional users will run    //
// out of things they can do with UNIX. They'll want a real system and will end   //
// up doing VMS when they get to be serious about programming.                    //
//         With UNIX, if you're looking for something, you can easily and quickly //
// check that small manual and find out that it's not there.  With VMS, no matter //
// what you look for -- it's literally a five-foot shelf of documentation -- if   //
// you look long enough it's there.  That's the difference -- the beauty of UNIX  //
// is it's simple; and the beauty of VMS is that it's all there.                  //
//                 -- Ken Olsen, president of DEC, DECWORLD Vol. 8 No. 5, 1984    //
// [It's been argued that the beauty of UNIX is the same as the beauty of Ken     //
// Olsen's brain.  Ed.]                                                           //
// ============================================================================== //
