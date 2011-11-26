package com.example.einstein;

import com.example.einstein.constants.DimensionConstants;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

class EinsteinView extends View {
    private Bitmap mBitmap;

    public EinsteinView(Context context) {
        super(context);
        mBitmap = Bitmap.createBitmap(DimensionConstants.SCREEN_WIDTH, DimensionConstants.SCREEN_HEIGHT, Bitmap.Config.RGB_565);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        renderEinsteinView(mBitmap);
        final Rect dstRect = new Rect();
        super.getDrawingRect(dstRect);
		//Log.i("ScreenRefresh", "onDraw");
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
