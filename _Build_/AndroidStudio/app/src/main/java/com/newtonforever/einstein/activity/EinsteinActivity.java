// TODO FG Review
package com.newtonforever.einstein.activity;

import android.Manifest;
import android.annotation.TargetApi;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.newtonforever.einstein.Einstein;
import com.newtonforever.einstein.EinsteinApplication;
import com.newtonforever.einstein.R;
import com.newtonforever.einstein.action.EinsteinActionHandler;
import com.newtonforever.einstein.jni.Native;
import com.newtonforever.einstein.startup.IStartup.LoadResult;
import com.newtonforever.einstein.startup.Startup;
import com.newtonforever.einstein.startup.StartupConstants;
import com.newtonforever.einstein.utils.MiscUtils;
import com.newtonforever.einstein.utils.StringUtils;
import com.newtonforever.einstein.utils.debug.DebugUtils;
import com.newtonforever.einstein.utils.screen.ScreenDimensions;
import com.newtonforever.einstein.utils.screen.ScreenDimensionsInitializer;
import com.newtonforever.einstein.view.EinsteinView;

import java.io.File;
import java.util.Timer;

/**
 * The main user interface to the emulator.
 * 
 * This class manages the visual representation of the emulator and
 * the screen interface.
 * 
 * @author matt
 */
public class EinsteinActivity extends Activity implements OnSharedPreferenceChangeListener
{    
    // Be aware that dialog ID values are arbitrary, but need to be unique within the Activity.
    private static final int DIALOG_DOWNLOAD_PROGRESS_ID = 0;

    private static final int REQUEST_WRITE = 1;

    private static EinsteinActivity pInstance = null;
    private Einstein pEinstein = null;
    private EinsteinView pEinsteinView = null;
    private ProgressDialog mProgressDialog; 
    private Timer mScreenRefreshTimer = null;
    private EinsteinActionHandler mScreenRefreshTask = null;
    private SharedPreferences sharedPrefs;
    private Startup startup;

    // Used to load the 'native-lib' library on application startup.
    /*
    static {
        System.loadLibrary("native-lib");
    }
    */

    public static EinsteinActivity getInstance() {
        return pInstance;
    }
/*
    public void DownloadFile(String src, String dst){

        try {
            URL u = new URL(src);
            InputStream is = u.openStream();

            DataInputStream dis = new DataInputStream(is);

            byte[] buffer = new byte[1024];
            int length;

            FileOutputStream fos = new FileOutputStream(new File(dst));
            while ((length = dis.read(buffer))>0) {
              fos.write(buffer, 0, length);
            }
            fos.close();

          } catch (MalformedURLException mue) {
            Log.e("SYNC getUpdate", "malformed url error", mue);
          } catch (IOException ioe) {
            Log.e("SYNC getUpdate", "io error", ioe);
          } catch (SecurityException se) {
            Log.e("SYNC getUpdate", "security error", se);
          }
}
*/
    // --- beginning of life cycle

    /** 
     * \brief Android creates a new activity, launching or waking Einstein.
     * 
     * If called for the first time, it starts Einstein.
     * 
     * The Activity then creates a view and connect Einstein to that view.
     * 
     * \todo Preferences are not managed well yet, just as proof of concept.
     * \todo No ROM management is implemented yet.
     */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onCreate().");
        pInstance = this;

        super.onCreate(savedInstanceState);

        // Download the ROM
        //DebugUtils.logGreen("ERR", "A");
        //DownloadFile("http://www.matthiasm.com/717006", "/mnt/sdcard/Download/Einstein/717006.rom");
        //DebugUtils.logGreen("ERR", "B");

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

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (checkCallingOrSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, REQUEST_WRITE);
                return;
            }
        }
        // Install all required assets, initialize host device dependent values, ...
        init();
    }

    private void init() {
        DebugUtils.logGreen("EinsteinActivity: ", "Creating Einstein application.");
        final EinsteinApplication app = (EinsteinApplication) getApplication();
        pEinstein = app.getEinstein();

        final LoadResult result = startup.installAssets();
        if (LoadResult.OK != result) {
            DebugUtils.logRed("EinsteinActivity: ", "Problem with installing assets.");
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
            //DebugUtils.logGreen("EinsteinActivity: ", "Sleeping for 2s after calling run because we're using Frank's ROM...");
        }
        final int rate = Integer.valueOf(sharedPrefs.getString("screenrefreshrate", StartupConstants.DEFAULT_SCREEN_REFRESH_RATE));
        startScreenRefresh(rate);
        // TODO FG 2013_10_19 Only required when using Frank's Flash ROM board data. Remove in a final version.
        //MiscUtils.sleepForMillis(2000);
        //DebugUtils.logGreen("EinsteinActivity: ", "Sleeping for 2s after starting screen refresh because we're using Frank's ROM...");

        app.raisePriority();
        
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving onCreate().");
    }

    @Override
    public void onStart()
    {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onStart().");
        super.onStart();
        int rate = Integer.valueOf(sharedPrefs.getString("screenrefreshrate", StartupConstants.DEFAULT_SCREEN_REFRESH_RATE));
        startScreenRefresh(rate);	
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving onStart().");
    }

    @Override
    public void onResume()
    {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onResume().");
        super.onResume();
        int rate = Integer.valueOf(sharedPrefs.getString("screenrefreshrate", StartupConstants.DEFAULT_SCREEN_REFRESH_RATE));
        startScreenRefresh(rate);
        Native.powerOnEmulator();
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving onResume().");
    }

    @Override
    public void onNewIntent(Intent intent) 
    {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onNewIntent().");
        if (intent.getBooleanExtra("EXIT", false)) {
            DebugUtils.logGreen("EinsteinActivity: ", "We need to exit...");
            finish();
        }
        String file = intent.getStringExtra("FILE"); 
        if (file!=null) {
            Native.installPackage(file);
            intent.removeExtra("FILE");
        }
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving onNewIntent().");
    }

    @Override
    public void onPause()
    {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onPause().");
        super.onPause();
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving onPause().");
    }

    @Override
    public void onStop()
    {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onStop().");
        stopScreenRefresh();	
        super.onStop();
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving onStop().");
    }

    @Override
    public void onDestroy()
    {
        // December 2011 Frank Gruendel This is because Android Applications do not really stop. They only
        // retreat into the background. To really stop them one has to use the Android Settings Manager.
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onDestroy().");
        Native.powerOffEmulator();
        stopScreenRefresh();	
        super.onDestroy();
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving onDestroy().");
    }

    // --- End of application life cycle


    // The following two methods aren't used yet, but we'll need them when we implement picking
    // the ROM and REX files. Shamelessly copied from http://www.blackmoonit.com/android/filebrowser/intents#intent.pick_file

    @SuppressWarnings("unused")
    private void  pickFile(File aFile) {
        Intent theIntent = new Intent(Intent.ACTION_PICK);
        theIntent.setData(Uri.fromFile(aFile));  //default file / jump directly to this file/folder
        theIntent.putExtra(Intent.EXTRA_TITLE,"A Custom Title"); //optional
        theIntent.addFlags(Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS); //optional
        try {
            startActivityForResult(theIntent,Activity.RESULT_FIRST_USER);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data){
        final int PICK_FILE_RESULT_CODE = Activity.RESULT_FIRST_USER; // ??
        switch (requestCode) {
            case PICK_FILE_RESULT_CODE: {
                if (resultCode==RESULT_OK && data!=null && data.getData()!=null) {
                    //String theFilePath = data.getData().getPath();
                    // TODO Check if we can use the file. If we can, copy it where it belongs
                }
                break;
            }
        }
    }

    /** Updates the fullscreen status. The app is shown fullscreen if <code>statusBarVisible</code> is <code>false</code>.
     * Note that this method must be called before invoking <code>setContentView</code> in the <code>onCreate</code> method. */
    private void updateFullscreenStatus(boolean statusBarVisible)
    {  
        DebugUtils.logGreen("EinsteinActivity: ", "Entering updateFullscreenStatus().");
        final int fullscreen = WindowManager.LayoutParams.FLAG_FULLSCREEN;
        final int notFullscreen = WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN;
        getWindow().addFlags(statusBarVisible ? notFullscreen : fullscreen);
        getWindow().clearFlags(statusBarVisible ? fullscreen : notFullscreen);
        pEinsteinView.requestLayout();
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving updateFullscreenStatus().");
    }

    private void registerPreferenceChangeListener() {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering registerPreferenceChangeListener().");
        sharedPrefs.registerOnSharedPreferenceChangeListener(this);
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving registerPreferenceChangeListener().");
    }

    /** Callback method invoked whenever a preference changes. Note that this method might
     *  even be called if the actual value of the preference has not changed. */
    @Override
    public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onSharedPreferenceChanged().");
        // The keys are defined in preferences.xml
        if ("keepnetworkcardpluggedin".equals(key)) {
            DebugUtils.logGreen("EinsteinActivity: ", "Keep network card plugged in setting changed.");
        } else if ("screenpresets".equals(key)) {
            DebugUtils.logGreen("EinsteinActivity: ", "Screen resolution setting changed.");
            stopScreenRefresh();
            // Send the emulator to sleep so that everything ist saved
            if (Native.isPowerOn()!=0) {
                Native.sendPowerSwitchEvent();
                while (Native.isPowerOn()!=0) {
                    MiscUtils.sleepForMillis(100);
                    DebugUtils.logGreen("EinsteinActivity: ", "Waiting for power down.");
                }
            }
            ScreenDimensionsInitializer.initNewtonScreenDimensions(this);
            Native.changeScreenSize(ScreenDimensions.NEWTON_SCREEN_WIDTH, ScreenDimensions.NEWTON_SCREEN_HEIGHT);
            pEinsteinView.updateDimensions();
            Toast.makeText(getApplicationContext(), "Rebooting NewtonOS", Toast.LENGTH_LONG).show();		
            Native.rebootEmulator();
            Native.powerOnEmulator();
            while (Native.isPowerOn()==0) {
                MiscUtils.sleepForMillis(100);
                DebugUtils.logGreen("EinsteinActivity: ", "Waiting for power up.");
            }
            final int rate = Integer.valueOf(sharedPrefs.getString("screenrefreshrate", StartupConstants.DEFAULT_SCREEN_REFRESH_RATE));
            startScreenRefresh(rate);
            // TODO: reconnect the view somehow!
        } else if ("install_rom".equals(key)) {
            DebugUtils.logGreen("EinsteinActivity: ", "ROM installation path setting changed.");
        } else if ("androidstatusbar".equals(key)) {
            final boolean statusBarVisible = sharedPreferences.getBoolean("androidstatusbar", true);
            DebugUtils.logGreen("EinsteinActivity: ", "Status bar visibility setting changed.");
            updateFullscreenStatus(statusBarVisible);
        } else if ("screenrefreshrate".equals(key)) {
            DebugUtils.logGreen("EinsteinActivity: ", "Screen refresh rate setting changed.");
            int rate = Integer.valueOf(sharedPrefs.getString("screenrefreshrate", StartupConstants.DEFAULT_SCREEN_REFRESH_RATE));
            changeScreenRefresh(rate);
        }
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving onSharedPreferenceChanged().");
    }

    @Override
    /** Used to create the menu. */
    public boolean onCreateOptionsMenu(Menu menu) {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onCreateOptionsMenu() and returning true.");
        return true;
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onCreateDialog().");
        switch (id) {
            case DIALOG_DOWNLOAD_PROGRESS_ID:
                this.mProgressDialog = new ProgressDialog(this);
                this.mProgressDialog.setMessage(StringUtils.getLocalizedString(this.getResources(), R.string.Startup_downloadingFile));
                this.mProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
                this.mProgressDialog.setCancelable(false);
                this.mProgressDialog.show();
                DebugUtils.logGreen("EinsteinActivity: ", "Leaving onCreateDialog().");
                return this.mProgressDialog;		
            default:
                DebugUtils.logGreen("EinsteinActivity: ", "Leaving onCreateDialog().");
                return null;
       }
    }

    private void startScreenRefresh(int rate) {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering startScreenRefresh().");
        if (mScreenRefreshTask==null) {
            mScreenRefreshTask = new EinsteinActionHandler(pEinsteinView);
        }
        if (mScreenRefreshTimer==null) {
            mScreenRefreshTimer = new Timer(true);
            mScreenRefreshTimer.schedule(mScreenRefreshTask, 1000, 1000/rate);
        }
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving startScreenRefresh().");
    }

    private void changeScreenRefresh(int rate) {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering changeScreenRefresh().");
        if (mScreenRefreshTimer!=null) {
            mScreenRefreshTimer.cancel();
            mScreenRefreshTimer.purge();
            mScreenRefreshTimer = null;
            mScreenRefreshTimer = new Timer(true);
            mScreenRefreshTimer.schedule(mScreenRefreshTask, 1000/rate, 1000/rate);
        }
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving changeScreenRefresh().");
   }

    private void stopScreenRefresh() {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering stopScreenRefresh().");
        if (mScreenRefreshTimer!=null) {
            mScreenRefreshTimer.cancel();
            mScreenRefreshTimer.purge();
            mScreenRefreshTimer = null;
        }
        mScreenRefreshTask = null;
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving stopScreenRefresh().");
    }

    void finishWithMessage(String msg) {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering finishWithMessage().");
        class MyOnClickListener implements DialogInterface.OnClickListener {
            Activity pv = null;
            MyOnClickListener(Activity v) { pv = v; }
            @Override public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                pv.finish();
            }
        };
        AlertDialog ad = new AlertDialog.Builder(this).create();  
        ad.setCancelable(false);
        ad.setMessage(msg);  
        ad.setButton("Quit", new MyOnClickListener(this));  
        ad.show();
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving finishWithMessage().");
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        DebugUtils.logGreen("EinsteinActivity: ", "Entering onKeyDown().");
        if ( keyCode == KeyEvent.KEYCODE_MENU ) {
            Intent intent = new Intent(this, ActionsActivity.class);  
            startActivity(intent);
            return true;
        }
        DebugUtils.logGreen("EinsteinActivity: ", "Leaving onKeyDown().");
        return super.onKeyDown(keyCode, event);
    }

    @Override
    @TargetApi(Build.VERSION_CODES.M)
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (requestCode == REQUEST_WRITE) {
            if (Manifest.permission.WRITE_EXTERNAL_STORAGE.equals(permissions[0]) && (grantResults[0] == PackageManager.PERMISSION_GRANTED)) {
                init();
                return;
            }
            finish();
        }
    }
}