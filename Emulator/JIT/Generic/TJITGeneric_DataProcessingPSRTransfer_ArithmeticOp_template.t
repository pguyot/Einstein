// ==============================
// File:			TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp_template.t
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

#define ArithmeticOp_TemplateName(op, mode, flag_s, rn, rd) \
	DataProcessingTranfer_ ## op ## _ ## mode ## _ ## flag_s ## _ ## rn ## _ ## rd

#define ArithmeticOp_Proto(op, mode, flag_s, rn, rd) \
		JITUnit* ArithmeticOp_TemplateName(op, mode, flag_s, rn, rd) \
			(JITUnit* ioUnit, TARMProcessor* ioCPU)
#if DECLARATION
	#define ArithmeticOp(op, mode, flag_s, rn, rd) \
		ArithmeticOp_Proto(op, mode, flag_s, rn, rd);
#endif
#if IMPLEMENTATION
	#define ArithmeticOp(op, mode, flag_s, rn, rd) \
		ArithmeticOp_Proto(op, mode, flag_s, rn, rd)
#endif

#define INCLUDE_TEMPLATE "TJITGeneric_DataProcessingPSRTransfer_ArithmeticOp_template.h"

#define OP		SUB
#include "IncludeModeRnRd.h"
#undef OP
#define OP		RSB
#include "IncludeModeRnRd.h"
#undef OP
#define OP		ADD
#include "IncludeModeRnRd.h"
#undef OP
#define OP		ADC
#include "IncludeModeRnRd.h"
#undef OP
#define OP		SBC
#include "IncludeModeRnRd.h"
#undef OP
#define OP		RSC
#include "IncludeModeRnRd.h"
#undef OP

#undef INCLUDE_TEMPLATE
#undef ArithmeticOp
#undef ArithmeticOp_Proto
