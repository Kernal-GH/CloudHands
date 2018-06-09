package com.antell.cloudhands.api.packet;
import com.antell.cloudhands.api.Dump;
import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.TextUtils;

/**
 * Created by dell on 2018/4/17.
 */
public class SessionEndPoint implements Dump {

    private long ip;
    private int port;
    private long packets;
    private long bytes;
    private long startTime;
    private long lastTime;
    private ByteData  content;

    public SessionEndPoint(){

        this.ip = 0;
        this.port = 0;
        this.packets = 0;
        this.bytes = 0;
        this.startTime = 0;
        this.lastTime = 0;
        this.content = null;
    }

    public void dump(StringBuffer sb){

        TextUtils.addText(sb,"IP", IPUtils.ipv4Str(ip));
        TextUtils.addInt(sb,"Port",port);
        TextUtils.addLong(sb,"Packets",packets);
        TextUtils.addLong(sb,"Bytes",bytes);
        TextUtils.addLong(sb,"StartTime",startTime);
        TextUtils.addLong(sb,"LastTime",lastTime);
    }

    public void setStartTime(long startTime) {
        this.startTime = startTime;
    }

    public long getStartTime() {
        return startTime;
    }

    public void setLastTime(long lastTime) {
        this.lastTime = lastTime;
    }

    public long getLastTime() {
        return lastTime;
    }

    public void setPackets(long packets) {
        this.packets = packets;
    }

    public long getPackets() {
        return packets;
    }

    public void setBytes(long bytes) {
        this.bytes = bytes;
    }

    public long getBytes() {
        return bytes;
    }

    public long getIp() {
        return ip;
    }
    public void setIp(long ip) {
        this.ip = ip;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public int getPort() {
        return port;
    }

    public ByteData getContent() {
        return content;
    }

    public void setContent(ByteData content) {
        this.content = content;
    }
}
