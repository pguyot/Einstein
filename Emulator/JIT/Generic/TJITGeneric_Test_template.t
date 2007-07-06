// ==============================
// File:			TJITGeneric_Test_template.t
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
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

#define Test_TemplateName(cond, offset) \
	Test ## cond ## offset

#if DECLARATION
	#define Test_Template(cond, offset, body) \
		JITInstructionProto(Test_TemplateName(cond, offset));
#endif
#if IMPLEMENTATION
	#define Test_Template(cond, offset, body) \
		JITInstructionProto(Test_TemplateName(cond, offset)) \
			body
#endif

// Include the template for all values of offset
#define OFFSET 2
#include "TJITGeneric_Test_template.h"
#undef OFFSET

#define OFFSET 3
#include "TJITGeneric_Test_template.h"
#undef OFFSET

#define OFFSET 4
#include "TJITGeneric_Test_template.h"
#undef OFFSET

#define OFFSET 5
#include "TJITGeneric_Test_template.h"
#undef OFFSET

#define OFFSET 6
#include "TJITGeneric_Test_template.h"
#undef OFFSET

#define OFFSET 7
#include "TJITGeneric_Test_template.h"
#undef OFFSET

#undef Test_Template
#undef Test_TemplateName

// =============================================================== //
// Per buck you get more computing action with the small computer. //
//                 -- R.W. Hamming                                 //
// =============================================================== //
