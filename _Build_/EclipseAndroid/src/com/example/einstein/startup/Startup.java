/** A helper class that takes care of issues that are required once before the emulator is started. */

package com.example.einstein.startup;

import java.io.File;

import android.app.Activity;
import android.content.res.AssetManager;
import android.content.res.Resources;

import com.example.einstein.DebugUtils;
import com.example.einstein.R;
import com.example.einstein.constants.StringConstants;
import com.example.einstein.startup.IStartup.LoadResult;
import com.example.einstein.utils.screen.ScreenDimensionsInitializer;

public class Startup {

	private final Activity activity;

	public Startup(final Activity activity) {
		super();
		this.activity = activity;
		ScreenDimensionsInitializer.initScreenDimensions(activity);
	}

	/** Checks if we have a ROM file, a REX file and an application icon in the folder where we expect them. */
	public final LoadResult installAssets(final AssetManager assetManager) {
		final Resources resources = this.activity.getResources();
		final File dataDir = new File(StartupConstants.DATA_FILE_PATH);
		dataDir.mkdirs();
		final String line2 = StringConstants.getLocalizedString(resources, R.string.Startup_expectedPath);
		// Make sure we have a ROM file
		if (!this.romFileAvailable(dataDir)) {
			DebugUtils.appendLog("Startup.installAssets: ROM file not found");
			final String line1 = StringConstants.getLocalizedString(resources, R.string.Startup_romFileMissing);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(activity, message);
			return LoadResult.ROM_FILE_MISSING;
		}
		// Make sure we have a REX file
		if (!this.rexFileAvailable(dataDir)) {
			DebugUtils.appendLog("Startup.installAssets: REX file not found");
			final String line1 = StringConstants.getLocalizedString(resources, R.string.Startup_rexFileMissing);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(activity, message);
			return LoadResult.REX_FILE_MISSING;
		}
		// Make sure we have an application icon
		if (!this.applicationIconAvailable(dataDir)) {
			DebugUtils.appendLog("Startup.installAssets: App icon file not found");
			final String line1 = StringConstants.getLocalizedString(resources, R.string.Startup_iconFileMissing);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(activity, message);
			return LoadResult.APPLICATION_ICON_MISSING;
		}
		return LoadResult.OK;
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
