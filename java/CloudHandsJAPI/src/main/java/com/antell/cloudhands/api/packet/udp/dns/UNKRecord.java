package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.DataInput;
import java.io.IOException;

/**
 * A class implementing Records of unknown and/or unimplemented types.  This
 * class can only be initialized using static Record initializers.
 */

public class UNKRecord extends Record {

    private byte[] data;

    public UNKRecord() {
    }

    @Override
    public Record getObject() {
        return new UNKRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        data = Text.readBytes(in,2);
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

}
