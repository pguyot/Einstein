// ==============================
// File:			TJITGeneric_STM2_template.t
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

#define STM2_TemplateName(flag_p, flag_u, rn) \
	STM2_ ## flag_p ## _ ## flag_u ## _ ## rn
	
#if DECLARATION
	#define STM2_Template(flag_p, flag_u, rn) \
		JITInstructionProto(STM2_TemplateName(flag_p, flag_u, rn));
#endif
#if IMPLEMENTATION
	#define STM2_Template(flag_p, flag_u, rn) \
		JITInstructionProto(STM2_TemplateName(flag_p, flag_u, rn))
#endif
#if TRANSLATION_ARRAY
	#define STM2_Template(flag_p, flag_u, rn) \
		STM2_TemplateName(flag_p, flag_u, rn),
#endif

#define INCLUDE_TEMPLATE	"TJITGeneric_STM2_template.h"
#define FLAG_P 0
#define FLAG_U 0
#include "IncludeRn.h"
#undef FLAG_U
#define FLAG_U 1
#include "IncludeRn.h"
#undef FLAG_U
#undef FLAG_P
#define FLAG_P 1
#define FLAG_U 0
#include "IncludeRn.h"
#undef FLAG_U
#define FLAG_U 1
#include "IncludeRn.h"
#undef FLAG_U
#undef FLAG_P

#undef INCLUDE_TEMPLATE
#undef STM2_Template
#undef STM2_TemplateName
