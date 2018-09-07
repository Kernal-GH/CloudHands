package com.antell.cloudhands.api.WBList;

public class IPPortRangeValue {

    private long id;
    private long ipStart;
    private long ipEnd;
    private long netmask;
    private int portStart;
    private int portEnd;

    public IPPortRangeValue(long id,long ipStart,long ipEnd,long netmask,int portStart,int portEnd){

        this.id = id;
        this.ipStart = ipStart;
        this.ipEnd = ipEnd;
        this.netmask = netmask;
        this.portStart = portStart;
        this.portEnd = portEnd;

    }
    public long getIpStart() {
        return ipStart;
    }

    public void setIpStart(long ipStart) {
        this.ipStart = ipStart;
    }

    public long getIpEnd() {
        return ipEnd;
    }

    public void setIpEnd(long ipEnd) {
        this.ipEnd = ipEnd;
    }

    public long getNetmask() {
        return netmask;
    }

    public void setNetmask(long netmask) {
        this.netmask = netmask;
    }

    public int getPortStart() {
        return portStart;
    }

    public void setPortStart(int portStart) {
        this.portStart = portStart;
    }

    public int getPortEnd() {
        return portEnd;
    }

    public void setPortEnd(int portEnd) {
        this.portEnd = portEnd;
    }

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }
}
