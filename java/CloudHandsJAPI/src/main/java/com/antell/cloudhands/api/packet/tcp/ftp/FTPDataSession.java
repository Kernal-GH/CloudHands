package com.antell.cloudhands.api.packet.tcp.ftp;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.tcp.FileTranSession;
import com.antell.cloudhands.api.packet.tcp.TCPSessionEntry;
import com.antell.cloudhands.api.source.AbstractSourceEntry;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.*;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class FTPDataSession extends AbstractSourceEntry {

    private SessionEntry sessionEntry;

    private String fdir;
    private String fname;
    private String path;

    public FTPDataSession(MessageUnpacker unpacker) throws IOException {

        sessionEntry = new TCPSessionEntry();
        fdir = "";
        fname = "";
        path = "";

        parse(unpacker);

    }

    private void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,false);
        Preconditions.checkArgument(n==2,"Invalid ftp session messagePack:"+n);

        /*parse session entry */
        sessionEntry.parse(unpacker);

        n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==3,"Invalid ftp session messagePack:"+n);

        fdir = MessagePackUtil.parseText(unpacker);
        fname = MessagePackUtil.parseText(unpacker);
        path = MessagePackUtil.parseText(unpacker);
    }

    @Override
    public String dataToString() {

        StringBuffer sb =  new StringBuffer();
        sb.append(sessionEntry.dataToString());
        TextUtils.addText(sb,"fdir",fdir);
        TextUtils.addText(sb,"fname",fname);
        TextUtils.addText(sb,"path",path);

        return sb.toString();
    }

    public String toString(){

        return dataToString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        XContentBuilder seCB = cb.startObject("sessionEntry");
        sessionEntry.dataToJson(seCB);
        seCB.endObject();

        cb.field("objectId",getObjectId());
        cb.field("fdir",TextUtils.getStrValue(fdir));
        cb.field("fname",TextUtils.getStrValue(fname));
        cb.field("path",TextUtils.getStrValue(path));

        return cb;
    }

    @Override
    public String getIndexMapping() {

        String mapping = "{" +
                "\"properties\":{" +
                "\"id\":{\"type\":\"keyword\"}," +
                "\"objectId\":{\"type\":\"keyword\"}," +
                "\"sessionEntry\":{" +
                "\"properties\":{" +
                "\"sessionID\":{\"type\":\"long\"}," +
                "\"protocol\":{\"type\":\"keyword\"}," +
                "\"srcIP\":{\"type\":\"keyword\"}," +
                "\"dstIP\":{\"type\":\"keyword\"}," +
                "\"srcPort\":{\"type\":\"integer\"}," +
                "\"dstPort\":{\"type\":\"integer\"}," +
                "\"reqStartTime\":{\"type\":\"long\"}," +
                "\"resStartTime\":{\"type\":\"long\"}," +
                "\"timeDate\":{\"type\":\"date\",\"format\":\"yyyy-MM-dd HH:mm:ss\"}," +
                "\"reqPackets\":{\"type\":\"long\"}," +
                "\"reqBytes\":{\"type\":\"long\"}," +
                "\"reqPBytes\":{\"type\":\"long\"}," +
                "\"resPackets\":{\"type\":\"long\"}," +
                "\"resBytes\":{\"type\":\"long\"}," +
                "\"resPBytes\":{\"type\":\"long\"}" +
                "}" +
                "}," +
                "\"fdir\":{\"type\":\"keyword\"}," +
                "\"fname\":{\"type\":\"keyword\"}," +
                "\"path\":{\"type\":\"keyword\"}," +
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
        return "log_tcp_session_ftp_data";
    }

    @Override
    public String getIndexDocType() {
        return Constants.ESLOGDOCTYPE;
    }

    @Override
    public String getSrcIP() {
        return IPUtils.ipv4Str(sessionEntry.getReqIP());
    }

    @Override
    public String getDstIP() {
        return IPUtils.ipv4Str(sessionEntry.getResIP());
    }

    private FileTranSession toFileTranSession(){

        FileTranSession fileTranSession = new FileTranSession();

        fileTranSession.setProto("ftp");

        fileTranSession.setParentObjectId(getObjectId());
        fileTranSession.setSrcIP(IPUtils.ipv4Str(sessionEntry.getReqIP()));
        fileTranSession.setSrcPort(sessionEntry.getReqPort());
        fileTranSession.setDstIP(IPUtils.ipv4Str(sessionEntry.getResIP()));
        fileTranSession.setDstPort(sessionEntry.getResPort());
        fileTranSession.setTime(sessionEntry.getReqStartTime());
        fileTranSession.setContentPath(path);

        fileTranSession.setFname(fname);
        fileTranSession.setExtName(Content.getExtName(fname));

        fileTranSession.setBytes(Content.getFsize(path));

        return fileTranSession;
    }

    private boolean canToFile(){
        return (!TextUtils.isEmpty(fname))&&(Content.getFsize(path)>0);
    }

    @Override
    public List<SourceEntry> generate() {

        List<SourceEntry> results = new ArrayList<>();

        if(!canToFile())
            return null;

        results.add(toFileTranSession());

        return results;
    }

    public SessionEntry getSessionEntry() {
        return sessionEntry;
    }

    public void setSessionEntry(SessionEntry sessionEntry) {
        this.sessionEntry = sessionEntry;
    }

    public void setFname(String fname) {
        this.fname = fname;
    }

    public void setFdir(String fdir) {
        this.fdir = fdir;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public String getFdir() {
        return fdir;
    }

    public String getFname() {
        return fname;
    }

    public String getPath() {
        return path;
    }
}
