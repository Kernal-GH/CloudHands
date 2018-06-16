package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.BinDataInput;
import com.antell.cloudhands.api.DataDump;
import com.antell.cloudhands.api.DataOutJson;
import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/4/16.
 */
public class ARPPacket implements BinDataInput,DataDump,DataOutJson{

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

    public ARPPacket(){

        sha = new MacAddress();

        tha= new MacAddress();
        srcMac = new MacAddress();
        dstMac = new MacAddress();
    }

    @Override
    public void read(DataInput input) throws IOException {

        hardwareAddrFmt = input.readUnsignedShort();
        protoAddrFmt = input.readUnsignedShort();
        hardwareAddrLen = input.readUnsignedByte();
        protoAddrLen = input.readUnsignedByte();
        opcode = input.readUnsignedShort();
        sip = input.readInt();
        tip = input.readInt();
        sha.read(input);
        tha.read(input);
        srcMac.read(input);
        dstMac.read(input);
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        sb.append("Dump The ARP Packet Informations:\n");
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

        cb.field("hardwareAddrFmt",hardwareAddrFmt);
        cb.field("protoAddrFmt",protoAddrFmt);
        cb.field("hardwareAddrLen",hardwareAddrLen);
        cb.field("protoAddrLen",protoAddrLen);
        cb.field("opcode",opcode);
        cb.field("sendIP", IPUtils.ipv4Str(sip));
        cb.field("targetIP", IPUtils.ipv4Str(tip));

        cb.field("sendMac", sha.toMacStr());
        cb.field("targetMac", tha.toMacStr());
        return cb;
    }

    @Override
    public String toString(){

        return dataToString();
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


}
