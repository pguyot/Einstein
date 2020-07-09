
// ==============================
// File:			TMonitor.h
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

#ifndef _TMONITOR_H
#define _TMONITOR_H

#include <K/Defines/KDefinitions.h>
#include <K/Threads/TThread.h>
#include <K/Threads/TMutex.h>
#include <K/Threads/TCondVar.h>

#include "TMonitorCore.h"
#include "Emulator/Log/TBufferLog.h"

class TEmulator;
class TMemory;
class TARMProcessor;
class TInterruptManager;
class TSymbolList;

///
/// Class for the monitor (in monitor mode).
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 113 $
///
/// \test	aucun test défini.
///
class TMonitor : public TMonitorCore
{
public:
	///
	/// Constructor from the emulator.
	///
	TMonitor(
			TBufferLog* inLog,
			TEmulator* inEmulator,
			TSymbolList* inSymbolList,
			const char* inROMPath);

	///
	/// Destructor.
	///
	virtual ~TMonitor( void );

	///
	/// Draw screen.
	/// Return true if the screen was erased.
	///
	virtual bool DrawScreen();

	///
	/// Determine if the machine is halted.
	///
	bool		IsHalted( void ) const
		{
			return mHalted;
		}

    ///
    /// Was the last screen update showing the "Halted..." screen?
    ///
    bool IsLastScreenHalted() const
    {
        return mLastScreenHalted;
    }

    ///
    /// Was the last screen update showing the "Halted..." screen?
    ///
    void SetLastScreenHalted(bool v)
    {
        mLastScreenHalted = v;
    }

	///
	/// Monitor loop.
	///
	void		Run( void );

	///
	/// Exit from monitor loop.
	///
	void		Stop( void );

	///
	/// Print help for the available commands.
	///
	void		PrintHelp( void );

	///
	/// Print help for the log commands.
	///
	void		PrintLoggingHelp( void );

	///
	/// Print help for the script commands.
	///
	void		PrintScriptingHelp( void );

	///
	/// Print help for the watchpoint commands.
	///
	void		PrintWatchpointHelp( void );

	///
	/// Output a line.
	///
	virtual void		PrintLine( const char* inLine, int type )
		{
			mLog->LogLine( inLine );
		}

#if TARGET_UI_FLTK
    // no support for signalling yet
#else
	///
	/// Get monitor socket (notified of state changes).
	///
	int			GetMonitorSocket( void ) const
		{
			return mSocketPair[0];
		}
#endif

	///
	/// Execute startup script.
	///
	/// \return true if /ROMPath/monitorrc was found and run.
	///
	bool		ExecuteStartupScript();

    ///
	/// Execute a command.
	///
	/// \return true if the command was known.
	///
	bool		ExecuteCommand( const char* inCommand );

	///
	/// Execute the help command.
	///
	/// \return true if the command was known.
	///
	bool		ExecuteHelpCommand( const char* inCommand );

	///
	/// Save the current state of the Emulator to a file.
	///
	void		SaveEmulatorState( const char *inFilename=0L );

	///
	/// Read the current emulator state from a file.
	///
	void		LoadEmulatorState( const char *inFilename=0L );

	///
	/// Save the current state of the Emulator to a file.
	///
	void		SnapEmulatorState( const char *inFilename=0L );

	///
	/// Read the current emulator state from a file.
	///
	void		RevertEmulatorState( const char *inFilename=0L );

    ///
    /// Run the emulator as soon as we run the monitor
    ///
    void RunOnStartup(bool inRun) { mRunOnStartup = inRun; }

    ///
    /// Show the Monitor window
    ///
    virtual void Show() { }

    ///
    /// Hide the Monitor window
    ///
    virtual void Hide() { }

protected:
	/// Constants.
	static const char* kEraseLine;
	static const char* const kRegisterNames[16];
	static const char* const kModesNames[32];

	enum ECommand {
		kNop,
		kRun,
		kStep,
		kExit,
		kSaveState,
		kLoadState
	};

	enum {
		kPermanentBP	= 0,
		kTemporaryBP	= 1,
		kWatch0BP		= 2,
		kWatch1BP		= 3,
		kWatch2BP		= 4,
		kWatch3BP		= 5
	};

	///
	/// Draw screen when the emulator is halted.
	///
	void		DrawScreenHalted( void );

	///
	/// Draw screen when the emulator is running.
	///
	void		DrawScreenRunning( void );

	///
	/// Output current instruction (to the log).
	///
	void		PrintCurrentInstruction( void );

	///
	/// Output instruction at a given address.
	///
	void		PrintInstruction( KUInt32 inAddr );

	///
	/// Output the contents of the current stack.
	///
	void		PrintBacktrace(KSInt32 inNWords=0);

	///
	/// Display a NS reference
	///
	void		PrintNSRef(KUInt32 inRef);

	///
	/// Accessor on interrupt manager.
	///
	inline TInterruptManager* GetInterruptManager() const {
		return mInterruptManager;
	}

    ///
    /// Accessor on processor.
    ///
    inline TARMProcessor* GetProcessor() const {
        return mProcessor;
    }

    ///
    /// Accessor on Log
    ///
    inline TBufferLog* GetLog() const {
        return mLog;
    }

	///
	/// Process a breakpoint.
	///
	/// \param inBPID		ID of the breakpoint.
	/// \param inBPAddr		address of the breakpoint.
	/// \return true if the machine should be stopped.
	///
	virtual bool		ProcessBreakpoint( KUInt16 inBPID, KUInt32 inBPAddr );

private:
	///
	/// Run the emulator (handle breakpoint if we're on a BP).
	///
	void		RunEmulator( void );

	///
	/// Step the emulator (handle breakpoint if we're on a BP).
	///
	void		StepEmulator( void );

	/// \name Platform threading primitives

	///
	/// Create the condition variable and the mutex.
	///
	void		CreateCondVarAndMutex( void );

	///
	/// Delete the condition variable and the mutex.
	///
	void		DeleteCondVarAndMutex( void );

	///
	/// Signal the condition varaible.
	///
	void		SignalCondVar( void );

	///
	/// Wait (forever) on the emulator condition varaible.
	///
	void		WaitOnCondVar( void );

	///
	/// Acquire the mutex.
	///
	void		AcquireMutex( void );

	///
	/// Release the mutex.
	///
	void		ReleaseMutex( void );

	enum {
		kTagInteger,
		kTagPointer,
		kTagImmed,
		kTagMagicPtr
	};

	///
	/// Format an NS Ref, using snprintf
	///
	int			FormatNSRef(char* buffer, size_t bufferSize, KUInt32 inRef, int ident, int maxDepth);

	///
	/// Format an NS frame.
	///
	int 		FormatNSFrame(char* buffer, size_t bufferSize, KUInt32 addr, unsigned int length, KUInt32 mapRef, int indent, int maxDepth);

	///
	/// Format an NS binary.
	///
	int 		FormatNSBinary(char* buffer, size_t bufferSize, KUInt32 addr, unsigned int length, KUInt32 classRef, int indent, int maxDepth);

	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMonitor( const TMonitor& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMonitor& operator = ( const TMonitor& inCopy );

	/// \name Variables
	TEmulator*				mEmulator;			///< Emulator.
	TARMProcessor*			mProcessor;			///< CPU.
	TInterruptManager*		mInterruptManager;	///< Interrupt manager.
	TBufferLog*				mLog;				///< Interface to the log.
	bool					mHalted;			///< If the emulator is halted.
	TCondVar				*mCondVar = nullptr;
	TMutex					*mMutex = nullptr;
	ECommand				mCommand;			///< Next command for the
												///< monitor thread.
	char*					mFilename;			///< Argument for next command.
#if TARGET_UI_FLTK
    // no signalling between monitor and log yet
#else
	int						mSocketPair[2];		///< Socket pair for monitor state changes.
#endif
	bool					mLastScreenHalted;	///< If last screen was halted.
	char*					mROMPath;			///< path to the ROM fle directory

    bool                    mRunOnStartup = false; ///< Run the emulation as soon as the monitor starts
};

#endif
		// _TMONITOR_H

// ============================ //
// All constants are variables. //
// ============================ //
