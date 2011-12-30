/** A helper class that takes care of issues that are required once before the emulator is started. */

package com.example.einstein.startup;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import com.example.einstein.DebugUtils;
import com.example.einstein.R;
import com.example.einstein.constants.StringConstants;
import com.example.einstein.startup.IStartup.LoadResult;

import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.util.Log;

public class Startup {

	private final Context context;

	public Startup(final Context context) {
		super();
		this.context = context;
	}

	/** Ensures that we have a ROM file, a REX file and an application icon */
	public final LoadResult installAssets(final AssetManager assetManager) {
		final File dataDir = new File(StartupConstants.DATA_FILE_PATH);
		dataDir.mkdirs();
		// Install ROM file
		if (!this.romFileAvailable(dataDir)) {
			final LoadResult result = this.loadRomFile(assetManager, dataDir);
			if (LoadResult.OK != result) {
				return result;
			}
			// TODO Log success or failure to file
		}
		// Install REX file
		if (!this.rexFileAvailable(dataDir)) {
			final LoadResult result = this.loadRexFile(assetManager, dataDir);
			if (LoadResult.OK != result) {
				return result;
			}
			// TODO Log success or failure to file
		}
		// Check if we have an application icon
		if (!this.applicationIconAvailable(dataDir)) {
			// TODO Log failure to file
			final Resources resources = this.context.getResources();
			final String line1 = StringConstants.getLocalizedString(resources, R.string.Startup_iconFileMissing);
			final String line2 = StringConstants.getLocalizedString(resources, R.string.Startup_expectedPath);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(context, message);
			return LoadResult.APPLICATION_ICON_MISSING;
		}
		return LoadResult.OK;
	}

	/** Returns <code>true</code> if a ROM file was found in <code>dataDir</code> */
	private final boolean romFileAvailable(final File dataDir) {
		final File romFile = new File(dataDir + StartupConstants.FILE_SEPARATOR + StartupConstants.ROM_FILE_NAME);
		return romFile.exists();		
	}

	/** Loads the ROM file */
	private final IStartup.LoadResult loadRomFile(final AssetManager assetManager, final File dataDir) {
		InputStream in = null;
		FileOutputStream out = null;
		try {
			final String path = dataDir + StartupConstants.FILE_SEPARATOR + StartupConstants.ROM_FILE_NAME;
			in = assetManager.open(StartupConstants.ROM_FILE_NAME + ".png");
			Log.w("Einstein", "Copying ROM file from assets...");
			out = new FileOutputStream(path);
			int sz = in.available();
			byte[] b = new byte[sz];
			in.read(b);
			out.write(b);
			Log.w("Einstein", "Finished loading ROM file.");
			return LoadResult.OK;
		}
		catch (IOException e) {
			final Resources resources = this.context.getResources();
			final String line1 = StringConstants.getLocalizedString(resources, R.string.Startup_romFileMissing);
			final String line2 = StringConstants.getLocalizedString(resources, R.string.Startup_expectedPath);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(context, message);
			return LoadResult.ROM_FILE_MISSING;
		}
		finally {
			try {
				if (null != in) {
					in.close();
				}
				if (null != out) {
					out.close();
				}
			}
			catch (IOException e) {
				// Do nothing. We've already returned true or false
			}
		}
	}

	/** Returns <code>true</code> if a REX file was found in <code>dataDir</code> */
	private final boolean rexFileAvailable(final File dataDir) {
		final File rexFile = new File(dataDir + StartupConstants.FILE_SEPARATOR + StartupConstants.REX_FILE_NAME);
		return rexFile.exists();		
	}

	/** Loads the REX file */
	private final LoadResult loadRexFile(final AssetManager assetManager, final File dataDir) {
		InputStream in = null;
		FileOutputStream out = null;
		try {
			final String path = dataDir + StartupConstants.FILE_SEPARATOR + StartupConstants.REX_FILE_NAME;
			in = assetManager.open(StartupConstants.REX_FILE_NAME + ".png");
			Log.w("Einstein", "Copying REX file from assets...");
			out = new FileOutputStream(path);
			int sz = in.available();
			byte[] b = new byte[sz];
			in.read(b);
			out.write(b);
			Log.w("Einstein", "Finished loading REX file.");
			return LoadResult.OK;
		}
		catch (IOException e) {
			final Resources resources = this.context.getResources();
			final String line1 = StringConstants.getLocalizedString(resources, R.string.Startup_rexFileMissing);
			final String line2 = StringConstants.getLocalizedString(resources, R.string.Startup_expectedPath);
			final String message = line1 + "\n" + line2;
			DebugUtils.showInfoDialog(context, message);
			return LoadResult.REX_FILE_MISSING;
		}
		finally {
			try {
				if (null != in) {
					in.close();
				}
				if (null != out) {
					out.close();
				}
			}
			catch (IOException e) {
				// Do nothing. We've already returned true or false
			}
		}
	}

	/** Returns <code>true</code> if an application icon was found in <code>dataDir</code> */
	private final boolean applicationIconAvailable(final File dataDir) {
		final File iconFile = new File(dataDir + StartupConstants.FILE_SEPARATOR + StartupConstants.APP_ICON_FILE_NAME);
		return iconFile.exists();
	} 

}
