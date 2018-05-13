package com.newtonforever.einstein.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.view.MotionEvent;
import android.view.View;

import com.newtonforever.einstein.jni.Native;
import com.newtonforever.einstein.utils.Dimension;
import com.newtonforever.einstein.utils.screen.ScreenDimensions;


/**
 * @author matt
 * @brief A View class that forwards events to native code and displays pixels from the native side.
 * <p>
 * This class implements a view that renders the contents of the emulated
 * Newton screen. The Java side of this class generates a bitmap in the size
 * of the Newton screen. This bitmap is filled with pixel data on the native side
 * whenever Android requires a refresh.
 * <p>
 * Android takes care of scaling the Newton screen to fit the device view.
 * <p>
 * This class also manages touch and pen input and forwards the corresponding
 * events to the native side.
 * @todo In this class, we probably have to handle input from a physical keyboard as well.
 */
public class EinsteinView extends View {
    private Bitmap mBitmap;
    private Dimension emulatorWindowSize;
    private Rect emulatorWindowBounds;
    final Rect dstRect = new Rect();

    public EinsteinView(Context context) {
        super(context);
        updateDimensions();
    }

    @Override
    /** Called by Android whenever we need to refresh the screen, or whenever the TimerTask <code>ScreenRefresh</code>
     *  tells Android to post a refresh request. */
    protected void onDraw(Canvas canvas) {
        // Copy the NewtonOS screen content from the emulated RAM into mBitmap. Both have the same resolution,
        // but different color formats. NewtonOS uses grey nibbles (4 bit per color). We requested RGB_565 on
        // the Android side (16 bit per color).
        Native.renderEinsteinView(mBitmap);
        // Request the Android display size minus some Android components like the Android status bar, if it is visible.
        super.getDrawingRect(dstRect);
        // The Rect emulatorWindowBounds has its origin at 0/0, and is exactly as big as the NewtonOS screen
        // (or mBitmap for that matter, so it could actually be null).
        // Canvas.drawBitmap copies and scales all pixels form mBitmap onto the Canvas, which in our case should
        // be most or all of the Android screen.
        // The last argument of the call can hold a Paint class which can add more details on how the bitmap is
        // copied (clipped, scaled, antialiased, etc.). At its simplest, this operation should be performed by
        // the Android video card without the help of the CPU.
        canvas.drawBitmap(mBitmap, this.emulatorWindowBounds, dstRect, null);
    }

    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        // See http://developer.android.com/reference/android/view/MotionEvent.html
        switch (ev.getAction()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:
                final Rect dstRect = new Rect();
                this.getDrawingRect(dstRect);
                final int x = (int) (ev.getX() * this.emulatorWindowSize.width / dstRect.width());
                final int y = (int) (ev.getY() * this.emulatorWindowSize.height / dstRect.height());
                Native.penDown(x, y);
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                Native.penUp();
                break;
        }
        return true;
    }

    /**
     * Updates the size and location of the emulator window.
     */
    public void updateDimensions() {
        this.mBitmap = null;
        this.emulatorWindowBounds = null;
        this.emulatorWindowSize = ScreenDimensions.NEWTON_SCREEN_SIZE;
        this.mBitmap = Bitmap.createBitmap(emulatorWindowSize.width, emulatorWindowSize.height, Bitmap.Config.RGB_565);
        this.emulatorWindowBounds = new Rect(0, 0, this.emulatorWindowSize.width, this.emulatorWindowSize.height);
    }
}
