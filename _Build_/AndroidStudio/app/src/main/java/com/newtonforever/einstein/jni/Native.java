package com.newtonforever.einstein.jni;

import android.graphics.Bitmap;

/**
 * Container class for all native methods we need in Einstein. @author Frank Gruendel.
 */
public class Native {

    private Native() {
        // No instantiation, please
    }

    /**
     * Initializes the emulator.
     *
     * @param logPath can be any path name in the Android file sytem. Special values for logPath are "NULL"
     *                or "" for no log, "STDOUT" for logging to some stdout device, and "CONSOLE" for logging to the
     *                Android debugging log (LogCat on Eclipse).
     */
    public static native void initEmulator(String logPath);

    /**
     * Launches the emulator.
     */
    public static native void runEmulator(String dataPath, int screenWidth, int screenHeight);

    /**
     * Stops the emulator.
     */
    public static native void stopEmulator();

    /**
     * Wakes the emulator from sleep.
     */
    public static native void powerOnEmulator();

    /**
     * Sends emulator to sleep.
     */
    public static native void powerOffEmulator();

    /**
     * Checks if the power is currently on.
     */
    public static native int isPowerOn();

    /**
     * Sends a power switch event.
     */
    public static native void sendPowerSwitchEvent();

    /**
     * Reboots the emulator.
     */
    public static native void rebootEmulator();

    /**
     * Changes the screen size (requires reboot).
     */
    public static native void changeScreenSize(int w, int h);

    /**
     * Toggles the network card in and out of the PCMCIA slot.
     */
    public static native void toggleNetworkCard();

    /** Checks if the screen contents has changed since the last call
     * @return 0 if the screen hasn't changed since the last call */
    //public static native int screenIsDirty();

    /**
     * Switches the backlight on or off.
     *
     * @param v 0 to switch backlight off
     */
    public static native void setBacklight(int v);

    /**
     * Checks if backlight is on.
     *
     * @return 0 if the backlight is currently off
     */
    public static native int backlightIsOn();

    /**
     * Installs packages that are new in the Einstein directory.
     *
     * @TODO find a better way to do this
     */
    public static native void installNewPackages();

    /**
     * Installs a package from a file.
     */
    public static native void installPackage(String filename);

    /**
     * Sets the Newton ID.
     */
    public static native void setNewtonID(String id);

    /**
     * Sends refresh requests
     */
    public static native int renderEinsteinView(Bitmap bitmap);

    /**
     * Sends a pen down event.
     */
    public static native void penDown(int x, int y);

    /**
     * Sends a pen down event.
     */
    public static native void penUp();

    /**
     * Java requests a bitmap of required actions from C
     */
    public static native int getRequiredActions();

    /**
     * Java requests the number of samples needed to fill the sound buffer.
     */
    public static native int getSoundBufferSize();

    /**
     * Java asks C to fill the sound buffer with samples.
     */
    public static native int fillSoundBuffer(short[] soundBuffer);

    /**
     * Java requests the current sound volume from C. Values range from 0 for off to 100 for full volume.
     */
    public static native int getSoundVolume();

    /**
     * Java tells C that a new sound buffer became available.
     */
    public static native void soundBufferAvailable();

    /**
     * Java tells C that the last sound finished playing, and no more filled buffers are pending.
     */
    public static native void soundBufferFinishedOrCanceled();

}


