package com.antell.cloudhands.api.packet.tcp.ssh;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.tcp.TCPSessionEntry;
import com.antell.cloudhands.api.source.AbstractSourceEntry;
import com.antell.cloudhands.api.utils.Constants;
import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;

public class SSHSession extends AbstractSourceEntry {

    private SessionEntry sessionEntry;
    private SSHData clientData;
    private SSHData serverData;
    private String statBruteForce;

    public SSHSession(MessageUnpacker unpacker) throws IOException {

        sessionEntry = new TCPSessionEntry();
        parse(unpacker);
    }

    private void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,false);
        Preconditions.checkArgument(n==2,"Invalid ssh session messagePack:"+n);

        /*parse session entry */
        sessionEntry.parse(unpacker);

        /*skip telnet proto name and to parse telnet proto info*/
        n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n == 2,"Invalid telnet session msgpack packet:"+n);

        setClientData(new SSHData(unpacker));
        setServerData(new SSHData(unpacker));

        setStatBruteForce();
    }

    public SessionEntry getSessionEntry() {
        return sessionEntry;
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        TextUtils.addText(sb,"ClientData","");
        clientData.dataToString(sb);
        TextUtils.addText(sb,"ServerData","");
        serverData.dataToString(sb);

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

        cb.field("statBruteForce",statBruteForce);
        clientData.dataToJson(cb,"clientData");
        serverData.dataToJson(cb,"serverData");

        return cb;
    }

    @Override
    public String getIndexMapping() {

        String mapping = "{" +
                "\"properties\":{" +
                "\"id\":{\"type\":\"keyword\"}," +
                "\"statBruteForce\":{\"type\":\"keyword\"}," +
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
                "\"clientData\":{" +
                "\"properties\":{" +
                "\"state\":{\"type\":\"integer\"}," +
                "\"version\":{\"type\":\"integer\"}," +
                "\"pversion\":{\"type\":\"keyword\"}," +
                "\"pkts\":{\"type\":\"long\"}," +
                "\"bytes\":{\"type\":\"long\"}," +
                "\"encPkts\":{\"type\":\"long\"}," +
                "\"encBytes\":{\"type\":\"long\"}," +
                "\"encMinBytes\":{\"type\":\"long\"}," +
                "\"encMaxBytes\":{\"type\":\"long\"}" +
                "}" +
                "}," +
                "\"serverData\":{" +
                "\"properties\":{" +
                "\"state\":{\"type\":\"integer\"}," +
                "\"version\":{\"type\":\"integer\"}," +
                "\"pversion\":{\"type\":\"keyword\"}," +
                "\"pkts\":{\"type\":\"long\"}," +
                "\"bytes\":{\"type\":\"long\"}," +
                "\"encPkts\":{\"type\":\"long\"}," +
                "\"encBytes\":{\"type\":\"long\"}," +
                "\"encMinBytes\":{\"type\":\"long\"}," +
                "\"encMaxBytes\":{\"type\":\"long\"}" +
                "}" +
                "}," +
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
        return "log_tcp_session_ssh";
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

    public String getProto(){

        return "ssh";
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

    public SSHData getClientData() {
        return clientData;
    }

    public void setClientData(SSHData clientData) {
        this.clientData = clientData;
    }

    public SSHData getServerData() {
        return serverData;
    }

    public void setServerData(SSHData serverData) {
        this.serverData = serverData;
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
