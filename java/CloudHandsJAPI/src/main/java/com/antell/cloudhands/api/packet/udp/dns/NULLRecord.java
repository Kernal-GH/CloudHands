package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
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


    @Override
    public void read(DataInput in) throws IOException {

        data = Text.readBytes(in,2);

    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        data = MessagePackUtil.parseBin(unpacker);
    }


    @Override
    public String rrToString() {
        return unknownToString(data);
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("nullData",unknownToString(data));
        return cb;
    }

    /**
     * Returns the contents of this record.
     */
    public byte[] getData() {
        return data;
    }


}
