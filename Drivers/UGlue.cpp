// =========== Header ===========
// File:                UGlue.cp
// Project:             Newton Includes
// Written by:          Paul Guyot (pguyot@kallisys.net)
//
// Created on:          11/08/2000
// Internal version:    1
//
// Copyright:           � 2000-2013 by Paul Guyot.
//                      All rights reserved worldwide.
// ===========

// =========== Change History ===========
// 11/08/2000   v1  [PG]    Creation of the file
// ===========

#include "UGlue.h"

// ============================================================ //
// int DetermineMachineType ( void ) [static]
// ============================================================ //
// Function to determine which machine we are running on.
// I only do that for ROM, hence I won't distinguish MP2000 US and MP2100 US.
// And I won't distinguish EM300 and EM300u.
// Returns kError if something wrong happened and kUnknown if the machine is not one of the 7 I know.

int UGlue::DetermineMachineType( void )
{
    // I simply call Gestalt.
    TUGestalt myGestalt;                // Create a Gestalt object on the stack.
    TGestaltSystemInfo mySystemInfo;    // Create a System Info structure on the stack.

    NewtonErr theErr = myGestalt.Gestalt( kGestalt_SystemInfo, &mySystemInfo, sizeof(mySystemInfo) );
        // Gestalt, tell me what I am running on.

    int theResult = kError;             // Let's suppose something wrong happened.

    if (theErr == noErr)
    {
        theResult = kUnknown;           // Default answer.

        if (mySystemInfo.fManufacturer == kGestalt_Manufacturer_Apple)
        {
            switch (mySystemInfo.fMachineType)
            {
                case kMP120_130_MT:
                    if (mySystemInfo.fROMStage == kUSFinal)
                    {
                        if (mySystemInfo.fRAMSize == kMP120_RS)
                        {
                            theResult = kMP120US;
                        } else if (mySystemInfo.fRAMSize == kMP130_RS) {
                            theResult = kMP130US;
                        }
                    } else if (mySystemInfo.fROMStage == kGermanFinal) {
                        if (mySystemInfo.fRAMSize == kMP120_RS)
                        {
                            theResult = kMP120D;
                        } else if (mySystemInfo.fRAMSize == kMP130_RS) {
                            theResult = kMP130D;
                        }
                    }
                    break;

                case kMP2x00_MT:
                    if (mySystemInfo.fROMStage == kUSFinal)
                    {
                        theResult = kMP2x00US;
                    } else if (mySystemInfo.fROMStage == kGermanFinal) {
                        theResult = kMP2100D;
                    }
                    break;

                case kEM300_MT:
                    if (mySystemInfo.fROMStage == kUSFinal)
                    {
                        theResult = kEM300;
                    }
            }
        }
    }

    return theResult;
}

// ============================================================ //
// int DetermineMachineTypeWithoutGestalt ( void ) [static]
// ============================================================ //
// Function to determine which machine we are running on.
// I only do that for ROM, hence I won't distinguish MP2000 US and MP2100 US.
// And I won't distinguish EM300 and EM300u.
// Returns kError if something wrong happened and kUnknown if the machine is not one of the 7 I know.

int
UGlue::DetermineMachineTypeWithoutGestalt( void )
{
    // Access the ROM directly.
    ULong theROMVersion = *((ULong*) kgROMVersionPtr);
    ULong theROMStage = *((ULong*) kgROMStagePtr);
    ULong theMachineType = *((ULong*) kgHardwareTypePtr);
    ULong theManufacturer = *((ULong*) kgROMManufacturerPtr);

    int theResult = kUnknown;           // Default answer.

    if (theManufacturer == kGestalt_Manufacturer_Apple)
    {
        switch (theMachineType)
        {
            case kMP120_130_MT:
                if (theROMStage == kUSFinal)
                {
//                      if (mySystemInfo.fRAMSize == kMP120_RS)
//                      {
                        theResult = kMP120US;
//                      } else if (mySystemInfo.fRAMSize == kMP130_RS) {
//                          theResult = kMP130US;
//                      }
                } else if (theROMStage == kGermanFinal) {
//                      if (mySystemInfo.fRAMSize == kMP120_RS)
//                      {
                        theResult = kMP120D;
//                      } else if (mySystemInfo.fRAMSize == kMP130_RS) {
//                          theResult = kMP130D;
//                      }
                }
                break;

            case kMP2x00_MT:
                if (theROMStage == kUSFinal)
                {
                    theResult = kMP2x00US;
                } else if (theROMStage == kGermanFinal) {
                    theResult = kMP2100D;
                }
                break;

            case kEM300_MT:
                if (theROMStage == kUSFinal)
                {
                    theResult = kEM300;
                }
        }
    }

    return theResult;
}
// ======================================= //
// Even bytes get lonely for a little bit. //
// ======================================= //
