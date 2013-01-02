package org.messagepademu.einstein.actions;

import org.messagepademu.einstein.R;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class LoadPackageActivity extends Activity {

	@Override
	public void onCreate(Bundle savedInstanceState) {
	   super.onCreate(savedInstanceState);
	   setContentView(R.layout.load_package);
	   
	   String filePath = getIntent().getData().getEncodedPath();
	   Log.i("GET FILE", filePath);
	}

}
