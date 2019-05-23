package cn.qianzhengwei.libhc;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    public native String TestQML();
    public native void TestQualcommComputeDsp(int mode);

    // Used to load the 'native-lib' library on application startup.
    static {

    }

    public ArrayList<String> libToCopy = new ArrayList<String>();
    public ArrayList<String> libFastCV = new ArrayList<String>();


    private void copyFile(File from, File to){
        java.io.InputStream in = null;
        java.io.OutputStream out = null;
        try {
            to.createNewFile();
            byte[] buff = new byte[1024];
            in = new java.io.BufferedInputStream(new java.io.FileInputStream(from));
            out = new java.io.BufferedOutputStream(new java.io.FileOutputStream(to));
            int len = 0;
            while((len= in.read(buff,0,1024)) > 0){
                out.write(buff, 0, len);
            }
        }
        catch (java.lang.Exception e) {

        }
        finally {
            if (in != null) {
                try {
                    in.close();
                    out.close();
                }
                catch (java.io.IOException e) {

                }
            }
        }
    }

    boolean supportFastCV = true;
    private void InitFastCV(){
        libToCopy.add("libc++.so");
        libToCopy.add("libcutils.so");
        libToCopy.add("libcdsprpc.so");
        //libToCopy.add("libvndksupport.so");
        //libToCopy.add("libOpenCL_system.so");
        //libToCopy.add("libOpenCL.so");
        libToCopy.add("libfastcvopt.so");
        libToCopy.add("libfastcvadsp_stub.so");

        for(String lib : libToCopy){
            java.io.File file = new java.io.File("/system/vendor/lib/" + lib);
            if(!file.exists() || !file.canRead()){
                file = new java.io.File("/system/lib/" + lib);
            }
            java.io.File localFile = new java.io.File(getCacheDir().getAbsolutePath() + "/" + lib);
            if(!file.exists()){
                supportFastCV = false;
            }
            if(file.exists() && (!localFile.exists() || file.length() != localFile.length())) {
                copyFile(file, localFile);
            }
        }
        if(supportFastCV) {
            for (String lib : libToCopy) {
                java.io.File localFile = new java.io.File(getCacheDir().getAbsolutePath() + "/" + lib);
                if (localFile.exists()) {
                    try {
                        System.load(localFile.getAbsolutePath());
                    } catch (java.lang.Exception e) {
                        supportFastCV = false;
                    }
                } else {
                    supportFastCV = false;
                }
            }
        }

        if(supportFastCV){
            System.loadLibrary("native-lib");
        }


    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        InitFastCV();

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        if(supportFastCV) {
            tv.setText(stringFromJNI());
        }
        else {
            tv.setText("not support fastcv");
        }

        Thread t = new java.lang.Thread(
                new Runnable() {
                    @Override
                    public void run() {
                        while(true) {
                            if(supportFastCV) {
                                TestQualcommComputeDsp(0);
                            }
                            else{
                                //TestQualcommComputeDsp(3);
                                Log.i("native-lib", "$$$$$ this device doesn't support fastcv. ");
                            }
                            try {
                                java.lang.Thread.sleep(100);
                            }catch (java.lang.Exception e){

                            }
                        }
                    }
                }
        );
        t.start();
        
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}

