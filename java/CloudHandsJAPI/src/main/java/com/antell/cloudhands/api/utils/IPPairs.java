package com.antell.cloudhands.api.utils;

/**
 * Created by dell on 2018/6/28.
 */
public class IPPairs {

    private String srcIP;
    private String dstIP;

    public IPPairs(){

        srcIP = null;
        dstIP = null;
    }

    public String getSrcIP() {
        return srcIP;
    }

    public void setSrcIP(String srcIP) {
        this.srcIP = srcIP;
    }

    public String getDstIP() {
        return dstIP;
    }

    public void setDstIP(String dstIP) {
        this.dstIP = dstIP;
    }
}
