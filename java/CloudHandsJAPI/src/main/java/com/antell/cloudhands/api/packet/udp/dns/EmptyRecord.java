
package com.antell.cloudhands.api.packet.udp.dns;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * A class implementing Records with no data; that is, records used in
 * the question section of messages and meta-records in dynamic update.
 */

class EmptyRecord extends Record {

    private static final long serialVersionUID = 3601852050646429582L;

    public EmptyRecord() {
    }

    @Override
    public Record getObject() {
        return new EmptyRecord();
    }

    @Override
    public void rrFromWire(DNSInput in) throws IOException {
    }

    @Override
    public void rdataFromString(Tokenizer st, Name origin) throws IOException {
    }

    @Override
    public String rrToString() {
        return "";
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {
        return cb;
    }

    @Override
    public void rrToWire(DNSOutput out, Compression c, boolean canonical) {
    }

}
