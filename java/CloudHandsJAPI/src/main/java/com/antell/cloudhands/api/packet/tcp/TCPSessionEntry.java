package com.antell.cloudhands.api.packet.tcp;

import com.antell.cloudhands.api.DataDump;
import com.antell.cloudhands.api.ESIndexable;
import com.antell.cloudhands.api.MsgPackDataInput;
import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.utils.*;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.IOException;

/**
 * Created by mala on 2018/6/16.
 */
public class TCPSessionEntry extends SessionEntry implements MsgPackDataInput,ESIndexable,DataDump {


    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("sessionID",getSessionID());
        cb.field("protocol",Constants.protoID2String[getProtocolID()]);
        cb.field("srcIP",getReqIP());
        cb.field("dstIP",getResIP());
        cb.field("srcPort",getReqPort());
        cb.field("dstPort",getResPort());
        cb.field("reqStartTime", getReqStartTime());
        cb.field("resStartTime", getResStartTime());
        cb.field("timeDate",DateUtils.format(getReqStartTime()));
        return cb;
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        sb.append("TCP.Session.Entry.info:\n\n");

        TextUtils.addText(sb,"proto",Constants.protoID2String[getProtocolID()]);
        TextUtils.addLong(sb,"sessionID",getSessionID());
        TextUtils.addText(sb,"srcIP", IPUtils.ipv4Str(getReqIP()));
        TextUtils.addText(sb,"dstIP", IPUtils.ipv4Str(getResIP()));
        TextUtils.addInt(sb,"srcPort",getReqPort());
        TextUtils.addInt(sb,"dstPort",getResPort());
        TextUtils.addText(sb,"reqStartTime", DateUtils.format(getReqStartTime()));
        TextUtils.addText(sb,"resStartTime", DateUtils.format(getResStartTime()));

        return sb.toString();
    }

    @Override
    public void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==8,"Invalid msgpack packet of session entry:"+n);

        setProtocolID(MessagePackUtil.parseInt(unpacker));
        setSessionID(MessagePackUtil.parseLong(unpacker));
        setReqStartTime(MessagePackUtil.parseLong(unpacker));
        setResStartTime(MessagePackUtil.parseLong(unpacker));
        setReqIP(MessagePackUtil.parseLong(unpacker));
        setResIP(MessagePackUtil.parseLong(unpacker));
        setReqPort(MessagePackUtil.parseInt(unpacker));
        setResPort(MessagePackUtil.parseInt(unpacker));

    }
}
