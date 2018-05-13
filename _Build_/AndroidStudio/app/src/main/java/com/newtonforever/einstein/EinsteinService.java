// TODO FG Review

package com.newtonforever.einstein;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.TaskStackBuilder;
import android.util.Log;

import com.newtonforever.einstein.activity.ActionsActivity;
import com.newtonforever.einstein.activity.EinsteinActivity;

/**
 * A service that keeps the emulator alive in the background.
 * <p>
 * It is impossible to store the state of the emulator when using native code. Android
 * however loves to destroy activities or apps whenever it needs resources, which
 * causes Einstein to go into a lengthy reboot of the MessagePad at the next start.
 * <p>
 * This service makes sure that the emulator is not destroyed unless absolutely necessary.
 * An indicator in the task bar shows the user that Einstein is still available.
 *
 * @todo This service should check if the MessagePad awoke due to an alarm and create
 * a new Activity in case we are currently in the background.
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

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public void onCreate() {
        Log.i(TAG, "Einstein Service created.");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
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
    }

    public void raisePriority() {
        mNM = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

        NotificationCompat.Builder mBuilder = new NotificationCompat.Builder(this);
// TODO: deprecated	    mBuilder.setSmallIcon(R.drawable.ic_stat_einstein);
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

    public void normalPriority() {
        mNM = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        mNM.cancel(0x4e256cc5);
        this.stopForeground(true);
    }

}
