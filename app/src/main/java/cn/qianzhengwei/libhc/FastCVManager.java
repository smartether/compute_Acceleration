package cn.qianzhengwei.libhc;

import android.content.Context;
import android.util.Log;

import java.io.File;
import java.util.ArrayList;

public class FastCVManager{

    private static FastCVManager _instance;
    public static FastCVManager Get(){
        if(_instance == null)
            _instance = new FastCVManager();
        return _instance;
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

    public boolean SupportFastCV(){return supportFastCV;}

    public boolean InitFastCV(String targetPath){
        //String targetPath = ctx.getCacheDir().getAbsolutePath();
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
            java.io.File localFile = new java.io.File(targetPath + "/" + lib);
            if(!file.exists()){
                supportFastCV = false;
            }
            if(file.exists() && (!localFile.exists() || file.length() != localFile.length())) {
                copyFile(file, localFile);
            }
        }
        if(supportFastCV) {
            for (String lib : libToCopy) {
                java.io.File localFile = new java.io.File(targetPath + "/" + lib);
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
        else{
            for (String lib : libToCopy) {
                java.io.File localFile = new java.io.File(targetPath + "/" + lib);
                if(localFile.exists()){
                    localFile.delete();
                }
            }
        }

        if(supportFastCV){
            System.loadLibrary("native-lib");
        }
        else{
            Log.d("native-lib", "$$$will load native-lib-qml");
            try {
                System.loadLibrary("native-lib-qml");
            }
            catch(java.lang.Exception e){
                Log.d("native-lib", "$$" + e.toString());
            }
            Log.d("native-lib", "$$$load native-lib-qml");
        }

        return supportFastCV;
    }

}
