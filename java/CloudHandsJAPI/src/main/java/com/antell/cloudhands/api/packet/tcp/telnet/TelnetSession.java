package com.antell.cloudhands.api.packet.tcp.telnet;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.tcp.TCPSessionEntry;
import com.antell.cloudhands.api.source.AbstractSourceEntry;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;

public class TelnetSession extends AbstractSourceEntry {

    private SessionEntry sessionEntry;
    private String user;
    private String passwd;
    private String contentPath;
    private String statBruteForce;

    public TelnetSession(MessageUnpacker unpacker) throws IOException {

       sessionEntry = new TCPSessionEntry();
        parse(unpacker);
    }

    private void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,false);
        Preconditions.checkArgument(n==2,"Invalid telnet session messagePack:"+n);

        /*parse session entry */
        sessionEntry.parse(unpacker);

        /*skip telnet proto name and to parse telnet proto info*/
        n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n == 3,"Invalid telnet session msgpack packet:"+n);

        setUser(MessagePackUtil.parseText(unpacker));
        setPasswd(MessagePackUtil.parseText(unpacker));
        setContentPath(MessagePackUtil.parseText(unpacker));

        setStatBruteForce();
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();
        sb.append(sessionEntry.dataToString());
        TextUtils.addText(sb,"user",user);
        TextUtils.addText(sb,"passwd",passwd);
        TextUtils.addText(sb,"contentPath",contentPath);
        TextUtils.addText(sb,"statBruteForce",statBruteForce);

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
        cb.field("user",user);
        cb.field("passwd",passwd);
        cb.field("contentPath",contentPath);
        cb.field("statBruteForce",statBruteForce);

        return cb;
    }
    public String getProto(){

        return "telnet";
    }

    public long getSrcIPI(){

        return sessionEntry.getReqIP();
    }

    public long getDstIPI(){
        return sessionEntry.getResIP();
    }

    public long getTime(){

        return sessionEntry.getReqStartTime();
    }

    public String getUser() {
        return user;
    }

    public void setUser(String user) {
        this.user = user;
    }

    public String getPasswd() {
        return passwd;
    }

    public void setPasswd(String passwd) {
        this.passwd = passwd;
    }

    public String getContentPath() {
        return contentPath;
    }

    public void setContentPath(String contentPath) {
        this.contentPath = contentPath;
    }

    public void setSessionEntry(SessionEntry sessionEntry) {
        this.sessionEntry = sessionEntry;
    }

    public SessionEntry getSessionEntry() {
        return sessionEntry;
    }

    public String getStatBruteForce() {
        return statBruteForce;
    }

    public void setStatBruteForce() {

        StringBuffer sb = new StringBuffer();

        sb.append(sessionEntry.getReqIP());
        sb.append("|");
        sb.append(sessionEntry.getResIP());
        this.statBruteForce = sb.toString();

    }
}
