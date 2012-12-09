package org.messagepademu.einstein;

import java.util.Timer;
import java.util.TimerTask;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

/*
 *  if you plan on having a service run in the background, make sure to look at startForeground() 
 *  and make sure to have an ongoing notification or android will kill your service if it 
 *  needs to free memory
 *  
 */
public class EinsteinService extends Service {
	
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
				Log.i(TAG, "Service Timer Pri: " + mTid + ":" + mPri);
			} catch(Exception a) {
				((NotificationManager)getSystemService(NOTIFICATION_SERVICE)).cancel(0x4e256cc5);
				Log.i(TAG, "Service Timer: qutting");
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
		super.onCreate();
	    Log.i(TAG, "!!!!!! Service created");
	    
	    mNM = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
	    showNotification();
	    
	    timer = new Timer("Service Timer");
	    timer.schedule(updateTask, 1000L, 5 * 1000L);
	}
	
	/*
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		// TODO: does this keep the calling Activity from beeing killed?
	    // We want this service to continue running until it is explicitly
	    // stopped, so return sticky.
	    return START_STICKY;
	}
	*/
	
	@Override
	public void onDestroy() {
		super.onDestroy();
		Log.i(TAG, "Service destroyed");
		
        // Remove the notification.
		mNM.cancel(0x4e256cc5);
        this.stopForeground(true);
        
	    timer.cancel();
	    timer = null;
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
    
}
