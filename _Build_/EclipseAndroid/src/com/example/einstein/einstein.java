package com.example.einstein;

import java.io.File;
import java.util.Timer;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.content.res.AssetManager;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.text.Editable;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;
import android.widget.EditText;

import com.example.einstein.constants.OtherConstants;
import com.example.einstein.constants.StringConstants;
import com.example.einstein.constants.URLConstants;
import com.example.einstein.prefs.EinsteinPreferencesActivity;
import com.example.einstein.startup.IStartup.LoadResult;
import com.example.einstein.startup.Startup;
import com.example.einstein.startup.StartupConstants;
import com.example.einstein.utils.screen.ScreenDimensions;


public class einstein extends Activity implements OnSharedPreferenceChangeListener
{
	private EinsteinView pEinsteinView = null;
	private ProgressDialog mProgressDialog; 
	private Timer mScreenRefreshTimer = null;
	private ScreenRefresh mScreenRefreshTask = null;
	private SharedPreferences sharedPrefs;
	private SharedPreferences.OnSharedPreferenceChangeListener sharedPrefsListener;

	// load the entire native program as a library at startup
	static {
		System.loadLibrary("einstein");
	}

	/** Called when the app is started for the first time or after it has been forcefully terminated from within Android. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		DebugUtils.appendLog("einstein.onCreate: Entered method");
		super.onCreate(savedInstanceState);
		Log.e("einstein", ">>>>>>>>>> onCreate()");
		this.sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);
		DebugUtils.appendLog("einstein.onCreate: Got preferences");
		// Create an intance of EinsteinPreferencesActivity. If we do not do this, the preferences that are calculated at
		// runtime wouldn't exist until the user has invoked the preferences window for the first time
		// Install all required assets, initialize host device dependant values, ...
		final Startup startup = new Startup(this);
		DebugUtils.appendLog("einstein.onCreate: CalledStartup");
		final AssetManager assetManager = getAssets();
		final LoadResult result = startup.installAssets(assetManager);
		if (LoadResult.OK != result) {
			DebugUtils.appendLog("einstein.onCreate: Problem installing assets. Result is " + result.toString());
			return;
		}
		DebugUtils.appendLog("einstein.onCreate: Pre-initialization finished successfully");
		// Register listener that'll notify us of preference changes
		this.registerPreferenceChangeListener();
		// Initialize emulator
		this.initEmulator("CONSOLE");
		// Create view
		this.pEinsteinView = new EinsteinView(this);     
		// Show or hide Android status bar. Note that this must take place before we call setContentView
		DebugUtils.appendLog("einstein.onCreate: Einstein view creation finished successfully");
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		final boolean statusBarVisible = this.sharedPrefs.getBoolean("androidstatusbar", true);
		DebugUtils.appendLog("einstein.onCreate: Status bar is " + (statusBarVisible ? "visible" : "not visible"));
		this.updateFullscreenStatus(statusBarVisible);
		super.setContentView(pEinsteinView);
		// Start emulator
		DebugUtils.appendLog("einstein.onCreate: Starting emulator with " + ScreenDimensions.HOST_SCREEN_SIZE);
		this.runEmulator(StartupConstants.DATA_FILE_PATH, ScreenDimensions.NEWTON_SCREEN_WIDTH, ScreenDimensions.NEWTON_SCREEN_HEIGHT);
		startScreenRefresh();	
	}
	
	/** Updates the fullscreen status. The app is shown fullscreen if <code>statusBarVisible</code> is <code>false</code>.
	 * Note that this method must be called before invoking <code>setContentView</code> in the <code>onCreate</code> method. */
	private void updateFullscreenStatus(boolean statusBarVisible)
	{  
		final int fullscreen = WindowManager.LayoutParams.FLAG_FULLSCREEN;
		final int notFullscreen = WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN;
        super.getWindow().addFlags(statusBarVisible ? notFullscreen : fullscreen);
        super.getWindow().clearFlags(statusBarVisible ? fullscreen : notFullscreen);
        this.pEinsteinView.requestLayout();
	}
	
	// The following two methods aren't used yet, but we'll need them when we implement picking
	// the ROM and REX files. Shamelessly copied from http://www.blackmoonit.com/android/filebrowser/intents#intent.pick_file
	
	void pickFile(File aFile) {
	    Intent theIntent = new Intent(Intent.ACTION_PICK);
	    theIntent.setData(Uri.fromFile(aFile));  //default file / jump directly to this file/folder
	    theIntent.putExtra(Intent.EXTRA_TITLE,"A Custom Title"); //optional
	    theIntent.addFlags(Intent.FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS); //optional
	    try {
	    	this.startActivityForResult(theIntent,Activity.RESULT_FIRST_USER);
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

	private void registerPreferenceChangeListener() {
		this.sharedPrefs.registerOnSharedPreferenceChangeListener(this);
	}

	/** Callback method invoked whenever a preference changes. Note that this method might
	 *  even be called if the actual value of the preference has not changed. */
	@Override
	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
		// The keys are defined in preferences.xml
		if ("keepnetworkcardpluggedin".equals(key)) {
			DebugUtils.debugTextOnScreen(this, "card setting changed ");
		} else if ("screenpresets".equals(key)) {
			DebugUtils.debugTextOnScreen(this, "Screen resolution changed");
		} else if ("install_rom".equals(key)) {
			final String installationType = sharedPreferences.getString("install_rom", "1");
			DebugUtils.debugTextOnScreen(this, "ROM installation path changed to ".concat(installationType));
		} else if ("androidstatusbar".equals(key)) {
			final boolean statusBarVisible = sharedPreferences.getBoolean("androidstatusbar", true);
			DebugUtils.debugTextOnScreen(this, "Status bar was turned ".concat(statusBarVisible ? "on" : "off"));
			//this.updateFullscreenStatus(statusBarVisible);
		}
	}

	@Override
	/** Used to create the menu. */
	public boolean onCreateOptionsMenu(Menu menu) {
		Log.e("XXXX", ">>>>>>>>>> onCreateOptionsMenu(Menu)");
		// Create the menu using the XML file
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.mainmenu, menu);
		return true;
	}

	@Override
	/** In case you want to disable or hide menu items you can use the method "onPrepareOptionsMenu",
		which is called every time the menu is called. */
	public boolean onOptionsItemSelected(MenuItem item) {
		Log.e("XXXX", ">>>>>>>>>> onOptionsItemSelected(MenuItem)");
		switch (item.getItemId()) {
			case R.id.preferences:
				// Launch Preferences activity
				final Intent intent = new Intent(einstein.this, EinsteinPreferencesActivity.class);
				startActivity(intent);
				break;
			case R.id.backlight:
				if (backlightIsOn()==1)
					setBacklight(0);
				else
					setBacklight(1);
				break;
			case R.id.installNewPackages:
				this.installNewPackages();
				break;
			case R.id.networkCard:
				this.toggleNetworkCard();
				break;
		}
		return true;	}

	@Override
	public void onStart()
	{
		Log.e("XXXX", ">>>>>>>>>> onStart()");
		super.onStart();
		startScreenRefresh();	
	}

	@Override
	public void onResume()
	{
		Log.e("XXXX", ">>>>>>>>>> onResume()");
		super.onResume();
		startScreenRefresh();
		powerOnEmulator();
	}

	@Override
	public void onPause()
	{
		Log.e("XXXX", ">>>>>>>>>> onPause()");
		powerOffEmulator();
		stopScreenRefresh();	
		super.onPause();
	}

	@Override
	public void onStop()
	{
		Log.e("XXXX", ">>>>>>>>>> onStop()");
		stopScreenRefresh();	
		super.onStop();
	}

	@Override
	public void onDestroy()
	{
		// TODO: the native resources are not given back! All threads continue to run.
		// TODO: even the Java timer continues to query screen updates! Weird!
		// December 2011 Frank Gruendel This is because Android Applications do not really stop. They only retreat into the background.
		// To really stop them one has to use the Android Settings Manager.
		Log.e("XXXX", ">>>>>>>>>> onDestroy()");
		stopEmulator();
		stopScreenRefresh();	
		super.onDestroy();
	}

	// native test method implemented in app/AndroidGlue.c
	public native String stringFromJNI();

	// initialize the emulator
	// logPath can be any path name in the Android file sytem
	// Special values for logPath are "NULL" or "" for no log, "STDOUT" for logging to some stdout device,
	// and "CONSOLE" for logging to the Android debugging log (LogCat on Eclipse). 
	public native void initEmulator(String logPath);

	// launch the emulator
	public native void runEmulator(String dataPath, int screenWidth, int screenHeight);

	// stop the emulator
	public native void stopEmulator();

	// wake emulator from sleep
	public native void powerOnEmulator();

	// send emulator to sleep
	public native void powerOffEmulator();

	// toggle the network card in and out of the PCMCIA slot
	public native void toggleNetworkCard();

	// check if the screen contents has changed since the last call
	public native int screenIsDirty();

	// switch backlight on or off
	public native void setBacklight(int v);

	// check if backlight is on
	public native int backlightIsOn();

	// install packages that are new in the Einstein directory
	public native void installNewPackages();

	private void startDownload() {
		new DownloadFileAsync(this).execute(URLConstants.SHIP_IMAGE);
	}

	@Override
	protected Dialog onCreateDialog(int id) {
		switch (id) {
		case OtherConstants.DIALOG_DOWNLOAD_PROGRESS:
			this.mProgressDialog = new ProgressDialog(this);
			this.mProgressDialog.setMessage(StringConstants.Einstein_001); // Downloading file..
			this.mProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
			this.mProgressDialog.setCancelable(false);
			this.mProgressDialog.show();
			return this.mProgressDialog;		
		default:
			return null;
		}
	}

	private void startScreenRefresh() {
		if (mScreenRefreshTask==null) {
			mScreenRefreshTask = new ScreenRefresh(this, pEinsteinView);
		}
		if (mScreenRefreshTimer==null) {
			mScreenRefreshTimer = new Timer(true);
			mScreenRefreshTimer.schedule(mScreenRefreshTask, 1000, 100);
		}
	}

	private void stopScreenRefresh() {
		if (mScreenRefreshTimer!=null) {
			mScreenRefreshTimer.cancel();
			mScreenRefreshTimer.purge();
			mScreenRefreshTimer = null;
		}
		mScreenRefreshTask = null;
	}

	private String setText(){
		final AlertDialog.Builder alert = new AlertDialog.Builder(this);
		alert.setTitle(StringConstants.Einstein_002); // Download the ROM
		alert.setMessage(StringConstants.Einstein_003); // Enter the URL of the Newton ROM file
		final EditText input = new EditText(this);
		alert.setView(input);
		final String out = null;
		alert.setPositiveButton(StringConstants.Einstein_004, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int which) {
				Editable ed = input.getText();
				String out = ed.toString();
				Log.i("Einstein", out);
			}
		});
		alert.show();
		return out;
	}

	void finishWithMessage(String msg) {
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
	}

}