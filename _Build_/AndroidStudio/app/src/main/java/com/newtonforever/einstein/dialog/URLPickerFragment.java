package com.newtonforever.einstein.dialog;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;

import com.newtonforever.einstein.R;
import com.newtonforever.einstein.utils.debug.DebugUtils;

// A good tutorial on how to use picker dialogs can be found here:
// http://developer.android.com/guide/topics/ui/dialogs.html

/**
 * A dialog for selecting URLs and opening them in the Android Browser. Note that the <code>DialogFragment</code>
 * class was originally added with Android 3.0 (API level 11), so what we use here is the DialogFragment class
 * that's provided with the Support Library. Be sure that you import the <code>android.support.v4.app.DialogFragment</code>
 * class and not the <code>android.app.DialogFragment</code> class. @author Frank Gruendel
 */
public class URLPickerFragment extends DialogFragment {

    /**
     * Currently defined URLs. If you add a URL here, you must also add an entry for it in array.xml.
     */
    private final String[] urls = {
            "http://www.pda-soft.de",
            "http://www.unna.org",
            "http://www.newtontalk.net",
            "http://myapplenewton.blogspot.com",
            "http://40hz.org"
    };

    /**
     * Opens the URL selection dialog. The selected site is opened in the web browser.
     */
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        DebugUtils.appendLog("URLPickerFragment.onCreateDialog: Entering method");
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle(R.string.dialogs_urlpicker_title);
        builder.setItems(R.array.entries_newtonsite_urls,
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        final String url = urls[which];
                        DebugUtils.appendLog("URLPickerFragment.onCreateDialog: User picked URL " + url);
                        openBrowserInURL(url);
                    }
                });
        builder.setNegativeButton(R.string.common_cancel, new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int id) {
                // When the user touches any of the action buttons created with an AlertDialog.Builder,
                // the system dismisses the dialog for us. So there's nothing we need to do here unless
                // we need to perform certain actions when the dialog goes away, in which case we'd have
                // to implement onCancel() or onDismiss() in our class.
            }
        });
        DebugUtils.appendLog("URLPickerFragment.onCreateDialog: Leaving method");
        return builder.create();
    }

    /**
     * Opens the browser and directs it to <code>url</code>.
     */
    private void openBrowserInURL(final String url) {
        DebugUtils.appendLog("URLPickerFragment.openBrowserInURL: Entering method");
        final Intent browserIntent = new Intent("android.intent.action.VIEW", Uri.parse(url));
        startActivity(browserIntent);
        DebugUtils.appendLog("URLPickerFragment.openBrowserInURL: Leaving method");
    }
}