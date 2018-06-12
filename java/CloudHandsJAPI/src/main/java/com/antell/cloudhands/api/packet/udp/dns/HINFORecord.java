package com.antell.cloudhands.api.packet.udp.dns;

import org.elasticsearch.common.xcontent.XContentBuilder;

import java.io.IOException;

/**
 * Host Information - describes the CPU and OS of a host
 */

public class HINFORecord extends Record {

    private static final long serialVersionUID = -4732870630947452112L;

    private byte[] cpu, os;

    public HINFORecord() {
    }

    @Override
    public Record getObject() {
        return new HINFORecord();
    }

    /**
     * Creates an HINFO Record from the given data
     *
     * @param cpu A string describing the host's CPU
     * @param os  A string describing the host's OS
     * @throws IllegalArgumentException One of the strings has invalid escapes
     */
    public HINFORecord(Name name, int dclass, long ttl, String cpu, String os) {
        super(name, Type.HINFO, dclass, ttl);
        try {
            this.cpu = byteArrayFromString(cpu);
            this.os = byteArrayFromString(os);
        } catch (TextParseException e) {
            throw new IllegalArgumentException(e.getMessage());
        }
    }

    @Override
    public void rrFromWire(DNSInput in) throws IOException {
        cpu = in.readCountedString();
        os = in.readCountedString();
    }

    @Override
    public void rdataFromString(Tokenizer st, Name origin) throws IOException {
        try {
            cpu = byteArrayFromString(st.getString());
            os = byteArrayFromString(st.getString());
        } catch (TextParseException e) {
            throw st.exception(e.getMessage());
        }
    }

    /**
     * Returns the host's CPU
     */
    public String getCPU() {
        return byteArrayToString(cpu, false);
    }

    /**
     * Returns the host's OS
     */
    public String getOS() {
        return byteArrayToString(os, false);
    }

    @Override
    public void rrToWire(DNSOutput out, Compression c, boolean canonical) {
        out.writeCountedString(cpu);
        out.writeCountedString(os);
    }

    /**
     * Converts to a string
     */
    @Override
    public String rrToString() {
        StringBuffer sb = new StringBuffer();
        sb.append(byteArrayToString(cpu, true));
        sb.append(" ");
        sb.append(byteArrayToString(os, true));
        return sb.toString();
    }

    @Override
    public XContentBuilder rdataToJson(XContentBuilder cb) throws IOException {

        cb.field("cpu",byteArrayToString(cpu,true));
        cb.field("os",byteArrayToString(os,true));

        return cb;
    }

}
