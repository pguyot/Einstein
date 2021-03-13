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

#ifndef T_FL_MONITOR_H
#define T_FL_MONITOR_H


#include <K/Defines/KDefinitions.h>
#include "Monitor/TMonitor.h"


class Fl_Window;
class Fl_Simple_Terminal;
class Fl_Button;
class Fl_Input;


class TFLMonitor : public TMonitor
{
public:
    TFLMonitor(TBufferLog* inLog,
               TEmulator* inEmulator,
               TSymbolList* inSymbolList,
               const char *inROMPath );

    ~TFLMonitor() override;

    ///
    /// Draw screen.
    /// Return true if the screen was erased.
    ///
    bool DrawScreen() override;

    ///
    /// Show the Monitor window
    ///
    void Show() override;

    ///
    /// Hide the Monitor window
    ///
    void Hide() override;

    ///
    /// Output a line.
    ///
    void PrintLine( const char* inLine, int type ) override;

private:
    ///
    /// Draw screen when the emulator is halted.
    ///
    void        DrawScreenHalted( void );

    ///
    /// Draw screen when the emulator is running.
    ///
    void        DrawScreenRunning( void );

    Fl_Window *mwWindow = nullptr;
    Fl_Simple_Terminal *mwTerminal = nullptr;
    Fl_Button *mwPause = nullptr;
    Fl_Button *mwRun = nullptr;
    Fl_Button *mwStepOver = nullptr;
    Fl_Button *mwStep = nullptr;
    Fl_Button *mwLeave = nullptr;
    Fl_Input *mwInput = nullptr;
    Fl_Button *mwHelp = nullptr;
};


#endif // T_FL_MONITOR_H

