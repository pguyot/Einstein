// ==============================
// File:			TFLTerminalPanel.cp
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


#include "TFLTerminalPanel.h"


// MARK: - TFLTerminalPanel -


/**
 * Terminal is a direct connection to the NEWT/64 environment.
 */
TFLTerminalPanel::TFLTerminalPanel(int x, int y, int w, int h, const char *label)
:   Fl_Group(x, y, w, h, label)
{
}

TFLTerminalPanel::~TFLTerminalPanel()
{
}


// MARK: - TFLTerminalEditor -


/**
 * This class displays output from the Toolkit and allows users to enter commands.
 */
TFLTerminalEditor::TFLTerminalEditor(int x, int y, int w, int h, const char *label)
:   Fl_Text_Editor(x, y, w, h, label)
{
}

TFLTerminalEditor::~TFLTerminalEditor()
{
}


// MARK: - TFLTerminalBuffer -


/**
 * This class manages the text buffer for the Terminal Panel.
 */
TFLTerminalBuffer::TFLTerminalBuffer()
:   Fl_Text_Buffer()
{
}

TFLTerminalBuffer::~TFLTerminalBuffer()
{
}


// MARK: - TFLTerminalBuffer -


TFLTile::TFLTile(int x, int y, int w, int h, const char *label)
:   Fl_Tile(x, y, w, h, label)
{

}

void TFLTile::resize(int x, int y, int w, int h)
{
    // try to differentiate between the user resizing the window or programmatically resizing
    if ( 1 ) {
        Fl_Widget *monitor = child(2);
        int mh = monitor->h();
        Fl_Tile::resize(x, y, w, h);
        child(1)->resize(x, y, w, h-mh);
        child(2)->resize(x, y+h-mh, w, mh);
    } else {
        Fl_Tile::resize(x, y, w, h);
    }
}

// ============================================================================ //
// Inspector
//
// A debugging window in the Newton Toolkit development environment that allows
// a user to browse the object storage system and execute NewtonScript code on
// a Newton device.
//
//   -- Newton Glossary
// ============================================================================ //
