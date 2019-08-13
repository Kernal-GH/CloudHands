package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Certification Authority Authorization
 */

public class CAARecord extends Record {

    public static class Flags {
        private Flags() {
        }

        public static final int IssuerCritical = 128;
    }

    private int flags;
    private String tag;
    private String value;

    public CAARecord() {
    }

    @Override
    public Record getObject() {
        return new CAARecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        flags = in.readUnsignedByte();
        tag = Text.readString(in,2);
        value = Text.readString(in,2);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        flags = MessagePackUtil.parseInt(unpacker);
        tag = MessagePackUtil.parseText(unpacker);
        value = MessagePackUtil.parseText(unpacker);
    }

    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(flags);
        sb.append(" ");
        sb.append(tag);
        sb.append(" ");
        sb.append(value);

        return sb.toString();
    }

    @Override
    XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("flags",flags);
        cb.field("tag",tag);
        cb.field("value",value);

        return cb;
    }

    /**
     * Returns the flags.
     */
    public int getFlags() {
        return flags;
    }

    /**
     * Returns the tag.
     */
    public String getTag() {
        return tag;
    }

    /**
     * Returns the value
     */
    public String getValue() {
        return value;
    }

}
