// ==============================
// File:			TJITGeneric_DataProcessingPSRTransfer_MSR_template.t
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

#define MSR_TemplateName(mode, flag_r, fields_mask, rm) \
	DataProcessingTranfer_MSR_ ## mode ## _ ## flag_r ## _ ## fields_mask ## _ ## rm

#define MSR_Proto(mode, flag_r, fields_mask, rm) \
		JITUnit* MSR_TemplateName(mode, flag_r, fields_mask, rm) \
			(JITUnit* ioUnit, TARMProcessor* ioCPU)
#if DECLARATION
	#define MSR(mode, flag_r, fields_mask, rm) \
		MSR_Proto(mode, flag_r, fields_mask, rm);
#endif
#if IMPLEMENTATION
	#define MSR(mode, flag_r, fields_mask, rm) \
		MSR_Proto(mode, flag_r, fields_mask, rm)
#endif

#define INCLUDE_TEMPLATE "TJITGeneric_DataProcessingPSRTransfer_MSR_template.h"

#define FLAG_R	0
#define MODE Imm
#include "IncludeMask.h"
#undef MODE
#define MODE Regular
#include "IncludeMaskRm.h"
#undef MODE
#undef FLAG_R
#define FLAG_R	1
#define MODE Imm
#include "IncludeMask.h"
#undef MODE
#define MODE Regular
#include "IncludeMaskRm.h"
#undef MODE
#undef FLAG_R

#undef INCLUDE_TEMPLATE
#undef MSR
#undef MSR_Proto
