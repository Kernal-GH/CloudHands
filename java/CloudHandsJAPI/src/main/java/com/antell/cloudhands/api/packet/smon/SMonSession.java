package com.antell.cloudhands.api.packet.smon;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.packet.tcp.TCPSessionEntry;
import com.antell.cloudhands.api.packet.udp.UDPSessionEntry;
import com.antell.cloudhands.api.source.SourceEntry;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by dell on 2018/7/17.
 */
public class SMonSession implements SourceEntry {

    private final SessionEntry sessionEntry;

    private long id;
    private String proto;
    private String reqBodyPath;
    private String resBodyPath;

    public SMonSession(MessageUnpacker unpacker) throws IOException {

        this.sessionEntry = new TCPSessionEntry();
        this.proto = "tcp";

        parse(unpacker);
    }

    public SMonSession(DataInput input) throws IOException {

        this.sessionEntry = new UDPSessionEntry();
        proto = "udp";

        parse(input);
    }

    private void parse(DataInput input) throws IOException {

        sessionEntry.read(input);
        id = input.readLong();

        reqBodyPath = Text.readString(input,2);
        resBodyPath = Text.readString(input,2);

    }

    private void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,false);
        Preconditions.checkArgument(n==2 ||n == 3,"Invalid smon session messagePack:"+n);

        /*parse session entry */
        sessionEntry.parse(unpacker);

        /*skip http proto name and to parse http proto info*/
        n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n == 3,"Invalid smon session msgpack packet:"+n);

        id = MessagePackUtil.parseLong(unpacker);
        reqBodyPath = MessagePackUtil.parseText(unpacker);
        resBodyPath = MessagePackUtil.parseText(unpacker);

    }

    @Override
    public String dataToString() {
        StringBuffer sb = new StringBuffer();

        sb.append(sessionEntry.dataToString());
        TextUtils.addLong(sb,"id",id);
        TextUtils.addText(sb,"proto",proto);
        TextUtils.addText(sb,"reqBodyPath",reqBodyPath);
        TextUtils.addText(sb,"resBodyPath",resBodyPath);

        return sb.toString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        XContentBuilder seCB = cb.startObject("sessionEntry");
        sessionEntry.dataToJson(seCB);
        seCB.endObject();

        cb.field("smonID",id);
        cb.field("proto",proto);
        cb.field("reqBodyPath",reqBodyPath);
        cb.field("resBodyPath",resBodyPath);
        return cb;
    }


    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }

    public String getProto() {
        return proto;
    }

    public void setProto(String proto) {
        this.proto = proto;
    }

    public String getReqBodyPath() {
        return reqBodyPath;
    }

    public void setReqBodyPath(String reqBodyPath) {
        this.reqBodyPath = reqBodyPath;
    }

    public String getResBodyPath() {
        return resBodyPath;
    }

    public void setResBodyPath(String resBodyPath) {
        this.resBodyPath = resBodyPath;
    }

    public SessionEntry getSessionEntry() {
        return sessionEntry;
    }


}
