package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.BinDataInput;
import com.antell.cloudhands.api.DataDump;
import com.antell.cloudhands.api.ESIndexable;
import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.utils.DateUtils;
import com.antell.cloudhands.api.utils.IPUtils;
import com.antell.cloudhands.api.utils.TextUtils;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by mala on 2018/6/16.
 */
public class DNSSessionEntry extends SessionEntry implements BinDataInput,ESIndexable,DataDump {

    @Override
    public void read(DataInput in) throws IOException {

        setSessionID(in.readLong());
        setReqIP(in.readInt());
        setResIP(in.readInt());
        setReqPort(in.readUnsignedShort());
        setResPort(in.readUnsignedShort());
        setReqPackets(in.readLong());
        setReqBytes(in.readLong());
        setResPackets(in.readLong());
        setResBytes(in.readLong());
        setReqStartTime(in.readLong());
        setReqLastTime(in.readLong());
        setResStartTime(in.readLong());
        setResLastTime(in.readLong());
    }

    @Override
    public String dataToString() {
        StringBuffer sb = new StringBuffer();

        sb.append("DNS.Session.Entry.info:\n\n");

        TextUtils.addLong(sb,"sessionID",getSessionID());
        TextUtils.addText(sb,"srcIP", IPUtils.ipv4Str(getReqIP()));
        TextUtils.addText(sb,"dstIP", IPUtils.ipv4Str(getResIP()));
        TextUtils.addInt(sb,"srcPort",getReqPort());
        TextUtils.addInt(sb,"dstPort",getResPort());
        TextUtils.addLong(sb,"reqPackets",getReqPackets());
        TextUtils.addLong(sb,"reqBytes",getReqBytes());
        TextUtils.addLong(sb,"resPackets",getResPackets());
        TextUtils.addLong(sb,"resBytes",getResBytes());
        TextUtils.addText(sb,"reqStartTime", DateUtils.format(getReqStartTime()));
        TextUtils.addText(sb,"reqLastTime", DateUtils.format(getReqLastTime()));
        TextUtils.addText(sb,"resStartTime", DateUtils.format(getResStartTime()));
        TextUtils.addText(sb,"resLastTime", DateUtils.format(getResLastTime()));

        return sb.toString();
    }

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        cb.field("sessionID",getSessionID());
        cb.field("srcIP",IPUtils.ipv4Str(getReqIP()));
        cb.field("dstIP",IPUtils.ipv4Str(getResIP()));
        cb.field("srcPort",getReqPort());
        cb.field("dstPort",getResPort());
        cb.field("reqPackets",getReqPackets());
        cb.field("reqBytes",getReqBytes());
        cb.field("resPackets",getResPackets());
        cb.field("resBytes",getResBytes());
        cb.field("reqStartTime", getReqStartTime());
        cb.field("reqLastTime", getReqLastTime());
        cb.field("resStartTime", getResStartTime());
        cb.field("resLastTime", getResLastTime());
        cb.field("timeDate",DateUtils.format(getReqStartTime()));
        return cb;
    }
}
