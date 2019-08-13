package com.antell.cloudhands.api.packet.udp.tftp;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.tcp.FileTranSession;
import com.antell.cloudhands.api.packet.udp.UDPSessionEntry;
import com.antell.cloudhands.api.source.AbstractSourceEntry;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.*;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class TFTPSession extends AbstractSourceEntry {
    private SessionEntry sessionEntry;
    private int isRead;
    private int isError;
    private String fname;
    private String mode;
    private String errmsg;
    private String fpath;

    public TFTPSession(DataInput in) throws IOException {

        sessionEntry = new UDPSessionEntry();
        sessionEntry.read(in);
        sessionEntry.setProtocol("TFTP");
        long reqIP = sessionEntry.getReqIP();
        sessionEntry.setReqIP(sessionEntry.getResIP());
        sessionEntry.setResIP(reqIP);

        int reqPort = sessionEntry.getReqPort();
        sessionEntry.setReqPort(sessionEntry.getResPort());
        sessionEntry.setResPort(reqPort);

        long reqBytes = sessionEntry.getReqBytes();
        sessionEntry.setReqBytes(sessionEntry.getResBytes());
        sessionEntry.setResBytes(reqBytes);

        long reqPackets = sessionEntry.getReqPackets();
        sessionEntry.setReqPackets(sessionEntry.getResPackets());
        sessionEntry.setResPackets(reqPackets);

        setIsRead(in.readUnsignedByte());
        setIsError(in.readUnsignedByte());
        setFname(Text.readString(in, 2));
        setMode(Text.readString(in, 2));
        setErrmsg(Text.readString(in, 2));
        setFpath(Text.readString(in, 2));

    }

    public TFTPSession(MessageUnpacker unpacker) throws IOException {

        sessionEntry = new UDPSessionEntry();
        int n = MessagePackUtil.parseMapHeader(unpacker,false);
        Preconditions.checkArgument(n==2,"Invalid msgpack packet of tftp session entry:"+n);

        sessionEntry.parse(unpacker);

        n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==6,"Invalid msgpack packet of tftp session:"+n);

        sessionEntry.setProtocol("TFTP");
        long reqIP = sessionEntry.getReqIP();
        sessionEntry.setReqIP(sessionEntry.getResIP());
        sessionEntry.setResIP(reqIP);

        int reqPort = sessionEntry.getReqPort();
        sessionEntry.setReqPort(sessionEntry.getResPort());
        sessionEntry.setResPort(reqPort);

        long reqBytes = sessionEntry.getReqBytes();
        sessionEntry.setReqBytes(sessionEntry.getResBytes());
        sessionEntry.setResBytes(reqBytes);

        long reqPackets = sessionEntry.getReqPackets();
        sessionEntry.setReqPackets(sessionEntry.getResPackets());
        sessionEntry.setResPackets(reqPackets);

        setIsRead(MessagePackUtil.parseInt(unpacker));
        setIsError(MessagePackUtil.parseInt(unpacker));
        setFname(MessagePackUtil.parseText(unpacker));
        setMode(MessagePackUtil.parseText(unpacker));
        setErrmsg(MessagePackUtil.parseText(unpacker));
        setFpath(MessagePackUtil.parseText(unpacker));

    }
    @Override
    public String dataToString() {
        StringBuffer sb = new StringBuffer();

        sb.append(sessionEntry.dataToString());
        TextUtils.addText(sb, "action", isRead == 1 ? "read" : "write");
        TextUtils.addText(sb, "isError", isError == 1 ? "true" : "false");
        TextUtils.addText(sb, "fname", fname);
        TextUtils.addText(sb, "mode", mode);
        TextUtils.addText(sb, "errmsg", errmsg);
        TextUtils.addText(sb, "fpath", fpath);

        return sb.toString();
    }

    @Override
    public String toString() {

        return dataToString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        XContentBuilder seCB = cb.startObject("sessionEntry");
        sessionEntry.dataToJson(seCB);
        seCB.endObject();
        cb.field("objectId",getObjectId());
        cb.field("isRead", isRead);
        cb.field("isError", isError);
        cb.field("fname", TextUtils.getStrValue(fname));
        cb.field("mode", TextUtils.getStrValue(mode));
        cb.field("errmsg", TextUtils.getStrValue(errmsg));
        cb.field("fpath", TextUtils.getStrValue(fpath));

        return cb;
    }

    @Override
    public String getIndexMapping() {

        String mapping = "{" +
                "\"properties\":{" +
                "\"id\":{\"type\":\"keyword\"}," +
                "\"sessionEntry\":{" +
                "\"properties\":{" +
                "\"sessionID\":{\"type\":\"long\"}," +
                "\"srcIP\":{\"type\":\"keyword\"}," +
                "\"dstIP\":{\"type\":\"keyword\"}," +
                "\"srcPort\":{\"type\":\"integer\"}," +
                "\"dstPort\":{\"type\":\"integer\"}," +
                "\"reqPackets\":{\"type\":\"long\"}," +
                "\"reqBytes\":{\"type\":\"long\"}," +
                "\"resPackets\":{\"type\":\"long\"}," +
                "\"resBytes\":{\"type\":\"long\"}," +
                "\"reqStartTime\":{\"type\":\"long\"}," +
                "\"reqLastTime\":{\"type\":\"long\"}," +
                "\"resStartTime\":{\"type\":\"long\"}," +
                "\"resLastTime\":{\"type\":\"long\"}," +
                "\"timeDate\":{\"type\":\"date\",\"format\":\"yyyy-MM-dd HH:mm:ss\"}" +
                "}" +
                "}," +
                "\"objectId\":{\"type\":\"keyword\"}," +
                "\"isRead\":{\"type\":\"integer\"}, " +
                "\"isError\":{\"type\":\"integer\"}, " +
                "\"fname\":{\"type\":\"keyword\"}," +
                "\"mode\":{\"type\":\"keyword\"}," +
                "\"errmsg\":{\"type\":\"keyword\"}," +
                "\"fpath\":{\"type\":\"keyword\"}," +
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
        return "log_udp_session_tftp";
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

    private boolean canGenerateFTrans() {

        if (TextUtils.isEmpty(fname) || TextUtils.isEmpty(fpath))
            return false;

        try {
            return Files.size(Paths.get(fpath)) > 0;
        } catch (IOException e) {
            return false;
        }
    }

    private FileTranSession toFileTranSession(long fsize) {

        FileTranSession fileTranSession = new FileTranSession();

        fileTranSession.setProto("tftp");
        fileTranSession.setParentObjectId(getObjectId());
        fileTranSession.setSrcIP(IPUtils.ipv4Str(sessionEntry.getReqIP()));
        fileTranSession.setSrcPort(sessionEntry.getReqPort());
        fileTranSession.setDstIP(IPUtils.ipv4Str(sessionEntry.getResIP()));
        fileTranSession.setDstPort(sessionEntry.getResPort());
        fileTranSession.setTime(sessionEntry.getReqStartTime());
        fileTranSession.setBytes(fsize);
        fileTranSession.setContentPath(fpath);
        fileTranSession.setFname(fname);
        fileTranSession.setExtName(Content.getExtName(fname));

        return fileTranSession;
    }

    @Override
    public List<SourceEntry> generate() {

        List<SourceEntry> results = new ArrayList<>();

        if (!canGenerateFTrans())
            return null;

        long fsize = 0;

        try {
            fsize = Files.size(Paths.get(fpath));
            results.add(toFileTranSession(fsize));
        } catch (IOException e) {
            //return null;
        }

        return results;
    }

    public SessionEntry getSessionEntry() {
        return sessionEntry;
    }

    public int getIsRead() {
        return isRead;
    }

    public void setIsRead(int isRead) {
        this.isRead = isRead;
    }

    public int getIsError() {
        return isError;
    }

    public void setIsError(int isError) {
        this.isError = isError;
    }

    public String getFname() {
        return fname;
    }

    public void setFname(String fname) {
        this.fname = fname;
    }

    public String getMode() {
        return mode;
    }

    public void setMode(String mode) {
        this.mode = mode;
    }

    public String getErrmsg() {
        return errmsg;
    }

    public void setErrmsg(String errmsg) {
        this.errmsg = errmsg;
    }

    public String getFpath() {
        return fpath;
    }

    public void setFpath(String fpath) {
        this.fpath = fpath;
    }
}
