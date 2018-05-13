package com.newtonforever.einstein.dialog;

/**
 * This is a container class for dialog fragment tags. These tags can be helpful for getting
 * a handle to a fragment by calling <code>findFragmentByTag()</code>. @author Frank Gruendel.
 */
public class DialogFragmentTags {
    private DialogFragmentTags() {
        // No instantiation, please
    }

    /**
     * The dialog fragment used for the <code>URLPickerFragment</code> class.
     */
    public static String URLPickerFragmentTag = "URLPickerFragmentTag";

}
