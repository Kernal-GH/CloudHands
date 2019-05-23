package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Mail Destination Record  - specifies a mail agent which delivers mail
 * for a domain (obsolete)
 */

public class MDRecord extends SingleNameBase {


    public MDRecord() {
    }

    @Override
    public Record getObject() {
        return new MDRecord();
    }

    /**
     * Gets the mail agent for the domain
     */
    public Name getMailAgent() {
        return getSingleName();
    }

    @Override
    public Name getAdditionalName() {
        return getSingleName();
    }

}
