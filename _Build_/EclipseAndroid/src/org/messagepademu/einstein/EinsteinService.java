package org.messagepademu.einstein;

import java.util.Timer;
import java.util.TimerTask;

import org.messagepademu.einstein.actions.ActionsActivity;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.TaskStackBuilder;

/*
 *  if you plan on having a service run in the background, make sure to look at startForeground() 
 *  and make sure to have an ongoing notification or android will kill your service if it 
 *  needs to free memory
 *  
 */


/**
 * A service that keeps the emulator alive in the background.
 * 
 * It is impossible to store the state of the emulator when using native code. Android
 * however loves to destroy activities or apps whenever it needs resources, which
 * causes Einstein to go into a lengthy reboot of the MessagePad at the next start.
 * 
 * This service makes sure that the emulator is not destroyed unless absolutely neccessary.
 * An indicator in task bar shows the user that Einstein is still available.
 * 
 * @todo This service should check if the MessagePad awoke due to an alarm and create
 *       a new Activity in case we are currently in the backgroud.
 *        
 * @todo Choosing "Quit" from the "Extras" [i] menu on the MP should quit the service.
 */
public class EinsteinService extends Service {
	
	public static final int TASK_LAUNCH = 0;
	public static final int TASK_RAISE_PRIORITY = 1;
	public static final int TASK_NORMAL_PRIORITY = 2;
	
	private static final String TAG = EinsteinService.class.getSimpleName();
	
	public class LocalBinder extends Binder {
    }
	
	private final IBinder mBinder = new LocalBinder();
	
	private NotificationManager mNM;	

	private Timer timer;
	
	/*
	 * The timer should check if the task was killed and remove the notification.
	 * final static int	 myPid()
	 * public static final int myTid ()
	 * public static final int getThreadPriority (int tid)
	 * throws IllegalArgumentException if that pid no longer exists
	 */
	public class KeepAliveTask extends TimerTask {
		
		int mTid;
		int mPri;
		
		public KeepAliveTask(int tid) {
			this.mTid = tid;
		}
		
		@Override
		public void run() {
			try {
			// the line below should trigger an exception if the launching process was killed
				this.mPri = android.os.Process.getThreadPriority(mTid);
				//Log.i(TAG, "Service Timer Pri: " + mTid + ":" + mPri);
			} catch(Exception a) {
				((NotificationManager)getSystemService(NOTIFICATION_SERVICE)).cancel(0x4e256cc5);
				Log.e(TAG, "--------- Service Timer: qutting");
				this.cancel();
			}
		}
	}
	
	private KeepAliveTask updateTask = new KeepAliveTask(android.os.Process.myTid());


	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return mBinder;
	}

	@Override
	public void onCreate() {
	    Log.i(TAG, "Einstein Service created.");
	}
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) 
	{
		//Log.e(TAG, "SERVICE: Intent " + intent.getIntExtra("task", -1));
		
		switch (intent.getIntExtra("task", -1)) {
		case TASK_NORMAL_PRIORITY:
			normalPriority();
			break;
		case TASK_RAISE_PRIORITY:
			raisePriority();
			break;
		}
		
	    // We want this service to continue running until it is explicitly
	    // stopped, so return sticky.
	    return START_STICKY;
	}
	
	@Override
	public void onDestroy() {
		normalPriority();
		super.onDestroy();
		Log.i(TAG, "Service destroyed");
/*		
        // Remove the notification.
		mNM.cancel(0x4e256cc5);
        this.stopForeground(true);
        
	    timer.cancel();
	    timer = null;
*/	    
	}

    @SuppressWarnings("deprecation")
	private void showNotification() {
        // In this sample, we'll use the same text for the ticker and the expanded notification
        //CharSequence text = getText(R.string.local_service_started);

        // Set the icon, scrolling text and timestamp
        Notification notification = new Notification(
        		R.drawable.ic_stat_einstein, 
        		"Starting Einstein Service...",
                System.currentTimeMillis());
        //notification.contentView = 0;
        notification.flags = notification.flags | Notification.FLAG_ONGOING_EVENT;
        notification.setLatestEventInfo(getApplicationContext(), "Einstein Service", "Keeps Einstein active at all times.", null);
        
        // The PendingIntent to launch our activity if the user selects this notification
        //PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
        //        new Intent(this, LocalServiceActivities.Controller.class), 0);

        // Set the info for the views that show in the notification panel.
        //notification.setLatestEventInfo(this, getText(R.string.local_service_label),
        //               text, contentIntent);

        // Send the notification.
        //mNM.notify(0x4e256cc5, notification);
        this.startForeground(0x4e256cc5, notification);
    }
    
    @SuppressWarnings("deprecation")
    public void raisePriority()
    {
	    mNM = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
	    
	    NotificationCompat.Builder mBuilder = new NotificationCompat.Builder(this);
	    mBuilder.setSmallIcon(R.drawable.ic_stat_einstein);
	    mBuilder.setContentTitle("Einstein NewtonOS Emulator");
	    mBuilder.setContentText("Options and Settings");
	    
		// Creates an explicit intent for an Activity in your app
		Intent resultIntent = new Intent(this, ActionsActivity.class);
		
		// The stack builder object will contain an artificial back stack for the started Activity.
		// The intention is that leaving the ActionActivity will go to the EinsteinActivity
		TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
		// Adds the back stack for the Intent (but not the Intent itself)
		stackBuilder.addParentStack(EinsteinActivity.class);
		// Adds the Intent that starts the Activity to the top of the stack
		stackBuilder.addNextIntent(resultIntent);
		PendingIntent resultPendingIntent = stackBuilder.getPendingIntent(0, PendingIntent.FLAG_UPDATE_CURRENT);
		mBuilder.setContentIntent(resultPendingIntent);
	    
	    this.startForeground(0x4e256cc5, mBuilder.build());
    }
    
    public void normalPriority()
    {
	    mNM = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
		mNM.cancel(0x4e256cc5);
        this.stopForeground(true);
    }
    
}
