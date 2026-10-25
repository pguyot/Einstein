// ==============================
// File:			TFLSettings
// Project:			Einstein
//
// Copyright 2003-2022 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef T_FL_SETTINGS_H
#define T_FL_SETTINGS_H

#include <K/Defines/KDefinitions.h>

#include <sys/types.h>
#include <string>
#include <vector>

class Fl_Double_Window;
class Fl_Preferences;
class Fl_Widget;
class TFLFileLock;
class TFLMachineList;
class TFLApp;
class TPCMCIACard;

/**
 * \brief Settings for each known PCCard.
 */
class TFLPCCardSettings
{
public:
	enum class CardType {
		kUndefined,
		kNetwork,
		kLinear
	};

	constexpr static const char* const kNetworkUUID = "3C709CD5-6573-4D91-B3B2-7B0D122C6AB5";

	TFLPCCardSettings(Fl_Preferences&);
	TFLPCCardSettings(const char* uuid, const char* name);
	TFLPCCardSettings();
	static TFLPCCardSettings* LinkLinearPCCard(const char* inName, const char* inImageFilename);
	static TFLPCCardSettings* NewLinearPCCard(const char* inName, const char* inImageFilename, KUInt32 inSizeMB);
	static TFLPCCardSettings* NewATACard(const char* inName, const char* inImageFilename, KUInt32 inSizeMB);
	~TFLPCCardSettings();

	TPCMCIACard* GetCard();

	TPCMCIACard*
	Card()
	{
		return mCard;
	}

	void WritePrefs(Fl_Preferences&);

	const char*
	GetUUID()
	{
		return mUUID;
	}

	const char*
	GetName()
	{
		return mName;
	}

	void SetName(const char* inName);

	const char*
	GetTag()
	{
		return mTag;
	}

	void SetTag(const char* inTag);

	void
	SetType(CardType inType)
	{
		mType = inType;
	}

	CardType
	GetType()
	{
		return mType;
	}

	void SetImagePath(const char* inImagePath);

	const char*
	GetImagePath()
	{
		return mImagePath;
	}

	int
	KeepInSlot()
	{
		return mKeepInSlot;
	}

	void
	KeepInSlot(int i)
	{
		mKeepInSlot = i;
	}

private:
	TPCMCIACard* mCard = nullptr;
	char* mUUID = nullptr;
	char* mName = nullptr;
	char* mImagePath = nullptr;
	char* mTag = nullptr;
	int mKeepInSlot = -1;
	CardType mType = CardType::kUndefined;
};

/**
 * \brief The settings of one emulated machine.
 *
 * Everything that belongs to one Newton: its ROM and flash, the RAM, the
 * screen, where its window was, fast start and the state file, and the
 * serial port. The settings that all machines share are in TFLSettings.
 */
class TFLMachineSettings
{
public:
	// A machine with this UUID, in <inUserDataPath>Machines/<inUUID>/.
	TFLMachineSettings(const char* inUUID, const char* inUserDataPath);
	~TFLMachineSettings();

	// Read or write the machine's own file, Machine.prefs in its folder:
	// its name, and the same groups as Load() and Save(). Saving creates the
	// folder if needed.
	void LoadFromFolder(const char* inAppPath);
	void SaveToFolder() const;

	// The folder of this machine, ending in a slash.
	const std::string&
	GetFolder() const
	{
		return mFolder;
	}

	const char*
	GetUUID() const
	{
		return mUUID.c_str();
	}

	const char*
	GetName() const
	{
		return mName.c_str();
	}

	void
	SetName(const char* inName)
	{
		mName = inName ? inName : "";
	}

	// Read the settings from the ROM, Flash, Screen, Memory, System and Serial
	// groups below inPrefs. inAppPath and inUserDataPath end in a slash; they
	// give the default ROM and flash files.
	void Load(Fl_Preferences& inPrefs, const char* inAppPath, const char* inUserDataPath);

	// Write the settings to the same groups below inPrefs.
	void Save(Fl_Preferences& inPrefs) const;

	// Create the machine's folder if it does not exist yet.
	void CreateFolder() const;

	// Give the machine a new, random Newton ID (for a new machine).
	void MakeRandomNewtonID();

	void SetExtrTcpServer(const char* inServer);

	// Set the state file used by "Save State" and "Restore Last".
	void SetStatePath(const char* inPath);

	char* ROMPath = nullptr;
	char* FlashPath = nullptr;
	int screenWidth = 320;
	int screenHeight = 480;
	int screenScale = 100;
	int fullScreen = 0;
	int RAMSize = 64;

	// if this is set, use the Einstein.rex within the app, otherwise load an external fiie
	int mUseBuiltinRex = true;

	// show the eMate's Special Keys under the screen (eMate ROMs only)
	int mShowSpecialKeys = 1;

	// when quitting, save the state of the sleeping Newton, and continue from
	// there on the next start (if nothing changed)
	int mFastStart = 1;

	// the state file used by "Save State" and "Restore Last": the last one
	// saved or restored. Empty for the default file in the user data folder.
	char* mStatePath = nullptr;

	// driver for the external serial port (TSerialPorts::EDriverID), default is the TCP client
	int mExtrDriver = 4;

	// server address and port used if the external serial port driver is the TCP client
	char* mExtrTcpServer = nullptr;
	int mExtrTcpPort = 3679;

	// some initial position for our application screen
	int mAppWindowPosX = 150;
	int mAppWindowPosY = 150;

	// the screen orientation (TScreenManager::EOrientation) when Einstein was
	// last quit, and the ROM (TROMImage::GetROMId()) that it belongs to. The
	// next start uses it only with the same ROM. -1 if not known.
	int mScreenOrientation = -1;
	int mScreenOrientationROM = -1;

	// The ROM it ran last or that the Settings found (TROMImage::GetROMId(),
	// -1 if not known), for the icon and the description in the list.
	int mROMId = -1;

	// The cards that this machine keeps in its slots: their UUIDs in the
	// card list (TFLPCCardSettings::GetUUID()), empty for none.
	std::string mKeepInSlot[2];

	// The Newton ID (48 bits, 16+32), which the desktop software uses to tell
	// Newtons apart. The machine made from the old settings keeps the one
	// Einstein always had, "Newton" in ASCII, so its backups still match.
	KUInt32 mNewtonID[2] { 0x00004E65, 0x77746F6E };

private:
	TFLMachineSettings(const TFLMachineSettings&) = delete;
	TFLMachineSettings& operator=(const TFLMachineSettings&) = delete;

	std::string mUUID;
	std::string mName;
	std::string mFolder;
};

class TFLSettings
{
public:
	TFLSettings();
	virtual ~TFLSettings();
	virtual Fl_Double_Window* CreateSettingsPanel() = 0;
	virtual Fl_Double_Window* createAboutDialog() = 0;
	virtual void revertDialog() = 0;
	virtual void applyDialog() = 0;
	virtual void updateEditable() = 0;
	virtual void updateHeadline() = 0;
	virtual void runningMode() = 0;
	virtual void HandlePopupMenu() = 0;

	void ShowAboutDialog();

	// Show the Settings and wait until they close. Only one Settings window
	// may be open, in all Einstein processes together.
	// \return false if the Settings are open in another Einstein
	bool ShowSettingsPanelModal();

	// Show the Settings while a machine runs (see ShowSettingsPanelModal()).
	void ShowSettingsPanel();

	// Releases the Settings lock once the window that ShowSettingsPanel()
	// opened is closed.
	static void SettingsLockTimer(void* inSettings);
	void setAppPath(const char* AppPath);
	void loadPreferences();
	void savePreferences();

	// The machine list in the preferences, see TFLSettings.cpp
	void LoadMachines(Fl_Preferences& inPrefs);
	void SaveMachines(Fl_Preferences& inPrefs);

	// Save only the machine that runs (when it quits, or when NewtonOS
	// changed its settings), not the list or the shared settings, which
	// only the Settings window changes. See TFLSettings.cpp.
	void SaveRunningMachine();

	// Is this machine running in another Einstein process?
	bool IsRunningElsewhere(const TFLMachineSettings* inMachine) const;

	// Is this machine running, here or in another Einstein?
	bool IsRunning(const TFLMachineSettings* inMachine) const;

	// A machine stopped running in another Einstein, which saved it: read it
	// again.
	void MachineStopped(TFLMachineSettings* inMachine);

	// The machine that ran here was closed, but the Settings window stays
	// open. Its machine can be edited and started again (in a new Einstein).
	void EmulationEnded();

	// Read the machine list again, which another Einstein may have changed.
	// The machine that runs here stays as it is.
	void RefreshMachines();

	// The machine list on the left of the Settings window, see
	// TFLMachineList and TFLSettings.cpp.
	void CreateMachineList(Fl_Widget* inSidebarButton);
	void ToggleSidebar();
	void ShowSidebar(bool inOpen);

	// The actions of the machine list. Changes to the list are saved at once;
	// changes in the tabs wait for Save or Start.
	void SelectMachine(TFLMachineSettings* inMachine);
	void StartMachine(TFLMachineSettings* inMachine);
	void NewMachine();
	void DuplicateMachine(TFLMachineSettings* inMachine);
	void RenameMachine(TFLMachineSettings* inMachine);
	void ShowMachineFolder(TFLMachineSettings* inMachine);
	void MoveMachine(TFLMachineSettings* inMachine, int inDelta);
	void DeleteMachine(TFLMachineSettings* inMachine);

	const char* GetROMDetails(const char* inFilename);

	TFLPCCardSettings* addLinearPCCard(const char* inName, const char* inImageFilename);
	void startUpdateCardList();
	void updateNextCard(const char* inName, TFLPCCardSettings* inCard);
	void endUpdateCardList();

	void UnplugPCCard(int ix);
	void KeepPCCardInSlot(int inSlot, size_t inCard);

	int CardToIndex(TPCMCIACard* inCard);
	int GetCardKeptInSlot(int inSlot);

	// In which slot does this machine keep this card? 0, 1, or -1 for none.
	static int SlotKeepingCard(const TFLMachineSettings* inMachine, TFLPCCardSettings* inCard);

	// Lock or unlock the image of a card that is inserted into a slot, see
	// TFLSettings.cpp.
	bool LockCard(int inSlot, TFLPCCardSettings* inCard);
	void UnlockCard(int inSlot);

	// protected:
	Fl_Double_Window* mSettingsPanel = nullptr;
	Fl_Double_Window* mAboutDialog = nullptr;
	char* appPath = nullptr;

	// Einstein's folder for user data, ending in a slash
	std::string mUserDataPath;

	// held while the Settings window is open (Settings.lock in the user data
	// folder), see ShowSettingsPanelModal()
	TFLFileLock* mSettingsLock = nullptr;

	// held while a card is inserted into a slot (Cards/<uuid>.lock)
	TFLFileLock* mCardLock[2] = { nullptr, nullptr };

	// the machine list, its button, and whether it is open: 1 or 0 as the
	// user chose, -1 if not chosen yet (open if there is more than one machine)
	TFLMachineList* mMachineList = nullptr;
	Fl_Widget* mSidebarButton = nullptr;
	int mSidebarOpen = -1;
	bool mSidebarShown = false;

	// the Settings window was opened while a machine runs in this Einstein
	bool mRunningMode = false;

	// the machine of this Einstein was closed, see EmulationEnded()
	bool mEmulationEnded = false;

	// All machines, in the order of the list. The first one is also stored
	// in the old preference keys, so an older Einstein launches it.
	std::vector<TFLMachineSettings*> mMachines;

	// The machine that this Einstein runs, one of mMachines.
	TFLMachineSettings* mMachine = nullptr;

	// The machine that the Settings dialog shows and changes, one of
	// mMachines (the one selected in the list).
	TFLMachineSettings* mEditMachine = nullptr;

	int hideMouse;
	int dontShow;
	int mShowMenubar = 1;
	int mShowToolbar = 1;

	// the Settings dialog: the ROM that is chosen there
	// (TROMImage::GetROMId(), -1 if none), and the Special Keys checkbox
	// while the chosen ROM is not an eMate's
	int mDialogROMId = -1;
	int mDialogShowSpecialKeys = 1;

	int mLaunchMonitorAtBoot = 0;
	int mBreakAtROMBoot = 0;

	// user can drag the corners of the emulator window to stretch the Newton screen
	int mAllowScreenResize = true;

	// user can go into fullscreen mode and back to windowed mode
	int mAllowFullscreen = true;

	// path to the unna archive, local or online
	char* mUnnaPath = nullptr;

	// path to the messagepad.org website
	char* mMessagepadOrgPath = nullptr;

	// path to the git repository, currently on GitHub
	char* mGitRepoPath = nullptr;

	// location of ARM cross assembler
	char* mDevAsmPath = nullptr;

	// location of ARM cross objcopy tool
	char* mDevObjCopyPath = nullptr;

	// location of ARM cross objdump tool
	char* mDevObjDumpPath = nullptr;

	std::vector<TFLPCCardSettings*> mCardList;
};

#endif // T_FL_SETTINGS_H
