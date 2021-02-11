// ==============================
// File:			TPCMCIACardList.cp
// Project:			Einstein
//
// Copyright 2021 by Matthias Melcher (einstein@matthiasm.com).
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

#include "TPCMCIACardList.h"

// Einstein
#include "Emulator/PCMCIA/TPCMCIACard.h"
#include "Emulator/PCMCIA/TNE2000Card.h"
#include "Emulator/PCMCIA/TLinearCard.h"



/*
 * Constructor
 */
TPCMCIACardList::TPCMCIACardList(TEmulator* emulator, TLog* log)
	:
	mEmulator(emulator),
	mLog(log)
{
}


/*
 * Destructor.
 */
TPCMCIACardList::~TPCMCIACardList()
{
	for (auto& card : mList)
		delete card;
}


void TPCMCIACardList::addDefaultCards()
{
	// FIXME: error checking
	mList.push_back(new TNE2000Card());
}


void TPCMCIACardList::addMemoryCard(const char* inFilename)
{
	// FIXME: error checking
	mList.push_back(new TLinearCard(inFilename));
}



// ================================================================== //
//                                                                    //
// G.2.2.1 Enabling Write-Protect Mode                                //
//                                                                    //
// Using a fine - tipped tool(such as a screwdriver), slide the       //
// write-protect switch toward the edge of the PCMCIA memory card,    //
// as shown in Figure G-1 to enable the write-protect mode.           //
//                                                                    //
// G.2.2.2 Disabling Write-Protect Mode                               //
//                                                                    //
// Using a fine - tipped tool(such as a screwdriver), slide the       //
// write-protect switch away from the edge of the PCMCIA memory card, //
// to disable the write-protect mode.                                 //
//                                                                    //
//  - Oracle Users Guide for PCMCIA Cards                             //
//                                                                    //
// -------------------------------------------------------------------//
//                                                                    //
// "Hold stick near centre of its length. Moisten pointed end in      //
// mouth. Insert in tooth space, blunt end next to gum. Use gentle    //
// in-out motion.”                                                    //
//                                                                    //
//   — Hitchhikers Guide to the Galaxy                                //
//                                                                    //
// ================================================================== //
