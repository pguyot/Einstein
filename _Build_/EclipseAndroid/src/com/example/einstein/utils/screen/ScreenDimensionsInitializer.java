/** Helper class for display-related code that we need all the time. */
package com.example.einstein.utils.screen;

import android.app.Activity;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.util.DisplayMetrics;
import android.view.WindowManager;

import com.example.einstein.DebugUtils;
import com.missinginandroid.Dimension;

public class ScreenDimensionsInitializer {
	
    private ScreenDimensionsInitializer() {
		// No instances, please;
    }
    
    public static void initScreenDimensions(Activity activity) {
		initHostScreenDimensions(activity);
		initNewtonScreenDimensions(activity);
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
		final int w = new Integer(sw).intValue();
		final int h = new Integer(sh).intValue();
		final Dimension size = new Dimension(w, h);
		DebugUtils.appendLog("ScreenDimensionInitializer.getNewtonScreenSize: Returning " + size);		
		return size;
	}
    
    /** Initializes the host screen dimensions. */
    private static void initHostScreenDimensions(Activity activity) {
		if (null == activity) {
			DebugUtils.appendLog("ScreenDimensionInitializer.initHostScreenDimensions: activity is null");
			return;
		}
		final WindowManager windowManager = activity.getWindowManager();
		if (null == windowManager) {
			DebugUtils.appendLog("ScreenDimensionInitializer.initHostScreenDimensions: windowManager is null");
			return;
		}
		// Could someone please tell me why getMetrics doesn't return a DisplayMetrics object? Hgrmpff...
		final DisplayMetrics metrics = new DisplayMetrics();
		windowManager.getDefaultDisplay().getMetrics(metrics);
		ScreenDimensions.HOST_SCREEN_WIDTH = metrics.widthPixels;
		ScreenDimensions.HOST_SCREEN_HEIGHT = metrics.heightPixels;
		ScreenDimensions.HOST_SCREEN_SIZE = new Dimension(metrics.widthPixels, metrics.heightPixels);
		DebugUtils.appendLog("ScreenDimensionInitializer.initHostScreenDimensions: Host screen size is " + ScreenDimensions.HOST_SCREEN_SIZE );
    }
    
    /** Initializes the host screen dimensions. */
    public static void initNewtonScreenDimensions(Activity activity) {
		SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(activity);
		final String value = prefs.getString("screenpresets", "320 x 480");
		DebugUtils.appendLog("ScreenDimensionInitializer.initNewtonScreenDimensions: Current preference is " + value);
		final int separatorPosition = value.indexOf(" x ");
		final String sw = value.substring(0, separatorPosition);
		DebugUtils.appendLog("ScreenDimensionInitializer.initNewtonScreenDimensions: Width got from preference string is " + sw);		
		final String sh = value.substring(separatorPosition + 3, value.length());
		DebugUtils.appendLog("ScreenDimensionInitializer.initNewtonScreenDimensions: Height got from preference string is " + sh);		
		ScreenDimensions.NEWTON_SCREEN_WIDTH = new Integer(sw).intValue();
		ScreenDimensions.NEWTON_SCREEN_HEIGHT = new Integer(sh).intValue();
		ScreenDimensions.NEWTON_SCREEN_SIZE = new Dimension(ScreenDimensions.NEWTON_SCREEN_WIDTH, ScreenDimensions.NEWTON_SCREEN_HEIGHT);
		DebugUtils.appendLog("ScreenDimensionInitializer.initNewtonScreenDimensions: Newton window size is " + ScreenDimensions.NEWTON_SCREEN_SIZE );
    }

}
