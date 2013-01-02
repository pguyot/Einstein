
package org.messagepademu.einstein;

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
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.Toast;

import org.messagepademu.einstein.actions.ActionsActivity;
import org.messagepademu.einstein.constants.OtherConstants;
import org.messagepademu.einstein.constants.StringConstants;
import org.messagepademu.einstein.constants.URLConstants;
import org.messagepademu.einstein.prefs.EinsteinPreferencesActivity;
import org.messagepademu.einstein.startup.IStartup.LoadResult;
import org.messagepademu.einstein.startup.Startup;
import org.messagepademu.einstein.startup.StartupConstants;
import org.messagepademu.einstein.utils.screen.ScreenDimensions;


public class EinsteinActivity extends Activity implements OnSharedPreferenceChangeListener
{
	private static EinsteinActivity pInstance = null;

	public static EinsteinActivity getInstance() {
		return pInstance;
	}
	
	private Einstein pEinstein = null;
	private EinsteinView pEinsteinView = null;
	private ProgressDialog mProgressDialog; 
	private Timer mScreenRefreshTimer = null;
	private ScreenRefresh mScreenRefreshTask = null;
	private SharedPreferences sharedPrefs;
	private SharedPreferences.OnSharedPreferenceChangeListener sharedPrefsListener;

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
			pEinstein.run(StartupConstants.DATA_FILE_PATH, ScreenDimensions.NEWTON_SCREEN_WIDTH, ScreenDimensions.NEWTON_SCREEN_HEIGHT);
			Toast.makeText(getApplicationContext(), "Booting Einstein", Toast.LENGTH_LONG).show();		
		}
		startScreenRefresh();
		
		Log.i("einstein", "------< Activity.onCreate()");

		app.raisePriority();
	}
	
	@Override
	public void onStart()
	{
		Log.e("XXXX", ">>>>>>>>>> onStart()");
		super.onStart();
		startScreenRefresh();	
		//EinsteinApplication app = (EinsteinApplication)getApplication();
		//app.normalPriority();
	}

	@Override
	public void onResume()
	{
		Log.e("XXXX", ">>>>>>>>>> onResume()");
		super.onResume();
		startScreenRefresh();
		pEinstein.powerOnEmulator();
	}

	@Override
	public void onNewIntent(Intent intent) 
	{
		if (intent.getBooleanExtra("EXIT", false)) {
			Log.e("XXXX", ">>>>>>>>>> EXIT");
			finish();
	    }
	}
	
	
	@Override
	public void onPause()
	{
		Log.e("XXXX", ">>>>>>>>>> onPause()");
		pEinstein.powerOffEmulator();
		stopScreenRefresh();	
		super.onPause();
	}

	@Override
	public void onStop()
	{
		Log.e("XXXX", ">>>>>>>>>> onStop()");
		stopScreenRefresh();	
		super.onStop();
		//setThreadPriority(-2); //Process.THREAD_PRIORITY_FOREGROUND);
		EinsteinApplication app = (EinsteinApplication)getApplication();
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
	 *  even be called if the actual value of the preference has not changed. 
	 */
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
				final Intent intent = new Intent(EinsteinActivity.this, EinsteinPreferencesActivity.class);
				startActivity(intent);
				break;
			case R.id.backlight:
				if (pEinstein.backlightIsOn()==1)
					pEinstein.setBacklight(0);
				else
					pEinstein.setBacklight(1);
				break;
			case R.id.installNewPackages:
				pEinstein.installNewPackages();
				break;
			case R.id.networkCard:
				pEinstein.toggleNetworkCard();
				break;
		}
		return true;	}

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
			mScreenRefreshTask = new ScreenRefresh(pEinstein, pEinsteinView);
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
	
	private void startDownload() {
		//new DownloadFileAsync(this).execute(URLConstants.SHIP_IMAGE);
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