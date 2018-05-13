package com.newtonforever.einstein.utils;

import android.content.res.Resources;
import android.content.res.Resources.NotFoundException;

/**
 * Utility class for string issues. @author Frank Gruendel.
 */
public class StringUtils {

    /**
     * Returns the string depicted by <code>id</code> in the current locale.
     */
    public static String getLocalizedString(Resources resources, int id) {
        try {
            return resources.getString(id);
        } catch (final NotFoundException e) {
            return "Text resource not found";
        }
    }
}
