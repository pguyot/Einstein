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

import android.app.Activity;
import android.widget.TextView;
import android.widget.LinearLayout;
import android.os.Bundle;
import android.view.View;
import android.view.MotionEvent;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;

class EinsteinView extends View {
    private Bitmap mBitmap;

    private static native void renderEinsteinView(Bitmap bitmap);

    public EinsteinView(Context context) {
        super(context);

        final int W = 320;
        final int H = 480;

        mBitmap = Bitmap.createBitmap(W, H, Bitmap.Config.RGB_565);
    }

    @Override protected void onDraw(Canvas canvas) {
        //canvas.drawColor(0xFFCCCCCC);
        renderEinsteinView(mBitmap);
        canvas.drawBitmap(mBitmap, 0, 0, null);
        // force a redraw, with a different time-based pattern.
        invalidate();
    }
}

public class einstein extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        EinsteinView ev = new EinsteinView(this);
        
        TextView  tv = new TextView(this);
        tv.setText( stringFromJNI() );
        
        LinearLayout grp = new LinearLayout(this);
        grp.setOrientation(LinearLayout.VERTICAL);
        grp.addView(tv);
        grp.addView(ev);
        
        setContentView(grp);
		
		initEmulator();
		runEmulator();
    }

    //@Override
    //protected void onStop() {
    //    finish();
    //}
	
    @Override
    public boolean onTouchEvent (MotionEvent ev) {
    	// http://developer.android.com/reference/android/view/MotionEvent.html
     final int historySize = ev.getHistorySize();
     final int pointerCount = ev.getPointerCount();
     for (int h = 0; h < historySize; h++) {
         System.out.printf("At time %d:", ev.getHistoricalEventTime(h));
         for (int p = 0; p < pointerCount; p++) {
             System.out.printf("  pointer %d: (%f,%f)",
                 ev.getPointerId(p), ev.getHistoricalX(p, h), ev.getHistoricalY(p, h));
         }
     }
     System.out.printf("At time %d:", ev.getEventTime());
     for (int p = 0; p < pointerCount; p++) {
         System.out.printf("  pointer %d: (%f,%f)",
             ev.getPointerId(p), ev.getX(p), ev.getY(p));
     }
       
	return false;	
    }

    // native test method implemented in app/AndroidGlue.c
    public native String  stringFromJNI();

	// initialize the emulator
	public native void initEmulator();
	
	// launch the emulator
	public native void runEmulator();
	
    // load the entire native program as a library at startup
    static {
        System.loadLibrary("einstein");
    }
}



// ============================================================================== //
// I can't uninstall it, there seems to be some kind of 'Uninstall Shield'.       //
// ============================================================================== //

