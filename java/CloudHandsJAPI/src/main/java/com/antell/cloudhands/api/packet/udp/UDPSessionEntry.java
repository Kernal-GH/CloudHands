package com.antell.cloudhands.api.packet.udp;

import com.antell.cloudhands.api.packet.SessionEntry;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Created by mala on 2018/6/16.
 */
public class UDPSessionEntry extends SessionEntry {

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
    public void parse(MessageUnpacker unpacker) throws IOException {

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
}
