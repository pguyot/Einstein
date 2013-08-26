package com.newtonforever.einstein;

//TODO FG Review

/** Class interfacing to the native Einstein emulator.
 * @author Matthias Melcher */
public class Einstein
{
	/** Load the entire native program as a library at startup. */
	static {
		System.loadLibrary("einstein");
	}
	
	private boolean pRunning = false;

	public void run(String dataPath, int screenWidth, int screenHeight)
	{
		runEmulator(dataPath, screenWidth, screenHeight);
		pRunning = true;
	}
	
	/** Returns <code>true</code> if the emulator is currently running. */
	public boolean isRunning()
	{
		return pRunning;
	}
	
	/** Initializes the emulator.
	 * 
	 * Special values for logPath are "NULL" or "" for no log, "STDOUT" for logging to some stdout device,
	 * and "CONSOLE" for logging to the Android debugging log (LogCat on Eclipse).
	 *  
	 * @param logPath can be any path name in the Android file sytem
	 */
	public native void initEmulator(String logPath);
	
	/** Launches the emulator. */
	public native void runEmulator(String dataPath, int screenWidth, int screenHeight);
	
	/** Stops the emulator. */
	public native void stopEmulator();
	
	/** Wakes the emulator from sleep. */
	public native void powerOnEmulator();
	
	/** Sends emulator to sleep. */
	public native void powerOffEmulator();

	/** Checks if the power is currently on. */
	public native int isPowerOn();
	
	/** Sends a power switch event. */
	public native void sendPowerSwitchEvent();

	/** Reboots the emulator. */
	public native void rebootEmulator();
	
	/** Changes the screen size (requires reboot). */
	public native void changeScreenSize(int w, int h);
	
	/** Toggles the network card in and out of the PCMCIA slot. */
	public native void toggleNetworkCard();

	/** Checks if the screen contents has changed since the last call
	 * @return 0 if the screen hasn't changed since the last call */
	public native int screenIsDirty();
	
	/** Switches the backlight on or off.
	 * @param v 0 to switch backlight off */
	public native void setBacklight(int v);
	
	/** Checks if backlight is on.
	 * @return 0 if the backlight is currently off */
	public native int backlightIsOn();

	/** Installs packages that are new in the Einstein directory.
	 * @TODO find a better way to do this */
	public native void installNewPackages();

	/** Installs a package from a file. */
	public native void installPackage(String filename);

	/** Sets the Newton ID. */
	public native void setNewtonID(String id);
	
}