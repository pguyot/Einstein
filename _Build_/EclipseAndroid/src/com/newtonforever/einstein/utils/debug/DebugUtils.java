package com.newtonforever.einstein.utils.debug;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

import com.newtonforever.einstein.startup.StartupConstants;

/** A small helper class for debugging the Einstein app. @author Frank Gruendel */
public class DebugUtils {
    
    private static final String EINSTEIN_LOG_PREFIX = "--- ";

    private DebugUtils() {
        // No instances, please
    }
    
    public static void logGreen(String tag, String message) {
        Log.i(EINSTEIN_LOG_PREFIX + tag, message);
    }
   
    public static void logRed(String tag, String message) {
        Log.e(EINSTEIN_LOG_PREFIX + tag, message);
    }

    private static File logFile = null;

    /** Show a quick message and pause execution for a short moment. */
    public static void debugTextOnScreen(final Context context, final String text) {
        Toast.makeText(context, text, Toast.LENGTH_LONG).show();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            appendLog(e.getMessage());
        }
    }

    /** Shows a message in a dialog and waits for the dialog to be dismissed. */
    public static void showInfoDialog(final Context context, final String text) {
        final AlertDialog dialog = new AlertDialog.Builder(context).create();  
        dialog.setCancelable(false);  
        dialog.setMessage(text);  
        dialog.setButton("OK", new DialogInterface.OnClickListener() { 
            @Override 
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            } 
        });  
        dialog.show();
    }

    /** Appends <code>text</code> to the end of the log file. */
    public static void appendLog(final String text) {
        if (null == logFile) {
            logFile = getLogFile();
        }
        BufferedWriter bufferedWriter = null;
        try {
            bufferedWriter = new BufferedWriter(new FileWriter(logFile, true));
            bufferedWriter.append(new Date().toGMTString() + ": " + text);
            bufferedWriter.append("\r\n"); // Add line feed. Do not use buf.newLine() since it'll only add hex 0A, which doesn't help under Windows
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

    /** Creates and returns an empty text file for log output. This file can be found in the folder Download\Einstein. */
    private static File getLogFile() {
        if (null != logFile) {
            return logFile;
        }
        String logFolderName = StartupConstants.DATA_FILE_PATH + File.separator + StartupConstants.LOG_FOLDER;
        if (logFolderName.startsWith("/mnt/")) {
        	logFolderName = logFolderName.substring(4);
        }
        final File logFolder = new File(logFolderName);
        if (!logFolder.exists()) {
            createLogFolderPath(logFolderName);
        }
        final String logFileName = logFolderName + File.separator + StartupConstants.LOG_FILE_NAME;
        logFile = new File(logFileName);
        if (logFile.exists()) {
            logFile.delete();
        }
        try {
            logFile.createNewFile();
        }
        catch (IOException e) {
            e.printStackTrace();
            return null;
        }
        return logFile;
    }

    private static void createLogFolderPath(final String path) {
    	File dir = new File(path);
    	dir.mkdirs();
    	if (!dir.isDirectory()) {
    		Log.e("ERROR", "Can't create path");
    	}
    }
    
}
