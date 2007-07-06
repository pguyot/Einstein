// ==============================
// File:			TJITGeneric_DataProcessingPSRTransfer_TestOp_template.t
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

#define TestOp_TemplateName(op, mode, rn) \
	DataProcessingTranfer_ ## op ## _ ## mode ## _ ## rn
	
#define TestOp_Proto(op, mode, rn) \
	JITUnit* TestOp_TemplateName(op, mode, rn) \
			(JITUnit* ioUnit, TARMProcessor* ioCPU)

#if DECLARATION
	#define TestOp(op, mode, rn, body) \
		TestOp_Proto(op, mode, rn);
#endif
#if IMPLEMENTATION
	#define TestOp(op, mode, rn, body) \
		TestOp_Proto(op, mode, rn) \
			body
#endif

#define INCLUDE_TEMPLATE "TJITGeneric_DataProcessingPSRTransfer_TestOp_template.h"

#define OP		TST
#include "IncludeModeRn.h"
#undef OP
#define OP		TEQ
#include "IncludeModeRn.h"
#undef OP
#define OP		CMP
#include "IncludeModeRn.h"
#undef OP
#define OP		CMN
#include "IncludeModeRn.h"
#undef OP

#undef INCLUDE_TEMPLATE
#undef TestOp
