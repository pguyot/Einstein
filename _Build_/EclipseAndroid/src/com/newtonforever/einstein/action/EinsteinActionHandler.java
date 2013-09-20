// TODO FG 2013-09-16 Most likely it will not be possible in the long run to handle all we need to do here
// using the same poll time. Sound, for example, will most likely have to be polled more often than a screen
// refresh. But for now we'll pretend that this isn't the case so we can get going and Matthias can test
// his sound API.
// TODO FG 2013-09-20 Throw an IllegalStateException if the run method gets an action type we do not know 
package com.newtonforever.einstein.action;

import java.util.TimerTask;

import android.util.Log;

import com.newtonforever.einstein.action.EinsteinActionHelper.ActionType;
import com.newtonforever.einstein.jni.Native;
import com.newtonforever.einstein.sound.SoundManager;
import com.newtonforever.einstein.view.EinsteinView;

/** Polls the C side and handles actions that the C side wants us to do. @author Frank Gruendel. */
public class EinsteinActionHandler extends TimerTask {

    private final EinsteinActionHelper m_einsteinActionHelper = new EinsteinActionHelper();

    private final EinsteinView m_einsteinView;
    
    private final SoundManager m_soundManager = new SoundManager();

    public EinsteinActionHandler(EinsteinView view) {
        this.m_einsteinView = view;
    }

    @Override
    public void run() {
        final int actionMask = Native.getRequiredActions();
        final ActionType actionType = m_einsteinActionHelper.getActionForMask(actionMask);
        if (0 != actionMask) {
            Log.e("EinsteinActionHandler ", "action type is " + actionType.toString());
        }
        switch (actionType) {
            case DO_NOTHING: // Will occur 99% of the time, but according to MM isn't an action.
            default:         // Can't normally happen since we're using a typesafe enum here.
                break;
            case REFRESH_SCREEN:
                this.refreshScreen();
                this.m_soundManager.playSound();
                break;
            case PLAY_SOUND:
                this.m_soundManager.playSound();
                break;
            case STOP_SOUND:
                this.m_soundManager.stopSound();
                break;
            case CHANGE_VOLUME:
                this.m_soundManager.changeVolume();
                break;
        }
    }

    private void refreshScreen() {
        this.m_einsteinView.postInvalidate();
    }
}
