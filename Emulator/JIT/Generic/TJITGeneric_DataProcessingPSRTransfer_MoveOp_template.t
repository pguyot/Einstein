// ==============================
// File:			TJITGeneric_DataProcessingPSRTransfer_MoveOp_template.t
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

#define MoveOp_TemplateName(op, mode, flag_s, rd) \
	DataProcessingTranfer_ ## op ## _ ## mode ## _ ## flag_s ## _ ## rd
	
#define MoveOp_Proto(op, mode, flag_s, rd) \
	JITUnit* MoveOp_TemplateName(op, mode, flag_s, rd) \
		(JITUnit* ioUnit, TARMProcessor* ioCPU)

#if DECLARATION
	#define MoveOp(op, mode, flag_s, rd, body) \
		MoveOp_Proto(op, mode, flag_s, rd);
#endif
#if IMPLEMENTATION
	#define MoveOp(op, mode, flag_s, rd, body) \
		MoveOp_Proto(op, mode, flag_s, rd) \
			body
#endif

#define INCLUDE_TEMPLATE "TJITGeneric_DataProcessingPSRTransfer_MoveOp_template.h"

#define OP		MOV
#include "IncludeModeRd.h"
#undef OP
#define OP		MVN
#include "IncludeModeRd.h"
#undef OP

#undef INCLUDE_TEMPLATE
#undef MoveOp
#undef MoveOp_Proto
