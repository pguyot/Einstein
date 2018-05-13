package com.newtonforever.einstein;

import com.newtonforever.einstein.jni.Native;

/**
 * Class interfacing to the native Einstein emulator.
 *
 * @author Matthias Melcher
 */
public class Einstein {
    /** Load the entire native program as a library at startup. */
    static {
        System.loadLibrary("native-lib");
    }

    private boolean pRunning = false;

    public void run(String dataPath, int screenWidth, int screenHeight) {
        Native.runEmulator(dataPath, screenWidth, screenHeight);
        this.pRunning = true;
    }

    /**
     * Returns <code>true</code> if the emulator is currently running.
     */
    public boolean isRunning() {
        return this.pRunning;
    }
}