// TODO FG Review

package com.newtonforever.einstein.startup;

import java.io.File;

import android.os.Environment;

public class StartupConstants {
	
	/** Standard directory in which to place files that have been downloaded by the user */
	public static final File EXTERNAL_STORAGE_PUBLIC_DIR;
	
	/** The sub-directory in which the Newton emulator expects its files */
	public static final String APPLICATION_SUBDIR;
	
	/** The name of the Newton ROM file */
	public static final String ROM_FILE_NAME;
	
	/** The name of the Newton REX file */
	public static final String REX_FILE_NAME;
	
	/** The absolute path to the Newton Emulator data files */
	public static final String DATA_FILE_PATH;
	
	/** The name of the application icon image file */
	public static final String APP_ICON_FILE_NAME;
	
	/** The folder in the SD card in which to put the log file */
	public static final String LOG_FOLDER;
	
	/** The name of the log file */
	public static final String LOG_FILE_NAME;
	
	static {
		EXTERNAL_STORAGE_PUBLIC_DIR = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
		APPLICATION_SUBDIR = "Einstein"; // TODO Change this?
		ROM_FILE_NAME = "717006.rom";
		REX_FILE_NAME = "Einstein.rex";
		DATA_FILE_PATH = EXTERNAL_STORAGE_PUBLIC_DIR + File.separator + APPLICATION_SUBDIR;
		APP_ICON_FILE_NAME = "717006.img";
		LOG_FOLDER = "log";
		LOG_FILE_NAME = "noselog.txt";
	}

}
