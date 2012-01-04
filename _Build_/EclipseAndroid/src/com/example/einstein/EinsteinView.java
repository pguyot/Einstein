package com.example.einstein;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Point;
import android.graphics.Rect;
//import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

class EinsteinView extends View {
    private Bitmap mBitmap;
    private Point newtonScreenSize;
    private Rect newtonScreenBounds;

    public EinsteinView(Context context, Point newtonScreenSize) {
        super(context);
        this.mBitmap = Bitmap.createBitmap(newtonScreenSize.x, newtonScreenSize.y, Bitmap.Config.RGB_565);
        this.newtonScreenSize = newtonScreenSize;
        this.newtonScreenBounds = new Rect(0, 0, this.newtonScreenSize.x, this.newtonScreenSize.y);
    }

    // This function will be called by Android whenever we need to refresh the screen, or whenever
    // the TimerTask "ScreenRefresh" tells Android to post a refresh request.
    @Override
    protected void onDraw(Canvas canvas) {
    	// Copy the NewtonOS screen content from the emulated RAM into mBitmap.
    	// Both have the same resolution, but different color formats.
    	// NewtonOS uses grey nibbles (4 bit per color).
    	// We requested RGB_565 on the Android side (16 bit per color).
        renderEinsteinView(mBitmap);
        // Create a Rect class.
        final Rect dstRect = new Rect();
        // And request the Android Display size minus some Android components like
        // the Android status bar, if it is visible.
        super.getDrawingRect(dstRect);
        // The Rect newtonScreenBounds has its origin at 0/0, and is exactly as big as the 
        // NewtonOS screen (or mBitmap for that matter, so it could actually be null).
        // Canvas.drawBitmap copies and scales all pixels form mBitmap onto the Canvas,
        // which in our case should be most or all of the Android screen.
        // The last argument of the call can hold a Paint class which can add more details
        // on how the bitmap is copied (clipped, scaled, antialiased, etc.)
        // At its simplest, this operation should be performed by the Android video card
        // without the help of the CPU.
		canvas.drawBitmap(mBitmap, this.newtonScreenBounds, dstRect, null);
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent ev) {
    	// http://developer.android.com/reference/android/view/MotionEvent.html
    	switch (ev.getAction()) {
    	case MotionEvent.ACTION_DOWN:
    	case MotionEvent.ACTION_MOVE:
            Rect dstRect = new Rect();
            getDrawingRect(dstRect);
    		penDown((int)(ev.getX()*this.newtonScreenSize.x/dstRect.width()), (int)(ev.getY()*this.newtonScreenSize.y/dstRect.height()));
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
