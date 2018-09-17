package com.antell.cloudhands.api.packet.udp.tftp;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.tcp.FileTranSession;
import com.antell.cloudhands.api.packet.udp.UDPSessionEntry;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.Text;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class TFTPSession implements SourceEntry {
    private final String objectId;
    private SessionEntry sessionEntry;
    private int isRead;
    private int isError;
    private String fname;
    private String mode;
    private String errmsg;
    private String fpath;

    public TFTPSession(DataInput in) throws IOException {

        objectId = TextUtils.getUUID();
        sessionEntry = new UDPSessionEntry();
        sessionEntry.read(in);

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
        cb.field("objectId",objectId);
        cb.field("isRead", isRead);
        cb.field("isError", isError);
        cb.field("fname", TextUtils.getStrValue(fname));
        cb.field("mode", TextUtils.getStrValue(mode));
        cb.field("errmsg", TextUtils.getStrValue(errmsg));
        cb.field("fpath", TextUtils.getStrValue(fpath));

        return cb;
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
    private final static String getExtName(String fileName){

        if(TextUtils.isEmpty(fileName))
            return "data";

        int index = fileName.lastIndexOf(".");
        if(index<0)
            return "data";

        return fileName.substring(index+1);
    }

    private FileTranSession toFileTranSession(long fsize) {

        FileTranSession fileTranSession = new FileTranSession();

        fileTranSession.setProto("tftp");
        fileTranSession.setParentObjectId(objectId);
        fileTranSession.setSrcIP(IPUtils.ipv4Str(sessionEntry.getReqIP()));
        fileTranSession.setSrcPort(sessionEntry.getReqPort());
        fileTranSession.setDstIP(IPUtils.ipv4Str(sessionEntry.getResIP()));
        fileTranSession.setDstPort(sessionEntry.getResPort());
        fileTranSession.setTime(sessionEntry.getReqStartTime());
        fileTranSession.setBytes(fsize);
        fileTranSession.setContentPath(fpath);
        fileTranSession.setFname(fname);
        fileTranSession.setExtName(getExtName(fname));

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
