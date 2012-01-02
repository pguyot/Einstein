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

    @Override
    protected void onDraw(Canvas canvas) {
        renderEinsteinView(mBitmap);
        final Rect dstRect = new Rect();
        super.getDrawingRect(dstRect);
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
