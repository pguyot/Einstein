package com.newtonforever.einstein.utils;

/**
 * A class for miscellaneous utilities. @author Frank Gruendel.
 */
public class MiscUtils {

    private MiscUtils() {
        // No instances, please
    }

    /**
     * This class allows putting our thread to sleep without having to code try/catch statements all the time.
     */
    public static void sleepForMillis(int millis) {
        try {
            Thread.sleep(millis);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
