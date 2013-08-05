package org.messagepademu.einstein.actions;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.net.URLConnection;

import org.messagepademu.einstein.EinsteinActivity;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

/**
 * Install packages and other files by slecting them in a file browser or clicking on them in a web browser.
 */
public class LoadPackageActivity extends Activity {

	ProgressDialog mProgressDialog;
	String pFilename;

	class FinishActivity implements DialogInterface.OnClickListener {
		@Override
		public void onClick(DialogInterface dialog, int which) {
			finish();
		}
	}
	
	private class DownloadFile extends AsyncTask<String, Integer, String> {
		
		public Activity mActivity;
		
		public DownloadFile(Activity activity) {
			super();
			mActivity = activity;
		}
		
	    @Override
	    protected String doInBackground(String... sUrls) {
	        try {
	        	String sUrl = sUrls[0];
	            // handle the destination filename	        
	            pFilename = Environment.getExternalStorageDirectory() + "/Download/";
	            File folder = new File(Environment.getExternalStorageDirectory() + "/Download/");
	            folder.mkdirs();
	            pFilename += sUrl.substring( sUrl.lastIndexOf('/')+1);
	            Log.e("DOWNLOAD", "To: " + pFilename);
	            // handle the source URL
	            URL url = new URL(sUrl);
	            // connect and download
	            URLConnection connection = url.openConnection();
	            connection.connect();
	            // this will be useful so that you can show a typical 0-100% progress bar
	            int fileLength = connection.getContentLength();

	            // download the file
	            InputStream input = new BufferedInputStream(url.openStream());
	            OutputStream output = new FileOutputStream(pFilename);

	            byte data[] = new byte[1024];
	            long total = 0;
	            int count;
	            while ((count = input.read(data)) != -1) {
	                total += count;
	                // publishing the progress....
	                publishProgress((int) (total * 100 / fileLength));
	                output.write(data, 0, count);
	            }

	            output.flush();
	            output.close();
	            input.close();
	        } catch (Exception e) {
	        	return "ERROR";
	        }
	        return null;
	    }

	    @Override
	    protected void onPreExecute() {
	        super.onPreExecute();
	        mProgressDialog.show();
	    }

	    @Override
	    protected void onProgressUpdate(Integer... progress) {
	        super.onProgressUpdate(progress);
	        mProgressDialog.setProgress(progress[0]);
	    }
	    
	    @Override
	    protected void onPostExecute(String result) {
	    	mProgressDialog.hide();
	    	if (result==null) {
	    		Uri uri = Uri.parse("file://"+pFilename);
	    		AlertDialog.Builder builder = new AlertDialog.Builder(mActivity);
	    		builder.setTitle("Einstein Package Installer")
		    		.setCancelable(true)
		    		.setMessage("Do you want to install the package " + uri.getLastPathSegment())
		    		.setNegativeButton("Cancel", new FinishActivity())
		    		.setPositiveButton("OK", new InstallFromFile(uri))
		    		.show();
	    	} else {
	        	showError("Download failed");
	    	}
	    }
	}
	
	class InstallFromFile implements DialogInterface.OnClickListener {
		public Uri pUri;
		public InstallFromFile(Uri uri) {
			pUri = uri;
		}
		@Override
		public void onClick(DialogInterface dialog, int which) {
			// call Einstein and install
		    Intent intent = new Intent(getApplicationContext(), EinsteinActivity.class);
		    intent.putExtra("FILE", pUri.getPath());
		    intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_TASK_ON_HOME);
		    startActivity(intent);
			finish();
		}
	}

	class InstallFromWeb implements DialogInterface.OnClickListener {
		public Uri pUri;
		public InstallFromWeb(Uri uri) {
			pUri = uri;
		}
		@Override
		public void onClick(DialogInterface dialog, int which) {
			// call Einstein and install
		    Intent intent = new Intent(getApplicationContext(), EinsteinActivity.class);
		    intent.putExtra("FILE", pUri.getPath());
		    intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_TASK_ON_HOME);
		    startActivity(intent);
			finish();
		}
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
		
		String filePath = getIntent().getData().getEncodedPath();
		Log.i("GET FILE", filePath);
		Log.i("GET FILE", "URI" + getIntent().getData().isRelative() + ":" + getIntent().getData().isAbsolute() + " " + getIntent().getData().toString());
		Log.i("GET FILE", getIntent().getData().getScheme());

		Uri uri = getIntent().getData(); //.normalizeScheme();

		if (uri.isRelative()) {
			// we do not know how to handle a relative path
			showError("Unsupported source:\n" + uri.toString());
		} else {
			String scheme = uri.getScheme();
			if (scheme.equals("http") || scheme.equals("https")) {
				installFromWeb(uri);
			} else if (scheme.equals("file")) {
				installFromFile(uri);
			} else {
				showError("Unsupported source scheme:\n" + uri.toString());
			}
		}
	   
		// setContentView(R.layout.load_package);
	}
	
	void installFromWeb(Uri uri)
	{
		Toast.makeText(getApplicationContext(), 
				"Downloading " + uri.getLastPathSegment() + " from " + uri.getAuthority(), 
				Toast.LENGTH_LONG).show();		

		// instantiate it within the onCreate method
		mProgressDialog = new ProgressDialog(this);
		mProgressDialog.setMessage("Downloading " + uri.getLastPathSegment() + " from " + uri.getAuthority());
		mProgressDialog.setIndeterminate(false);
		mProgressDialog.setMax(100);
		mProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);

		// execute this when the downloader must be fired
		DownloadFile downloadFile = new DownloadFile(this);
		downloadFile.execute(uri.toString());
	}

	void installFromFile(Uri uri)
	{		
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
	    builder.setTitle("Einstein Package Installer")
	    	.setCancelable(true)
	    	.setMessage("Do you want to install the package " + uri.getLastPathSegment())
	    	.setNegativeButton("Cancel", new FinishActivity())
	    	.setPositiveButton("OK", new InstallFromFile(uri))
	    	.show();
	}
	
	void showError(String msg)
	{
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
	    builder.setTitle("Einstein Package Installer")
	    	.setMessage(msg)
	    	.setCancelable(false)
	    	.setPositiveButton("OK",
	    		new DialogInterface.OnClickListener() {
	    			public void onClick(DialogInterface dialog, int id) {
	    				finish();
	    			}
	    		}
	    	)
	    	.show();
	}
	
}
