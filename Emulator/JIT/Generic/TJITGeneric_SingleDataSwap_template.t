// ==============================
// File:			TJITGeneric_SingleDataSwap_template.t
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

#define SingleDataSwap_TemplateName(flag_b, rn, rd, rm) \
	SingleDataSwap_ ## flag_b ## _ ## rn ## _ ## rd ## _ ## rm

#if DECLARATION
	#define SingleDataSwap_Template(flag_b, rn, rd, rm, body) \
		JITInstructionProto(SingleDataSwap_TemplateName(flag_b, rn, rd, rm));
#endif
#if IMPLEMENTATION
	#define SingleDataSwap_Template(flag_b, rn, rd, rm, body) \
		JITInstructionProto(SingleDataSwap_TemplateName(flag_b, rn, rd, rm)) \
			body
#endif
#if TRANSLATION_ARRAY
	#define SingleDataSwap_Template(flag_b, rn, rd, rm, body) \
		SingleDataSwap_TemplateName(flag_b, rn, rd, rm),
#endif

// Include the template for all values of registers and rm, etc.
#define INCLUDE_TEMPLATE "TJITGeneric_SingleDataSwap_template.h"
#define FLAG_B 0
#include "IncludeRnRdRm.h"
#undef FLAG_B
#define FLAG_B 1
#include "IncludeRnRdRm.h"
#undef FLAG_B
#undef INCLUDE_TEMPLATE

#undef SingleDataSwap_Template
#undef SingleDataSwap_TemplateName
