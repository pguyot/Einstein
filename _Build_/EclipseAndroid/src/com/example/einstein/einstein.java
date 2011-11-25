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

package com.example.einstein;

import com.example.einstein.constants.OtherConstants;
import com.example.einstein.constants.StringConstants;
import com.example.einstein.constants.URLConstants;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Timer;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Environment;
import android.text.Editable;
import android.util.Log;
import android.widget.EditText;


public class einstein extends Activity
{
	private EinsteinView pEinsteinView = null;
	private ProgressDialog mProgressDialog;
	private Timer mScreenRefreshTimer = null;
	private ScreenRefresh mScreenRefreshTask = null;

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
		super.onCreate(savedInstanceState);

		this.pEinsteinView = new EinsteinView(this);        
		super.setContentView(pEinsteinView);

		// FILE* file = fopen("/sdcard/hello.txt","w+");
		final String dataPath = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS) + "/Einstein"; 
		if (!installAssets(dataPath))
			return;
		this.initEmulator();
		this.runEmulator(dataPath);

		startScreenRefresh();	
	}

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
		Log.e("XXXX", ">>>>>>>>>> onDestroy()");
		stopEmulator();
		stopScreenRefresh();	
		super.onDestroy();
		// FreeLibrary(...); or use Class ClassLoader
	}

	// native test method implemented in app/AndroidGlue.c
	public native String stringFromJNI();

	// initialize the emulator
	public native void initEmulator();

	// launch the emulator
	public native void runEmulator(String dataPath);

	// stop the emulator
	public native void stopEmulator();

	// wake emulator from sleep
	public native void powerOnEmulator();
	
	// send emulator to sleep
	public native void powerOffEmulator();
	
	// check if the screen contents has changed since the last call
	public native int screenIsDirty();
	
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

	public boolean installAssets(String dataPath) {
		/* TODO: 
		 * - check all install locations for valid ROM
		 *   - check EXT/Einstein
		 *   - check /sdcard/Einstein
		 *   - check /sdcard/external_sd/Einstein
		 *   - check /sdcard/sd/Einstein
		 *   - further possible dirs are /system/media/sdcard/... and /mnt/sdcard/... 
		 *   - check /proc/self/mounts ... Einstein (don't! This is messy!)
		 *   - check EXT/download/Einstein (outdated)
		 * - check download directory for .zip
		 *   - if found, extract into EXT/Einstein
		 * - message to the user
		 */

		// setText(); // this is asynchronous! No keyboard is shown!
		// startDownload(); // this is asynchronous!

		final File dataDir = new File(dataPath);
		dataDir.mkdirs();
		final AssetManager assetManager = getAssets();

		try {
			// TODO Do not hard-code the path separator and the ROM name. The path separator is platform dependent,
			// and the ROM name can change FG_2011_11_24
			final File rom = new File(dataPath + "/717006.rom");
			if (rom.exists()) {
				Log.i("Einstein", "ROM file found");
			} else {
				InputStream in = null;
				FileOutputStream out = null;
				try {
					in = assetManager.open("717006.rom.png");
					Log.w("Einstein", "Copying ROM file from assets...");
					out = new FileOutputStream(dataPath+"/717006.rom");
					int sz = in.available();
					byte[] b = new byte[sz];
					in.read(b);
					out.write(b);
				} catch (IOException e) {
					finishWithMessage(StringConstants.Einstein_005);
					return false;
				}
				finally {
					in.close();
					out.close();
					Log.w("Einstein", "Done.");
				}
			}
			final File rex = new File(dataPath + "/Einstein.rex");
			if (rex.exists()) {
				Log.i("Einstein", "REX file found.");
			} else {
				InputStream in = null;
				FileOutputStream out = null;
				try {
					in = assetManager.open("Einstein.rex.png");
					Log.w("Einstein", "Copying REX file from assets...");
					out = new FileOutputStream(dataPath+"/Einstein.rex");
					int sz = in.available();
					byte[] b = new byte[sz];
					in.read(b);
					out.write(b);
				} catch (IOException e) {
					finishWithMessage(StringConstants.Einstein_005);
					return false;
				}
				finally {
					in.close();
					out.close();
					Log.w("Einstein", "Done.");
				}
			}
			final File img = new File(dataPath + "/717006.img");
			if (!img.exists()) {
				warnUser(StringConstants.Einstein_006);
			} 
		} catch (IOException e) {
			Log.e("Einstein", "Installing assets: " + e.getMessage());
		}
		return true;
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

	void warnUser(String msg) {
		AlertDialog ad = new AlertDialog.Builder(this).create();  
		ad.setCancelable(false);  
		ad.setMessage(msg);  
		ad.setButton("OK", new DialogInterface.OnClickListener() { 
			@Override public void onClick(DialogInterface dialog, int which) { dialog.dismiss(); } 
		} 
		);  
		ad.show();
	}

	// load the entire native program as a library at startup
	static {
		System.loadLibrary("einstein");
	}
}

/*
 * 
 * http://www.weinbrennerei-dujardin.de/Newton/717006.rom
 * 
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

