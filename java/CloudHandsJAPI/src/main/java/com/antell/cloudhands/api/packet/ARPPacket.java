package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.source.AbstractSourceEntry;
import com.antell.cloudhands.api.utils.Constants;
import com.antell.cloudhands.api.utils.DateUtils;
import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/4/16.
 */
public class ARPPacket extends AbstractSourceEntry {

    private long time;
    private int hardwareAddrFmt;
    private int protoAddrFmt;
    private int hardwareAddrLen;
    private int protoAddrLen;
    private int opcode;

    private long sip;
    private long tip;

    private MacAddress sha;
    private MacAddress tha;

    private MacAddress srcMac;
    private MacAddress dstMac;

    private String arpScan;

    public ARPPacket(DataInput input,long time) throws IOException {

        this.time = time;
        hardwareAddrFmt = input.readUnsignedShort();
        protoAddrFmt = input.readUnsignedShort();
        hardwareAddrLen = input.readUnsignedByte();
        protoAddrLen = input.readUnsignedByte();
        opcode = input.readUnsignedShort();
        sip = input.readInt();
        tip = input.readInt();

        sha =  new MacAddress(input);
        tha = new MacAddress(input);

        srcMac = new MacAddress(input);

        dstMac = new MacAddress(input);

        setArpScan();
    }


    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        sb.append("Dump The ARP Packet Informations:");
        TextUtils.addLong(sb,"time",time);
        TextUtils.addInt(sb,"format of hardware address",hardwareAddrFmt);
        TextUtils.addInt(sb,"format of protocol address",protoAddrFmt);
        TextUtils.addInt(sb,"length of hardware address",hardwareAddrLen);
        TextUtils.addInt(sb,"length of protocol address",protoAddrLen);
        TextUtils.addInt(sb,"ARP opcode (command)",opcode);

        TextUtils.addText(sb,"Send IP address", IPUtils.ipv4Str(sip));
        TextUtils.addText(sb,"Target IP address", IPUtils.ipv4Str(tip));

        TextUtils.addText(sb,"Send Mac address", sha.toMacStr());
        TextUtils.addText(sb,"Target Mac address", tha.toMacStr());

        TextUtils.addText(sb,"Src Mac address", srcMac.toMacStr());
        TextUtils.addText(sb,"Dst Mac address", dstMac.toMacStr());
        return sb.toString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("time",time);
        cb.field("timeDate", DateUtils.format(time));
        cb.field("hardwareAddrFmt",hardwareAddrFmt);
        cb.field("protoAddrFmt",protoAddrFmt);
        cb.field("hardwareAddrLen",hardwareAddrLen);
        cb.field("protoAddrLen",protoAddrLen);
        cb.field("opcode",opcode);
        cb.field("sendIP", IPUtils.ipv4Str(sip));
        cb.field("targetIP", IPUtils.ipv4Str(tip));

        cb.field("sendMac", sha.toMacStr());
        cb.field("targetMac", tha.toMacStr());

        cb.field("srcMac", srcMac.toMacStr());
        cb.field("dstMac", dstMac.toMacStr());

        cb.field("arpScan",arpScan);

        return cb;
    }

    @Override
    public String getIndexMapping() {
        String mapping = "{" +
                "\"properties\":{" +
                "\"id\":{\"type\":\"keyword\"}," +
                "\"time\":{\"type\":\"long\"}," +
                "\"timeDate\":{\"type\":\"date\",\"format\":\"yyyy-MM-dd HH:mm:ss\"}," +
                "\"hardwareAddrFmt\":{\"type\":\"integer\"}," +
                "\"protoAddrFmt\":{\"type\":\"integer\"}," +
                "\"hardwareAddrLen\":{\"type\":\"integer\"}," +
                "\"protoAddrLen\":{\"type\":\"integer\"}," +
                "\"opcode\":{\"type\":\"integer\"}," +
                "\"sendIP\":{\"type\":\"keyword\"}," +
                "\"targetIP\":{\"type\":\"keyword\"}," +
                "\"sendMac\":{\"type\":\"keyword\"}," +
                "\"targetMac\":{\"type\":\"keyword\"}," +
                "\"srcMac\":{\"type\":\"keyword\"}," +
                "\"dstMac\":{\"type\":\"keyword\"}," +
                "\"arpScan\":{\"type\":\"keyword\"}," +
                "\"srcIPLocation\":{" +
                "\"properties\":{" +
                "\"location\":{\"type\":\"keyword\"}," +
                "\"country\":{\"type\":\"keyword\"}," +
                "\"city\":{\"type\":\"keyword\"}," +
                "\"longitude\":{\"type\":\"double\"}," +
                "\"latitude\":{\"type\":\"double\"}" +
                "}" +
                "}," +
                "\"dstIPLocation\":{" +
                "\"properties\":{" +
                "\"location\":{\"type\":\"keyword\"}," +
                "\"country\":{\"type\":\"keyword\"}," +
                "\"city\":{\"type\":\"keyword\"}," +
                "\"longitude\":{\"type\":\"double\"}," +
                "\"latitude\":{\"type\":\"double\"}" +
                "}" +
                "}" +
                "}" +
                "}";
        return mapping;
    }

    @Override
    public String getIndexNamePrefix() {
        return "log_stream_packet_arp";
    }

    @Override
    public String getIndexDocType() {
        return Constants.ESLOGDOCTYPE;
    }

    @Override
    public String getSrcIP() {
        return IPUtils.ipv4Str(sip);
    }

    @Override
    public String getDstIP() {
        return IPUtils.ipv4Str(tip);
    }

    @Override
    public String toString(){

        return dataToString();
    }

    public long getTime() {
        return time;
    }

    public void setTime(long time) {
        this.time = time;
    }

    public int getHardwareAddrFmt() {
        return hardwareAddrFmt;
    }

    public void setHardwareAddrFmt(int hardwareAddrFmt) {
        this.hardwareAddrFmt = hardwareAddrFmt;
    }

    public int getProtoAddrFmt() {
        return protoAddrFmt;
    }

    public void setProtoAddrFmt(int protoAddrFmt) {
        this.protoAddrFmt = protoAddrFmt;
    }

    public int getHardwareAddrLen() {
        return hardwareAddrLen;
    }

    public void setHardwareAddrLen(int hardwareAddrLen) {
        this.hardwareAddrLen = hardwareAddrLen;
    }

    public int getProtoAddrLen() {
        return protoAddrLen;
    }

    public void setProtoAddrLen(int protoAddrLen) {
        this.protoAddrLen = protoAddrLen;
    }

    public int getOpcode() {
        return opcode;
    }

    public void setOpcode(int opcode) {
        this.opcode = opcode;
    }

    public long getSip() {
        return sip;
    }

    public void setSip(long sip) {
        this.sip = sip;
    }

    public long getTip() {
        return tip;
    }

    public void setTip(long tip) {
        this.tip = tip;
    }

    public MacAddress getSha() {
        return sha;
    }

    public void setSha(MacAddress sha) {
        this.sha = sha;
    }

    public MacAddress getTha() {
        return tha;
    }

    public void setTha(MacAddress tha) {
        this.tha = tha;
    }

    public MacAddress getDstMac() {
        return dstMac;
    }

    public void setDstMac(MacAddress dstMac) {
        this.dstMac = dstMac;
    }

    public MacAddress getSrcMac() {
        return srcMac;
    }

    public void setSrcMac(MacAddress srcMac) {
        this.srcMac = srcMac;
    }


    public String getArpScan() {
        return arpScan;
    }

    public void setArpScan() {

        StringBuffer sb = new StringBuffer();
        sb.append(sip);
        sb.append("|");
        sb.append(tip);

        this.arpScan = sb.toString();
    }
}
