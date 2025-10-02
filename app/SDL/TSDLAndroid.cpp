// ==============================
// File:			TSDLAndroid.cp
// Project:			Einstein
//
// Copyright 2025 by Matthias Melcher.
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

// =============================================================================
// NOTE: This file is no longer used, but kept for reference.
// =============================================================================


#include "TSDLAndroid.h"

// Einstein
//#include "Emulator/TEmulator.h"

// SDL3
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>

// C++17
#include <string>

// Java
#include <jni.h>

/*
 * TODO: how to copy the ROM
 *
 * The ROM, Flash (and Packages) are held in SDL_GetAndroidExternalStoragePath
 * The settings and RAM image will be in SDL_GetAndroidInternalStoragePath
 *
 * Ensure that both parts are writable, and subdirectories exist as needed.
 *   - dialog box if any of these things fail.
 *
 * Find the Settings file. If it does not exist, create it.
 * Read the settings file.
 *
 * If in the setting the flag is set for loading a new ROM:
 *   - delete the old ROM file now (or save it as a backup)
 *   - dialog box that the ROM was deleted
 *   - clear flag in settings and save settings
 *   - go to "Open ROM File picker"
 * Find the ROM. If it does not exist:
 *   - dialog box about non-existing ROM, [exit] or [pick]
 *   - open ROM File picker
 *   - copy picked file to SDL_GetAndroidExternalStoragePath
 *   - go back to "Find the ROM"
 * Try to load the ROM file. If that fails (size, checksum):
 *   - dialog box about faulty ROM, [exit] or [pick other]
 *   - go to "Open ROM File picker"
 *
 * Try to boot now. If booting fails:
 *   - dialog why booting fails, [exit]
 *
 * Handle Android specific events!
 */

extern "C" {

/**
 Called by Java after file import is done onFileImported.
 */
JNIEXPORT void JNICALL
Java_org_messagepad_einstein_EinsteinActivity_onFileImported(JNIEnv* env, jobject thiz, jstring jpath) {
    const char* path = env->GetStringUTFChars(jpath, nullptr);
    SDL_Log("Imported file saved to: %s", path);
    // TODO: let the SDL loop know that there may be a valid ROM now
    env->ReleaseStringUTFChars(jpath, path);
}

} // extern "C"


/**
 Open a file picker, so the user can pick teh ROM file.

 Most users will download the ROM into the "Downloads" folder. This function opens
 the Android equivalent of a file chooser. When the user picks a file,
 EinsteinActivity::onActivityResult will call copyUriToAppStorage which
 will then copy the file into our app sandbox.
 */
void AndroidPickROMFile() {
    JNIEnv* env = (JNIEnv*)SDL_GetAndroidJNIEnv();
    jobject activity = (jobject)SDL_GetAndroidActivity();

    jclass clazz = env->GetObjectClass(activity);
    jmethodID method = env->GetMethodID(clazz, "openFilePicker", "()V");
    env->CallVoidMethod(activity, method);

    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(activity);
}


/*
 * This is the skeleton code for giving the user a dialog box, so they can
 * download files from a URL instead of using the Downloads folder.
 */
#if 0

import android.app.DownloadManager;
import android.content.Context;
import android.net.Uri;
import android.os.Environment;

public void downloadFileFromUrl(String url, String subdir) {
    DownloadManager.Request request = new DownloadManager.Request(Uri.parse(url));
    request.setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED);

    // Destination: /storage/emulated/0/Android/data/<pkg>/files/<subdir>/
    java.io.File destDir = new java.io.File(getExternalFilesDir(null), subdir);
    if (!destDir.exists()) destDir.mkdirs();

    String filename = Uri.parse(url).getLastPathSegment();
    if (filename == null) filename = "downloaded.rom";

    request.setDestinationUri(Uri.fromFile(new java.io.File(destDir, filename)));

    DownloadManager manager = (DownloadManager) getSystemService(Context.DOWNLOAD_SERVICE);
    manager.enqueue(request);
}
Emulator_DownloadRom("https://example.com/my.rom");

import android.app.AlertDialog;
import android.widget.EditText;

public void promptForUrlAndDownload(final String subdir) {
    final EditText input = new EditText(this);
    input.setHint("https://example.com/game.rom");

    new AlertDialog.Builder(this)
        .setTitle("Enter ROM URL")
        .setView(input)
        .setPositiveButton("Download", (dialog, which) -> {
            String url = input.getText().toString().trim();
            if (!url.isEmpty()) {
                downloadFileFromUrl(url, subdir);
            }
        })
        .setNegativeButton("Cancel", null)
        .show();
}

void Emulator_PromptForUrlDownload(const char* subdir) {
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject activity = (jobject)SDL_AndroidGetActivity();

    jclass clazz = env->GetObjectClass(activity);
    jmethodID method = env->GetMethodID(clazz,
        "promptForUrlAndDownload", "(Ljava/lang/String;)V");

    jstring jsubdir = env->NewStringUTF(subdir);
    env->CallVoidMethod(activity, method, jsubdir);

    env->DeleteLocalRef(jsubdir);
    env->DeleteLocalRef(clazz);
    env->DeleteLocalRef(activity);
}

if (user_pressed_download_from_url) {
    Emulator_PromptForUrlDownload("ROMs");
}

#endif

// ======================================================================= //
// "If one programmer can do it in one week, then 2 programmers            //
// can do it in 2 weeks"                                                   //
// ======================================================================= //
