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
public class ICMPPacket extends AbstractSourceEntry {

    private long time;
    private int type;
    private int code;
    private int ttl;
    private int cksum;
    private int ident;
    private int seqNumber;

    private long sip;
    private long tip;
    private ByteData content;
    private String sessionID;

    public ICMPPacket(DataInput input,long time) throws IOException {

        this.time = time;
        type = input.readUnsignedByte();
        code = input.readUnsignedByte();
        ttl =  input.readUnsignedByte();
        cksum = input.readUnsignedShort();
        ident = input.readUnsignedShort();
        seqNumber = input.readUnsignedShort();
        sip = (long)input.readInt();
        tip = (long)input.readInt();

        content = new ByteData(input);
        sessionID(sip,tip,seqNumber);
    }



    @Override
    public String dataToString() {
        StringBuffer sb = new StringBuffer();
        sb.append("Dump ICMP Packet informations:");
        TextUtils.addLong(sb,"time",time);
        TextUtils.addInt(sb,"type",type);
        TextUtils.addInt(sb,"code",code);
        TextUtils.addInt(sb,"ttl",ttl);
        TextUtils.addInt(sb,"cksum",cksum);
        TextUtils.addInt(sb,"ident",ident);
        TextUtils.addInt(sb,"seqNumber",seqNumber);
        TextUtils.addText(sb,"sendIP", IPUtils.ipv4Str(sip));
        TextUtils.addText(sb,"targetIP", IPUtils.ipv4Str(tip));
        return sb.toString();
    }

    @Override
    public String toString()
    {

        return dataToString();
    }

    private boolean isCommonEchoPacket(){

        if(type!=8&&type!=0)
            return false;

        if(content == null||content.getDataSize()==0)
            return true;

        String text = content.decode();
        if(text.contains("/01234567")||text.contains("abcdefghijklmnopqrstuvw"))
            return true;

        return false;
    }

    private String getIPPairs(){

        StringBuffer sb = new StringBuffer();
        sb.append(sip);
        sb.append("|");
        sb.append(tip);

        return sb.toString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("time",time);
        cb.field("timeDate", DateUtils.format(time));
        cb.field("type",type);
        cb.field("code",code);
        cb.field("ttl",ttl);
        cb.field("cksum",cksum);
        cb.field("ident",ident);
        cb.field("seqNumber",seqNumber);
        cb.field("sendIP", IPUtils.ipv4Str(sip));
        cb.field("targetIP", IPUtils.ipv4Str(tip));
        cb.field("dlen",content!=null?content.getDataSize():0);
        cb.field("data",content!=null?content.getData():"".getBytes());
        cb.field("sessionID",sessionID);
        cb.field("isCommonEchoPacket",isCommonEchoPacket()?1:0);
        cb.field("ippairs",getIPPairs());

        return cb;
    }

    @Override
    public String getIndexMapping() {

        String mapping = "{" +
                "\"properties\":{" +
                "\"id\":{\"type\":\"keyword\"}," +
                "\"time\":{\"type\":\"long\"}," +
                "\"timeDate\":{\"type\":\"date\",\"format\":\"yyyy-MM-dd HH:mm:ss\"}," +
                "\"type\":{\"type\":\"integer\"}," +
                "\"code\":{\"type\":\"integer\"}," +
                "\"ttl\":{\"type\":\"integer\"}," +
                "\"cksum\":{\"type\":\"integer\"}," +
                "\"ident\":{\"type\":\"integer\"}," +
                "\"seqNumber\":{\"type\":\"integer\"}," +
                "\"sendIP\":{\"type\":\"keyword\"}," +
                "\"targetIP\":{\"type\":\"keyword\"}," +
                "\"dlen\":{\"type\":\"long\"}," +
                "\"data\":{\"type\":\"binary\"}," +
                "\"sessionID\":{\"type\":\"keyword\"}," +
                "\"isCommonEchoPacket\":{\"type\":\"integer\"}," +
                "\"ippairs\":{\"type\":\"keyword\"}," +
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
        return "log_stream_packet_icmp";
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


    public ByteData getContent() {
        return content;
    }

    public void setContent(ByteData content) {
        this.content = content;
    }

    public String getSessionID() {
        return sessionID;
    }


    private void sessionID(long sip, long tip, int seqNumber) {

        StringBuffer sb = new StringBuffer();

        if(sip<tip){
            sb.append(sip);
            sb.append("|");
            sb.append(tip);
            sb.append("|");
            sb.append(seqNumber);
        }else{
            sb.append(tip);
            sb.append("|");
            sb.append(sip);
            sb.append("|");
            sb.append(seqNumber);

        }
        this.sessionID = sb.toString();
    }



}
