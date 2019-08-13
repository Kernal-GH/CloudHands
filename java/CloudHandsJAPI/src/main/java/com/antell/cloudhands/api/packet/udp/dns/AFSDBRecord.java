package com.antell.cloudhands.api.packet.udp.dns;


/**
 * AFS Data Base Record - maps a domain name to the name of an AFS cell
 * database server.
 */

public class AFSDBRecord extends U16NameBase {

    public AFSDBRecord() {
    }

    @Override
    public Record getObject() {
        return new AFSDBRecord();
    }

    /**
     * Gets the subtype indicating the service provided by the host.
     */
    public int getSubtype() {
        return getU16Field();
    }

    /**
     * Gets the host providing service for the domain.
     */
    public Name getHost() {
        return getNameField();
    }

}
