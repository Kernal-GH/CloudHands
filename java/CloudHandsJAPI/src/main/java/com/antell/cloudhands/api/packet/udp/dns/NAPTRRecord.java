package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Name Authority Pointer Record  - specifies rewrite rule, that when applied
 * to an existing string will produce a new domain.
 */

public class NAPTRRecord extends Record {

    private int order;
    private int preference;

    private String flags;
    private String service;
    private String regexp;

    private Name replacement;

    public NAPTRRecord() {
    }

    @Override
    public Record getObject() {
        return new NAPTRRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {

        order = in.readUnsignedShort();
        preference = in.readUnsignedShort();
        flags = Text.readString(in,2);
        service = Text.readString(in,2);
        regexp = Text.readString(in,2);
        replacement = new Name(in);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        order = MessagePackUtil.parseInt(unpacker);
        preference = MessagePackUtil.parseInt(unpacker);
        flags = MessagePackUtil.parseText(unpacker);
        service = MessagePackUtil.parseText(unpacker);
        regexp = MessagePackUtil.parseText(unpacker);
        replacement = new Name(unpacker);
    }

    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(order);
        sb.append(" ");
        sb.append(preference);
        sb.append(" ");
        sb.append(flags);
        sb.append(" ");
        sb.append(service);
        sb.append(" ");
        sb.append(regexp);
        sb.append(" ");
        sb.append(replacement);

        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("order",order);
        cb.field("preference",preference);
        cb.field("flags",flags);
        cb.field("service",service);
        cb.field("regexp",regexp);
        cb.field("replacement",replacement);

        return cb;
    }

    /**
     * Returns the order
     */
    public int getOrder() {
        return order;
    }

    /**
     * Returns the preference
     */
    public int getPreference() {
        return preference;
    }

    /**
     * Returns flags
     */
    public String getFlags() {
        return flags;
    }

    /**
     * Returns service
     */
    public String getService() {
        return service;
    }

    /**
     * Returns regexp
     */
    public String getRegexp() {
        return regexp;
    }

    /**
     * Returns the replacement domain-name
     */
    public Name getReplacement() {
        return replacement;
    }


    @Override
    public Name getAdditionalName() {
        return replacement;
    }

}
