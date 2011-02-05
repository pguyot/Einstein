// ==============================
// File:			TJITGeneric_Multiply_template.t
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

#define Multiply_TemplateName(flag_s, rd, rs, rm) \
	 Multiply_ ## flag_s ## _ ## rd ## _ ## rs ## _ ## rm

#define Multiply_Proto(flag_s, rd, rs, rm) \
		JITUnit* Multiply_TemplateName(flag_s, rd, rs, rm) \
			 (JITUnit* ioUnit, TARMProcessor* ioCPU)
	
#if DECLARATION
	#define Multiply_Template(flag_s, rd, rs, rm) \
		Multiply_Proto(flag_s, rd, rs, rm);
#endif
#if IMPLEMENTATION
	#define Multiply_Template(flag_s, rd, rs, rm) \
		Multiply_Proto(flag_s, rd, rs, rm)
#endif
#if TRANSLATION_ARRAY
	#define Multiply_Template(flag_s, rd, rs, rm) \
		Multiply_TemplateName(flag_s, rd, rs, rm),
#endif

// Include the template for all values of registers and rm, etc.
#define INCLUDE_TEMPLATE "TJITGeneric_Multiply_template.h"
#define FLAG_S 0
#include "IncludeRdRsRm15.h"
#undef FLAG_S
#define FLAG_S 1
#include "IncludeRdRsRm15.h"
#undef FLAG_S
#undef INCLUDE_TEMPLATE

#undef Multiply_Proto
#undef Multiply_Template
#undef Multiply_TemplateName
