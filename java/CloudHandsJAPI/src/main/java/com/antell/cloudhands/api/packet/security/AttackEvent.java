package com.antell.cloudhands.api.packet.security;

import com.antell.cloudhands.api.packet.security.clamav.FileContentSecResult;
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
public class AttackEvent implements SourceEntry {

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

        TextUtils.addText(sb, "parentObjectID", parentObjectID);
        TextUtils.addText(sb, "proto", proto);
        TextUtils.addLong(sb, "startTime", startTime);
        TextUtils.addLong(sb, "endTime", endTime);
        TextUtils.addText(sb, "timeDate", DateUtils.format(startTime));
        TextUtils.addText(sb, "assetIP", assetIP);
        TextUtils.addText(sb, "dstIP", dstIP);
        TextUtils.addText(sb, "eventType", eventType);
        TextUtils.addText(sb, "engineName", engineName);
        TextUtils.addLong(sb, "engineID", engineID);
        TextUtils.addLong(sb, "level", level);

        return sb.toString();
    }

    @Override
    public String toString() {

        return dataToString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("parentObjectID", parentObjectID);
        cb.field("proto", proto);
        cb.field("startTime", startTime);
        cb.field("endTime", endTime);
        cb.field("timeDate", DateUtils.format(startTime));
        cb.field("assetIP", assetIP);
        cb.field("dstIP", dstIP);
        cb.field("eventType", eventType);
        cb.field("engineName", engineName);
        cb.field("engineID", engineID);
        cb.field("level", level);

        return cb;
    }

    public AttackEvent(HTTPSession httpSession) {

        this("WebRuleEngine",
                httpSession.getObjectId(),
                httpSession.getSessionEntry().getReqStartTime(),
                httpSession.getSessionEntry().getResLastTime(),
                httpSession.getSessionEntry().getReqIP(),
                httpSession.getSessionEntry().getResIP(),
                "http",
                httpSession.getSecMatchResult().getMainMatchInfo().getRuleType(),
                httpSession.getSecMatchResult().getMainMatchInfo().getRuleLevel());

    }

    public AttackEvent(FileContentSecResult fileSec) {

        this("FileContentCheckEngine",
                fileSec.getObjectId(),
                fileSec.getFileTranSession().getTime(),
                fileSec.getFileTranSession().getTime(),
                IPUtils.ipv4LongBE(fileSec.getFileTranSession().getSrcIP()),
                IPUtils.ipv4LongBE(fileSec.getFileTranSession().getDstIP()),
                fileSec.getFileTranSession().getProto(),
                "FileSec/Malware",1);
    }

    public AttackEvent(SourceEntry sourceEntry,String engineName,String eventType){

        this(
                engineName,
                sourceEntry.getObjectId(),
                sourceEntry.getTime(),
                sourceEntry.getTime(),
                sourceEntry.getSrcIPI(),
                sourceEntry.getDstIPI(),
                sourceEntry.getProto(),
                eventType,
                1);

    }

    public AttackEvent(String engineName,
                       String pid,
                       long startTime,
                       long endTime,
                       long srcIP,
                       long dstIP,
                       String proto,
                       String eventType,
                       long ruleLevel) {

        setParentObjectID(pid);
        setProto(proto);
        setStartTime(startTime);
        setEndTime(endTime);

        String ssrcIP = IPUtils.ipv4Str(srcIP);
        String sdstIP = IPUtils.ipv4Str(dstIP);

        if (IPUtils.isInnerIPBE(dstIP)) {
            setAssetIP(sdstIP);
            setDstIP(ssrcIP);
        } else {
            setAssetIP(ssrcIP);
            setDstIP(sdstIP);
        }

        setEventType(eventType);
        setEngineName(engineName);
        setEngineID(0);
        setLevel(ruleLevel);

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
