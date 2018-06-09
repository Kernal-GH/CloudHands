package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.utils.TextUtils;

/**
 * Created by dell on 2018/4/16.
 */
public class PacketRecordReader {

    private final PacketRecord packetRecord;
    private final String fname;
    private final String key;
    private final int projID;

    static {
        System.loadLibrary("CloudHandsJNI");
    };

    public PacketRecordReader(String fname, String key, int projID, long bsize){

        packetRecord = new PacketRecord(bsize);
        this.fname = fname;
        this.key = key;
        this.projID  = projID;

    }

    public int open(){

        if(!TextUtils.isEmpty(fname))
            return openMMap(fname);

        return openSHM(key,projID);
    }

    public native int openMMap(String fname);
    public native int openSHM(String key,int projID);

    public PacketRecord read(){

        int ret = read(packetRecord);

        if(ret == 0)
            return packetRecord;

        return null;
    }

    public native int read(PacketRecord packetRecord);

    public native void close();
}
