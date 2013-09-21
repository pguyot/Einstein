// TODO FG Review

package com.newtonforever.einstein.activity;

import com.newtonforever.einstein.R;

import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.content.IntentCompat;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;

import com.newtonforever.einstein.EinsteinApplication;
import com.newtonforever.einstein.dialog.DialogFragmentTags;
import com.newtonforever.einstein.dialog.URLPickerFragment;
import com.newtonforever.einstein.jni.Native;
import com.newtonforever.einstein.utils.debug.DebugUtils;
// Note that the names of the onClick methods must match those defined in actions.xml
public class ActionsActivity extends FragmentActivity {

	@Override
	public void onCreate(Bundle savedInstanceState) {
	   super.onCreate(savedInstanceState);
	   setContentView(R.layout.actions);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
	    if ( keyCode == KeyEvent.KEYCODE_MENU ) {
	    	super.finish();
	        return true;
	    }
	    return super.onKeyDown(keyCode, event);
	}

	public void backToEinstein(View v) {
	    Intent intent = new Intent(v.getContext(), EinsteinActivity.class);
	    // TODO FG Check if it is OK to use the class Intent with flags from the class IntentCompat.
	    // Since the flag wasn't available when the Intent class was written, it is difficult to
	    // believe that this will be able to do anything useful with this flag.
	    intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK | IntentCompat.FLAG_ACTIVITY_TASK_ON_HOME);
	    startActivity(intent);
	}
	
	public void onClickToEmulator(View v) {
		backToEinstein(v);
	}

	public void onClickInstallPackages(View v) {
		DebugUtils.appendLog("ActionsActivity.onClickInstallPackages: Installing new packages");
		Native.installNewPackages();
		backToEinstein(v);
	}
	public void onClickOpenURL(View v) {
		DebugUtils.appendLog("ActionsActivity.onClickOpenURL: Invoking URLPickerFragment");
	    final DialogFragment newFragment = new URLPickerFragment();
	    newFragment.show(getSupportFragmentManager(), DialogFragmentTags.URLPickerFragmentTag);
		DebugUtils.appendLog("ActionsActivity.onClickOpenURL: Leaving method");
	}

	public void onClickInsertNetworkCard(View v) {
		Native.toggleNetworkCard();
		backToEinstein(v);
	}

	public void onClickBacklight(View v) {
		// FIXME: this does not toggle the light, only switches it on. Somehow the backlight state is not not retained when the activity changes
		if (Native.backlightIsOn()==1)
			Native.setBacklight(0);
		else
			Native.setBacklight(1);
		backToEinstein(v);
	}

	public void onClickPreferences(View v) {
		// FIXME: after preferences are closed, we end up in Actions again. We should probably be in Einstein instead.
	    Intent intent = new Intent(v.getContext(), EinsteinPreferencesActivity.class);
	    startActivity(intent);
	    this.finish();
	}

	public void onClickQuit(View v) {
		// FIXME: stop emulator
		Log.e("ACTION", "onClickQuitEinstein");
		EinsteinApplication app = (EinsteinApplication)getApplication();
		Native.powerOffEmulator();
		app.normalPriority();
	    Intent intent = new Intent(v.getContext(), EinsteinActivity.class);
	    intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK | IntentCompat.FLAG_ACTIVITY_TASK_ON_HOME);
	    intent.putExtra("EXIT", true);
	    startActivity(intent);
	    
	    // android.os.Process.killProcess(android.os.Process.myPid());
	    // super.finish();
	}
	
}
