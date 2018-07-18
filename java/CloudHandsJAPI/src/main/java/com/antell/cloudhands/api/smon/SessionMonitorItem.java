package com.antell.cloudhands.api.smon;

/**
 * Created by dell on 2018/7/13.
 */
public class SessionMonitorItem {

    private long id;
    private long srcIP;
    private long dstIP;

    private int srcPort;
    private int dstPort;

    private long timeTV;
    private long startTime;
    private long lastTime;

    private int type;
    private int state;

    public SessionMonitorItem(){

        id = -1;

    }

    public String toString(){

        StringBuffer sb = new StringBuffer();


        return sb.toString();
    }

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }

    public long getSrcIP() {
        return srcIP;
    }

    public void setSrcIP(long srcIP) {
        this.srcIP = srcIP;
    }

    public long getDstIP() {
        return dstIP;
    }

    public void setDstIP(long dstIP) {
        this.dstIP = dstIP;
    }

    public int getSrcPort() {
        return srcPort;
    }

    public void setSrcPort(int srcPort) {
        this.srcPort = srcPort;
    }

    public int getDstPort() {
        return dstPort;
    }

    public void setDstPort(int dstPort) {
        this.dstPort = dstPort;
    }

    public long getTimeTV() {
        return timeTV;
    }

    public void setTimeTV(long timeTV) {
        this.timeTV = timeTV;
    }

    public long getStartTime() {
        return startTime;
    }

    public void setStartTime(long startTime) {
        this.startTime = startTime;
    }

    public long getLastTime() {
        return lastTime;
    }

    public void setLastTime(long lastTime) {
        this.lastTime = lastTime;
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public int getState() {
        return state;
    }

    public void setState(int state) {
        this.state = state;
    }
}
