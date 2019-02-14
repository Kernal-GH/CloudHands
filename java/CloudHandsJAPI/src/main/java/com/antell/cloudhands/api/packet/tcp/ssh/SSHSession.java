package com.antell.cloudhands.api.packet.tcp.ssh;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.tcp.TCPSessionEntry;
import com.antell.cloudhands.api.source.AbstractSourceEntry;
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

    }

    public SessionEntry getSessionEntry() {
        return sessionEntry;
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        TextUtils.addText(sb,"ClientData","\n");
        clientData.dataToString(sb);
        TextUtils.addText(sb,"ServerData","\n");
        serverData.dataToString(sb);

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

        clientData.dataToJson(cb,"clientData");
        serverData.dataToJson(cb,"serverData");

        return cb;
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
}
