// ==============================
// File:			TTkScript.cp
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


#include "TTkScript.h"

#include "TToolkit.h"
#include "TFLScriptPanel.h"


/*
 //#include "/Users/matt/dev/newt64/defs/newt.2.2.ns"
 //#include "/Users/matt/dev/newt64/defs/newt.bc.2.2.ns"

 p("Start");

 global bc := {
 ...
 };

 p("Interprete...");

 src :=
 {
...
 };

 p("Build package...");

 pkg := MakePkg(src);

 p("Save Package...");

 SaveBinary(pkg, "/Users/matt/dev/newton-test/mini.pkg");

 p("DONE");
*/

TTkScript::TTkScript(TToolkit *toolkit)
:   mToolkit(toolkit)
{
}


TTkScript::~TTkScript()
{
    if (mFilename)
        ::free(mFilename);
}



char *TTkScript::DupSourceCode()
{
    return mPanel->DupSourceCode();
}


void TTkScript::SetSourceCode(const char *sourcecode)
{
    mPanel->SetSourceCode(sourcecode);
}


void TTkScript::LoadFile(const char *filename)
{
    SetFilename(filename);
    mPanel->LoadFile(filename);
    ClearDirty();
}


void TTkScript::SetFilename(const char *filename)
{
    if (mFilename) ::free(mFilename);
    mFilename = nullptr;
    if (filename) mFilename = ::strdup(filename);
    mToolkit->UpdateTitle();
}


/**
 * Save the current text to a file on disk.
 *
 * \return a negative number if we could not save the file for any reason
 * \return 0 if we saved the file
 */
int TTkScript::Save()
{
    if (mFilename) {
        FILE *f = fopen(mFilename, "wb");
        if (!f)
            return -1;
        char *src = DupSourceCode();
        int len = strlen(src);
        int n = ::fwrite(src, 1, len, f);
        ::fclose(f);
        ::free(src);
        if (n!=len) return -1;
    }
    mIsDirty = false;
    mToolkit->UpdateTitle();
    return 0;
}


void TTkScript::SetDirty()
{
    if (!mIsDirty) {
        mIsDirty = true;
        mToolkit->UpdateTitle();
    }
}


void TTkScript::ClearDirty()
{
    if (mIsDirty) {
        mIsDirty = false;
        mToolkit->UpdateTitle();
    }
}


// ============================================================================ //
// NewtonScript
//
// A dynamic, object-oriented programming language developed for the Newton
// platform. Lovingly nicknamed WallyScript after its designer Walter Smith.
//
//   -- Newton Glossary
// ============================================================================ //
