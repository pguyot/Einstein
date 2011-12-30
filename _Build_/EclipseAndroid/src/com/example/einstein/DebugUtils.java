package com.example.einstein;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.widget.Toast;

public class DebugUtils {
	
	private DebugUtils() {
		// No instances, please
	}
	
	public static void debugTextOnScreen(einstein es, String text) {
		Toast.makeText(es, text, Toast.LENGTH_LONG).show();
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Log this exception to log file
		}
	}
	
	public static void showInfoDialog(Context context, String text) {
		final AlertDialog dialog = new AlertDialog.Builder(context).create();  
		dialog.setCancelable(false);  
		dialog.setMessage(text);  
		dialog.setButton("OK", new DialogInterface.OnClickListener() { 
			@Override public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
			} 
		});  
		dialog.show();
	}

}
