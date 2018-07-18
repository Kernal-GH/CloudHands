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

    private native int open(int contextID,String mmapFileName);

    public void close(){

        close(contextID);
    }

    private native void close(int contextID);

    public long add(long ip,long tv){

        return add(contextID,ip,tv);
    }

    private native long add(int contextID,long ip,long tv);

    public long add(int port,long tv){

        return add(contextID,port,tv);
    }

    private native long add(int contextID,int port,long tv);

    public long add(long ip,int port,long tv){

        return add(contextID,ip,port,tv);
    }

    private native long add(int contextID,long ip,int port,long tv);

    public long add(long srcIP,int srcPort,long dstIP,int dstPort,long tv){

        return add(contextID,srcIP,srcPort,dstIP,dstPort,tv);
    }

    private native long add(int contextID,long srcIP,int srcPort,long dstIP,int dstPort,long tv);

    public SessionMonitorItem find(long id){

        SessionMonitorItem item = new SessionMonitorItem();

        if(find(contextID,item,id)==-1)
            return null;

        return item;
    }

    private native int  find(int contextID,SessionMonitorItem item,long id);

    public SessionMonitorItem find(long srcIP,int srcPort,long dstIP,int dstPort){

        SessionMonitorItem item = new SessionMonitorItem();
        if(find(contextID,item,srcIP,srcPort,dstIP,dstPort) == -1)
            return null;

        return item;
    }

    private native int  find(int contextID,SessionMonitorItem item,long srcIP,int srcPort,long dstIP,int dstPort);

    public void delete(long id){

        delete(contextID,id);
    }

    private native int  delete(int contextID,long id);

    public void stop(long id){

        stop(contextID,id);
    }

    private native void  stop(int contextID,long id);

    public void restart(long id){

        restart(contextID,id);
    }

    private native void  restart(int contextID,long id);


    public SessionMonitorItem get(int index){

        SessionMonitorItem item = new SessionMonitorItem();
        if(get(contextID,item,index) == -1)
            return null;

        return item;
    }

    private native int get(int contextID,SessionMonitorItem item,int index);


    public int count(){

        return count(contextID);
    }

    private native int count(int contextID);

    public int getContextID() {
        return contextID;
    }


}
