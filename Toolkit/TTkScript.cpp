// ==============================
// File:			TTkScript.cp
// Project:			Einstein
//
// Copyright 2003-2022 by Paul Guyot and Matthias Melcher.
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

#include <cstdlib>

#include "TTkScript.h"

#include "TFLScriptPanel.h"
#include "TToolkit.h"

/**
 Create a Toolkit script file manager.
 */
TTkScript::TTkScript(TToolkit* toolkit) :
		mToolkit(toolkit)
{
}

/**
 Release all resources.
 */
TTkScript::~TTkScript()
{
	if (mFilename)
		::free(mFilename);
}

/**
 * Return a duplicate of the current source code in the editor.
 *
 * Caller is responsible to \c free() the text that was returned.
 *
 * \return a copy of the current source code
 */
char*
TTkScript::DupSourceCode()
{
	return mPanel->DupSourceCode();
}

/**
 Replace the current source code with new text.
 \param sourcecode the new source code in utf8 encoding.
 */
void
TTkScript::SetSourceCode(const char* sourcecode)
{
	mPanel->SetSourceCode(sourcecode);
}

/**
 Replace the current script with text from a file.
 Also changes the current filename.
 \param filename read new script from this file in utf8 encoding.
 */
void
TTkScript::LoadFile(const char* filename)
{
	SetFilename(filename);
	mPanel->LoadFile(filename);
	ClearDirty();
}

/**
 Set a new filename.
 Does not replace the script itself.
 \param filename the new path and name of a file.
 \see TTkScript::LoadFile(const char* filename)
 */
void
TTkScript::SetFilename(const char* filename)
{
	if (mFilename)
		::free(mFilename);
	mFilename = nullptr;
	if (filename)
		mFilename = ::strdup(filename);
	mToolkit->UpdateTitle();
}

/**
 * Save the current text to a file on disk.
 *
 * \return a negative number if we could not save the file for any reason
 * \return 0 if we saved the file
 */
int
TTkScript::Save()
{
	if (mFilename)
	{
		FILE* f = fopen(mFilename, "wb");
		if (!f)
			return -1;
		char* src = DupSourceCode();
		int len = strlen(src);
		int n = ::fwrite(src, 1, len, f);
		::fclose(f);
		::free(src);
		if (n != len)
			return -1;
	}
	mIsDirty = false;
	mToolkit->UpdateTitle();
	return 0;
}

/**
 Mark the current text as modified vs. the saved version of the text.
 Also updates the \a dirty indicator in the title.
 */
void
TTkScript::SetDirty()
{
	if (!mIsDirty)
	{
		mIsDirty = true;
		mToolkit->UpdateTitle();
	}
}

/**
 Mark the current text as unmodified vs. the saved version of the text.
 Also updates the \a dirty indicator in the title.
 */
void
TTkScript::ClearDirty()
{
	if (mIsDirty)
	{
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
