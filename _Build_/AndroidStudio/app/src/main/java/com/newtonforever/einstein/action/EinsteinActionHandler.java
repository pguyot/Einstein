package com.newtonforever.einstein.action;

import com.newtonforever.einstein.jni.Native;
import com.newtonforever.einstein.sound.SoundManager;
import com.newtonforever.einstein.view.EinsteinView;

import java.util.TimerTask;

/**
 * Polls the C side and handles actions that the C side wants us to do. @author Frank Gruendel.
 *
 * Currently our run method is called at every screen refresh request. This must urgently be changed.
 */
// TODO FG 2013_10_19 Put sound issues into a separate Thread. */
public class EinsteinActionHandler extends TimerTask {

    /** If no bits are set, we will do nothing. */
    private final int NO_ACTION_MASK = 0x00;

    /** If bit 0 is set, we'll refresh the Android screen. */
    private final int REFRESH_SCREEN_MASK = 0x01;

    /** If bit 1 is set, we'll start or continue playing sound. */
    private final int PLAY_SOUND_MASK = 0x02;

    /** If bit 2 is set, we will stop playing sound. */
    private final int STOP_SOUND_MASK = 0x04;

    /** If bit 3 is set, we'll change the sound volume. */
    private final int CHANGE_VOLUME_MASK = 0x08;

    /** Our Einstein view. */
    private final EinsteinView einsteinView;

    /** The sound manager that takes care of all sound issues. */
    private final SoundManager soundManager = new SoundManager();

    /** A helper flag for logging a message the first time a screen refresh is requested. */
    private boolean screenRefreshStarted = false;

    public EinsteinActionHandler(EinsteinView view) {
        einsteinView = view;
    }

    @Override
    public void run() {
        final int actionMask = Native.getRequiredActions();
        if (NO_ACTION_MASK == actionMask) {
            return;
        }
        // Note that multiple actions might be required if more than one bit is set in actionMask.
        // So don't even think of using else if here...
        if (isPlaySound(actionMask)) {
            // NewtonOS wants to start a sound or (if a sound is currently played) add more samples to the buffers.
            soundManager.playSound(Native.getSoundBufferSize());
            Native.soundBufferFinishedOrCanceled();
        }
        if (isStopSound(actionMask)) {
            // NewtonOS has stopped the current sound. This must be done before starting a new sound.
            soundManager.stopSound();
        }
        if (isVolumeChanged(actionMask)) {
            // NewtonOS has changed the volume setting. Android must be notified.
            soundManager.changeVolume();
        }
        if (isScreenRefresh(actionMask)) {
            // NewtonOS changed the Newton's screen content. Java must refresh the Android screen.
            handleScreenRefresh();
        }
    }

    /** Returns <code>true</code> if the bit for a screen refresh is set in <code>mask</code> */
    private boolean isScreenRefresh(final int mask) {
        return (mask & REFRESH_SCREEN_MASK) != 0;
    }

    /** Returns <code>true</code> if the bit for stopping the sound is set in <code>mask</code> */
    private boolean isStopSound(final int mask) {
        return (mask & STOP_SOUND_MASK) != 0;
    }

    /** Returns <code>true</code> if the bit for starting or continuing a sound is set in <code>mask</code> */
    private boolean isPlaySound(final int mask) {
        return (mask & PLAY_SOUND_MASK) != 0;
    }

    /** Returns <code>true</code> if the bit for changing the volume is set in <code>mask</code> */
    private boolean isVolumeChanged(final int mask) {
        return (mask & CHANGE_VOLUME_MASK) != 0;
    }

    /** Refreshes the Android screen area inhabited by our Einstein view. */
    private void handleScreenRefresh() {
        if (!screenRefreshStarted) {
            screenRefreshStarted = true;
        }
        einsteinView.postInvalidate();
    }
}
