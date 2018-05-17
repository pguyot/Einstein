// TODO FG Review

package com.newtonforever.einstein.utils.screen;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.util.DisplayMetrics;

import com.newtonforever.einstein.utils.Dimension;
import com.newtonforever.einstein.utils.debug.DebugUtils;

/**
 * @brief Find the sizes of the Newton and the host screen.
 * 
 * This class uses the preferences to find the size of the Newton screen.
 * It also searches the Display Metrics database to initialize the Host screen size.
 * 
 * @author matt
 *
 */
public class ScreenDimensionsInitializer {
	
    private ScreenDimensionsInitializer() {
		// No instances, please;
    }
    
    public static void initScreenDimensions(Context context) {
		initHostScreenDimensions(context);
		initNewtonScreenDimensions(context);
    }
    
	/** Returns the size of the Newton screen. This value depends on the setting in the preferences. */
	public static Dimension getNewtonScreenSize(SharedPreferences prefs) {
		final String value = prefs.getString("screenpresets", "1");
		DebugUtils.appendLog("ScreenDimensionInitializer.getNewtonScreenSize: Currently set preference is " + value);		
		final int separatorPosition = value.indexOf(" x ");
		final String sw = value.substring(0, separatorPosition - 1);
		DebugUtils.appendLog("ScreenDimensionInitializer.getNewtonScreenSize: Width got from preference string is " + sw);		
		final String sh = value.substring(separatorPosition + 3, value.length() - 1);
		DebugUtils.appendLog("ScreenDimensionInitializer.getNewtonScreenSize: Height got from preference string is " + sh);		
		final int w = Integer.valueOf(sw);
		final int h = Integer.valueOf(sh);
		final Dimension size = new Dimension(w, h);
		DebugUtils.appendLog("ScreenDimensionInitializer.getNewtonScreenSize: Returning " + size);		
		return size;
	}
    
    /** Initializes the host screen dimensions. */
    private static void initHostScreenDimensions(Context context) {
		if (null == context) {
			DebugUtils.appendLog("ScreenDimensionInitializer.initHostScreenDimensions: context is null");
			return;
		}
		final DisplayMetrics metrics = context.getResources().getDisplayMetrics();
		ScreenDimensions.HOST_SCREEN_WIDTH = metrics.widthPixels;
		ScreenDimensions.HOST_SCREEN_HEIGHT = metrics.heightPixels;
		ScreenDimensions.HOST_SCREEN_SIZE = new Dimension(metrics.widthPixels, metrics.heightPixels);
		DebugUtils.appendLog("ScreenDimensionInitializer.initHostScreenDimensions: Host screen size is " + ScreenDimensions.HOST_SCREEN_SIZE );
    }
    
    /** Initializes the host screen dimensions. */
    public static void initNewtonScreenDimensions(Context context) {
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
		final String value = prefs.getString("screenpresets", "320 x 480");
		DebugUtils.appendLog("ScreenDimensionInitializer.initNewtonScreenDimensions: Current preference is " + value);
		final int separatorPosition = value.indexOf(" x ");
		final String sw = value.substring(0, separatorPosition);
		DebugUtils.appendLog("ScreenDimensionInitializer.initNewtonScreenDimensions: Width got from preference string is " + sw);		
		String sh = value.substring(separatorPosition + 3, value.length());
		final int endPosition = sh.indexOf(" ");
		if (endPosition!=-1) sh = sh.substring(0, endPosition);
		DebugUtils.appendLog("ScreenDimensionInitializer.initNewtonScreenDimensions: Height got from preference string is " + sh);		
		ScreenDimensions.NEWTON_SCREEN_WIDTH = Integer.valueOf(sw);
		ScreenDimensions.NEWTON_SCREEN_HEIGHT = Integer.valueOf(sh);
		ScreenDimensions.NEWTON_SCREEN_SIZE = new Dimension(ScreenDimensions.NEWTON_SCREEN_WIDTH, ScreenDimensions.NEWTON_SCREEN_HEIGHT);
		DebugUtils.appendLog("ScreenDimensionInitializer.initNewtonScreenDimensions: Newton window size is " + ScreenDimensions.NEWTON_SCREEN_SIZE );
    }

}
