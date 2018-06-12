package com.antell.cloudhands.api.packet.udp.dns;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Start of Authority - describes properties of a zone.
 *
 */

public class SOARecord extends Record {

    private static final long serialVersionUID = 1049740098229303931L;

    private Name host, admin;
    private long serial, refresh, retry, expire, minimum;

    public SOARecord() {
    }

    @Override
    public Record getObject() {
        return new SOARecord();
    }

    /**
     * Creates an SOA Record from the given data
     *
     * @param host    The primary name server for the zone
     * @param admin   The zone administrator's address
     * @param serial  The zone's serial number
     * @param refresh The amount of time until a secondary checks for a new serial
     *                number
     * @param retry   The amount of time between a secondary's checks for a new
     *                serial number
     * @param expire  The amount of time until a secondary expires a zone
     * @param minimum The minimum TTL for records in the zone
     */
    public SOARecord(Name name, int dclass, long ttl, Name host, Name admin,
                     long serial, long refresh, long retry, long expire, long minimum) {
        super(name, Type.SOA, dclass, ttl);
        this.host = checkName("host", host);
        this.admin = checkName("admin", admin);
        this.serial = checkU32("serial", serial);
        this.refresh = checkU32("refresh", refresh);
        this.retry = checkU32("retry", retry);
        this.expire = checkU32("expire", expire);
        this.minimum = checkU32("minimum", minimum);
    }

    @Override
    public void rrFromWire(DNSInput in) throws IOException {
        host = new Name(in);
        admin = new Name(in);
        serial = in.readU32();
        refresh = in.readU32();
        retry = in.readU32();
        expire = in.readU32();
        minimum = in.readU32();
    }

    @Override
    public void rdataFromString(Tokenizer st, Name origin) throws IOException {
        host = st.getName(origin);
        admin = st.getName(origin);
        serial = st.getUInt32();
        refresh = st.getTTLLike();
        retry = st.getTTLLike();
        expire = st.getTTLLike();
        minimum = st.getTTLLike();
    }

    /**
     * Convert to a String
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(host);
        sb.append(" ");
        sb.append(admin);
        sb.append(" ");
        sb.append(serial);
        sb.append(" ");
        sb.append(refresh);
        sb.append(" ");
        sb.append(retry);
        sb.append(" ");
        sb.append(expire);
        sb.append(" ");
        sb.append(minimum);
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("host",host);
        cb.field("admin",admin);
        cb.field("serial",serial);
        cb.field("refresh",refresh);
        cb.field("retry",retry);
        cb.field("expire",expire);
        cb.field("minimum",minimum);

        return cb;
    }

    /**
     * Returns the primary name server
     */
    public Name
    getHost() {
        return host;
    }

    /**
     * Returns the zone administrator's address
     */
    public Name
    getAdmin() {
        return admin;
    }

    /**
     * Returns the zone's serial number
     */
    public long
    getSerial() {
        return serial;
    }

    /**
     * Returns the zone refresh interval
     */
    public long
    getRefresh() {
        return refresh;
    }

    /**
     * Returns the zone retry interval
     */
    public long
    getRetry() {
        return retry;
    }

    /**
     * Returns the time until a secondary expires a zone
     */
    public long
    getExpire() {
        return expire;
    }

    /**
     * Returns the minimum TTL for records in the zone
     */
    public long
    getMinimum() {
        return minimum;
    }

    @Override
    public void rrToWire(DNSOutput out, Compression c, boolean canonical) {
        host.toWire(out, c, canonical);
        admin.toWire(out, c, canonical);
        out.writeU32(serial);
        out.writeU32(refresh);
        out.writeU32(retry);
        out.writeU32(expire);
        out.writeU32(minimum);
    }

}
