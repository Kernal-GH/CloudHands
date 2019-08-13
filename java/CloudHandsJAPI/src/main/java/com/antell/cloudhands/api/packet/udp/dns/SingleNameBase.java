package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Implements common functionality for the many record types whose format
 * is a single name.
 *
 */

abstract class SingleNameBase extends Record {


    protected Name singleName;

    protected SingleNameBase() {
    }

    @Override
    public void read(DataInput in) throws IOException {
        singleName = new Name(in);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        singleName = new Name(unpacker);
    }


    @Override
    public String rrToString() {
        return singleName.toString();
    }

    protected Name getSingleName() {
        return singleName;
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("name",singleName.toString());
        return cb;
    }
}
