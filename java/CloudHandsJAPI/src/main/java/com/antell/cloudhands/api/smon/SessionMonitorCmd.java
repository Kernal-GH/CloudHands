package com.antell.cloudhands.api.smon;

/**
 * Created by dell on 2018/7/13.
 */
public class SessionMonitorCmd {

    private final String mmapFileName;
    private final int contextID;

    static {
        System.loadLibrary("CloudHandsJNI");
    };

    public SessionMonitorCmd(int contextID,String mmapFileName) {
        this.contextID = contextID;
        this.mmapFileName = mmapFileName;
    }

    public void open() throws SessionMonitorOpenException{

        if(open(contextID,mmapFileName)!=0){

            throw new SessionMonitorOpenException("Cannot open Session Monitor,mmapFile:"+mmapFileName);
        }

    }

    public native int open(int contextID,String mmapFileName);

    public void close(){

        close(contextID);
    }

    public native void close(int contextID);

    public long add(long ip,long tv){

        return add(contextID,ip,tv);
    }

    public native long add(int contextID,long ip,long tv);

    public long add(int port,long tv){

        return add(contextID,port,tv);
    }

    public native long add(int contextID,int port,long tv);

    public long add(long ip,int port,long tv){

        return add(contextID,ip,port,tv);
    }

    public native long add(int contextID,long ip,int port,long tv);

    public long add(long srcIP,int srcPort,long dstIP,int dstPort,long tv){

        return add(contextID,srcIP,srcPort,dstIP,dstPort,tv);
    }

    public native long add(int contextID,long srcIP,int srcPort,long dstIP,int dstPort,long tv);

    public SessionMonitorItem find(long id){

        SessionMonitorItem item = new SessionMonitorItem();

        if(find(contextID,item,id)==-1)
            return null;

        return item;
    }

    public native int  find(int contextID,SessionMonitorItem item,long id);

    public SessionMonitorItem find(long srcIP,int srcPort,long dstIP,int dstPort){

        SessionMonitorItem item = new SessionMonitorItem();
        if(find(contextID,item,srcIP,srcPort,dstIP,dstPort) == -1)
            return null;

        return item;
    }

    public native int  find(int contextID,SessionMonitorItem item,long srcIP,int srcPort,long dstIP,int dstPort);

    public void delete(long id){

        delete(contextID,id);
    }

    public native int  delete(int contextID,long id);

    public void stop(long id){

        stop(contextID,id);
    }

    public native void  stop(int contextID,long id);

    public void restart(long id){

        restart(contextID,id);
    }

    public native void  restart(int contextID,long id);

    public int getContextID() {
        return contextID;
    }


}
