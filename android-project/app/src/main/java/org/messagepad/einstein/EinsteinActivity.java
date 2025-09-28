
package org.messagepad.einstein;

import org.libsdl.app.SDLActivity;
import android.app.Activity;
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

}
