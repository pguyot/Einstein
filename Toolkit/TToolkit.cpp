// ==============================
// File:			TFLToolkit.cp
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

// TODO: spontaneous, improvised, casual, experimental, inpromptu <> planned, formal, serious, orderly, systematic
// TODO: Build, Install, Run, Stop must know the state we are in to avoid building twice
// TODO: Horizontal scrollbar in Inspector must go
// TODO: ScriptEditor needs its own class and Find/Replace, Cut/Copy/Paste, etc.
// TODO: Better syntax highlighting

/* NOTE:
 This is where the MP2100US ROM interpretes bytecodes "fast"
    cmp     r0, #207                    @ [ 0x000000CF ] 0x002EE1DC 0xE35000CF - .P..
 and this is the slow version:
    cmp     r2, #207                    @ [ 0x000000CF ] 0x002F2028 0xE35200CF - .R..
 We can add a new BC command, BC26 (0xD0nnnn), where nnnn is the line in the
 source code. This would allow the debugger to show where bytecode execution is
 at right now. We should have a BC27 at the start of every bytecode stream that
 gives us an index to teh source file, and an array of source files at the start
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
#include "TFLToolkitUI.h"
#include "TFLScriptPanel.h"
#include "TTkScript.h"

#define IGNORE_TNEWT
#include "Emulator/Platform/TPlatformManager.h"
#undef IGNORE_TNEWT

#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtVM.h"
#include "NewtParser.h"

#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>

Fl_Text_Buffer *gTerminalBuffer = nullptr;

TToolkit *gToolkit = nullptr;


void TToolkit::Show()
{
    if (!wToolkitWindow) {
        Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
        Fl_Preferences toolkit(prefs, "Toolkit");
        Fl_Preferences tkWindow(toolkit, "Window");
        int x, y, w, h;
        tkWindow.get("x", x, 400);
        tkWindow.get("y", y, 80);
        tkWindow.get("w", w, 720);
        tkWindow.get("h", h, 600);

        wToolkitWindow = CreateToolkitWindow(x, y);
        wToolkitWindow->resize(x, y, w, h);

        wToolkitTerminal->buffer( gTerminalBuffer = new Fl_Text_Buffer() );

        // FIXME: allow multiple scripts and multiple panels in a Tab, and a hierarchy of scripts in a Project
        mScript = new TTkScript(this);
        wScriptPanel->SetScript(mScript);
        mScript->SetPanel( wScriptPanel );
        mScript->LoadFile("/Users/matt/dev/newton-test/mini.ns");
    }
    UpdateTitle();
    wToolkitWindow->show();
}


void TToolkit::Hide()
{
    Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
    Fl_Preferences toolkit(prefs, "Toolkit");
    Fl_Preferences tkWindow(toolkit, "Window");
    if (wToolkitWindow) {
        tkWindow.set("x", wToolkitWindow->x());
        tkWindow.set("y", wToolkitWindow->y());
        tkWindow.set("w", wToolkitWindow->w());
        tkWindow.set("h", wToolkitWindow->h());
        wToolkitWindow->hide();
    }
}


/**
 * Create a new NewtonScript file.
 *
 * Present a file chooser to the user to set the filename of a new NewtonScript file.
 * Do not create the file, but create an editor with minimal startup text and the filename set
 * and marked dirty.
 */
int TToolkit::UserActionNew()
{
    int ret = UserActionClose();
    if (ret<0)
        return ret;

    const char *filename = fl_file_chooser("New NewtonScript File",
                                           "NewtonScript (*.{ns,nscript,script})",
                                           nullptr);
    if (!filename)
        return -1;

    // set script to the new filename and load the file
    mScript->SetFilename(filename);
    FILE *f = fopen(filename, "rb");
    if (f) {
        fclose(f);
        ret = fl_choice("File already exists.\n\n"
                        "Do you want to overwrite the existing file\n"
                        "with a new, empty script?",
                        "Open existing Script", "Overwrite Script", "Abort");
        if (ret==0) return -1;
        if (ret==1) mScript->SetDirty();
        if (ret==2) mScript->LoadFile(filename);
    }
    return 0;
}


/**
 * Close the current file and open a new one from disk.
 *
 * Close the current file, notifying the user if it is dirty.
 *
 * Present a file choose to load another existing file from disk.
 */
int TToolkit::UserActionOpen()
{
    int ret = UserActionClose();
    if (ret<0)
        return ret;

    const char *filename = fl_file_chooser("Open NewtonScript File",
                                           "NewtonScript (*.{ns,nscript,script})",
                                           nullptr);
    if (!filename)
        return -1;

    // set script to the new filename and load the file
    mScript->SetFilename(filename);
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fl_alert("Error reading file\n%s\n%s", mScript->GetFilename(), strerror(errno));
    } else {
        fclose(f);
        mScript->LoadFile(filename);
    }
    return 0;
}


/**
 * Write this file to disk.
 *
 * If there is no filename given, ask the user for a filename.
 */
int TToolkit::UserActionSave()
{
    int result = 0;
    if (mScript->GetFilename()) {
        // we have a filename, so just save the file to disk
        result = mScript->Save();
        if (result<0) {
            fl_alert("Error writing file\n%s\n%s", mScript->GetFilename(), strerror(errno));
        }
    } else {
        // if there is no filename given, run SaveAs instead
        return UserActionSaveAs();
    }
    return result;
}


/**
 * Ask the user for a filename, change to the new name, and save the file to disk,
 */
int TToolkit::UserActionSaveAs()
{
    // TODO: Fix FLTK to allow selecting new & existing files in OS X
    const char *filename = fl_file_chooser("Save NewtonScript As...",
                                           "NewtonScript (*.{ns,nscript,script})",
                                           mScript->GetFilename());
    if (!filename)
        return -1;
    mScript->SetFilename(filename);
    mScript->SetDirty();
    return UserActionSave();
}


/**
 * Close the current script file.
 *
 * This leaves the user with an empty canvas. Users can simply type away and
 * write NewtonScript commands. They can even them from memory by choosing "Buidl"
 * and install them if the happen to create a package (Toolkit will assign a temp file).
 */
int TToolkit::UserActionClose()
{
    if (mScript->IsDirty()) {
        int ret = fl_choice(
                            "Unsaved changes.\n\n"
                            "Do you want to save your changes before\n"
                            "closing this script?",
                            "Return to Script", "Save Script", "Discard Script");
        if (ret==0) return -1;
        if (ret==1) if (UserActionSave()<0) return -1;
    }
    // set script to empty and filename to null
    mScript->SetSourceCode("");
    mScript->SetFilename(nullptr);
    return 0;
}


/**
 * Close the app.
 *
 * If the script is dirty, try to save it first. If the user cancels, cacel the Quit action.
 */
void TToolkit::UserActionQuit()
{
    if (UserActionClose()==0)
        gApp->UserActionQuit();
}


void TToolkit::UserActionUndo() { }
void TToolkit::UserActionRedo() { }
void TToolkit::UserActionCut() { }
void TToolkit::UserActionCopy() { }
void TToolkit::UserActionPaste() { }
void TToolkit::UserActionFind() { }


void TToolkit::UserActionBuild()
{
    // TODO: if source is dirty, save
    // TODO: is source is newer than package, build
    // TODO: handle errors
    AppBuild();
}

void TToolkit::UserActionInstall()
{
    // TODO: if source is dirty, save
    // TODO: is source is newer than package, build
    // TODO: handle errors
    AppBuild();
    // Always install if the pkg exists
    // TODO: handle errors
    AppInstall();
}

void TToolkit::UserActionRun()
{
    // TODO: if source is dirty, save
    // TODO: is source is newer than package, build
    // TODO: handle errors
    AppBuild();
    // TODO: if newly build, install
    AppInstall();
    // TODO: erroro, if we can;t run the app
    AppRun();
}

void TToolkit::UserActionStop()
{
    AppStop();
}


void TToolkit::AppBuild()
{
    gTerminalBuffer->append("Compiling...\n");
    newtRefVar    result;
    newtErr    err;
    NewtInit(0, 0L, 0);
    //    newt_chdir();
    //result = NVMInterpretFile("/Users/matt/dev/newton-test/mini.ns", &err);

    char *sourceCode = mScript->DupSourceCode();
    result = NVMInterpretStr(sourceCode, &err);
    free(sourceCode);


    //    newt_result_message(result, err);
    NewtCleanup();
}


void TToolkit::AppInstall()
{
    gTerminalBuffer->append("Installing...\n");
    TPlatformManager *mgr = mApp->GetPlatformManager();
    mgr->EvalNewtonScript(
                          "if HasSlot(GetRoot(), '|PictIndex:PIEDTS|) then begin\n"
                          "  GetRoot().|PictIndex:PIEDTS|:Close();\n"
                          "  SafeRemovePackage(GetPkgRef(\"PictIndex:PIEDTS\", GetStores()[0]))\n"
                          "end;\n"
                          );
    mApp->InstallPackagesFromURI("/Users/matt/dev/newton-test/mini.pkg");
}


void TToolkit::AppRun()
{
    gTerminalBuffer->append("Run...\n");
    TPlatformManager *mgr = mApp->GetPlatformManager();
    mgr->EvalNewtonScript(
                          "GetRoot().|PictIndex:PIEDTS|:Open();\n"
                          );
}


void TToolkit::AppStop()
{
    gTerminalBuffer->append("Stop...\n");
    TPlatformManager *mgr = mApp->GetPlatformManager();
    mgr->EvalNewtonScript(
                          "if HasSlot(GetRoot(), '|PictIndex:PIEDTS|) then begin\n"
                          "  GetRoot().|PictIndex:PIEDTS|:Close();\n"
                          "end;\n"
                          );
}


TToolkit::TToolkit(TFLApp *inApp)
:   mApp(inApp)
{
    gToolkit = this;
}


TToolkit::~TToolkit()
{
    delete mScript;
}

/**
 * Tell the Toolkit UI to redraw the titlebar.
 */
void TToolkit::UpdateTitle()
{
    char buf[2048];
    const char *filename = mScript->GetFilename();
    if (!filename) filename = "<memory>";
    if (mScript) {
        if (mScript->IsDirty()) {
            snprintf(buf, 2048, "Einstein Toolkit - %s*", filename);
        } else {
            snprintf(buf, 2048, "Einstein Toolkit - %s", filename);
        }
        wToolkitWindow->copy_label(buf);
    } else {
        wToolkitWindow->label("Einstein Toolkit");
    }
}

extern "C" void yyerror(char * s)
{
    if (s[0] && s[1] == ':')
        NPSErrorStr(*s, s + 2);
    else
        NPSErrorStr('E', s);
}


void TToolkit::LoadSampleCode(int n)
{
#if 0
    wToolkitEditor->buffer()->text
    (
     "//#include \"/Users/matt/dev/newt64/defs/newt.2.2.ns\"\n"
     "//#include \"/Users/matt/dev/newt64/defs/newt.bc.2.2.ns\"\n"
     "\n"
     "helloButton := {\n"
     "    text: \"Say Hello\",\n"
     "    viewBounds: {\n"
     "        left: 42, top: 82, right: 210, bottom: 138\n"
     "    },\n"
     "    buttonClickScript: func()\n"
     "        begin\n"
     // TODO: pop up a dialog
     "        end,\n"
     "    _proto: @226 /* protoTextButton */\n"
     "};\n"
     "\n"
     "mainWindow := {\n"
     "    viewBounds: {\n"
     "        left: -16, top: 38, right: 212, bottom: 322\n"
     "    },\n"
     "    stepChildren: [\n"
     "        helloText,\n"
     "    },\n"
     "    _proto: @180 /* protoFloatNGo */\n"
     "};\n"
     "\n"
     "app := {\n"
     "    class: 'PackageHeader,\n"
     "    type: 2021161080,\n"
     "    pkg_version: 0,\n"
     "    version: 1,\n"
     "    copyright: \"Written in 2020 by Matthias Melcher, licensed under GPL.\",\n"
     "    name: \"Hello:WONKO\",\n"
     "    flags: 0,\n"
     "    parts: [\n"
     "        {\n"
     "            class: 'PackagePart,\n"
     "            info: MakeBinaryFromHex(\"41204E6577746F6E20546F6F6C6B6974206170706C69636174696F6E\", 'binary),\n"
     "            flags: 129,\n"
     "            type: 1718579821,\n"
     "            align: 8,\n"
     "            data: {\n"
     "                app: '|Hello:WONKO|,\n"
     "                text: \"Hello\",\n"
     "                icon: {\n"
     "                    mask: MakeBinaryFromHex(\"000000000004000300030005001F001A007000000070000007FF00001FFFC0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0003FFFE0007FFFF0007FFFF0007FFFF000\", 'mask),\n"
     "                    bits: MakeBinaryFromHex(\"000000000004000300030005001F001A007000000070000007FF00001FFFC0003FFFE0003B76E0003B76E0003FFFE0003B76E0003B76E0003FFFE0003D75E0003AFAE0003D75E0003AFAE0003FFFE0003D75E0003AFAE0003D75E0003AFAE0003FFFE0003D75E0003AFAE0003D75E0003AFAE0007FFFF0007FFFF0007FFFF000\", 'bits),\n"
     "                    bounds: {\n"
     "                        left: 0, top: 0, right: 21, bottom: 28\n"
     "                    }\n"
     "                },\n"
     "                theForm: mainWindow\n"
     "            },\n"
     "        }\n"
     "    ]\n"
     "    InstallScript: func(partFrame)\n"
     "    begin\n"
     "        partFrame.removeFrame := (partFrame.theForm):NewtInstallScript(partFrame.theForm);\n"
     "    end;\n"
     "    RemoveScript: func(partFrame)\n"
     "    begin\n"
     "        (partFrame.removeFrame):NewtRemoveScript(removeFrame);\n"
     "    end;\n"
     "};\n"
     "\n"
     "p(\"Build package...\");\n"
     "pkg := MakePkg(app);\n"
     "p(\"Save Package...\");\n"
     "SaveBinary(pkg, \"/Users/matt/dev/newton-test/mini.pkg\");\n"
     "p(\"DONE\");\n"
     );
#endif
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
