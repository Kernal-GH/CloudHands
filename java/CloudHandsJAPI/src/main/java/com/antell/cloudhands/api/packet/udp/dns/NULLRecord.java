package com.antell.cloudhands.api.packet.udp.dns;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * The NULL Record.  This has no defined purpose, but can be used to
 * hold arbitrary data.
 *
 */

public class NULLRecord extends Record {

    private static final long serialVersionUID = -5796493183235216538L;

    private byte[] data;

    public NULLRecord() {
    }

    @Override
    public Record getObject() {
        return new NULLRecord();
    }

    /**
     * Creates a NULL record from the given data.
     *
     * @param data The contents of the record.
     */
    public NULLRecord(Name name, int dclass, long ttl, byte[] data) {
        super(name, Type.NULL, dclass, ttl);

        if (data.length > 0xFFFF) {
            throw new IllegalArgumentException("data must be <65536 bytes");
        }
        this.data = data;
    }

    @Override
    public void rrFromWire(DNSInput in) throws IOException {
        data = in.readByteArray();
    }

    @Override
    public void rdataFromString(Tokenizer st, Name origin) throws IOException {
        throw st.exception("no defined text format for NULL records");
    }

    @Override
    public String rrToString() {
        return unknownToString(data);
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {
        return cb;
    }

    /**
     * Returns the contents of this record.
     */
    public byte[] getData() {
        return data;
    }

    @Override
    public void rrToWire(DNSOutput out, Compression c, boolean canonical) {
        out.writeByteArray(data);
    }

}
