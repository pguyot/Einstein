// ==============================
// File:            TFLMonitor.cp
// Project:            Einstein
//
// Copyright 2003-2007 by Matthias Melcher
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



#include "TFLMonitor.h"
#include "TSymbolList.h"
#include "UDisasm.h"

#include "Emulator/TARMProcessor.h"
#include "Emulator/TInterruptManager.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Simple_Terminal.H>



TFLMonitor::TFLMonitor(TBufferLog* inLog,
                       TEmulator* inEmulator,
                       TSymbolList* inSymbolList,
                       const char *inROMPath )
:   TMonitor(inLog, inEmulator, inSymbolList, inROMPath)
{
}


TFLMonitor::~TFLMonitor()
{
    Hide();
    delete mwWindow;
}

// -------------------------------------------------------------------------- //
// DrawScreen( void )
// -------------------------------------------------------------------------- //
bool TFLMonitor::DrawScreen()
{
    if (!mwTerminal)
        return false;

    bool theResult = false;
    if (mHalted)
    {
        if (!mLastScreenHalted)
        {
            // Clear the terminal.
//            mwTerminal->clear();
            theResult = true;
        }
        mLastScreenHalted = true;
        DrawScreenHalted();
    } else {
        if (mLastScreenHalted)
        {
            // Clear the terminal.
//            mwTerminal->clear();
            theResult = true;
        }
        mLastScreenHalted = false;
        DrawScreenRunning();
    }
    mwTerminal->redraw();
    return theResult;
}


// -------------------------------------------------------------------------- //
// DrawScreenHalted( void )
// -------------------------------------------------------------------------- //
void TFLMonitor::DrawScreenHalted()
{
    Fl::lock();
    KUInt32 realPC = mProcessor->GetRegister(15) - 4;

    // Go to the uppermost position.
    mwTerminal->clear();
    mwTerminal->text("");
    int indexRegisters;
    for (indexRegisters = 0; indexRegisters < 16; indexRegisters++)
    {
        mwTerminal->printf( "%s= %.8X | %s\n",
                        kRegisterNames[indexRegisters],
                        (unsigned int) mProcessor->GetRegister(indexRegisters),
                        mLog->GetLine(indexRegisters) );
    }
    KUInt32 theCPSR = mProcessor->GetCPSR();
    KUInt32 theMode = theCPSR & TARMProcessor::kPSR_ModeMask;
    mwTerminal->printf( "%c%c%c%c %c%c%c %s  | %s\n",
                    theCPSR & TARMProcessor::kPSR_NBit ? 'N' : 'n',
                    theCPSR & TARMProcessor::kPSR_ZBit ? 'Z' : 'z',
                    theCPSR & TARMProcessor::kPSR_CBit ? 'C' : 'c',
                    theCPSR & TARMProcessor::kPSR_VBit ? 'V' : 'v',
                    theCPSR & TARMProcessor::kPSR_IBit ? 'I' : 'i',
                    theCPSR & TARMProcessor::kPSR_FBit ? 'F' : 'f',
                    theCPSR & TARMProcessor::kPSR_TBit ? 'T' : 't',
                    kModesNames[theMode],
                    mLog->GetLine(16) );
    KUInt32 theSPSR = 0;
    if ((theMode != TARMProcessor::kSystemMode)
        && (theMode != TARMProcessor::kUserMode))
    {
        theSPSR = mProcessor->GetSPSR();
    }
    if (theSPSR == 0)
    {
        mwTerminal->printf( "---- --- ---  | %s\n",
                        mLog->GetLine(17) );
    } else {
        mwTerminal->printf( "%c%c%c%c %c%c%c %s  | %s\n",
                        theSPSR & TARMProcessor::kPSR_NBit ? 'N' : 'n',
                        theSPSR & TARMProcessor::kPSR_ZBit ? 'Z' : 'z',
                        theSPSR & TARMProcessor::kPSR_CBit ? 'C' : 'c',
                        theSPSR & TARMProcessor::kPSR_VBit ? 'V' : 'v',
                        theSPSR & TARMProcessor::kPSR_IBit ? 'I' : 'i',
                        theSPSR & TARMProcessor::kPSR_FBit ? 'F' : 'f',
                        theSPSR & TARMProcessor::kPSR_TBit ? 'T' : 't',
                        kModesNames[theSPSR & TARMProcessor::kPSR_ModeMask],
                        mLog->GetLine(17) );
    }

    mwTerminal->printf( "==============| %s\n",
                    mLog->GetLine(18) );

    mwTerminal->printf( "Tmr= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetFrozenTimer(),
                    mLog->GetLine(19) );

    mwTerminal->printf( "TM0= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetTimerMatchRegister(0),
                    mLog->GetLine(20) );

    mwTerminal->printf( "TM1= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetTimerMatchRegister(1),
                    mLog->GetLine(21) );

    mwTerminal->printf( "TM2= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetTimerMatchRegister(2),
                    mLog->GetLine(22) );

    mwTerminal->printf( "TM3= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetTimerMatchRegister(3),
                    mLog->GetLine(23) );

    mwTerminal->printf( "RTC= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetRealTimeClock(),
                    mLog->GetLine(24) );

    mwTerminal->printf( "Alm= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetAlarm(),
                    mLog->GetLine(25) );

    mwTerminal->printf( "IR = %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetIntRaised(),
                    mLog->GetLine(26) );

    mwTerminal->printf( "ICR= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetIntCtrlReg(),
                    mLog->GetLine(27) );

    mwTerminal->printf( "FM = %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetFIQMask(),
                    mLog->GetLine(28) );

    mwTerminal->printf( "IC1= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetIntEDReg1(),
                    mLog->GetLine(29) );

    mwTerminal->printf( "IC2= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetIntEDReg2(),
                    mLog->GetLine(30) );

    mwTerminal->printf( "IC3= %.8X | %s\n",
                    (unsigned int) mInterruptManager->GetIntEDReg3(),
                    mLog->GetLine(31) );

    mwTerminal->printf( "-------------------------------------------------------------------------------\n");

    char theInstr[512];
    theInstr[0] = 0;

    char theSymbol[512];
    char theComment[512];
    int theOffset;
    char theLine[512];

    if ( mSymbolList )
    {
        // Get the symbol.
        mSymbolList->GetNearestSymbolByAddress(
                                               realPC,
                                               theSymbol,
                                               theComment,
                                               &theOffset );
        mwTerminal->printf("%s+%X\n", theSymbol, theOffset );
    }
    else
    {
        theSymbol[0] = '\0';
        theComment[0] = '\0';
        theLine[0] = '\0';
        theOffset = 0;
        mwTerminal->printf("%X\n", realPC );
    }


    KUInt32 instruction;

    // Write 5 lines.
    int indexLines;
    for (indexLines = 0; indexLines < 20; indexLines += 4)
    {
        if (mMemory->Read(
                          (TMemory::VAddr) realPC + indexLines, instruction ))
        {
            mwTerminal->printf(
                            "%.8X Memory Error [%.8X]\n",
                            (unsigned int) realPC + indexLines,
                            (unsigned int) mMemory->GetFaultStatusRegister() );
        } else {
            bool instIsBP = false;
            if ((instruction & 0xFFF000F0) == 0xE1200070)
            {
                if (!mMemory->ReadBreakpoint(
                                             (TMemory::VAddr) realPC + indexLines, instruction ))
                {
                    instIsBP = true;
                }
            }
            UDisasm::Disasm(
                            theInstr,
                            sizeof(theInstr),
                            realPC + indexLines,
                            instruction,
                            mSymbolList );

            char status[32];
            status[0] = '\0';
            if (indexLines == 0)
            {
                if (instruction >> 28 != 0xE)
                {
                    KUInt32 skip = 0;
                    switch (instruction >> 28)
                    {
                            // 0000 = EQ - Z set (equal)
                        case 0x0:
                            skip = !(theCPSR & TARMProcessor::kPSR_ZBit);
                            break;

                            // 0001 = NE - Z clear (not equal)
                        case 0x1:
                            skip = theCPSR & TARMProcessor::kPSR_ZBit;
                            break;
                            // 0010 = CS - C set (unsigned higher or same)
                        case 0x2:
                            skip = !(theCPSR & TARMProcessor::kPSR_CBit);
                            break;

                            // 0011 = CC - C clear (unsigned lower)
                        case 0x3:
                            skip = theCPSR & TARMProcessor::kPSR_CBit;
                            break;

                            // 0100 = MI - N set (negative)
                        case 0x4:
                            skip = !(theCPSR & TARMProcessor::kPSR_NBit);
                            break;

                            // 0101 = PL - N clear (positive or zero)
                        case 0x5:
                            skip = theCPSR & TARMProcessor::kPSR_NBit;
                            break;

                            // 0110 = VS - V set (overflow)
                        case 0x6:
                            skip = !(theCPSR & TARMProcessor::kPSR_VBit);
                            break;

                            // 0111 = VC - V clear (no overflow)
                        case 0x7:
                            skip = theCPSR & TARMProcessor::kPSR_VBit;
                            break;

                            // 1000 = HI - C set and Z clear (unsigned higher)
                        case 0x8:
                            skip = !(theCPSR & TARMProcessor::kPSR_CBit)
                            || (theCPSR & TARMProcessor::kPSR_ZBit);
                            break;

                            // 1001 = LS - C clear or Z set (unsigned lower or same)
                        case 0x9:
                            skip = (theCPSR & TARMProcessor::kPSR_CBit)
                            && !(theCPSR & TARMProcessor::kPSR_ZBit);
                            break;

                            // 1010 = GE - N set and V set, or N clear and V clear (greater or equal)
                        case 0xA:
                            skip = ((theCPSR & TARMProcessor::kPSR_NBit) != 0)
                            != ((theCPSR & TARMProcessor::kPSR_VBit) != 0);
                            break;

                            // 1011 = LT - N set and V clear, or N clear and V set (less than)
                        case 0xB:
                            skip = ((theCPSR & TARMProcessor::kPSR_NBit) != 0)
                            == ((theCPSR & TARMProcessor::kPSR_VBit) != 0);
                            break;

                            // 1100 = GT - Z clear, and either N set and V set, or N clear and V clear (greater than)
                        case 0xC:
                            skip = (theCPSR & TARMProcessor::kPSR_ZBit) ||
                            (((theCPSR & TARMProcessor::kPSR_NBit) != 0)
                             != ((theCPSR & TARMProcessor::kPSR_VBit) != 0));
                            break;

                            // 1101 = LE - Z set, or N set and V clear, or N clear and V set (less than or equal)
                        case 0xD:
                            skip = (!(theCPSR & TARMProcessor::kPSR_ZBit))
                            && (((theCPSR & TARMProcessor::kPSR_NBit) != 0)
                                == ((theCPSR & TARMProcessor::kPSR_VBit) != 0));
                            break;

                            // 1111 = NV - never
                        case 0xF:
                        default:
                            skip = 1;
                    }

                    if (skip)
                    {
                        (void) ::sprintf( status, " (will skip)" );
                    } else {
                        (void) ::sprintf( status, " (will do it)" );
                    }
                }
            }
            if (instIsBP)
            {
                (void) ::sprintf( theLine, "%.8X * %s",
                                 (unsigned int) realPC + indexLines,
                                 theInstr );
            } else {
                (void) ::sprintf( theLine, "%.8X   %s",
                                 (unsigned int) realPC + indexLines,
                                 theInstr );
            } // if (indexLines == 0)
            mwTerminal->printf( "%s%s\n",
                            theLine,
                            status );
        }
    }

    // Footer.
    mwTerminal->printf( "-------------------------------------------------------------------------------\n" );
    Fl::unlock();
}


// -------------------------------------------------------------------------- //
// DrawScreenRunning( void )
// -------------------------------------------------------------------------- //
void TFLMonitor::DrawScreenRunning()
{
    Fl::lock();
    mwTerminal->text("");
    mwTerminal->printf( "Machine is running. Use stop to halt it.\n");
    mwTerminal->printf( "-------------------------------------------------------------------------------\n");
    int indexLog;
    for (indexLog = 0; indexLog < 32; indexLog++)
    {
        mwTerminal->printf( "%s\n", mLog->GetLine(indexLog) );
    }
    mwTerminal->printf( "-------------------------------------------------------------------------------\n");
    Fl::unlock();
}


static void set_attributes(Fl_Button *w) {
    w->box(FL_FLAT_BOX);
    w->labelcolor(FL_DARK2);
    w->labelsize(12);
    w->visible_focus(0);
}

void TFLMonitor::Show()
{
    if (!mwWindow) {
        const int cw=9, ch=13, wh=44;
        int xp = 0;
        // TODO: this should probably be done in Fluid
        mwWindow = new Fl_Double_Window(cw*80, ch*(wh+2), "Einstein Monitor");
        // ---- terminal window for all text output
        mwTerminal = new Fl_Simple_Terminal(0, 0, cw*80, ch*wh);
        mwTerminal->box(FL_FLAT_BOX);
        mwTerminal->hide_cursor();
        mwTerminal->color(FL_LIGHT3);
        mwTerminal->textcolor(FL_FOREGROUND_COLOR);
        mwTerminal->cursor_color(FL_BACKGROUND_COLOR);
        //mwTerminal->ansi(true);
        mwTerminal->stay_at_bottom(true);
        // --- group all this for perfect resizing
        Fl_Group *mwToolbar = new Fl_Group(0, ch*wh, mwWindow->w(), 2*ch);
        // --- the stop button stops the emulation
        mwPause = new Fl_Button(xp, ch*wh, 3*cw, 2*ch, "@||"); xp += 3*cw;
        mwPause->callback([](Fl_Widget*,void*m) { ((TMonitor*)m)->ExecuteCommand("stop"); }, this);
        set_attributes(mwPause);
        // --- continue running the app
        mwRun = new Fl_Button(xp, ch*wh, 3*cw, 2*ch, "@|>"); xp += 3*cw;
        mwRun->callback([](Fl_Widget*,void*m) { ((TMonitor*)m)->ExecuteCommand("run"); }, this);
        set_attributes(mwRun);
        // --- step over
        mwStepOver = new Fl_Button(xp, ch*wh, 3*cw, 2*ch, "@3redo"); xp += 3*cw;
        mwStepOver->callback([](Fl_Widget*,void*m) { ((TMonitor*)m)->ExecuteCommand("trace"); }, this);
        set_attributes(mwStepOver);
        // --- step into
        mwStep = new Fl_Button(xp, ch*wh, 3*cw, 2*ch, "@2->|"); xp += 3*cw;
        mwStep->callback([](Fl_Widget*,void*m) { ((TMonitor*)m)->ExecuteCommand("step"); }, this);
        set_attributes(mwStep);
        // --- leave
        mwLeave = new Fl_Button(xp, ch*wh, 3*cw, 2*ch, "@8->|"); xp += 3*cw;
        mwLeave->callback([](Fl_Widget*,void*m) { ((TMonitor*)m)->ExecuteCommand("mr"); }, this);
        set_attributes(mwLeave);
        // --- enter your commands here
        mwInput = new Fl_Input(xp, ch*wh, mwWindow->w()-xp-3*cw, 2*ch);
        mwInput->color(FL_LIGHT3);
        mwInput->textfont(mwTerminal->textfont());
        mwInput->textsize(mwTerminal->textsize());
        mwInput->box(FL_BORDER_BOX);
        mwInput->when(FL_WHEN_ENTER_KEY_ALWAYS);
        mwInput->callback([](Fl_Widget*w,void*m)
        {
            Fl_Input *in = (Fl_Input*)w;
            ((TMonitor*)m)->ExecuteCommand(in->value());
            in->value("");
            Fl::focus(in);
        }, this);
        // --- help
        mwHelp = new Fl_Button(mwWindow->w()-3*cw, ch*wh, 3*cw, 2*ch, "?");
        mwHelp->callback([](Fl_Widget*,void*m) { ((TMonitor*)m)->ExecuteCommand("help"); }, this);
        set_attributes(mwHelp);
        mwHelp->labelcolor(FL_BLACK);
        // --- set resizing properties
        mwToolbar->resizable(mwInput);
        mwToolbar->end();
        mwWindow->resizable(mwTerminal);
        mwWindow->end();
        Fl::focus(mwInput);
    }
    // TODO: get the coordinates from TFLSettings
    DrawScreen();
    mwWindow->show();
}


void TFLMonitor::Hide()
{
    if (mwWindow)
        mwWindow->hide();
}

void TFLMonitor::PrintLine( const char* inLine, int type )
{
    if (mwTerminal) {
        Fl::lock();
        mwTerminal->append(inLine);
        mwTerminal->append("\n");
        Fl::unlock();
    }
}

