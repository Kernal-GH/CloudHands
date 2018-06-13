package com.antell.cloudhands.api.packet.udp.dns;

/**
 * NSAP Pointer Record  - maps a domain name representing an NSAP Address to
 * a hostname.
 */

public class NSAP_PTRRecord extends SingleNameBase {


    public NSAP_PTRRecord() {
    }

    @Override
    public Record getObject() {
        return new NSAP_PTRRecord();
    }

    /**
     * Gets the target of the NSAP_PTR Record
     */
    public Name getTarget() {
        return getSingleName();
    }

}
