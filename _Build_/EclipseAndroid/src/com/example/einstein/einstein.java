// ==============================
// File:			einstein.java
// Project:			Einstein
//
// Copyright 2011 by Matthias Melcher (einstein@matthiasm.com)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

// Suggestion for Android Menu and Dialog setup, strictly divided into 
// one-shot actions and persistent settings (Preferences)

// 1: Menu
//  Maintenance >> 2
//  Install Newton Packages >> 3
//  Insert Network Card / Remove Network Card
//  [ ] Backlight (Checkbox) 
//  Preferences >> 4

// 2: Maintenance
//  Install NewtonOS ROM:
//   Creating the Newton ROM file >> Internet Link, Browser
//   From The Network >> 2.1
//   From SD Card >> 2.2
//   Via USB >> 2.3
//   Via Kies Air >> 2.4 (Only show if Kies Air exists)
//  Reset Newton:
//   Soft Reset >> Dialog: "Are you sure, this will..."
//   Power Reset >> Dialog: "Are you sure, this will..."
//   Hard Reset >> Dialog: "Are you sure, this will..."
//   Cold Boot >> Dialog: "Are you sure, this will..."

// 2.1 Network ROM install
//  Text Dialog, user can enter a URL (http, ftp, file...), OK, Cancel
//  OK -> Progress Dialog -> Verify ROM -> Success Dialog / Error Dialog

// 2.2 Newton ROM from SD Card
//  File Browser to search for the ROM as a plain file or zip on the current media, OK, Cancel
//  OK -> Verify ROM -> Success Dialog / Error Dialog

// 2.3 Newton via USB
//  Dialog Box "ROM file must be copied to /Downloads/Einstein ...", OK, Cancel
//  Launch settings to make device a USB drive

// 2.4 Newton via Kies Air
//  Dialog Box "ROM file must be copied to /Downloads/Einstein ...", OK, Cancel
//  Launch Kies Air

// 3: Install Newton Packages
//  Same mennu as (2), additionally we can provide and FTP setup to unna.org and others

// 4: Preferences
//  Screen Resolution
//   Preset Screen Resolution >> 4.1
//   Custom Resolution >> Enter two integers
//  Emulator Settings
//   RAM Size >> 4.1
//   [ ] Keep Network Card plugged in (Checkbox)
//  Host Settings
//   Resource Path >> Text Dialog defaulting to "/Downloads/Einstein" (or whatever)
//   Back button >> 4.2

// 4.1 Screen Resolution
//  ( ) Original MP: 320x480
//  ( ) Host Resolution: 800x1010 (or whatever the resolution may be minus the status bar)
//  ( ) 3/4 Host Resolution: 600x700 (or whatever the resolution may be minus the status bar)
//  ( ) 1/2 Host Resolution: 400x505 (or whatever the resolution may be minus the status bar)
//  ( ) 1/3 Host Resolution: 300x400 (or whatever the resolution may be minus the status bar)
//  ( ) Custom Resolution

// 4.2 Android Back Button
//  ( ) Quit Einstein, Reboot Newton
//  ( ) Pause Einstein, Newton Sleeps
//  ( ) Newton Dot Button
//  ( ) Newton Context Menu (not sure if I can implement that)


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

	/* We need to override these:
	 * 
	 * Activity Starts:  onCreate() (cold start)
	 *         visible:  onStart()  <-  onRestart()
	 *      foreground:  onResume()
	 *         running
	 *       backgroud:  onPause()
	 *       invisible:  onStop()
	 *        shutdown:  onDestroy()
	 *            kill?
	 *            
	 *        examples:
	 *           launch  create->start->resume
	 *    "Home" button  pause->stop
	 *     launch again  start->resume
	 *     "off" button  pause
	 *      "on" button  resume
	 *    "back" button  pause->stop->destroy 
	 *     launch again  create->start->resume
	 *  Android "sleep"  pause
	 *             wake  resume  
	 */

	/** Called when the activity is first created. */
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
		// TODO We are currently getting exceptions when using the getInt or getBoolean methods of sharedPreferences.
		// Apart from that, we are getting an exception when we click on the preferences menu after the Einstein emulator is started.
		if ("screenwidth".equals(key)) {
			DebugUtils.debugTextOnScreen(this, "screen width changed ");
			//DebugUtils.debugTextOnScreen(this, "screen width changed to " + String.valueOf(sharedPreferences.getInt(key, -1)));
		} else if ("screenheight".equals(key)) {
			DebugUtils.debugTextOnScreen(this, "screen height changed ");
			//DebugUtils.debugTextOnScreen(this, "screen height changed to " + String.valueOf(sharedPreferences.getInt(key, -1)));
		} else if ("keepnetwrorkcardpluggedin".equals(key)) {
			DebugUtils.debugTextOnScreen(this, "card setting changed ");
			//DebugUtils.debugTextOnScreen(this, "Network card plugged in state changed to " + String.valueOf(sharedPreferences.getBoolean(key, false)));
		} else if ("install_rom".equals(key)) {
			final String installationType = sharedPreferences.getString("install_rom", "1");
			DebugUtils.debugTextOnScreen(this, "ROM installation path changed to ".concat(installationType));
		}
	}

	@Override
	/** Used to create the menu. */
	public boolean onCreateOptionsMenu(Menu menu) {
		Log.e("XXXX", ">>>>>>>>>> onCreateOptionsMenu(Menu)");
		/*
		// now seems to be a good time to find out about the available screen space
		// so we can update the menu accordingly (can we? at run time?)
		DisplayMetrics displaymetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displaymetrics);
        int ht = displaymetrics.heightPixels;
        int wt = displaymetrics.widthPixels;
		Log.i("XXXX", "Display metrics are " + String.valueOf(wt) + "," + String.valueOf(ht));		
        Rect dstRect = new Rect();
        pEinsteinView.getDrawingRect(dstRect);
		Log.i("XXXX", "Drawing view is " + String.valueOf(dstRect.width()) + "," + String.valueOf(dstRect.height()));
		// Galaxy Note is 800x1280, available are 800x1230 
		 */

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
//			// Some feedback to the user
//			Toast.makeText(einstein.this, "Enter your user credentials.",
//				Toast.LENGTH_LONG).show();
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
		// TODO: FreeLibrary(...); or use Class ClassLoader
		// TODO: handle the function if it was called to free resources, but not to finish the app
		// if (!isFinishing()) { } 
	}

	@Override
	public void onBackPressed()
	{
		super.onBackPressed();
		// showDialog(DIALOG_REALLY_EXIT_ID);
		// this.finish();
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
			/*        case DIALOG_REALLY_EXIT_ID:
            dialog = new AlertDialog.Builder(this).setMessage(
                                "Are you sure you want to exit?")
            .setCancelable(false)
            .setPositiveButton("Yes",
                    new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    TestMain.this.finish();
                }
            })
            .setNegativeButton("No",
                    new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    dialog.cancel();
                }
            }).create();
            break; */
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
//
//	void warnUser(String msg) {
//		AlertDialog ad = new AlertDialog.Builder(this).create();  
//		ad.setCancelable(false);  
//		ad.setMessage(msg);  
//		ad.setButton("OK", new DialogInterface.OnClickListener() { 
//			@Override public void onClick(DialogInterface dialog, int which) { dialog.dismiss(); } 
//		} 
//		);  
//		ad.show();
//	}

	// load the entire native program as a library at startup
	static {
		System.loadLibrary("einstein");
	}

/*
 * 
 * 
 * permissions:
 * 	
Òandroid.permission.WRITE_EXTERNAL_STORAGEÓ
Òandroid.permission.READ_EXTERNAL_STORAGEÓ
<uses-permission android:name="android.permission.INTERNET" />


main.xml:
<?xml version="1.0" encoding="utf-8"?>
<LinearLayout xmlns:android="http://schemas.android.com/apk/res/android"
   android:orientation="vertical"
   android:layout_width="fill_parent"
   android:layout_height="fill_parent"
   >
<TextView  
   android:layout_width="fill_parent" 
   android:layout_height="wrap_content" 
   android:text="@string/hello"
   />
<Button 
    android:text="Start long running task.." 
    android:id="@+id/startBtn" 
    android:layout_width="fill_parent" 
    android:layout_height="wrap_content">
</Button>
</LinearLayout>
 * 
import java.io.BufferedInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;

import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class AndroAsync extends Activity {

    public static final int DIALOG_DOWNLOAD_PROGRESS = 0;
    private Button startBtn;
    private ProgressDialog mProgressDialog;

    // Called when the activity is first created. 
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        startBtn = (Button)findViewById(R.id.startBtn);
        startBtn.setOnClickListener(new OnClickListener(){
            public void onClick(View v) {
                startDownload();
            }
        });
    }

    private void startDownload() {
        String url = "http://farm1.static.flickr.com/114/298125983_0e4bf66782_b.jpg";
        new DownloadFileAsync().execute(url);
    }
    @Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
            case DIALOG_DOWNLOAD_PROGRESS:
                mProgressDialog = new ProgressDialog(this);
                mProgressDialog.setMessage("Downloading file..");
                mProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
                mProgressDialog.setCancelable(false);
                mProgressDialog.show();
                return mProgressDialog;
            default:
                return null;
        }
    }
    class DownloadFileAsync extends AsyncTask<String, String, String> {

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            showDialog(DIALOG_DOWNLOAD_PROGRESS);
        }

        @Override
        protected String doInBackground(String... aurl) {
            int count;

            try {
                URL url = new URL(aurl[0]);
                URLConnection conexion = url.openConnection();
                conexion.connect();

                int lenghtOfFile = conexion.getContentLength();
                Log.d("ANDRO_ASYNC", "Lenght of file: " + lenghtOfFile);

                InputStream input = new BufferedInputStream(url.openStream());
                OutputStream output = new FileOutputStream("/sdcard/some_photo_from_gdansk_poland.jpg");

                byte data[] = new byte[1024];

                long total = 0;

                while ((count = input.read(data)) != -1) {
                    total += count;
                    publishProgress(""+(int)((total*100)/lenghtOfFile));
                    output.write(data, 0, count);
                }

                output.flush();
                output.close();
                input.close();
            } catch (Exception e) {}
            return null;

        }
        protected void onProgressUpdate(String... progress) {
             Log.d("ANDRO_ASYNC",progress[0]);
             mProgressDialog.setProgress(Integer.parseInt(progress[0]));
        }

        @Override
        protected void onPostExecute(String unused) {
            dismissDialog(DIALOG_DOWNLOAD_PROGRESS);
        }
    }
}

 */

// ============================================================================== //
// I can't uninstall it, there seems to be some kind of 'Uninstall Shield'.       //
// ============================================================================== //

}