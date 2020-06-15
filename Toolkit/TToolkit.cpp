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

#define IGNORE_TNEWT
#include "Emulator/Platform/TPlatformManager.h"
#undef IGNORE_TNEWT

#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtVM.h"
#include "NewtParser.h"

Fl_Text_Buffer *gSourcecodeBuffer = nullptr;
Fl_Text_Buffer *gTerminalBuffer = nullptr;

TToolkit *gToolkit = nullptr;


// FIXME: the syntax highlighting was taken from the FLTK "C" editor demo and needs to be updated to NewtonScript
// FIXME: actually, lex and yacc could be used for syntax highlighting... .

// Syntax highlighting stuff...
#define TS 14 // default editor textsize
Fl_Text_Buffer     *stylebuf = 0;
Fl_Text_Display::Style_Table_Entry
styletable[] = {    // Style table
    { FL_BLACK,         FL_COURIER,           TS }, // A - Plain
    { FL_DARK_GREEN,    FL_COURIER,           TS }, // B - Line comments
    { FL_DARK_GREEN,    FL_COURIER,           TS }, // C - Block comments
    { 0x99000000,       FL_COURIER,           TS }, // D - Strings
    { FL_DARK_MAGENTA,  FL_COURIER,           TS }, // E - Directives
    { FL_BLUE,          FL_COURIER_BOLD,      TS }, // F - Types
    { FL_DARK_MAGENTA,  FL_COURIER_BOLD,      TS }, // G - Keywords
};
const char         *code_keywords[] = {    // List of known C/C++ keywords...
    "and",
    "begin",
    "break",
    "by",
    "call",
    "constant",
    "div",
    "do",
    "else",
    "end",
    "exists",
    "for",
    "foreach",
    "func",
    "global",
    "if",
    "in",
    "inherited",
    "local",
    "loop",
    "mod",
    "native",
    "not",
    "onexception",
    "or",
    "repeat",
    "return",
    "self",
    "then",
    "to",
    "try",
    "until",
    "while",
    "with",
};
const char         *code_types[] = {    // List of known C/C++ types...
    "nil",
    "true",
};


//
// 'compare_keywords()' - Compare two keywords...
//

extern "C"
int compare_keywords(const void *a, const void *b)
{
//    static const char *lut[] = {"<", "=", ">"};
    const char *aa = *(const char **)a;
    const char *bb = *(const char **)b;
    int ret = (strcasecmp(aa, bb));
//    printf("%s %s %s\n", aa, lut[((rewt > 0) - (ret < 0))+1], bb);
    return ret;
}


//
// 'style_parse()' - Parse text and produce style data.
//

void
style_parse(const char *text,
            char       *style,
            int        length) {
    char         current;
    int         col;
    int         last;
    char         buf[255],
    *bufptr;
    const char *temp;

    // Style letters:
    //
    // A - Plain
    // B - Line comments
    // C - Block comments
    // D - Strings
    // E - Directives
    // F - Types
    // G - Keywords

    for (current = *style, col = 0, last = 0; length > 0; length --, text ++) {
        if (current == 'B' || current == 'F' || current == 'G') current = 'A';
        if (current == 'A') {
            // Check for directives, comments, strings, and keywords...
            if (*text == '\'') {
                // Set style to directive
                current = 'E';
            } else if (strncmp(text, "//", 2) == 0) {
                current = 'B';
                for (; length > 0 && *text != '\n'; length --, text ++) *style++ = 'B';

                if (length == 0) break;
            } else if (strncmp(text, "/*", 2) == 0) {
                current = 'C';
            } else if (strncmp(text, "\\\"", 2) == 0) {
                // Quoted quote...
                *style++ = current;
                *style++ = current;
                text ++;
                length --;
                col += 2;
                continue;
            } else if (*text == '\"') {
                current = 'D';
            } else if (!last && (islower((*text)&255) || *text == '_')) {
                // Might be a keyword...
                for (temp = text, bufptr = buf;
                     (islower((*temp)&255) || *temp == '_') && bufptr < (buf + sizeof(buf) - 1);
                     *bufptr++ = *temp++) {
                    // nothing
                }

                if (!islower((*temp)&255) && *temp != '_') {
                    *bufptr = '\0';

                    bufptr = buf;

                    if (bsearch(&bufptr, code_types,
                                sizeof(code_types) / sizeof(code_types[0]),
                                sizeof(code_types[0]), compare_keywords)) {
                        while (text < temp) {
                            *style++ = 'F';
                            text ++;
                            length --;
                            col ++;
                        }

                        text --;
                        length ++;
                        last = 1;
                        continue;
                    } else if (bsearch(&bufptr, code_keywords,
                                       sizeof(code_keywords) / sizeof(code_keywords[0]),
                                       sizeof(code_keywords[0]), compare_keywords)) {
                        while (text < temp) {
                            *style++ = 'G';
                            text ++;
                            length --;
                            col ++;
                        }

                        text --;
                        length ++;
                        last = 1;
                        continue;
                    }
                }
            }
        } else if (current == 'C' && strncmp(text, "*/", 2) == 0) {
            // Close a C comment...
            *style++ = current;
            *style++ = current;
            text ++;
            length --;
            current = 'A';
            col += 2;
            continue;
        } else if (current == 'D') {
            // Continuing in string...
            if (strncmp(text, "\\\"", 2) == 0) {
                // Quoted end quote...
                *style++ = current;
                *style++ = current;
                text ++;
                length --;
                col += 2;
                continue;
            } else if (*text == '\"') {
                // End quote...
                *style++ = current;
                col ++;
                current = 'A';
                continue;
            }
        } else if (current == 'E') {
            if (!isalnum(*text) && *text!='_')
                current = 'A';
        }

        // Copy style info...
        if (current == 'A' && (*text == '{' || *text == '}')) *style++ = 'G';
        else *style++ = current;
        col ++;

        last = isalnum((*text)&255) || *text == '_' || *text == '.';

        if (*text == '\n') {
            // Reset column and possibly reset the style
            col = 0;
            if (current == 'B') current = 'A';
        }
    }
}


//
// 'style_init()' - Initialize the style buffer...
//

void
style_init(void) {
    char *style = new char[gSourcecodeBuffer->length() + 1];
    char *text = gSourcecodeBuffer->text();

    memset(style, 'A', gSourcecodeBuffer->length());
    style[gSourcecodeBuffer->length()] = '\0';

    if (!stylebuf) stylebuf = new Fl_Text_Buffer(gSourcecodeBuffer->length());

    style_parse(text, style, gSourcecodeBuffer->length());

    stylebuf->text(style);
    delete[] style;
    free(text);
}


//
// 'style_unfinished_cb()' - Update unfinished styles.
//

void
style_unfinished_cb(int, void*) {
}


//
// 'style_update()' - Update the style buffer...
//

void
style_update(int        pos,        // I - Position of update
             int        nInserted,    // I - Number of inserted chars
             int        nDeleted,    // I - Number of deleted chars
             int        /*nRestyled*/,    // I - Number of restyled chars
             const char * /*deletedText*/,// I - Text that was deleted
             void       *cbArg) {    // I - Callback data
    int    start,                // Start of text
    end;                // End of text
    char    last,                // Last style on line
    *style,                // Style data
    *text;                // Text data


    // If this is just a selection change, just unselect the style buffer...
    if (nInserted == 0 && nDeleted == 0) {
        stylebuf->unselect();
        return;
    }

    // Track changes in the text buffer...
    if (nInserted > 0) {
        // Insert characters into the style buffer...
        style = new char[nInserted + 1];
        memset(style, 'A', nInserted);
        style[nInserted] = '\0';

        stylebuf->replace(pos, pos + nDeleted, style);
        delete[] style;
    } else {
        // Just delete characters in the style buffer...
        stylebuf->remove(pos, pos + nDeleted);
    }

    // Select the area that was just updated to avoid unnecessary
    // callbacks...
    stylebuf->select(pos, pos + nInserted - nDeleted);

    // Re-parse the changed region; we do this by parsing from the
    // beginning of the previous line of the changed region to the end of
    // the line of the changed region...  Then we check the last
    // style character and keep updating if we have a multi-line
    // comment character...
    start = gSourcecodeBuffer->line_start(pos);
    //  if (start > 0) start = textbuf->line_start(start - 1);
    end   = gSourcecodeBuffer->line_end(pos + nInserted);
    text  = gSourcecodeBuffer->text_range(start, end);
    style = stylebuf->text_range(start, end);
    if (start==end)
        last = 0;
    else
        last  = style[end - start - 1];

    //  printf("start = %d, end = %d, text = \"%s\", style = \"%s\", last='%c'...\n",
    //         start, end, text, style, last);

    style_parse(text, style, end - start);

    //  printf("new style = \"%s\", new last='%c'...\n",
    //         style, style[end - start - 1]);

    stylebuf->replace(start, end, style);
    ((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);

    if (start==end || last != style[end - start - 1]) {
        //    printf("Recalculate the rest of the buffer style\n");
        // Either the user deleted some text, or the last character
        // on the line changed styles, so reparse the
        // remainder of the buffer...
        free(text);
        free(style);

        end   = gSourcecodeBuffer->length();
        text  = gSourcecodeBuffer->text_range(start, end);
        style = stylebuf->text_range(start, end);

        style_parse(text, style, end - start);

        stylebuf->replace(start, end, style);
        ((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);
    }

    free(text);
    free(style);
}


void TToolkit::Show() //TFLApp::UserActionToggleToolkit()
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

        wToolkitEditor->buffer( gSourcecodeBuffer = new Fl_Text_Buffer() );
        stylebuf = new Fl_Text_Buffer();
        wToolkitEditor->highlight_data(stylebuf, styletable,
                                  sizeof(styletable) / sizeof(styletable[0]),
                                  'A', style_unfinished_cb, 0);
        wToolkitEditor->linenumber_width(40);    // enable
        wToolkitEditor->linenumber_size(wToolkitEditor->textsize()-2);
        wToolkitEditor->wrap_mode(Fl_Text_Display::WRAP_NONE, 0);

        gSourcecodeBuffer->add_modify_callback(style_update, wToolkitEditor);
        //gSourcecodeBuffer->add_modify_callback(changed_cb, w);
        gSourcecodeBuffer->call_modify_callbacks();
        gSourcecodeBuffer->loadfile("/Users/matt/dev/newton-test/mini.ns");

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
    result = NVMInterpretStr(gSourcecodeBuffer->text(), &err);
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
