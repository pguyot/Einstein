package com.example.einstein;

import java.util.TimerTask;

class ScreenRefresh extends TimerTask {

	public EinsteinView pev = null;
	
	public ScreenRefresh(EinsteinView ev) {
		pev = ev;
	}
	
	@Override
	public void run() {
		if (screenIsDirty()!=0)
			pev.postInvalidate();
	}
	
	// return 1 if the screen wants to be re-rendered
	public native int screenIsDirty();

}
