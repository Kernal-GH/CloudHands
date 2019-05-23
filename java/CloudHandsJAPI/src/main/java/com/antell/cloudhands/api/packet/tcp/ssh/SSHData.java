package com.antell.cloudhands.api.packet.tcp.ssh;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;

public class SSHData {

    private int state;
    private int version;
    private String pversion;
    private long pkts;
    private long bytes;
    private long encPkts;
    private long encBytes;
    private long encMinBytes;
    private long encMaxBytes;

    public SSHData(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==9,"Invalid ssh-data session messagePack:"+n);

        setState(MessagePackUtil.parseByte(unpacker));
        setVersion(MessagePackUtil.parseByte(unpacker));
        setPversion(MessagePackUtil.parseText(unpacker));
        setPkts(MessagePackUtil.parseLong(unpacker));
        setBytes(MessagePackUtil.parseLong(unpacker));
        setEncPkts(MessagePackUtil.parseLong(unpacker));
        setEncBytes(MessagePackUtil.parseLong(unpacker));
        setEncMinBytes(MessagePackUtil.parseLong(unpacker));
        setEncMaxBytes(MessagePackUtil.parseLong(unpacker));

    }

    public void dataToString(StringBuffer sb) {

        TextUtils.addInt(sb,"state",state);
        TextUtils.addInt(sb,"version",version);
        TextUtils.addText(sb,"pversion",pversion);
        TextUtils.addLong(sb,"pkts",pkts);
        TextUtils.addLong(sb,"bytes",bytes);
        TextUtils.addLong(sb,"encPkts",encPkts);
        TextUtils.addLong(sb,"encBytes",encBytes);
        TextUtils.addLong(sb,"encMinBytes",encMinBytes);
        TextUtils.addLong(sb,"encMaxBytes",encMaxBytes);

    }


    public XContentBuilder dataToJson(XContentBuilder cb,String key) throws IOException {

        XContentBuilder cbb = cb.startObject(key);

        cbb.field("state",state);
        cbb.field("version",version);
        cbb.field("pversion",pversion);
        cbb.field("pkts",pkts);
        cbb.field("bytes",bytes);
        cbb.field("encPkts",encPkts);
        cbb.field("encBytes",encBytes);
        cbb.field("encMinBytes",encMinBytes);
        cbb.field("encMaxBytes",encMaxBytes);

        cbb.endObject();

        return cb;
    }

    public int getState() {
        return state;
    }

    public void setState(int state) {
        this.state = state;
    }

    public int getVersion() {
        return version;
    }

    public void setVersion(int version) {
        this.version = version;
    }

    public String getPversion() {
        return pversion;
    }

    public void setPversion(String pversion) {
        this.pversion = pversion;
    }

    public long getPkts() {
        return pkts;
    }

    public void setPkts(long pkts) {
        this.pkts = pkts;
    }

    public long getBytes() {
        return bytes;
    }

    public void setBytes(long bytes) {
        this.bytes = bytes;
    }

    public long getEncPkts() {
        return encPkts;
    }

    public void setEncPkts(long encPkts) {
        this.encPkts = encPkts;
    }

    public long getEncBytes() {
        return encBytes;
    }

    public void setEncBytes(long encBytes) {
        this.encBytes = encBytes;
    }

    public long getEncMinBytes() {
        return encMinBytes;
    }

    public void setEncMinBytes(long encMinBytes) {
        this.encMinBytes = encMinBytes;
    }

    public long getEncMaxBytes() {
        return encMaxBytes;
    }

    public void setEncMaxBytes(long encMaxBytes) {
        this.encMaxBytes = encMaxBytes;
    }
}
