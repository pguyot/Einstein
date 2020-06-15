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

#define IGNORE_TNEWT
#include "Emulator/Platform/TPlatformManager.h"
#undef IGNORE_TNEWT

#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtVM.h"
#include "NewtParser.h"

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

        wToolkitEditor->buffer()->loadfile("/Users/matt/dev/newton-test/mini.ns");
        wToolkitTerminal->buffer( gTerminalBuffer = new Fl_Text_Buffer() );
    }
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


void TToolkit::AppBuild()
{
    gTerminalBuffer->append("Compiling...\n");
    newtRefVar    result;
    newtErr    err;
    NewtInit(0, 0L, 0);
    //    newt_chdir();
    //result = NVMInterpretFile("/Users/matt/dev/newton-test/mini.ns", &err);
    result = NVMInterpretStr(wToolkitEditor->buffer()->text(), &err);
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
}



extern "C" void yyerror(char * s)
{
    if (s[0] && s[1] == ':')
        NPSErrorStr(*s, s + 2);
    else
        NPSErrorStr('E', s);
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
