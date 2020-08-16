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


#include <vector>


class Fl_Double_Window;
class Fl_Preferences;
class TFLApp;


/**
 * \brief Settings for each known PCCard.
 */
class TFLPCCardSettings
{
public:
    constexpr static const char *const kNetworkUUID = "3C709CD5-6573-4D91-B3B2-7B0D122C6AB5";

    static const int kUnknownCard = 0;
    static const int kNetworkCard = 1;
    static const int kLinearFlashCard = 2;

    TFLPCCardSettings(Fl_Preferences&);
    TFLPCCardSettings(const char *uuid, const char *name);
    TFLPCCardSettings();
    ~TFLPCCardSettings();

    void WritePrefs(Fl_Preferences&);

    const char *GetUUID() { return mUUID; }

    const char *GetName() { return mName; }

    void SetType(int type) { mType = type; }

private:

    char *mUUID = nullptr;
    char *mName = nullptr;
    int mType = kUnknownCard;
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

    void UnplugPCCard(int ix);

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
    int useMonitor;
    int fullScreen;
    int RAMSize;
    int dontShow;

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

