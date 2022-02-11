// ==============================
// File:			TFLApp_Graphics.cpp
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

#include "TFLApp_Graphics.h"

// FLTK user interface
#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Tooltip.H>
#include <FL/fl_draw.H>

enum class FileChooser {
    SaveFile,
    OpenFile,
    ChooseDirectory
};

static const char*
tfl_file_chooser(const char* message, const char* pat, const char* fname, FileChooser type)
{
#if UPDATED_TARGET_OS_LINUX
  char pattern[FL_PATH_MAX];
  pattern[0] = 0;
  if (pat)
  {
    const char* s = pat;
    char* d = pattern;
    Boolean brackets = false;
    while (*s)
    {
      char c = *s++;
      if (c == '\t')
      {
        *d++ = ' ';
        *d++ = '(';
        brackets = true;
      } else if (c == '\n' && brackets)
      {
        *d++ = ')';
        *d++ = '\t';
      } else
      {
        *d++ = c;
      }
    }
    *d = 0;
  }
  return fl_file_chooser(message, pattern[0] ? pattern : nullptr, fname);
#else
  static char tfl_file_chooser_filename[FL_PATH_MAX];
  char name[FL_PATH_MAX];
  name[0] = 0;
  char fdir[FL_PATH_MAX];
  fdir[0] = 0;

  if (fname && *fname)
  {
    const char* n = fl_filename_name(fname);
    if (n)
    {
      int len = n - fname;
      strcpy(name, n);
      strncpy(fdir, fname, len);
      fdir[len] = 0;
    } else
    {
      strcpy(name, fname);
    }
  }

  Fl_Native_File_Chooser fnfc;
  fnfc.title(message);
  switch (type)
  {
    case FileChooser::SaveFile:
      fnfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
      fnfc.options(Fl_Native_File_Chooser::NEW_FOLDER | Fl_Native_File_Chooser::USE_FILTER_EXT);
      break;
    case FileChooser::OpenFile:
      fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
      fnfc.options(Fl_Native_File_Chooser::USE_FILTER_EXT);
      break;
    case FileChooser::ChooseDirectory:
      fnfc.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
      fnfc.options(Fl_Native_File_Chooser::USE_FILTER_EXT);
      break;
  }
  fnfc.filter(pat);
  fnfc.directory(fdir);
  fnfc.preset_file(name);
  switch (fnfc.show())
  {
    case -1:
      return nullptr; // Error text is in fnfc.errmsg()
    case 1:
      return nullptr; // user canceled
  }
  if (fnfc.filename())
  {
    strcpy(tfl_file_chooser_filename, fnfc.filename());
    return tfl_file_chooser_filename;
  } else
  {
    return nullptr;
  }
#endif
}

const char*
TFLApp::ChooseExistingFile(const char* message, const char* pat, const char* fname)
{
  return tfl_file_chooser(message, pat, fname, FileChooser::OpenFile);
}

const char*
TFLApp::ChooseExistingDirectory(const char* message, const char* pat, const char* fname)
{
  return tfl_file_chooser(message, pat, fname, FileChooser::ChooseDirectory);
}

const char*
TFLApp::ChooseNewFile(const char* message, const char* pat, const char* fname)
{
  return tfl_file_chooser(message, pat, fname, FileChooser::SaveFile);
}

#define BP fl_begin_polygon()
#define EP fl_end_polygon()
#define BCP fl_begin_complex_polygon()
#define ECP fl_end_complex_polygon()
#define BL fl_begin_line()
#define EL fl_end_line()
#define BC fl_begin_loop()
#define EC fl_end_loop()
#define vv(x, y) fl_vertex(x, y)

#define VF 0.9
#define VL 0.65
#define VB 0.0

#define AF 1.0
#define AL 0.6
#define AB 0.15
#define AA 0.15

static void
extSymbol(Fl_Color c)
{
	fl_color(c);
	BCP;
	// outline square, starting right side
	vv(VF, -VB);
	vv(VF, VF);
	vv(-VF, VF);
	vv(-VF, -VF);
	vv(VB, -VF);
	// inline square
	vv(VB, -VL);
	vv(-VL, -VL);
	vv(-VL, VL);
	vv(VL, VL);
	vv(VL, -VB);
	ECP;
	BCP;
	vv(-AA, -AA);
	vv(0.4, -AL - AA);
	vv(AB, -AF);
	vv(AF, -AF);
	vv(AF, -AB);
	vv(AL + AA, -0.4);
	vv(AA, AA);
	ECP;
#if 0
    fl_color(fl_darker(c));
    BC;
    // outline square, starting right side
    vv(VF, -VB); vv(VF, VF); vv(-VF, VF); vv(-VF, -VF); vv(VB, -VF);
    // inline square
    vv(VB, -VL); vv(-VL, -VL); vv(-VL, VL); vv(VL, VL); vv(VL, -VB);
    EC;
    BC;
    vv(-AA, -AA); vv(0.4, -AL-AA); vv(AB, -AF); vv(AF, -AF); vv(AF, -AB); vv(AL+AA, -0.4); vv(AA, AA);
    EC;
#endif
}

static void
draw_ramp(int x, int y, int w, int h, Fl_Color c)
{
	for (int i = y; i < y + h; i++)
	{
		fl_color(fl_color_average(FL_BACKGROUND_COLOR, c, i / 100.0));
		// fl_rectf(x, y, w, h, Fl::box_color(c));
		fl_xyline(x, i, x + w);
	}
}

static void
tabs_box(int x, int y, int w, int h, Fl_Color c)
{
	const int barHgt = 1;
	fl_rectf(x, y, w, barHgt, fl_color_average(FL_FOREGROUND_COLOR, c, 0.5));
	fl_rectf(x, y + barHgt, w, h - barHgt, c);
}

void
TFLApp::InitFLTK(int argc, char** argv)
{
  Fl::scheme("gtk+");
  Fl::args(argc, argv);
  Fl::get_system_colors();
  Fl::use_high_res_GL(1);
  Fl::visual(FL_RGB);
  Fl_Tooltip::size(12);
  Fl::set_boxtype(FL_FREE_BOXTYPE, draw_ramp, 0, 0, 0, 0);
  Fl::set_boxtype((Fl_Boxtype) (FL_FREE_BOXTYPE + 1), draw_ramp, 0, 0, 0, 0);
  Fl::set_boxtype((Fl_Boxtype) (FL_FREE_BOXTYPE + 2), tabs_box, 0, 2, 0, 0);
  Fl::set_boxtype((Fl_Boxtype) (FL_FREE_BOXTYPE + 3), tabs_box, 0, 2, 0, 0);
  fl_add_symbol("ext", extSymbol, 1);
}

// ======================================================================= //
// We build our computer (systems) the way we build our cities: over time,
// without a plan, on top of ruins.
//   -- Ellen Ullman
// ======================================================================= //
