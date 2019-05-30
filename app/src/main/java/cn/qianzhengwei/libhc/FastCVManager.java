package cn.qianzhengwei.libhc;

import android.app.Activity;
import android.content.Context;
import android.util.Log;

import java.io.File;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Dictionary;
import java.util.HashMap;
import java.util.Map;

public class FastCVManager{
    private static native void InitLib(boolean supportFastCV);
    private static FastCVManager _instance;
    public static FastCVManager Get(){
        if(_instance == null)
            _instance = new FastCVManager();
        return _instance;
    }

    public static ArrayList<String> libToCopy = new ArrayList<String>();
    public static ArrayList<String> libFastCV = new ArrayList<String>();
    public static Map<String, Boolean> libRequiredOrOption = new HashMap<>();


    private static void copyFile(File from, File to){
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
            Log.w("native-lib", "copyFile exception: " + e.toString() );
        }
        finally {
            if (in != null) {
                try {
                    in.close();
                    out.close();
                    to.setReadable(true);
                }
                catch (java.io.IOException e) {

                }
            }
        }
    }

    static boolean  supportFastCV = true;

    public static boolean SupportFastCV(){return supportFastCV;}

    static boolean inited = false;

    public static boolean InitFastCV(String targetPath){

        if(targetPath == "" || targetPath == null){
            try {
                Class clz = FastCVManager.class.getClassLoader().loadClass("com.unity3d.player.UnityPlayer");
                Field curAct = clz.getField("currentActivity");
                if(curAct != null) {
                    Activity curActivity = (Activity) curAct.get(null);
                    targetPath = curActivity.getCacheDir().getAbsolutePath();
                }
            }catch (ClassNotFoundException e){
                Log.e("FastCVMgr", e.getMessage(), e);
            }
            catch (NoSuchFieldException e){
                Log.e("FastCVMgr", e.getMessage(), e);
            }
            catch (IllegalAccessException e){
                Log.e("FastCVMgr", e.getMessage(), e);
            }
            catch(java.lang.Exception e){
                Log.e("FastCVMgr", e.getMessage(), e);
            }
        }

        Log.i("native-lib", "InitFastCV targetPath: " + targetPath);
        if(inited) return supportFastCV;
        inited = true;
        //String targetPath = ctx.getCacheDir().getAbsolutePath();
        libToCopy.add("libc++.so");
        libToCopy.add("libstdc++.so");
        libToCopy.add("libcutils.so");
        libToCopy.add("libvndksupport.so");
        libToCopy.add("libOpenCL_system.so");
        libToCopy.add("libOpenCL.so");
        libToCopy.add("libcdsprpc.so");
        libToCopy.add("libfastcvopt.so");
        libToCopy.add("libfastcvadsp_stub.so");

        libRequiredOrOption.put("libc++.so", false);
        libRequiredOrOption.put("libstdc++.so", false);
        libRequiredOrOption.put("libcutils.so", false);
        libRequiredOrOption.put("libvndksupport.so", false);
        libRequiredOrOption.put("libOpenCL_system.so", false);
        libRequiredOrOption.put("libOpenCL.so", false);
        libRequiredOrOption.put("libcdsprpc.so", true);
        libRequiredOrOption.put("libfastcvopt.so", true);
        libRequiredOrOption.put("libfastcvadsp_stub.so", true);



        for(String lib : libToCopy){
            java.io.File file = new java.io.File("/system/vendor/lib/" + lib);
            if(!file.exists() || !file.canRead()){
                file = new java.io.File("/system/lib/" + lib);
            }
            java.io.File localFile = new java.io.File(targetPath + "/" + lib);
            if(!file.exists()){
                if(libRequiredOrOption.containsKey(lib) && libRequiredOrOption.get(lib)) {
                    supportFastCV = false;
                }
                Log.w("native-lib", "InitFastCV file " + file.getAbsolutePath() + " not exist." );
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
                        if(libRequiredOrOption.containsKey(lib) && libRequiredOrOption.get(lib)) {
                            supportFastCV = false;
                        }
                        Log.w("native-lib", "InitFastCV exception: " + e.toString() );
                    }
                } else {
                    if(libRequiredOrOption.containsKey(lib) && libRequiredOrOption.get(lib)) {
                        supportFastCV = false;
                    }
                    Log.w("native-lib", "InitFastCV lib " + lib + " not found..."  );
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
                //System.loadLibrary("native-lib-qml");
            }
            catch(java.lang.Exception e){
                Log.d("native-lib", "$$" + e.toString());
            }
            Log.d("native-lib", "$$$load native-lib-qml");
        }

//        InitLib(supportFastCV);
        Log.d("native-lib", "InitFastCV support faseCV: " + supportFastCV);
        return supportFastCV;
    }

}
