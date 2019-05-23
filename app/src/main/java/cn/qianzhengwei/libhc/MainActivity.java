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
    public native void TestQualcommComputeDsp();

    // Used to load the 'native-lib' library on application startup.
    static {
        //System.load("/system/vendor/lib/libcdsprpc.so");
        //System.load("/system/vendor/lib/libfastcvopt.so");

    }

   public ArrayList<String> libToCopy = new ArrayList<String>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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
            if(!file.exists()){
                file = new java.io.File("/system/lib/" + lib);
            }
            java.io.File localFile = new java.io.File(getCacheDir().getAbsolutePath() + "/" + lib);
            if(file.exists() && !localFile.exists()) {
                copyFile(file, localFile);
            }
            if(localFile.exists()){
                System.load(localFile.getAbsolutePath());
            }
        }
/*
        String filePathSystem = "/system/lib/libfastcvopt.so";
        String filePathVendor = "/system/vendor/lib/libfastcvopt.so";
        String localFilePath  = getCacheDir().getAbsolutePath() + "/libfastcvopt.so";

        java.io.File file = new java.io.File(filePathVendor);
        if(!file.exists()){
            file = new java.io.File(filePathSystem);
        }
        java.io.File localFile = new java.io.File(localFilePath);
        copyFile(file, localFile);
        System.load(localFilePath);
*/

        System.loadLibrary("native-lib");

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
//        String res = TestQML();
//        Log.d("native-lib", "onCreate: " + res);
        Thread t = new java.lang.Thread(
                new Runnable() {
                    @Override
                    public void run() {
                        while(true) {
                            TestQualcommComputeDsp();
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

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}

