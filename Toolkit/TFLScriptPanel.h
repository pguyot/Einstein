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

class TFLScriptPanel;
class TFLScriptEditor;
class TFLScriptBuffer;

class TTkScript;


/**
 * This class provides and FLTK UI to edit NewtonScript source code.
 */
class TFLScriptPanel: public Fl_Group
{
public:
    TFLScriptPanel(int x, int y, int w, int h, const char *label = nullptr);
    ~TFLScriptPanel();

    void SetScript(TTkScript *script) { mScript = script; }
    TFLScriptEditor *GetEditor() { return mEditor; }

    void LoadFile(const char *filename);
    char *DupSourceCode();
    void SetSourceCode(const char *sourcecode);
    void SetDirty();
    
private:
    TFLScriptEditor *mEditor = nullptr;
    TTkScript *mScript = nullptr;
};


/**
 * This class displays and provides editing capabilities for NewtonScript source code.
 */
class TFLScriptEditor : public Fl_Text_Editor
{
public:
    TFLScriptEditor(int x, int y, int w, int h, const char *label = nullptr);
    ~TFLScriptEditor();

    void SetPanel(TFLScriptPanel *panel) { mPanel = panel; }
    
private:
    void style_parse(const char *text, char *style, int length);
    void style_init();
    void style_update(int pos, int nInserted, int nDeleted,
                             int nRestyled, const char *deletedText);

    Fl_Text_Buffer *mTextBuffer  = nullptr;
    Fl_Text_Buffer *mStyleBuffer = nullptr;
    TFLScriptPanel *mPanel = nullptr;

// static:
    static Fl_Text_Display::Style_Table_Entry styletable[];
    static const char *code_keywords[];
    static const char *code_types[];
    static void style_update_cb(int pos, int nInserted, int nDeleted,
                      int nRestyled, const char *deletedText, void *This)
        { ((TFLScriptEditor*)This)->style_update(pos, nInserted, nDeleted, nRestyled, deletedText); }
    static void style_unfinished_cb(int, void*);
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
