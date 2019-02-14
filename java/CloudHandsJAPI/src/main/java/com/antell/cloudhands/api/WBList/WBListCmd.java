package com.antell.cloudhands.api.WBList;

import java.util.List;

public class WBListCmd {

    private final String mmapFileName;
    private final int contextID;

    static {
        System.loadLibrary("CloudHandsJNI");
    };

    public WBListCmd(int contextID,String mmapFileName) {
        this.contextID = contextID;
        this.mmapFileName = mmapFileName;
    }

    public void open() throws WBListOpenException {

        if(open(contextID,mmapFileName)!=0){

            throw new WBListOpenException("Cannot open WBList,mmapFile:"+mmapFileName);
        }

    }

    private native int open(int contextID,String mmapFileName);

    public void close(){

        close(contextID);
    }

    private native void close(int contextID);

    public long add(long ipStart,long ipEnd,long netmask,int portStart,int portEnd){

        return add(contextID,ipStart,ipEnd,netmask,portStart,portEnd);
    }

    private native long add(int contextID,long ipStart,long ipEnd,long netmask,int portStart,int portEnd);

    public long add(String value){

        return add(contextID,value);
    }

    private native long add(int contextID,String value);


    public void delete(long id){

        delete(contextID,id);
    }

    private native int  delete(int contextID,long id);

    public List<IPPortRangeValue> getWBIPList(){

        return getWBIPList(contextID);
    }

    private native List<IPPortRangeValue> getWBIPList(int contextID);

    public List<StrValue> getWBStrList(){

        return getWBStrList(contextID);
    }

    private native List<StrValue> getWBStrList(int contextID);

    public void start(){

        start(contextID);
    }

    private native void start(int contextID);

    public void stop(){

        stop(contextID);
    }

    private native void stop(int contextID);
}
