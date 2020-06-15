// ==============================
// File:			TFLScriptPanel.h
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

#ifndef _T_FL_SCRIPT_PANEL_H
#define _T_FL_SCRIPT_PANEL_H

#include "app/Version.h"
#include "app/TFLApp.h"

#include <FL/Fl_Group.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>


/**
 * This class provides and FLTK UI to edit NewtonScript source code.
 */
class TFLScriptPanel: public Fl_Group
{
public:
    TFLScriptPanel();
    ~TFLScriptPanel();
};


/**
 * This class displays and provides editing capabilities for NewtonScript source code.
 */
class TFLScriptEditor : public Fl_Text_Editor
{
public:
    TFLScriptEditor();
    ~TFLScriptEditor();
};


/**
 * This class manages the text buffer for NewtonScript source code.
 */
class TFLScriptBuffer : public Fl_Text_Buffer
{
public:
    TFLScriptBuffer();
    ~TFLScriptBuffer();
};


#endif
		// _T_FL_SCRIPT_PANEL_H

// ============================================================================ //
// NewtonScript
//
// A dynamic, object-oriented programming language developed for the Newton
// platform. Lovingly nicknamed WallyScript after its designer Walter Smith.
//
//   -- Newton Glossary
// ============================================================================ //
