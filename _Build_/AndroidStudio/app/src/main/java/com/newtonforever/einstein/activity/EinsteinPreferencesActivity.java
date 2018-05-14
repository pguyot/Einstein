package com.newtonforever.einstein.activity;

import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;

import com.newtonforever.einstein.R;
import com.newtonforever.einstein.utils.Dimension;
import com.newtonforever.einstein.utils.screen.ScreenDimensions;

import java.util.Vector;

/**
 * This activity loads the "preferences.xml" file.
 */
public class EinsteinPreferencesActivity extends PreferenceActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        addPreferencesFromResource(R.xml.preferences);
        initScreenSizePreferences();
    }

    /**
     * Initializes the emulator screen size preferences.
     */
    private void initScreenSizePreferences() {
        final Dimension hostScreenSize = ScreenDimensions.HOST_SCREEN_SIZE;
        final boolean isPortrait = hostScreenSize.width <= hostScreenSize.height;
        final int minWidth = isPortrait ? 320 : 480;
        final int minHeight = isPortrait ? 480 : 320;
        int w = minWidth;
        int h = minHeight;

        final Vector<String> temp = new Vector<String>();
        int i, currentSize = 0;
        temp.add(String.valueOf(w) + " x " + String.valueOf(h) + " (original size)"); // always add the native NewtonOS size
        double d = Math.abs(ScreenDimensions.NEWTON_SCREEN_WIDTH - w) + Math.abs(ScreenDimensions.NEWTON_SCREEN_HEIGHT - h);

        for (i = 10; i >= 3; i--) {
            int f = i / 3;
            switch (i % 3) {
                case 0:
                    w = hostScreenSize.width / f;
                    h = hostScreenSize.height / f;
                    break;
                case 1:
                    w = hostScreenSize.width * 3 / 4 / f;
                    h = hostScreenSize.height * 3 / 4 / f;
                    break;
                case 2:
                    w = hostScreenSize.width * 2 / 3 / f;
                    h = hostScreenSize.height * 2 / 3 / f;
                    break;
            }
            w = w & 0xfffffffe; // width must always be an even number
            if (w <= minWidth || h <= minHeight) continue;
            // Create a preference entry of the form "w x h", e. g. "320 x 480"
            temp.add(String.valueOf(w) + " x " + String.valueOf(h));
            double d1 = Math.abs(ScreenDimensions.NEWTON_SCREEN_WIDTH - w) + Math.abs(ScreenDimensions.NEWTON_SCREEN_HEIGHT - h);
            if (d1 < d) {
                d = d1;
                currentSize = temp.size() - 1;
            }
        }
        final int entryCount = temp.size();
        final CharSequence[] entryValues = new CharSequence[entryCount];
        final CharSequence[] entries = new CharSequence[entryCount];
        // Note that entries only contains the string the user sees, whereas entryValues contains what will be returned
        // when we query the preference. In our case both must be the same, since ScreenDimensionInitializer will query
        // the preference and parse the string to determine the Newton screen size
        for (i = 0; i < entryCount; i++) {
            entryValues[i] = temp.get(i);
            entries[i] = temp.get(i);
        }
        final ListPreference screenPresets = (ListPreference) (super.findPreference("screenpresets"));
        screenPresets.setEntries(entries);
        screenPresets.setEntryValues(entryValues);
        screenPresets.setValue(temp.get(currentSize));
    }

}
