
package org.messagepad.einstein;

import org.libsdl.app.SDLActivity;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.net.Uri;
import android.os.ParcelFileDescriptor;
import android.provider.DocumentsContract;

import java.io.FileInputStream;
import java.io.FileOutputStream;

public class EinsteinActivity extends SDLActivity {

    private static final int REQUEST_CODE_IMPORT_ROM = 1337;

    protected String[] getLibraries() {
        return new String[] { "SDL3", "einstein" };
    }

    // Called from C via JNI
    public void openFilePicker() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        // Works, but requires later API
        //intent.putExtra(DocumentsContract.EXTRA_INITIAL_URI, "content://com.android.providers.downloads.documents/");
        // Adjust MIME type if needed (e.g. "application/zip", "application/octet-stream")
        intent.setType("*/*");
        startActivityForResult(intent, REQUEST_CODE_IMPORT_ROM);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_CODE_IMPORT_ROM && resultCode == Activity.RESULT_OK) {
            Uri uri = data.getData();
            if (uri != null) {
                copyUriToAppStorage(uri, "ROMs");
            }
        }
    }

    private void copyUriToAppStorage(Uri uri, String subdir) {
        try {
            ParcelFileDescriptor pfd = getContentResolver().openFileDescriptor(uri, "r");
            if (pfd == null) return;

            FileInputStream in = new FileInputStream(pfd.getFileDescriptor());

            // Destination: /storage/emulated/0/Android/data/<pkg>/files/ROMs/
            String base = getExternalFilesDir(null).getAbsolutePath();
            java.io.File dir = new java.io.File(base, subdir);
            if (!dir.exists()) dir.mkdirs();

            // Use the last path segment as filename
            String filename = "imported.rom";
            String name = uri.getLastPathSegment();
            if (name != null) {
                int idx = name.lastIndexOf('/');
                if (idx >= 0) name = name.substring(idx + 1);
                filename = name;
            }

            FileOutputStream out = new FileOutputStream(new java.io.File(dir, filename));

            byte[] buf = new byte[8192];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }

            in.close();
            out.close();
            pfd.close();

            // Notify C side that import finished
            onFileImported(dir.getAbsolutePath() + "/" + filename);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // Native method called after import
    private native void onFileImported(String path);

    // Called from C via JNI: show a list of menu items. With checked >= 0 the
    // list is a single choice with that item selected. The choice (or -1 for
    // Cancel) goes back to C through onMenuItemSelected, with the menu id.
    public void showMenu(final int menuId, final String title, final String[] items, final int checked) {
        runOnUiThread(() -> {
            AlertDialog.Builder builder = new AlertDialog.Builder(this).setTitle(title);
            if (checked >= 0) {
                builder.setSingleChoiceItems(items, checked, (dialog, which) -> {
                    dialog.dismiss();
                    onMenuItemSelected(menuId, which);
                });
            } else {
                builder.setItems(items, (dialog, which) -> onMenuItemSelected(menuId, which));
            }
            builder.setNegativeButton("Cancel", (dialog, which) -> onMenuItemSelected(menuId, -1))
                   .setOnCancelListener(dialog -> onMenuItemSelected(menuId, -1))
                   .show();
        });
    }

    // Native method called with the menu id and the chosen item, or -1.
    private native void onMenuItemSelected(int menuId, int which);

    // Called from C via JNI, on a worker thread: download a URL (also https,
    // which the C++ HTTP client cannot do) and return its contents. Throws
    // with a message on failure.
    /** Newton packages are small; a larger download is not a package. */
    private static final int kMaxDownloadSize = 16 * 1024 * 1024;

    public static byte[] httpGet(String url) throws java.io.IOException {
        for (int redirects = 0; redirects < 5; redirects++) {
            java.net.HttpURLConnection connection =
                (java.net.HttpURLConnection) new java.net.URL(url).openConnection();
            connection.setConnectTimeout(20000);
            connection.setReadTimeout(60000);
            connection.setInstanceFollowRedirects(false);
            try {
                int code = connection.getResponseCode();
                if (code >= 300 && code < 400) {
                    String location = connection.getHeaderField("Location");
                    if (location == null) throw new java.io.IOException("Redirect without a location");
                    String next = new java.net.URL(new java.net.URL(url), location).toString();
                    if (url.startsWith("https:") && !next.startsWith("https:"))
                        throw new java.io.IOException("Redirect from https to an insecure address");
                    url = next;
                    continue;
                }
                if (code != 200) throw new java.io.IOException("HTTP status " + code);
                long length = connection.getContentLengthLong();
                if (length > kMaxDownloadSize) throw new java.io.IOException("The file is too large for a Newton package");
                try (java.io.InputStream in = connection.getInputStream();
                     java.io.ByteArrayOutputStream out = new java.io.ByteArrayOutputStream()) {
                    byte[] buffer = new byte[65536];
                    int n;
                    while ((n = in.read(buffer)) > 0) {
                        if (out.size() + n > kMaxDownloadSize)
                            throw new java.io.IOException("The file is too large for a Newton package");
                        out.write(buffer, 0, n);
                    }
                    return out.toByteArray();
                }
            } finally {
                connection.disconnect();
            }
        }
        throw new java.io.IOException("Too many redirects");
    }

}
