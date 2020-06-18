// ==============================
// File:			TTkScript.h
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

#ifndef _T_SCRIPT_FILE_H
#define _T_SCRIPT_FILE_H

class TToolkit;
class TTkProject;
class TFLScriptPanel;

/**
 * This class manages NewtonScript source code files for the Einstein Toolkit.
 *
 * ScriptFiles can contain the entire command set for generating a package.
 */
class TTkScript
{
public:
    TTkScript(TToolkit *toolkit);
    ~TTkScript();

    void SetPanel(TFLScriptPanel *panel) { mPanel = panel; }
    void LoadFile(const char *filename);
    void SetSourceCode(const char *sourcecode);
    char *DupSourceCode();
    char *GetFilename() { return mFilename; }
    void SetFilename(const char *filename);
    bool IsDirty() { return mIsDirty; }
    void SetDirty();
    void ClearDirty();
    int Save();

private:
    TToolkit *mToolkit = nullptr;
    TFLScriptPanel *mPanel = nullptr;
    char *mFilename = nullptr;
    bool mIsDirty = false;
};

#endif
		// _T_SCRIPT_FILE_H

// ============================================================================ //
// NewtonScript
//
// A dynamic, object-oriented programming language developed for the Newton
// platform. Lovingly nicknamed WallyScript after its designer Walter Smith.
//
//   -- Newton Glossary
// ============================================================================ //
