/** A helper class that takes care of issues that are required once before the emulator is started. */

package com.example.einstein.startup;

import java.io.File;

import com.example.einstein.DebugUtils;
import com.example.einstein.R;
import com.example.einstein.constants.StringConstants;
import com.example.einstein.startup.IStartup.LoadResult;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.graphics.Point;
import android.preference.PreferenceManager;
import android.util.DisplayMetrics;

public class Startup {

	private final Context context;

	public Startup(final Context context) {
		super();
		this.context = context;
	}

	/** Checks if we have a ROM file, a REX file and an application icon in the folder where we expect them. */
	public final LoadResult installAssets(final AssetManager assetManager) {
		final Resources resources = this.context.getResources();
		final File dataDir = new File(StartupConstants.DATA_FILE_PATH);
		dataDir.mkdirs();
		final String line2 = StringConstants.getLocalizedString(resources, R.string.Startup_expectedPath);
		// Make sure we have a ROM file
		if (!this.romFileAvailable(dataDir)) {
			final String line1 = StringConstants.getLocalizedString(resources, R.string.Startup_romFileMissing);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(context, message);
			return LoadResult.ROM_FILE_MISSING;
		}
		// Make sure we have a REX file
		if (!this.rexFileAvailable(dataDir)) {
			final String line1 = StringConstants.getLocalizedString(resources, R.string.Startup_rexFileMissing);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(context, message);
			return LoadResult.REX_FILE_MISSING;
		}
		// Make sure we have an application icon
		if (!this.applicationIconAvailable(dataDir)) {
			final String line1 = StringConstants.getLocalizedString(resources, R.string.Startup_iconFileMissing);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(context, message);
			return LoadResult.APPLICATION_ICON_MISSING;
		}
		return LoadResult.OK;
	}
	
	/** Returns the dimension of the Newton emulator screen. This depends on the preferences menu setting. We return an instance 
	 * of class <code>Point</code> because surprisingly there is no <code>Dimension</code> class in the Android VM. */
	public Point getNewtonScreenSize() {
		// Get host screen dimensions. Note that this depends on the current screen rotation
		// Why the heck doesn't getMetrics return a DisplayMetrics object??
		final DisplayMetrics metrics = new DisplayMetrics();
		final Activity activity = (Activity)context;
		activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);
		int hostScreenWidth = metrics.widthPixels;
		int hostScreenHeight = metrics.heightPixels;
		DebugUtils.showInfoDialog(activity, "Host screen size is " + String.valueOf(hostScreenWidth) + " x " + String.valueOf(hostScreenHeight));
		// Get current preferences setting
		final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
		final String currentPreset = prefs.getString("screenpresets", "1");
		// Calculate Newton screen dimensions depending on preferences setting
		final boolean isPortrait = hostScreenWidth < hostScreenHeight;
		int w = isPortrait ? 320 : 480;
		int h = isPortrait ? 480 : 320;
		if ("2".equals(currentPreset)) { // Host screen resolution
			w = hostScreenWidth;
			h = hostScreenHeight;
		} else if ("3".equals(currentPreset)) { // 3/4 host screen resolution
			w = hostScreenWidth * 3 / 4;
			h = hostScreenHeight * 3 / 4;
		} else if ("4".equals(currentPreset)) { // 1/2 host screen resolution
			w = hostScreenWidth / 2;
			h = hostScreenHeight / 2;
		}
		else if ("5".equals(currentPreset)) { // 1/3 host screen resolution
			w = hostScreenWidth / 3;
			h = hostScreenHeight / 3;
		}
		DebugUtils.showInfoDialog(activity, "Newton screen size is " + String.valueOf(w) + " x " + String.valueOf(h));
		return new Point(w, h);
	}

	/** Returns <code>true</code> if a ROM file was found in <code>dataDir</code> */
	private final boolean romFileAvailable(final File dataDir) {
		final File romFile = new File(dataDir + File.separator + StartupConstants.ROM_FILE_NAME);
		return romFile.exists();		
	}

	/** Returns <code>true</code> if a REX file was found in <code>dataDir</code> */
	private final boolean rexFileAvailable(final File dataDir) {
		final File rexFile = new File(dataDir + File.separator + StartupConstants.REX_FILE_NAME);
		return rexFile.exists();		
	}

	/** Returns <code>true</code> if an application icon was found in <code>dataDir</code> */
	private final boolean applicationIconAvailable(final File dataDir) {
		final File iconFile = new File(dataDir + File.separator + StartupConstants.APP_ICON_FILE_NAME);
		return iconFile.exists();
	} 
}
