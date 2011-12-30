package com.example.einstein.startup;

import java.io.File;

import android.os.Environment;

public class StartupConstants {
	
	/** Standard directory in which to place files that have been downloaded by the user */
	public static final File EXTERNAL_STORAGE_PUBLIC_DIR;
	
	/** Platform-dependent file path separator character */
	public static final String FILE_SEPARATOR;
	
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
	
	static {
		EXTERNAL_STORAGE_PUBLIC_DIR = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
		FILE_SEPARATOR = System.getProperty("file.separator");
		APPLICATION_SUBDIR = "Einstein"; // TODO Change this?
		ROM_FILE_NAME = "717006.rom";
		REX_FILE_NAME = "Einstein.rex";
		DATA_FILE_PATH = EXTERNAL_STORAGE_PUBLIC_DIR + FILE_SEPARATOR + APPLICATION_SUBDIR;
		APP_ICON_FILE_NAME = "717006.img";
	}

}
