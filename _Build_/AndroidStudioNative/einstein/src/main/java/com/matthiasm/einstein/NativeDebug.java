package com.matthiasm.einstein;

import android.app.NativeActivity;

/*
 * Older versions of NativeActivity have a bug. It messes up the path to external native libraries
 * in the x86 version. By deriving a new class and launching that instead, NativeActivity will
 * see that we already loaded 'libeinstein.so' and not try that again (which would crash the app).
 */
public class NativeDebug extends NativeActivity {

    static {
        System.loadLibrary("einstein");
    }

}
