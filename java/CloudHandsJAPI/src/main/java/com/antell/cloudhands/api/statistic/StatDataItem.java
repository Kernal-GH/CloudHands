package com.antell.cloudhands.api.statistic;

/**
 * Created by dell on 2018/6/8.
 */
public class StatDataItem {

    private long time;
    private long packets;
    private long bytes;

    public long getTime() {
        return time;
    }

    public void setTime(long time) {
        this.time = time;
    }

    public long getPackets() {
        return packets;
    }

    public void setPackets(long packets) {
        this.packets = packets;
    }

    public long getBytes() {
        return bytes;
    }

    public void setBytes(long bytes) {
        this.bytes = bytes;
    }


}
