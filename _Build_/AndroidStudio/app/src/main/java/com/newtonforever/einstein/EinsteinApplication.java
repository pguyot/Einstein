// TODO FG Review

package com.newtonforever.einstein;


import android.app.Application;
import android.content.ComponentName;
import android.content.Intent;
import android.util.Log;

/*
 * EinsteinApplication manages Einstein (the native interface to the emulator) and EinsteinService, 
 * a notification manager that keeps the emulator alive when there is no more activity. 
 * EinsteinService also give access to the ActionsActivity, a replacement for the now vanished
 * hardware context menu button.
 * 
 * EinsteinActivity is the main activity that should always be the lowest on the stack.
 * 
 * The EinsteinActivity can open the ActionActivity by either pressing the menu button, 
 * by tapping the Service notification, or by tapping an item in the NewtonOS 
 * Extras Folder menu (// TODO).
 * 
 * ActionActivity renders as a popup window and provides quick access to a variety of actions
 * that otherwise would be done by handling the MessagePad hardware. The current list includes
 *   - toggle backlight (on the message pad, this is done by holding the power button for a second)
 *   - insert network card (plugging a PCMCIA card into one of the ports)
 *   - install packages (this action has no good interface yet)
 *   
 * There are a number of administrative actions as well:
 *   - quit the emulator (this merely quits the service and puts the main activity in the background. Quitting the emulator itself is left to Android)
 *   - back to the emulator (may be useful if opened through the notification, but it would be better to just cancel the activity to go back!)
 *   - call the preferences activity (this is another big dialog that we provide)
 *   * we may need to offer various kinds of resets and reboots, inserting a Flash card, connecting a serial cord, etc... .
 *   
 *  EinsteinPreferencesAction provides some settings that are useful (or not so useful) to remember between 
 *  launches, most prominently the screen resolution and orientation, visibility of the Android status bar, 
 *  and the permanent availability of the network card. Most of these settings will require a reboot of the 
 *  emulator, directly offering that to the user.
 *  
 *  Further activities should help the user to install the ROM, Rex, and all available NewtonOS packages.
 
 CHANGES (last update Jan 5th 2013):
  
    newest:
    	- fixed text in NewtonID dialog
    	- fixed finish/start activity order in actions menu to correctly launch the preferences on some machines
    
 	05.01.2013:
		- added option to change screen resolution (requires Newton reboot)
		- added option to enable/disable status bar
		- added option to change screen refresh rate (now very fluid at 30fps on fast machines)
		- added option to change NewtonID (if you already have licensed software, you can move your ID with your device)
		- added global event handler to install from file (start file browser and tap on any package (*.pkg))
		- added global event handler to install form web (start web browser, got to inna.org or similar, tap on any package)
		- fixed background light
		
	03.01.2013:
		- replaced deprecated context menu with actions menu that can also be reached via notification
		- fixed packages installer, but this method is clunky
		- fixed "insert network card"
		- added action to quit the service, so Android can quit the app if needed

 TODO (last updated Jan 5th 2013):
		- rebooting the emulated Newton does not work reliably, the pen/touch interface does not reconnect. 
		  Going to the home screen and tapping Einstein again is a temp fix
		- Einstein sometimes hangs after the first welcome sequence. After a reboot, it usually never hangs again
		- Options missing for the various resets and reboots, clear Flash, etc.
		- Network support may actually work, but must be tested (will allow syncing)
		- keep network card connected - option
		- install ROM and REX the same way we install packages
		- sound support
		- interface to Android API
		- automatic screen rotation?
		- keyboard support
		- if network support is not enough, a serial port?
		- more icons in the Android UI
		- Newton alerts and alarms while in the background
		- improve TCP/IP speed, make connection more secure
		- support for DHCP

 */

/**
 * This class provides resources that must be available for the lifetime of this app.
 *
 * @author Matthias Melcher
 */
public class EinsteinApplication extends Application {

    private Einstein pEinstein = null;

    /**
     * Get a link to the native emulator interface.
     *
     * @return access to the emulator
     */
    public Einstein getEinstein() {
        return pEinstein;
    }

    /**
     * Initialize what we need besides the Activity.
     * <p>
     * Create the Einstein instance that is shared across Activities.
     * Create a Service that will keep this app in the foreground as
     * long as possible to avoid lengty emulator reboots.
     */
    @Override
    public void onCreate() {
        //Log.i("einstein", "--------> App.onCreate()");

        super.onCreate();

        // create and load the Emulator
        pEinstein = new Einstein();

        // create the keep-alive Service (will be created asynchronously)
        Intent intent = new Intent(getApplicationContext(), EinsteinService.class);
        intent.putExtra("task", EinsteinService.TASK_LAUNCH);
        ComponentName name = startService(intent);
        if (name == null) {
            Log.i("einstein", "--------< App.onCreate() - CANT LAUNCH SERVICE");
        }
        //Log.i("einstein", "--------< App.onCreate()");
    }

    /**
     * Use the Service to keep this application around as long as possible.
     */
    public void raisePriority() {
        Intent intent = new Intent(getApplicationContext(), EinsteinService.class);
        intent.putExtra("task", EinsteinService.TASK_RAISE_PRIORITY);
        startService(intent);
    }

    /**
     * Return this app to normal priority by sending the service to the background.
     */
    public void normalPriority() {
        Intent intent = new Intent(getApplicationContext(), EinsteinService.class);
        intent.putExtra("task", EinsteinService.TASK_NORMAL_PRIORITY);
        startService(intent);
    }

}
