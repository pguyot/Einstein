package org.messagepademu.einstein;

import android.app.Application;
import android.content.ComponentName;
import android.content.Intent;
import android.util.Log;

/*
 * EinsteinApplication manages Einstein (the native interface to the emulator) and EinsteinService, 
 * a notification manager that keeps the emulator alive when there is no more activity. 
 * EinsteinService also give access to the ActionsActivity, a rplacement for the now vanished
 * hardware context menu button.
 * 
 * EinsteinActivity is the main activity that should always be the lowest on the stack.
 * 
 * The EinsteinActivity can open the ActionActivity by either pressing the menu button, 
 * by tapping the Service notification, or by tapping an item in the NewtonOS 
 * Extras Folder menu (todo).
 * 
 * ActionActivity renders as a popup window and provides quick access to a variety of actions
 * that otherwise would be done by handling the MessagePad hardware. The current list includes
 *   - toggle backlight (on the message pad, this is done by holding the power button for a second)
 *   - insert network card (plugging a PCMCIA card into one of the ports)
 *   - install packages (this action has no good interface yet)
 *   
 * There are a number of administrative actions as well:
 *   - quit the emulator (this merely quits the service and puts the main activity in the background. quitting the emulator itself is left to Android)
 *   - back to the emulator (may be useful if opened through the notification, but it would be better to just cancel the activity to go back!)
 *   - call the preferences activity (this is another big dialog that we provide)
 *   * we may need to offer various kinds of resets and reboots, inserting a Flash card, connecting a serial cord, etc... .
 *   
 *  EinsteinPreferencesAction provides some settings that are useful (or not so useful) to remeber between 
 *  launches, most prominently the screen resolution and orientation, visibility of the Android status bar, 
 *  and the permanent availability of the network card. Most of these settings will require a reboot of the 
 *  emulator, directly offering that to the user.
 *  
 *  Further ativities should help the user to install the ROM, Rex, and all available NewtonOS packages.
 *  
 */

/**
 * This class provides resources that must be available for the lifetime of this app.
 * 
 * @author Matthias Melcher
 *
 */
public class EinsteinApplication extends Application {

	private Einstein pEinstein = null;
	
	/**
	 * Get a link to the native emulator interface.
	 * 
	 * @return access to the emulator 
	 */
	public Einstein getEinstein() 
	{
		return pEinstein;
	}
	
	/**
	 * Initialize what we need besides the Activity.
	 * 
	 * Create the Einstein instance that is shared across Activities.
	 * Create a Service that will keep this app in the foreground as 
	 * long as possible to avoid lengty emulator reboots.
	 */
	@Override
	public void onCreate()
	{
		//Log.i("einstein", "--------> App.onCreate()");

	    super.onCreate();

	    // create and load the Emulator
		pEinstein = new Einstein();
		
		// create the keep-alive Service (will be created asynchronously)
	    Intent intent = new Intent(EinsteinService.class.getName());
	    intent.putExtra("task", EinsteinService.TASK_LAUNCH);
	    ComponentName name = startService(intent);
	    if (name==null) {
			Log.i("einstein", "--------< App.onCreate() - CANT LAUNCH SERVICE");
	    }
	    
		//Log.i("einstein", "--------< App.onCreate()");
	}
	
	/**
	 * Use the Service to keep this application around as long as possible.
	 */
	public void raisePriority()
	{
	    Intent intent = new Intent(EinsteinService.class.getName());
	    intent.putExtra("task", EinsteinService.TASK_RAISE_PRIORITY);
	    startService(intent);
	}
	
	/**
	 * Return this app to normal priority by sending the service to the background.
	 */
	public void normalPriority()
	{
	    Intent intent = new Intent(EinsteinService.class.getName());
	    intent.putExtra("task", EinsteinService.TASK_NORMAL_PRIORITY);
	    startService(intent);
	}

}
