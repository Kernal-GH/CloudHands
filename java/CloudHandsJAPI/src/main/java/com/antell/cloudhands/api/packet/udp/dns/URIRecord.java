package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import com.antell.cloudhands.api.utils.Text;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Uniform Resource Identifier (URI) DNS Resource Record
 *
 */

public class URIRecord extends Record {

    private int priority;
    private int weight;
    private String target;

    public URIRecord() {
        target = "";
    }

    @Override
    public Record getObject() {
        return new URIRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        priority = in.readUnsignedShort();
        weight = in.readUnsignedShort();
        target = Text.readString(in,2);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        MessagePackUtil.parseMapHeader(unpacker,true);
        priority = MessagePackUtil.parseInt(unpacker);
        weight = MessagePackUtil.parseInt(unpacker);
        target = MessagePackUtil.parseText(unpacker);
    }

    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(priority + " ");
        sb.append(weight + " ");
        sb.append(target);
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("priority",priority);
        cb.field("weight",weight);
        cb.field("target",target);

        return cb;
    }

    /**
     * Returns the priority
     */
    public int getPriority() {
        return priority;
    }

    /**
     * Returns the weight
     */
    public int getWeight() {
        return weight;
    }

    /**
     * Returns the target URI
     */
    public String getTarget() {
        return target;
    }
}
