// ==============================
// File:			TFLToolkit.h
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

#ifndef _TFLTOOLKIT_H
#define _TFLTOOLKIT_H

#include <K/Defines/KDefinitions.h>

#include <FL/x.H>
#include <FL/Fl_Widget.H>

#include "app/Version.h"
#include "app/TFLApp.h"

/**
 * Toolkit is an integrated developer environment for Einstein, loosely based on NTK.
 */
class TToolkit
{
public:
    TToolkit(TFLApp *inApp);
    ~TToolkit();
    void Show();
    void Hide();

    void AppBuild();
    void AppInstall();
    void AppRun();
    void AppStop();

private:
    TFLApp *mApp = nullptr;
};


extern TToolkit *gToolkit;


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
