package com.antell.cloudhands.api.packet.udp.dns;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Uniform Resource Identifier (URI) DNS Resource Record
 *
 * @see <a href="http://tools.ietf.org/html/draft-faltstrom-uri">http://tools.ietf.org/html/draft-faltstrom-uri</a>
 */

public class URIRecord extends Record {

    private static final long serialVersionUID = 7955422413971804232L;

    private int priority, weight;
    private byte[] target;

    public URIRecord() {
        target = new byte[]{};
    }

    @Override
    public Record getObject() {
        return new URIRecord();
    }

    /**
     * Creates a URI Record from the given data
     *
     * @param priority The priority of this URI.  Records with lower priority
     *                 are preferred.
     * @param weight   The weight, used to select between records at the same
     *                 priority.
     * @param target   The host/port running the service
     */
    public URIRecord(Name name, int dclass, long ttl, int priority,
                     int weight, String target) {
        super(name, Type.URI, dclass, ttl);
        this.priority = checkU16("priority", priority);
        this.weight = checkU16("weight", weight);
        try {
            this.target = byteArrayFromString(target);
        } catch (TextParseException e) {
            throw new IllegalArgumentException(e.getMessage());
        }
    }

    @Override
    public void rrFromWire(DNSInput in) throws IOException {
        priority = in.readU16();
        weight = in.readU16();
        target = in.readByteArray();
    }

    @Override
    public void rdataFromString(Tokenizer st, Name origin) throws IOException {
        priority = st.getUInt16();
        weight = st.getUInt16();
        try {
            target = byteArrayFromString(st.getString());
        } catch (TextParseException e) {
            throw st.exception(e.getMessage());
        }
    }

    /**
     * Converts rdata to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(priority + " ");
        sb.append(weight + " ");
        sb.append(byteArrayToString(target, true));
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("priority",priority);
        cb.field("weight",weight);
        cb.field("target",byteArrayToString(target,true));

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
        return byteArrayToString(target, false);
    }

    @Override
    public void rrToWire(DNSOutput out, Compression c, boolean canonical) {
        out.writeU16(priority);
        out.writeU16(weight);
        out.writeByteArray(target);
    }

}
