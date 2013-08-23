// TODO FG Review

package com.newtonforever.einstein;

import java.util.TimerTask;


class ScreenRefresh extends TimerTask {

	public Einstein pe = null;
	public EinsteinView pev = null;
	
	public ScreenRefresh(Einstein e, EinsteinView ev) {
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
	
	int cc = 0;
}
