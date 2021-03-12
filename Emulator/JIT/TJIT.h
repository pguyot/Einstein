// ==============================
// File:			TJIT.h
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

#ifndef _TJIT_H
#define _TJIT_H

#include <K/Defines/KDefinitions.h>

// Einstein
#include "TJITCache.h"

// C++
#include <string>

class TMemory;
class TMMU;
class TARMProcessor;

///
/// Base class for the JIT implementation.
///
template< class TImplementation, class TPage >
class TJIT
{
public:
	///
	/// Constructor from the memory object, the ROM base, the ROM size, the
	/// RAM base and the RAM size.
	///
	/// \param inMemoryIntf	interface to memory.
	/// \param inROMBase	base of ROM bank
	/// \param inROMSize	size of ROM bank
	/// \param inRAMBase	base of RAM bank
	/// \param inRAMSize	size of RAM bank
	///
	TJIT(
		TMemory* inMemoryIntf,
		 TMMU* inMMUIntf )
		:
		mCache( inMemoryIntf, inMMUIntf ) {};

	///
	/// Destructor.
	///
	virtual ~TJIT( void ) {};

	///
	/// Invalidate an instruction.
	///
	/// \param inPAddr			physical address of the instruction.
	///
	void			Invalidate(
						KUInt32 inPAddr )
		{
			mCache.InvalidatePage( inPAddr );
		}

	///
	/// Invalidate all V->P bindings.
	///
	void			InvalidateTLB( void )
		{
			mCache.InvalidateTLB();
		}

	///
	/// One or more steps with JIT.
	///
	/// \param ioObject			ARM CPU.
	/// \param inCount			number of instructions to execute.
	///
	virtual void	Step(
						TARMProcessor* ioObject,
						KUInt32 inCount ) = 0;


	///
	/// Run with JIT.
	///
	/// \param ioObject			ARM CPU.
	/// \param inSignal			pointer to a signal set to false to stop
	///							execution.
	///
	virtual void	Run(
						TARMProcessor* ioObject,
						volatile Boolean* inSignal ) = 0;

	///
	/// Accessor to a page.
	/// Return NULL if the page cannot be accessed because a translation
	/// error occurred.
	///
	TPage*			GetPage( KUInt32 inVAddr )
		{
			return mCache.GetPage( inVAddr );
		}

	///
	/// Get the offset in page.
	///
	KUInt32			GetOffsetInPage( KUInt32 inVAddr )
		{
			return mCache.GetOffsetInPage( inVAddr );
		}
		
	///
	/// Patch the ROM.
	/// This function is called to modify the ROM before it is saved on disk.
	/// It is only called when the image is created.
	///
	static void PatchROM(KUInt32* romPointer, KSInt32 inROMId) {
	    TImplementation::DoPatchROM(romPointer, inROMId);
	}

    ///	
	/// Return the ID as stored in the ROM image (for patching purposes).
	///
	static KUInt32 GetID() {
	    return TImplementation::kID;
	}

    ///	
	/// Return the version as stored in the ROM image (for patching purposes).
	///
	static KUInt32 GetVersion() {
	    return TImplementation::kVersion;
	}

private:
	/// \name Variables
	TJITCache<TPage>		mCache;			///< Cache.
};


#endif
		// _TJIT_H

// ========================================================================== //
// (defun NF (a c)                                                            //
//   (cond ((null c) () )                                                     //
//         ((atom (car c))                                                    //
//           (append (list (eval (list 'getchar (list (car c) 'a) (cadr c)))) //
//                  (nf a (cddr c))))                                         //
//         (t (append (list (implode (nf a (car c)))) (nf a (cdr c))))))      //
//                                                                            //
// (defun AD (want-job challenging boston-area)                               //
//   (cond                                                                    //
//    ((or (not (equal want-job 'yes))                                        //
//         (not (equal boston-area 'yes))                                     //
//         (lessp challenging 7)) () )                                        //
//    (t (append (nf  (get 'ad 'expr)                                         //
//           '((caaddr 1 caadr 2 car 1 car 1)                                 //
//             (car 5 cadadr 9 cadadr 8 cadadr 9 caadr 4 car 2 car 1)         //
//             (car 2 caadr 4)))                                              //
//       (list '851-5071x2661)))))                                            //
// ;;;     We are an affirmative action employer.                             //
// ========================================================================== //
