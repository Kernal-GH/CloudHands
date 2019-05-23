package com.antell.cloudhands.api.packet.udp.dns;

/**
 * Mailbox Record  - specifies a host containing a mailbox.
 */

public class MBRecord extends SingleNameBase {


    public MBRecord() {
    }

    @Override
    public Record getObject() {
        return new MBRecord();
    }

    /**
     * Gets the mailbox for the domain
     */
    public Name getMailbox() {
        return getSingleName();
    }

    @Override
    public Name getAdditionalName() {
        return getSingleName();
    }

}
