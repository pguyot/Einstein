// TODO FG Review

package com.newtonforever.einstein;

import java.io.File;
import java.util.Timer;

import org.messagepademu.einstein.Einstein;
import org.messagepademu.einstein.EinsteinView;
import org.messagepademu.einstein.R;

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
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.newtonforever.einstein.actions.ActionsActivity;
import com.newtonforever.einstein.startup.IStartup.LoadResult;
import com.newtonforever.einstein.startup.Startup;
import com.newtonforever.einstein.startup.StartupConstants;
import com.newtonforever.einstein.utils.StringUtils;
import com.newtonforever.einstein.utils.debug.DebugUtils;
import com.newtonforever.einstein.utils.screen.ScreenDimensions;
import com.newtonforever.einstein.utils.screen.ScreenDimensionsInitializer;


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
	private static EinsteinActivity pInstance = null;
	
	// Note that dialog ID values are arbitrary, but need to be unique within the Activity.
	private static final int DIALOG_DOWNLOAD_PROGRESS_ID = 0;


	public static EinsteinActivity getInstance() {
		return pInstance;
	}
	
	private Einstein pEinstein = null;
	private EinsteinView pEinsteinView = null;
	private ProgressDialog mProgressDialog; 
	private Timer mScreenRefreshTimer = null;
	private ScreenRefresh mScreenRefreshTask = null;
	private SharedPreferences sharedPrefs;
//	private SharedPreferences.OnSharedPreferenceChangeListener sharedPrefsListener;

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
		pInstance = this;
		
		DebugUtils.appendLog("einstein.onCreate: Entered method");
		super.onCreate(savedInstanceState);
		Log.i("einstein", "------> Activity.onCreate()");

		EinsteinApplication app = (EinsteinApplication)getApplication();
		pEinstein = app.getEinstein();

		this.sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);
		DebugUtils.appendLog("einstein.onCreate: Got preferences");
		// Create an instance of EinsteinPreferencesActivity. If we do not do this, the preferences that are calculated at
		// runtime wouldn't exist until the user has invoked the preferences window for the first time
		// Install all required assets, initialize host device dependent values, ...
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
		
		if (!pEinstein.isRunning()) {
			// Initialize emulator
			pEinstein.initEmulator("CONSOLE");
		}
		
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
		
		if (pEinstein.isRunning()) {
			Toast.makeText(getApplicationContext(), "Reconnecting to Einstein", Toast.LENGTH_SHORT).show();		
			// wake up
		} else {
			String id = this.sharedPrefs.getString("newtonid", "00004E6577746F6E");
			pEinstein.setNewtonID(id);
			pEinstein.run(StartupConstants.DATA_FILE_PATH, ScreenDimensions.NEWTON_SCREEN_WIDTH, ScreenDimensions.NEWTON_SCREEN_HEIGHT);
			Toast.makeText(getApplicationContext(), "Booting Einstein", Toast.LENGTH_LONG).show();		
		}
		int rate = Integer.valueOf(this.sharedPrefs.getString("screenrefreshrate", "10"));
		Log.e("REFRESH", "Rate is "+rate);
		startScreenRefresh(rate);
		
		Log.i("einstein", "------< Activity.onCreate()");

		app.raisePriority();
	}
	
	@Override
	public void onStart()
	{
		Log.e("XXXX", ">>>>>>>>>> onStart()");
		super.onStart();
		int rate = Integer.valueOf(this.sharedPrefs.getString("screenrefreshrate", "10"));
		startScreenRefresh(rate);	
		//EinsteinApplication app = (EinsteinApplication)getApplication();
		//app.normalPriority();
	}

	@Override
	public void onResume()
	{
		Log.e("XXXX", ">>>>>>>>>> onResume()");
		super.onResume();
		int rate = Integer.valueOf(this.sharedPrefs.getString("screenrefreshrate", "10"));
		startScreenRefresh(rate);
		pEinstein.powerOnEmulator();
	}

	@Override
	public void onNewIntent(Intent intent) 
	{
		if (intent.getBooleanExtra("EXIT", false)) {
			Log.e("XXXX", ">>>>>>>>>> EXIT");
			finish();
	    }
		String file = intent.getStringExtra("FILE"); 
		if (file!=null) {
			pEinstein.installPackage(file);
			intent.removeExtra("FILE");
		}
	}
	
	
	@Override
	public void onPause()
	{
		Log.e("XXXX", ">>>>>>>>>> onPause()");
		//pEinstein.powerOffEmulator();
		//stopScreenRefresh();	
		super.onPause();
	}

	@Override
	public void onStop()
	{
		Log.e("XXXX", ">>>>>>>>>> onStop()");
		stopScreenRefresh();	
		super.onStop();
		//setThreadPriority(-2); //Process.THREAD_PRIORITY_FOREGROUND);
		//EinsteinApplication app = (EinsteinApplication)getApplication();
		//app.raisePriority();
	}
	
	@Override
	public void onDestroy()
	{
		// December 2011 Frank Gruendel This is because Android Applications do not really stop. They only retreat into the background.
		// To really stop them one has to use the Android Settings Manager.
		Log.e("XXXX", ">>>>>>>>>> onDestroy()");
		//pEinstein.stopEmulator();
		pEinstein.powerOffEmulator();
		stopScreenRefresh();	
		
	    //Intent intent = new Intent(EinsteinService.class.getName());
	    //stopService(intent);
	    
		super.onDestroy();
	}

	// --- End of application life cycle
	
	
	/** 
	 * Updates the fullscreen status. The app is shown fullscreen if <code>statusBarVisible</code> is <code>false</code>.
	 * Note that this method must be called before invoking <code>setContentView</code> in the <code>onCreate</code> method. 
	 */
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
	 *  even be called if the actual value of the preference has not changed. 
	 */
	@Override
	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
		// The keys are defined in preferences.xml
		if ("keepnetworkcardpluggedin".equals(key)) {
			//DebugUtils.debugTextOnScreen(this, "card setting changed ");
		} else if ("screenpresets".equals(key) /* || "newtonid".equals(key) */) {
			//DebugUtils.debugTextOnScreen(this, "Screen resolution changed");
			stopScreenRefresh();
			// Send the emulator to sleep so that everything ist saved
			if (pEinstein.isPowerOn()!=0) {
				pEinstein.sendPowerSwitchEvent();;
				while (pEinstein.isPowerOn()!=0) {
					try {
						Thread.sleep(100);
					} catch (InterruptedException e) {
					}
					Log.i("SCREEN", "Wait for power down");
				}
			}
			ScreenDimensionsInitializer.initNewtonScreenDimensions(this);
			pEinstein.changeScreenSize(ScreenDimensions.NEWTON_SCREEN_WIDTH, ScreenDimensions.NEWTON_SCREEN_HEIGHT);
			pEinsteinView.updateDimensions();
			Toast.makeText(getApplicationContext(), "Rebooting NewtonOS", Toast.LENGTH_LONG).show();		
			pEinstein.rebootEmulator();
			pEinstein.powerOnEmulator();
			while (pEinstein.isPowerOn()==0) {
				try {
					Thread.sleep(100);
				} catch (InterruptedException e) {
				}
				Log.i("SCREEN", "Wait for power up");
			}
			int rate = Integer.valueOf(this.sharedPrefs.getString("screenrefreshrate", "10"));
			startScreenRefresh(rate);
			// TODO: reconnect the view somehow!
		} else if ("install_rom".equals(key)) {
			//final String installationType = sharedPreferences.getString("install_rom", "1");
			//DebugUtils.debugTextOnScreen(this, "ROM installation path changed to ".concat(installationType));
		} else if ("androidstatusbar".equals(key)) {
			final boolean statusBarVisible = sharedPreferences.getBoolean("androidstatusbar", true);
			//DebugUtils.debugTextOnScreen(this, "Status bar was turned ".concat(statusBarVisible ? "on" : "off"));
			this.updateFullscreenStatus(statusBarVisible);
		} else if ("screenrefreshrate".equals(key)) {
			int rate = Integer.valueOf(this.sharedPrefs.getString("screenrefreshrate", "10"));
			changeScreenRefresh(rate);
		}
	}

	@Override
	/** Used to create the menu. */
	public boolean onCreateOptionsMenu(Menu menu) {
		return true;
	}

	/*
	 * Keep the "Back" key from destroying this activity!
	 * @see android.app.Activity#onBackPressed()
	 */
	/*
	@Override
	public void onBackPressed() {
	   Log.d("XXXX", "onBackPressed Called");
	   Intent setIntent = new Intent(Intent.ACTION_MAIN);
	   setIntent.addCategory(Intent.CATEGORY_HOME);
	   setIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
	   startActivity(setIntent);
	   // or: onKeyDown(KeyEvent.KEYCODE_HOME); 
	}
	*/

	@Override
	protected Dialog onCreateDialog(int id) {
		switch (id) {
		case DIALOG_DOWNLOAD_PROGRESS_ID:
			this.mProgressDialog = new ProgressDialog(this);
			this.mProgressDialog.setMessage(StringUtils.getLocalizedString(this.getResources(), R.string.Startup_downloadingFile));
			this.mProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
			this.mProgressDialog.setCancelable(false);
			this.mProgressDialog.show();
			return this.mProgressDialog;		
		default:
			return null;
		}
	}

	private void startScreenRefresh(int rate) {
		if (mScreenRefreshTask==null) {
			mScreenRefreshTask = new ScreenRefresh(pEinstein, pEinsteinView);
		}
		if (mScreenRefreshTimer==null) {
			mScreenRefreshTimer = new Timer(true);
			Log.e("REFRESH", "rate is set to "+rate);
			mScreenRefreshTimer.schedule(mScreenRefreshTask, 1000, 1000/rate);
		}
	}
	
	private void changeScreenRefresh(int rate) {
		if (mScreenRefreshTimer!=null) {
			mScreenRefreshTimer.cancel();
			mScreenRefreshTimer.purge();
			mScreenRefreshTimer = null;
			mScreenRefreshTimer = new Timer(true);
			Log.e("REFRESH", "rate is changed to "+rate);
			mScreenRefreshTimer.schedule(mScreenRefreshTask, 1000/rate, 1000/rate);
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

//@SuppressWarnings("deprecation")
//	private String setText(){
//		final AlertDialog.Builder alert = new AlertDialog.Builder(this);
//		alert.setTitle(StringConstants.Einstein_002); // Download the ROM
//		alert.setMessage(StringConstants.Einstein_003); // Enter the URL of the Newton ROM file
//		final EditText input = new EditText(this);
//		alert.setView(input);
//		final String out = null;
//		alert.setPositiveButton(StringConstants.Einstein_004, new DialogInterface.OnClickListener() {
//			public void onClick(DialogInterface dialog, int which) {
//				Editable ed = input.getText();
//				String out = ed.toString();
//				Log.i("Einstein", out);
//			}
//		});
//		alert.show();
//		return out;
//	}

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
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
	    if ( keyCode == KeyEvent.KEYCODE_MENU ) {
		    Intent intent = new Intent(this, ActionsActivity.class);  
		    startActivity(intent);
	        return true;
	    }
	    return super.onKeyDown(keyCode, event);
	}
}
