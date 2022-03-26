// ==============================
// File:			TFLToolkit.cp
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

// TODO: Horizontal scrollbar in Inspector must go
// TODO: ScriptEditor needs its own class and Find/Replace, Cut/Copy/Paste, etc.
// TODO: Better syntax highlighting
// TODO: single project with multiple files
// TODO: NTK import
// TODO: visual editor -> source code
// TODO: assembler error messages could have line numbers
// TODO: include C++ the same way we included ARM code
// TODO: byte code to source code decompiler
// TODO: pulldown menu to suggest protos based on the parent _proto.
// TODO: pulldown menus to suggest slots for the current _proto (see NTK/DyneTK)
// TODO: a connection to a physical MessagePad (see NTK Toolkit/DyneTK)

/* NOTE:
 This is where the MP2100US ROM interpretes bytecodes "fast"
 cmp     r0, #207                    @ [ 0x000000CF ] 0x002EE1DC 0xE35000CF - .P..
 and this is the slow version:
 cmp     r2, #207                    @ [ 0x000000CF ] 0x002F2028 0xE35200CF - .R..
 We could add a new BC command, BC26 (0xD0nnnn), where nnnn is the line in the
 source code. This would allow the debugger to show where bytecode execution is
 at right now. We should have a BC27 at the start of every bytecode stream that
 gives us an index to the source file, and an array of source files at the start
 of the NSOF part in the package. BC31 is the highest possible bytecode.
 TODO: __LINE__ implement a "current line"
 TODO: __FILE__ implement a "current file" as a stack (call...return)
 TODO: the Newt/64 #include statement must push and pop the current filename and line number
 TODO: can we implement single-stepping by just pausing at __LINE__ bytecode?
 TODO: can we use the top bit of the __LINE__ bytecode (or another BC) to indicate a breakpoint?
 */

/*
 TFLToolkit is an integrated NewtonScript compiler, decompiler, and debugger
 for Einstein.
 */

#include <K/Defines/KDefinitions.h>
#include "TToolkit.h"
#include "TFLSampleScripts.h"
#include "TFLScriptPanel.h"
#include "TFLToolkitUI.h"
#include "TTkScript.h"
#include "TToolkitPrototypes.h"
#include "app/FLTK/TFLSettingsUI.h"

#define IGNORE_TNEWT
#include "Emulator/Platform/TPlatformManager.h"
#undef IGNORE_TNEWT

extern "C" {
#include "NewtBC.h"
#include "NewtCore.h"
#include "NewtEnv.h"
#include "NewtFile.h"
#include "NewtNSOF.h"
#include "NewtParser.h"
#include "NewtPkg.h"
#include "NewtPrint.h"
#include "NewtVM.h"
}

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>

#if TARGET_OS_WIN32
#include <Winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <streambuf>
#include <string>
#include <unistd.h>

Fl_Text_Buffer* gTerminalBuffer = nullptr;

TToolkit* gToolkit = nullptr;

/**
 Create the Toolkit environment inside the Einstein app.
 */
TToolkit::TToolkit(TFLApp* inApp) :
		mApp(inApp)
{
	gToolkit = this;
}

/**
 Return all resources to the system.
 */
TToolkit::~TToolkit()
{
	delete mCurrentScript;
	if (mPkgPath)
		::free(mPkgPath);
	if (mPkgName)
		::free(mPkgName);
	if (mPkgSymbol)
		::free(mPkgSymbol);
	if (mPkgLabel)
		::free(mPkgLabel);
}

/**
 Show the Toolkit window.

 If the window has not been created yet, use the FLuid generated code to
 build it. Also use Preferences to show the window at the previous location.
 */
void
TToolkit::Show()
{
	if (!wToolkitWindow)
	{
		Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
		Fl_Preferences toolkit(prefs, "Toolkit");
		Fl_Preferences tkWindow(toolkit, "Window");
		int x, y, w, h, hTileSep;
		tkWindow.get("x", x, 400);
		tkWindow.get("y", y, 80);
		tkWindow.get("w", w, 720);
		tkWindow.get("h", h, 600);
		tkWindow.get("hTileSep", hTileSep, 500);

		wToolkitWindow = CreateToolkitWindow(x, y);
		wToolkitWindow->size_range(150, 78 + 150);

		wToolkitTerminal->buffer(gTerminalBuffer = new Fl_Text_Buffer());
		wToolkitTerminal->scrollbar_align(FL_ALIGN_RIGHT);
		LoadRecentFileMenu();

		// FIXME: allow multiple scripts and multiple panels in a Tab, and a hierarchy of scripts in a Project
		mCurrentScript = new TTkScript(this);
		wScriptPanel->SetScript(mCurrentScript);
		mCurrentScript->SetPanel(wScriptPanel);
		mCurrentScript->SetSourceCode(kToolkitSampleScriptHelloWorld);
		// mCurrentScript->LoadFile("/Users/matt/dev/newton-test/mini.ns");
		mCurrentScript->ClearDirty();

		wToolkitWindow->show();
		wToolkitWindow->resize(x, y, w, h);
		wTile->init_sizes();
		wTile->position(0, wToolkitTerminal->y(), 0, hTileSep);
	}
	UpdateTitle();
	wToolkitWindow->show();
}

/**
 Hide the Toolkit window.

 This does not release any resources.
 */
void
TToolkit::Hide()
{
	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
	Fl_Preferences toolkit(prefs, "Toolkit");
	Fl_Preferences tkWindow(toolkit, "Window");
	if (wToolkitWindow)
	{
		tkWindow.set("x", wToolkitWindow->x());
		tkWindow.set("y", wToolkitWindow->y());
		tkWindow.set("w", wToolkitWindow->w());
		tkWindow.set("h", wToolkitWindow->h());
		tkWindow.set("hTileSep", wToolkitTerminal->y());
		wToolkitWindow->hide();
	}
}

/**
 * Create a new NewtonScript file.
 *
 * Present a file chooser to the user to set the filename of a new NewtonScript file.
 * Do not create the file, but create an editor with minimal startup text and the filename set
 * and marked dirty.
 *
 * \return a value<0 if creating the script failed for some reason.
 */
void testNSOFReader(const char* filename);

int
TToolkit::UserActionNew()
{
	//  testNSOFReader("/Users/matt/dev/Newton/NewtonDev/NewtonDev/NTK 1.6.4/Platforms/Newton 2.1");
	//  return 0;
	char prev_file[FL_PATH_MAX];
	prev_file[0] = 0;
	const char* fn = mCurrentScript->GetFilename();
	if (fn)
	{
		strncpy(prev_file, fn, sizeof(prev_file));
	} else
	{
		fn = fl_getenv("HOME");
		if (fn)
			snprintf(prev_file, sizeof(prev_file), "%s/", fn);
	}

	int ret = UserActionClose();
	if (ret < 0)
		return ret;

	const char* filename = gApp->ChooseNewFile("New NewtonScript File",
		"NewtonScript\t*.{ns,nscript,script}",
		prev_file);
	if (!filename)
		return -1;

	// set script to the new filename and load the file
	mCurrentScript->SetFilename(filename);
	FILE* f = fopen(filename, "rb");
	if (f)
	{
		fclose(f);
		ret = fl_choice("File already exists.\n\n"
						"Do you want to overwrite the existing file\n"
						"with a new, empty script?",
			"Open existing Script", "Overwrite Script", "Abort");
		if (ret == 0)
			return -1;
		if (ret == 1)
			mCurrentScript->SetDirty();
		if (ret == 2)
			mCurrentScript->LoadFile(filename);
	}
	UpdateRecentFileMenu(filename);
	return 0;
}

/**
 * Close the current file and open a new one from disk.
 *
 * Close the current file, notifying the user if it is dirty.
 * If the filename is the default `nullptr`, Present a file chooser to load
 * another existing file from disk.
 *
 * \param in_filename optional file path and name
 */
int
TToolkit::UserActionOpen(const char* in_filename)
{
	char prev_file[FL_PATH_MAX];
	prev_file[0] = 0;
	const char* fn = mCurrentScript->GetFilename();
	if (fn)
	{
		strncpy(prev_file, fn, sizeof(prev_file));
	} else
	{
		fn = fl_getenv("HOME");
		if (fn)
			snprintf(prev_file, sizeof(prev_file), "%s/", fn);
	}

	int ret = UserActionClose();
	if (ret < 0)
		return ret;

	const char* filename = in_filename;
	if (!filename || !*filename)
	{
		filename = gApp->ChooseExistingFile("Open NewtonScript File",
			"NewtonScript\t*.{ns,nscript,script}",
			prev_file);
		if (!filename)
			return -1;
	}

	// set script to the new filename and load the file
	FILE* f = fopen(filename, "rb");
	if (!f)
	{
		fl_alert("Error reading file\n%s\n%s", filename, strerror(errno));
	} else
	{
		fclose(f);
		mCurrentScript->SetFilename(filename);
		UpdateRecentFileMenu(filename);
		mCurrentScript->LoadFile(filename);
	}
	return 0;
}

/**
 * Write this file to disk.
 *
 * If there is no filename given, ask the user for a filename.
 * If writing failed, we present a dialog box to the user.
 *
 * \return a value<0 if the operation failed.
 */
int
TToolkit::UserActionSave()
{
	int result = 0;
	if (mCurrentScript->GetFilename())
	{
		// we have a filename, so just save the file to disk
		result = mCurrentScript->Save();
		if (result < 0)
		{
			fl_alert("Error writing file\n%s\n%s", mCurrentScript->GetFilename(), strerror(errno));
		}
	} else
	{
		// if there is no filename given, run SaveAs instead
		return UserActionSaveAs();
	}
	return result;
}

/**
 * Ask the user for a filename, change to the new name, and save the file to disk,
 *
 * \return a value<0 if the operation failed.
 */
int
TToolkit::UserActionSaveAs()
{
	const char* filename = gApp->ChooseNewFile("Save NewtonScript As...",
		"NewtonScript\t*.{ns,nscript,script}",
		mCurrentScript->GetFilename());
	if (!filename)
		return -1;
	mCurrentScript->SetFilename(filename);
	UpdateRecentFileMenu(filename);
	mCurrentScript->SetDirty();
	return UserActionSave();
}

/**
 * Close the current script file.
 *
 * This leaves the user with an empty canvas. Users can simply type away and
 * write NewtonScript commands. They can even them from memory by choosing "Buidl"
 * and install them if the happen to create a package (Toolkit will assign a temp file).
 *
 * \return a value<0 if the operation failed (i.e. user canceled the operation).
 */
int
TToolkit::UserActionClose()
{
	if (mCurrentScript->IsDirty())
	{
		int ret = fl_choice(
			"Unsaved changes.\n\n"
			"Do you want to save your changes before\n"
			"closing this script?",
			"Continue Editing", "Save and Close", "Discard Script");
		if (ret == 0)
			return -1;
		if (ret == 1)
			if (UserActionSave() < 0)
				return -1;
	}
	// set script to empty and filename to null
	mCurrentScript->SetSourceCode("");
	mCurrentScript->SetFilename(nullptr);
	return 0;
}

/**
 * Close the app.
 *
 * If the script is dirty, try to save it first. If the user cancels, cacel the Quit action.
 */
void
TToolkit::UserActionQuit()
{
	if (UserActionClose() == 0)
		gApp->UserActionQuit();
}

void
TToolkit::UserActionUndo()
{
}

void
TToolkit::UserActionRedo()
{
}

void
TToolkit::UserActionCut()
{
}

void
TToolkit::UserActionCopy()
{
}

void
TToolkit::UserActionPaste()
{
}

/**
 Prepare the GUI so the user can find a string in the script file.
 */
void
TToolkit::UserActionFind()
{
	// select the text in the find text widget
	wToolkitFindText->position(0, wToolkitFindText->size());
	// find the first occurence of this text
	UserActionFindNext(false);
	// and activate the find-text input field
	wToolkitFindText->take_focus();
}

/**
 User wants to add a user element by generating frame with the given proto.
 \param protoName name of the new proto
 */
void
TToolkit::UserActionAddProto(const char* protoName)
{
	if (mCurrentScript->Panel())
		mCurrentScript->Panel()->AddProtoTemplate(protoName);
}

/**
 User wants to build the current script into a package.
 */
void
TToolkit::UserActionBuild()
{
	if (mCurrentScript->GetFilename())
		mCurrentScript->Save();
	AppBuild();
	// TODO: handle build errors
}

/**
 User wants to build a package and install it on Einstein.
 */
void
TToolkit::UserActionInstall()
{
	if (mCurrentScript->GetFilename())
		mCurrentScript->Save();
	AppBuild();
	// TODO: handle build errors
	AppInstall();
	// TODO: handle install errors
}

/**
 User wants to build, install, and run the current script on Einstein.
 */
void
TToolkit::UserActionRun()
{
	if (mCurrentScript->GetFilename())
		mCurrentScript->Save();
	AppBuild();
	// TODO: handle build errors
	AppInstall();
	// TODO: handle install errors
	AppRun();
	// TODO: handle run errors
}

/**
 Stop the running app on Einstein that corresponds to our script.
 */
void
TToolkit::UserActionStop()
{
	AppStop();
}

/**
 Take a full filename and path and create a user readable text in the menu.

 \param i index into recent file menu array
 \param path full path and name of file
 */
static void
set_recent_file_menu_item(int i, const char* path)
{
	Fl_Menu_Item* mi = wTKOpenRecentMenu[i];
	// -- avoid reallocating if path is the same
	if (mi->user_data_ && strcmp(path, (char*) mi->user_data_) == 0)
		return;
	// -- replace the user_data
	if (mi->user_data_)
		::free(mi->user_data_);
	mi->user_data_ = (void*) strdup(path);
	// -- replace the label
	if (mi->text && *mi->text != '\n')
		::free((void*) mi->text);
	// -- copy the entire path, but shorten it with an elipsis if it is too long
	char* menu_text = strdup(path);
	int n = strlen(path);
	if (n > 43)
	{
		strcpy(menu_text + 20, "...");
		memmove(menu_text + 23, menu_text + n - 20, 21);
		mi->text = strdup(menu_text);
		::free(menu_text);
	} else
	{
		mi->text = menu_text;
	}
	// -- set flags
	if (*path)
	{
		mi->show();
		mi->flags &= ~FL_MENU_DIVIDER;
	} else
	{
		mi->hide();
		if (i > 0)
			wTKOpenRecentMenu[i - 1]->flags |= FL_MENU_DIVIDER;
	}
}

/**
 Load a list of recent file paths and names from the app Preferences.
 */
void
TToolkit::LoadRecentFileMenu()
{
	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
	Fl_Preferences recent_files(prefs, "Toolkit/RecentFiles");
	for (int i = 0; i < 8; i++)
	{
		char filename[FL_PATH_MAX];
		filename[0] = 0;
		recent_files.get(Fl_Preferences::Name(i), filename, "", FL_PATH_MAX);
		set_recent_file_menu_item(i, filename);
	}
}

/**
 Save a list of recent file paths and names to the app Preferences.
 */
void
TToolkit::SaveRecentFileMenu()
{
	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
	Fl_Preferences recent_files(prefs, "Toolkit/RecentFiles");
	for (int i = 0; i < 8; i++)
	{
		recent_files.set(Fl_Preferences::Name(i), (char*) wTKOpenRecentMenu[i]->user_data());
	}
}

/**
 Add a file path with name to the list of recently opened files.

 If the file is already in the list, make it the first enty and push all other
 entries down by one. If the file is not in the list, make it first and push
 all other filenames down.

 If anything changed, save the list to the app Preferences.
 */
void
TToolkit::UpdateRecentFileMenu(const char* new_file)
{
	if (!new_file || !*new_file)
		return;
	int i;
	for (i = 0; i < 8; i++)
	{
		if (strcmp(new_file, (char*) wTKOpenRecentMenu[i]->user_data()) == 0)
			break;
	}
	if (i == 8)
	{ // not found
		auto a = wTKOpenRecentMenu[7]->user_data_;
		auto b = wTKOpenRecentMenu[7]->text;
		for (i = 7; i > 0; i--)
		{
			wTKOpenRecentMenu[i]->user_data_ = wTKOpenRecentMenu[i - 1]->user_data_;
			wTKOpenRecentMenu[i]->text = wTKOpenRecentMenu[i - 1]->text;
			wTKOpenRecentMenu[i]->flags = wTKOpenRecentMenu[i - 1]->flags;
		}
		wTKOpenRecentMenu[0]->user_data_ = a;
		wTKOpenRecentMenu[0]->text = b;
		set_recent_file_menu_item(0, new_file);
		SaveRecentFileMenu();
	} else if (i != 0)
	{
		auto a = wTKOpenRecentMenu[i]->user_data_;
		auto b = wTKOpenRecentMenu[i]->text;
		for (; i > 0; i--)
		{
			wTKOpenRecentMenu[i]->user_data_ = wTKOpenRecentMenu[i - 1]->user_data_;
			wTKOpenRecentMenu[i]->text = wTKOpenRecentMenu[i - 1]->text;
			wTKOpenRecentMenu[i]->flags = wTKOpenRecentMenu[i - 1]->flags;
		}
		wTKOpenRecentMenu[0]->user_data_ = a;
		wTKOpenRecentMenu[0]->text = b;
		SaveRecentFileMenu();
	}
}

/**
 Remove all filepaths from the recent file menu and save an empty list
 to the app Preferences.
 */
void
TToolkit::ClearRecentFileMenu()
{
	for (int i = 0; i < 8; i++)
	{
		set_recent_file_menu_item(0, "");
	}
	SaveRecentFileMenu();
}

/**
 The 'info' element in the Package header wants a binary object containing ASCII without a trailing 0.

 This is a simple function because newt/64 stores strings internally as UTF8 already.

 \param klass provide a class symbol for the binary
 \param text in UTF8
 \param literal see NewtMakeBinary
 \return a binary object, or `kNewtRefUnbind`
 */
static newtRef
NewtMakeBinaryFromString(newtRefArg klass, const char* text, bool literal)
{
	uint32_t size = (uint32_t) strlen(text);
	newtRef obj = NewtMakeBinary(klass, 0, size, literal);
	if (obj)
	{
		uint8_t* dst = NewtRefToBinary(obj);
		memcpy(dst, text, size);
		return obj;
	}
	return kNewtRefUnbind;
}

/**
 NewtonScript interface to `NewtMakeBinaryFromString`.
 \param rcvr ignored
 \param text UTF8 text ref
 \param klass ref to class symbol
 \return a binary object, or throws an exception
 */
static newtRef
NsMakeBinaryFromString(newtRefArg rcvr, newtRefArg text, newtRefArg klass)
{
	(void) rcvr;
	if (!NewtRefIsString(text))
		return NewtThrow(kNErrNotAString, text);
	return NewtMakeBinaryFromString(klass, NewtRefToString(text), false);
}

/**
 Compile the text filed of the function into a binary object.

 This allows users to inline ARM code directly into a NewtonScript app.
 \code
	peek: {
		class : 'BinCFunction,
		code: MakeBinaryFromARM(
		"	mov		r0, [r1]	\n"	// unreference the first argument
		"	mov		r0, [r0]	\n"	// and return it
		"  	mov		pc, lr 		\n"	// return an NS value
		),
		numArgs: 1,
		offset: 0
	}
 \endcode
 \param text utf8 formatted source code
 \return a binary object
 */
newtRef
NewtMakeBinaryFromARM(const char* text, bool /*literal*/)
{
	auto text_len = strlen(text);
	// write the string to a temporary file
	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
	char basename[FL_PATH_MAX + 1];
	prefs.get_userdata_path(basename, FL_PATH_MAX);
	char srcfilename[FL_PATH_MAX];
	strncpy(srcfilename, basename, FL_PATH_MAX);
	strncat(srcfilename, "inline.s", FL_PATH_MAX);
	char objfilename[FL_PATH_MAX];
	strncpy(objfilename, basename, FL_PATH_MAX);
	strncat(objfilename, "inline.o", FL_PATH_MAX);
	char binfilename[FL_PATH_MAX];
	strncpy(binfilename, basename, FL_PATH_MAX);
	strncat(binfilename, "inline", FL_PATH_MAX);
	char errfilename[FL_PATH_MAX];
	strncpy(errfilename, basename, FL_PATH_MAX);
	strncat(errfilename, "inline.err", FL_PATH_MAX);
	// run `arm-none-eabi-as -march=armv4 -mbig-endian test.s -o test.o`

	FILE* f = fl_fopen(srcfilename, "wb");
	fwrite(text, text_len, 1, f);
	fclose(f);
	char cmd[4 * FL_PATH_MAX];
	snprintf(cmd, sizeof(cmd),
		"\"%s\" -march=armv4 -mbig-endian \"%s\" -o \"%s\" >\"%s\" 2>&1",
		gApp->GetSettings()->mDevAsmPath,
		srcfilename, objfilename, errfilename);
	fl_system(cmd);
	gToolkit->PrintErrFile(errfilename);

	// run `arm-none-eabi-objcopy -O binary -j .text test.o test`
	snprintf(cmd, sizeof(cmd),
		"\"%s\" -O binary -j .text \"%s\" \"%s\" >\"%s\" 2>&1",
		gApp->GetSettings()->mDevObjCopyPath,
		objfilename, binfilename, errfilename);
	fl_system(cmd);
	gToolkit->PrintErrFile(errfilename);

	newtRef filename_ref = NewtMakeString(binfilename, false);
	return NsLoadBinary(kNewtRefUnbind, filename_ref);
}

/**
 NewtonScript interface to `NewtMakeBinaryFromARM`.
 \param rcvr ignored
 \param text UTF8 arm assembly code (note indents and newline formatting)
 \return a binary object, or throws an exception
 */
static newtRef
NsMakeBinaryFromARM(newtRefArg rcvr, newtRefArg text)
{
	(void) rcvr;
	if (!NewtRefIsString(text))
		return NewtThrow(kNErrNotAString, text);
	return NewtMakeBinaryFromARM(NewtRefToString(text), false);
}

/**
 Patch a ROM file using the given ARM instructions.

 This function will assemble the ARM instructions in the first string
 into an object file. Then we read the object file and patch the segment
 into the binary file.

 \code
 PatchFileFromARM("
	.org 0x001412f8
	ldmdb   r11, {r11, sp, pc} @ return early from screen calibrartion
	", "myROMFile")
 \endcode
 \param text utf8 formatted source code
 \param filename using the path to the current ROM, or a path plus filename
 \return a binary object
 \note This patch function is not perfect. Four or more consecutine 0 bytes
 may not actually be written.
 \todo Update ROM and REx checksums to avoid deleteing interneal flash.
 \todo Fix overall checksum to give Einstein a chance to recognize pre-patched ROMs.
 */
newtRef
NewtPatchFileFromARM(const char* text, const char* filename, bool /*literal*/)
{
	auto text_len = strlen(text);
	// write the string to a temporary file
	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
	char basename[FL_PATH_MAX + 1];
	prefs.get_userdata_path(basename, FL_PATH_MAX);
	char srcfilename[FL_PATH_MAX];
	strncpy(srcfilename, basename, FL_PATH_MAX);
	strncat(srcfilename, "inline.s", FL_PATH_MAX);
	char objfilename[FL_PATH_MAX];
	strncpy(objfilename, basename, FL_PATH_MAX);
	strncat(objfilename, "inline.o", FL_PATH_MAX);
	char disfilename[FL_PATH_MAX];
	strncpy(disfilename, basename, FL_PATH_MAX);
	strncat(disfilename, "inline.dis", FL_PATH_MAX);
	char errfilename[FL_PATH_MAX];
	strncpy(errfilename, basename, FL_PATH_MAX);
	strncat(errfilename, "inline.err", FL_PATH_MAX);

	// run `arm-none-eabi-as -march=armv4 -mbig-endian test.s -o test.o`
	FILE* f = fl_fopen(srcfilename, "wb");
	fwrite(text, text_len, 1, f);
	fclose(f);
	char cmd[4 * FL_PATH_MAX];
	snprintf(cmd, sizeof(cmd),
		"\"%s\" -march=armv4 -mbig-endian \"%s\" -o \"%s\" >\"%s\" 2>&1",
		gApp->GetSettings()->mDevAsmPath,
		srcfilename, objfilename, errfilename);
	fl_system(cmd);
	gToolkit->PrintErrFile(errfilename);

	// run `arm-none-eabi-objdump -d test.o >test.dis`
	snprintf(cmd, sizeof(cmd),
		"\"%s\" -d --show-raw-insn \"%s\" >\"%s\" 2>\"%s\"",
		gApp->GetSettings()->mDevObjDumpPath,
		objfilename, disfilename, errfilename);
	fl_system(cmd);
	gToolkit->PrintErrFile(errfilename);

	char appDir[FL_PATH_MAX], romDir[FL_PATH_MAX];
	getcwd(appDir, FL_PATH_MAX);
	fl_filename_absolute(romDir, FL_PATH_MAX, gApp->GetSettings()->ROMPath);
	char* x = (char*) fl_filename_name(romDir);
	if (x)
		x[0] = 0;
	chdir(romDir);
	int bin = fl_open(filename, O_WRONLY | O_CREAT);
	chdir(appDir);
	if (bin == -1)
	{
		gToolkit->PrintErr("Can't open ");
		gToolkit->PrintErr(filename);
		gToolkit->PrintErr(": ");
		gToolkit->PrintErr(strerror(errno));
		gToolkit->PrintErr("\n");
		return NewtMakeInteger(errno);
	}
	f = fl_fopen(disfilename, "rb");
	if (!f)
	{
		close(bin);
		gToolkit->PrintErr("Can't open disassembled file\n");
		return NewtMakeInteger(-1);
	}
	for (;;)
	{
		char buf[FL_PATH_MAX];
		if (fgets(buf, FL_PATH_MAX, f) == nullptr)
			break;
		char* sep = strstr(buf, ":\t");
		if (sep == nullptr)
			continue;
		KUInt32 addr;
		union {
			KUInt32 data;
			KUInt8 d[4];
		};
		int n = sscanf(buf, "%8x:\t%x", &addr, &data);
		if (n != 2)
			continue;
		lseek(bin, addr, SEEK_SET);
		data = htonl(data);
		if (sep[4] == ' ')
			write(bin, d + 3, 1);
		else if (sep[6] == ' ')
			write(bin, d + 2, 2);
		else if (sep[8] == ' ')
			write(bin, d + 1, 3);
		else
			write(bin, d, 4);
		if (n == 2)
		{
			printf("0x%08x: 0x%08x\n", addr, data);
		}
	}
	close(bin);
	fclose(f);
	return kNewtRefNIL;
}

/**
 NewtonScript interface to `NewtMakeBinaryFromARM`.
 \param rcvr ignored
 \param text UTF8 arm assembly code (note indents and newline formatting)
 \return a binary object, or throws an exception
 */
static newtRef
NsPatchFileFromARM(newtRefArg rcvr, newtRefArg text, newtRefArg filename)
{
	(void) rcvr;
	if (!NewtRefIsString(text))
		return NewtThrow(kNErrNotAString, text);
	return NewtPatchFileFromARM(NewtRefToString(text), NewtRefToString(filename), false);
}

/**
 Implements the NTK function `AddStepForm`.
 \param rcvr
 \param form add to this form
 \param child add this child
 \return `kNewtRefNIL` or throws an exception.
 */
static newtRef
NSAddStepForm(newtRefArg rcvr, newtRefArg form, newtRefArg child)
{
	(void) rcvr;
	if (!NewtRefIsFrame(form))
	{
		return NewtThrow(kNErrNotAFrame, NewtMakeString("First argument should be a view form", true));
	}
	if (!NewtRefIsFrame(child))
	{
		return NewtThrow(kNErrNotAFrame, NewtMakeString("Second argument should be a view form", true));
	}
	newtRef stepChildren = NcGetSlot(form, NSSYM(StepChildren));
	if (!NewtRefIsArray(stepChildren))
	{
		stepChildren = NewtMakeArray(NSSYM(array), 0);
		NcSetSlot(form, NSSYM(StepChildren), stepChildren);
	}
	NcAddArraySlot(stepChildren, child);
	return kNewtRefNIL;
}

/**
 Implements the NTK function `StepDeclare`.
 \param rcvr
 \param form add to this form
 \param ref reference to child
 \param sym symbol to access child
 \return `kNewtRefNIL` or throws an exception.
 */
static newtRef
NSStepDeclare(newtRefArg rcvr, newtRefArg form, newtRefArg ref, newtRefArg sym)
{
	(void) rcvr;

	if (!NewtRefIsFrame(form))
	{
		return NewtThrow(kNErrNotAFrame, NewtMakeString("First argument should be a view form", true));
	}
	if (!NewtRefIsFrame(ref))
	{
		return NewtThrow(kNErrNotAFrame, NewtMakeString("Second argument should be a view form", true));
	}
	if (!NewtRefIsSymbol(sym))
	{
		return NewtThrow(kNErrNotASymbol, NewtMakeString("Third argument should be a symbol", true));
	}
	newtRef stepAllocateContext = NcGetSlot(form, NSSYM(StepAllocateContext));
	if (!NewtRefIsArray(stepAllocateContext))
	{
		stepAllocateContext = NewtMakeArray(NSSYM(array), 0);
		NcSetSlot(form, NSSYM(StepAllocateContext), stepAllocateContext);
	}
	NcAddArraySlot(stepAllocateContext, sym);
	NcAddArraySlot(stepAllocateContext, ref);
	return kNewtRefNIL;
}

/**
 Grab the current script from the editor and build a package file.

 Package filename is generated from Script filename.
 NewtonScript definitions are always loaded first.
 A default package header is loaded to allow minimal scripts. All aspects of
 the default header can be overwritten.

 Decalre `kAppName := "AppName:DEVNAME";` to customize the name of your app.
 Declare `kAppSymbol := 'AppName:DEVNAME;` to identify your app, and
 declare `kAppLabel := "Hello";` to define the name of your app in the
 \a Extras folder.

 All package attributes are defined in `newt.app` and `newt.app.parts[0]` and
 can be modified as needed.

 There is no return code. All error texts are sent to the Toolkit console.

 \note This function does not save the script to a file! If no filename is
 given, we create a temporary filename for the package.

 \todo add function to easily create app icons and graphics using PNG and GIF
 images. Do the same for importing sound.
 */
void
TToolkit::AppBuild()
{
	wToolkitTerminal->buffer()->text("");

	char buf[2 * FL_PATH_MAX];
	//  newtRefVar result;
	newtErr err;
	const char* argv[] = { "Einstein" };
	NewtInit(1, argv, 0);
	// NEWT_TRACE = true;

	NewtDefGlobalFunc0(NSSYM(MakeBinaryFromString), (void*) NsMakeBinaryFromString, 2, false, (char*) "MakeBinaryFromString(str, sym)");
	NewtDefGlobalFunc0(NSSYM(MakeBinaryFromARM), (void*) NsMakeBinaryFromARM, 1, false, (char*) "MakeBinaryFromARM(ARM_Instructions)");
	NewtDefGlobalFunc0(NSSYM(PatchFileFromARM), (void*) NsPatchFileFromARM, 2, false, (char*) "PatchFileFromARM(ARM_Instructions, filename)");
	NewtDefGlobalFunc0(NSSYM(AddStepForm), (void*) NSAddStepForm, 2, false, (char*) "AddStepForm(mainView, scrollClipper);");
	NewtDefGlobalFunc0(NSSYM(StepDeclare), (void*) NSStepDeclare, 3, false, (char*) "StepDeclare(mainView, scrollClipper, 'scrollClipper);");

	// FIXME: does this work?
	NcDefGlobalVar(NSSYM0(_STDERR_), NewtMakeString("", false));
	NcDefGlobalVar(NSSYM0(_STDOUT_), NewtMakeString("", false));

	// #file ...
	// #line 1
	std::string src;
	src.append(TToolkitPrototype::NewtonDefs21);
	src.append(TToolkitPrototype::BytecodeDefs);
	src.append(TToolkitPrototype::ToolkitDefs);
	src.append(TToolkitPrototype::DefaultPackage);
	if (mCurrentScript->GetFilename())
	{
		char pkg_path[FL_PATH_MAX];
		strcpy(pkg_path, mCurrentScript->GetFilename());
		fl_filename_setext(pkg_path, FL_PATH_MAX, ".pkg");
		SetPkgPath(pkg_path);
	} else
	{
		SetTempPkgPath();
	}
	sprintf(buf, "newt.pkgPath := \"%s\";\n", mPkgPath);
	src.append(buf);
	src.append(TToolkitPrototype::ToolkitLaunch);

	if (mCurrentScript->GetFilename())
	{
		PrintStd("Compiling file...\n");
		if (mCurrentScript->IsDirty())
		{
			mCurrentScript->Save();
		}
		std::ifstream t(mCurrentScript->GetFilename());
		std::string str((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
		src.append(str);
	} else
	{
		PrintStd("Compiling inline...\n");
		char* sourceCode = mCurrentScript->DupSourceCode();
		src.append(sourceCode);
		free(sourceCode);
	}
	src.append(TToolkitPrototype::ToolkitDone);

	// FIXME: NVMInterpretStr sets _STDERR_ and _STDOUT_ to NIL
	/* result = */ NVMInterpretStr(src.c_str(), &err);

	//    puts(src.c_str());

	// TODO: get the app symbol to install and uninstall it
	// TODO: get the app name
	// TODO: get the package path, or build a temp package

	newtRef outRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDOUT_));
	if (NewtRefIsString(outRef))
	{
		const char* outStr = NewtRefToString(outRef);
		PrintStd(outStr);
	}

	newtRef errRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDERR_));
	if (NewtRefIsString(errRef))
	{
		const char* errStr = NewtRefToString(errRef);
		PrintErr(errStr);
	}

	if (ReadScriptResults() == 0)
	{
		newtRef newt = NsGetGlobalVar(kNewtRefNIL, NSSYM(newt));
		NcSend0(newt, NSSYM(writePkg));
	}

	// TODO: does this work?
	NcDefGlobalVar(NSSYM0(_STDERR_), NewtMakeString("", false));
	NcDefGlobalVar(NSSYM0(_STDOUT_), NewtMakeString("", false));
	outRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDOUT_));
	if (NewtRefIsString(outRef))
	{
		const char* outStr = NewtRefToString(outRef);
		PrintStd(outStr);
	}
	errRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDERR_));
	if (NewtRefIsString(errRef))
	{
		const char* errStr = NewtRefToString(errRef);
		PrintErr(errStr);
	}
	// PrintErr(mPkgPath);

	NewtCleanup();
}

/**
 Install a package into the Einstein emulation.

 If an app with that Package Symbol already exists, it will be deleted first.

 The package name is derived from the name of the current script. If the script
 is unnamed, Toolkit wil generate a temporary name.
 */
void
TToolkit::AppInstall()
{
	PrintStd("Installing...\n");
	TPlatformManager* mgr = mApp->GetPlatformManager();

#if 0
	// This code installs a global function that can call the Einstein Platform Manager from NewtonScript.
	// It is currently not needed, but may be used to synchronize and return data from NewtonOS to Einstein.
	mgr->EvalNewtonScript(
						  "cdata := MakeBinary(20, 'nativeModule);\n"
						  "StuffLong(cdata,  0, -0x16D2C000);\n"  //      stmdb	sp!, { lr }
						  "StuffLong(cdata,  4, -0x1A601FFC);\n"  //      ldr		lr, sym
						  "StuffLong(cdata,  8, -0x11FF15F0);\n"  //      mcr		p10, 0, lr, c0, c0
						  "StuffLong(cdata, 12, -0x17428000);\n"  //      ldmia	sp!, { pc }
						  "StuffLong(cdata, 16,  0x00000122);\n"  // sym: dcd     0x00000122
						  "ff := {\n"
						  "       class : 'BinCFunction,\n"
						  "       code : cdata,\n"
						  "       numArgs: 2,\n"
						  "       offset : 0\n"
						  "};\n"
						  "DefGlobalFn('CallEinstein, func(a, b) call ff with (a, b) );\n"
						  "CallEinstein('x, 'y);\n"
						  );
#endif

	// uninstall the current package first
	const char* cmd = "if HasSlot(GetRoot(), '|%s|) then begin\n"
					  "  GetRoot().|%s|:Close();\n"
					  "  SafeRemovePackage(GetPkgRef(\"%s\", GetStores()[0]))\n"
					  "end;\n";
	char* buf = (char*) ::malloc(strlen(cmd) + 1024);
	sprintf(buf, cmd, mPkgSymbol, mPkgSymbol, mPkgName);
	mgr->EvalNewtonScript(buf);
	// puts(buf);
	::free(buf);

	// install the package that we just created
	mApp->InstallPackagesFromURI(mPkgPath);
	// mgr->EvalNewtonScript("|Einstein:log|(\"Hello, Einstein\n\");");
}

/**
 Run an app on Einstein using the current Package Symbol.
 */
void
TToolkit::AppRun()
{
	PrintStd("Run...\n");
	TPlatformManager* mgr = mApp->GetPlatformManager();

	const char* cmd = "GetRoot().|%s|:Open();\n";
	char* buf = (char*) ::malloc(strlen(cmd) + 1024);
	sprintf(buf, cmd, mPkgSymbol);
	mgr->EvalNewtonScript(buf);
	// puts(buf);
	::free(buf);
}

/**
 Close any app on Einstein using the current Package Symbol.
 */
void
TToolkit::AppStop()
{
	PrintStd("Stop...\n");
	TPlatformManager* mgr = mApp->GetPlatformManager();

	const char* cmd = "if HasSlot(GetRoot(), '|%s|) then begin\n"
					  "  GetRoot().|%s|:Close();\n"
					  "end;\n";
	char* buf = (char*) ::malloc(strlen(cmd) + 1024);
	sprintf(buf, cmd, mPkgSymbol, mPkgSymbol);
	mgr->EvalNewtonScript(buf);
	// puts(buf);
	::free(buf);
}

/**
 Send a NewtonScrippt line to Einstein and run it there.

 \note Running NewtonScript this way is limited to 256 characters.

 \param cmd utf8 encoded NewtonScript
 */
void
TToolkit::AppCmd(const char* cmd)
{
	TPlatformManager* mgr = mApp->GetPlatformManager();
	mgr->EvalNewtonScript(cmd);
}

/**
 * Tell the Toolkit UI to redraw the titlebar.
 *
 * This adds an asterisk if the current script is dirty, menaing that it
 * is different than the saved vorsion of the script.
 */
void
TToolkit::UpdateTitle()
{
	char buf[2048];
	const char* filename = mCurrentScript->GetFilename();
	if (!filename)
		filename = "(no file)";
	if (mCurrentScript)
	{
		if (mCurrentScript->IsDirty())
		{
			snprintf(buf, 2048, "Einstein Toolkit - %s*", filename);
		} else
		{
			snprintf(buf, 2048, "Einstein Toolkit - %s", filename);
		}
		wToolkitWindow->copy_label(buf);
	} else
	{
		wToolkitWindow->label("Einstein Toolkit");
	}
}

/**
 Update the visibility and activity status of menu items in the menu bar.

 This is supposed to recreate the pulldown menus `Specific`, `Methods`, and
 `Attributes` in NTK. These menus provide quick access to slot prototypes
 that are useful in the context of the active proto. Slots that are already
 used are underlined.

 `NTK 1.6.4/Platforms/Newton 2.1` contains a NSOF database for all supported
 protos and menus.

 \todo we could find the active _proto by interpreting the script and finding
 the Frame around the current text cursor, interprete that frame, and find the
 proto. To make life easier for now, we will search backwards for the word
 `_proto:` and use the next word as the database lookup.

 \todo pre-read the 2.1 Platform database and store it within Toolkit.

 \todo We could also read the name of the current slot and offer an Edit
 menu that pops up an NTK style dialog box to edit parameters.

 \todo lastly, I would *love* to provide a quick link to the documentation for
 a given proto and/or slot.
 */
void
TToolkit::UpdateMenuBar()
{
}

/**
 Print text to the Toolkit terminal window.

 \param text utf8 encoded text
 */
void
TToolkit::PrintStd(const char* text)
{
	// TODO: insert at the current position, or in the line after, in case we are editing text?
	gTerminalBuffer->append(text);
	int c = gTerminalBuffer->length();
	c = gTerminalBuffer->line_start(c);
	wToolkitTerminal->insert_position(c);
	wToolkitTerminal->show_insert_position();
}

/**
 Print an error message to the Toolkit terminal window.

 \param text utf8 encoded text

 \todo text should have a different color than regular messages.
 */
void
TToolkit::PrintErr(const char* text)
{
	// TODO: highlight text that went to stderr vs. stdout
	PrintStd(text);
}

/**
 Print the contents of a file as an error message.

 This is used if the output from an operation was redirected to a file.

 \param filename full path and file name

 \note it would be better if we could differentiate between regular messages
 and error messages.
 */
void
TToolkit::PrintErrFile(const char* filename)
{
	FILE* f = fopen(filename, "rb");
	while (!feof(f))
	{
		char buf[FL_PATH_MAX];
		buf[0] = 0;
		fgets(buf, sizeof(buf), f);
		if (buf[0])
			PrintErr(buf);
	}
	fclose(f);
}

/**
 Decompile a `.pkg` file.

 Create a NewtonScript document by interpreting the content of a package file.

 This code calls newt/64 for decompilation, which ATM is still highly
 experimental. The resulting scipt will probably not run without modifications,
 and functions are currently represented in ByteCode instead of NewtonScript
 (albeit recompilable into working scripts).

 For better decompilation, please check out the great work of Jason Harper
 in \a ViewFrame.

 \param in_filename the path to a package file, or `nullptr` to pop up a file dialog box.
 \return -1 if the user canceled the operation.
 */
int
TToolkit::UserActionDecompilePkg(const char* in_filename)
{
	if (UserActionClose() < 0)
		return -1;
	const char* filename = in_filename;
	if (filename == nullptr || *filename == 0)
	{
		filename = fl_file_chooser("Select a Newton Package file", "Package (*.pkg)", nullptr);
		if (!filename)
			return -1;
	}

	// FIXME: the string must not be longer than 255 characters!
	const char* cmd = "global _STDERR_ := \"\";\n"
					  "global _STDOUT_ := \"\";\n"
					  "printDepth := 9999;\n"
					  "printLength := 9999;\n"
					  "printBinaries := 1;\n"
					  "printUnique := 1;\n"
					  "pkg := ReadPkg(LoadBinary(\"%s\"));\n"
					  "print(\"//\\n// This NewtonScript code was created by decompiling\\n\");\n"
					  "print(\"// %s\\n//\\n\\n\");\n"
					  "print(\"newt.app := \\n\");\n"
					  "p(pkg);\n"
					  "print(\";\\n\");\n";
	char* buf = (char*) ::malloc(strlen(cmd) + 2 * FL_PATH_MAX);
	sprintf(buf, cmd, filename, filename);

	//  newtRefVar result;
	newtErr err;
	static const char* argv[] = { "Einstein", nullptr };
	int argc = 1;
	NewtInit(argc, argv, 0);

	NsUndefGlobalVar(kNewtRefNIL, NSSYM0(_STDERR_));
	NcDefGlobalVar(NSSYM0(_STDERR_), NewtMakeString("", false));
	NsUndefGlobalVar(kNewtRefNIL, NSSYM0(_STDOUT_));
	NcDefGlobalVar(NSSYM0(_STDOUT_), NewtMakeString("", false));
	/* result =*/NVMInterpretStr(buf, &err);

	::free(buf);

	newtRef outRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDOUT_));
	if (NewtRefIsString(outRef))
	{
		const char* outStr = NewtRefToString(outRef);
		mCurrentScript->SetSourceCode(outStr);
	}

	newtRef errRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDERR_));
	if (NewtRefIsString(errRef))
	{
		const char* errStr = NewtRefToString(errRef);
		PrintErr(errStr);
	}

	NewtCleanup();

	return 0;
}

extern "C" void
yyerror(char* s)
{
	if (s[0] && s[1] == ':')
		NPSErrorStr(*s, s + 2);
	else
		NPSErrorStr('E', s);
}

/**
 Load one of the many sample source code files.

 As of Feb 2022, there is exactly one NewtonScript example.

 \param n index into the list of sampel codes.
 */
void
TToolkit::LoadSampleCode(int n)
{
	switch (n)
	{
		case 1:
			wScriptPanel->SetSourceCode(kToolkitSampleScriptHelloWorld);
			wScriptPanel->ClearDirty();
			break;
		case 2:
			wScriptPanel->SetSourceCode(kToolkitSampleScriptNativeFunction);
			wScriptPanel->ClearDirty();
			break;
	}
}

/**
 Create a temporary package file name for compiling unnnamed Script files.

 The result is not returned, but by calling `SetPkgPath()`.

 \see SetPkgPath()
 */
void
TToolkit::SetTempPkgPath()
{
	char buf[FL_PATH_MAX];
	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
	prefs.getUserdataPath(buf, FL_PATH_MAX);
	strcat(buf, "tmp.pkg");
	SetPkgPath(buf);
	::remove(buf);
}

/**
 Set the file path to the package file for this project.
 This is usually created using the source file path and replacing the file
 extension with `.pkg`.
 \param name file path and name.
 */
void
TToolkit::SetPkgPath(const char* name)
{
	if (name == mPkgPath)
		return;
	if (mPkgPath)
		::free(mPkgPath);
	mPkgPath = nullptr;
	if (name)
		mPkgPath = strdup(name);
}

/**
 Set the name of the package, see `newt.app.name`.
 \param name a name for the package
 */
void
TToolkit::SetPkgName(const char* name)
{
	if (name == mPkgName)
		return;
	if (mPkgName)
		::free(mPkgName);
	mPkgName = nullptr;
	if (name)
		mPkgName = strdup(name);
}

/**
 Set the symbol of the package, see `newt.app.parts[0].data.app`.
 \param name a symbol for the package
 */
void
TToolkit::SetPkgSymbol(const char* name)
{
	if (name == mPkgSymbol)
		return;
	if (mPkgSymbol)
		::free(mPkgSymbol);
	mPkgSymbol = nullptr;
	if (name)
		mPkgSymbol = strdup(name);
}

/**
 Set the label of the package, see `newt.app.parts[0].data.text`.
 \param name a label for the package
 */
void
TToolkit::SetPkgLabel(const char* name)
{
	if (name == mPkgLabel)
		return;
	if (mPkgLabel)
		::free(mPkgLabel);
	mPkgLabel = nullptr;
	if (name)
		mPkgLabel = strdup(name);
}

/**
 * Extract project settings from the newt environment.
 * \return -1 if any resource is missing and print the reason to the terminal
 */
int
TToolkit::ReadScriptResults()
{
	newtRef newt = NsGetGlobalVar(kNewtRefNIL, NSSYM(newt));
	if (!NewtRefIsFrame(newt))
	{
		PrintErr("Error: can't build package, 'newt' not defined!\n");
		return -1;
	}
	// if newt.pkPath is a string, the package will be written to this path
	newtRef pkgPath = NsGetSlot(kNewtRefNIL, newt, NSSYM(pkgPath));
	if (NewtRefIsString(pkgPath))
	{
		const char* pkgPathStr = NewtRefToString(pkgPath);
		SetPkgPath(pkgPathStr);
	}
	// check if we the user has created an app
	newtRef app = NsGetSlot(kNewtRefNIL, newt, NSSYM(app));
	if (!NewtRefIsFrame(app))
	{
		PrintErr("Error: can't build package, 'newt.app' not defined!\n");
		return -1;
	}
	newtRef pkgName = NsGetSlot(kNewtRefNIL, app, NSSYM(name));
	if (NewtRefIsString(pkgName))
	{
		const char* pkgNameStr = NewtRefToString(pkgName);
		SetPkgName(pkgNameStr);
	} else
	{
		PrintErr("Error: can't build package, 'newt.app.name' not defined!\n");
		return -1;
	}
	newtRef pkgParts = NsGetSlot(kNewtRefNIL, app, NSSYM(parts));
	if (!NewtRefIsArray(pkgParts))
	{
		PrintErr("Error: can't build package, 'newt.app.parts' not defined!\n");
		return -1;
	}
	newtRef pkgPart0 = NewtGetArraySlot(pkgParts, 0);
	if (!NewtRefIsFrame(pkgPart0))
	{
		PrintErr("Error: can't build package, 'newt.app.parts[0]' not defined!\n");
		return -1;
	}
	newtRef data = NsGetSlot(kNewtRefNIL, pkgPart0, NSSYM(data));
	if (!NewtRefIsFrame(data))
	{
		PrintErr("Error: can't build package, 'newt.app.parts[0].data' not defined!\n");
		return -1;
	}
	newtRef pkgSymbol = NsGetSlot(kNewtRefNIL, data, NSSYM(app));
	if (NewtRefIsSymbol(pkgSymbol))
	{
		const char* pkgSymbolStr = NewtSymbolGetName(pkgSymbol);
		SetPkgSymbol(pkgSymbolStr);
	} else
	{
		PrintErr("Error: can't build package, package symbol not defined! Expected in 'newt.app.parts[0].data'.\n");
		return -1;
	}
	newtRef pkgLabel = NsGetSlot(kNewtRefNIL, data, NSSYM(text));
	if (NewtRefIsString(pkgLabel))
	{
		const char* pkgLabelStr = NewtRefToString(pkgLabel);
		SetPkgLabel(pkgLabelStr);
	} else
	{
		SetPkgLabel("<unknown>");
	}
	PrintStd("Info: package compiled.\n");
	return 0;
}

/**
 Called whenever the user changes the search text.
 */
void
TToolkit::UserActionFindTextChanged()
{
	if (Fl::event() == FL_KEYBOARD && Fl::event_key() == FL_Enter)
	{
		UserActionFindNext(true);
		int p = wToolkitFindText->position();
		wToolkitFindText->take_focus();
		wToolkitFindText->position(p);
	} else
	{
		UserActionFindNext(false);
	}
}

/**
 Called whenever the user changes the replacement text.
 */
void
TToolkit::UserActionReplaceTextChanged()
{
	if (Fl::event() == FL_KEYBOARD && Fl::event_key() == FL_Enter)
	{
		UserActionReplaceNext();
		int p = wToolkitReplaceText->position();
		wToolkitReplaceText->take_focus();
		wToolkitReplaceText->position(p);
	}
}

/**
 Called whenever the user changes search parameters.
 */
void
TToolkit::UserActionCaseChanged()
{
	// empty
	// TODO: UserActionFindNext() ?
}

/**
 Called whenever the user changes search parameters.
 */
void
TToolkit::UserActionRegexChanged()
{
	// not yet used
	// TODO: UserActionFindNext() ?
}

/**
 User wants to search backward.
 */
void
TToolkit::UserActionFindPrev()
{
	TFLScriptEditor* editor = wScriptPanel->GetEditor();
	Fl_Text_Buffer* buffer = editor->buffer();

	const char* findText = wToolkitFindText->value();
	if (findText == nullptr || *findText == 0)
		return;

	int pos, dummy;
	if (!buffer->selection_position(&pos, &dummy))
		pos = editor->insert_position();

	int found = 0;
	if (pos > 0)
		found = buffer->search_backward(pos - 1, findText, &pos, wToolkitFindCase->value());

	// if not found to beginning of ile, wrap
	if (!found)
		found = buffer->search_backward(buffer->length(), findText, &pos, wToolkitFindCase->value());

	// if found, select and set the cursor
	if (found)
	{
		buffer->select(pos, pos + strlen(findText));
		editor->insert_position(pos);
		editor->show_insert_position();
	}
}

/**
 User wants to search forward.
 \param fromLast if true, continue search, if fals, start at the beginning of the file.
 \todo fromLast should probably start from the cursor position when the searach
 was oriiginally started.
 */
bool
TToolkit::UserActionFindNext(bool fromLast)
{
	TFLScriptEditor* editor = wScriptPanel->GetEditor();
	Fl_Text_Buffer* buffer = editor->buffer();

	const char* findText = wToolkitFindText->value();
	if (findText == nullptr || *findText == 0)
		return false;

	int pos, first, last;
	int selected = buffer->selection_position(&first, &last);
	if (selected)
		pos = fromLast ? last : first;
	else
		pos = editor->insert_position();

	int found = buffer->search_forward(pos, findText, &pos, wToolkitFindCase->value());

	// if not found to end of ile, wrap
	if (!found)
		found = buffer->search_forward(0, findText, &pos, wToolkitFindCase->value());

	// if found, select and set the cursor
	if (found)
	{
		buffer->select(pos, pos + strlen(findText));
		editor->insert_position(pos + strlen(findText));
		editor->show_insert_position();
	}
	return (found == 1);
}

/**
 Close the \a search dialog box
 */
void
TToolkit::UserActionFindClose()
{
	// TODO: not yet
}

/**
 User wants to replace the ext occurence of the search string.
 */
void
TToolkit::UserActionReplaceNext()
{
	TFLScriptEditor* editor = wScriptPanel->GetEditor();
	Fl_Text_Buffer* buffer = editor->buffer();
	const char* replaceText = wToolkitReplaceText->value();

	if (UserActionFindNext(false))
	{
		int first, last;
		if (buffer->selection_position(&first, &last))
		{
			buffer->remove_selection();
		} else
		{
			first = last = editor->insert_position();
		}
		buffer->insert(first, replaceText);
		editor->insert_position(first + strlen(replaceText));
		UserActionFindNext();
	}
}

/**
 User wants to replace all occurences of the search string.
 */
void
TToolkit::UserActionReplaceAll()
{
	TFLScriptEditor* editor = wScriptPanel->GetEditor();
	Fl_Text_Buffer* buffer = editor->buffer();

	const char* findText = wToolkitFindText->value();
	if (findText == nullptr || *findText == 0)
		return;

	const char* replaceText = wToolkitReplaceText->value();
	int replLen = strlen(replaceText);
	int pos = 0;
	int line = buffer->count_lines(0, editor->insert_position());
	int col = editor->insert_position() - buffer->line_start(editor->insert_position());

	for (;;)
	{
		int found = buffer->search_forward(pos, findText, &pos, wToolkitFindCase->value());
		if (!found)
			break;
		buffer->select(pos, pos + strlen(findText));
		buffer->remove_selection();
		buffer->insert(pos, replaceText);
		pos = pos + replLen;
	}
	pos = buffer->skip_lines(0, line);
	int max = buffer->line_end(pos);
	if (pos + col > max)
		pos = max;
	else
		pos = pos + col;
	pos = buffer->utf8_align(pos);
	editor->insert_position(pos);
	editor->show_insert_position();
}

/*

 TemplateArray: [
 'protoFloatNGo,
 {
 _proto: 180,
 __ntAncestor: 'protoFloater,
 __ntRequired: {
 viewBounds: "RECT"
 },
 __ntOptional: {
 noScroll: "EVAL",
 noOverview: "EVAL",
 viewEffect: "NUMB"
 }
 },


 -e 'printLength:=9999;printDepth:=1;p(ReadNSOF(LoadBinary("/Users/matt/dev/Newton/NewtonDev/NewtonDev/NTK 1.6.4/Platforms/Newton 2.1")) )'
*/

void
testNSOFReader(const char* filename)
{
	if (!filename)
		filename = fl_file_chooser("Load NSOF File", "*", 0L);
	if (!filename)
		return;

	const char* argv[] = { "Einstein" };
	NewtInit(1, argv, 0);

	uint8_t* buffer;
	FILE* f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int nn = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = (uint8_t*) malloc(nn);
	int n = fread(buffer, 1, nn, f);
	fclose(f);
	if (n)
	{
		NcSetGlobalVar(NSSYM(printLength), NSINT(9999));
		NcSetGlobalVar(NSSYM(printDepth), NSINT(12));
		NEWT_DUMPBC = 0;
		NEWT_INDENT = -2;

		FILE* g = fopen("dump_nsof.txt", "wb");
		newtRef pkg = NewtReadNSOF(buffer, n);
		NewtPrintObject(g, pkg);
		fclose(g);
	}
}

// ======================================================================= //
// Newton Toolkit
//
// A development environment created by Apple for writing NewtonScript programs
// for Newton OS. A prototype for Newton Toolkit called EtchASketch was written
// using Macintosh Common Lisp. Also referred to as NTK.
//
//   -- Newton Glossary
// ======================================================================= //
