
package com.newtonforever.einstein.activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.FragmentActivity;
import android.view.View;

import com.newtonforever.einstein.EinsteinApplication;
import com.newtonforever.einstein.R;
import com.newtonforever.einstein.dialog.URLPickerFragment;
import com.newtonforever.einstein.jni.Native;

public class ActionsActivity extends FragmentActivity {

    private static final String URL_PICKER_FRAGMENT_TAG = "URL_PICKER";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.actions_activity);

        findViewById(R.id.toEmulatorAction)
                .setOnClickListener(this::onClickToEmulator);
        findViewById(R.id.installPackagesAction)
                .setOnClickListener(this::onClickInstallPackages);
        findViewById(R.id.openNewtonSiteAction)
                .setOnClickListener(this::onClickOpenURL);
        findViewById(R.id.insertNetworkCardAction)
                .setOnClickListener(this::onClickInsertNetworkCard);
        findViewById(R.id.backlightAction)
                .setOnClickListener(this::onClickBacklight);
        findViewById(R.id.preferencesAction)
                .setOnClickListener(this::onClickPreferences);
        findViewById(R.id.quitEinsteinAction)
                .setOnClickListener(this::onClickQuit);
    }

    private void backToEinstein() {
        Intent intent = new Intent(this, EinsteinActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP
                | Intent.FLAG_ACTIVITY_NEW_TASK
                | Intent.FLAG_ACTIVITY_TASK_ON_HOME);
        startActivity(intent);
    }

    private void onClickToEmulator(View view) {
        backToEinstein();
    }

    private void onClickInstallPackages(View view) {
        Native.installNewPackages();
    }

    private void onClickOpenURL(View view) {
        final DialogFragment newFragment = new URLPickerFragment();
        newFragment.show(getSupportFragmentManager(),
                URL_PICKER_FRAGMENT_TAG);
    }

    private void onClickInsertNetworkCard(View view) {
        Native.toggleNetworkCard();
        backToEinstein();
    }

    private void onClickBacklight(View view) {
        // FIXME: this does not toggle the light, only switches it on.
        // Somehow the backlight state is not not retained when the activity changes
        boolean on = Native.backlightIsOn() == 1;
        Native.setBacklight(on ? 0 : 1);
        backToEinstein();
    }

    private void onClickPreferences(View view) {
        // FIXME: after preferences are closed, we end up in Actions again.
        // We should probably be in Einstein instead.
        Intent intent = new Intent(this, EinsteinPreferencesActivity.class);
        startActivity(intent);
        finish();
    }

    private void onClickQuit(View view) {
        // FIXME: stop emulator
        EinsteinApplication app = (EinsteinApplication) getApplication();
        Native.powerOffEmulator();
        app.normalPriority();
        Intent intent = new Intent(this, EinsteinActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP
                | Intent.FLAG_ACTIVITY_NEW_TASK
                | Intent.FLAG_ACTIVITY_TASK_ON_HOME);
        intent.putExtra("EXIT", true);
        startActivity(intent);
    }
}
