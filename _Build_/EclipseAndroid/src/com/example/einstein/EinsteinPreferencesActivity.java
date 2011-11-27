package com.example.einstein;

import android.os.Bundle;
import android.preference.PreferenceActivity;

/** This activity loads the "preferences.xml" file. To make this class available as an
	activity for Android you need to register it in the "AndroidManifest.xml" file. */ 
public class EinsteinPreferencesActivity extends PreferenceActivity {
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    addPreferencesFromResource(R.xml.preferences);
	}
}