// TODO FG Review
package com.newtonforever.einstein;

import java.util.TimerTask;

class ScreenRefresh extends TimerTask {

	private final Einstein einstein;
	private final EinsteinView einsteinView;
	int cc = 0;

	public ScreenRefresh(Einstein e, EinsteinView ev) {
		einstein = e;
		einsteinView = ev;
	}

	@Override
	public void run() {
		// TODO MM Immer wieder bis 10 zu zählen ist keine wirklich reproduzierbare Zeitspanne. Warum hast Du das
		// so gemacht? Vermutlich ist diese Zeit auf jedem Device anders. Besser wäre es, wenn man hier so was machen
		// würde wie Thread.currentThread().sleep(zeitInMillisekunden). Das wäre zumindest halbwegs reproduzierbar.
		cc++;
		if (10 == cc) {
			cc = 0;
			//Log.i("ScreenRefresh", "Tick");
		}
		if (0 != einstein.screenIsDirty()) {
			einsteinView.postInvalidate();
			//Log.i("ScreenRefresh", "Drawing");
		}
	}
}
