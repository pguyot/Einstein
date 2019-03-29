// ==============================
// File:			squash.cpp
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
 The "squash" tool removes all .aif ROM image based information from
 a give proteus file.

 The "proteus.cpp" file is a monolithic handwritten C++ code file that can
 also contain a readable copy of the .aif image, created by "analyze".

 Task (- = implemented, * = still to do):
  * read proteus file line by line
  * compress lines that start with "// /+ 0x00000000-0x11111111 +/"
  * hand-written code is never changed
  * save squashed file, replacing the original proteus file

 */


/*
 // /* 0x0071FC4C-0x007EE048...
 */

#include <iostream>

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << argv[0] << " " << argv[1] << " " << argv[2] << "\n";
    return 0;
}

// ============================================================================== //
// I've ended up calling my cat Brexit. It wakes me up meowing like crazy every
// morning because it wants to go out, but as soon as I open the door, it just
// sits there undecided and then looks angry when I put it outside.
//   -- French Europe Minister Nathalie Loiseau
// ============================================================================== //
