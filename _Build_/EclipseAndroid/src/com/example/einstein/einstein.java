// ==============================
// File:			einstein.java
// Project:			Einstein
//
// Copyright 2011 by Matthias Melcher (einstein@matthiasm.com)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

package com.example.einstein;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.MotionEvent;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;

class EinsteinView extends View {
    private Bitmap mBitmap;

    public EinsteinView(Context context) {
        super(context);

        final int W = 320;
        final int H = 480;

        mBitmap = Bitmap.createBitmap(W, H, Bitmap.Config.RGB_565);
    }

    @Override protected void onDraw(Canvas canvas) {
        renderEinsteinView(mBitmap);
        Rect srcRect = new Rect(0, 0, 320, 480); 
        Rect dstRect = new Rect(); getDrawingRect(dstRect);
        canvas.drawBitmap(mBitmap, srcRect, dstRect, null);
    }
    
    @Override
    public boolean onTouchEvent (MotionEvent ev) {
    	// http://developer.android.com/reference/android/view/MotionEvent.html
    	switch (ev.getAction()) {
    	case MotionEvent.ACTION_DOWN:
    	case MotionEvent.ACTION_MOVE:
            Rect dstRect = new Rect(); getDrawingRect(dstRect);
    		penDown((int)(ev.getX()*320/dstRect.width()), (int)(ev.getY()*480/dstRect.height()));
    		break;
    	case MotionEvent.ACTION_UP:
    	case MotionEvent.ACTION_CANCEL:
    		penUp();
    		break;
    	}
    	return true;
    }

    // send refresh requests
    private static native int renderEinsteinView(Bitmap bitmap);

	// send touch events
	public native void penDown(int x, int y);
	public native void penUp();   
}

public class einstein extends Activity
{
	public EinsteinView pEinsteinView = null;
	
	public class ScreenRefresh extends TimerTask {

		public EinsteinView pev = null;
		
		public ScreenRefresh(EinsteinView ev) {
			pev = ev;
		}
		
		@Override public void run() {
			if (screenIsDirty()!=0)
				pev.postInvalidate();
		}
	};
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        
        pEinsteinView = new EinsteinView(this);        
        setContentView(pEinsteinView);

        String dataPath = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS) + "/Einstein"; 
        if (!installAssets(dataPath))
        	return;
		initEmulator();
		runEmulator(dataPath);
		
		Timer t = new Timer();
		t.scheduleAtFixedRate(new ScreenRefresh(pEinsteinView), 1000, 200);
    }

    //@Override
    //protected void onStop() {
    //    finish();  // FIXME: we must cancel the main thread
    //}

    // native test method implemented in app/AndroidGlue.c
    public native String  stringFromJNI();

	// initialize the emulator
	public native void initEmulator();
	
	// launch the emulator
	public native void runEmulator(String dataPath);

	// return 1 if the screen want to be re-rendered
	public native int screenIsDirty();

	public boolean installAssets(String dataPath) {
        File dataDir = new File(dataPath);
        dataDir.mkdirs();
        AssetManager assetManager = getAssets();
        
        try {
        	File rom = new File(dataPath + "/717006.rom");
        	if (rom.exists()) {
        		Log.i("Einstein", "ROM file found.");
        	} else {
        		InputStream in = null;
        		try {
        			in = assetManager.open("717006.rom.png");
                } catch (IOException e) {
            		finishWithMessage(
            				"ROM file not found.\n\n" +
            				"Please copy the files\n\n  \"717006.rom\" " +
            				"and \"Einstein.rex\"\n\nto\n\n" +
            				"  \"/Downloads/Einstein\"\n\non your Android device.");
            		return false;
        		}
        		Log.w("Einstein", "Copying ROM file from assets...");
        		FileOutputStream out = new FileOutputStream(dataPath+"/717006.rom");
        		int sz = in.available();
        		byte[] b = new byte[sz];
        		in.read(b);
        		out.write(b);
        		in.close();
        		out.close();
        		Log.w("Einstein", "Done.");
        	}
        	File rex = new File(dataPath + "/Einstein.rex");
        	if (rex.exists()) {
        		Log.i("Einstein", "REX file found.");
        	} else {
        		InputStream in = null;
        		try {
        			in = assetManager.open("Einstein.rex.png");
                } catch (IOException e) {
            		finishWithMessage(
            				"REX file not found.\n\n" +
            				"Please copy the files\n\n  \"717006.rom\" " +
            				"and \"Einstein.rex\"\n\nto\n\n" +
            				"  \"/Downloads/Einstein\"\n\non your Android device.");
            		return false;
        		}
                Log.w("Einstein", "Copying REX file from assets...");
        		FileOutputStream out = new FileOutputStream(dataPath+"/Einstein.rex");
        		int sz = in.available();
        		byte[] b = new byte[sz];
        		in.read(b);
        		out.write(b);
        		in.close();
        		out.close();
        		Log.w("Einstein", "Done.");
        	}
        	File img = new File(dataPath + "/717006.img");
        	if (!img.exists()) {
        		warnUser(
        				"EXPERIMENTAL CODE\n\n" +
        				"This is a very early prerelease of Einstein for Android. First boot may take up to six minutes! " +
        				"Please watch for stray threads eating battery after Einstein quits!");
        	} 
        } catch (IOException e) {
        	Log.e("Einstein", "Installing assets: " + e.getMessage());
        }
        return true;
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
		ad.setCancelable(false); // This blocks the 'BACK' button  
		ad.setMessage(msg);  
		ad.setButton("Quit", new MyOnClickListener(this));  
		ad.show();
	}
	
	void warnUser(String msg) {
		AlertDialog ad = new AlertDialog.Builder(this).create();  
		ad.setCancelable(false); // This blocks the 'BACK' button  
		ad.setMessage(msg);  
		ad.setButton("OK", new DialogInterface.OnClickListener() { 
			@Override public void onClick(DialogInterface dialog, int which) { dialog.dismiss(); } 
			} 
		);  
		ad.show();
	}
	
    // load the entire native program as a library at startup
    static {
        System.loadLibrary("einstein");
    }
}



// ============================================================================== //
// I can't uninstall it, there seems to be some kind of 'Uninstall Shield'.       //
// ============================================================================== //

