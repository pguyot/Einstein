// ==============================
// File:			TFLTerminalPanel.h
// Project:			Einstein
//
// Copyright 2003-2020 by Paul Guyot and Matthias Melcher.
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

#ifndef _T_FL_TERMINAL_PANEL
#define _T_FL_TERMINAL_PANEL

#include "app/Version.h"
#include "app/FLTK/TFLApp.h"

#include <FL/Fl_Group.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>


/**
 * Terminal is a direct connection to the NEWT/64 environment.
 */
class TFLTerminalPanel : public Fl_Group
{
public:
    TFLTerminalPanel(int x, int y, int w, int h, const char *label = nullptr);
    ~TFLTerminalPanel();
};


/**
 * This class displays output from the Toolkit and allows users to enter commands.
 */
class TFLTerminalEditor : public Fl_Text_Editor
{
public:
    TFLTerminalEditor(int x, int y, int w, int h, const char *label = nullptr);
    ~TFLTerminalEditor();
protected:
    int handle_command_enter(int key);
    static int handle_command_enter_cb(int key, Fl_Text_Editor* w) { return ((TFLTerminalEditor*)w)->handle_command_enter(key); }
};


/**
 * This class manages the text buffer for the Terminal Panel.
 */
class TFLTerminalBuffer : public Fl_Text_Buffer
{
public:
    TFLTerminalBuffer();
    ~TFLTerminalBuffer();
};


class TFLTile : public Fl_Tile
{
public:
    TFLTile(int x, int y, int w, int h, const char *label = nullptr);
    void resize(int x, int y, int w, int h) override;
};

#endif
		// _T_FL_TERMINAL_PANEL

// ============================================================================ //
// Inspector
//
// A debugging window in the Newton Toolkit development environment that allows
// a user to browse the object storage system and execute NewtonScript code on
// a Newton device.
//
//   -- Newton Glossary
// ============================================================================ //
