package com.antell.cloudhands.api.statistic;

import com.antell.cloudhands.api.utils.TextUtils;

/**
 * Created by dell on 2018/6/8.
 */
public class StatDataReader {

    private final StatDataInput statDataInput;
    private final String fname;

    static {
        System.loadLibrary("CloudHandsJNI");
    };

    public StatDataReader(String fname,long bsize){

        statDataInput = new StatDataInput(bsize);
        this.fname = fname;
    }

    public int open(){

        if(!TextUtils.isEmpty(fname))
            return open(fname);

        return -1;
    }

    public native int open(String fname);

    public StatDataInput read(long time, int type){

        int ret = read(statDataInput,time,type);

        if(ret != -1)
            return statDataInput;

        return null;
    }

    public StatDataInput read(long time){

        int ret = read(statDataInput,time);

        if(ret != -1)
            return statDataInput;

        return null;
    }

    private native int read(StatDataInput statDataInput, long time);

    public native int read(StatDataInput statDataInput, long time, int type);

    public native void close();

}
