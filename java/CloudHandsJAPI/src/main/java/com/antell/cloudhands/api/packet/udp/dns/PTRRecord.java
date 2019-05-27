package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Pointer Record  - maps a domain name representing an Internet Address to
 * a hostname.
 *
 */

public class PTRRecord extends SingleCompressedNameBase {


    public PTRRecord() {
    }

    @Override
    public Record getObject() {
        return new PTRRecord();
    }

    /**
     * Gets the target of the PTR Record
     */
    public Name getTarget() {
        return getSingleName();
    }

}
