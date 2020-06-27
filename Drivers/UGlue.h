// =========== Header ===========
// File:                UGlue.h
// Project:             Newton Includes
// Written by:          Paul Guyot (pguyot@kallisys.net)
//
// Created on:          11/08/2000
// Internal version:    1
//
// Copyright:           © 2000-2013 by Paul Guyot.
//                      All rights reserved worldwide.
// ===========

// =========== Change History ===========
// 11/08/2000   v1  [PG]    Creation of the file
// ===========

// Fonction pour déterminer quelle est la ROM de la machine en question.
// Je me fonde sur plusieurs éléments:
//  machine type
//  taille de la RAM
//  ROM Stage
//  Version de la ROM (vérification)

// OS 2.0 seulement.

#ifndef __UGLUE__
#define __UGLUE__

#ifndef __NEWTONGESTALT_H
    #include "NewtonGestalt.h"
#endif

class UGlue
{
public:

    enum {
        kError      = -1,
        kUnknown    = 0,

        kMP120US    = 1,
        kMP130US    = 2,
        kMP2x00US   = 3,
        kEM300      = 4,
        kMP120D     = 5,
        kMP130D     = 6,
        kMP2100D    = 7
    };

    enum {
        // Internal constants used for Gestalt

        // ===== ROM Stage =====
        // ROM Stage is in fact coded like that:
        // RRRRSSUU
        // RRRR: region code, see Script.h for the list
        // SS: stage code: dev 0x20, alpha: 0x40, beta: 0x60, final/release 0x80
        // UU: unknown, maybe non final release, maybe should be zero.
        // So, a French alpha would be 0x00014000
        // and a Japanese beta would be 0x000E6000
        kUSFinal            = 0x00008000,
        kGermanFinal        = 0x00038000,

        // ===== Machine Type =====
        kMP120_130_MT       = kGestalt_MachineType_Lindy,
        kMP2x00_MT          = 0x10003000,
        kEM300_MT           = 0x10004000,

        // ==== ROM Version =====
        // ROM Version is encoded as:
        // MMMMmmmm where MMMM is the major revision in BCD and mmmm is the minor revision in BCD.

        // MP120 US has NOS 2.0
        kMP120US_RV         = 0x20000,
        // MP120 D, MP130 US & MP130 D have NOS 2.1
        kMP120D_130_RV      = 0x20001,
        // MP2x00 US & EM300 have NOS 2.2
        kMP2x00US_EM300RV   = 0x20002,
        // MP2100 D has NOS 2.3
        kMP2100D_RV         = 0x20003,

        // RAM Size - to distinguish the MP120 and the MP130
        kMP120_RS           = 0x080000,
        kMP130_RS           = 0x100000,

        // Unused
        kMP2100_RS          = 0x3E4000,
        kEM300u_RS          = 0x3E1000,

        // Offset to these values in ROM
        kgROMVersionPtr         = 0x000013DC,
        kgROMStagePtr           = 0x000013E0,
        kgHardwareTypePtr       = 0x000013EC,
        kgROMManufacturerPtr    = 0x000013F0
    };

    static  int     DetermineMachineType( void );

    static  int     DetermineMachineTypeWithoutGestalt( void );
};

#endif
        // __UGLUE__

// ======================================================== //
// As of next Tuesday, C will be flushed in favor of COBOL. //
// Please update your programs.                             //
// ======================================================== //