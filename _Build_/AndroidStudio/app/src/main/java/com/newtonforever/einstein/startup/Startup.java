// TODO FG Review

package com.newtonforever.einstein.startup;

import android.content.Context;
import android.content.res.Resources;

import com.newtonforever.einstein.R;
import com.newtonforever.einstein.startup.IStartup.LoadResult;
import com.newtonforever.einstein.utils.StringUtils;
import com.newtonforever.einstein.utils.debug.DebugUtils;
import com.newtonforever.einstein.utils.screen.ScreenDimensionsInitializer;

import java.io.File;

public class Startup {

	private final Context context;

	public Startup(final Context context) {
		super();
		this.context = context;
		ScreenDimensionsInitializer.initScreenDimensions(context);
	}

	/** Checks if we have a ROM file, a REX file and an application icon in the folder where we expect them. */
	public final LoadResult installAssets() {
		final Resources resources = context.getResources();
		final File dataDir = new File(StartupConstants.DATA_FILE_PATH);
		dataDir.mkdirs();
		final String line2 = StringUtils.getLocalizedString(resources, R.string.Startup_expectedPath);
		// Make sure we have a ROM file
		if (!this.romFileAvailable(dataDir)) {
			DebugUtils.appendLog("Startup.installAssets: ROM file not found");
			final String line1 = StringUtils.getLocalizedString(resources, R.string.Startup_romFileMissing);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(context, message);
			return LoadResult.ROM_FILE_MISSING;
		}
		// Make sure we have a REX file
		if (!this.rexFileAvailable(dataDir)) {
			DebugUtils.appendLog("Startup.installAssets: REX file not found");
			final String line1 = StringUtils.getLocalizedString(resources, R.string.Startup_rexFileMissing);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(context, message);
			return LoadResult.REX_FILE_MISSING;
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

}
