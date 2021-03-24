package com.matthiasm.einstein;

import android.Manifest;
import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.NativeActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.res.Resources;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import androidx.core.content.ContextCompat;
import androidx.core.app.ActivityCompat;
import android.content.pm.PackageManager;
import java.util.concurrent.Semaphore;

/*
 * Older versions of NativeActivity have a bug. It messes up the path to external native libraries
 * in the x86 version. By deriving a new class and launching that instead, NativeActivity will
 * see that we already loaded 'libeinstein.so' and not try that again (which would crash the app).
 */
public class NativeDebug extends NativeActivity {

    static {
        System.loadLibrary("einstein");
    }

    public void test() {
        /*
        // status bar height
        int statusBarHeight = 0;
        int resourceId = getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            statusBarHeight = getResources().getDimensionPixelSize(resourceId);
        }

        // action bar height
        int actionBarHeight = 0;
        final TypedArray styledAttributes = getActivity().getTheme().obtainStyledAttributes(
                new int[] { android.R.attr.actionBarSize }
        );
        actionBarHeight = (int) styledAttributes.getDimension(0, 0);
        styledAttributes.recycle();

        // navigation bar height
        int navigationBarHeight = 0;
        int resourceId = getResources().getIdentifier("navigation_bar_height", "dimen", "android");
        if (resourceId > 0) {
            navigationBarHeight = resources.getDimensionPixelSize(resourceId);
        }
         */
    }

    public int getStatusBarHeight() {
        int result = -1;
        int resourceId = getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            result = getResources().getDimensionPixelSize(resourceId);
        }
        return result;
    }

    public int getNavigationBarHeight() {
        int navigationBarHeight = -1;
        int resourceId = getResources().getIdentifier("navigation_bar_height", "dimen", "android");
        if (resourceId > 0) {
            navigationBarHeight = getResources().getDimensionPixelSize(resourceId);
        }
        return navigationBarHeight;
    }

    public int getScreenHeight() {
        return Resources.getSystem().getDisplayMetrics().heightPixels;
    }

    /** Returns the consumer friendly device name */
    public String getDeviceName() {
        String manufacturer = Build.MANUFACTURER;
        String model = Build.MODEL;
        if (model.startsWith(manufacturer)) {
            return model;
        }
        return manufacturer + " " + model;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        /*  --->  StatusBar (I don't want that to be translucent!)
         *  --->  ActionBar
         *    \
         *     >  Content
         *    /
         * ---->  NavigationBar (Back, Home, Menu)
         */
        // Here, thisActivity is the current activity
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
        }
        while (true) {
            if (ContextCompat.checkSelfPermission(this,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED) {
                break;
            }
            try { Thread.sleep(1000); } catch (InterruptedException e) { e.printStackTrace(); }
        }
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 1);
        }
        while (true) {
            if (ContextCompat.checkSelfPermission(this,
                    Manifest.permission.READ_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED) {
                break;
            }
            try { Thread.sleep(1000); } catch (InterruptedException e) { e.printStackTrace(); }
        }
        super.onCreate(savedInstanceState);
    }


    // This function will be called from C++ by name and signature
    public void showAlert(final String message)
    {
        Log.e("JNI", "showAlert: yes?" );
        final NativeDebug activity = this;
        this.runOnUiThread(new Runnable() {
                               public void run() {
                                   AlertDialog alertDialog = new AlertDialog.Builder(activity).create();
                                   alertDialog.setTitle("Alert");
                                   alertDialog.setMessage(message);
                                   alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK",
                                           new DialogInterface.OnClickListener() {
                                               public void onClick(DialogInterface dialog, int which) {
                                                   dialog.dismiss();
                                               }
                                           });
                                   alertDialog.show();
                               }
                           }
        );
    }
/*
    @Override
    public void onBackPressed() {
        // show the Einstein Settings activity
        Intent myIntent = new Intent(this, SettingsActivity.class);
        startActivity(myIntent);
        showAlert("Prefs?");
    }
*/
}


/* TODO: try to get the toolbar and title bar into shape
 * or use  style.xml
public class NameOfActivity extends NativeActivity {

void setImmersiveSticky() {
    View decorView = getWindow().getDecorView();
    decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
}

@Override
protected void onCreate(Bundle savedInstanceState) {
    setContentView(R.layout.activity_main);

    int SDK_INT = android.os.Build.VERSION.SDK_INT;
    if (SDK_INT >= 19) {
        setImmersiveSticky();

        View decorView = getWindow().getDecorView();
        decorView.setOnSystemUiVisibilityChangeListener
                (new View.OnSystemUiVisibilityChangeListener() {
                    @Override
                    public void onSystemUiVisibilityChange(int visibility) {
                        setImmersiveSticky();
                    }
                });
    }
    super.onCreate(savedInstanceState);
}

@Override
protected void onResume() {
    //Hide toolbar
    int SDK_INT = android.os.Build.VERSION.SDK_INT;
    if (SDK_INT >= 11 && SDK_INT < 14) {
        getWindow().getDecorView().setSystemUiVisibility(View.STATUS_BAR_HIDDEN);
    } else if (SDK_INT >= 14 && SDK_INT < 19) {
        getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_LOW_PROFILE);
    } else if (SDK_INT >= 19) {
        setImmersiveSticky();
    }
    super.onResume();
}
*
* 				case States.Visible:
					_isStatusBarTranslucent = false;
					newFlagsValue |= WINDOW_FLAG_FORCE_NOT_FULLSCREEN;
					break;
				case States.VisibleOverContent:
					_isStatusBarTranslucent = false;
					newFlagsValue |= WINDOW_FLAG_FORCE_NOT_FULLSCREEN | WINDOW_FLAG_LAYOUT_IN_SCREEN;
					newSystemUiVisibilityValue |= VIEW_SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
					break;
				case States.TranslucentOverContent:
					_isStatusBarTranslucent = true;
					newFlagsValue |= WINDOW_FLAG_FORCE_NOT_FULLSCREEN | WINDOW_FLAG_LAYOUT_IN_SCREEN | WINDOW_FLAG_TRANSLUCENT_STATUS;
					newSystemUiVisibilityValue |= VIEW_SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
					break;
				case States.Hidden:
					newFlagsValue |= WINDOW_FLAG_FULLSCREEN | WINDOW_FLAG_LAYOUT_IN_SCREEN;
					if (_isStatusBarTranslucent) newFlagsValue |= WINDOW_FLAG_TRANSLUCENT_STATUS;
					break;


 */