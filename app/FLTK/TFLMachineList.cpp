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

#include "TFLMachineList.h"

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/Fl_Scroll.H>

#include <string>

#include "Emulator/ROM/TROMImage.h"
#include "app/FLTK/TFLSettings.h"

// A MessagePad: dark body, greenish screen, the speaker slot below it.
static const char* kMessagePadIcon = "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 24 32'>"
									 "<rect x='3' y='1' width='18' height='30' rx='3' fill='#3c3c3c'/>"
									 "<rect x='5.5' y='4' width='13' height='20' rx='1' fill='#b9c7a6'/>"
									 "<rect x='9' y='26.5' width='6' height='1.5' rx='0.75' fill='#8a8a8a'/>"
									 "</svg>";

// An eMate 300: green translucent clamshell, open, with its screen and keyboard.
static const char* kEMateIcon = "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 32 32'>"
								"<path d='M5 4 H27 L28 18 H4 Z' fill='#2e7d64'/>"
								"<rect x='8' y='6.5' width='16' height='9' fill='#b9c7a6'/>"
								"<path d='M4 18 H28 L31 27 Q31 29 29 29 H3 Q1 29 1 27 Z' fill='#3a9a7c'/>"
								"<path d='M6 21 H26 M5.5 23.5 H26.5' stroke='#2a6b56' stroke-width='1.2'/>"
								"</svg>";

static constexpr int kTitleHeight = 32;
static constexpr int kRowHeight = 52;

// Name of the ROM for the line under the machine's name.
static const char*
ROMName(int inROMId)
{
	switch (inROMId)
	{
		case TROMImage::k717006:
			return "MessagePad 2x00 (US)";
		case TROMImage::kMP2x00DROM:
			return "MessagePad 2x00 (D)";
		case TROMImage::kEMate300ROM:
			return "eMate 300";
		case TROMImage::kWatsonROM:
			return "Watson (FR)";
		default:
			return "Unknown ROM";
	}
}

/**
 One row of the list: icon, name and description, play button. The row
 handles the clicks that its play button does not take.
 */
class TFLMachineRow : public Fl_Group
{
public:
	TFLMachineRow(int x, int y, int w, int h, TFLMachineList* inList, TFLMachineSettings* inMachine) :
			Fl_Group(x, y, w, h),
			mList(inList),
			mMachine(inMachine)
	{
		box(FL_FLAT_BOX);

		auto* icon = new Fl_Box(x + 8, y + 7, 38, 38);
		Fl_SVG_Image* image = (Fl_SVG_Image*) inList->IconFor(inMachine)->copy(34, 34);
		icon->image(image);
		icon->deimage(nullptr);
		mIcon = image;

		mName = new Fl_Box(x + 52, y + 6, w - 52 - 44, 20);
		mName->copy_label(inMachine->GetName());
		mName->labelfont(FL_HELVETICA_BOLD);
		mName->labelsize(13);
		mName->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);

		int ram = (inMachine->RAMSize + 8) / 16; // 64 KB pages, rounded to MB
		std::string about = std::string(ROMName(inMachine->mROMId)) + " \xC2\xB7 " + std::to_string(ram) + " MB";
		mDescription = new Fl_Box(x + 52, y + 26, w - 52 - 44, 16);
		mDescription->copy_label(about.c_str());
		mDescription->labelsize(11);
		mDescription->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);

		mPlay = new Fl_Button(x + w - 38, y + (h - 28) / 2, 28, 28, "@>");
		mPlay->box(FL_OVAL_BOX);
		mPlay->down_box(FL_OVAL_BOX);
		mPlay->labelsize(11);
		mPlay->tooltip("Start this machine");
		mPlay->clear_visible_focus();
		mPlay->callback([](Fl_Widget* w, void*) {
			auto* row = (TFLMachineRow*) w->parent();
			row->mList->GetSettings()->StartMachine(row->mMachine);
		});
		end();
		SetSelected(inMachine == inList->GetSettings()->mEditMachine);
	}

	// Colors of a selected row (like the selection in a browser) or not.
	void
	SetSelected(bool inSelected)
	{
		color(inSelected ? FL_SELECTION_COLOR : mList->color());
		Fl_Color textColor = inSelected ? fl_contrast(FL_FOREGROUND_COLOR, FL_SELECTION_COLOR) : FL_FOREGROUND_COLOR;
		mName->labelcolor(textColor);
		mDescription->labelcolor(textColor);
		mPlay->color(inSelected ? FL_WHITE : FL_BLACK);
		mPlay->labelcolor(inSelected ? FL_SELECTION_COLOR : FL_WHITE);
		redraw();
	}

	TFLMachineSettings*
	GetMachine() const
	{
		return mMachine;
	}

	~TFLMachineRow() override
	{
		delete mIcon;
	}

	int
	handle(int event) override
	{
		if (event == FL_PUSH && (Fl::event_button() == FL_RIGHT_MOUSE || (Fl::event_state() & FL_CTRL)))
		{
			mList->GetSettings()->SelectMachine(mMachine);
			mList->OpenMenuLater(mMachine);
			return 1;
		}
		if (Fl_Group::handle(event))
			return 1;
		if (event == FL_PUSH)
		{
			if (Fl::event_clicks())
				mList->GetSettings()->StartMachine(mMachine);
			else
				mList->GetSettings()->SelectMachine(mMachine);
			return 1;
		}
		return 0;
	}

private:
	TFLMachineList* mList { nullptr };
	TFLMachineSettings* mMachine { nullptr };
	Fl_SVG_Image* mIcon { nullptr };
	Fl_Box* mName { nullptr };
	Fl_Box* mDescription { nullptr };
	Fl_Button* mPlay { nullptr };
};

TFLMachineList::TFLMachineList(int x, int y, int w, int h, TFLSettings* inSettings) :
		Fl_Group(x, y, w, h),
		mSettings(inSettings),
		mMessagePadIcon(new Fl_SVG_Image(nullptr, kMessagePadIcon)),
		mEMateIcon(new Fl_SVG_Image(nullptr, kEMateIcon))
{
	box(FL_FLAT_BOX);
	color(fl_lighter(FL_BACKGROUND_COLOR));

	auto* title = new Fl_Box(x + 12, y, w - 50, kTitleHeight, "Machines");
	title->labelfont(FL_HELVETICA_BOLD);
	title->labelsize(13);
	title->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

	auto* add = new Fl_Button(x + w - 34, y + (kTitleHeight - 24) / 2, 24, 24, "+");
	add->box(FL_FLAT_BOX);
	add->color(color());
	add->labelsize(20);
	add->tooltip("Add a machine");
	add->clear_visible_focus();
	add->callback([](Fl_Widget* w, void*) { ((TFLMachineList*) w->parent())->GetSettings()->NewMachine(); });

	mScroll = new Fl_Scroll(x, y + kTitleHeight, w, h - kTitleHeight);
	mScroll->box(FL_FLAT_BOX);
	mScroll->color(color());
	mScroll->type(Fl_Scroll::VERTICAL);
	mPack = new Fl_Pack(x, y + kTitleHeight, w - Fl::scrollbar_size(), 0);
	mPack->type(Fl_Pack::VERTICAL);
	mPack->end();
	mScroll->end();
	resizable(mScroll);
	end();
}

TFLMachineList::~TFLMachineList()
{
	delete mMessagePadIcon;
	delete mEMateIcon;
}

Fl_SVG_Image*
TFLMachineList::IconFor(const TFLMachineSettings* inMachine) const
{
	return (inMachine->mROMId == TROMImage::kEMate300ROM) ? mEMateIcon : mMessagePadIcon;
}

void
TFLMachineList::Update()
{
	mPack->clear();
	mPack->begin();
	for (auto* machine : mSettings->mMachines)
		new TFLMachineRow(mPack->x(), 0, mPack->w(), kRowHeight, this, machine);
	mPack->end();
	mPack->size(mPack->w(), (int) mSettings->mMachines.size() * kRowHeight);
	mScroll->init_sizes();
	redraw();
}

void
TFLMachineList::UpdateSelection()
{
	for (int i = 0; i < mPack->children(); i++)
	{
		auto* row = (TFLMachineRow*) mPack->child(i);
		row->SetSelected(row->GetMachine() == mSettings->mEditMachine);
	}
}

void
TFLMachineList::OpenMenuLater(TFLMachineSettings* inMachine)
{
	mMenuMachine = inMachine;
	Fl::add_timeout(0.0, [](void* inList) {
		auto* list = (TFLMachineList*) inList;
		if (list->mMenuMachine)
			list->ShowMenu(list->mMenuMachine);
		list->mMenuMachine = nullptr;
	}, this);
}

void
TFLMachineList::ShowMenu(TFLMachineSettings* inMachine)
{
#if TARGET_OS_MAC
	const char* showFolder = "Show in Finder";
#elif TARGET_OS_WIN32
	const char* showFolder = "Show in Explorer";
#else
	const char* showFolder = "Show Folder";
#endif
	Fl_Menu_Item menu[] = {
		{ "Start", 0, nullptr, (void*) 1 },
		{ "Duplicate", 0, nullptr, (void*) 2 },
		{ "Rename...", 0, nullptr, (void*) 3 },
		{ showFolder, 0, nullptr, (void*) 4, FL_MENU_DIVIDER },
		{ "Move Up", 0, nullptr, (void*) 5 },
		{ "Move Down", 0, nullptr, (void*) 6, FL_MENU_DIVIDER },
		{ "Delete...", 0, nullptr, (void*) 7 },
		{ nullptr }
	};
	const Fl_Menu_Item* picked = menu->popup(Fl::event_x(), Fl::event_y(), inMachine->GetName());
	if (!picked)
		return;
	switch ((int) (fl_intptr_t) picked->user_data())
	{
		case 1:
			mSettings->StartMachine(inMachine);
			break;
		case 2:
			mSettings->DuplicateMachine(inMachine);
			break;
		case 3:
			mSettings->RenameMachine(inMachine);
			break;
		case 4:
			mSettings->ShowMachineFolder(inMachine);
			break;
		case 5:
			mSettings->MoveMachine(inMachine, -1);
			break;
		case 6:
			mSettings->MoveMachine(inMachine, +1);
			break;
		case 7:
			mSettings->DeleteMachine(inMachine);
			break;
	}
}
