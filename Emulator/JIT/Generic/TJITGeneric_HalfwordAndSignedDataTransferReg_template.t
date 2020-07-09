// ==============================
// File:			TJITGeneric_HalfwordAndSignedDataTransferReg_template.t
// Project:			Einstein
//
// Copyright 2003-2007, 2020 by Paul Guyot (pguyot@kallisys.net).
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

#define HalfwordAndSignedDataTransferReg_TemplateName(flags, rn, rd) \
	HalfwordAndSignedDataTransferReg_ ## flags ## _ ## rn ## _ ## rd
	
#if DECLARATION
	#define HalfwordAndSignedDataTransferReg_Template(flags, rn, rd) \
		JITInstructionProto(HalfwordAndSignedDataTransferReg_TemplateName(flags, rn, rd));
#endif
#if IMPLEMENTATION
	#define HalfwordAndSignedDataTransferReg_Template(flags, rn, rd) \
		JITInstructionProto(HalfwordAndSignedDataTransferReg_TemplateName(flags, rn, rd))
#endif
#if TRANSLATION_ARRAY
	#define HalfwordAndSignedDataTransferReg_Template(flags, rn, rd) \
		HalfwordAndSignedDataTransferReg_TemplateName(flags, rn, rd),
#endif

#define FLAG_I 0
#define INCLUDE_TEMPLATE "TJITGeneric_HalfwordAndSignedDataTransfer_template.h"
#include "IncludeRnRd63.h"
#undef FLAG_I

#undef HalfwordAndSignedDataTransferReg_Template
#undef HalfwordAndSignedDataTransferReg_TemplateName
