/** Helper class for display-related code that we need all the time. */
package com.example.einstein.utils;

import com.example.einstein.DebugUtils;

import android.app.Activity;
import android.content.SharedPreferences;
import android.graphics.Point;
import android.util.DisplayMetrics;

public class ScreenUtils {
	
	/** Returns the size of the host screen. We return an instance of <code>Point</code> here
	 * because surprisingly the Android VM doesn't know the java <code>Dimension</code> object. */
	public static Point getHostScreenSize(final Activity activity) {
		final DisplayMetrics metrics = new DisplayMetrics();
		activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);
		return new Point(metrics.widthPixels, metrics.heightPixels);
	}
	
	/** Returns the size of the Newton screen. We return an instance of <code>Point</code> here
	 * because surprisingly the Android VM doesn't know the java <code>Dimension</code> object. */
	public static Point getNewtonScreenSize(SharedPreferences prefs) {
		//DebugUtils.appendLog("ScreenUtils: getNewtonScreenSize");		
		final String value = prefs.getString("screenpresets", "1");
		//DebugUtils.appendLog("ScreenUtils: got preference " + value);		
		final int separator = value.indexOf(" x ");
		final String sw = value.substring(0, separator - 1);
		final String sh = value.substring(separator + 3, value.length() - 1);
		//DebugUtils.appendLog("ScreenUtils: sw = " + sw);		
		//DebugUtils.appendLog("ScreenUtils: sh = " + sh);		
		final int w = new Integer(sw).intValue();
		final int h = new Integer(sh).intValue();
		//DebugUtils.appendLog("ScreenUtils: leaving getNewtonScreenSize");		
		return new Point(w, h);
	}

}
