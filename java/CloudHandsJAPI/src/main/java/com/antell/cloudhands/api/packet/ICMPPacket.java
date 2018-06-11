package com.antell.cloudhands.api.packet;

import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.TextUtils;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/4/16.
 */
public class ICMPPacket implements DataReadable {

    private int type;
    private int code;
    private int ttl;
    private int cksum;
    private int ident;
    private int seqNumber;

    private long sip;
    private long tip;

    @Override
    public void read(DataInput input) throws IOException {
        type = input.readUnsignedByte();
        code = input.readUnsignedByte();
        ttl =  input.readUnsignedByte();
        cksum = input.readUnsignedShort();
        ident = input.readUnsignedShort();
        seqNumber = input.readUnsignedShort();
        sip = (long)input.readInt();
        tip = (long)input.readInt();
    }

    @Override
    public void parse(MessageUnpacker unpacker) {

    }

    @Override
    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("Dump ICMP Packet informations:\n");
        TextUtils.addInt(sb,"ICMP Type",type);
        TextUtils.addInt(sb,"ICMP Code",code);
        TextUtils.addInt(sb,"ICMP TTL",ttl);
        TextUtils.addInt(sb,"ICMP CKSum",cksum);
        TextUtils.addInt(sb,"ICMP SeqNumber",seqNumber);
        TextUtils.addText(sb,"ICMP Send IP", IPUtils.ipv4Str(sip));
        TextUtils.addText(sb,"ICMP Target IP", IPUtils.ipv4Str(tip));

        return sb.toString();
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public int getTtl() {
        return ttl;
    }

    public void setTtl(int ttl) {
        this.ttl = ttl;
    }

    public int getCksum() {
        return cksum;
    }

    public void setCksum(int cksum) {
        this.cksum = cksum;
    }

    public int getIdent() {
        return ident;
    }

    public void setIdent(int ident) {
        this.ident = ident;
    }

    public int getSeqNumber() {
        return seqNumber;
    }

    public void setSeqNumber(int seqNumber) {
        this.seqNumber = seqNumber;
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
}
