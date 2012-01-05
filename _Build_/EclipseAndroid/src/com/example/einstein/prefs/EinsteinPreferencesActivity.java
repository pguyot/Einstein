package com.example.einstein.prefs;

import java.util.Vector;

import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;

import com.example.einstein.DebugUtils;
import com.example.einstein.R;
import com.example.einstein.R.xml;
import com.example.einstein.utils.screen.ScreenDimensions;
import com.missinginandroid.Dimension;

/** This activity loads the "preferences.xml" file. To make this class available as an
	activity for Android you need to register it in the "AndroidManifest.xml" file. */ 
public class EinsteinPreferencesActivity extends PreferenceActivity {
	
	public EinsteinPreferencesActivity() {
		super();
		DebugUtils.appendLog("EinsteinPreferencesActivity: Leaving constructor");
	}
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		DebugUtils.appendLog("EinsteinPreferencesActivity.onCreate: Entered method");
	    super.onCreate(savedInstanceState);
	    addPreferencesFromResource(R.xml.preferences);
		DebugUtils.appendLog("EinsteinPreferencesActivity.onCreate: Calling initDeviceDependentPreferences");
	    this.initDeviceDependentPreferences();
	}
	
	/** Initializes all preferences that we can only know at runtime. */
	public void initDeviceDependentPreferences() {
		DebugUtils.appendLog("EinsteinPreferencesActivity.initDeviceDependentPreferences: Calling initScreenSizePreferences");
		this.initScreenSizePreferences();
	}

	/** Initializes the emulator screen size preferences. */
	private final void initScreenSizePreferences() {
		DebugUtils.appendLog("EinsteinPreferencesActivity: Entering initScreenSizePreferences");
		final Dimension hostScreenSize = ScreenDimensions.HOST_SCREEN_SIZE;
		final boolean isPortrait = hostScreenSize.width > hostScreenSize.height;
		final int minWidth = isPortrait ? 320 : 480;
		final int minHeight = isPortrait ? 480 : 320;
		final int widthIncrease = minWidth / 2;
		final int heightIncrease = minHeight / 2;
		int w = minWidth;
		int h = minHeight;
		DebugUtils.appendLog("EinsteinPreferencesActivity: Calculating entries");
		final Vector<String>temp = new Vector<String>();
		while (w <= hostScreenSize.width && h <= hostScreenSize.height) {
			temp.add(String.valueOf(w) + " x " + String.valueOf(h));
			DebugUtils.appendLog("EinsteinPreferencesActivity: Entry w = " + w + " h = " + h);		
			w += widthIncrease;
			h += heightIncrease;
		}
		final int entryCount = temp.size();
		DebugUtils.appendLog("EinsteinPreferencesActivity: Preparing " + entryCount + " preference entries");		
		final CharSequence[] entryValues = new CharSequence[entryCount];
		final CharSequence[] entries = new CharSequence[entryCount];
		for (int i = 0; i < entryCount; i++) {
			entryValues[i] = String.valueOf(i + 1);
			entries[i] = temp.get(i);
		}
		DebugUtils.appendLog("EinsteinPreferencesActivity: Setting preference entries");
		final ListPreference screenPresets = (ListPreference)(super.findPreference("screenpresets"));
		screenPresets.setEntries(entries);
		screenPresets.setEntryValues(entryValues);
		DebugUtils.appendLog("EinsteinPreferencesActivity: Leaving EinsteinPreferencesActivity");		
	}

}