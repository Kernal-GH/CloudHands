package com.antell.cloudhands.api.packet.udp.dns;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * A class implementing Records of unknown and/or unimplemented types.  This
 * class can only be initialized using static Record initializers.
 */

public class UNKRecord extends Record {

    private static final long serialVersionUID = -4193583311594626915L;

    private byte[] data;

    public UNKRecord() {
    }

    @Override
    public Record getObject() {
        return new UNKRecord();
    }

    @Override
    public void rrFromWire(DNSInput in) throws IOException {
        data = in.readByteArray();
    }

    @Override
    public void rdataFromString(Tokenizer st, Name origin) throws IOException {
        throw st.exception("invalid unknown RR encoding");
    }

    /**
     * Converts this Record to the String "unknown format"
     */

    @Override
    public String rrToString() {
        return unknownToString(data);
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {
        cb.field("data",unknownToString(data));
        return cb;
    }

    /**
     * Returns the contents of this record.
     */
    public byte[]
    getData() {
        return data;
    }

    @Override
    public void rrToWire(DNSOutput out, Compression c, boolean canonical) {
        out.writeByteArray(data);
    }

}
