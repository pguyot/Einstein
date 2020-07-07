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
#include "TToolkitPrototypes.h"

#define IGNORE_TNEWT
#include "Emulator/Platform/TPlatformManager.h"
#undef IGNORE_TNEWT

extern "C" {
#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtVM.h"
#include "NewtParser.h"
#include "NewtPkg.h"
#include "NewtEnv.h"
}

#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>

#include <errno.h>


Fl_Text_Buffer *gTerminalBuffer = nullptr;

TToolkit *gToolkit = nullptr;

TToolkit::TToolkit(TFLApp *inApp)
:   mApp(inApp)
{
    gToolkit = this;
}


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


void TToolkit::Show()
{
    if (!wToolkitWindow) {
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
        wToolkitWindow->size_range(150, 78+150);

        wToolkitTerminal->buffer( gTerminalBuffer = new Fl_Text_Buffer() );
        wToolkitTerminal->scrollbar_align(FL_ALIGN_RIGHT);

        // FIXME: allow multiple scripts and multiple panels in a Tab, and a hierarchy of scripts in a Project
        mCurrentScript = new TTkScript(this);
        wScriptPanel->SetScript(mCurrentScript);
        mCurrentScript->SetPanel( wScriptPanel );
        mCurrentScript->SetSourceCode(TToolkitPrototype::HelloWorld);
        //mCurrentScript->LoadFile("/Users/matt/dev/newton-test/mini.ns");

        wToolkitWindow->show();
        wToolkitWindow->resize(x, y, w, h);
        wTile->init_sizes();
        wTile->position(0, wToolkitTerminal->y(), 0, hTileSep);
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
    mCurrentScript->SetFilename(filename);
    FILE *f = fopen(filename, "rb");
    if (f) {
        fclose(f);
        ret = fl_choice("File already exists.\n\n"
                        "Do you want to overwrite the existing file\n"
                        "with a new, empty script?",
                        "Open existing Script", "Overwrite Script", "Abort");
        if (ret==0) return -1;
        if (ret==1) mCurrentScript->SetDirty();
        if (ret==2) mCurrentScript->LoadFile(filename);
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
    mCurrentScript->SetFilename(filename);
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fl_alert("Error reading file\n%s\n%s", mCurrentScript->GetFilename(), strerror(errno));
    } else {
        fclose(f);
        mCurrentScript->LoadFile(filename);
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
    if (mCurrentScript->GetFilename()) {
        // we have a filename, so just save the file to disk
        result = mCurrentScript->Save();
        if (result<0) {
            fl_alert("Error writing file\n%s\n%s", mCurrentScript->GetFilename(), strerror(errno));
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
                                           mCurrentScript->GetFilename());
    if (!filename)
        return -1;
    mCurrentScript->SetFilename(filename);
    mCurrentScript->SetDirty();
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
    if (mCurrentScript->IsDirty()) {
        int ret = fl_choice(
                            "Unsaved changes.\n\n"
                            "Do you want to save your changes before\n"
                            "closing this script?",
                            "Return to Script", "Save Script", "Discard Script");
        if (ret==0) return -1;
        if (ret==1) if (UserActionSave()<0) return -1;
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

#include <string>
#include <fstream>
#include <streambuf>

// This is a simple function because newt/64 stores strings internally as UTF8 already
/**
 * The 'info' element in the Package header wants a binary object containing ASCII without a trailing 0.
 */
static newtRef NewtMakeBinaryFromString(newtRefArg klass, const char *text, bool literal)
{
    uint32_t size = (uint32_t)strlen(text);
    newtRef obj = NewtMakeBinary(klass, 0, size, literal);
    if (obj) {
        uint8_t *dst = NewtRefToBinary(obj);
        memcpy(dst, text, size);
        return obj;
    }
    return kNewtRefUnbind;
}

static newtRef NsMakeBinaryFromString(newtRefArg rcvr, newtRefArg text, newtRefArg klass)
{
    if (!NewtRefIsString(text))
        return NewtThrow(kNErrNotAString, text);
    return NewtMakeBinaryFromString(klass, NewtRefToString(text), false);
}


void TToolkit::AppBuild()
{
    wToolkitTerminal->buffer()->text("");

    char buf[2*FL_PATH_MAX];
    newtRefVar result;
    newtErr err;
    const char* argv[] = { "Einstein" };
    NewtInit(1, argv, 0);

    NewtDefGlobalFunc0(NSSYM(MakeBinaryFromString), (void*)NsMakeBinaryFromString, 2, false, (char*)"MakeBinaryFromString(str, sym)");
    NcDefGlobalVar(NSSYM0(_STDERR_), NewtMakeString("", false));
    NcDefGlobalVar(NSSYM0(_STDOUT_), NewtMakeString("", false));

    // #file ...
    // #line 1
    std::string src;
    src.append( TToolkitPrototype::NewtonDefs21 );
    src.append( TToolkitPrototype::BytecodeDefs );
    src.append( TToolkitPrototype::ToolkitDefs );
    src.append( TToolkitPrototype::DefaultPackage );
    SetTempPkgPath();
    sprintf(buf, "newt.pkgPath := \"%s\";\n", mPkgPath);
    src.append( buf );
    src.append( TToolkitPrototype::ToolkitLaunch );

    if (mCurrentScript->GetFilename()) {
        PrintStd("Compiling file...\n");
        if (mCurrentScript->IsDirty()) {
            mCurrentScript->Save();
        }
        std::ifstream t(mCurrentScript->GetFilename());
        std::string str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
        src.append(str);
    } else {
        PrintStd("Compiling inline...\n");
        char *sourceCode = mCurrentScript->DupSourceCode();
        src.append(sourceCode);
        free(sourceCode);
    }
    src.append( TToolkitPrototype::ToolkitDone );

    // FIXME: somewhere inside this call, _STDERR_ and _STDOUT_ are cleared by Newt/64
    result = NVMInterpretStr(src.c_str(), &err);

//    puts(src.c_str());

    // TODO: get the app symbol to install and uninstall it
    // TODO: get the app name
    // TODO: get the package path, or build a temp package

    newtRef outRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDOUT_));
    if (NewtRefIsString(outRef)) {
        const char *outStr = NewtRefToString(outRef);
        PrintStd(outStr);
    }

    newtRef errRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDERR_));
    if (NewtRefIsString(errRef)) {
        const char *errStr = NewtRefToString(errRef);
        PrintErr(errStr);
    }

    if (ReadScriptResults()==0) {
        newtRef newt = NsGetGlobalVar(kNewtRefNIL, NSSYM(newt));
        NcSend0(newt, NSSYM(writePkg));
    }
    NcDefGlobalVar(NSSYM0(_STDERR_), NewtMakeString("", false));
    NcDefGlobalVar(NSSYM0(_STDOUT_), NewtMakeString("", false));
    outRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDOUT_));
    if (NewtRefIsString(outRef)) {
        const char *outStr = NewtRefToString(outRef);
        PrintStd(outStr);
    }
    errRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDERR_));
    if (NewtRefIsString(errRef)) {
        const char *errStr = NewtRefToString(errRef);
        PrintErr(errStr);
    }
    //PrintErr(mPkgPath);

    NewtCleanup();
}


void TToolkit::AppInstall()
{
    PrintStd("Installing...\n");
    TPlatformManager *mgr = mApp->GetPlatformManager();

    const char *cmd =
    "if HasSlot(GetRoot(), '|%s|) then begin\n"
    "  GetRoot().|%s|:Close();\n"
    "  SafeRemovePackage(GetPkgRef(\"%s\", GetStores()[0]))\n"
    "end;\n";

    char *buf = (char*)::malloc(strlen(cmd)+1024);
    sprintf(buf, cmd, mPkgSymbol, mPkgSymbol, mPkgName);
    mgr->EvalNewtonScript(buf);
    //puts(buf);
    ::free(buf);

    mApp->InstallPackagesFromURI(mPkgPath);
}


void TToolkit::AppRun()
{
    PrintStd("Run...\n");
    TPlatformManager *mgr = mApp->GetPlatformManager();

    const char *cmd = "GetRoot().|%s|:Open();\n";
    char *buf = (char*)::malloc(strlen(cmd)+1024);
    sprintf(buf, cmd, mPkgSymbol);
    mgr->EvalNewtonScript(buf);
    puts(buf);
    ::free(buf);
}


void TToolkit::AppStop()
{
    PrintStd("Stop...\n");
    TPlatformManager *mgr = mApp->GetPlatformManager();

    const char *cmd =
    "if HasSlot(GetRoot(), '|%s|) then begin\n"
    "  GetRoot().|%s|:Close();\n"
    "end;\n";
    char *buf = (char*)::malloc(strlen(cmd)+1024);
    sprintf(buf, cmd, mPkgSymbol, mPkgSymbol);
    mgr->EvalNewtonScript(buf);
    puts(buf);
    ::free(buf);
}


/**
 * Tell the Toolkit UI to redraw the titlebar.
 */
void TToolkit::UpdateTitle()
{
    char buf[2048];
    const char *filename = mCurrentScript->GetFilename();
    if (!filename) filename = "(no file)";
    if (mCurrentScript) {
        if (mCurrentScript->IsDirty()) {
            snprintf(buf, 2048, "Einstein Toolkit - %s*", filename);
        } else {
            snprintf(buf, 2048, "Einstein Toolkit - %s", filename);
        }
        wToolkitWindow->copy_label(buf);
    } else {
        wToolkitWindow->label("Einstein Toolkit");
    }
}

void TToolkit::PrintStd(const char *text)
{
    gTerminalBuffer->append(text);
    int c = gTerminalBuffer->length();
    c = gTerminalBuffer->line_start(c);
    wToolkitTerminal->insert_position(c);
    wToolkitTerminal->show_insert_position();
}

void TToolkit::PrintErr(const char *text)
{
    // TODO: highlight text that went to stderr vs. stdout
    PrintStd(text);
}


int TToolkit::UserActionDecompilePkg()
{
    if (UserActionClose()<0)
        return -1;
    char *filename = fl_file_chooser("Select a Newton Package file", "Package (*.pkg)", nullptr);
    if (!filename)
        return -1;

    const char *cmd =
    "global _STDERR_ := \"\";\n"
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
    "print(\";\\n\");\n"
    ;
    char *buf = (char*)::malloc(strlen(cmd)+2*FL_PATH_MAX);
    sprintf(buf, cmd, filename, filename);

    newtRefVar    result;
    newtErr    err;
    NewtInit(0, 0L, 0);
    NsUndefGlobalVar(kNewtRefNIL, NSSYM0(_STDERR_));
    NcDefGlobalVar(NSSYM0(_STDERR_), NewtMakeString("", false));
    NsUndefGlobalVar(kNewtRefNIL, NSSYM0(_STDOUT_));
    NcDefGlobalVar(NSSYM0(_STDOUT_), NewtMakeString("", false));
    result = NVMInterpretStr(buf, &err);

    ::free(buf);

    newtRef outRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDOUT_));
    if (NewtRefIsString(outRef)) {
        const char *outStr = NewtRefToString(outRef);
        mCurrentScript->SetSourceCode(outStr);
    }

    newtRef errRef = NsGetGlobalVar(kNewtRefNIL, NSSYM0(_STDERR_));
    if (NewtRefIsString(errRef)) {
        const char *errStr = NewtRefToString(errRef);
        PrintErr(errStr);
    }

    NewtCleanup();
    
    return 0;
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
}

void TToolkit::SetTempPkgPath()
{
    char buf[FL_PATH_MAX];
    Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
    prefs.getUserdataPath(buf, FL_PATH_MAX);
    strcat(buf, "tmp.pkg");
    SetPkgPath(buf);
    ::remove(buf);
}

void TToolkit::SetPkgPath(const char *name)
{
    if (name==mPkgPath) return;
    if (mPkgPath)
        ::free(mPkgPath);
    mPkgPath = nullptr;
    if (name)
        mPkgPath = strdup(name);
}

void TToolkit::SetPkgName(const char *name)
{
    if (name==mPkgName) return;
    if (mPkgName)
        ::free(mPkgName);
    mPkgName = nullptr;
    if (name)
        mPkgName = strdup(name);
}

void TToolkit::SetPkgSymbol(const char *name)
{
    if (name==mPkgSymbol) return;
    if (mPkgSymbol)
        ::free(mPkgSymbol);
    mPkgSymbol = nullptr;
    if (name)
        mPkgSymbol = strdup(name);
}

void TToolkit::SetPkgLabel(const char *name)
{
    if (name==mPkgLabel) return;
    if (mPkgLabel)
        ::free(mPkgLabel);
    mPkgLabel = nullptr;
    if (name)
        mPkgLabel = strdup(name);
}


/**
 * Extract project settings from the newt environment.
 */
int TToolkit::ReadScriptResults()
{
    newtRef newt = NsGetGlobalVar(kNewtRefNIL, NSSYM(newt));
    if (!NewtRefIsFrame(newt)) {
        PrintErr("Error: can't build package, 'newt' not defined!\n");
        return -1;
    }
    // if newt.pkPath is a string, the package will be written to this path
    newtRef pkgPath = NsGetSlot(kNewtRefNIL, newt, NSSYM(pkgPath));
    if (NewtRefIsString(pkgPath)) {
        const char *pkgPathStr = NewtRefToString(pkgPath);
        SetPkgPath(pkgPathStr);
    }
    // check if we the user has created an app
    newtRef app = NsGetSlot(kNewtRefNIL, newt, NSSYM(app));
    if (!NewtRefIsFrame(app)) {
        PrintErr("Error: can't build package, 'newt.app' not defined!\n");
        return -1;
    }
    newtRef pkgName = NsGetSlot(kNewtRefNIL, app, NSSYM(name));
    if (NewtRefIsString(pkgName)) {
        const char *pkgNameStr = NewtRefToString(pkgName);
        SetPkgName(pkgNameStr);
    } else {
        PrintErr("Error: can't build package, 'newt.app.name' not defined!\n");
        return -1;
    }
    newtRef pkgParts = NsGetSlot(kNewtRefNIL, app, NSSYM(parts));
    if (!NewtRefIsArray(pkgParts)) {
        PrintErr("Error: can't build package, 'newt.app.parts' not defined!\n");
        return -1;
    }
    newtRef pkgPart0 = NewtGetArraySlot(pkgParts, 0);
    if (!NewtRefIsFrame(pkgPart0)) {
        PrintErr("Error: can't build package, 'newt.app.parts[0]' not defined!\n");
        return -1;
    }
    newtRef data = NsGetSlot(kNewtRefNIL, pkgPart0, NSSYM(data));
    if (!NewtRefIsFrame(data)) {
        PrintErr("Error: can't build package, 'newt.app.parts[0].data' not defined!\n");
        return -1;
    }
    newtRef pkgSymbol = NsGetSlot(kNewtRefNIL, data, NSSYM(app));
    if (NewtRefIsSymbol(pkgSymbol)) {
        const char *pkgSymbolStr = NewtSymbolGetName(pkgSymbol);
        SetPkgSymbol(pkgSymbolStr);
    } else {
        PrintErr("Error: can't build package, package symbol not defined! Expected in 'newt.app.parts[0].data'.\n");
        return -1;
    }
    newtRef pkgLabel = NsGetSlot(kNewtRefNIL, data, NSSYM(text));
    if (NewtRefIsString(pkgLabel)) {
        const char *pkgLabelStr = NewtRefToString(pkgLabel);
        SetPkgLabel(pkgLabelStr);
    } else {
        SetPkgLabel("<unknown>");
    }
    PrintStd("Info: package compiled.\n");
    return 0;
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
