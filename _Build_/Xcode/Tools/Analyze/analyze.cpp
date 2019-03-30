// ==============================
// File:			analyze.cpp
// Project:			Einstein
//
// Copyright 2019 by Matthais Melcher (proteus@matthiasm.com).
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

/*
 The "analyze" tool is meant to read and interprete the NetonOS debugging image
 that came with Apple Driver Development Kit. The image is available in the .aif
 file format and contains a fully working ROM and REx, and a large number of
 debugging symbols. Many symbols are in C and C++ format, providing basic
 insights into the inner workings of the ROM.

 Usage: analyze --proteus proteusfile --aif path_to_717006.aif

 Xcode: when using Xcode, put your copy of 717006.aif into the _Data_ directory.

 "Analyze" will go through a list of tasks to understand as much as possible of
 the given image and create a map of its knowledge. It then merges its finding
 into the "proteus.cpp" file.

 The "proteus.cpp" file is a monolithic handwritten C++ code file that can
 also contain a readable copy of the .aif image, created by "analyze".

 Task (- = implemented, * = still to do):
  * verify aif
  * read the ROM binary data
  * read the symbol table and demangles C++ symbols
  * (analyze known header files)
  * static analysis of ARM code, following known entry points
  * static analysis of ROM Soup (Newton Script object tree)
  * analyze ROM Extension

  * read proteus file line by line
  * expand lines that start with "// /+ 0x00000000-0x11111111 +/" and
    add the findings from the analysis
  * hand-written code is never changed
  * save squashed file, replacing the original proteus file

 */

#include <iostream>

// Topic, Issue, Knowledge, Effect, Conclusion, Finding, Discovery, Clue
// gROMMagicPointerTable
// gROMSoupData
// gROMSoupDataSize

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}


// ============================================================================== //
// I've ended up calling my cat Brexit. It wakes me up meowing like crazy every
// morning because it wants to go out, but as soon as I open the door, it just
// sits there undecided and then looks angry when I put it outside.
//   -- French Europe Minister Nathalie Loiseau
// ============================================================================== //
