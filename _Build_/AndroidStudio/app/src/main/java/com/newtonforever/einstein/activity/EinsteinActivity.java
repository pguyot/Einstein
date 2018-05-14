// TODO FG Review
package com.newtonforever.einstein.activity;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.newtonforever.einstein.Einstein;
import com.newtonforever.einstein.EinsteinApplication;
import com.newtonforever.einstein.action.EinsteinActionHandler;
import com.newtonforever.einstein.jni.Native;
import com.newtonforever.einstein.startup.LoadResult;
import com.newtonforever.einstein.startup.Startup;
import com.newtonforever.einstein.startup.StartupConstants;
import com.newtonforever.einstein.utils.MiscUtils;
import com.newtonforever.einstein.utils.screen.ScreenDimensions;
import com.newtonforever.einstein.utils.screen.ScreenDimensionsInitializer;
import com.newtonforever.einstein.view.EinsteinView;

import java.util.Timer;

/**
 * The main user interface to the emulator.
 * <p>
 * This class manages the visual representation of the emulator and
 * the screen interface.
 *
 * @author matt
 */
public class EinsteinActivity extends Activity implements OnSharedPreferenceChangeListener {

    private static final String TAG = EinsteinActivity.class.toString();
    
    private static final int REQUEST_WRITE = 1;

    private Einstein pEinstein = null;
    private EinsteinView pEinsteinView = null;
    private Timer mScreenRefreshTimer = null;
    private EinsteinActionHandler mScreenRefreshTask = null;
    private SharedPreferences sharedPrefs;
    private Startup startup;

    // --- beginning of life cycle

    /**
     * \brief Android creates a new activity, launching or waking Einstein.
     * <p>
     * If called for the first time, it starts Einstein.
     * <p>
     * The Activity then creates a view and connect Einstein to that view.
     * <p>
     * \todo Preferences are not managed well yet, just as proof of concept.
     * \todo No ROM management is implemented yet.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "Entering onCreate().");

        super.onCreate(savedInstanceState);

        // Create an instance of EinsteinPreferencesActivity. If we do not do this, the preferences that are calculated
        // at runtime wouldn't exist until the user has invoked the preferences window for the first time.
        this.sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);

        this.startup = new Startup(this);

        // Create view
        this.pEinsteinView = new EinsteinView(this);

        // Show or hide Android status bar. Note that this must take place before we call setContentView
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        final boolean statusBarVisible = sharedPrefs.getBoolean("androidstatusbar", true);
        updateFullscreenStatus(statusBarVisible);
        setContentView(pEinsteinView);

        int permission =
                ContextCompat.checkSelfPermission(this,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (permission != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    REQUEST_WRITE);
            return;
        }

        // Install all required assets, initialize host device dependent values, ...
        init();
    }

    private void init() {
        Log.i(TAG, "Creating Einstein application.");
        final EinsteinApplication app = (EinsteinApplication) getApplication();
        pEinstein = app.getEinstein();

        final LoadResult result = startup.installAssets();
        if (LoadResult.OK != result) {
            Log.e(TAG, "Problem with installing assets.");
            return;
        }

        // Register a listener that'll notify us of preference changes.
        registerPreferenceChangeListener();

        // Initialize emulator
        if (!pEinstein.isRunning()) {
            Native.initEmulator("CONSOLE");
        }

        // Start the emulator
        if (pEinstein.isRunning()) { // Wake up
            Toast.makeText(getApplicationContext(), "Reconnecting to Einstein", Toast.LENGTH_SHORT).show();
        } else {
            String id = sharedPrefs.getString("newtonid", StartupConstants.DEFAULT_NEWTON_ID);
            Native.setNewtonID(id);
            pEinstein.run(StartupConstants.DATA_FILE_PATH, ScreenDimensions.NEWTON_SCREEN_WIDTH, ScreenDimensions.NEWTON_SCREEN_HEIGHT);
            // TODO FG 2013_10_19 Only required when using Frank's Flash ROM board data. Remove in a final version.
            //MiscUtils.sleepForMillis(2000);
            //Log.i(TAG, "Sleeping for 2s after calling run because we're using Frank's ROM...");
        }
        final int rate = Integer.valueOf(sharedPrefs.getString("screenrefreshrate", StartupConstants.DEFAULT_SCREEN_REFRESH_RATE));
        startScreenRefresh(rate);
        // TODO FG 2013_10_19 Only required when using Frank's Flash ROM board data. Remove in a final version.
        //MiscUtils.sleepForMillis(2000);
        //Log.i(TAG, "Sleeping for 2s after starting screen refresh because we're using Frank's ROM...");

        app.raisePriority();

        Log.i(TAG, "Leaving onCreate().");
    }

    @Override
    public void onStart() {
        Log.i(TAG, "Entering onStart().");
        super.onStart();
        int rate = Integer.valueOf(sharedPrefs.getString("screenrefreshrate", StartupConstants.DEFAULT_SCREEN_REFRESH_RATE));
        startScreenRefresh(rate);
        Log.i(TAG, "Leaving onStart().");
    }

    @Override
    public void onResume() {
        Log.i(TAG, "Entering onResume().");
        super.onResume();
        int rate = Integer.valueOf(sharedPrefs.getString("screenrefreshrate", StartupConstants.DEFAULT_SCREEN_REFRESH_RATE));
        startScreenRefresh(rate);
        Native.powerOnEmulator();
        Log.i(TAG, "Leaving onResume().");
    }

    @Override
    public void onNewIntent(Intent intent) {
        Log.i(TAG, "Entering onNewIntent().");
        if (intent.getBooleanExtra("EXIT", false)) {
            Log.i(TAG, "We need to exit...");
            finish();
        }
        String file = intent.getStringExtra("FILE");
        if (file != null) {
            Native.installPackage(file);
            intent.removeExtra("FILE");
        }
        Log.i(TAG, "Leaving onNewIntent().");
    }

    @Override
    public void onPause() {
        Log.i(TAG, "Entering onPause().");
        super.onPause();
        Log.i(TAG, "Leaving onPause().");
    }

    @Override
    public void onStop() {
        Log.i(TAG, "Entering onStop().");
        stopScreenRefresh();
        super.onStop();
        Log.i(TAG, "Leaving onStop().");
    }

    @Override
    public void onDestroy() {
        // December 2011 Frank Gruendel This is because Android Applications do not really stop. They only
        // retreat into the background. To really stop them one has to use the Android Settings Manager.
        Log.i(TAG, "Entering onDestroy().");
        Native.powerOffEmulator();
        stopScreenRefresh();
        super.onDestroy();
        Log.i(TAG, "Leaving onDestroy().");
    }

    // --- End of application life cycle

    /**
     * Updates the fullscreen status. The app is shown fullscreen if <code>statusBarVisible</code> is <code>false</code>.
     * Note that this method must be called before invoking <code>setContentView</code> in the <code>onCreate</code> method.
     */
    private void updateFullscreenStatus(boolean statusBarVisible) {
        Log.i(TAG, "Entering updateFullscreenStatus().");
        final int fullscreen = WindowManager.LayoutParams.FLAG_FULLSCREEN;
        final int notFullscreen = WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN;
        getWindow().addFlags(statusBarVisible ? notFullscreen : fullscreen);
        getWindow().clearFlags(statusBarVisible ? fullscreen : notFullscreen);
        pEinsteinView.requestLayout();
        Log.i(TAG, "Leaving updateFullscreenStatus().");
    }

    private void registerPreferenceChangeListener() {
        Log.i(TAG, "Entering registerPreferenceChangeListener().");
        sharedPrefs.registerOnSharedPreferenceChangeListener(this);
        Log.i(TAG, "Leaving registerPreferenceChangeListener().");
    }

    /**
     * Callback method invoked whenever a preference changes. Note that this method might
     * even be called if the actual value of the preference has not changed.
     */
    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        Log.i(TAG, "Entering onSharedPreferenceChanged().");
        // The keys are defined in preferences.xml
        if ("keepnetworkcardpluggedin".equals(key)) {
            Log.i(TAG, "Keep network card plugged in setting changed.");
        } else if ("screenpresets".equals(key)) {
            Log.i(TAG, "Screen resolution setting changed.");
            stopScreenRefresh();
            // Send the emulator to sleep so that everything ist saved
            if (Native.isPowerOn() != 0) {
                Native.sendPowerSwitchEvent();
                while (Native.isPowerOn() != 0) {
                    MiscUtils.sleepForMillis(100);
                    Log.i(TAG, "Waiting for power down.");
                }
            }
            ScreenDimensionsInitializer.initNewtonScreenDimensions(this);
            Native.changeScreenSize(ScreenDimensions.NEWTON_SCREEN_WIDTH, ScreenDimensions.NEWTON_SCREEN_HEIGHT);
            pEinsteinView.updateDimensions();
            Toast.makeText(getApplicationContext(), "Rebooting NewtonOS", Toast.LENGTH_LONG).show();
            Native.rebootEmulator();
            Native.powerOnEmulator();
            while (Native.isPowerOn() == 0) {
                MiscUtils.sleepForMillis(100);
                Log.i(TAG, "Waiting for power up.");
            }
            final int rate = Integer.valueOf(sharedPrefs.getString("screenrefreshrate", StartupConstants.DEFAULT_SCREEN_REFRESH_RATE));
            startScreenRefresh(rate);
            // TODO: reconnect the view somehow!
        } else if ("install_rom".equals(key)) {
            Log.i(TAG, "ROM installation path setting changed.");
        } else if ("androidstatusbar".equals(key)) {
            final boolean statusBarVisible = sharedPreferences.getBoolean("androidstatusbar", true);
            Log.i(TAG, "Status bar visibility setting changed.");
            updateFullscreenStatus(statusBarVisible);
        } else if ("screenrefreshrate".equals(key)) {
            Log.i(TAG, "Screen refresh rate setting changed.");
            int rate = Integer.valueOf(sharedPrefs.getString("screenrefreshrate", StartupConstants.DEFAULT_SCREEN_REFRESH_RATE));
            changeScreenRefresh(rate);
        }
        Log.i(TAG, "Leaving onSharedPreferenceChanged().");
    }

    private void startScreenRefresh(int rate) {
        Log.i(TAG, "Entering startScreenRefresh().");
        if (mScreenRefreshTask == null) {
            mScreenRefreshTask = new EinsteinActionHandler(pEinsteinView);
        }
        if (mScreenRefreshTimer == null) {
            mScreenRefreshTimer = new Timer(true);
            mScreenRefreshTimer.schedule(mScreenRefreshTask, 1000, 1000 / rate);
        }
        Log.i(TAG, "Leaving startScreenRefresh().");
    }

    private void changeScreenRefresh(int rate) {
        Log.i(TAG, "Entering changeScreenRefresh().");
        if (mScreenRefreshTimer != null) {
            mScreenRefreshTimer.cancel();
            mScreenRefreshTimer.purge();
            mScreenRefreshTimer = null;
            mScreenRefreshTimer = new Timer(true);
            mScreenRefreshTimer.schedule(mScreenRefreshTask, 1000 / rate, 1000 / rate);
        }
        Log.i(TAG, "Leaving changeScreenRefresh().");
    }

    private void stopScreenRefresh() {
        Log.i(TAG, "Entering stopScreenRefresh().");
        if (mScreenRefreshTimer != null) {
            mScreenRefreshTimer.cancel();
            mScreenRefreshTimer.purge();
            mScreenRefreshTimer = null;
        }
        mScreenRefreshTask = null;
        Log.i(TAG, "Leaving stopScreenRefresh().");
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {

        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == REQUEST_WRITE) {
            if (grantResults.length > 0
                    && permissions[0].equals(Manifest.permission.WRITE_EXTERNAL_STORAGE)
                    && (grantResults[0] == PackageManager.PERMISSION_GRANTED)) {

                init();
                return;
            }
            finish();
        }
    }
}