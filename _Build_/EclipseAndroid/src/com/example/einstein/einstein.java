package com.example.einstein;

import java.util.Timer;

import com.example.einstein.constants.DimensionConstants;
import com.example.einstein.constants.OtherConstants;
import com.example.einstein.constants.StringConstants;
import com.example.einstein.constants.URLConstants;
import com.example.einstein.startup.IStartup.LoadResult;
import com.example.einstein.startup.Startup;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.OnSharedPreferenceChangeListener;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.text.Editable;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.EditText;


public class einstein extends Activity implements OnSharedPreferenceChangeListener
{
	private EinsteinView pEinsteinView = null;
	private ProgressDialog mProgressDialog; 
	private Timer mScreenRefreshTimer = null;
	private ScreenRefresh mScreenRefreshTask = null;
	private SharedPreferences sharedPrefs;
	private SharedPreferences.OnSharedPreferenceChangeListener sharedPrefsListener;

	/** Called when the app is started for the first time. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		Log.e("einstein", ">>>>>>>>>> onCreate()");    	
		DebugUtils.debugTextOnScreen(this, "onCreate");
		super.onCreate(savedInstanceState);
		this.pEinsteinView = new EinsteinView(this);     
		super.setContentView(pEinsteinView);
		// Install all required assets
		final Startup startup = new Startup(this);
		final AssetManager assetManager = getAssets();
		final LoadResult result = startup.installAssets(assetManager);
		if (!(LoadResult.OK == result)) {
			return;
		}
		// Register listener that'll notify us of preference changes
		this.registerPreferenceChangeListener();
		DebugUtils.debugTextOnScreen(this, "initEmulator");
		this.initEmulator();
		final String dataPath = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS) + "/Einstein"; 
		this.runEmulator(dataPath, DimensionConstants.SCREEN_WIDTH, DimensionConstants.SCREEN_HEIGHT);

		startScreenRefresh();	

	}

	private void registerPreferenceChangeListener() {
		this.sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);
		this.sharedPrefs.registerOnSharedPreferenceChangeListener(this);
	}

	/** Callback method invoked whenever a preference changes. Note that this method might
	 *  even be called if the actual value of the preference has not changed. */
	@Override
	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
		// The keys are defined in preferences.xml
		if ("screenwidth".equals(key)) {
			DebugUtils.debugTextOnScreen(this, "screen width changed ");
		} else if ("screenheight".equals(key)) {
			DebugUtils.debugTextOnScreen(this, "screen height changed ");
		} else if ("keepnetwrorkcardpluggedin".equals(key)) {
			DebugUtils.debugTextOnScreen(this, "card setting changed ");
		} else if ("install_rom".equals(key)) {
			final String installationType = sharedPreferences.getString("install_rom", "1");
			DebugUtils.debugTextOnScreen(this, "ROM installation path changed to ".concat(installationType));
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
	/** This method's behavior is currently hard-coded to show a short message. Soon it will call the preference
		settings. In case you want to disable or hide menu items you can use the method "onPrepareOptionsMenu",
		which is called every time the menu is called. */
	public boolean onOptionsItemSelected(MenuItem item) {
		Log.e("XXXX", ">>>>>>>>>> onOptionsItemSelected(MenuItem)");
		// We have only one menu option yet, but this might change. So we'll use a switch
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
	public native void initEmulator();

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

	// load the entire native program as a library at startup
	static {
		System.loadLibrary("einstein");
	}

}