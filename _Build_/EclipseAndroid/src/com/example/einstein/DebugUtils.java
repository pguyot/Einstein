package com.example.einstein;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.widget.Toast;

import com.example.einstein.startup.StartupConstants;

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

	/** Appends <code>text</code> to the log file. */
	public static void appendLog(String text) {
		final File logFile = getLogFile();
		if (null == logFile) {
			return;
		}
		BufferedWriter bufferedWriter = null;
		try {
			bufferedWriter = new BufferedWriter(new FileWriter(logFile, true));
			bufferedWriter.append(new Date().toGMTString() + ": " + text);
			bufferedWriter.append("\r\n"); // Add line feed. Do not use buf.newLine() since it'll only add 0A, which doesn't help under Windows
			bufferedWriter.close();
		}
		catch (IOException e) {
			e.printStackTrace();
		}
		finally {
			try {
				if (null != bufferedWriter) {
					bufferedWriter.close();
				}
			}
			catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	/** Creates the log file unless it already exists. The log file'noselog.txt will be in the Download\Einstein folder.*/
	private static File getLogFile() {
		final String logFolderName = StartupConstants.DATA_FILE_PATH + File.separator + StartupConstants.LOG_FOLDER;
		final File logFolder = new File(logFolderName);
		if (!logFolder.exists()) {
			final boolean success = logFolder.mkdir();
			if (!success) {
				return null;
			}
		}
		final String logFileName = logFolderName + File.separator + StartupConstants.LOG_FILE_NAME;
		final File logFile = new File(logFileName);
		if (!logFile.exists()) {
			try {
				final boolean success = logFile.createNewFile();
				if (!success) {
					return null;
				}
			}
			catch (IOException e) {
				e.printStackTrace();
				return null;
			}
		}
		return logFile;
	}

}
