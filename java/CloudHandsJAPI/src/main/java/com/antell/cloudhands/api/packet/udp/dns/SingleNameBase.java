package com.antell.cloudhands.api.packet.udp.dns;

import org.elasticsearch.common.xcontent.XContentBuilder;

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
