// TODO FG Review
package com.newtonforever.einstein;

//TODO FG Review

/**
 * Class interfacing to the native Einstein emulator.
 * 
 * @author matt
 */
public class Einstein
{

	/** 
	 * Load the entire native program as a library at startup.
	 */
	static {
		System.loadLibrary("einstein");
	}
	
	private boolean pRunning = false;
	
	public boolean isRunning()
	{
		return pRunning;
	}
	
	/**
	 * Initialize the emulator.
	 * 
	 * Special values for logPath are "NULL" or "" for no log, "STDOUT" for logging to some stdout device,
	 * and "CONSOLE" for logging to the Android debugging log (LogCat on Eclipse).
	 *  
	 * @param logPath can be any path name in the Android file sytem
	 */
	public native void initEmulator(String logPath);

	
	/**
	 * Launch the emulator.
	 * 
	 * @param dataPath
	 * @param screenWidth
	 * @param screenHeight
	 */
	public native void runEmulator(String dataPath, int screenWidth, int screenHeight);

	public void run(String dataPath, int screenWidth, int screenHeight)
	{
		runEmulator(dataPath, screenWidth, screenHeight);
		pRunning = true;
	}
	
	/**
	 * Stop the emulator.
	 */
	public native void stopEmulator();

	
	/**
	 * Wake emulator from sleep.
	 */
	public native void powerOnEmulator();

	
	/** 
	 * Send emulator to sleep.
	 */
	public native void powerOffEmulator();


	public native int isPowerOn();
	
	public native void sendPowerSwitchEvent();

	/** 
	 * Reboot.
	 */
	public native void rebootEmulator();

	
	/** 
	 * Change Screen Size (requires reboot).
	 */
	public native void changeScreenSize(int w, int h);

	
	/**
	 * Toggle the network card in and out of the PCMCIA slot.
	 */
	public native void toggleNetworkCard();


	/**
	 * Check if the screen contents has changed since the last call
	 * @return 0 if the screen hasn't changed since the last call
	 */
	public native int screenIsDirty();

	
	/** 
	 * Switch backlight on or off.
	 * @param v 0 to switch backlight off
	 */
	public native void setBacklight(int v);

	
	/** 
	 * Check if backlight is on.
	 * @return 0 if the backlight is currently off
	 */
	public native int backlightIsOn();

	/**
	 * Install packages that are new in the Einstein directory
	 * 
	 * @todo find a better way to do this
	 */
	public native void installNewPackages();

	/**
	 * Install a package from a file
	 */
	public native void installPackage(String filename);

	public native void setNewtonID(String id);
	
}