package com.antell.cloudhands.api.packet.security.clamav;

/**
 * Created by g on 5/23/17.
 */
public class ClamAVScan {


    public int init(String dbDir){

        return init(dbDir,ClamAVConstants.CL_DB_STDOPT);
    }

    public String scanFile(String fname){

        return scanFile(fname,ClamAVConstants.CL_SCAN_STDOPT);
    }

    public native int init(String dbDir, int loadOpt);

    public native String scanFile(String filename, int scanOpt);

    static {
        System.loadLibrary("CloudHandsJNI");
    }
}
