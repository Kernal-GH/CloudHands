package com.antell.cloudhands.api.packet.tcp;

import com.antell.cloudhands.api.source.AbstractSourceEntry;
import com.antell.cloudhands.api.utils.Constants;
import com.antell.cloudhands.api.utils.DateUtils;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Created by dell on 2018/7/20.
 */
public class FileTranSession extends AbstractSourceEntry {

    private String proto;
    private String parentObjectId;

    private long   time;
    private String srcIP;
    private int srcPort;
    private String dstIP;
    private int dstPort;
    private String fname;
    private String extName;
    private long bytes;

    private String contentPath;

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        TextUtils.addText(sb,"proto",proto);
        TextUtils.addText(sb,"parentObjectId",parentObjectId);
        TextUtils.addLong(sb,"time",time);
        TextUtils.addText(sb,"srcIP",srcIP);
        TextUtils.addInt(sb,"srcPort",srcPort);
        TextUtils.addText(sb,"dstIP",dstIP);
        TextUtils.addInt(sb,"dstPort",dstPort);

        TextUtils.addText(sb,"fname",fname);
        TextUtils.addText(sb,"extName",extName);
        TextUtils.addLong(sb,"bytes",bytes);
        TextUtils.addText(sb,"contentPath",contentPath);

        return sb.toString();
    }

    public String toString(){


        return dataToString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("proto",proto);
        cb.field("parentObjectId",parentObjectId);
        cb.field("time",time);
        cb.field("timeDate", DateUtils.format(time));
        cb.field("srcIP",srcIP);
        cb.field("srcPort",srcPort);
        cb.field("dstIP",dstIP);
        cb.field("dstPort",dstPort);

        cb.field("fname",fname);
        cb.field("extName",extName);
        cb.field("bytes",bytes);
        cb.field("contentPath",contentPath);

        return cb;
    }

    @Override
    public String getIndexMapping() {
        String mapping = "{" +
                "\"properties\":{" +
                "\"id\":{\"type\":\"keyword\"}," +
                "\"proto\":{\"type\":\"keyword\"}," +
                "\"parentObjectId\":{\"type\":\"keyword\"}," +
                "\"time\":{\"type\":\"long\"}," +
                "\"timeDate\":{\"type\":\"date\",\"format\":\"yyyy-MM-dd HH:mm:ss\"}," +
                "\"srcIP\":{\"type\":\"keyword\"}," +
                "\"dstIP\":{\"type\":\"keyword\"}," +
                "\"srcPort\":{\"type\":\"integer\"}," +
                "\"dstPort\":{\"type\":\"integer\"}," +
                "\"fname\":{\"type\":\"keyword\"}," +
                "\"extName\":{\"type\":\"keyword\"}," +
                "\"contentPath\":{\"type\":\"keyword\"}," +
                "\"bytes\":{\"type\":\"long\"}," +
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
        return "log_file_trans";
    }

    @Override
    public String getIndexDocType() {
        return Constants.ESLOGDOCTYPE;
    }

    public String getProto() {
        return proto;
    }

    public void setProto(String proto) {
        this.proto = proto;
    }

    public String getParentObjectId() {
        return parentObjectId;
    }

    public void setParentObjectId(String parentObjectId) {
        this.parentObjectId = parentObjectId;
    }

    public long getTime() {
        return time;
    }

    public void setTime(long time) {
        this.time = time;
    }

    public String getSrcIP() {
        return srcIP;
    }

    public void setSrcIP(String srcIP) {
        this.srcIP = srcIP;
    }

    public int getSrcPort() {
        return srcPort;
    }

    public void setSrcPort(int srcPort) {
        this.srcPort = srcPort;
    }

    public String getDstIP() {
        return dstIP;
    }

    public void setDstIP(String dstIP) {
        this.dstIP = dstIP;
    }

    public int getDstPort() {
        return dstPort;
    }

    public void setDstPort(int dstPort) {
        this.dstPort = dstPort;
    }

    public String getFname() {
        return fname;
    }

    public void setFname(String fname) {
        this.fname = fname;
    }

    public String getExtName() {
        return extName;
    }

    public void setExtName(String extName) {
        this.extName = extName;
    }

    public long getBytes() {
        return bytes;
    }

    public void setBytes(long bytes) {
        this.bytes = bytes;
    }

    public String getContentPath() {
        return contentPath;
    }

    public void setContentPath(String contentPath) {
        this.contentPath = contentPath;
    }
}
