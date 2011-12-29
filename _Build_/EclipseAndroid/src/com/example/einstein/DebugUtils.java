package com.example.einstein;

import android.widget.Toast;

public class DebugUtils {
	
	private DebugUtils() {
		// No instances, please
	}
	
	public static void debugTextOnScreen(einstein es, String text) {
		Toast.makeText(es, text, Toast.LENGTH_LONG).show();
	}

}
