// ==============================
// File:			TROMImage.h
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

#ifndef _TROMIMAGE_H
#define _TROMIMAGE_H

#include <K/Defines/KDefinitions.h>

// POSIX & ANSI C
#if TARGET_OS_WIN32
	#include <time.h>
#else
	#include <sys/time.h>
#endif

// Einstein
#include "TMemoryConsts.h"
#include "JIT.h"

class TMappedFile;

///
/// Base class for the ROM Image.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TROMImage
{
public:
	///
	/// Default constructor.
	///
	TROMImage( void );

	///
	/// Destructor.
	///
	~TROMImage( void );

	///
	/// Accessor to the ROM Pointer.
	///
	/// \return a pointer to the ROM data.
	///
	KUInt8* GetPointer( void ) const
		{
			return mImage->fROM;
		}

	///
	/// Compute the checksums.
	///
	void	ComputeChecksums( KUInt32 outChecksums[10] ) const;

protected:
	///
	/// Determine if the mmap is outdated and should be redone.
	///
	/// \param inPath			path to the map.
	/// \param inModDate		modification date (to compare with the
	///							modification date of the map).
	/// \param inMachineString	identification of the machine (and of the ROM).
	///
	static Boolean IsImageOutdated(
				const char* inPath,
				time_t inModDate,
				const char inMachineString[6] );
	
	///
	/// Create the mmap file.
	///
	static void CreateImage(
						const char* inPath,
						const KUInt8* inBuffer,
						KUInt32 inBufferSize,
						const char inMachineString[6] );

	///
	/// Init from the mmap path.
	///
	/// \param inMonitorMode	if we are in monitor mode (more things are
	///							allowed then)
	///
	void Init( const char* inPath, Boolean inMonitorMode );

	///
	/// Check the modification date of a file.
	/// Update the timespec to reflect the latest modification date.
	///
	static int	GetLatestModDate(
				time_t* ioModDate,
				const char* inPath );

private:
	///
	/// Structure of the image.
	///
	struct SImage {
		KUInt8		fROM[TMemoryConsts::kHighROMEnd];
		KUInt32		fMagic;
		KUInt32		fVersion;
		char		fMachineString[6];
		char		fPadding[2];
		KUInt32		fChecksums[10];
	};

	enum {
		kMagic		= 0x424C5447,
		kVersion	= 1,
	};
	
	///
	/// Compute the checksums.
	///
	static void	DoComputeChecksums( SImage* inImage );

	///
	/// Compute the checksum for a segment.
	///
	static void	ComputeSegmentChecksums(
						const KUInt8* inPointer,
						KUInt32 inSize,
						KUInt32 outChecksums[2] );

	///
	/// Analyze the first 8 MB of a flat rom and fill the
	/// ROM Base Size, and Rex Bases and Sizes.
	///
	/// \param inBuffer		8 MB buffer
	/// \param outBaseSize	the base of the ROM
	/// \param outRexBases	the bases of the RExes
	/// \param outRexSizes	the sizes of the RExes
	/// \return the number of rexes found
	///
	static KUInt32	LookForREXes(
						const KUInt8* inBuffer,
						KUInt32* outBaseSize,
						KUInt32 outRexBases[4],
						KUInt32 outRexSizes[4] );

	///
	/// Patch the ROM (virtualization).
	///
	/// \param inImage		ROM Image to patch.
	///
	static void		PatchROM( SImage* inImage );

	///
	/// Patch the ROM by replacing single commands.
	///
	/// \param inImage		ROM Image to patch.
	///
	static void		DoPatchROMFromDatabase(SImage* inImage);
  
	///
	/// Patch the ROM (virtualization), with a given set of patches.
	///
	/// \param inImage		ROM Image to patch.
	///
	static void		DoPatchROMVirtualizationCalls(
                                              SImage* inImage,
                                              const KUInt32* inPatches,
                                              KUInt32 inCount );
  
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TROMImage( const TROMImage& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TROMImage& operator = ( const TROMImage& inCopy );
	
	TMappedFile*	mMappedFile;	///< mapped file with the rom.
	SImage*			mImage; 		///< image structure.
};


// The three lines below are tricking the compiler into letting
// us handle a whole range of function calls. This may not work
// with all compilers out there!
typedef void (*AnyFunctionPtr)();
class TAnyClass { };
typedef void (TAnyClass::*AnyMethodPtr)();


///
/// This class defines a patch location.
/// Patches can be created in any module by static declaration of TROMPatch's
/// which will then be linked into the patch database and applied to a
/// freshly loaded ROM.
/// Usage is currently limited to ROM v717006
///
class TROMPatch {
	
protected:
	static TROMPatch *first_;
	TROMPatch *next_;
	KUInt32 address_;
	KUInt32 value_;
	KUInt32 originalInstruction_;
	JITFuncPtr stub_;
	AnyFunctionPtr function_;
	AnyMethodPtr method_;
	const char *name_;
	
	static TROMPatch **patch_;
	static KUInt32 nPatch, NPatch;
	static KUInt32 addPatch(TROMPatch *patch);
	
public:
	/// Create and add a new patch
	TROMPatch(KUInt32 address, KUInt32 value);
	
	/// Create and add a new patch
	TROMPatch(KUInt32 address, KUInt32 value, const char *name);
	
	/// Create and add a call to a JIT instruction
	TROMPatch(KUInt32 address, JITFuncPtr stub, const char *name);
	
	/// Create and add a call to an Simulator function or static method
	TROMPatch(KUInt32 address, JITFuncPtr stub, const char *name, AnyFunctionPtr function);
	
	/// Create and add a call to an Simulator class method
	TROMPatch(KUInt32 address, JITFuncPtr stub, const char *name, AnyMethodPtr function);
	
	/// Destructor 
	virtual ~TROMPatch();

	/// Return the first patch in the list.
	/// @return NULL if no patches
	static TROMPatch *first() { return first_; }
	
	/// Return the next patch in the list
	/// @return NULL if no more patches in the database
	TROMPatch *next() { return next_; }
	
	/// Return the address of this patch (must be dividable by four)
	KUInt32 address() { return address_; }
	
	/// Return the 32-bit patch value for this address
	KUInt32 value() { return value_; }
	
	/// Set the original code for this address
	void originalInstruction(KUInt32 instr) { originalInstruction_ = instr; }
	
	/// Return the Simulator stub by index
	static JITFuncPtr GetSimulatorStubAt(KUInt32 index);
	
	/// Return the Simulator function by index
	static AnyFunctionPtr GetSimulatorFunctionAt(KUInt32 index);
	
	/// Return the Simulator class method by index
	static AnyMethodPtr GetSimulatorMethodAt(KUInt32 index);
	
	/// Return the original ROM instruction
	static KUInt32 GetOriginalInstructionAt(KUInt32 index);
	
	/// Return the original ROM instruction
	static const char* GetNameAt(KUInt32 index);
	
	/// Patch the ROM word
	virtual void apply(KUInt32 *ROM);
};


///
/// An Injection is different to a Patch. It will call native code, but then
/// return and execute the original code.
///
class TROMInjection : public TROMPatch {
	
public:
	/// Create and add a call to a JIT instruction as an injection
	TROMInjection(KUInt32 address, JITFuncPtr stub, const char *name)
	: TROMPatch(address, stub, name) { }
	
	/// Patch the ROM word
	virtual void apply(KUInt32 *ROM);
};


#define T_ROM_INJECTION(addr, name) \
extern JITInstructionProto(p##addr); \
TROMInjection i##addr(addr, p##addr, name); \
JITInstructionProto(p##addr)


///
/// Native Injections are functions that can be executed in native mode (using
/// the Simulator fibre system) or in JIT mode if needed.
///
class TROMSimulatorInjection : public TROMPatch {
	
public:
	/// Create and add a call to a JIT instruction as an injection
	TROMSimulatorInjection(KUInt32 address, KSInt32 (*stub)(), const char *name)
	: TROMPatch(address, (JITFuncPtr)stub, name) { }
	
	/// Patch the ROM word
	virtual void apply(KUInt32 *ROM);
};


#endif
// _TROMIMAGE_H

// =================================================================== //
// By long-standing tradition, I take this opportunity to savage other //
// designers in the thin disguise of good, clean fun.                  //
//                 -- P.J. Plauger, "Computer Language", 1988, April   //
//                    Fool's column.                                   //
// =================================================================== //
