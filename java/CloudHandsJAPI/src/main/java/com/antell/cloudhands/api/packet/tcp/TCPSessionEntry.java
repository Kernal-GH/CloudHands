package com.antell.cloudhands.api.packet.tcp;

import com.antell.cloudhands.api.packet.SessionEntry;
import com.antell.cloudhands.api.utils.Constants;
import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.TextUtils;
import com.google.common.base.Preconditions;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by mala on 2018/6/16.
 */
public class TCPSessionEntry extends SessionEntry {

    private long reqPBytes;
    private long resPBytes;

    @Override
    public XContentBuilder dataToJson(XContentBuilder cb) throws IOException {

        super.dataToJson(cb);

        cb.field("reqPBytes",getReqPBytes());
        cb.field("resPBytes",getResPBytes());

        return cb;
    }

    @Override
    public String getIndexMapping() {
        return null;
    }

    @Override
    public String getIndexNamePrefix() {
        return null;
    }

    @Override
    public String getIndexDocType() {
        return null;
    }

    @Override
    public String getSrcIP() {
        return null;
    }

    @Override
    public String getDstIP() {
        return null;
    }

    @Override
    public String dataToString() {

        StringBuffer sb = new StringBuffer();

        sb.append("TCP.Session.Entry.info:\n\n");
        sb.append(super.dataToString());

        TextUtils.addLong(sb,"reqPBytes",getReqPBytes());
        TextUtils.addLong(sb,"resPBytes",getResPBytes());
        return sb.toString();
    }

    @Override
    public void parse(MessageUnpacker unpacker) throws IOException {

        int n = MessagePackUtil.parseMapHeader(unpacker,true);
        Preconditions.checkArgument(n==17,"Invalid msgpack packet of session entry:"+n);

        setProtocolID(MessagePackUtil.parseLong(unpacker));
        setProtocol(Constants.protoID2String[(int)getProtocolID()]);
        setSessionID(MessagePackUtil.parseLong(unpacker));
        setReqStartTime(MessagePackUtil.parseLong(unpacker));
        setResStartTime(MessagePackUtil.parseLong(unpacker));
        setIPV6(MessagePackUtil.parseByte(unpacker)==1);
        setReqIP(MessagePackUtil.parseLong(unpacker));
        setResIP(MessagePackUtil.parseLong(unpacker));
        setReqAddr(MessagePackUtil.parseBin(unpacker));
        setResAddr(MessagePackUtil.parseBin(unpacker));
        setReqPort(MessagePackUtil.parseInt(unpacker));
        setResPort(MessagePackUtil.parseInt(unpacker));
        setReqPackets(MessagePackUtil.parseLong(unpacker));
        setReqBytes(MessagePackUtil.parseLong(unpacker));
        setReqPBytes(MessagePackUtil.parseLong(unpacker));

        setResPackets(MessagePackUtil.parseLong(unpacker));
        setResBytes(MessagePackUtil.parseLong(unpacker));
        setResPBytes(MessagePackUtil.parseLong(unpacker));

    }

    public long getReqPBytes() {
        return reqPBytes;
    }

    public void setReqPBytes(long reqPBytes) {
        this.reqPBytes = reqPBytes;
    }

    public long getResPBytes() {
        return resPBytes;
    }

    public void setResPBytes(long resPBytes) {
        this.resPBytes = resPBytes;
    }

    @Override
    public void read(DataInput in) throws IOException {

    }
}
