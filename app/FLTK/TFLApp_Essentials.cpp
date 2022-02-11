// ==============================
// File:			TFLApp.cp
// Project:			Einstein
//
// Copyright 2003-2022 by Paul Guyot and Matthias Melcher.
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

#include "TFLApp_Essentials.h"

#include "TFLSettingsUI.h"

#include <FL/fl_ask.H>

/**
 Create and open a dialog that will give quick access to Newton packages on the Net.

 The dialog presents a hierarchy of Titles, containing Groups, which in turn can
 contain Links to external text and pdf documents, Text blocks, Scripts, and
 Installer links to packages.

 addInstallerLink requires a title and points to multiple external documents.
 The tooltip will display the links after expansion. Users expect only
 one single link though.

 addInstallerText points to a C string. Text is wrapped, and the field is
 sized automatically.

 addInstallerScript can contains multiple scripts that are run sequentially.
 The tooltip will reveal the script texts. One script must not be longer
 than 255 characters. Script lines start with 'S'. Lines starting with 'W'
 will create a Warning dialog for the user.

 addInstallerPackage can contain multiple package links that are installe
 sequentially. Links are expanded using the following rules. Lines starting with
 'W' will create a Warning dialog for the user.

 Links starting with a 'U' will have the Unna link prepended. 'M' will add
 the messegapad.or link as defined in the Settings. Staring text with a 'W'
 will create a Warning dialog to the user, so they can cancel the operation.
 Links starting with a ':' will remain unchanged.
 Scripts must start with the letter 'S'.

 \note We should make the titles, groups, and maybe even comments foldable.
 \note Eventually if would be nice to be able to link to packages inside .zip
 and .sit.hqx archives.
 \note Tooltips for scripts can get very big.
 */
void
TFLApp::UserActionInstallEssentials()
{
	if (!wInstallerWindow)
	{
		wInstallerWindow = makeInstaller();
		// --- Essentials
		addInstallerTitle("Essentials");
		// Y2K10
		addInstallerGroup("NewtonOS Y2K10 Fix");
		addInstallerText("NewtonOS has a bug in handling years past 18:48:31 on January 5, 2010. "
						 "Einstein contains a fix for US MP2x00 MessagePads, but "
						 "for eMates and German MP2x00 MessagePads, "
						 "the patch below will fix all date issues until 2026.\n\n"
						 "Please install this patch before installing anything else, "
						 "as this will wipe your Newton's memory.");
		addInstallerLink("explained by Eckhart Köppen", new StringList { ":https://40hz.org/Pages/newton/hacking/newton-year-2010-problem/" });
		addInstallerLink("Readme file for the patch", new StringList { "MDownloads/Einstein/Essentials/y2k10/README.txt" });
		addInstallerText("Please select the patch that matches the ROM image of your machine:");
		addInstallerPackage("German MP2x00 patch", new StringList { "WInstalling this patch may irreversibly erase all data\n"
			"on your MessagePad.\n\n"
			"Please proceed only if this a new device, or if your\n"
			"data is securely backed up!",
			"MDownloads/Einstein/Essentials/y2k10/Patch_D.pkg" });
		addInstallerPackage("eMate 300 patch", new StringList { "WInstalling this patch may irreversibly erase all data\n"
			"on your eMate 300.\n\n"
			"Please proceed only if this a new device, or if your\n"
			"data is securely backed up!",
			"MDownloads/Einstein/Essentials/y2k10/Patch_eMate.pkg" });
		// --- Networking
		addInstallerTitle("Networking");
		// NIE
		addInstallerGroup("NIE: Newton Internet Enabler");
		addInstallerText("The Newton Internet Enabler (NIE) allows you to "
						 "access the Internet with your Newton. "
						 "NIE package was released by Apple in 1997.");
		addInstallerPackage("Apple NIE packages", new StringList { "WThis will install four packages on your Newton\nwhich may take a little while.", "Uunna/apple/software/Internet/NIE2/ENETSUP/enetsup.pkg", "Uunna/apple/software/Internet/NIE2/REGPKGS/inetenbl.pkg", "Uunna/apple/software/Internet/NIE2/ENETSUP/newtdev.pkg", "Uunna/apple/software/Internet/NIE2/REGPKGS/inetstup.pkg" });
		addInstallerPackage("Einstein Network Card driver", new StringList { "MDownloads/Einstein/Essentials/NIE/NE2K.pkg" });
		addInstallerScript("Open Internet Setup", new StringList { "SGetRoot().|InternetSetup:NIE|:Open();" });
		// NewtonScript: Reboot(), Sleep(), PowerOff()
		// PlaySoundSync()
		// ROM_alarmWakeup
		//      ROM_click
		//      ROM_crumple
		//      ROM_drawerClose
		//      ROM_drawerOpen
		//      ROM_flip
		//      ROM_funBeep
		//      ROM_hiliteSound
		//      ROM_plinkBeep
		//      ROM_simpleBeep
		//      ROM_wakeupBeep
		//      ROM_plunk
		//      ROM_poof
		// Courier
		addInstallerGroup("Courier Browser 0.5");
		addInstallerText("Courier is a small internet browser. The source code is available on UNNA.");
		addInstallerPackage("Courier packages", new StringList { "WThis will install three packages on your Newton\nwhich may take a little while.", "Uunna/internet/web-browsers/Courier0.5/Courier0.5.pkg", "Uunna/internet/web-browsers/Courier0.5/NHttpLib-3.1.pkg", "Uunna/internet/web-browsers/Courier0.5/NTox-1.6.1.pkg" });
		addInstallerScript("Open Courier Browser", new StringList { "SGetRoot().|Courier:40Hz|:Open();" });
		// -- Developers
		addInstallerTitle("Developer Apps");
		addInstallerGroup("ViewFrame 1.3b");
		addInstallerPackage("ViewFrame packages", new StringList { "WThis will install eight packages on your Newton\nwhich may take a little while.", "Uunna/development/tools/ViewFrame1.3b/PROGKEYB.PKG", "Uunna/development/tools/ViewFrame1.3b/VFEDITOR.PKG", "Uunna/development/tools/ViewFrame1.3b/VFFUNCTI.PKG", "Uunna/development/tools/ViewFrame1.3b/VFGENERA.PKG", "Uunna/development/tools/ViewFrame1.3b/VFINTERC.PKG", "Uunna/development/tools/ViewFrame1.3b/VIEWFRAM.PKG", "Uunna/development/tools/ViewFrame1.3b/ONLYFOR2/VFDANTE.PKG", "Uunna/development/tools/ViewFrame1.3b/ONLYFOR2/VFKEYS.PKG" });
	}
	wInstallerWindow->show();
}


// ======================================================================= //
// We build our computer (systems) the way we build our cities: over time,
// without a plan, on top of ruins.
//   -- Ellen Ullman
// ======================================================================= //
