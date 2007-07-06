// ==============================
// File:			TJITGeneric_DataProcessingPSRTransfer_MRS_template.t
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

#define MRS_TemplateName(flag_r, rd) \
	DataProcessingTranfer_MRS_ ## flag_r ## _ ## rd
	
#define MRS_Proto(flag_r, rd) \
		JITUnit* MRS_TemplateName(flag_r, rd) \
			(JITUnit* ioUnit, TARMProcessor* ioCPU)
#if DECLARATION
	#define MRS(flag_r, rd, body) \
		MRS_Proto(flag_r, rd);
#endif
#if IMPLEMENTATION
	#define MRS(flag_r, rd, body) \
		MRS_Proto(flag_r, rd) \
			body
#endif

#define INCLUDE_TEMPLATE "TJITGeneric_DataProcessingPSRTransfer_MRS_template.h"

#define FLAG_R	0
#include "IncludeRd.h"
#undef FLAG_R
#define FLAG_R	1
#include "IncludeRd.h"
#undef FLAG_R

#undef INCLUDE_TEMPLATE
#undef MRS
#undef MRS_Proto