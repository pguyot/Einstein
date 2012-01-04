package com.example.einstein;

import java.util.Vector;

import android.graphics.Point;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;

import com.example.einstein.utils.ScreenUtils;

/** This activity loads the "preferences.xml" file. To make this class available as an
	activity for Android you need to register it in the "AndroidManifest.xml" file. */ 
public class EinsteinPreferencesActivity extends PreferenceActivity {
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    addPreferencesFromResource(R.xml.preferences);
	    this.initScreenSizePresets();
	}
	

	/** Initializes number and text of the screen size preference entries. */
	private final void initScreenSizePresets() {
		DebugUtils.appendLog("EinsteinPreferencesActivity: Entering initScreenSizePresets");		
		final Point hostScreenSize = ScreenUtils.getHostScreenSize(this);
		final boolean isPortrait = hostScreenSize.y > hostScreenSize.x;
		final int minWidth = isPortrait ? 320 : 480;
		final int minHeight = isPortrait ? 480 : 320;
		final int widthIncrease = minWidth / 2;
		final int heightIncrease = minHeight / 2;
		int w = minWidth;
		int h = minHeight;
		final Vector<String> temp = new Vector<String>();
		DebugUtils.appendLog("EinsteinPreferencesActivity: Calculating entries");		
		while (w <= hostScreenSize.x && h <= hostScreenSize.y) {
			temp.add(String.valueOf(w) + " x " + String.valueOf(h));
			DebugUtils.appendLog("EinsteinPreferencesActivity: Entry w = " + w + " h = " + h);		
			w += widthIncrease;
			h += heightIncrease;
		}
		final int entryCount = temp.size();
		DebugUtils.appendLog("EinsteinPreferencesActivity: Preparing preference entries");		
		final CharSequence[] entryValues = new CharSequence[entryCount];
		final CharSequence[] entries = new CharSequence[entryCount];
		for (int i = 0; i < entryCount; i++) {
			entryValues[i] = String.valueOf(i + 1);
			entries[i] = temp.get(i);
		}
		final ListPreference screenPresets = (ListPreference)(super.findPreference("screenpresets"));
		DebugUtils.appendLog("EinsteinPreferencesActivity: Setting preference entries");
		screenPresets.setEntries(entries);
		screenPresets.setEntryValues(entryValues);
		DebugUtils.appendLog("EinsteinPreferencesActivity: Leaving EinsteinPreferencesActivity");		
	}

}