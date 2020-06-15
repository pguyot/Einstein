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


class TFLScriptPanel: public Fl_Group
{
public:
    TFLScriptPanel();
    ~TFLScriptPanel();
};

class TFLScriptEditor : public Fl_Text_Editor
{
public:
    TFLScriptEditor();
    ~TFLScriptEditor();
};

class TFLScriptBuffer : public Fl_Text_Buffer
{
public:
    TFLScriptBuffer();
    ~TFLScriptBuffer();
};


#endif
		// _T_FL_SCRIPT_PANEL_H

// ============================================================================ //
// Inspector
//
// A debugging window in the Newton Toolkit development environment that allows
// a user to browse the object storage system and execute NewtonScript code on
// a Newton device.
//
//   -- Newton Glossary
// ============================================================================ //
