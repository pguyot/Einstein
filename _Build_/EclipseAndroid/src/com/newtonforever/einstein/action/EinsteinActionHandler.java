package com.newtonforever.einstein.action;

import java.util.TimerTask;

import com.newtonforever.einstein.jni.Native;
import com.newtonforever.einstein.sound.SoundManager;
import com.newtonforever.einstein.utils.debug.DebugUtils;
import com.newtonforever.einstein.view.EinsteinView;

/** Polls the C side and handles actions that the C side wants us to do. @author Frank Gruendel. */
/** Currently our run method is called at every screen refresh request. This must urgently be changed. */
// TODO FG 2013_10_19 Put sound issues into a separate Thread. */
public class EinsteinActionHandler extends TimerTask {

    /** If no bits are set, we will do nothing. */
    private final int NO_ACTION_MASK = 0x00;

    /** If bit 0 is set, we'll do refresh the Android screen. */
    private final int REFRESH_SCREEN_MASK = 0x01;

    /** If bit 1 is set, we'll start or continue playing sound. */
    private final int PLAY_SOUND_MASK = 0x02;

    /** If bit 2 is set, we will stop playing sound. */
    private final int STOP_SOUND_MASK = 0x04;

    /** If bit 3 is set, we'll change the sound volume. */
    private final int CHANGE_VOLUME_MASK = 0x08;

    /** Our Einstein view. */
    private final EinsteinView m_einsteinView;

    /** The sound manager that takes care of all sound issues. */
    private final SoundManager m_soundManager = new SoundManager();

    /** A helper flag for logging a message the first time a screen refresh is requested. */
    private boolean m_screenRefreshStarted = false;

    public EinsteinActionHandler(EinsteinView view) {
        this.m_einsteinView = view;
    }

    @Override
    public void run() {
        final int actionMask = Native.getRequiredActions();
        if (NO_ACTION_MASK == actionMask) {
            return;
        }
        // Note that multiple actions might be required if more than one bit is set in actionMask.
        // So don't even think of using else if here...
        if (this.isScreenRefresh(actionMask)) {
            // NewtonOS changed the Newton's screen content. Java must refresh the Android screen.
            this.handleScreenRefresh();
        }
        if (this.isStopSound(actionMask)) {
            // NewtonOS has stopped the current sound. This must be done before starting a new sound.
            DebugUtils.logGreen("EinsteinActionHandler: ", "Stopping sound output");
            this.m_soundManager.stopSound();
        }
        if (this.isPlaySound(actionMask)) {
            // NewtonOS wants to start a sound or (if a sound is currently played) add more samples to the buffers.
            DebugUtils.logGreen("EinsteinActionHandler: ", "Requesting sound output");
            this.m_soundManager.playSound();
        }
        if (this.isVolumeChanged(actionMask)) {
            // NewtonOS has changed the volume setting. Android must be notified.
            DebugUtils.logGreen("EinsteinActionHandler: ", "Volume changed");
            this.m_soundManager.changeVolume();
        }
    }

    /** Returns <code>true</code> if the bit for a screen refresh is set in <code>mask</code> */
    private boolean isScreenRefresh(final int mask) {
        return 0 != (mask & REFRESH_SCREEN_MASK);
    }

    /** Returns <code>true</code> if the bit for stopping the sound is set in <code>mask</code> */
    private boolean isStopSound(final int mask) {
        return 0 != (mask & STOP_SOUND_MASK);
    }

    /** Returns <code>true</code> if the bit for starting or continuing a sound is set in <code>mask</code> */
    private boolean isPlaySound(final int mask) {
        return 0 != (mask & PLAY_SOUND_MASK);
    }

    /** Returns <code>true</code> if the bit for changing the volume is set in <code>mask</code> */
    private boolean isVolumeChanged(final int mask) {
        return 0 != (mask & CHANGE_VOLUME_MASK);
    }

    /** Refreshes the Android screen area inhabited by our Einstein view. */
    private void handleScreenRefresh() {
        if (!m_screenRefreshStarted)
        {
            DebugUtils.logGreen("EinsteinActionHandler: ", "First time screen refresh");
            m_screenRefreshStarted = true;
        }
        this.m_einsteinView.postInvalidate();
    }
}
