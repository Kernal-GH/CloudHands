package com.antell.cloudhands.api.smon;

/**
 * Created by dell on 2018/7/13.
 */
public class SessionMonitorCmd {

    private final String mmapFileName;


    static {
        System.loadLibrary("CloudHandsJNI");
    };

    public SessionMonitorCmd(String mmapFileName) {
        this.mmapFileName = mmapFileName;
    }

    public void open() throws SessionMonitorOpenException{

        if(open(mmapFileName)!=0){

            throw new SessionMonitorOpenException("Cannot open Session Monitor,mmapFile:"+mmapFileName);
        }

    }

    public native int open(String mmapFileName);

    public native long add(long ip);

    public native long add(int port);

    public native long add(long ip,int port);

    public native long add(long srcIP,int srcPort,long dstIP,int dstPort);

    public native int  find(SessionMonitorItem item,long id);

    public native int  find(SessionMonitorItem item,long srcIP,int srcPort,long dstIP,int dstPort);

    public native int  delete(long id);

    public native int  stop(long id);

    public native int  restart(long id);

}
