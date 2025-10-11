// ==============================
// File:			TPCMCIACard.cp
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

//
// PCMCIA Cards are 54mm x 85.5mm big.
// The MP display is 480x320 pixel at 4.8" x 3.2" (100dpi).
// So one screen pixel is 3.2/320*25.4 ~= 0.25mm wide.
// The PCMCIA graphics should then be 342 x 216 pixels
//
// HuRes MessagePad image on German Wikipedia:
// "Von Elvis untot auf Wikipedia auf Deutsch - selbst fotographiert,
// CC BY - SA 2.0 de, https://commons.wikimedia.org/w/index.php?curid=17730048"
//

#include "TPCMCIACard.h"

// Einstein
#include "Emulator/PCMCIA/TPCMCIAController.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TPCMCIACard( void )
// -------------------------------------------------------------------------- //
TPCMCIACard::TPCMCIACard(void)
{
}

// -------------------------------------------------------------------------- //
//  * ~TPCMCIACard( void )
// -------------------------------------------------------------------------- //
TPCMCIACard::~TPCMCIACard(void)
{
}

// -------------------------------------------------------------------------- //
//  * Init( TPCMCIAController* )
// -------------------------------------------------------------------------- //
int
TPCMCIACard::Init(TPCMCIAController* inController)
{
	mLog = inController->GetLog();
	mEmulator = inController->GetEmulator();
	mPCMCIAController = inController;
	return 0;
}

// ======================== //
// You've been Berkeley'ed! //
// ======================== //
