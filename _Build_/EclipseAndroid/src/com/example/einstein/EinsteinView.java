package com.example.einstein;

import com.example.einstein.constants.DimensionConstants;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
//import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

class EinsteinView extends View {
    private Bitmap mBitmap;

    public EinsteinView(Context context) {
        super(context);
        // FIXME: allocate a bitmap that corresponds to the Newton screen size
        mBitmap = Bitmap.createBitmap(DimensionConstants.SCREEN_WIDTH, DimensionConstants.SCREEN_HEIGHT, Bitmap.Config.RGB_565);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        renderEinsteinView(mBitmap);
        final Rect dstRect = new Rect();
        super.getDrawingRect(dstRect);
		//Log.i("ScreenRefresh", "onDraw");
        // FIXME: resize the Newton screen to the Android screen size.
		canvas.drawBitmap(mBitmap, DimensionConstants.SCREEN_BOUNDS, dstRect, null);
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent ev) {
    	// http://developer.android.com/reference/android/view/MotionEvent.html
    	switch (ev.getAction()) {
    	case MotionEvent.ACTION_DOWN:
    	case MotionEvent.ACTION_MOVE:
            Rect dstRect = new Rect();
            getDrawingRect(dstRect);
    		//Log.i("XXXX", "Destination Rect at " + String.valueOf(dstRect.left) + "," + String.valueOf(dstRect.top) 
    		//		+ " - " + String.valueOf(dstRect.width()) + "x" + String.valueOf(dstRect.height()) + " pixels");
            // FIXME: scale peninput  to the Newton screen size
    		penDown((int)(ev.getX()*DimensionConstants.SCREEN_WIDTH/dstRect.width()), (int)(ev.getY()*DimensionConstants.SCREEN_HEIGHT/dstRect.height()));
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
