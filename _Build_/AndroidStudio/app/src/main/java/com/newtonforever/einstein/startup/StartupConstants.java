// TODO FG Review

package com.newtonforever.einstein.startup;

import android.media.AudioFormat;
import android.media.AudioTrack;
import android.os.Environment;

import java.io.File;

public class StartupConstants {

    /**
     * Standard directory in which to place files that have been downloaded by the user
     */
    public static final File EXTERNAL_STORAGE_PUBLIC_DIR;

    /**
     * The sub-directory in which the Newton emulator expects its files
     */
    public static final String APPLICATION_SUBDIR;

    /**
     * The name of the Newton ROM file
     */
    public static final String ROM_FILE_NAME;

    /**
     * The name of the Newton REX file
     */
    public static final String REX_FILE_NAME;

    /**
     * The absolute path to the Newton Emulator data files
     */
    public static final String DATA_FILE_PATH;

    /**
     * The folder in the SD card in which to put the log file
     */
    public static final String LOG_FOLDER;

    /**
     * The name of the log file
     */
    public static final String LOG_FILE_NAME;

    /**
     * The default Newton ID if no ID has been set in the preferences.
     */
    public static final String DEFAULT_NEWTON_ID;

    /**
     * The default screen refresh rate if no rate has been set in the preferences.
     */
    public static final String DEFAULT_SCREEN_REFRESH_RATE;

    /**
     * The minimum buffer size required for the successful creation of an AudioTrack object created in MODE_STREAM mode
     */
    public static final int AUDIO_TRACK_MIN_BUFFER_SIZE;

    static {
        EXTERNAL_STORAGE_PUBLIC_DIR = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
        APPLICATION_SUBDIR = "Einstein"; // TODO Change this?
        ROM_FILE_NAME = "717006.rom";
        //ROM_FILE_NAME = "FranksFlashRomBoard.rom";
        REX_FILE_NAME = "Einstein.rex";
        DATA_FILE_PATH = EXTERNAL_STORAGE_PUBLIC_DIR + File.separator + APPLICATION_SUBDIR;
        LOG_FOLDER = "log";
        LOG_FILE_NAME = "noselog.txt";
        DEFAULT_NEWTON_ID = "00004E6577746F6E";
        DEFAULT_SCREEN_REFRESH_RATE = "10";
        AUDIO_TRACK_MIN_BUFFER_SIZE = AudioTrack.getMinBufferSize(22050, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_16BIT);
    }

}
