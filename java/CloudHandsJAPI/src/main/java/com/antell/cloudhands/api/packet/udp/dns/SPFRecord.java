package com.antell.cloudhands.api.packet.udp.dns;

import java.util.List;

/**
 * Sender Policy Framework (RFC 4408, experimental)
 */

public class SPFRecord extends TXTBase {

    private static final long serialVersionUID = -2100754352801658722L;

    public SPFRecord() {
    }

    @Override
    public Record getObject() {
        return new SPFRecord();
    }



    /**
     * Creates a SPF Record from the given data
     *
     * @param strings The text strings
     * @throws IllegalArgumentException One of the strings has invalid escapes
     */
    public SPFRecord(Name name, int dclass, long ttl, List strings) {
        super(name, Type.SPF, dclass, ttl, strings);
    }

    /**
     * Creates a SPF Record from the given data
     *
     * @param string One text string
     * @throws IllegalArgumentException The string has invalid escapes
     */
    public SPFRecord(Name name, int dclass, long ttl, String string) {
        super(name, Type.SPF, dclass, ttl, string);
    }

}
