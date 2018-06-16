package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.base.Preconditions;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;

/**
 * Created by dell on 2018/4/17.
 */
public abstract class SessionEntry {

    private boolean isTimeout;
    private int timeoutTV;
    private long sessionID;
    private int protocolID;

    private final SessionEndPoint req;
    private final SessionEndPoint res;


     public SessionEntry() {
        this.req = new SessionEndPoint();
        this.res = new SessionEndPoint();
    }

    public void dump(StringBuffer sb){

        TextUtils.addLong(sb,"SessionID",sessionID);
        TextUtils.addInt(sb,"TimeoutTV",timeoutTV);
        TextUtils.addText(sb,"isTimeout",isTimeout?"YES":"NO");
        sb.append("Dump Req Endpoint Informath:\n");
        req.dump(sb);
        sb.append("Dump Res Endpoint Informath:\n");
        res.dump(sb);
    }



    public long getSessionID() {
        return sessionID;
    }

    public void setSessionID(long sessionID) {
        this.sessionID = sessionID;
    }

    public int getTimeoutTV() {
        return timeoutTV;
    }

    public void setTimeoutTV(int timeoutTV) {
        this.timeoutTV = timeoutTV;
    }

    public boolean isTimeout() {
        return isTimeout;
    }

    public void setTimeout(boolean timeout) {
        isTimeout = timeout;
    }

    public void setReqIP(long ip){

        req.setIp(ip);
    }

    public long getReqIP(){

        return req.getIp();
    }

    public void setReqPort(int port){

        req.setPort(port);
    }

    public int getReqPort(){

        return req.getPort();
    }

    public void setReqContent(ByteData content){

        req.setContent(content);
    }

    public ByteData getReqContent(){

        return req.getContent();
    }

    public void setReqPackets(long packets){

        req.setPackets(packets);
    }

    public long getReqPackets(){

        return req.getPackets();
    }

    public void setReqBytes(long bytes){

        req.setBytes(bytes);
    }

    public long getReqBytes(){

        return req.getBytes();
    }

    public void setReqStartTime(long startTime){

        req.setStartTime(startTime);
    }

    public long getReqStartTime(){
        return req.getStartTime();
    }

    public void setReqLastTime(long lastTime){

        req.setLastTime(lastTime);
    }

    public long getReqLastTime(){

        return req.getLastTime();
    }

    public void setResIP(long ip){

        res.setIp(ip);
    }

    public long getResIP(){

        return res.getIp();
    }

    public void setResPort(int port){

        res.setPort(port);
    }

    public int getResPort(){

        return res.getPort();
    }

    public void setResContent(ByteData content){

        res.setContent(content);
    }

    public ByteData getResContent(){

        return res.getContent();
    }


    public void setResPackets(long packets){

        res.setPackets(packets);
    }
    public long getResPackets(){

        return res.getPackets();
    }

    public void setResBytes(long bytes){

        res.setBytes(bytes);
    }

    public long getResBytes(){

        return res.getBytes();
    }

    public void setResStartTime(long startTime){

        res.setStartTime(startTime);
    }

    public long getResStartTime(){
        return res.getStartTime();
    }

    public void setResLastTime(long lastTime){

        res.setLastTime(lastTime);
    }

    public long getResLastTime(){

        return res.getLastTime();
    }

    public int getProtocolID() {
        return protocolID;
    }

    public void setProtocolID(int protocolID) {
        this.protocolID = protocolID;
    }
}
