// ==============================
// File:			TFLMachineList
// Project:			Einstein
//
// Copyright 2026 by Matthias Melcher.
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

#ifndef T_FL_MACHINE_LIST_H
#define T_FL_MACHINE_LIST_H

#include <FL/Fl_Group.H>

class Fl_PNG_Image;
class Fl_Pack;
class Fl_Scroll;
class TFLSettings;
class TFLMachineSettings;

/**
 \brief The list of machines on the left of the Settings window.

 A title with "+" to add a machine, and below it one row per machine in an
 Fl_Pack inside an Fl_Scroll. Each row shows an icon (MessagePad or eMate,
 from the machine's ROM), the name, a line about the machine, and a play
 button. Clicking a row selects the machine for the tabs on the right;
 double-clicking it or its play button starts it. A right click opens the
 menu with the actions for the machine.
 */
class TFLMachineList : public Fl_Group
{
public:
	TFLMachineList(int x, int y, int w, int h, TFLSettings* inSettings);
	~TFLMachineList() override;

	// Width of the list when it is shown.
	static constexpr int kWidth = 260;

	// Rebuild the rows after machines were added, removed, renamed or moved.
	void Update();

	// Show which machine is selected, without rebuilding the rows.
	void UpdateSelection();

	// Open the menu with the actions for a machine, after the current event
	// (the actions may rebuild the rows).
	void OpenMenuLater(TFLMachineSettings* inMachine);

	TFLSettings*
	GetSettings() const
	{
		return mSettings;
	}

	Fl_PNG_Image* IconFor(const TFLMachineSettings* inMachine) const;

private:
	void ShowMenu(TFLMachineSettings* inMachine);

	// Once a second: show which machines run (their lock files).
	static void WatchRunning(void* inList);

	TFLSettings* mSettings { nullptr };
	TFLMachineSettings* mMenuMachine { nullptr };
	Fl_Scroll* mScroll { nullptr };
	Fl_Pack* mPack { nullptr };
	Fl_PNG_Image* mMessagePadIcon { nullptr };
	Fl_PNG_Image* mEMateIcon { nullptr };
};

#endif // T_FL_MACHINE_LIST_H
