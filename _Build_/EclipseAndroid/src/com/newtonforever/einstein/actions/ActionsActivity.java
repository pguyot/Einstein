// TODO FG Review

package com.newtonforever.einstein.actions;

import org.messagepademu.einstein.R;

import com.newtonforever.einstein.Einstein;
import com.newtonforever.einstein.EinsteinActivity;
import com.newtonforever.einstein.EinsteinApplication;
import com.newtonforever.einstein.prefs.EinsteinPreferencesActivity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.content.IntentCompat;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;

public class ActionsActivity extends Activity {

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
		EinsteinApplication app = (EinsteinApplication)getApplication();
		Einstein einstein = app.getEinstein();
		einstein.installNewPackages();
		backToEinstein(v);
	}

	public void onClickInsertNetworkCard(View v) {
		EinsteinApplication app = (EinsteinApplication)getApplication();
		Einstein einstein = app.getEinstein();
		einstein.toggleNetworkCard();
		backToEinstein(v);
	}

	public void onClickBacklight(View v) {
		// FIXME: this does not toggle the light, only switches it on. Somehow the backlight state is not not retained when the activity changes
		EinsteinApplication app = (EinsteinApplication)getApplication();
		Einstein einstein = app.getEinstein();
		if (einstein.backlightIsOn()==1)
			einstein.setBacklight(0);
		else
			einstein.setBacklight(1);
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
		Einstein einstein = app.getEinstein();
		einstein.powerOffEmulator();
		app.normalPriority();
	    Intent intent = new Intent(v.getContext(), EinsteinActivity.class);
	    intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK | IntentCompat.FLAG_ACTIVITY_TASK_ON_HOME);
	    intent.putExtra("EXIT", true);
	    startActivity(intent);
	    
	    // android.os.Process.killProcess(android.os.Process.myPid());
	    // super.finish();
	}
	
}
