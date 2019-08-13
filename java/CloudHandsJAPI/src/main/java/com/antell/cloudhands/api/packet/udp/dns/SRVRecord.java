package com.antell.cloudhands.api.packet.udp.dns;

import com.antell.cloudhands.api.utils.MessagePackUtil;
import org.elasticsearch.common.xcontent.XContentBuilder;
import org.msgpack.core.MessageUnpacker;

import java.io.DataInput;
import java.io.IOException;

/**
 * Server Selection Record  - finds hosts running services in a domain.  An
 * SRV record will normally be named _&lt;service&gt;._&lt;protocol&gt;.domain
 * - examples would be _sips._tcp.example.org (for the secure SIP protocol) and
 * _http._tcp.example.com (if HTTP used SRV records)
 */

public class SRVRecord extends Record {


    private int priority;
    private int weight;
    private int port;
    private Name target;

    public SRVRecord() {
    }

    @Override
    public Record getObject() {
        return new SRVRecord();
    }

    @Override
    public void read(DataInput in) throws IOException {
        priority = in.readUnsignedShort();
        weight = in.readUnsignedShort();
        port = in.readUnsignedShort();
        target = new Name(in);
    }

    @Override
    public void read(MessageUnpacker unpacker) throws IOException {

        priority = MessagePackUtil.parseInt(unpacker);
        weight = MessagePackUtil.parseInt(unpacker);
        port = MessagePackUtil.parseInt(unpacker);
        target = new Name(unpacker);
    }


    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(priority + " ");
        sb.append(weight + " ");
        sb.append(port + " ");
        sb.append(target);
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("priority",priority);
        cb.field("weight",weight);
        cb.field("port",port);
        cb.field("target",target.toString());
        return cb;
    }

    /**
     * Returns the priority
     */
    public int
    getPriority() {
        return priority;
    }

    /**
     * Returns the weight
     */
    public int
    getWeight() {
        return weight;
    }

    /**
     * Returns the port that the service runs on
     */
    public int
    getPort() {
        return port;
    }

    /**
     * Returns the host running that the service
     */
    public Name
    getTarget() {
        return target;
    }


    @Override
    public Name getAdditionalName() {
        return target;
    }

}
