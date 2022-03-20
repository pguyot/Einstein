// ==============================
// File:			TToolkit.h
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

#ifndef _TFLTOOLKIT_H
#define _TFLTOOLKIT_H

#include <K/Defines/KDefinitions.h>

#include <FL/Fl_Widget.H>
#include <FL/x.H>

class TFLApp;
class TTkScript;
class TTkProject;

/**
 * Toolkit is an integrated developer environment for Einstein.
 */
class TToolkit
{
public:
	TToolkit(TFLApp* inApp);
	~TToolkit();
	void Show();
	void Hide();

	int UserActionNew();
	int UserActionOpen(const char* in_filename = nullptr);
	int UserActionSave();
	int UserActionSaveAs();
	int UserActionClose();
	void UserActionQuit();

	void UserActionUndo();
	void UserActionRedo();
	void UserActionCut();
	void UserActionCopy();
	void UserActionPaste();
	void UserActionFind();

	void UserActionAddProto(const char *protoName);

	int UserActionDecompilePkg(const char* in_filename = nullptr);
	void UserActionBuild();
	void UserActionInstall();
	void UserActionRun();
	void UserActionStop();

	void UserActionFindTextChanged();
	void UserActionReplaceTextChanged();
	void UserActionCaseChanged();
	void UserActionRegexChanged();
	void UserActionFindPrev();
	bool UserActionFindNext(bool fromLast = true);
	void UserActionFindClose();
	void UserActionReplaceNext();
	void UserActionReplaceAll();

	void AppBuild();
	void AppInstall();
	void AppRun();
	void AppStop();
	void AppCmd(const char* cmd);

	void LoadSampleCode(int n);
	void UpdateTitle();
	void UpdateMenuBar();

	void LoadRecentFileMenu();
	void SaveRecentFileMenu();
	void UpdateRecentFileMenu(const char* new_file = nullptr);
	void ClearRecentFileMenu();

	void PrintStd(const char* text);
	void PrintErr(const char* text);
	void PrintErrFile(const char* filename);

private:
	int ReadScriptResults();
	void SetTempPkgPath();
	void SetPkgPath(const char* name);
	void SetPkgName(const char* name);
	void SetPkgSymbol(const char* name);
	void SetPkgLabel(const char* name);

	TFLApp* mApp = nullptr;

	// TODO: we currently allow one single script to be loaded. Add support for multiple files, and file hierarchies managed inside projects.
	TTkScript* mCurrentScript = nullptr;
	// TODO: implement a default project that collects all data required from the source code.
	// TTkProject *mProject = nullptr;

	char* mPkgPath = nullptr;
	char* mPkgName = nullptr;
	char* mPkgSymbol = nullptr;
	char* mPkgLabel = nullptr;
};

extern TToolkit* gToolkit;

#endif
// _TFLTOOLKIT_H

// ======================================================================= //
// Newton Toolkit
//
// A development environment created by Apple for writing NewtonScript programs
// for Newton OS. A prototype for Newton Toolkit called EtchASketch was written
// using Macintosh Common Lisp. Also referred to as NTK.
//
//   -- Newton Glossary
// ======================================================================= //
