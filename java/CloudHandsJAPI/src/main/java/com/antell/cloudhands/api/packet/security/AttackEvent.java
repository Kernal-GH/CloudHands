package com.antell.cloudhands.api.packet.security;

import com.antell.cloudhands.api.packet.tcp.http.HTTPSession;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.DateUtils;
import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Created by dell on 2018/8/1.
 */
public class AttackEvent implements SourceEntry{

    private String parentObjectID;
    private String proto;
    private long startTime;
    private long endTime;
    private String assetIP;
    private String dstIP;
    private String eventType;
    private String engineName;
    private long engineID;
    private long level;

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        TextUtils.addText(sb,"parentObjectID",parentObjectID);
        TextUtils.addText(sb,"proto",proto);
        TextUtils.addLong(sb,"startTime",startTime);
        TextUtils.addLong(sb,"endTime",endTime);
        TextUtils.addText(sb,"timeDate", DateUtils.format(startTime));
        TextUtils.addText(sb,"assetIP",assetIP);
        TextUtils.addText(sb,"dstIP",dstIP);
        TextUtils.addText(sb,"eventType",eventType);
        TextUtils.addText(sb,"engineName",engineName);
        TextUtils.addLong(sb,"engineID",engineID);
        TextUtils.addLong(sb,"level",level);

        return sb.toString();
    }

    @Override
    public String toString(){

        return dataToString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("parentObjectID",parentObjectID);
        cb.field("proto",proto);
        cb.field("startTime",startTime);
        cb.field("endTime",endTime);
        cb.field("timeDate", DateUtils.format(startTime));
        cb.field("assetIP",assetIP);
        cb.field("dstIP",dstIP);
        cb.field("eventType",eventType);
        cb.field("engineName",engineName);
        cb.field("engineID",engineID);
        cb.field("level",level);

        return cb;
    }

    public AttackEvent(HTTPSession httpSession){

        MatchInfo matchInfo = httpSession.getSecMatchResult().getMainMatchInfo();

        setParentObjectID(httpSession.getObjectId());
        setProto("http");
        setStartTime(httpSession.getSessionEntry().getReqStartTime());
        setEndTime(httpSession.getSessionEntry().getResStartTime());

        long srcIP = httpSession.getSessionEntry().getReqIP();
        long dstIP = httpSession.getSessionEntry().getResIP();

        if(IPUtils.isInnerIPBE(dstIP)){
            setAssetIP(IPUtils.ipv4Str(dstIP));
            setDstIP(IPUtils.ipv4Str(srcIP));
        }else{
            setAssetIP(IPUtils.ipv4Str(srcIP));
            setDstIP(IPUtils.ipv4Str(dstIP));
        }

        setEventType(matchInfo.getRuleType());
        setEngineName("WebRuleEngine");
        setEngineID(matchInfo.getRuleID());
        setLevel(matchInfo.getRuleLevel());
    }



    public String getParentObjectID() {
        return parentObjectID;
    }

    public void setParentObjectID(String parentObjectID) {
        this.parentObjectID = parentObjectID;
    }

    public String getProto() {
        return proto;
    }

    public void setProto(String proto) {
        this.proto = proto;
    }

    public long getStartTime() {
        return startTime;
    }

    public void setStartTime(long startTime) {
        this.startTime = startTime;
    }

    public long getEndTime() {
        return endTime;
    }

    public void setEndTime(long endTime) {
        this.endTime = endTime;
    }

    public String getAssetIP() {
        return assetIP;
    }

    public void setAssetIP(String assetIP) {
        this.assetIP = assetIP;
    }

    public String getDstIP() {
        return dstIP;
    }

    public void setDstIP(String dstIP) {
        this.dstIP = dstIP;
    }

    public String getEventType() {
        return eventType;
    }

    public void setEventType(String eventType) {
        this.eventType = eventType;
    }

    public String getEngineName() {
        return engineName;
    }

    public void setEngineName(String engineName) {
        this.engineName = engineName;
    }

    public long getEngineID() {
        return engineID;
    }

    public void setEngineID(long engineID) {
        this.engineID = engineID;
    }

    public long getLevel() {
        return level;
    }

    public void setLevel(long level) {
        this.level = level;
    }


}
