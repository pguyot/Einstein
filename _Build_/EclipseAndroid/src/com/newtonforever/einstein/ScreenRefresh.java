// TODO FG Review
package com.newtonforever.einstein;

import java.util.TimerTask;
import android.util.Log;

import com.newtonforever.einstein.jni.Native;

class ScreenRefresh extends TimerTask {

	private final EinsteinView einsteinView;
	int cc = 0;

	public ScreenRefresh(Einstein e, EinsteinView ev) {
		this.einsteinView = ev;
	}

	@Override
	public void run() {
		
		// const jint kRequiredActionsRefreshScreen = 1;
		// const jint kRequiredActionsStartSound = 2;
		// const jint kRequiredActionsStopSound = 4;
		// const jint kRequiredActionsSetVolume = 8;

		
		// TODO MM Immer wieder bis 10 zu zählen ist keine wirklich reproduzierbare Zeitspanne. Warum hast Du das
		// so gemacht? Vermutlich ist diese Zeit auf jedem Device anders. Besser wäre es, wenn man hier so was machen
		// würde wie Thread.currentThread().sleep(zeitInMillisekunden). Das wäre zumindest halbwegs reproduzierbar.
		
		// Matt: the counter is used to divide the number of log outputs by ten. This keeps the
		//       log from getting flooded, but still allows for some verification that the app is polling.
		cc++;
		if (10 == cc) {
			cc = 0;
			//Log.i("ScreenRefresh", "Tick");
		}
		int requiredActions = Native.getRequiredActions();
		if ( (requiredActions & 1) != 0 /* screen refresh */ ) {
			einsteinView.postInvalidate();
			//Log.i("ScreenRefresh", "Drawing");
		}
		if ( (requiredActions & 2) != 0 /* start sound */ ) {
			Log.i("SoundRefresh", "Start Sound");
		}
	}
}
