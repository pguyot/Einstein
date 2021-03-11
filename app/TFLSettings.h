// ==============================
// File:			TFLSettings
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

#ifndef T_FL_SETTINGS_H
#define T_FL_SETTINGS_H

#include <K/Defines/KDefinitions.h>

#include <vector>


class Fl_Double_Window;
class Fl_Preferences;
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

    constexpr static const char *const kNetworkUUID = "3C709CD5-6573-4D91-B3B2-7B0D122C6AB5";

    TFLPCCardSettings(Fl_Preferences&);
    TFLPCCardSettings(const char *uuid, const char *name);
    TFLPCCardSettings();
    static TFLPCCardSettings* LinkLinearPCCard(const char* inName, const char* inImageFilename);
    static TFLPCCardSettings* NewLinearPCCard(const char* inName, const char* inImageFilename, KUInt32 inSizeMB);
    ~TFLPCCardSettings();

    TPCMCIACard* GetCard();

    TPCMCIACard* Card() { return mCard; }

    void WritePrefs(Fl_Preferences&);

    const char *GetUUID() { return mUUID; }

    const char* GetName() { return mName; }

    void SetName(const char* inName);

    const char* GetTag() { return mTag; }

    void SetTag(const char* inTag);

    void SetType(CardType inType) { mType = inType; }

    CardType GetType() { return mType; }

    void SetImagePath(const char *inImagePath);

    const char* GetImagePath() { return mImagePath; }

    int KeepInSlot() { return mKeepInSlot; }

    void KeepInSlot(int i) { mKeepInSlot = i; }

private:
    TPCMCIACard* mCard = nullptr;
    char *mUUID = nullptr;
    char *mName = nullptr;
    char* mImagePath = nullptr;
    char* mTag = nullptr;
    int mKeepInSlot = -1;
    CardType mType = CardType::kUndefined;
};


class TFLSettings
{
public:
    TFLSettings();
    virtual ~TFLSettings();
    virtual Fl_Double_Window *CreateSettingsPanel() = 0;
    virtual Fl_Double_Window *createAboutDialog() = 0;
    virtual void revertDialog() = 0;
    virtual void runningMode() = 0;
    virtual void HandlePopupMenu() = 0;

    void ShowAboutDialog();
    void ShowSettingsPanelModal();
    void ShowSettingsPanel();
    void setApp(TFLApp *App, const char *AppPath);
    void loadPreferences();
    void savePreferences();

    const char *GetROMDetails(const char *inFilename);

    TFLPCCardSettings* addLinearPCCard(const char* inName, const char* inImageFilename);
    void startUpdateCardList();
    void updateNextCard(const char* inName, TFLPCCardSettings* inCard);
    void endUpdateCardList();

    void UnplugPCCard(int ix);
    void KeepPCCardInSlot(int inSlot, int inCard);

    int CardToIndex(TPCMCIACard* inCard);
    int GetCardKeptInSlot(int inSlot);

//protected:
    Fl_Double_Window *mSettingsPanel = nullptr;
    Fl_Double_Window *mAboutDialog = nullptr;
    TFLApp *app = nullptr;
    char *appPath = nullptr;
    char *ROMPath = nullptr;
    char *FlashPath = nullptr;
    int screenWidth;
    int screenHeight;
    int hideMouse;
    int fullScreen;
    int RAMSize;
    int dontShow;

    int mLaunchMonitorAtBoot = 0;
    int mBreatAtROMBoot = 0;

    // some initial position for our application screen
    int mAppWindowPosX = 150;
    int mAppWindowPosY = 150;

    // user can drag the corners of the emulator window to stretch the Newton screen
    int mAllowScreenResize = true;

    // user can go into fullscreen mode and back to windowed mode
    int mAllowFullscreen = true;

    // if this is set, use the Einstein.rex within the app, otherwise load an external fiie
    int mUseBuiltinRex = true;

    std::vector<TFLPCCardSettings*> mCardList;
};


#endif // T_FL_SETTINGS_H

