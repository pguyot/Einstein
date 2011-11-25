package com.example.einstein;

import java.util.TimerTask;

import android.util.Log;

class ScreenRefresh extends TimerTask {

	public einstein pe = null;
	public EinsteinView pev = null;
	
	public ScreenRefresh(einstein e, EinsteinView ev) {
		pe = e;
		pev = ev;
	}
	
	@Override
	public void run() {
		cc++;
		if (cc==10) {
			cc = 0;
			//Log.i("ScreenRefresh", "Tick");
		}
		if (pe.screenIsDirty()!=0) {
			pev.postInvalidate();
			//Log.i("ScreenRefresh", "Drawing");
		}
		
	}
	
	// return 1 if the screen wants to be re-rendered
	public native int screenIsDirty();

	int cc = 0;
}
