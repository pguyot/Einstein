package com.example.einstein.constants;

import android.content.res.Resources;
import android.content.res.Resources.NotFoundException;

public class StringConstants {

	// TODO Eventually we need to localize these strings. Can the Android VM handle .properties files?

	public static final String Einstein_001 = "Downloading file..";
	public static final String Einstein_002 = "Download the ROM";
	public static final String Einstein_003 = "Enter the URL of the Newton ROM file";
	public static final String Einstein_004 = "OK";
	// TODO Do not hard-code the ROM name. It might change.
	public static final String Einstein_005 = "ROM file not found.\n\nPlease copy the files\n\n\"717006.rom\" and " +
	"\"Einstein.rex\"\n\nto\n\n\"/Download/Einstein\"\n\non your Android device.";
	public static final String Einstein_006 = "EXPERIMENTAL CODE\n\nThis is a very early prerelease of Einstein for Android. " +
	"First boot may take up to six minutes, later cold boots will be quicker.";

	public static String getLocalizedString(Resources resources, int id) {
		try {
			return resources.getString(id);
		} catch (NotFoundException e) {
			return "Text resource not found";
		}
	}
}
