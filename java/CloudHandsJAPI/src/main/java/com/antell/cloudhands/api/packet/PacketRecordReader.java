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
    private final int id;

    static {
        System.loadLibrary("CloudHandsJNI");
    };

    public PacketRecordReader(int id,String fname, String key, int projID, long bsize){

        packetRecord = new PacketRecord(bsize);
        this.fname = fname;
        this.key = key;
        this.projID  = projID;

        this.id = id;
    }

    public int open(){

        if(!TextUtils.isEmpty(fname))
            return openMMap(id,fname);

        return openSHM(id,key,projID);
    }

    public native int openMMap(int id,String fname);
    public native int openSHM(int id,String key,int projID);

    public PacketRecord read(){

        int ret = read(id,packetRecord);

        if(ret == 0)
            return packetRecord;

        return null;
    }

    public void close(){

        close(id);

    }

    public native int read(int id,PacketRecord packetRecord);

    public native void close(int id);
}
