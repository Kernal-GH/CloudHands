package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Implements common functionality for the many record types whose format
 * is an unsigned 16 bit integer followed by a name.
 */

public abstract class U16NameBase extends Record {

    protected int u16Field;
    protected Name nameField;

    protected U16NameBase() {
    }

    @Override
    public void read(DataInput in) throws IOException {
        u16Field = in.readUnsignedShort();
        nameField = new Name(in);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        u16Field = MessagePackUtil.parseInt(unpacker);
        nameField = new Name(unpacker);
    }

    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(u16Field);
        sb.append(" ");
        sb.append(nameField);
        return sb.toString();
    }

    protected int getU16Field() {
        return u16Field;
    }

    protected Name getNameField() {
        return nameField;
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("u16Field",u16Field);
        cb.field("nameField",nameField.toString());
        return cb;
    }

}
