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


#include "Toolkit/TFLTerminalPanel.h"
#include "Toolkit/TToolkit.h"

#include <FL/Enumerations.H>

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

static void kill_selection(Fl_Text_Editor* e) {
    if (e->buffer()->selected()) {
        e->insert_position(e->buffer()->primary_selection()->start());
        e->buffer()->remove_selection();
    }
}

/**
 * This class displays output from the Toolkit and allows users to enter commands.
 */
TFLTerminalEditor::TFLTerminalEditor(int x, int y, int w, int h, const char *label)
:   Fl_Text_Editor(x, y, w, h, label)
{
    add_key_binding(FL_Enter, FL_COMMAND, handle_command_enter_cb);
}

TFLTerminalEditor::~TFLTerminalEditor()
{
}

int TFLTerminalEditor::handle_command_enter(int key)
{
    if (!buffer()->selected()) {
        // find the start and end of the given line and send it as a NewtonScript command
        int p = insert_position();
        int pStart = buffer()->line_start(p);
        int pEnd = buffer()->line_end(p);
        // TODO: extend selection if line endings have continuation characters
        buffer()->select(pStart, pEnd);
    }
    if (buffer()->selected()) {
        // copy the selection and send it as a NewtonScript text
        char *cmd = buffer()->selection_text();
        
        //insert_position(buffer()->primary_selection()->end());
        //buffer()->unselect();

        if (cmd) {
            gToolkit->AppCmd(cmd);
            ::free(cmd);
        }
    } else {
        // find the start and end of the given line and send it as a NewtonScript command
        // if the previous line ends in a continuation character ('\'), select that line as well
    }
    /*
    kill_selection(e);
    insert("\\\n");
    show_insert_position();
    set_changed();
    if (when() & FL_WHEN_CHANGED) do_callback();
    */
    return 1;
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
