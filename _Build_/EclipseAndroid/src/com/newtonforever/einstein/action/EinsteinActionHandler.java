// TODO FG 2013-09-16 Most likely it will not be possible in the long run to handle all we need to do here
// using the same poll time. Sound, for example, will most likely have to be polled more often than a screen
// refresh. But for now we'll pretend that this isn't the case so we can get going and Matthias can test
// his sound API.
// TODO FG 2013-09-20 Throw an IllegalStateException if the run method gets an action type we do not know 
package com.newtonforever.einstein.action;

import java.util.TimerTask;

import android.util.Log;

import com.newtonforever.einstein.jni.Native;
import com.newtonforever.einstein.sound.SoundManager;
import com.newtonforever.einstein.view.EinsteinView;

/** Polls the C side and handles actions that the C side wants us to do. @author Frank Gruendel. */
public class EinsteinActionHandler extends TimerTask {

    private final int NO_ACTION_MASK = 0x00;
    private final int REFRESH_SCREEN_MASK = 0x01;
    private final int PLAY_SOUND_MASK = 0x02;
    private final int STOP_SOUND_MASK = 0x04;
    private final int CHANGE_VOLUME_MASK = 0x08;
    
    private final EinsteinView m_einsteinView;
    
    private final SoundManager m_soundManager = new SoundManager();

    public EinsteinActionHandler(EinsteinView view) {
        this.m_einsteinView = view;
    }

    @Override
    public void run() {
    	
    	// this call returns multiple values as bits, packed into an integer variable
        final int actionMask = Native.getRequiredActions();
        
        if (actionMask!=NO_ACTION_MASK) {
        	// Note: one request can trigger multiple actions
            Log.e("EinsteinActionHandler ", "Native.getRequiredActions() requires Android actions");
            
            // NewtonOS changed the screen contents. Java must re-render the Android surface.
            if (0!=(actionMask&REFRESH_SCREEN_MASK)) {
            	Log.e("EinsteinActionHandler ", "  Requesting Screen Refresh");
                this.refreshScreen();
            }
        
            // NewtonOS has canceled the current sound. This must be done before starting a new sound.
            if (0!=(actionMask&STOP_SOUND_MASK)) {
            	Log.e("EinsteinActionHandler ", "  Requesting Sound Output Cancelation");
                this.m_soundManager.stopSound();
            }
        
            // NewtonOS wants to start a sound, or, if a sound is currently played, add more samples to the buffers.
            if (0!=(actionMask&PLAY_SOUND_MASK)) {
            	Log.e("EinsteinActionHandler ", "  Requesting Sound Output");
                this.m_soundManager.playSound();
            }
        
            // NewtonOS has changed the volume setting. Android needs to be notified.
            if (0!=(actionMask&CHANGE_VOLUME_MASK)) {
            	Log.e("EinsteinActionHandler ", "  Requesting new Sound Output volume");
                this.m_soundManager.changeVolume();
            }
            
        }
        
    }

    private void refreshScreen() {
        this.m_einsteinView.postInvalidate();
    }
}
